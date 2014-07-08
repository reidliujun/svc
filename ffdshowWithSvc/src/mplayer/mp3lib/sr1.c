// #define NEWBUFFERING
//#define DEBUG_RESYNC

/* 1 frame = 4608 byte PCM */

#ifdef __GNUC__
#define LOCAL static inline
#else
#define LOCAL static __inline
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <math.h>

#include "mpg123.h"
#include "huffman.h"
#include "inttypes.h"
#include "../bswap.h"
#include "../cpudetect.h"
#include "../mp_msg.h"

#include "../libvo/fastmemcpy.h"

static const long outscale = 32768;
#include "tabinit.c"

static int mp3_read(mp3lib_ctx *ctx,unsigned char *buf,int size);

//void mp3_seek(int pos){
//  fseek(mp3_file,pos,SEEK_SET);
//  return (MP3_fpos=ftell(mp3_file));
//}

static const int tabsel_123[2][3][16] = {
   { {0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,},
     {0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,},
     {0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,} },

   { {0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,},
     {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,},
     {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,} }
};

static const long freqs[9] = { 44100, 48000, 32000, 22050, 24000, 16000 , 11025 , 12000 , 8000 };

LOCAL unsigned int getbits(mp3lib_ctx *ctx,int number_of_bits)
{
  unsigned rval;
//  if(MP3_frames>=7741) printf("getbits: bits=%d  bitsleft=%d  wordptr=%x\n",number_of_bits,bitsleft,wordpointer);
  if((ctx->bitsleft-=number_of_bits)<0) return 0;
  if(!number_of_bits) return 0;
  rval = ctx->wordpointer[0];
         rval <<= 8;
  rval |= ctx->wordpointer[1];
         rval <<= 8;
  rval |= ctx->wordpointer[2];
  rval <<= ctx->bitindex;
         rval &= 0xffffff;
  ctx->bitindex += number_of_bits;
         rval >>= (24-number_of_bits);
  ctx->wordpointer += (ctx->bitindex>>3);
  ctx->bitindex &= 7;
  return rval;
}


LOCAL unsigned int getbits_fast(mp3lib_ctx *ctx,int number_of_bits)
{
  unsigned rval;
//  if(MP3_frames>=7741) printf("getbits_fast: bits=%d  bitsleft=%d  wordptr=%x\n",number_of_bits,bitsleft,wordpointer);
  if((ctx->bitsleft-=number_of_bits)<0) return 0;
  if(!number_of_bits) return 0;
#ifdef ARCH_X86
  rval = bswap_16(*((uint16_t *)ctx->wordpointer));
#else
  /*
   * we may not be able to address unaligned 16-bit data on non-x86 cpus.
   * Fall back to some portable code.
   */
  rval = ctx->wordpointer[0] << 8 | ctx->wordpointer[1];
#endif
  rval <<= ctx->bitindex;
         rval &= 0xffff;
  ctx->bitindex += number_of_bits;
         rval >>= (16-number_of_bits);
  ctx->wordpointer += (ctx->bitindex>>3);
  ctx->bitindex &= 7;
  return rval;
}

LOCAL unsigned int get1bit(mp3lib_ctx *ctx)
{
  unsigned char rval;
//  if(MP3_frames>=7741) printf("get1bit: bitsleft=%d  wordptr=%x\n",bitsleft,wordpointer);
  if((--ctx->bitsleft)<0) return 0;
  rval = (unsigned char)(*ctx->wordpointer << ctx->bitindex);
  ctx->bitindex++;
  ctx->wordpointer += (ctx->bitindex>>3);
  ctx->bitindex &= 7;
  return ((rval>>7)&1);
}

LOCAL void set_pointer(mp3lib_ctx *ctx,int backstep)
{
//  if(backstep!=512 && backstep>fsizeold)
//    printf("\rWarning! backstep (%d>%d)                                         \n",backstep,fsizeold);
  ctx->wordpointer = ctx->bsbuf + ctx->ssize - backstep;
  if (backstep) memcpy(ctx->wordpointer,ctx->bsbufold+ctx->fsizeold-backstep,backstep);
  ctx->bitindex = 0;
  ctx->bitsleft+=8*backstep;
//  printf("Backstep %d  (bitsleft=%d)\n",backstep,bitsleft);
}

