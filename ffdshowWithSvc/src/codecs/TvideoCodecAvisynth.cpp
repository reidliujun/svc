/*
 * Copyright (c) 2003-2006 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "TvideoCodecAvisynth.h"
#include "avisynth/Tavisynth.h"
#include "IffdshowBase.h"
#include "dsutil.h"

TvideoCodecAvisynth::Tavisynth::Tavisynth(const Textradata &extradata,TffPictBase &pict,Trect &r,int &csp,IffdshowBase *deci):
 script(NULL),
 env(NULL),clip(NULL)
{
 if (ok)
  {
   env=CreateScriptEnvironment(AVISYNTH_INTERFACE_VERSION);
   if (env)
    {
     script=::Tavisynth::getScriptVideo(extradata.size+sizeof(VIDEOINFOHEADER)+2+4+1,(const char*)extradata.data,extradata.size);
     if (!script) {ok=false;return;}
     AVSValue eval_args[]={script,"ffdshow_dec_avisynth_script"};
     try
      {
       AVSValue val=env->Invoke("Eval",AVSValue(eval_args,2));
       if (val.IsClip())
        {
         clip=new PClip(val,env);
         //*clip=val.AsClip();
         const VideoInfo &vi=(*clip)->GetVideoInfo();
         if      (vi.IsRGB24()) pict.csp=csp=FF_CSP_RGB24|FF_CSP_FLAGS_VFLIP;
         else if (vi.IsRGB32()) pict.csp=csp=FF_CSP_RGB32|FF_CSP_FLAGS_VFLIP;
         else if (vi.IsYUY2())  pict.csp=csp=FF_CSP_YUY2;
         else if (vi.IsYV12())  pict.csp=csp=FF_CSP_420P;
         r=pict.rectFull;
         ok=true;
        }
      }
     catch (AvisynthError &err)
      {
       deci->dbgError(text<char_t>(err.msg));
       ok=false;
      }
    }
  }
}

TvideoCodecAvisynth::Tavisynth::~Tavisynth()
{
 if (clip) delete clip;clip=NULL;
 if (env) delete env;env=NULL;
 if (script) free((void*)script);script=NULL;
}

TvideoCodecAvisynth::TvideoCodecAvisynth(IffdshowBase *Ideci,IdecVideoSink *Isink):
 Tcodec(Ideci),TcodecDec(Ideci,Isink),
 TvideoCodec(Ideci),
 TvideoCodecDec(Ideci,Isink),
 avisynth(NULL)
{
 ok=true;
}
TvideoCodecAvisynth::~TvideoCodecAvisynth()
{
 end();
}

void TvideoCodecAvisynth::forceOutputColorspace(const BITMAPINFOHEADER *hdr,int *ilace,TcspInfos &forcedCsps)
{
 forcedCsps.clear();
 if (hdr->biSize==sizeof(BITMAPINFOHEADER)) return;
 const char *extradata=(const char*)hdr+sizeof(BITMAPINFOHEADER);
 *ilace=extradata[5];
 FOURCC fcc=*(FOURCC*)(extradata+1);
 if (fcc) forcedCsps.push_back(csp_getInfoFcc(fcc));
}
bool TvideoCodecAvisynth::beginDecompress(TffPictBase &pict,FOURCC infcc,const CMediaType &mt,int sourceFlags)
{
 Textradata extradata(mt);
 if (!extradata.data || extradata.size==0) return false;
 if (!avisynth)
  {
   avisynth=new Tavisynth(extradata,pict,r,csp,deci);
   return avisynth->ok;
  }
 return false;
}
HRESULT TvideoCodecAvisynth::decompress(const unsigned char *src,size_t srcLen,IMediaSample *pIn)
{
 if (!avisynth->clip || !src || srcLen!=4) return S_FALSE;
 Tavisynth::PVideoFrame frame=(*avisynth->clip)->GetFrame(*(int*)src);
 if (pIn->IsPreroll()==S_OK)
  return sinkD->deliverPreroll(FRAME_TYPE::I);
 unsigned char *dst[4];stride_t stride[4];
 if (csp_isYUVplanar(csp))
  {
   dst[0]=(unsigned char*)frame->GetReadPtr(PLANAR_Y);stride[0]=frame->GetPitch(PLANAR_Y);
   dst[1]=(unsigned char*)frame->GetReadPtr(PLANAR_U);stride[1]=frame->GetPitch(PLANAR_U);
   dst[2]=(unsigned char*)frame->GetReadPtr(PLANAR_V);stride[2]=frame->GetPitch(PLANAR_V);
   dst[3]=NULL;stride[3]=0;
  }
 else
  {
   dst[0]=(unsigned char*)frame->GetReadPtr();dst[1]=dst[2]=dst[3]=NULL;
   stride[0]=frame->GetPitch();stride[1]=stride[2]=stride[3]=NULL;
  }
 TffPict pict(csp,dst,stride,r,true,FRAME_TYPE::I,FIELD_TYPE::PROGRESSIVE_FRAME,srcLen,pIn);
 return sinkD->deliverDecodedSample(pict);
}
void TvideoCodecAvisynth::end(void)
{
 if (avisynth) delete avisynth;avisynth=NULL;
}
