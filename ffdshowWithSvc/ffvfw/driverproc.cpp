/*
 * Copyright (c) 2002-2004 Milan Cutka
 * based on driverproc.c from Xvid VFW
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ole2.h>
#include <vfw.h>
#include <commctrl.h>
#include <stdio.h>
#include "ffdshow_mediaguids.h"
#include "IffdshowEnc.h"
#include <initguid.h>
#include "Iffvfw.h"
#include "ffdebug.h"
#include "uClibc++/algorithm"
#include "uClibc++/map"

#ifndef ICVERSION
#define ICVERSION       0x0104
#endif

std::map<DWORD,HRESULT> CoInitializedThreads;

extern "C" __declspec(dllexport) LRESULT WINAPI DriverProc(DWORD dwDriverId,HDRVR hDriver,UINT uMsg,LPARAM lParam1,LPARAM lParam2)
{
 Iffvfw *ffvfw=(Iffvfw*)dwDriverId;
 switch(uMsg)
  {
   // driver primitives
   case DRV_LOAD:
    {
     InitCommonControls();
     DWORD currentthread= GetCurrentThreadId();
     std::map<DWORD,HRESULT>::iterator i= CoInitializedThreads.find(currentthread);
     if(i==CoInitializedThreads.end()
        || FAILED(CoInitializedThreads[currentthread]))
      CoInitializedThreads[currentthread]= CoInitialize(NULL);
     return DRV_OK;
    }
    /*
     * rev 742-746
     * Bug fix(?): Explorer.exe was crashing in Windows Vista.
     * http://sourceforge.net/tracker/index.php?func=detail&aid=1611309&group_id=53761&atid=471489.

     * Explorer.exe use vfw decoder to get thumbnail.
     * CoInitialize & CoUninitialize were called from different threads,
     * which was the cause of the crashes.
     */
   case DRV_FREE:
    {
     DWORD currentthread= GetCurrentThreadId();
     std::map<DWORD,HRESULT>::iterator i= CoInitializedThreads.find(currentthread);
     if(i==CoInitializedThreads.end()
        || FAILED(CoInitializedThreads[currentthread]))
      return DRV_OK;
     CoUninitialize();
     CoInitializedThreads.erase(currentthread);
     return DRV_OK;
    }
   case DRV_OPEN:
    {
     ICOPEN *icopen=(ICOPEN*)lParam2;
     if (icopen!=NULL && icopen->fccType!=ICTYPE_VIDEO) return DRVCNF_CANCEL;
     if (FAILED(CoCreateInstance(CLSID_FFVFW,NULL,CLSCTX_INPROC_SERVER,IID_Iffvfw,(void**)&ffvfw))) return DRVCNF_CANCEL;
     if (icopen!=NULL) icopen->dwError=ICERR_OK;
     DPRINTF("VFW DRV_OPEN %i %p",hDriver,ffvfw);
     return (LRESULT)ffvfw;
    }
   case DRV_CLOSE:
    DPRINTF("VFW DRV_CLOSE %i %p",hDriver,ffvfw);
    //compress_end/decompress_end don't always get called
    if (ffvfw) ffvfw->Release();
    return DRV_OK;
   case DRV_DISABLE:
   case DRV_ENABLE:
    return DRV_OK;
   case DRV_INSTALL:
   case DRV_REMOVE:
    return DRV_OK;
   case DRV_QUERYCONFIGURE:
   case DRV_CONFIGURE:
    return DRV_CANCEL;
   // info
   case ICM_GETINFO:
    {
     DPRINTF("VFW ICM_GETINFO %i %p",hDriver,ffvfw);
     ICINFO *icinfo=(ICINFO*)lParam1;
     icinfo->fccType=ICTYPE_VIDEO;
     icinfo->fccHandler=FOURCC_FFDS;
     icinfo->dwFlags=VIDCF_FASTTEMPORALC|VIDCF_FASTTEMPORALD|VIDCF_COMPRESSFRAMES;
     icinfo->dwVersion=0;
     icinfo->dwVersionICM=ICVERSION;
     wcscpy(icinfo->szName,FFDSHOWENC_NAME_L);
     wcscpy(icinfo->szDescription,FFDSHOWVFW_DESC_L);
     return lParam2; /* size of struct */
    }
   // state control
   case ICM_ABOUT:
    DPRINTF("VFW ICM_ABOUT %i %p",hDriver,ffvfw);
    if (lParam1!=-1) ffvfw->aboutDlg((HWND)lParam1);
    return ICERR_OK;
   case ICM_CONFIGURE:
    DPRINTF("VFW ICM_CONFIGURE %i %p",hDriver,ffvfw);
    if (lParam1!=-1)
     ffvfw->configDlg((HWND)lParam1);
    return ICERR_OK;
   case ICM_GETSTATE:
    DPRINTF("VFW ICM_GETSTATE %i %p",hDriver,ffvfw);
    if ((void*)lParam1==NULL)
     return ffvfw->saveMem(NULL,0);
    ffvfw->saveMem((void*)lParam1,lParam2);
    return ICERR_OK;
   case ICM_SETSTATE:
    {
     DPRINTF("VFW ICM_SETSTATE %i %p",hDriver,ffvfw);
     if ((void*)lParam1==NULL)
      {
       DPRINTF("VFW ICM_SETSTATE: DEFAULT %i %p",hDriver,ffvfw);
       ffvfw->loadReg();
       return 0;
      }
     return ffvfw->loadMem((void*)lParam1,lParam2);
    }
   // not sure the difference, private/public data?
   case ICM_GET:
   case ICM_SET:
    return ICERR_OK;

   // older-type config
   case ICM_GETDEFAULTQUALITY:
   case ICM_GETQUALITY:
   case ICM_SETQUALITY:
   case ICM_GETBUFFERSWANTED:
   case ICM_GETDEFAULTKEYFRAMERATE:
    return ICERR_UNSUPPORTED;

   // compressor
   case ICM_COMPRESS_QUERY:
    DPRINTF("VFW ICM_COMPRESS_QUERY %i %p",hDriver,ffvfw);
    return ffvfw->coQuery((BITMAPINFO*)lParam1,(BITMAPINFO*)lParam2);
   case ICM_COMPRESS_GET_FORMAT:
    DPRINTF("VFW ICM_COMPRESS_GET_FORMAT %i %p",hDriver,ffvfw);
    return ffvfw->coGetFormat((BITMAPINFO*)lParam1,(BITMAPINFO*)lParam2);
   case ICM_COMPRESS_GET_SIZE:
    DPRINTF("VFW ICM_COMPRESS_GET_SIZE %i %p",hDriver,ffvfw);
    return ffvfw->coGetSize((BITMAPINFO*)lParam1,(BITMAPINFO*)lParam2);
   case ICM_COMPRESS_FRAMES_INFO:
    DPRINTF("VFW ICM_COMPRESS_FRAMES_INFO %i %p",hDriver,ffvfw);
    return ffvfw->coFramesInfo((ICCOMPRESSFRAMES*)lParam1);
   case ICM_COMPRESS_BEGIN:
    DPRINTF("VFW ICM_COMPRESS_BEGIN %i %p",hDriver,ffvfw);
    return ffvfw->coBegin((BITMAPINFO*)lParam1,(BITMAPINFO*)lParam2);
   case ICM_COMPRESS_END:
    DPRINTF("VFW ICM_COMPRESS_END %i %p",hDriver,ffvfw);
    return ffvfw->coEnd();
   case ICM_COMPRESS:
    //DPRINTF("VFW ICM_COMPRESS %i",ffvfw);
    return ffvfw->coRun((ICCOMPRESS*)lParam1);

   // decompressor
   case ICM_DECOMPRESS_QUERY:
    DPRINTF("VFW ICM_DECOMPRESS_QUERY %i %p",hDriver,ffvfw);
    return ffvfw->decQuery((BITMAPINFO*)lParam1,(BITMAPINFO*)lParam2);
   case ICM_DECOMPRESS_GET_FORMAT:
    DPRINTF("VFW ICM_DECOMPRESS_GET_FORMAT %i %p",hDriver,ffvfw);
    return ffvfw->decGetFormat((BITMAPINFO*)lParam1,(BITMAPINFO*)lParam2);
   case ICM_DECOMPRESS_BEGIN:
    DPRINTF("VFW ICM_DECOMPRESS_BEGIN %i %p",hDriver,ffvfw);
    return ffvfw->decBegin((BITMAPINFO*)lParam1,(BITMAPINFO*)lParam2);
   case ICM_DECOMPRESS_END:
    DPRINTF("VFW ICM_DECOMPRESS_END %i %p",hDriver,ffvfw);
    return ffvfw->decEnd();
   case ICM_DECOMPRESS:
    //DPRINTF("VFW ICM_DECOMPRESS %i",ffvfw);
    return ffvfw->decRun((ICDECOMPRESS*)lParam1);

   case ICM_DECOMPRESS_GET_PALETTE:
   case ICM_DECOMPRESS_SET_PALETTE:
   case ICM_DECOMPRESSEX_QUERY:
   case ICM_DECOMPRESSEX_BEGIN:
   case ICM_DECOMPRESSEX_END:
   case ICM_DECOMPRESSEX:
    return ICERR_UNSUPPORTED;

   default:
    if (uMsg<DRV_USER)
     return DefDriverProc(dwDriverId,hDriver,uMsg,lParam1,lParam2);
    else
     return ICERR_UNSUPPORTED;
  }
}

//thanks to suxen_drol for the idea of simplifying this function
extern "C" void CALLBACK configureVFW(HWND hwnd,HINSTANCE hinst,LPTSTR lpCmdLine,int nCmdShow)
{
 DriverProc(0,0,DRV_LOAD,0,0);
 DWORD dwDriverId=(DWORD)DriverProc(0,0,DRV_OPEN,0,0);
 if (dwDriverId)
  {
   DriverProc(dwDriverId,0,ICM_CONFIGURE,0,0);
   DriverProc(dwDriverId,0,DRV_CLOSE,0,0);
  }
 DriverProc(0,0,DRV_FREE,0,0);
}