LOCAL int stream_head_read(mp3lib_ctx *ctx,unsigned char *hbuf,uint32_t *newhead){
  if(mp3_read(ctx,hbuf,4) != 4) return FALSE;
#ifdef ARCH_X86
  *newhead = bswap_32(*((uint32_t*)hbuf));
#else
  /*
   * we may not be able to address unaligned 32-bit data on non-x86 cpus.
   * Fall back to some portable code.
   */
  *newhead = 
      hbuf[0] << 24 |
      hbuf[1] << 16 |
      hbuf[2] <<  8 |
      hbuf[3];
#endif
  return TRUE;
}

LOCAL int stream_head_shift(mp3lib_ctx *ctx,unsigned char *hbuf,uint32_t *head){
  *((uint32_t*)hbuf) >>= 8;
  if(mp3_read(ctx,hbuf+3,1) != 1) return 0;
  *head <<= 8;
  *head |= hbuf[3];
  return 1;
}

/*
 * decode a header and write the information
 * into the frame structure
 */
LOCAL int decode_header(mp3lib_ctx *ctx,struct frame *fr,uint32_t newhead){

    // head_check:
    if( (newhead & 0xffe00000) != 0xffe00000 ||  
        (newhead & 0x0000fc00) == 0x0000fc00) return FALSE;

    fr->lay = 4-((newhead>>17)&3);
//    if(fr->lay!=3) return FALSE;

    if( newhead & (1<<20) ) {
      fr->lsf = (newhead & (1<<19)) ? 0x0 : 0x1;
      fr->mpeg25 = 0;
    } else {
      fr->lsf = 1;
      fr->mpeg25 = 1;
    }

    if(fr->mpeg25)
      fr->sampling_frequency = 6 + ((newhead>>10)&0x3);
    else
      fr->sampling_frequency = ((newhead>>10)&0x3) + (fr->lsf*3);

    if(fr->sampling_frequency>8) return FALSE;  // valid: 0..8

    fr->error_protection = ((newhead>>16)&0x1)^0x1;
    fr->bitrate_index = ((newhead>>12)&0xf);
    fr->padding   = ((newhead>>9)&0x1);
    fr->extension = ((newhead>>8)&0x1);
    fr->mode      = ((newhead>>6)&0x3);
    fr->mode_ext  = ((newhead>>4)&0x3);
    fr->copyright = ((newhead>>3)&0x1);
    fr->original  = ((newhead>>2)&0x1);
    fr->emphasis  = newhead & 0x3;

    ctx->MP3_channels = fr->stereo    = (fr->mode == MPG_MD_MONO) ? 1 : 2;

    if(!fr->bitrate_index){
//      fprintf(stderr,"Free format not supported.\n");
      return FALSE;
    }

switch(fr->lay){
  case 2:
    ctx->MP3_bitrate=tabsel_123[fr->lsf][1][fr->bitrate_index];
    ctx->MP3_samplerate=freqs[fr->sampling_frequency];
    fr->framesize = ctx->MP3_bitrate * 144000;
    fr->framesize /= ctx->MP3_samplerate;
    ctx->MP3_framesize=fr->framesize;
    fr->framesize += fr->padding - 4;
    break;
  case 3:
    if(fr->lsf)
      ctx->ssize = (fr->stereo == 1) ? 9 : 17;
    else
      ctx->ssize = (fr->stereo == 1) ? 17 : 32;
    if(fr->error_protection) ctx->ssize += 2;

    ctx->MP3_bitrate=tabsel_123[fr->lsf][2][fr->bitrate_index];
    ctx->MP3_samplerate=freqs[fr->sampling_frequency];
    fr->framesize  = ctx->MP3_bitrate * 144000;
    fr->framesize /= ctx->MP3_samplerate<<(fr->lsf);
    ctx->MP3_framesize=fr->framesize;
    fr->framesize += fr->padding - 4;
    break;
  case 1:
//    fr->jsbound = (fr->mode == MPG_MD_JOINT_STEREO) ? (fr->mode_ext<<2)+4 : 32;
    ctx->MP3_bitrate=tabsel_123[fr->lsf][0][fr->bitrate_index];
    ctx->MP3_samplerate=freqs[fr->sampling_frequency];
    fr->framesize  = ctx->MP3_bitrate * 12000;
    fr->framesize /= ctx->MP3_samplerate;
    ctx->MP3_framesize  = ((fr->framesize+fr->padding)<<2);
    fr->framesize  = ctx->MP3_framesize-4;
//    printf("framesize=%d\n",fr->framesize);
    break;
  default:
    ctx->MP3_framesize=fr->framesize=0;
//    fprintf(stderr,"Sorry, unsupported layer type.\n");
    return 0;
}
    if(fr->framesize<=0 || fr->framesize>MAXFRAMESIZE) return FALSE;

    return 1;
}

