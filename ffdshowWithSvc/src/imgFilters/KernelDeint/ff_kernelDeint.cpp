/*
	KernelDeint() deinterlacing plugin for Avisynth.

	Based on the original KernelDeint plugin (c) 2003 Donald A. Graft
	MMX optimizations + new motion mask code (c) 2004 Kurt B. Pruenner

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <stddef.h>
#include "../../mem_align.h"
#include <malloc.h>
#include <string.h>
#include <unknwn.h>
#include "../../inttypes.h"
#include "../../simd.h"
#include "ff_kernelDeint.h"
#include "../../compiler.h"
#include <assert.h>

#pragma warning(disable:4127)
#pragma warning(disable:4799)
#pragma warning(disable:963)

#if !defined(DEBUG) && (!defined(__INTEL_COMPILER) && !defined(__GNUC__))
 #pragma message("Microsoft compilers are unable to produce optimized binary of ff_kernelDeint, use GCC or Intel C++ Compiler instead.")
#endif

static const __int64 qword_4354h=0x4354435443544354LL;
static const __int64 qword_15c2h=0x15c215c215c215c2LL;
static const __int64 qword_0ed9h=0x0ed90ed90ed90ed9LL;
static const __int64 qword_0354h=0x0354035403540354LL;
static const __int64 qword_03f8h=0x03f803f803f803f8LL;
static const __int64 qword_0fh=0x0f0f0f0f0f0f0f0fLL;
static const __int64 qword_14h=0x1414141414141414LL;
static const __int64 qword_10h=0x1010101010101010LL;
static const __int64 qword_140fh=0x140f140f140f140fLL;
static const __int64 qword_ff00h=0xff00ff00ff00ff00ULL;
static const __int64 qword_000000ffh=0x000000ff000000ffLL;
static const __int64 qword_00ff0000h=0x00ff000000ff0000LL;

struct TVideoFrame
{
private:
 int Bpp;
 void init(void)
  {
   memset(ptr,0,sizeof(ptr));
   memset(dx,0,sizeof(dx));
   memset(dy,0,sizeof(dy));
   memset(stride,0,sizeof(stride));
   Bpp=0;
  }

 void copy(BYTE* dstp, stride_t dst_pitch, const BYTE* srcp, stride_t src_pitch, int row_size, int height)
  {
   if (dst_pitch == src_pitch && src_pitch == row_size)
    {
     memcpy(dstp, srcp, src_pitch * height);
    }
   else
    {
     for (int y=height; y>0; --y)
      {
       memcpy(dstp, srcp, row_size);
       dstp += dst_pitch;
       srcp += src_pitch;
      }
    }
  }

public:
 TVideoFrame(void)
  {
   init();
  }
 TVideoFrame(bool yv12,unsigned int Idx,unsigned int Idy,const unsigned char *src[3],stride_t srcStride[3],stride_t Istride,int field)
  {
   init();
   Bpp=yv12?1:2;
   static const int shift[3]={0,1,1};
   for (int i=0;i<(yv12?3:1);i++)
    {
     dx[i]=Idx>>shift[i];dy[i]=(Idy/2)>>shift[i];
     stride[i]=Istride;
     ptr[i]=(unsigned char*)aligned_malloc(stride[i]*dy[i]);
     copy(ptr[i],stride[i],src[i]+field*srcStride[i],srcStride[i]*2,Bpp*dx[i],dy[i]);
    }
  }
 ~TVideoFrame(void)
  {
   for (int i=0;i<3;i++)
    if (ptr[i]) aligned_free(ptr[i]);
  }
 unsigned char* ptr[3];
 unsigned int dx[3],dy[3];
 stride_t stride[3];
 const unsigned char* GetReadPtr(int plane=0) {return ptr[plane];}
 unsigned char* GetWritePtr(int plane=0) {return ptr[plane];}
 stride_t GetPitch(int plane=0) {return stride[plane];}
 unsigned int GetRowSize(int plane=0) {return Bpp*dx[plane];}
 unsigned int GetHeight(int plane=0) {return dy[plane];}
};
typedef TVideoFrame *PVideoFrame;

static const bool YV12=true,YUY2=false;

template<bool YV12> struct TshowMotionMaskTraits;
template<> struct TshowMotionMaskTraits<YV12>
{
 typedef unsigned char Tpixel;
 static const Tpixel mask=0xff,mask2=(Tpixel)~235;
};
template<> struct TshowMotionMaskTraits<YUY2>
{
 typedef unsigned long Tpixel;
 static const Tpixel mask=0x00ff00ff,mask2=(Tpixel)~0x00eb00eb;
};

struct KernelDeintMask
{
 KernelDeintMask(int Iwidth,int Iheight):width(Iwidth),byteWidth(Iwidth),height(Iheight),size(1)
  {
   bytePitch=pitch=(width+15)&(-16);
   buffer=(unsigned char*)aligned_malloc(bytePitch*height);
  }
 KernelDeintMask(int Iwidth,int Iheight,int Isize):width(Iwidth),height(Iheight),size(Isize)
  {
   byteWidth=width*size;
   bytePitch=(byteWidth+15)&(-16);
   pitch=bytePitch/size;
   buffer=(unsigned char*)aligned_malloc(bytePitch*height);
  }
 ~KernelDeintMask()
  {
   aligned_free(buffer);
  }
 void set(int b)
  {
   memset(buffer,b,bytePitch*height);
  }

 int width;
 int byteWidth;
 int height;
 stride_t bytePitch;
 stride_t pitch;
 int size;

 unsigned char *buffer;
};

class TkernelDeint :public IkernelDeint
{
private:
 bool isYV12;
 int order,threshold;
 unsigned int width,height;
 bool sharp,twoway,linked,map,bob;
 KernelDeintMask *fullsizeMask;
 KernelDeintMask *halfsizeMask;

 unsigned char *scratch;int scratchPitch;
 void (TkernelDeint::*Deinterlace_0fc)(int plane,int n,int order,KernelDeintMask* mask,unsigned char *dst[3],stride_t dstStride[3]);
 bool masksFilled;
public:
 TkernelDeint(bool IisYV12,unsigned int Iwidth,unsigned int Iheight,unsigned int rowsize,int Iorder,int Ithreshold,bool Isharp,bool Itwoway,bool Ilinked,bool Imap,bool Ibob,int /*cpuflags*/):
  width(Iwidth),height(Iheight),
  isYV12(IisYV12),
  order(Iorder),
  threshold(Ithreshold),
  sharp(Isharp),
  twoway(Itwoway),
  linked(Ilinked),
  map(Imap),
  bob(Ibob),
  fieldsptr(0),fieldstart(0),

  masksFilled(false)
  {
   if (isYV12)
    {
     fullsizeMask=new KernelDeintMask(width,height/2);
     halfsizeMask=new KernelDeintMask(width/2,height/4);
     if (!sharp && !twoway)
      Deinterlace_0fc=&TkernelDeint::Deinterlace_0<false,false,8,YV12>;
     else if (!sharp && twoway)
      Deinterlace_0fc=&TkernelDeint::Deinterlace_0<false,true,8,YV12>;
     else if (sharp && !twoway)
      Deinterlace_0fc=&TkernelDeint::Deinterlace_0<true,false,8,YV12>;
     else if (sharp && twoway)
      Deinterlace_0fc=&TkernelDeint::Deinterlace_0<true,true,8,YV12>;
    }
   else
    {
     fullsizeMask=new KernelDeintMask(width,height/2,2); //YUY2
     halfsizeMask=NULL;
     if (!sharp && !twoway)
      Deinterlace_0fc=&TkernelDeint::Deinterlace_0<false,false,8,YUY2>;
     else if (!sharp && twoway)
      Deinterlace_0fc=&TkernelDeint::Deinterlace_0<false,true,8,YUY2>;
     else if (sharp && !twoway)
      Deinterlace_0fc=&TkernelDeint::Deinterlace_0<true,false,8,YUY2>;
     else if (sharp && twoway)
      Deinterlace_0fc=&TkernelDeint::Deinterlace_0<true,true,8,YUY2>;
    }

   scratchPitch=(rowsize+15)&(-16);
   scratch=(unsigned char*)aligned_malloc(scratchPitch*8*3);

   framenum=0;memset(fields,0,sizeof(fields));
  }
 ~TkernelDeint()
  {
   for (int i=0;i<NFIELDS;i++)
    if (fields[i]) delete fields[i];
   delete fullsizeMask;
   if (halfsizeMask) delete halfsizeMask;
   aligned_free(scratch);
  }
 STDMETHODIMP_(void) destroy(void)
  {
   delete this;
  }
 STDMETHODIMP_(void) setOrder(int order)
  {
   this->order=order;
  }
