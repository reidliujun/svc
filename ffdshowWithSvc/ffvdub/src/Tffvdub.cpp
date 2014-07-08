/*
 * Copyright (c) 2002-2004 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "stdafx.h"
#include "Tffvdub.h"
#include <initguid.h>
#define FF_CSP_ONLY
#include "../../src/inttypes.h"
#include "../../src/imgFilters/IffProc.h"
#include "../../src/imgFilters/ffImgfmt.h"
#include "../../src/IffdshowBase.h"
#include "../../src/IffdshowDecVideo.h"
#include "../../src/ffdshow_constants.h"
#include "resource.h"
#include "base64.h"
#include "bzip2/bzlib.h"

void Tffvdub::initProc(void)
{
 page=NULL;pageSite=NULL;
 proc=NULL;
 srcDx=srcDy=dstDx=dstDy=dstStride=0;
 buf=NULL;
 if (CoCreateInstance(CLSID_TFFPROC,NULL,CLSCTX_INPROC_SERVER,IID_IffProc8,(void**)&proc)==S_OK)
  {
   ok=true;
   proc->loadPreset("ffvdub");
  }
 else
  ok=false;
}

void Tffvdub::shiftWindow(HWND h,int dx,int dy)
{
 RECT r;
 GetWindowRect(h,&r);
 RECT pr;
 GetWindowRect(GetParent(h),&pr);
 OffsetRect(&r,-pr.left,-pr.top);
 OffsetRect(&r,dx,dy);
 SetWindowPos(h,NULL,r.left,r.top,0,0,SWP_NOSIZE|SWP_NOZORDER);
}

Tffvdub::Tffvdub(void)
{
 initProc();
}
Tffvdub::Tffvdub(const Tffvdub* Iffvdub)
{
 initProc();
 unsigned int buflen;
 Iffvdub->proc->saveBytestreamConfig(&buflen,0);
 void *buf=_alloca(buflen);
 Iffvdub->proc->saveBytestreamConfig(buf,buflen);
 proc->loadBytestreamConfig(buf,buflen);
}
Tffvdub::~Tffvdub()
{
 if (proc)
  {
   proc->end();
   proc->Release();
  }
}

int Tffvdub::config(FilterActivation *fa,HWND m_hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
 switch (msg)
  {
   case WM_INITDIALOG:
    {
     fa->ifp->InitButton(0,GetDlgItem(m_hwnd,IDC_BT_PREVIEW));
     CoCreateInstance(CLSID_TFFDSHOWPAGE,NULL,CLSCTX_INPROC_SERVER,IID_IPropertyPage,(void**)&page);
     if (!page) return FALSE;
     HRESULT res;
     pageSite=new TpropertyPageSite(NULL,&res);
     pageSite->setFilterActivation(fa);
     page->SetPageSite(pageSite);
     proc->putParam(IDFF_filterMode,IDFF_FILTERMODE_PROC);
     proc->setBasePageSite(page);
     //IffdshowBase *deciB;proc->getIffDecoder(&deciB);
     //deciB->putParam(IDFF_filterMode,IDFF_FILTERMODE_PROC);
     //page->SetObjects(1,(IUnknown**)&deciB);
     PROPPAGEINFO ppi;memset(&ppi,0,sizeof(ppi));
     ppi.cb=sizeof(ppi);
     page->GetPageInfo(&ppi);
     if (ppi.pszTitle) CoTaskMemFree(ppi.pszTitle);
     if (ppi.pszDocString) CoTaskMemFree(ppi.pszDocString);
     if (ppi.pszHelpFile) CoTaskMemFree(ppi.pszHelpFile);
     RECT placeRect;GetWindowRect(GetDlgItem(m_hwnd,IDC_PLACE_PAGE),&placeRect);
     int dx=ppi.size.cx-(placeRect.right-placeRect.left),dy=ppi.size.cy-(placeRect.bottom-placeRect.top);
     RECT wr,wr0;GetWindowRect(m_hwnd,&wr);wr0=wr;
     RECT cr;GetClientRect(m_hwnd,&cr);
     OffsetRect(&cr,(wr.right-wr.left)-cr.right,(wr.bottom-wr.top)-cr.bottom);
     SetWindowPos(m_hwnd,NULL,0,0,(wr.right-wr.left)+dx,(wr.bottom-wr.top)+dy,SWP_NOMOVE|SWP_NOZORDER);
     OffsetRect(&placeRect,-wr.left,-wr.top);
     OffsetRect(&placeRect,-cr.left,-cr.top);
     placeRect.right=placeRect.left+ppi.size.cx;placeRect.bottom=placeRect.top+ppi.size.cy;
     page->Activate(m_hwnd,&placeRect,FALSE);
     page->Show(SW_SHOW);
     int difx=(wr.right-wr.left)+dx-(wr0.right-wr0.left)-cr.left;
     int dify=(wr.bottom-wr.top)+dy-(wr0.bottom-wr0.top)-cr.top;
     shiftWindow(GetDlgItem(m_hwnd,IDC_BT_PREVIEW),difx,dify);
     shiftWindow(GetDlgItem(m_hwnd,IDOK),difx,dify);
     shiftWindow(GetDlgItem(m_hwnd,IDCANCEL),difx,dify);
     MoveWindow(GetDlgItem(m_hwnd,IDC_LINE),2,placeRect.bottom,placeRect.right-4,2,TRUE);
     return TRUE;
    }
   case WM_DESTROY:
    {
     if (page) page->Release();page=NULL;
     return FALSE;
    }
   case WM_COMMAND:
    {
     switch(LOWORD(wParam))
      {
       case IDOK:
        page->Apply();
        page->Deactivate();
        page->SetObjects(0,NULL);
        EndDialog(m_hwnd,0);
        page->Release();page=NULL;
        return TRUE;
       case IDCANCEL:
        page->Deactivate();
        page->SetObjects(0,NULL);
        EndDialog(m_hwnd,1);
        page->Release();page=NULL;
        return FALSE;
       case IDC_BT_CFG:
        proc->config(m_hwnd);
        fa->ifp->RedoFrame(0);
        return TRUE;
       case IDC_BT_PREVIEW:
        fa->ifp->Toggle(0,GetDlgItem(m_hwnd,IDC_BT_PREVIEW));
        return TRUE;
      }
     break;
    }
  }
 return FALSE;
}

int Tffvdub::run(const FilterActivation *fa)
{
 const unsigned char *src[4]={(const unsigned char*)fa->src.data,NULL,NULL,NULL};
 stride_t srcStride[4]={fa->src.pitch,0,0,0};
 unsigned char *dst[4]={(unsigned char*)fa->dst.data,NULL,NULL,NULL};
 stride_t dstStride[4]={fa->dst.pitch,0,0,0};
 proc->processTime(fa->pfsi->lCurrentSourceFrame,10000LL*fa->pfsi->lSourceFrameMS,10*(1000LL*fa->pfsi->lSourceFrameMS+fa->pfsi->lMicrosecsPerSrcFrame),FF_CSP_RGB32|FF_CSP_FLAGS_VFLIP,src,srcStride,true,FF_CSP_RGB32|FF_CSP_FLAGS_VFLIP,dst,dstStride);
 return 0;
}
bool Tffvdub::fss(char *dstbuf,int dstbuflen)
{
 proc->saveBytestreamConfig(&buflen,0);
 buf=(unsigned char*)realloc(buf,buflen);
 proc->saveBytestreamConfig(buf,buflen);
 unsigned int buflenC=buflen*2;
 unsigned char *bufC=(unsigned char*)_alloca(buflenC);
 BZ2_bzBuffToBuffCompress((char*)bufC,&buflenC,(char*)buf,buflen,1,0,30);
 int base64len=buflenC+((buflenC+2)/3)*4 + 1;
 char *bufS=(char*)_alloca(base64len);
 membase64(bufS,(const char*)bufC,buflenC);
 if (size_t(dstbuflen)>=strlen(bufS)+3)
  {
   _snprintf(dstbuf,dstbuflen,"Config2(%i,\"%s\")",buflenC,bufS);
   return true;
  }
 else
  return false;
}

long Tffvdub::param(FilterActivation *fa)
{
 unsigned int dstDx1,dstDy1;
 proc->getNewSize(fa->src.w,fa->src.h,&dstDx1,&dstDy1);
 if (srcDx!=fa->src.w || srcDy!=fa->src.h || dstDx1!=fa->dst.w || dstDy1!=fa->dst.h)
  {
   outchanged=true;
   proc->end();
   srcDx=fa->src.w;srcDy=fa->src.h;
   proc->begin(srcDx,srcDy,25,1);
   dstDx=dstDx1;dstDy=dstDy1;
   dstStride=(dstDx*4 + 7) & -8;
  }
 fa->dst.w=dstDx;fa->dst.h=dstDy;fa->dst.pitch=dstStride;
 return 0;
}
void Tffvdub::scriptConfig(IScriptInterpreter *isi, CScriptValue *argv, int argc)
{
 unsigned int buflenC=argv[0].asInt();
 const char *inputS=*argv[1].asString();
 char *bufC=(char*)_alloca(buflenC);
 memunbase64(bufC,inputS,buflenC);
 unsigned int buflen=buflenC*10;
 buf=(unsigned char*)realloc(buf,buflen);
 BZ2_bzBuffToBuffDecompress((char*)buf,&buflen,bufC,buflenC,1,0);
 proc->loadBytestreamConfig(buf,buflen);
}