LOCAL int stream_read_frame_body(mp3lib_ctx *ctx,int size){

  /* flip/init buffer for Layer 3 */
  ctx->bsbufold = ctx->bsbuf;
  ctx->bsbuf = ctx->bsspace[ctx->bsnum]+512;
  ctx->bsnum = (ctx->bsnum + 1) & 1;

  if( mp3_read(ctx,ctx->bsbuf,size) != size) return 0; // broken frame

  ctx->bitindex = 0;
  ctx->wordpointer = (unsigned char *) ctx->bsbuf;
  ctx->bitsleft=8*size;

  return 1;
}

/*****************************************************************
 * read next frame     return number of frames read.
 */
LOCAL int read_frame(mp3lib_ctx *ctx){
  uint32_t newhead;
  unsigned char hbuf[8];
  int skipped,resyncpos;
  int frames=0;
  struct frame *fr=&ctx->fr;

resync:
  skipped=ctx->MP3_fpos;
  resyncpos=ctx->MP3_fpos;

  set_pointer(ctx,512);
  ctx->fsizeold=fr->framesize;       /* for Layer3 */
  if(!stream_head_read(ctx,hbuf,&newhead)) return 0;
  if(!decode_header(ctx,fr,newhead)){
    // invalid header! try to resync stream!
#ifdef DEBUG_RESYNC
    printf("ReSync: searching for a valid header...  (pos=%X)\n",MP3_fpos);
#endif
retry1:
    while(!decode_header(ctx,fr,newhead)){
      if(!stream_head_shift(ctx,hbuf,&newhead)) return 0;
    }
    resyncpos=ctx->MP3_fpos-4;
    // found valid header
#ifdef DEBUG_RESYNC
    printf("ReSync: found valid hdr at %X  fsize=%ld  ",resyncpos,fr->framesize);
#endif
    if(!stream_read_frame_body(ctx,fr->framesize)) return 0; // read body
    set_pointer(ctx,512);
    ctx->fsizeold=fr->framesize;       /* for Layer3 */
    if(!stream_head_read(ctx,hbuf,&newhead)) return 0;
    if(!decode_header(ctx,fr,newhead)){
      // invalid hdr! go back...
#ifdef DEBUG_RESYNC
      printf("INVALID\n");
#endif
//      mp3_seek(resyncpos+1);
      if(!stream_head_read(ctx,hbuf,&newhead)) return 0;
      goto retry1;
    }
#ifdef DEBUG_RESYNC
    printf("OK!\n");
    ++frames;
#endif
  }

  skipped=resyncpos-skipped;
//  if(skipped && !MP3_resync) printf("\r%d bad bytes skipped  (resync at 0x%X)                          \n",skipped,resyncpos);

//  printf("%8X [%08X] %d %d (%d)%s%s\n",MP3_fpos-4,newhead,fr->framesize,fr->mode,fr->mode_ext,fr->error_protection?" CRC":"",fr->padding?" PAD":"");

  /* read main data into memory */
  if(!stream_read_frame_body(ctx,fr->framesize)){
    //printf("\nBroken frame at 0x%X                                                  \n",resyncpos);
    return 0;
  }
  ++frames;

  if(ctx->MP3_resync){
    ctx->MP3_resync=0;
    if(frames==1) goto resync;
  }

  return frames;
}

static int _has_mmx = 0;  // used by layer2.c, layer3.c to pre-scale coeffs

#include "layer2.c"
#include "layer3.c"
#include "layer1.c"

/******************************************************************************/
/*           PUBLIC FUNCTIONS                  */
/******************************************************************************/

/* It's hidden from gcc in assembler */
#ifdef ARCH_X86
extern void dct64_MMX(short *, short *, real *);
extern void dct64_MMX_3dnow(short *, short *, real *);
extern void dct64_MMX_3dnowex(short *, short *, real *);
extern void dct64_sse(short *, short *, real *);
void (*dct64_MMX_func)(short *, short *, real *);
#endif

#include "../cpudetect.h"

