/*
 * mpg123 defines
 * used source: musicout.h from mpegaudio package
 */

#include "../config.h"

#ifndef M_PI
#define M_PI		3.141592653589793238462
#endif
#ifndef M_SQRT2
#define M_SQRT2		1.414213562373095048802
#endif
#define REAL_IS_FLOAT
typedef float real;
#define NEW_DCT9

#define         FALSE                   0
#define         TRUE                    1

#define         SBLIMIT                 32
#define         SCALE_BLOCK             12
#define         SSLIMIT                 18

#define         MPG_MD_STEREO           0
#define         MPG_MD_JOINT_STEREO     1
#define         MPG_MD_DUAL_CHANNEL     2
#define         MPG_MD_MONO             3

struct al_table
{
  short bits;
  short d;
};

struct mp3lib_ctx;
struct frame {
    const struct al_table *alloc;
    int (*synth)(struct mp3lib_ctx *ctx,real *,int,unsigned char *,int *);
    int (*synth_mono)(struct mp3lib_ctx *ctx,real *,unsigned char *,int *);
    int stereo;
    int jsbound;
    int single;
    int II_sblimit;
    int down_sample_sblimit;
    int lsf;
    int mpeg25;
    int down_sample;
    int header_change;
    int lay;
    int error_protection,crc_target;
    int bitrate_index;
    int sampling_frequency;
    int padding;
    int extension;
    int mode;
    int mode_ext;
    int copyright;
    int original;
    int emphasis;
    int framesize; /* computed framesize */
};


struct gr_info_s {
      int scfsi;
      unsigned part2_3_length;
      unsigned big_values;
      unsigned scalefac_compress;
      unsigned block_type;
      unsigned mixed_block_flag;
      unsigned table_select[3];
      unsigned subblock_gain[3];
      unsigned maxband[3];
      unsigned maxbandl;
      unsigned maxb;
      unsigned region1start;
      unsigned region2start;
      unsigned preflag;
      unsigned scalefac_scale;
      unsigned count1table_select;
      real *full_gain[3];
      real *pow2gain;
};

struct III_sideinfo
{
  unsigned main_data_begin;
  unsigned private_bits;
  struct {
         struct gr_info_s gr[2];
  } ch[2];
};

extern int synth_1to1_pent( struct mp3lib_ctx *,real *,int,short * );

#ifdef ARCH_X86
//extern int synth_1to1_MMX( struct mp3lib_ctx*,real *,int,short * );
extern int synth_1to1_MMX_s(real *, int, short *, short *, int *);

extern void dct36_3dnow(real *,real *,real *,real *,real *);
extern void dct36_3dnowex(real *,real *,real *,real *,real *);
extern void dct36_sse(real *,real *,real *,real *,real *);
#endif

typedef int (*synth_func_t)( struct mp3lib_ctx *,real *,int,short * );
typedef void (*dct36_func_t)(real *,real *,real *,real *,real *);

typedef struct mp3lib_ctx
{
 synth_func_t synth_func;
 struct frame fr;
 int MP3_frames;
 int MP3_filesize;
 int MP3_fpos;      // current file position
 int MP3_framesize; // current framesize
 int MP3_bitrate;   // current bitrate
 int MP3_samplerate;  // current samplerate
 int MP3_resync;
 int MP3_channels;
 int MP3_bps;

 int fsizeold,ssize;
#define MAXFRAMESIZE 1280
#define MAXFRAMESIZE2 (512+MAXFRAMESIZE)
 unsigned char bsspace[2][MAXFRAMESIZE2]; /* !!!!! */
 unsigned char *bsbufold;
 unsigned char *bsbuf;
 int bsnum;

 // Frame reader
 int bitindex;
 unsigned char *wordpointer;
 int bitsleft;

 unsigned char *pcm_sample;   // outbuffer address
 int pcm_point;           // outbuffer offset

 const unsigned char *src;int srclen;

 real buffs[2][2][0x110];
 short __attribute__((aligned(16))) buffsMMX[2][2][0x110];
 int bo;

 unsigned int scfsi_buf[64];

 real block[2][2][SBLIMIT*SSLIMIT];
 int blc[2];
 real hybridIn[2][SBLIMIT][SSLIMIT];
 real hybridOut[2][SSLIMIT][SBLIMIT];
} mp3lib_ctx;