private:
 enum {PLANAR_Y=0,PLANAR_U=1,PLANAR_V=2};
 int framenum;
 STDMETHODIMP_(void) getFrame(const unsigned char *cur[3],stride_t srcStride[3],unsigned char *dst[3],stride_t dstStride[3],int bobframe) //hinting is done by caller
  {
   if (bobframe==0)
    {
     if (fields[fieldsptr]) delete fields[fieldsptr];fields[fieldsptr++]=new TVideoFrame(isYV12,width,height,cur,srcStride,scratchPitch,0);
     if (fields[fieldsptr]) delete fields[fieldsptr];fields[fieldsptr++]=new TVideoFrame(isYV12,width,height,cur,srcStride,scratchPitch,1);
     fieldstart+=2;if (fieldsptr==NFIELDS) fieldsptr=0;
    }
   //if (!bob) n*=2;
   if (isYV12)
    KernelDeint<YV12>(dst,dstStride,framenum*2+bobframe);
   else
    KernelDeint<YUY2>(dst,dstStride,framenum*2+bobframe);
   if (!bob || bobframe==1)
    framenum++;
   _mm_empty();
  }
 static const int NFIELDS=6;
 PVideoFrame fields[NFIELDS];int fieldstart,fieldsptr;
 PVideoFrame GetField(int n)
  {
   PVideoFrame field=NULL;
   do
    {
     if (n<0) n=0;
     n=fieldsptr-(fieldstart-n);
     if (n<0) n+=NFIELDS;
     field=fields[n^(1-order)];
     n--;
    } while (!field);
   return field;
  }

 static void HalveMotionMask_YV12_MMX(KernelDeintMask* halfsizeMask, KernelDeintMask* fullsizeMask)
  {
   unsigned char* halfMaskPtr=halfsizeMask->buffer;
   stride_t halfMaskPitch=halfsizeMask->bytePitch;

   unsigned char* fullMaskPtr=fullsizeMask->buffer;
   stride_t fullMaskPitch=fullsizeMask->bytePitch;

   int rowSize=halfsizeMask->width;

   __m64 mm2=_mm_set1_pi8(-128/*(char)0x80*/);

   for (int rowsLeft=halfsizeMask->height;rowsLeft;halfMaskPtr+=halfMaskPitch,fullMaskPtr+=fullMaskPitch*2,rowsLeft--)
    {
     unsigned char *esi=fullMaskPtr;
     unsigned char *edx=esi+fullMaskPitch;
     unsigned char *edi=halfMaskPtr;

     int ecx=rowSize;
     ecx+=7;
     ecx&=-8;

     esi+=ecx;
     esi+=ecx;

     edx+=ecx;
     edx+=ecx;

     edi+=ecx;

     ecx=-ecx;

     for (;ecx;ecx+=8)
      {
       __m64 mm0,mm1;
       movq (mm0,esi+2*ecx  );
       movq (mm1,esi+2*ecx+8);

       por (mm0,edx+2*ecx  );
       por (mm1,edx+2*ecx+8);

       psrlw (mm0,1);
       psrlw (mm1,1);

       packuswb (mm0,mm1);
       pxor (mm0,mm2);
       pcmpgtb (mm0,mm2);

       movq (edi+ecx,mm0);
      }
    }
    //_mm_empty();
  }

 template<int PART,bool HALFSIZE,bool OVERWRITE,bool TOPFIRST> static __forceinline void MotionMaskLine_YV12_2_MMX(int &bytesLeft,const unsigned char* &curSrcAPtr,const stride_t srcAPitch,const unsigned char* &curSrcBPtr/*,const int srcBPitch*/,unsigned char* &curMaskPtr,const stride_t maskPitch,int threshold)
  {
	//mov eax,080808080h
	//movd mm5,eax
	//punpcklbw mm5,mm5

	__m64 mm5=_mm_set1_pi8(-128/*(char)0x80*/);

	//mov eax,001010101h
	//movd mm6,eax
	//punpcklbw mm6,mm6

	__m64 mm6=_mm_set1_pi8(1);

        __m64 mm7=_mm_set1_pi8((char)threshold);
        /*
	mov eax,threshold
	movd mm7,eax
	punpcklbw mm7,mm7
	punpcklbw mm7,mm7
	punpcklbw mm7,mm7
	*/

	pxor (mm7,mm5);

	stride_t eax=srcAPitch;

	stride_t ebx=maskPitch;
	ebx+=ebx;

	const unsigned char *esi=curSrcAPtr;
	const unsigned char *edi=curSrcBPtr;
	unsigned char *edx=curMaskPtr;

	int ecx=bytesLeft;
	ecx+=7;
	ecx&=-8;

        for (;ecx!=0;esi+=8,edi+=8,ecx-=8)
        {
              __m64 mm0,mm1,mm2,mm3;

	      if (PART != 2 || TOPFIRST)
	      {
		      movq (mm0,esi);
		      movq (mm2,edi);

		      // convert unsigned to signed

		      pxor (mm0,mm5);
		      pxor (mm2,mm5);

		      // build a mask in mm3 of which bytes in mm2 are bigger
		      // than their counterparts in mm0

		      movq (mm3,mm2);
		      pcmpgtb (mm3,mm0);

		      // calculate the differences

		      psubb (mm0,mm2);

		      // flip sign of the bytes masked by mm3 (twos complement)

		      pxor (mm0,mm3);
		      pand (mm3,mm6);
		      paddb (mm0,mm3);

		      pxor (mm0,mm5);
		      pcmpgtb (mm0,mm7);

	      }else{
		      pxor (mm0,mm0);
	      }

	      if ((PART > 1) && ((PART < 4) || !(TOPFIRST)))
	      {

		      movq (mm1,esi+eax);
		      movq (mm2,edi+eax);

		      // convert unsigned to signed

		      pxor (mm1,mm5);
		      pxor (mm2,mm5);

		      // build a mask in mm3 of which bytes in mm2 are bigger
		      // than their counterparts in mm1

		      movq (mm3,mm2);
		      pcmpgtb (mm3,mm1);

		      // calculate the differences

		      psubb (mm1,mm2);

		      // flip sign of the bytes masked by mm3 (twos complement)

		      pxor (mm1,mm3);
		      pand (mm3,mm6);
		      paddb (mm1,mm3);

		      pxor (mm1,mm5);
		      pcmpgtb (mm1,mm7);

		      por (mm0,mm1);

	      }

	      if (HALFSIZE)
	      {

		      movq (mm1,mm0);

		      punpcklbw (mm0,mm0);
		      punpckhbw (mm1,mm1);

		      if (OVERWRITE){
			      movq (edx      ,mm0);
			      movq (edx+8    ,mm1);
			      movq (edx+ebx  ,mm0);
			      movq (edx+ebx+8,mm1);
		      }else{

			      movq (mm2,edx      );
			      movq (mm3,edx+8    );

			      por (mm2,mm0);
			      por (mm3,mm1);

			      movq (edx      ,mm2);
			      movq (edx+8    ,mm3);

			      movq (mm2,edx+ebx  );
			      movq (mm3,edx+ebx+8);

			      por (mm2,mm0);
			      por (mm3,mm1);

			      movq (edx+ebx  ,mm2);
			      movq (edx+ebx+8,mm3);

		      }

		      edx+=16;

	      }else{

		      if (OVERWRITE){
			      movq (edx,mm0);
		      }else{
			      movq (mm1,edx);
			      por  (mm1,mm0);
			      movq (edx,mm1);
		      }

		      edx+=8;

	      }
       	}
  }

 template<int PART> static __forceinline void MotionMaskLine_YV12_1(const int rowSize,const unsigned char* &srcAPtr,const stride_t srcAPitch,const unsigned char* &srcBPtr,const stride_t srcBPitch,unsigned char* &maskPtr,const stride_t maskPitch,bool halfsize,bool overwrite,int threshold,int order)
  {
	int bytesLeft=rowSize;
	const unsigned char *curSrcAPtr=srcAPtr;
	const unsigned char *curSrcBPtr=srcBPtr;

	if (halfsize)
	{
		//#define HALFSIZE

		unsigned char* curMaskPtr=maskPtr;

		if (PART == 1)
		 {

			if      ( overwrite && (order == 1))
			{
				//#define OVERWRITE
				//#define TOPFIRST

				MotionMaskLine_YV12_2_MMX<PART,true,true,true>(bytesLeft,curSrcAPtr,srcAPitch,curSrcBPtr,curMaskPtr,maskPitch,threshold);
				goto part1end1;
			}
			else if ( overwrite && (order != 1))
			{
				//#define OVERWRITE
				//#undef  TOPFIRST

				MotionMaskLine_YV12_2_MMX<PART,true,true,false>(bytesLeft,curSrcAPtr,srcAPitch,curSrcBPtr,curMaskPtr,maskPitch,threshold);
				goto part1end1;
			}
			//else

		 }

		if (!overwrite && (order == 1))
		{
			//#undef  OVERWRITE
			//#define TOPFIRST

			MotionMaskLine_YV12_2_MMX<PART,true,false,true>(bytesLeft,curSrcAPtr,srcAPitch,curSrcBPtr,curMaskPtr,maskPitch,threshold);
		}
		else if (!overwrite && (order != 1))
		{
			//#undef  OVERWRITE
			//#undef  TOPFIRST

			MotionMaskLine_YV12_2_MMX<PART,true,false,false>(bytesLeft,curSrcAPtr,srcAPitch,curSrcBPtr,curMaskPtr,maskPitch,threshold);
		}
               part1end1:
		maskPtr+=2*2*maskPitch;
	}
	else
	{
		//#undef HALFSIZE

		unsigned char* curMaskPtr=maskPtr;

		if (PART == 1)
		 {

			if      ( overwrite && (order == 1))
			{
				//#define OVERWRITE
				//#define TOPFIRST

				MotionMaskLine_YV12_2_MMX<PART,false,true,true>(bytesLeft,curSrcAPtr,srcAPitch,curSrcBPtr,curMaskPtr,maskPitch,threshold);
				goto part1end2;
			}
			else if ( overwrite && (order != 1))
			{
				//#define OVERWRITE
				//#undef  TOPFIRST

				MotionMaskLine_YV12_2_MMX<PART,false,true,false>(bytesLeft,curSrcAPtr,srcAPitch,curSrcBPtr,curMaskPtr,maskPitch,threshold);
				goto part1end2;
			}
			//else

		 }

		if (!overwrite && (order == 1))
		{
			//#undef  OVERWRITE
			//#define TOPFIRST

			MotionMaskLine_YV12_2_MMX<PART,false,false,true>(bytesLeft,curSrcAPtr,srcAPitch,curSrcBPtr,curMaskPtr,maskPitch,threshold);
		}
		else if (!overwrite && (order != 1))
		{
			//#undef  OVERWRITE
			//#undef  TOPFIRST

			MotionMaskLine_YV12_2_MMX<PART,false,false,false>(bytesLeft,curSrcAPtr,srcAPitch,curSrcBPtr,curMaskPtr,maskPitch,threshold);
		}
               part1end2:
		maskPtr+=maskPitch;
	}

	srcAPtr+=srcAPitch;
	srcBPtr+=srcBPitch;
  }

 template<int PITCH_MOD> __forceinline void MotionMask_YV12_0(KernelDeintMask* mask,
                                                int order,int plane,
                                                bool overwrite,bool linked,int n)
  {
   bool halfsize=(plane != PLANAR_Y) && linked;
   stride_t maskPitch=mask->pitch;
   if (halfsize) maskPitch/=2;

   PVideoFrame srcA;
   PVideoFrame srcB;

   stride_t srcAPitch;
   stride_t srcBPitch;

   const unsigned char* srcAPtr;
   const unsigned char* srcBPtr;

   int rowSize;

   unsigned char* maskPtr;

   srcA=GetField(n+1);
   srcB=GetField(n-1);

   srcAPitch=srcA->GetPitch(plane);
   srcBPitch=srcB->GetPitch(plane);

   srcAPtr=srcA->GetReadPtr(plane);
   srcBPtr=srcB->GetReadPtr(plane);

   rowSize=srcA->GetRowSize(plane);

   bool oddPitch=(srcAPitch % PITCH_MOD) != 0;

   maskPtr=mask->buffer;

   //unsigned short doubleThreshold=(unsigned short)(2*threshold);

   int height=srcA->GetHeight(plane);
   int rowsLeft=height;

   if (oddPitch) rowsLeft--;

   while (rowsLeft > 0)
   {
           MotionMaskLine_YV12_1<1>(rowSize,srcAPtr,srcAPitch,srcBPtr,srcBPitch,maskPtr,maskPitch,halfsize,overwrite,threshold,order);

           rowsLeft--;
   }

   if (oddPitch)
   {
           // copy bottom parts of fields to scratch buffer

           stride_t safePitch=(scratchPitch >= srcAPitch ? srcAPitch : scratchPitch);

           //unsigned char* scratchPtr=scratch;

           memcpy(scratch,srcAPtr,safePitch);
           memcpy(scratch+scratchPitch,srcBPtr,safePitch);

           // change pointers to point to scratch buffer

           srcAPtr=scratch;
           srcBPtr=srcAPtr+scratchPitch;

           srcAPitch=srcBPitch=scratchPitch*2;

           MotionMaskLine_YV12_1<1>(rowSize,srcAPtr,srcAPitch,srcBPtr,srcBPitch,maskPtr,maskPitch,halfsize,overwrite,threshold,order);
   }

   overwrite=false;

   srcA=GetField(n  );
   srcB=GetField(n-2);

   srcAPtr=srcA->GetReadPtr(plane)-(1-order)*srcAPitch;
   srcBPtr=srcB->GetReadPtr(plane)-(1-order)*srcBPitch;;

   maskPtr=mask->buffer;

   MotionMaskLine_YV12_1<2>(rowSize,srcAPtr,srcAPitch,srcBPtr,srcBPitch,maskPtr,maskPitch,halfsize,overwrite,threshold,order);

   rowsLeft=height-2;

   if (oddPitch) rowsLeft--;

   while (rowsLeft > 0)
   {
           MotionMaskLine_YV12_1<3>(rowSize,srcAPtr,srcAPitch,srcBPtr,srcBPitch,maskPtr,maskPitch,halfsize,overwrite,threshold,order);

           rowsLeft--;
   }

   if (oddPitch)
   {
           // copy bottom parts of fields to scratch buffer

           stride_t safePitch=(scratchPitch >= srcAPitch ? srcAPitch : scratchPitch);

           unsigned char* scratchPtr=scratch;

           for (rowsLeft=2; rowsLeft > 0; rowsLeft--)
           {
                   memcpy(scratchPtr,srcAPtr,safePitch);
                   scratchPtr+=scratchPitch;

                   memcpy(scratchPtr,srcBPtr,safePitch);
                   scratchPtr+=scratchPitch;

                   srcAPtr+=srcAPitch; srcBPtr+=srcBPitch;
           }

           // change pointers to point to scratch buffer

           srcAPtr=scratch;
           srcBPtr=srcAPtr+scratchPitch;

           srcAPitch=srcBPitch=scratchPitch*2;

           MotionMaskLine_YV12_1<3>(rowSize,srcAPtr,srcAPitch,srcBPtr,srcBPitch,maskPtr,maskPitch,halfsize,overwrite,threshold,order);
   }

   MotionMaskLine_YV12_1<4>(rowSize,srcAPtr,srcAPitch,srcBPtr,srcBPitch,maskPtr,maskPitch,halfsize,overwrite,threshold,order);
  }

 __forceinline void BuildPlaneMotionMask_YV12_MMX(KernelDeintMask* mask,
                                    int order,
                                    int plane,
                                    bool linked,
                                    bool overwrite,int n)
  {
   //#define COLORSPACE COLORSPACE_YV12
   //#define PITCH_MOD 8

   //#define MOTIONMASK_STAGE1 "include/MotionMaskLine_YV12_1.cpp"
   //#define MOTIONMASK_STAGE2 "include/MotionMaskLine_YV12_2_MMX.cpp"

   MotionMask_YV12_0<8>(mask,order,plane,overwrite,linked,n);

   //_mm_empty();
  }

 template<int PART,bool LINKED,bool TOPFIRST> static __forceinline void MotionMaskLine_YUY2_2_MMX(int &bytesLeft,const unsigned char* &curSrcAPtr,const stride_t srcAPitch,const unsigned char* &curSrcBPtr/*,const int srcBPitch*/,unsigned char* &curMaskPtr,const stride_t /*maskPitch*/,int threshold)
  {
        __m64 mm4,mm0,mm2,mm3,mm1;
        pxor (mm4,mm4);

        //;mov eax,080808080h
        //;movd mm5,eax
        //;punpcklbw mm5,mm5

	__m64 mm5=_mm_set1_pi8(-128/*(char)0x80*/);

	//mov eax,001010101h
	//movd mm6,eax
	//punpcklbw mm6,mm6

	__m64 mm6=_mm_set1_pi8(1);

        __m64 mm7=_mm_set1_pi8((char)threshold);
        //mov eax,threshold
        //movd mm7,eax
        //punpcklbw mm7,mm7
        //punpcklbw mm7,mm7
        //punpcklbw mm7,mm7

        pxor (mm7,mm5);

        stride_t eax=srcAPitch;

        const unsigned char *esi=curSrcAPtr;
        const unsigned char *edi=curSrcBPtr;
        unsigned char *edx=curMaskPtr;

        int ecx=bytesLeft;
        ecx+=7;
        ecx&=-8;

ColLoop:

        if ((PART != 2) || (TOPFIRST)){

                movq (mm0,esi);
                movq (mm2,edi);

                //; convert unsigned to signed

                pxor (mm0,mm5);
                pxor (mm2,mm5);

                //; build a mask in mm3 of which bytes in mm2 are bigger
                //; than their counterparts in mm0

                movq (mm3,mm2);
                pcmpgtb (mm3,mm0);

                //; calculate the differences

                psubb (mm0,mm2);

                //; flip sign of the bytes masked by mm3 (twos complement)

                pxor (mm0,mm3);
                pand (mm3,mm6);
                paddb (mm0,mm3);

                pxor (mm0,mm5);
                pcmpgtb (mm0,mm7);

        }else{
                pxor (mm0,mm0);
        }

        if ((PART > 1) && ((PART < 4) || !(TOPFIRST))){

                movq (mm1,esi+eax);
                movq (mm2,edi+eax);

                //; convert unsigned to signed

                pxor (mm1,mm5);
                pxor (mm2,mm5);

                //; build a mask in mm3 of which bytes in mm2 are bigger
                //; than their counterparts in mm1

                movq (mm3,mm2);
                pcmpgtb (mm3,mm1);

                //; calculate the differences

                psubb (mm1,mm2);

                //; flip sign of the bytes masked by mm3 (twos complement)

                pxor (mm1,mm3);
                pand (mm3,mm6);
                paddb (mm1,mm3);

                pxor (mm1,mm5);
                pcmpgtb (mm1,mm7);
                por (mm0,mm1);

        }

        if (LINKED){

                //;mov ebx,0ff00ff00h
                //;movd mm1,ebx
                //;punpckldq mm1,mm1

                movq (mm1,qword_ff00h);

                pand (mm1,mm0);
                psrld (mm1,1);
                pcmpgtd (mm1,mm4);

                por (mm0,mm1);

                //;mov ebx,0000000ffh
                //;movd mm2,ebx

                movq (mm2,qword_000000ffh);

                //;shl ebx,16
                //;movd mm3,ebx

                movq (mm2,qword_00ff0000h);

                //;punpckldq mm2,mm2
                //;punpckldq mm3,mm3

                movq (mm1,mm2);
                pand (mm1,mm0);
                psrld (mm1,1);
                pcmpgtd (mm1,mm4);

                psubusb (mm1,mm3);

                por (mm0,mm1);

                pand (mm3,mm0);
                psrld (mm3,1);
                pcmpgtd (mm3,mm4);

                psubusb (mm3,mm2);

                por (mm0,mm3);

        }else{

                //;mov ebx,0ff00ff00h
                //;movd mm1,ebx
                //;punpckldq mm1,mm1

                movq (mm1,qword_ff00h);
                movq (mm2,mm1);

                pand (mm1,mm0);
                psrld (mm1,1);

                pcmpgtd (mm1,mm4);
                pand (mm2,mm1);

                por (mm0,mm2);

        }

        if (PART == 1){
                movq (edx,mm0);
        }else{
                movq (mm1,edx);
                por (mm1,mm0);
                movq (edx,mm1);
        }

        esi+=8;
        edi+=8;
        edx+=8;

        ecx-=8;
        if (ecx!=0)
         goto ColLoop;
  }
 template<int COLORSPACE,int PART> static __forceinline void MotionMaskLine_YUY2_RGB_1(const int rowSize,const unsigned char* &srcAPtr,const stride_t srcAPitch,const unsigned char* &srcBPtr,const stride_t srcBPitch,unsigned char* &maskPtr,const stride_t maskPitch,bool linked,int order,int threshold)
  {
        int bytesLeft=rowSize;
        const unsigned char *curSrcAPtr=srcAPtr;
        const unsigned char *curSrcBPtr=srcBPtr;

        unsigned char* curMaskPtr=(unsigned char*)maskPtr;//long

        if (COLORSPACE == YUY2){

                if (!linked)
                {
                        if      (order == 1)
                        {
                                //#undef  LINKED
                                //#define TOPFIRST

                                MotionMaskLine_YUY2_2_MMX<PART,false,true>(bytesLeft,curSrcAPtr,srcAPitch,curSrcBPtr,curMaskPtr,maskPitch,threshold);
                        }
                        else if (order != 1)
                        {
                                //#undef  LINKED
                                //#undef  TOPFIRST

                                MotionMaskLine_YUY2_2_MMX<PART,false,false>(bytesLeft,curSrcAPtr,srcAPitch,curSrcBPtr,curMaskPtr,maskPitch,threshold);
                        }
                }
                //else

        }

        if (linked)
        {
                if      (order == 1)
                {
                        //#define LINKED
                        //#define TOPFIRST

                        MotionMaskLine_YUY2_2_MMX<PART,true,true>(bytesLeft,curSrcAPtr,srcAPitch,curSrcBPtr,curMaskPtr,maskPitch,threshold);
                }
                else if (order != 1)
                {
                        //#define LINKED
                        //#undef  TOPFIRST

                        MotionMaskLine_YUY2_2_MMX<PART,true,false>(bytesLeft,curSrcAPtr,srcAPitch,curSrcBPtr,curMaskPtr,maskPitch,threshold);
                }
        }

        maskPtr+=maskPitch;
        srcAPtr+=srcAPitch;
        srcBPtr+=srcBPitch;

  }
 template<int COLORSPACE,int PITCH_MOD> __forceinline void MotionMask_YUY2_RGB_0(KernelDeintMask* mask,
                                                int order,
                                                int n)
  {
        stride_t maskPitch=mask->bytePitch;

        PVideoFrame srcA;
        PVideoFrame srcB;

        stride_t srcAPitch;
        stride_t srcBPitch;

        const unsigned char* srcAPtr;
        const unsigned char* srcBPtr;

        int rowSize;

        unsigned char* maskPtr;

        srcA=GetField(n  );
        srcB=GetField(n-2);

        srcAPitch=srcA->GetPitch();
        srcBPitch=srcB->GetPitch();

        srcAPtr=srcA->GetReadPtr();
        srcBPtr=srcB->GetReadPtr();

        rowSize=srcA->GetRowSize();

        bool oddPitch=(srcAPitch % PITCH_MOD) != 0;

        maskPtr=mask->buffer;

        //int bytesLeft;
        //unsigned short doubleThreshold=(unsigned short)(2*threshold);

        //const unsigned char* curSrcAPtr;
        //const unsigned char* curSrcBPtr;

        int height=srcA->GetHeight();
        int rowsLeft=height;

        if (oddPitch) rowsLeft--;

        while (rowsLeft > 0)
        {
                //#define PART 1

                MotionMaskLine_YUY2_RGB_1<COLORSPACE,1>(rowSize,srcAPtr,srcAPitch,srcBPtr,srcBPitch,maskPtr,maskPitch,linked,order,threshold);

                rowsLeft--;
        }

        if (oddPitch)
        {
                // copy bottom parts of fields to scratch buffer

                stride_t safePitch=(scratchPitch >= srcAPitch ? srcAPitch : scratchPitch);

                memcpy(scratch,srcAPtr,safePitch);
                memcpy(scratch+scratchPitch,srcBPtr,safePitch);

                // change pointers to point to scratch buffer

                srcAPtr=scratch;
                srcBPtr=srcAPtr+scratchPitch;

                srcAPitch=srcBPitch=scratchPitch*2;

                //#define PART 1

                MotionMaskLine_YUY2_RGB_1<COLORSPACE,1>(rowSize,srcAPtr,srcAPitch,srcBPtr,srcBPitch,maskPtr,maskPitch,linked,order,threshold);
        }

        srcA=GetField(n+1);
        srcB=GetField(n-1);

        srcAPtr=srcA->GetReadPtr()-(1-order)*srcAPitch;
        srcBPtr=srcB->GetReadPtr()-(1-order)*srcBPitch;;

        maskPtr=mask->buffer;

        //#define PART 2

        MotionMaskLine_YUY2_RGB_1<COLORSPACE,2>(rowSize,srcAPtr,srcAPitch,srcBPtr,srcBPitch,maskPtr,maskPitch,linked,order,threshold);

        rowsLeft=height-2;

        if (oddPitch) rowsLeft--;

        while (rowsLeft > 0)
        {
                //#define PART 3

                MotionMaskLine_YUY2_RGB_1<COLORSPACE,3>(rowSize,srcAPtr,srcAPitch,srcBPtr,srcBPitch,maskPtr,maskPitch,linked,order,threshold);

                rowsLeft--;
        }

        if (oddPitch)
        {
                // copy bottom parts of fields to scratch buffer

                stride_t safePitch=(scratchPitch >= srcAPitch ? srcAPitch : scratchPitch);

                unsigned char* scratchPtr=scratch;

                for (rowsLeft=2; rowsLeft > 0; rowsLeft--)
                {
                        memcpy(scratchPtr,srcAPtr,safePitch);
                        scratchPtr+=scratchPitch;

                        memcpy(scratchPtr,srcBPtr,safePitch);
                        scratchPtr+=scratchPitch;

                        srcAPtr+=srcAPitch; srcBPtr+=srcBPitch;
                }

                // change pointers to point to scratch buffer

                srcAPtr=scratch+2*scratchPitch;
                srcBPtr=srcAPtr+scratchPitch;

                srcAPitch=srcBPitch=scratchPitch*2;

                //#define PART 3

                MotionMaskLine_YUY2_RGB_1<COLORSPACE,3>(rowSize,srcAPtr,srcAPitch,srcBPtr,srcBPitch,maskPtr,maskPitch,linked,order,threshold);
        }

        //#define PART 4

        MotionMaskLine_YUY2_RGB_1<COLORSPACE,4>(rowSize,srcAPtr,srcAPitch,srcBPtr,srcBPitch,maskPtr,maskPitch,linked,order,threshold);

  }

 void BuildMotionMask_YUY2_MMX(KernelDeintMask* mask,
                               int order,
                               int n)
  {
   //#define COLORSPACE COLORSPACE_YUY2
   //#define PITCH_MOD 8

   //#define MOTIONMASK_STAGE1 "include/MotionMaskLine_YUY2+RGB_1.cpp"
   //#define MOTIONMASK_STAGE2 "include/MotionMaskLine_YUY2_2_MMX.cpp"

   MotionMask_YUY2_RGB_0<YUY2,8>(mask,order,n);

   //#include "include\MotionMask_YUY2+RGB_0.cpp"
   //_mm_empty();
  }

 template<bool YV12> void BuildMotionMask(KernelDeintMask* fullsizeMask,
                                          KernelDeintMask* halfsizeMask,
                                          int order,
                                          bool linked,
                                          int n)
  {
   if (YV12)
    if (linked)
     {
      BuildPlaneMotionMask_YV12_MMX(fullsizeMask,order,PLANAR_Y,true,true,n);
      BuildPlaneMotionMask_YV12_MMX(fullsizeMask,order,PLANAR_U,true,false,n);
      BuildPlaneMotionMask_YV12_MMX(fullsizeMask,order,PLANAR_V,true,false,n);
      HalveMotionMask_YV12_MMX(halfsizeMask,fullsizeMask);
     }
    else
     {
      BuildPlaneMotionMask_YV12_MMX(fullsizeMask,order,PLANAR_Y,false,true,n);
      BuildPlaneMotionMask_YV12_MMX(halfsizeMask,order,PLANAR_U,false,true,n);
      BuildPlaneMotionMask_YV12_MMX(halfsizeMask,order,PLANAR_V,false,false,n);
     }
   else
    BuildMotionMask_YUY2_MMX(fullsizeMask,order,n);
  }

 static __forceinline void COPY_LINE(const unsigned char* &srcPtr,stride_t srcPitch,unsigned char* &dstPtr,stride_t dstPitch,int rowSize,int order)
  {
   const unsigned char *esi=srcPtr;
   unsigned char *edi=dstPtr;

   int edx=rowSize;
   int ecx=edx;

   edx+=7;
   ecx&=-32;
   edx&=24;

   esi+=ecx;
   edi+=ecx;

   ecx=-ecx;

   for (;ecx;ecx+=32)
    {
     int eax=ecx;
     __m64 mm0,mm1,mm2,mm3;
     movq (mm0,esi+ecx   );
     movq (mm1,esi+ecx+8 );
     movq (mm2,esi+ecx+16);
     movq (mm3,esi+ecx+24);

     //mov eax,ecx



     movq (edi+eax   ,mm0);
     movq (edi+eax+8 ,mm1);
     movq (edi+eax+16,mm2);
     movq (edi+eax+24,mm3);
    }

   esi+=edx;
   edi+=edx;

   edx=-edx;
   if (edx!=0)
    for (;edx;edx+=8)
     {
      int eax=edx;
      __m64 mm0;
      movq (mm0,esi+edx);
      //mov eax,edx
      movq (edi+eax,mm0);
    }
  dstPtr+=dstPitch;

  if (order == 0) srcPtr+=srcPitch;
 }

 static const int SRC_PREV=1;
 static const int SRC_CUR =2;
 static const int SRC_NEXT=3;

 template<int DIRECTION,int SOURCE> static __forceinline void DeinterlaceLoad(stride_t &eax,const unsigned char* &esi,const unsigned char* &ecx,const unsigned char* &edi,__m64 &TARGET,bool &EAX_INVERTED)
  {
        if ((DIRECTION < 0) && !(EAX_INVERTED)){
                EAX_INVERTED=true;

                eax=-eax;
        }else if ((DIRECTION > 0) && (EAX_INVERTED)){
                EAX_INVERTED=false;

                eax=-eax;
        }

        const unsigned char *ASOURCE;
        if (SOURCE == SRC_PREV)
                ASOURCE=esi;
        else if (SOURCE == SRC_CUR)
                ASOURCE=ecx;
        else if (SOURCE == SRC_NEXT)
                ASOURCE=edi;


        if ((DIRECTION == -2) || (DIRECTION == 2))
                movq (TARGET,ASOURCE + 2*eax);
        else if ((DIRECTION == -1) || (DIRECTION == 1))
                movq (TARGET,ASOURCE + eax);
        else
                movq (TARGET,ASOURCE);


  }

 template<bool HALFSIZE,bool SHARP,bool TWOWAY,int COLORSPACE,int U2,int U1,int D1,int D2,int ASU2,int ASU1,int ASD1,int ASD2> static __forceinline void DeinterlaceLine_2(int &bytesLeft,const unsigned char* &curPrevPtr,const unsigned char* &curSrcPtr,const unsigned char* &curNextPtr,unsigned char* &curDstPtr,const unsigned char* &curMaskPtr,stride_t srcPitch)
  {

        bytesLeft+=7;
        bytesLeft&=-8;

        const unsigned char *esi=curPrevPtr;
        const unsigned char *ecx=curSrcPtr;
        const unsigned char *edi=curNextPtr;
        unsigned char *edx=curDstPtr;

        stride_t eax=srcPitch;
        __m64 mm0,mm1,mm2,mm3,mm4,mm5,mm6,mm7;
  ColLoop:

        const unsigned char  *ebx1=curMaskPtr;
        movq (mm7,ebx1);

        movq (mm6,mm7);
        psrlw (mm6,1);
        packuswb (mm6,mm6);

        curMaskPtr+=8;

        int ebx;movd(ebx,mm6);
        if (ebx==0)
         goto CopyThrough;

        pxor (mm6,mm6);

        if (!SHARP){

                if (!TWOWAY){

                        // intVal=+8*( *(curSrcPtr +ASD1*srcPitch)

                        //#define SOURCE SRC_CUR
                        //#define TARGET mm0
                        //#define DIRECTION ASD1
                        bool EAX_INVERTED=false;
                        DeinterlaceLoad<ASD1,SRC_CUR>(eax,esi,ecx,edi,mm0,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm1,mm0);

                        punpcklbw (mm0,mm6);
                        punpckhbw (mm1,mm6);

                        //                +*(curSrcPtr +ASU1*srcPitch))

                        //#define SOURCE SRC_CUR
                        //#define TARGET mm4
                        //#define DIRECTION ASU1

                        DeinterlaceLoad<ASU1,SRC_CUR>(eax,esi,ecx,edi,mm4,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm5,mm4);

                        punpcklbw (mm4,mm6);
                        punpckhbw (mm5,mm6);

                        paddsw (mm4,mm0);
                        paddsw (mm5,mm1);

                        psllw (mm4,3);
                        psllw (mm5,3);

                        //;            +2*( *(curPrevPtr              ))

                        //#define SOURCE SRC_PREV
                        //#define TARGET mm0
                        //#define DIRECTION 0

                        DeinterlaceLoad<0,SRC_PREV>(eax,esi,ecx,edi,mm0,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm1,mm0);

                        punpcklbw (mm0,mm6);
                        punpckhbw (mm1,mm6);

                        psllw (mm0,1);
                        psllw (mm1,1);

                        paddsw (mm4,mm0);
                        paddsw (mm5,mm1);

                        //;            -    *(curPrevPtr+U1*prevPitch )

                        //#define SOURCE SRC_PREV
                        //#define TARGET mm0
                        //#define DIRECTION U1

                        DeinterlaceLoad<U1,SRC_PREV>(eax,esi,ecx,edi,mm0,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm1,mm0);

                        punpcklbw (mm0,mm6);
                        punpckhbw (mm1,mm6);

                        psubsw (mm4,mm0);
                        psubsw (mm5,mm1);

                        //;            -    *(curPrevPtr+D1*prevPitch );

                        //#define SOURCE SRC_PREV
                        //#define TARGET mm0
                        //#define DIRECTION D1

                        DeinterlaceLoad<D1,SRC_PREV>(eax,esi,ecx,edi,mm0,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm1,mm0);

                        punpcklbw (mm0,mm6);
                        punpckhbw (mm1,mm6);

                        psubsw (mm4,mm0);
                        psubsw (mm5,mm1);

                        if (EAX_INVERTED)
                                eax=-eax;



                }else{// defined TWOWAY

                        //; intVal=+8*( *(curSrcPtr +ASD1*srcPitch)

                        //#define SOURCE SRC_CUR
                        //#define TARGET mm0
                        //#define DIRECTION ASD1
                        bool EAX_INVERTED=false;
                        DeinterlaceLoad<ASD1,SRC_CUR>(eax,esi,ecx,edi,mm0,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm1,mm0);

                        punpcklbw (mm0,mm6);
                        punpckhbw (mm1,mm6);

                        //;                    +*(curSrcPtr +ASU1*srcPitch))

                        //#define SOURCE SRC_CUR
                        //#define TARGET mm4
                        //#define DIRECTION ASU1

                        DeinterlaceLoad<ASU1,SRC_CUR>(eax,esi,ecx,edi,mm4,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm5,mm4);

                        punpcklbw (mm4,mm6);
                        punpckhbw (mm5,mm6);

                        paddsw (mm4,mm0);
                        paddsw (mm5,mm1);

                        psllw (mm4,3);
                        psllw (mm5,3);

                        //;            +2*( *(curNextPtr              )

                        //#define SOURCE SRC_NEXT
                        //#define TARGET mm0
                        //#define DIRECTION 0

                        DeinterlaceLoad<0,SRC_NEXT>(eax,esi,ecx,edi,mm0,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm1,mm0);

                        punpcklbw (mm0,mm6);
                        punpckhbw (mm1,mm6);

                        //;                    +*(curPrevPtr              ))

                        //#define SOURCE SRC_PREV
                        //#define TARGET mm2
                        //#define DIRECTION 0

                        DeinterlaceLoad<0,SRC_PREV>(eax,esi,ecx,edi,mm2,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm3,mm2);

                        punpcklbw (mm2,mm6);
                        punpckhbw (mm3,mm6);

                        paddsw (mm2,mm0);
                        paddsw (mm3,mm1);

                        psllw (mm2,1);
                        psllw (mm3,1);

                        paddsw (mm4,mm2);
                        paddsw (mm5,mm3);

                        //;            -    *(curNextPtr+U1*nextPitch )

                        //#define SOURCE SRC_NEXT
                        //#define TARGET mm0
                        //#define DIRECTION U1

                        DeinterlaceLoad<U1,SRC_NEXT>(eax,esi,ecx,edi,mm0,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm1,mm0);

                        punpcklbw (mm0,mm6);
                        punpckhbw (mm1,mm6);

                        psubsw (mm4,mm0);
                        psubsw (mm5,mm1);

                        //;            -    *(curNextPtr+D1*nextPitch )

                        //#define SOURCE SRC_NEXT
                        //#define TARGET mm0
                        //#define DIRECTION D1

                        DeinterlaceLoad<D1,SRC_NEXT>(eax,esi,ecx,edi,mm0,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm1,mm0);

                        punpcklbw (mm0,mm6);
                        punpckhbw (mm1,mm6);

                        psubsw (mm4,mm0);
                        psubsw (mm5,mm1);

                        //;            -    *(curPrevPtr+D1*prevPitch )

                        //#define SOURCE SRC_PREV
                        //#define TARGET mm0
                        //#define DIRECTION D1

                        DeinterlaceLoad<D1,SRC_PREV>(eax,esi,ecx,edi,mm0,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm1,mm0);

                        punpcklbw (mm0,mm6);
                        punpckhbw (mm1,mm6);

                        psubsw (mm4,mm0);
                        psubsw (mm5,mm1);

                        //;            -    *(curPrevPtr+U1*prevPitch );

                        //#define SOURCE SRC_PREV
                        //#define TARGET mm0
                        //#define DIRECTION U1

                        DeinterlaceLoad<U1,SRC_PREV>(eax,esi,ecx,edi,mm0,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm1,mm0);

                        punpcklbw (mm0,mm6);
                        punpckhbw (mm1,mm6);

                        psubsw (mm4,mm0);
                        psubsw (mm5,mm1);

                        if (EAX_INVERTED)
                                eax=-eax;
                }

                //; dstVal/=16;

                psraw (mm4,4);
                psraw (mm5,4);

        }else{// defined SHARP

                if (!TWOWAY){

                        //; lIntVal=+17236*( *(curSrcPtr +ASD1*srcPitch)

                        //#define SOURCE SRC_CUR
                        //#define TARGET mm0
                        //#define DIRECTION ASD1
                        bool EAX_INVERTED=false;
                        DeinterlaceLoad<ASD1,SRC_CUR>(eax,esi,ecx,edi,mm0,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm1,mm0);

                        punpcklbw (mm0,mm0);//  Byte XX -> Word XXXX
                        punpckhbw (mm1,mm1);//  (more accurate than XX00)

                        psrlw (mm0,2);
                        psrlw (mm1,2);

                        //;                        +*(curSrcPtr +ASU1*srcPitch))

                        //#define SOURCE SRC_CUR
                        //#define TARGET mm4
                        //#define DIRECTION ASU1

                        DeinterlaceLoad<ASU1,SRC_CUR>(eax,esi,ecx,edi,mm4,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm5,mm4);

                        punpcklbw (mm4,mm4);
                        punpckhbw (mm5,mm5);

                        psrlw (mm4,2);
                        psrlw (mm5,2);

                        paddsw (mm4,mm0);
                        paddsw (mm5,mm1);

                        //;mov ebx,043544354h ; 32768*0.526
                        //;movd mm2,ebx
                        //;punpcklwd mm2,mm2

                        movq (mm2,qword_4354h);

                        pmulhw (mm4,mm2);
                        pmulhw (mm5,mm2);

                        //;                 + 5570*( *(curPrevPtr              ))

                        //#define SOURCE SRC_PREV
                        //#define TARGET mm0
                        //#define DIRECTION 0

                        DeinterlaceLoad<0,SRC_PREV>(eax,esi,ecx,edi,mm0,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm1,mm0);

                        punpcklbw (mm0,mm0);
                        punpckhbw (mm1,mm1);

                        psrlw (mm0,2);
                        psrlw (mm1,2);

                        //;mov ebx,015c215c2h ; 32768*0.170
                        //;movd mm2,ebx
                        //;punpcklwd mm2,mm2

                        movq (mm2,qword_15c2h);

                        pmulhw (mm0,mm2);
                        pmulhw (mm1,mm2);

                        paddsw (mm4,mm0);
                        paddsw (mm5,mm1);

                        //;                 - 3801*( *(curPrevPtr+U1*prevPitch )

                        //#define SOURCE SRC_PREV
                        //#define TARGET mm0
                        //#define DIRECTION U1

                        DeinterlaceLoad<U1,SRC_PREV>(eax,esi,ecx,edi,mm0,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm1,mm0);

                        punpcklbw (mm0,mm0);
                        punpckhbw (mm1,mm1);

                        psrlw (mm0,2);
                        psrlw (mm1,2);

                        //;                                 +*(curPrevPtr+D1*prevPitch ))

                        //#define SOURCE SRC_PREV
                        //#define TARGET mm2
                        //#define DIRECTION D1

                        DeinterlaceLoad<D1,SRC_PREV>(eax,esi,ecx,edi,mm2,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm3,mm2);

                        punpcklbw (mm2,mm2);
                        punpckhbw (mm3,mm3);

                        psrlw (mm2,2);
                        psrlw (mm3,2);

                        paddsw (mm0,mm2);
                        paddsw (mm1,mm3);

                        //;mov ebx,00ed90ed9h ; 32768*0.116
                        //;movd mm2,ebx
                        //;punpcklwd mm2,mm2

                        movq (mm2,qword_0ed9h);

                        pmulhw (mm0,mm2);
                        pmulhw (mm1,mm2);

                        psubsw (mm4,mm0);
                        psubsw (mm5,mm1);

                        //;                 -  852*( *(curSrcPtr +ASD2*srcPitch)

                        //#define SOURCE SRC_CUR
                        //#define TARGET mm0
                        //#define DIRECTION ASD2

                        DeinterlaceLoad<ASD2,SRC_CUR>(eax,esi,ecx,edi,mm0,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm1,mm0);

                        punpcklbw (mm0,mm0);
                        punpckhbw (mm1,mm1);

                        psrlw (mm0,2);
                        psrlw (mm1,2);

                        //;                                 +*(curSrcPtr +ASU2*srcPitch))

                        //#define SOURCE SRC_CUR
                        //#define TARGET mm2
                        //#define DIRECTION ASU2

                        DeinterlaceLoad<ASU2,SRC_CUR>(eax,esi,ecx,edi,mm2,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm3,mm2);

                        punpcklbw (mm2,mm2);
                        punpckhbw (mm3,mm3);

                        psrlw (mm2,2);
                        psrlw (mm3,2);

                        paddsw (mm0,mm2);
                        paddsw (mm1,mm3);

                        //;mov ebx,003540354h ; 32768*0.026
                        //;movd mm2,ebx
                        //;punpcklwd mm2,mm2

                        movq (mm2,qword_0354h);

                        pmulhw (mm0,mm2);
                        pmulhw (mm1,mm2);

                        psubsw (mm4,mm0);
                        psubsw (mm5,mm1);

                        //;                 + 1016*( *(curPrevPtr+U2*prevPitch )

                        //#define SOURCE SRC_PREV
                        //#define TARGET mm0
                        //#define DIRECTION U2

                        DeinterlaceLoad<U2,SRC_PREV>(eax,esi,ecx,edi,mm0,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm1,mm0);

                        punpcklbw (mm0,mm0);
                        punpckhbw (mm1,mm1);

                        psrlw (mm0,2);
                        psrlw (mm1,2);

                        //;                                 +*(curPrevPtr+D2*prevPitch ));

                        //#define SOURCE SRC_PREV
                        //#define TARGET mm2
                        //#define DIRECTION D2

                        DeinterlaceLoad<D2,SRC_PREV>(eax,esi,ecx,edi,mm2,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm3,mm2);

                        punpcklbw (mm2,mm2);
                        punpckhbw (mm3,mm3);

                        psrlw (mm2,2);
                        psrlw (mm3,2);

                        paddsw (mm0,mm2);
                        paddsw (mm1,mm3);

                        //;mov ebx,003f803f8h ; 32768*0.031
                        //;movd mm2,ebx
                        //;punpcklwd mm2,mm2

                        movq (mm2,qword_03f8h);

                        pmulhw (mm0,mm2);
                        pmulhw (mm1,mm2);

                        paddsw (mm4,mm0);
                        paddsw (mm5,mm1);

                        //; Word XXYY -> Byte XX via XXYY-00XX
                        //; (result closer to C++ implementation)

                        movq (mm0,mm4);
                        movq (mm1,mm5);

                        psraw (mm0,8);
                        psraw (mm1,8);

                        psubsw (mm4,mm0);
                        psubsw (mm5,mm1);

                        if (EAX_INVERTED)
                                eax=-eax;

                        // intVal=lIntVal/16384;

                        psraw (mm4,5);
                        psraw (mm5,5);

                }else{// ; defined TWOWAY

                        //; lIntVal=+17236*( *(curSrcPtr +ASD1*srcPitch)

                        //#define SOURCE SRC_CUR
                        //#define TARGET mm0
                        //#define DIRECTION ASD1
                        bool EAX_INVERTED=false;
                        DeinterlaceLoad<ASD1,SRC_CUR>(eax,esi,ecx,edi,mm0,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm1,mm0);

                        punpcklbw (mm0,mm0);
                        punpckhbw (mm1,mm1);

                        psrlw (mm0,3);
                        psrlw (mm1,3);

                        //;                         +*(curSrcPtr +ASU1*srcPitch))

                        //#define SOURCE SRC_CUR
                        //#define TARGET mm4
                        //#define DIRECTION ASU1

                        DeinterlaceLoad<ASU1,SRC_CUR>(eax,esi,ecx,edi,mm4,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm5,mm4);

                        punpcklbw (mm4,mm4);
                        punpckhbw (mm5,mm5);

                        psrlw (mm4,3);
                        psrlw (mm5,3);

                        paddsw (mm4,mm0);
                        paddsw (mm5,mm1);

                        //;mov ebx,043544354h
                        //;movd mm2,ebx
                        //;punpcklwd mm2,mm2

                        movq (mm2,qword_4354h);

                        pmulhw (mm4,mm2);
                        pmulhw (mm5,mm2);

                        //;                 + 5570*( *(curNextPtr              )

                        //#define SOURCE SRC_NEXT
                        //#define TARGET mm0
                        //#define DIRECTION 0

                        DeinterlaceLoad<0,SRC_NEXT>(eax,esi,ecx,edi,mm0,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm1,mm0);

                        punpcklbw (mm0,mm0);
                        punpckhbw (mm1,mm1);

                        psrlw (mm0,3);
                        psrlw (mm1,3);

                        //;                                 +*(curPrevPtr              ))

                        //#define SOURCE SRC_PREV
                        //#define TARGET mm2
                        //#define DIRECTION 0

                        DeinterlaceLoad<0,SRC_PREV>(eax,esi,ecx,edi,mm2,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm3,mm2);

                        punpcklbw (mm2,mm2);
                        punpckhbw (mm3,mm3);

                        psrlw (mm2,3);
                        psrlw (mm3,3);

                        paddsw (mm0,mm2);
                        paddsw (mm1,mm3);

                        //;mov ebx,015c215c2h
                        //;movd mm2,ebx
                        //;punpcklwd mm2,mm2

                        movq (mm2,qword_15c2h);

                        pmulhw (mm0,mm2);
                        pmulhw (mm1,mm2);

                        paddsw (mm4,mm0);
                        paddsw (mm5,mm1);

                        //;                 - 3801*( *(curNextPtr+U1*nextPitch )

                        //#define SOURCE SRC_NEXT
                        //#define TARGET mm0
                        //#define DIRECTION U1

                        DeinterlaceLoad<U1,SRC_NEXT>(eax,esi,ecx,edi,mm0,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm1,mm0);

                        punpcklbw (mm0,mm0);
                        punpckhbw (mm1,mm1);

                        psrlw (mm0,3);
                        psrlw (mm1,3);

                        //;                                 +*(curNextPtr+D1*nextPitch )

                        //#define SOURCE SRC_NEXT
                        //#define TARGET mm2
                        //#define DIRECTION D1

                        DeinterlaceLoad<D1,SRC_NEXT>(eax,esi,ecx,edi,mm2,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm3,mm2);

                        punpcklbw (mm2,mm2);
                        punpckhbw (mm3,mm3);

                        psrlw (mm2,3);
                        psrlw (mm3,3);

                        paddsw (mm0,mm2);
                        paddsw (mm1,mm3);

                        //;                                 +*(curPrevPtr+D1*prevPitch )

                        //#define SOURCE SRC_PREV
                        //#define TARGET mm2
                        //#define DIRECTION D1

                        DeinterlaceLoad<D1,SRC_PREV>(eax,esi,ecx,edi,mm2,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm3,mm2);

                        punpcklbw (mm2,mm2);
                        punpckhbw (mm3,mm3);

                        psrlw (mm2,3);
                        psrlw (mm3,3);

                        paddsw (mm0,mm2);
                        paddsw (mm1,mm3);

                        //;                                 +*(curPrevPtr+U1*prevPitch ))

                        //#define SOURCE SRC_PREV
                        //#define TARGET mm2
                        //#define DIRECTION U1

                        DeinterlaceLoad<U1,SRC_PREV>(eax,esi,ecx,edi,mm2,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm3,mm2);

                        punpcklbw (mm2,mm2);
                        punpckhbw (mm3,mm3);

                        psrlw (mm2,3);
                        psrlw (mm3,3);

                        paddsw (mm0,mm2);
                        paddsw (mm1,mm3);

                        //;mov ebx,00ed90ed9h
                        //;movd mm2,ebx
                        //;punpcklwd mm2,mm2

                        movq (mm2,qword_0ed9h);

                        pmulhw (mm0,mm2);
                        pmulhw (mm1,mm2);

                        psubsw (mm4,mm0);
                        psubsw (mm5,mm1);

                        //;                 -  852*( *(curSrcPtr +ASU2*srcPitch)

                        //#define SOURCE SRC_CUR
                        //#define TARGET mm0
                        //#define DIRECTION ASU2

                        DeinterlaceLoad<ASU2,SRC_CUR>(eax,esi,ecx,edi,mm0,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm1,mm0);

                        punpcklbw (mm0,mm0);
                        punpckhbw (mm1,mm1);

                        psrlw (mm0,3);
                        psrlw (mm1,3);

                        //;                                 +*(curSrcPtr +ASD2*srcPitch))

                        //#define SOURCE SRC_CUR
                        //#define TARGET mm2
                        //#define DIRECTION ASD2

                        DeinterlaceLoad<ASD2,SRC_CUR>(eax,esi,ecx,edi,mm2,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm3,mm2);

                        punpcklbw (mm2,mm2);
                        punpckhbw (mm3,mm3);

                        psrlw (mm2,3);
                        psrlw (mm3,3);

                        paddsw (mm0,mm2);
                        paddsw (mm1,mm3);

                        //;mov ebx,003540354h
                        //;movd mm2,ebx
                        //;punpcklwd mm2,mm2

                        movq (mm2,qword_0354h);

                        pmulhw (mm0,mm2);
                        pmulhw (mm1,mm2);

                        psubsw (mm4,mm0);
                        psubsw (mm5,mm1);

                        //;                 + 1016*( *(curNextPtr+D2*nextPitch )

                        //#define SOURCE SRC_NEXT
                        //#define TARGET mm0
                        //#define DIRECTION D2

                        DeinterlaceLoad<D2,SRC_NEXT>(eax,esi,ecx,edi,mm0,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm1,mm0);

                        punpcklbw (mm0,mm0);
                        punpckhbw (mm1,mm1);

                        psrlw (mm0,3);
                        psrlw (mm1,3);

                        //;                                 +*(curNextPtr+U2*nextPitch )

                        //#define SOURCE SRC_NEXT
                        //#define TARGET mm2
                        //#define DIRECTION U2

                        DeinterlaceLoad<U2,SRC_NEXT>(eax,esi,ecx,edi,mm2,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm3,mm2);

                        punpcklbw (mm2,mm2);
                        punpckhbw (mm3,mm3);

                        psrlw (mm2,3);
                        psrlw (mm3,3);

                        paddsw (mm0,mm2);
                        paddsw (mm1,mm3);

                        //;                                 +*(curPrevPtr+U2*prevPitch )

                        //#define SOURCE SRC_PREV
                        //#define TARGET mm2
                        //#define DIRECTION U2

                        DeinterlaceLoad<U2,SRC_PREV>(eax,esi,ecx,edi,mm2,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm3,mm2);

                        punpcklbw (mm2,mm2);
                        punpckhbw (mm3,mm3);

                        psrlw (mm2,3);
                        psrlw (mm3,3);

                        paddsw (mm0,mm2);
                        paddsw (mm1,mm3);

                        //;                                 +*(curPrevPtr+D2*prevPitch ));

                        //#define SOURCE SRC_PREV
                        //#define TARGET mm2
                        //#define DIRECTION D2

                        DeinterlaceLoad<D2,SRC_PREV>(eax,esi,ecx,edi,mm2,EAX_INVERTED);// #include "include\DeinterlaceLoad.cpp"

                        movq (mm3,mm2);

                        punpcklbw (mm2,mm2);
                        punpckhbw (mm3,mm3);

                        psrlw (mm2,3);
                        psrlw (mm3,3);

                        paddsw (mm0,mm2);
                        paddsw (mm1,mm3);

                        //;mov ebx,003f803f8h
                        //;movd mm2,ebx
                        //;punpcklwd mm2,mm2

                        movq (mm2,qword_03f8h);

                        pmulhw (mm0,mm2);
                        pmulhw (mm1,mm2);

                        paddsw (mm4,mm0);
                        paddsw (mm5,mm1);

                        if (EAX_INVERTED)
                                eax=-eax;

                        //; intVal=lIntVal/16384;

                        movq (mm0,mm4);
                        movq (mm1,mm5);

                        psraw (mm0,8);
                        psraw (mm1,8);

                        psubsw (mm4,mm0);
                        psubsw (mm5,mm1);

                        psraw (mm4,4);
                        psraw (mm5,4);

                }

        }

        //; convert words to bytes

        packuswb (mm4,mm5);

        //; clamp value ranges

        if (COLORSPACE == YV12){

                if (!HALFSIZE){
                        //;mov ebx,00f0f0f0fh

                        movq (mm0,qword_0fh);
                }else{ // defined HALFSIZE
                        //;mov ebx,014141414h

                        movq (mm0,qword_14h);
                }

                //;movd mm0,ebx

                //;mov ebx,010101010h
                //;movd mm1,ebx

                movq (mm1,qword_10h);

                punpcklbw (mm0,mm0);
                punpcklbw (mm1,mm1);

                paddusb (mm4,mm0);
                psubusb (mm4,mm0);
                psubusb (mm4,mm1);
                paddusb (mm4,mm1);

        }else if (COLORSPACE == YUY2){

                //;mov ebx,0140f140fh
                //;movd mm0,ebx

                movq (mm0,qword_140fh);

                //;mov ebx,010101010h
                //;movd mm1,ebx

                movq (mm1,qword_10h);

                punpckldq (mm0,mm0);
                punpckldq (mm1,mm1);

                paddusb (mm4,mm0);
                psubusb (mm4,mm0);
                psubusb (mm4,mm1);
                paddusb (mm4,mm1);

        }

        // apply motion mask

        pand (mm4,mm7);
        pcmpeqb (mm6,mm7);

        movq (mm0,edi);
        pand (mm0,mm6);
        por (mm0,mm4);

        movq (edx,mm0);

        //; Update pointers

        esi+=8;
        ecx+=8;
        edi+=8;
        edx+=8;

        bytesLeft-=8;
        if (bytesLeft!=0)
         goto ColLoop;

        goto RowEnd;


CopyThrough:

        movq (mm0,edi);

        esi+=8;
        ecx+=8;
        edi+=8;

        movq (edx,mm0);

        edx+=8;

        bytesLeft-=8;
        if (bytesLeft!=0)
         goto ColLoop;

RowEnd:;
  }
 template<bool SHARP,bool TWOWAY,int COLORSPACE,int U2,int U1,int D1,int D2,int SU2,int SU1,int SD1,int SD2> static __forceinline void DeinterlaceLine_1(int rowSize,const unsigned char* &prevPtr,const unsigned char* &srcPtr,const unsigned char* &nextPtr,const unsigned char* &maskPtr,unsigned char* &dstPtr,stride_t maskPitch,stride_t dstPitch,stride_t prevPitch,stride_t nextPitch,stride_t srcPitch,bool halfsize,int order)
  {
        int bytesLeft=rowSize;

        const unsigned char *curPrevPtr=prevPtr;
        const unsigned char *curSrcPtr=srcPtr;
        const unsigned char *curNextPtr=nextPtr;
        const unsigned char *curMaskPtr=maskPtr;
        unsigned char *curDstPtr=dstPtr;

        if (!halfsize)
        {
                //#undef  HALFSIZE

                if (order == 0)
                {
                        #undef TOPFIRST

                        //#define ASU2 (SU2-1)
                        //#define ASU1 (SU1-1)
                        //#define ASD1 (SD1-1)
                        //#define ASD2 (SD2-1)
                        DeinterlaceLine_2<false,SHARP,TWOWAY,COLORSPACE, U2,U1,D1,D2, SU2-1,SU1-1,SD1-1,SD2-1>(bytesLeft,curPrevPtr,curSrcPtr,curNextPtr,curDstPtr,curMaskPtr,srcPitch);
                }
                else
                {
                        #define TOPFIRST

                        //#define ASU2 SU2
                        //#define ASU1 SU1
                        //#define ASD1 SD1
                        //#define ASD2 SD2

                        DeinterlaceLine_2<false,SHARP,TWOWAY,COLORSPACE, U2,U1,D1,D2, SU2,SU1,SD1,SD2>(bytesLeft,curPrevPtr,curSrcPtr,curNextPtr,curDstPtr,curMaskPtr,srcPitch);
                }
        }

        if (COLORSPACE == YV12 && halfsize){

                //else
                //{
                        //#define HALFSIZE

                        if (order == 0)
                        {
                                #undef TOPFIRST

                                //#define ASU2 (SU2-1)
                                //#define ASU1 (SU1-1)
                                //#define ASD1 (SD1-1)
                                //#define ASD2 (SD2-1)

                                DeinterlaceLine_2<true,SHARP,TWOWAY,COLORSPACE, U2,U1,D1,D2, SU2-1,SU1-1,SD1-1,SD2-1>(bytesLeft,curPrevPtr,curSrcPtr,curNextPtr,curDstPtr,curMaskPtr,srcPitch);
                        }
                        else
                        {
                                #define TOPFIRST

                                //#define ASU2 SU2
                                //#define ASU1 SU1
                                //#define ASD1 SD1
                                //#define ASD2 SD2

                                DeinterlaceLine_2<true,SHARP,TWOWAY,COLORSPACE, U2,U1,D1,D2, SU2,SU1,SD1,SD2>(bytesLeft,curPrevPtr,curSrcPtr,curNextPtr,curDstPtr,curMaskPtr,srcPitch);
                        }
                //}

        }

        maskPtr+=maskPitch; dstPtr+=dstPitch; prevPtr+=prevPitch; nextPtr+=nextPitch;

        if (order == 1) srcPtr+=srcPitch;
  }
 template<int SHARP,int TWOWAY,int PITCH_MOD,int COLORSPACE> void Deinterlace_0(int plane,int n,int order,KernelDeintMask* mask,unsigned char *dst[3],stride_t dstStride[3])
  {
        bool halfsize=(plane != PLANAR_Y);

        PVideoFrame prev=GetField(n-1);
        PVideoFrame src =GetField(n  );
        PVideoFrame next=GetField(n+1);

        stride_t prevPitch=prev->GetPitch(plane);
        stride_t srcPitch=src->GetPitch(plane);
        stride_t nextPitch=next->GetPitch(plane);

        stride_t maskPitch=mask->bytePitch;

        stride_t dstPitch=dstStride[plane];

        int rowSize=src->GetRowSize(plane);

        bool oddPitch=(srcPitch % PITCH_MOD) != 0;

        const unsigned char* prevPtr=prev->GetReadPtr(plane);
        const unsigned char* srcPtr=src->GetReadPtr(plane);
        const unsigned char* nextPtr=next->GetReadPtr(plane);

        const unsigned char* maskPtr=mask->buffer;

        unsigned char* dstPtr=dst[plane];

        //int bytesLeft;

        //const unsigned char* curPrevPtr;
        //const unsigned char* curSrcPtr;
        //const unsigned char* curNextPtr;

        //unsigned char* curMaskPtr;

        //unsigned char* curDstPtr;

        if (order == 1)
                COPY_LINE(srcPtr,srcPitch,dstPtr,dstPitch,rowSize,order);

        //#define U2 2
        //#define U1 1
        //#define D1 1
        //#define D2 2

        //#define SU2 2
        //#define SU1 1
        //#define SD1 1
        //#define SD2 2

        DeinterlaceLine_1<SHARP,TWOWAY,COLORSPACE, 2,1,1,2, 2,1,1,2>(rowSize,prevPtr,srcPtr,nextPtr,maskPtr,dstPtr,maskPitch,dstPitch,prevPitch,nextPitch,srcPitch,halfsize,order);
        COPY_LINE(srcPtr,srcPitch,dstPtr,dstPitch,rowSize,order);

        //#define U2 1
        //#define U1 0
        //#define D1 1
        //#define D2 2

        //#define SU2 1
        //#define SU1 0
        //#define SD1 1
        //#define SD2 2

        DeinterlaceLine_1<SHARP,TWOWAY,COLORSPACE, 1,0,1,2, 1,0,1,2>(rowSize,prevPtr,srcPtr,nextPtr,maskPtr,dstPtr,maskPitch,dstPitch,prevPitch,nextPitch,srcPitch,halfsize,order);

        COPY_LINE(srcPtr,srcPitch,dstPtr,dstPitch,rowSize,order);

        //#define U2 0
        //#define U1 0
        //#define D1 1
        //#define D2 2

        //#define SU2 -1
        //#define SU1 0
        //#define SD1 1
        //#define SD2 2

        DeinterlaceLine_1<SHARP,TWOWAY,COLORSPACE, 0,0,1,2, -1,0,1,2>(rowSize,prevPtr,srcPtr,nextPtr,maskPtr,dstPtr,maskPitch,dstPitch,prevPitch,nextPitch,srcPitch,halfsize,order);

        COPY_LINE(srcPtr,srcPitch,dstPtr,dstPitch,rowSize,order);

        //#define U2 -1
        //#define U1 -1
        //#define D1 1
        //#define D2 2

        //#define SU2 -1
        //#define SU1 0
        //#define SD1 1
        //#define SD2 2

        DeinterlaceLine_1<SHARP,TWOWAY,COLORSPACE, -1,-1,1,2, -1,0,1,2>(rowSize,prevPtr,srcPtr,nextPtr,maskPtr,dstPtr,maskPitch,dstPitch,prevPitch,nextPitch,srcPitch,halfsize,order);

        unsigned long height=src->GetHeight(plane);
        int rowsLeft=height-8;

        if (oddPitch) rowsLeft--;

        while (rowsLeft > 0)
        {
                COPY_LINE(srcPtr,srcPitch,dstPtr,dstPitch,rowSize,order);

                //#define U2 -2
                //#define U1 -1
                //#define D1 1
                //#define D2 2

                //#define SU2 -1
                //#define SU1 0
                //#define SD1 1
                //#define SD2 2

                DeinterlaceLine_1<SHARP,TWOWAY,COLORSPACE, -2,-1,1,2, -1,0,1,2>(rowSize,prevPtr,srcPtr,nextPtr,maskPtr,dstPtr,maskPitch,dstPitch,prevPitch,nextPitch,srcPitch,halfsize,order);

                rowsLeft--;
        }

        if (oddPitch)
        {
                // copy bottom parts of fields to scratch buffer

                stride_t safePitch=(scratchPitch >= srcPitch ? srcPitch : scratchPitch);

                prevPtr-=2*prevPitch;
                srcPtr-=2*srcPitch;
                nextPtr-=2*nextPitch;

                unsigned char* scratchPtr=scratch;

                for (rowsLeft=7; rowsLeft > 0; rowsLeft--)
                {
                        memcpy(scratchPtr,prevPtr,safePitch);
                        scratchPtr+=scratchPitch;

                        memcpy(scratchPtr,srcPtr,safePitch);
                        scratchPtr+=scratchPitch;

                        memcpy(scratchPtr,nextPtr,safePitch);
                        scratchPtr+=scratchPitch;

                        prevPtr+=prevPitch; srcPtr+=srcPitch; nextPtr+=nextPitch;
                }

                if (order == 0)
                        memcpy(scratchPtr+scratchPitch,srcPtr,safePitch);

                // change pointers to point to scratch buffer

                prevPtr=scratch+6*scratchPitch;
                srcPtr=prevPtr+scratchPitch;
                nextPtr=srcPtr+scratchPitch;

                prevPitch=srcPitch=nextPitch=scratchPitch*3;

                COPY_LINE(srcPtr,srcPitch,dstPtr,dstPitch,rowSize,order);

                //#define U2 -2
                //#define U1 -1
                //#define D1 1
                //#define D2 2

                //#define SU2 -1
                //#define SU1 0
                //#define SD1 1
                //#define SD2 2

                DeinterlaceLine_1<SHARP,TWOWAY,COLORSPACE, -2,-1,1,2, -1,0,1,2>(rowSize,prevPtr,srcPtr,nextPtr,maskPtr,dstPtr,maskPitch,dstPitch,prevPitch,nextPitch,srcPitch,halfsize,order);
        }


        COPY_LINE(srcPtr,srcPitch,dstPtr,dstPitch,rowSize,order);

        //#define U2 -2
        //#define U1 -1
        //#define D1 1
        //#define D2 1

        //#define SU2 -1
        //#define SU1 0
        //#define SD1 1
        //#define SD2 2

        DeinterlaceLine_1<SHARP,TWOWAY,COLORSPACE, -2,-1,1,1, -1,0,1,2>(rowSize,prevPtr,srcPtr,nextPtr,maskPtr,dstPtr,maskPitch,dstPitch,prevPitch,nextPitch,srcPitch,halfsize,order);

        COPY_LINE(srcPtr,srcPitch,dstPtr,dstPitch,rowSize,order);

        //#define U2 -2
        //#define U1 -1
        //#define D1 1
        //#define D2 1

        //#define SU2 -1
        //#define SU1 0
        //#define SD1 1
        //#define SD2 1

        DeinterlaceLine_1<SHARP,TWOWAY,COLORSPACE, -2,-1,1,1, -1,0,1,1>(rowSize,prevPtr,srcPtr,nextPtr,maskPtr,dstPtr,maskPitch,dstPitch,prevPitch,nextPitch,srcPitch,halfsize,order);

        COPY_LINE(srcPtr,srcPitch,dstPtr,dstPitch,rowSize,order);

        //#define U2 -2
        //#define U1 -1
        //#define D1 0
        //#define D2 -1

        //#define SU2 -1
        //#define SU1 0
        //#define SD1 1
        //#define SD2 0

        DeinterlaceLine_1<SHARP,TWOWAY,COLORSPACE, -2,-1,0,-1, -1,0,1,0>(rowSize,prevPtr,srcPtr,nextPtr,maskPtr,dstPtr,maskPitch,dstPitch,prevPitch,nextPitch,srcPitch,halfsize,order);

        COPY_LINE(srcPtr,srcPitch,dstPtr,dstPitch,rowSize,order);

        //#define U2 -2
        //#define U1 -1
        //#define D1 -1
        //#define D2 -2

        //#define SU2 -1
        //#define SU1 0
        //#define SD1 0
        //#define SD2 -1

        DeinterlaceLine_1<SHARP,TWOWAY,COLORSPACE, -2,-1,-1,-2, -1,0,0,-1>(rowSize,prevPtr,srcPtr,nextPtr,maskPtr,dstPtr,maskPitch,dstPitch,prevPitch,nextPitch,srcPitch,halfsize,order);

        if (order == 0)
          COPY_LINE(srcPtr,srcPitch,dstPtr,dstPitch,rowSize,order);
  }

 template<bool YV12> inline void Deinterlace(unsigned char *dst[3],stride_t dstStride[3],
	                                     int order,
	                                     int n,
	                                     KernelDeintMask* fullsizeMask,
	                                     KernelDeintMask* halfsizeMask)
  {
   if (YV12)
    {
     (this->*Deinterlace_0fc)(PLANAR_Y,n,order,fullsizeMask,dst,dstStride);
     (this->*Deinterlace_0fc)(PLANAR_U,n,order,halfsizeMask,dst,dstStride);
     (this->*Deinterlace_0fc)(PLANAR_V,n,order,halfsizeMask,dst,dstStride);
    }
   else
    (this->*Deinterlace_0fc)(PLANAR_Y,n,order,fullsizeMask,dst,dstStride);
  }
 template<bool YV12> void KernelDeint(unsigned char *dst[3],stride_t dstStride[3],int n)
  {
   int usedOrder=order^(n&1);
   if (threshold>0)
    BuildMotionMask<YV12>(fullsizeMask,halfsizeMask,usedOrder,linked,n);
   else if (!masksFilled)
    {
     fullsizeMask->set(255);
     if (halfsizeMask) halfsizeMask->set(255);
     masksFilled=true;
    }
   if (map) //if map, use getCurNext with copy
    ShowMotionMask<YV12>(dst,dstStride,usedOrder,fullsizeMask);
   else
    Deinterlace<YV12>(dst,dstStride,usedOrder,n,fullsizeMask,halfsizeMask);
  }

 template<bool YV12> void ShowMotionMask(unsigned char *dst[3],stride_t dstStride[3],int order,const KernelDeintMask *fullsizeMask)
  {
   stride_t maskPitch=fullsizeMask->bytePitch;
   unsigned char* maskPtr=fullsizeMask->buffer;

   stride_t dstPitch=dstStride[0];
   unsigned char* dstPtr=dst[0]+order*dstPitch;

   int rowSize=width*(YV12?1:2);

   for (int rowsLeft=height/2; rowsLeft > 0; rowsLeft--)
    {
     const typename TshowMotionMaskTraits<YV12>::Tpixel* curMaskPtr=(const typename TshowMotionMaskTraits<YV12>::Tpixel*)maskPtr;
     typename TshowMotionMaskTraits<YV12>::Tpixel* curDstPtr=(typename TshowMotionMaskTraits<YV12>::Tpixel*)dstPtr;

     for (int colsLeft=rowSize/sizeof(typename TshowMotionMaskTraits<YV12>::Tpixel); colsLeft > 0; colsLeft--)
      {
       typename TshowMotionMaskTraits<YV12>::Tpixel curMask=*curMaskPtr&TshowMotionMaskTraits<YV12>::mask;
       *curDstPtr|=curMask; curMask&=TshowMotionMaskTraits<YV12>::mask2; *curDstPtr^=curMask;
       curMaskPtr++; curDstPtr++;
      }
     maskPtr+=maskPitch;
     dstPtr+=2*dstPitch;
    }
  }
};