// Init decoder tables.  Call first, once!
mp3lib_ctx* MP3_Init(int mono){
    mp3lib_ctx *ctx=(mp3lib_ctx*)malloc(sizeof(*ctx));
    memset(ctx,0,sizeof(*ctx));
    ctx->MP3_bps=2;
    ctx->fsizeold=0;ctx->ssize=0;
    ctx->bsbufold=ctx->bsspace[0]+512;
    ctx->bsbuf=ctx->bsspace[1]+512;
    ctx->bsnum=0;
    ctx->pcm_point=0;
    ctx->bo=1;

    _has_mmx = 0;
    dct36_func = dct36;

    make_decode_tables(outscale);

#ifdef ARCH_X86

    if (gCpuCaps.hasMMX)
    {
	_has_mmx = 1;
	make_decode_tables_MMX(outscale);
	mp_msg(MSGT_DECAUDIO,MSGL_V,"mp3lib: made decode tables with MMX optimization\n");
	ctx->synth_func = synth_1to1_MMX;
    }

    if (gCpuCaps.has3DNowExt)
    {
	dct36_func=dct36_3dnowex;
	dct64_MMX_func= dct64_MMX_3dnowex;
	mp_msg(MSGT_DECAUDIO,MSGL_V,"mp3lib: using 3DNow!Ex optimized decore!\n");
    }
    else
    if (gCpuCaps.has3DNow)
    {
	dct36_func = dct36_3dnow;
	dct64_MMX_func = dct64_MMX_3dnow;
	mp_msg(MSGT_DECAUDIO,MSGL_V,"mp3lib: using 3DNow! optimized decore!\n");
    }
    else
    if (gCpuCaps.hasSSE)
    {
	dct64_MMX_func = dct64_sse;
	mp_msg(MSGT_DECAUDIO,MSGL_V,"mp3lib: using SSE optimized decore!\n");
    }
    else
    if (gCpuCaps.hasMMX)
    {
	dct64_MMX_func = dct64_MMX;
	mp_msg(MSGT_DECAUDIO,MSGL_V,"mp3lib: using MMX optimized decore!\n");
    }
    else
#endif
    {
	ctx->synth_func = NULL; /* use default c version */
#ifdef DEBUG
	mp_msg(MSGT_DECAUDIO,MSGL_V,"mp3lib: using generic C decore!\n");
#endif
    }

    ctx->fr.synth=synth_1to1;
    ctx->fr.synth_mono=mono?synth_1to1_mono:synth_1to1_mono2stereo;
    ctx->fr.down_sample=0;
    ctx->fr.down_sample_sblimit = SBLIMIT>>(ctx->fr.down_sample);

    init_layer2();
    init_layer3(ctx->fr.down_sample_sblimit);
#ifdef DEBUG
    mp_msg(MSGT_DECAUDIO,MSGL_INFO,"MP3lib: init layer2&3 finished, tables done\n");
#endif
    return ctx;
}

static int mp3_read(mp3lib_ctx *ctx,unsigned char *buf,int size)
{
 int len=size<=ctx->srclen?size:ctx->srclen;
 memcpy(buf,ctx->src,len);ctx->src+=len;ctx->srclen-=len;
 if(len>0) ctx->MP3_fpos+=len;
 return len;
}

// Read & decode a single frame. Called by sound driver.
int MP3_DecodeFrame(mp3lib_ctx *ctx,const unsigned char *Isrc,unsigned int Isrclen,unsigned char *hova,short single,unsigned int *srcUsed){
   ctx->src=Isrc;ctx->srclen=Isrclen;
   ctx->pcm_sample = hova;
   ctx->pcm_point = 0;
   if(!read_frame(ctx))return(0);
   if(single==-2){ set_pointer(ctx,512); return(1); }
   if(ctx->fr.error_protection)
    {
     ctx->fr.crc_target=getbits(ctx,16); /* skip crc */
    }
   ctx->fr.single=single;
   switch(ctx->fr.lay){
     case 2: do_layer2(ctx,single);break;
     case 3: do_layer3(ctx,single);break;
     case 1: do_layer1(ctx,single);break;
     default:
         return 0;	// unsupported
   }
//   ++MP3_frames;
   *srcUsed=ctx->src-Isrc;
   return(ctx->pcm_point?ctx->pcm_point:2);
}

void MP3_Done(mp3lib_ctx *ctx)
{
 if (ctx) free(ctx);
}
