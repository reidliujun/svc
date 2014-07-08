/*
 * Copyright (c) 2004-2006 Milan Cutka
 * based on CoreVorbis Audio Decoder filter by Christophe Paris
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "tremor/ogg.h"
#include "TaudioCodecTremor.h"
#include "Tdll.h"
#include "IffdshowBase.h"
#include "IffdshowDec.h"
#include "IffdshowDecAudio.h"
#include "ffdshow_mediaguids.h"
#include "xiph/vorbis/vorbisformat.h"
#include "xiph/ogm/ogmstreams.h"
#include "ffdebug.h"
#include "dsutil.h"

const char_t* TaudioCodecTremor::dllname=_l("ff_tremor.dll");

const int TaudioCodecTremor::chmap[MAXCHANNELS][MAXCHANNELS]=
{
 { 0, },                 // mono
 { 0, 1, },              // l, r
 { 0, 2, 1, },           // l, c, r -> l, r, c
 { 0, 1, 2, 3, },        // l, r, bl, br
 { 0, 2, 1, 3, 4, },     // l, c, r, bl, br -> l, r, c, bl, br
 { 0, 2, 1, 5, 3, 4 }    // l, c, r, bl, br, lfe -> l, r, c, lfe, bl, br
};

TaudioCodecTremor::TaudioCodecTremor(IffdshowBase *deci,IdecAudioSink *Isink):
 Tcodec(deci),
 TaudioCodec(deci,Isink)
{
 dll=NULL;
 inited=false;
 pbVorbisInfo=NULL;cbVorbisInfo=0;
 pbVorbisComment=NULL;cbVorbisComment=0;
 pbVorbisCodebook=NULL;cbVorbisCodebook=0;
 memset(&vd,0,sizeof(vd));
 memset(&vb,0,sizeof(vb));
 memset(&vc,0,sizeof(vc));
 memset(&vi,0,sizeof(vi));
}

bool TaudioCodecTremor::initVorbis(void)
{
 ogg_reference orId,orCmt,orSetup;
 ogg_buffer obId,obCmt,obSetup;
 clearVorbis(false);

 vorbis_info_init(&vi);
 vorbis_comment_init(&vc);

 m_packetno = 0;
 ogg_packet opId;memset(&opId, 0, sizeof(ogg_packet));
 obId.data = pbVorbisInfo;
 obId.size = cbVorbisInfo;
 obId.refcount = 255; // large enough not to be freed by the Tremor.
 obId.ptr.next = NULL;
 orId.buffer = &obId;
 orId.begin = 0;
 orId.length = cbVorbisInfo;
 orId.next = NULL;
 opId.packet= &orId;
 opId.bytes=cbVorbisInfo;
 opId.b_o_s = 1;
 opId.packetno = m_packetno++;

 // Build the "Comment Header" packet
 ogg_packet opCmt;memset(&opCmt, 0, sizeof(ogg_packet));
 obCmt.data = pbVorbisComment;
 obCmt.size = cbVorbisComment;
 obCmt.refcount = 255;
 obCmt.ptr.next = NULL;
 orCmt.buffer = &obCmt;
 orCmt.begin = 0;
 orCmt.length = cbVorbisComment;
 orCmt.next = NULL;
 opCmt.packet = &orCmt;
 opCmt.bytes=cbVorbisComment;
 opCmt.b_o_s = 0;
 opCmt.packetno = m_packetno++;

 // Build the "Setup Header" packet
 ogg_packet opSetup;memset(&opSetup, 0, sizeof(ogg_packet));
 obSetup.data = pbVorbisCodebook;
 obSetup.size = cbVorbisCodebook;
 obSetup.refcount = 255;
 obSetup.ptr.next = NULL;
 orSetup.buffer = &obSetup;
 orSetup.begin = 0;
 orSetup.length = cbVorbisCodebook;
 orSetup.next = NULL;
 opSetup.packet= &orSetup;
 opSetup.bytes=cbVorbisCodebook;
 opSetup.b_o_s = 0;
 opSetup.packetno = m_packetno++;

 if ((    vorbis_synthesis_headerin(&vi,&vc,&opId) >= 0)
      && (vorbis_synthesis_headerin(&vi,&vc,&opCmt) >= 0)
      && (vorbis_synthesis_headerin(&vi,&vc,&opSetup) >= 0))
  {
   postgain=1.0f;
   if (vorbis_comment_query_count(&vc,"LWING_GAIN"))
    sscanf(vorbis_comment_query(&vc,"LWING_GAIN",0),"%f",&postgain);
   if (vorbis_comment_query_count(&vc,"POSTGAIN"))
    sscanf(vorbis_comment_query(&vc,"POSTGAIN",0),"%f",&postgain);
   if(vorbis_comment_query_count(&vc,"REPLAYGAIN_TRACK_GAIN"))
    {
     if (sscanf(vorbis_comment_query(&vc,"REPLAYGAIN_TRACK_GAIN",0),"%f dB",&postgain)==1)
      postgain=(float)pow(10.0,postgain/20.0);
    }
   vorbis_synthesis_init(&vd,&vi);
   vorbis_block_init(&vd,&vb);
   return true;
  }
 else
  return false;
}

void TaudioCodecTremor::clearVorbis(bool all)
{
 if (dll)
  {
   vorbis_block_clear(&vb);
   vorbis_dsp_clear(&vd);
   vorbis_comment_clear(&vc);
   vorbis_info_clear(&vi);
  }
 if (all)
  {
   if (pbVorbisInfo) free(pbVorbisInfo);pbVorbisInfo=NULL;
   if (pbVorbisComment) free(pbVorbisComment);pbVorbisComment=NULL;
   if (pbVorbisCodebook) free(pbVorbisCodebook);pbVorbisCodebook=NULL;
  }
}

bool TaudioCodecTremor::init(const CMediaType &mt)
{
 dll=new Tdll(dllname,config);
 dll->loadFunction(vorbis_info_init,"vorbis_info_init");
 dll->loadFunction(vorbis_comment_init,"vorbis_comment_init");
 dll->loadFunction(vorbis_synthesis_headerin,"vorbis_synthesis_headerin");
 dll->loadFunction(vorbis_synthesis_init,"vorbis_synthesis_init");
 dll->loadFunction(vorbis_block_init,"vorbis_block_init");
 dll->loadFunction(vorbis_synthesis,"vorbis_synthesis");
 dll->loadFunction(vorbis_synthesis_blockin,"vorbis_synthesis_blockin");
 dll->loadFunction(vorbis_synthesis_pcmout,"vorbis_synthesis_pcmout");
 dll->loadFunction(vorbis_synthesis_read,"vorbis_synthesis_read");
 dll->loadFunction(vorbis_block_clear,"vorbis_block_clear");
 dll->loadFunction(vorbis_dsp_clear,"vorbis_dsp_clear");
 dll->loadFunction(vorbis_comment_clear,"vorbis_comment_clear");
 dll->loadFunction(vorbis_info_clear,"vorbis_info_clear");
 dll->loadFunction(vorbis_comment_query,"vorbis_comment_query");
 dll->loadFunction(vorbis_comment_query_count,"vorbis_comment_query_count");
 if (dll->ok)
  {
   fmt.sf=TsampleFormat::SF_PCM32;
   postgain=1;isGain=deci->getParam2(IDFF_vorbisgain);
   if (mt.formattype==FORMAT_VorbisFormat2 || mt.subtype==MEDIASUBTYPE_FFVORBIS)
    {
     Textradata extradata(mt);
     if (mt.formattype==FORMAT_VorbisFormat2)
      {
       const VORBISFORMAT2 *vf2=(const VORBISFORMAT2*)mt.pbFormat;
       // Save the 3 vorbis header packets
       cbVorbisInfo=vf2->HeaderSize[0];
       pbVorbisInfo=(unsigned char*)vf2+sizeof(VORBISFORMAT2);
       cbVorbisComment=vf2->HeaderSize[1];
       pbVorbisComment=(unsigned char*)vf2+sizeof(VORBISFORMAT2)+vf2->HeaderSize[0];
       cbVorbisCodebook=vf2->HeaderSize[2];
       pbVorbisCodebook=(unsigned char*)vf2+sizeof(VORBISFORMAT2)+vf2->HeaderSize[0]+vf2->HeaderSize[1];
      }
     else
      {
       if (!extradata.size) return false;
       uint8_t *headers=(uint8_t*)extradata.data;;
       size_t headers_len=extradata.size;
       uint8_t *header_start[3];
       int header_len[3];
       if (headers[0]==0 && headers[1]==30)
        for (int i=0;i<3;i++)
         {
          header_len[i]=*headers++<<8;
          header_len[i]+=*headers++;
          header_start[i]=headers;
          headers+=header_len[i];
         }
       else if (headers[0]==2)
        {
         size_t i,j;
         for (j=1,i=0;i<2;++i, ++j)
          {
           header_len[i]=0;
           while (j<headers_len && headers[j]==0xff)
            {
             header_len[i]+=0xff;
             ++j;
            }
           if (j>=headers_len)
            return false;
           header_len[i]+=headers[j];
          }
         header_len[2]=int(headers_len-header_len[0]-header_len[1]-j);
         headers+=j;
         header_start[0]=headers;
         header_start[1]=header_start[0]+header_len[0];
         header_start[2]=header_start[1]+header_len[1];
        }
       cbVorbisInfo=header_len[0];
       pbVorbisInfo=header_start[0];
       cbVorbisComment=header_len[1];
       pbVorbisComment=header_start[1];
       cbVorbisCodebook=header_len[2];
       pbVorbisCodebook=header_start[2];
      }

     bool initok=initVorbis();
     pbVorbisInfo=pbVorbisComment=pbVorbisCodebook=NULL;
     if (initok)
      {
       inited=true;
       oggds=false;
       return true;
      }
    }
   else
    {
     inited=false;
     m_packetno=0;
     oggds=true;
     return true;
    }
  }
 return false;
}
TaudioCodecTremor::~TaudioCodecTremor()
{
 clearVorbis(true);
 if (dll)
  delete dll;
}

void TaudioCodecTremor::getInputDescr1(char_t *buf,size_t buflen) const
{
 ff_strncpy(buf,_l("vorbis"),buflen);
 buf[buflen-1]='\0';
}

HRESULT TaudioCodecTremor::decode(TbyteBuffer &src)
{
 ogg_reference ogr;
 ogg_buffer ob;
 ogg_packet op;
 memset(&op,0,sizeof(op));
 ob.data = &*src.begin();
 ob.size = src.size();
 ob.refcount = 255;
 ob.ptr.next = NULL;
 ogr.buffer = &ob;
 ogr.begin = 0;
 ogr.length = ob.size;
 ogr.next = NULL;
 op.packet = &ogr;
 op.bytes = (long)src.size();
 op.b_o_s = 0;
 op.packetno = m_packetno++;
 if (oggds)
  if (*(unsigned char*)op.packet&PACKET_TYPE_HEADER)
   {
    inited=false;
    switch (*(unsigned char*)op.packet&PACKET_TYPE_BITS)
     {
      case PACKET_TYPE_HEADER:
       DPRINTF(_l("header:%i"),op.bytes);
       if (pbVorbisInfo) free(pbVorbisInfo);
       memcpy(pbVorbisInfo=(unsigned char*)malloc(cbVorbisInfo=op.bytes),op.packet,op.bytes);
       break;
      case PACKET_TYPE_COMMENT:
       DPRINTF(_l("comment:%i"),op.bytes);
       if (pbVorbisComment) free(pbVorbisComment);
       memcpy(pbVorbisComment=(unsigned char*)malloc(cbVorbisComment=op.bytes),op.packet,op.bytes);
       break;
      case PACKET_TYPE_CODEBOOK:
       DPRINTF(_l("codebook:%i"),op.bytes);
       if (pbVorbisCodebook) free(pbVorbisCodebook);
       memcpy(pbVorbisCodebook=(unsigned char*)malloc(cbVorbisCodebook=op.bytes),op.packet,op.bytes);
       break;
     }
    src.clear();
    return S_OK;
   }
  else
   if (!inited)
    if (!initVorbis())
     {
      DPRINTF(_l("vorbisInit:bad"));
      src.clear();
      return S_FALSE;
     }
    else
     {
      DPRINTF(_l("vorbisInit:ok"));
      inited=true;
     }

 if (vorbis_synthesis(&vb,&op,1)==0)
  {
   vorbis_synthesis_blockin(&vd,&vb);
   int samples;
   ogg_int32_t **pcm;
   unsigned int samplessum=0;
   while ((samples=vorbis_synthesis_pcmout(&vd,&pcm))>0)
    {
     samplessum+=samples;
     fmt.sf=TsampleFormat::SF_PCM32;
     int32_t *dst=(int32_t*)getDst(samples*fmt.blockAlign());
     for (unsigned int ch=0;ch<fmt.nchannels;ch++)
      {
       ogg_int32_t *pcmch=pcm[chmap[fmt.nchannels-1][ch]];
       int32_t *dstch=dst+ch;
       for (int i=0;i<samples;i++,dstch+=fmt.nchannels)
        *dstch=limit(int32_t(pcmch[i]),TsampleFormatInfo<int32_t>::min()>>7,TsampleFormatInfo<int32_t>::max()>>7)<<7;
      }
     HRESULT hr=sinkA->deliverDecodedSample(dst,samples,fmt,isGain?postgain:1);
     if (hr!=S_OK) return hr;
     vorbis_synthesis_read(&vd,samples);
    }
   if (samplessum)
    {
     numframes++;
     bpssum+=(lastbps=roundDiv(8*fmt.freq*(unsigned int)op.bytes,samplessum)/1000);
    }
  }
 src.clear();
 return S_OK;
}
bool TaudioCodecTremor::onSeek(REFERENCE_TIME segmentStart)
{
 return false;
}