extern "C" IkernelDeint* createI(bool IisYV12,unsigned int width,unsigned int height,unsigned int rowsize,int Iorder,int Ithreshold,bool Isharp,bool Itwoway,bool Ilinked,bool Imap,bool Ibob,int cpuflags)
{
 return new TkernelDeint(IisYV12,width,height,rowsize,Iorder,Ithreshold,Isharp,Itwoway,Ilinked,Imap,Ibob,cpuflags);
}

extern "C" void __stdcall getVersion(char *ver,const char* *license)
{
 strcpy(ver,"1.5.4, build date "__DATE__" "__TIME__" ("COMPILER COMPILER_X64")");
 *license="Based on the original KernelDeint plugin (c) 2003 Donald A. Graft\nMMX optimizations + new motion mask code (c) 2004 Kurt B. Pruenner";
}

//======================================== mem_align ========================================
#ifdef __GNUC__
#define _aligned_malloc __mingw_aligned_malloc
#define _aligned_realloc __mingw_aligned_realloc
#define _aligned_free __mingw_aligned_free
#endif

void* aligned_malloc(size_t size, size_t /*alignment*/)
{
 return _aligned_malloc(size,16);
}

void* aligned_realloc(void *ptr,size_t size,size_t alignment)
{
 if (!ptr)
  return aligned_malloc(size,alignment);
 else
  if (size==0)
   {
    aligned_free(ptr);
    return NULL;
   }
  else
   return _aligned_realloc(ptr,size,16);
}

void aligned_free(void *mem_ptr)
{
 _aligned_free(mem_ptr);
}

void* aligned_calloc(size_t size1, size_t size2, size_t alignment)
{
 size_t size=size1*size2;
 void *ret=aligned_malloc(size,alignment);
 if (ret) memset(ret,0,size);
 return ret;
}
