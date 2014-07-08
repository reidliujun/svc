/*
 * Copyright (c) 2002-2006 Milan Cutka
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
#include "ffglobals.h"
#include <shlobj.h>
#include "libavcodec/bitstream.h"
#include "libavcodec/golomb.h"
#include "ffdshow_mediaguids.h"
#include "rational.h"
#include "libavcodec/avcodec.h"
#include "TffPict.h"
#include "autoptr.h"
#include "reg.h"
#include "Tstream.h"
#include "compiler.h"
//#include "ffdebug.h"

#ifdef UNICODE
 #define UNICODE_BUILD "unicode"
#else
 #define UNICODE_BUILD "ansi"
#endif

const char_t *FFDSHOW_VER=_l(__DATE__) _l(" ") _l(__TIME__) _l(" (") _l(COMPILER) _l(COMPILER_X64) _l(", ") _l(UNICODE_BUILD) _l(")");
#undef COMPILER
#undef COMPILER_SSE
#undef COMPILER_SSE2
#undef COMPILER_X64

char_t* strncatf(char_t *dst,size_t dstlen,const char_t *fmt,...)
{
 char_t *pomS = (char_t *)_alloca(dstlen * sizeof(char_t));
 va_list va;
 va_start(va,fmt);
 vsnprintf_s(pomS, dstlen , _TRUNCATE, fmt, va);
 va_end(va);
 strncat_s(dst, dstlen, pomS, _TRUNCATE);
 return dst;
}

char_t* strncpyf(char_t *dst,size_t dstlen,const char_t *fmt,...)
{
 va_list va;
 va_start(va,fmt);
 vsnprintf_s(dst, dstlen, _TRUNCATE, fmt, va);
 va_end(va);
 return dst;
}

template<class tchar,class TlistItem> void strtok(const tchar *s,const tchar *delim,std::vector<TlistItem > &lst,bool add_empty,size_t max_parts)
{
 typedef typename tchar_traits<tchar>::ffstring ffstring;
 lst.clear();
 size_t delimLen=strlen(delim);
 const tchar *f;
 while ((f=strstr(s,delim))!=NULL && lst.size()+1<max_parts)
  {
   if (f-s || add_empty)
    lst.push_back(TlistItem(s,f-s));
   s=f+delimLen;
  }
 if (s[0] || add_empty)
  lst.push_back(s);
}
template<class tchar> void strtok(const tchar *s,const tchar *delim,ints &lst,bool add_empty,size_t max_parts)
{
 typedef typename tchar_traits<tchar>::strings strings;
 strings listS;strtok(s,delim,listS,add_empty,max_parts);
 lst.clear();
 for (typename strings::const_iterator i=listS.begin();i!=listS.end();i++)
  {
   tchar *end;
   int x=strtol(i->c_str(),&end,10);
   if (*end=='\0') lst.push_back(x);
  }
}
void mergetok(char_t *dst,size_t dstlen,const char_t *delim,const strings &list)
{
 size_t pos=0,delimlen=strlen(delim);
 for (strings::const_iterator l=list.begin();pos+1<dstlen && l!=list.end();l++)
  {
   size_t strlenl=l->size(),n=std::min(dstlen-pos,strlenl);
   memcpy(dst+pos,l->c_str(),n*sizeof(char_t));
   pos+=n;
   if (l!=list.end()-1)
    {
     n=std::min(dstlen-pos,delimlen);
     memcpy(dst+pos,delim,n*sizeof(char_t));
     pos+=n;
    }
  }
 dst[pos]='\0';
}

//inspired by XBrowseForFolder by Hans Dietrich
static int CALLBACK dlgGetDirFn(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData)
{
 switch (uMsg)
  {
   case BFFM_INITIALIZED:
    {
     // remove context help button from dialog caption
     LONG lStyle=GetWindowLong(hwnd,GWL_STYLE);
     lStyle&=~DS_CONTEXTHELP;
     SetWindowLong(hwnd,GWL_STYLE,lStyle);
     lStyle=GetWindowLong(hwnd,GWL_EXSTYLE);
     lStyle&=~WS_EX_CONTEXTHELP;
     SetWindowLong(hwnd,GWL_EXSTYLE,lStyle);
     // set initial directory
     SendMessage(hwnd,BFFM_SETSELECTION,TRUE,lpData);
     break;
    }
  }
 return 0;
}
bool dlgGetDir(HWND owner,char_t *dir,const char_t *capt)
{
 bool ret=false;
 IMalloc *g_pMalloc;CoGetMalloc(1,&g_pMalloc);
 LPTSTR lpBuffer=(LPTSTR)g_pMalloc->Alloc(MAX_PATH*sizeof(char_t));
 ff_strncpy(lpBuffer,dir,MAX_PATH);
 BROWSEINFO bi;
 bi.hwndOwner=owner;
 bi.pidlRoot=NULL;
 bi.pszDisplayName=lpBuffer;
 bi.lpszTitle=capt;
 bi.ulFlags=BIF_RETURNONLYFSDIRS/*|BIF_NEWDIALOGSTYLE*/;
 bi.lpfn=dlgGetDirFn;
 bi.lParam=LPARAM(dir);
 LPITEMIDLIST pidlBrowse=SHBrowseForFolder(&bi);
 if (pidlBrowse)
  {
   SHGetPathFromIDList(pidlBrowse,lpBuffer);
   g_pMalloc->Free((void*)pidlBrowse);
   if (lpBuffer[0])
    {
     strcpy(dir,lpBuffer);
     ret=true;
    }
  }
 g_pMalloc->Free(lpBuffer);
 return ret;
}

void findFiles(const char_t *mask,strings &lst,bool fullpaths)
{
 lst.clear();
 char_t dsk[MAX_PATH],dir[MAX_PATH];
 _splitpath_s(mask,dsk,MAX_PATH,dir,MAX_PATH,NULL,0,NULL,0);
 WIN32_FIND_DATA ff;
 HANDLE hFind=FindFirstFile(mask,&ff);
 BOOL bOk=(hFind!=(HANDLE)-1);
 while (bOk)
  {
   if (fullpaths)
    {
     char_t flnm[MAX_PATH];
     _makepath_s(flnm,MAX_PATH,dsk,dir,ff.cFileName,NULL);
     lst.push_back(flnm);
    }
   else
    lst.push_back(ff.cFileName);
   bOk=FindNextFile(hFind,&ff);
  }
 if (hFind!=(HANDLE)-1) FindClose(hFind);
}
bool fileexists(const char_t *flnm)
{
 return GetFileAttributes(flnm)!=INVALID_FILE_ATTRIBUTES;
}
bool directoryexists(const char_t *dir)
{
 DWORD r=GetFileAttributes(dir);
 return r!=INVALID_FILE_ATTRIBUTES && (r&FILE_ATTRIBUTE_DIRECTORY)!=0;
}

void extractfilepath(const char_t *flnm,char_t *path)
{
 char_t dsk[MAX_PATH],dir[MAX_PATH];
 _splitpath_s(flnm,dsk,MAX_PATH,dir,MAX_PATH,NULL,0,NULL,0);
 _makepath_s(path,MAX_PATH,dsk,dir,NULL,NULL);
}

void extractfilepath(const char_t *flnm,ffstring &path)
{
 path = _l("");
 if (!flnm || !flnm[0])
  return;
 size_t len = strlen(flnm) + 1;
 if (len > 4096)
  return;
 char_t *dsk     = (char_t *)_alloca(len * sizeof(char_t));
 char_t *dir     = (char_t *)_alloca(len * sizeof(char_t));
 char_t *pathbuf = (char_t *)_alloca(len * sizeof(char_t));
 _splitpath_s(flnm, dsk, len, dir, len, NULL, 0, NULL, 0);
 _makepath_s(pathbuf, len, dsk,dir,NULL,NULL);
 path = pathbuf;
}

void extractfilename(const char_t *flnm,char_t *nameext)
{
 char_t nm[MAX_PATH],ext[MAX_PATH];
 _splitpath_s(flnm,NULL,0,NULL,0,nm,MAX_PATH,ext,MAX_PATH);
 _makepath_s(nameext,MAX_PATH,NULL,NULL,nm,ext);
}

void extractfilename(const char_t *flnm,ffstring &nameext)
{
 nameext = _l("");
 if (!flnm || !flnm[0])
  return;
 size_t len = strlen(flnm) + 1;
 if (len > 4096)
  return;
 char_t *nm         = (char_t *)_alloca(len * sizeof(char_t));
 char_t *ext        = (char_t *)_alloca(len * sizeof(char_t));
 char_t *nameextbuf = (char_t *)_alloca(len * sizeof(char_t));
 _splitpath_s(flnm, NULL, 0, NULL, 0, nm, len, ext, len);
 _makepath_s(nameextbuf, len, NULL, NULL, nm, ext);
 nameext = nameextbuf;
}

void extractfilenameWOext(const char_t *flnm,char_t *name)
{
 char_t nm[MAX_PATH];
 _splitpath_s(flnm,NULL,0,NULL,0,nm,MAX_PATH,NULL,0);
 _makepath_s(name,MAX_PATH,NULL,NULL,nm,NULL);
}

void extractfilenameWOext(const char_t *flnm, ffstring &name)
{
 name = _l("");
 if (!flnm || !flnm[0])
  return;
 size_t len = strlen(flnm) + 1;
 if (len > 4096)
  return;
 char_t *nm      = (char_t *)_alloca(len * sizeof(char_t));
 char_t *namebuf = (char_t *)_alloca(len * sizeof(char_t));
 _splitpath_s(flnm, NULL, 0, NULL,0, nm, len, NULL, 0);
 _makepath_s(namebuf, len, NULL, NULL, nm, NULL);
 name = namebuf;
}

void extractfileext(const char_t *flnm,char_t *ext)
{
 _splitpath_s(flnm,NULL,0,NULL,0,NULL,0,ext,MAX_PATH);
 if (ext[0]) memmove(ext,ext+1,strlen(ext)*sizeof(char_t));
}

void extractfileext(const char_t *flnm,ffstring &ext)
{
 ext = _l("");
 if (!flnm || !flnm[0])
  return;
 size_t len=strlen(flnm) + 1;
 if (len > 4096)
  return;
 char_t *extbuf = (char_t *)_alloca(len * sizeof(char_t));
 _splitpath_s(flnm,NULL,0,NULL,0,NULL,0,extbuf,len);
 if (extbuf[0])
  ext = extbuf + 1;
}

void changepathext(const char_t *flnm, const char_t *ext, ffstring &path)
{
 path = _l("");
 if (!flnm || !flnm[0])
  return;
 size_t len = strlen(flnm) + strlen(ext) + 1;
 if (len > 4096)
  return;
 char_t *drv        = (char_t *)_alloca(len * sizeof(char_t));
 char_t *dir        = (char_t *)_alloca(len * sizeof(char_t));
 char_t *nm         = (char_t *)_alloca(len * sizeof(char_t));
 char_t *pathbuf    = (char_t *)_alloca(len * sizeof(char_t));
 _splitpath_s(flnm, drv, len, dir, len, nm, len, NULL, 0);
 _makepath_s(pathbuf, len, drv, dir, nm, ext);
 path = pathbuf;
}

FILETIME fileLastWriteTime(const char_t *flnm)
{
 WIN32_FIND_DATA fd;
 HANDLE h=FindFirstFile(flnm,&fd);
 if (h==INVALID_HANDLE_VALUE)
  {
   FILETIME ft={0,0};
   return ft;
  }
 else
  {
   FindClose(h);
   return fd.ftLastWriteTime;
  }
}

int nCopyAnsiToWideChar(WCHAR *pWCStr,PCTSTR pAnsiIn,int cchAnsi)
{
 if (!cchAnsi) cchAnsi = lstrlen(pAnsiIn) + 1;
#ifdef UNICODE
 return (int)strlen(ff_strncpy(pWCStr, pAnsiIn,cchAnsi)) + 1;
#else
 return MultiByteToWideChar(GetACP(), MB_PRECOMPOSED, pAnsiIn, cchAnsi, pWCStr, cchAnsi) + 1;
#endif
}

char *unicode16toAnsi(const WCHAR *data16,int data16len,char *data8)
{
 int data8len=WideCharToMultiByte(CP_ACP,0,data16,data16len,NULL,0,NULL,NULL);
 if (!data8)
  data8=(char*)calloc(data8len+1,1);
 BOOL wasDefChar;
 WideCharToMultiByte(CP_ACP,0,data16,data16len,data8,data8len,NULL,&wasDefChar);
 return data8;
}

char *utf8toAnsi(const char *data,int datalen,char *data8)
{
 int data16len=MultiByteToWideChar(CP_UTF8,0,data,datalen,NULL,0);
 LPWSTR data16=(LPWSTR)_alloca(data16len*2);
 MultiByteToWideChar(CP_UTF8,0,data,-1,data16,data16len);
 return unicode16toAnsi(data16,data16len,data8);
}
wchar_t *utf8toUnicode(const char *data,int datalen,wchar_t *data16)
{
 int data16len=MultiByteToWideChar(CP_UTF8,0,data,datalen,NULL,0);
 if (!data16)
  data16=(wchar_t*)calloc(data16len+1,1);
 MultiByteToWideChar(CP_UTF8,0,data,-1,data16,data16len);
 return data16;
}

bool dlgGetFile(bool save,HWND owner,const char_t *capt,const char_t *filter,const char_t *defext,char_t *flnm,const char_t *initdir,DWORD flags,DWORD *filterIndex)
{
 OPENFILENAME ofn;
 memset(&ofn,0,sizeof(ofn));
 ofn.lStructSize    =sizeof(OPENFILENAME);
 ofn.hwndOwner      =owner;
 ofn.lpstrFilter    =filter;
 if (filterIndex)
  ofn.nFilterIndex=*filterIndex;
 ofn.lpstrInitialDir=initdir;
 ofn.lpstrFile      =flnm;
 ofn.lpstrTitle     =capt;
 ofn.lpstrDefExt    =defext;
 ofn.nMaxFile       =MAX_PATH;
 ofn.Flags          =flags|(save?OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT:OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST)|OFN_ENABLESIZING|OFN_HIDEREADONLY;
 bool ret=(save?GetSaveFileName(&ofn):GetOpenFileName(&ofn))?true:false;
 if (ret && filterIndex)
  *filterIndex=ofn.nFilterIndex;
 return ret;
}
bool dlgOpenFiles(HWND owner,const char_t *capt,const char_t *filter,const char_t *defext,strings &files,const char_t *initdir,DWORD flags)
{
 OPENFILENAME ofn;
 memset(&ofn,0,sizeof(ofn));
 ofn.lStructSize    =sizeof(OPENFILENAME);
 ofn.hwndOwner      =owner;
 ofn.lpstrFilter    =filter;
 ofn.lpstrInitialDir=initdir;
 char_t *flnm=(char_t*)malloc(65536*sizeof(char_t));
 if (files.empty())
  flnm[0]='\0';
 else
  ff_strncpy(flnm, files[0].c_str(), 65536);
 ofn.lpstrFile      =LPTSTR(flnm);
 ofn.lpstrTitle     =capt;
 ofn.lpstrDefExt    =defext;
 ofn.nMaxFile       =MAX_PATH;
 ofn.Flags          =flags|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_ENABLESIZING|OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT|OFN_EXPLORER;
 BOOL ret=GetOpenFileName(&ofn);
 if (ret)
  {
   const char_t *dir=ofn.lpstrFile;
   if (directoryexists(dir))
    while ((ofn.lpstrFile=strchr(ofn.lpstrFile,'\0')+1)!=NULL && ofn.lpstrFile[0])
     {
      char_t flnm1[MAX_PATH];
      _makepath_s(flnm1,MAX_PATH,NULL,dir,ofn.lpstrFile,NULL);
      files.push_back(flnm1);
     }
   else
    files.push_back(dir);
   free(flnm);
   return true;
  }
 else
  {
   free(flnm);
   return false;
  }
}

// copied from ffmpeg
// (c) Michael Niedermayer

static const unsigned char ff_sqrt_tab[128]={
 0, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5,
 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
 9, 9, 9, 9,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,11,11,11,11,11,11,11
};
unsigned int ff_sqrt(unsigned int a)
{
 if (a<128) return ff_sqrt_tab[a];

 unsigned int ret=0;
 unsigned int ret_sq=0;
 for(int s=15; s>=0; s--)
  {
   unsigned int b= ret_sq + (1<<(s*2)) + (ret<<s)*2;
   if (b<=a)
    {
     ret_sq=b;
     ret+= 1<<s;
    }
  }
 return ret;
}

int64_t lavc_gcd(int64_t a, int64_t b){
    if(b) return lavc_gcd(b, a%b);
    else  return a;
}

int lavc_reduce(int *dst_nom, int *dst_den, int64_t nom, int64_t den, int64_t max){
/*
    int exact=1, sign=0;
    int64_t gcd;

    assert(den != 0);

    if(den < 0){
        den= -den;
        nom= -nom;
    }

    if(nom < 0){
        nom= -nom;
        sign= 1;
    }

        gcd = lavc_gcd(nom, den);
        nom /= gcd;
        den /= gcd;

    if(nom > max || den > max){
        AVRational a0={0,1}, a1={1,0};
        exact=0;

        for(;;){
            int64_t x= nom / den;
            int64_t a2n= x*a1.num + a0.num;
            int64_t a2d= x*a1.den + a0.den;

            if(a2n > max || a2d > max) break;

            nom %= den;

            a0= a1;
            a1.num=int(a2n);a1.den=int(a2d);// = (AVRational){a2n, a2d};
            if(nom==0) break;
            x= nom; nom=den; den=x;
        }
        nom= a1.num;
        den= a1.den;
    }

    assert(lavc_gcd(nom, den) == 1);

    if(sign) nom= -nom;

    *dst_nom = int(nom);
    *dst_den = int(den);

    return exact;
*/
    AVRational a0={0,1}, a1={1,0};
    int sign= (nom<0) ^ (den<0);
    int64_t gcd= lavc_gcd(ff_abs(nom), ff_abs(den));

    if(gcd){
    nom = ff_abs(nom)/gcd;
    den = ff_abs(den)/gcd;
    }
    if(nom<=max && den<=max){
        a1.num=(int)nom;a1.den=(int)den;//= (AVRational){nom, den};
        den=0;
    }

    while(den){
        int64_t x       = nom / den;
        int64_t next_den= nom - den*x;
        int64_t a2n= x*a1.num + a0.num;
        int64_t a2d= x*a1.den + a0.den;

        if(a2n > max || a2d > max) break;

        a0= a1;
        a1.num=(int)a2n;a1.den=(int)a2d;//= (AVRational){a2n, a2d};
        nom= den;
        den= next_den;
    }
    assert(lavc_gcd(a1.num, a1.den) == 1U);

    *dst_nom = sign ? -a1.num : a1.num;
    *dst_den = a1.den;

    return den==0;

}

static const uint8_t ff_log2_tab[256]={
        0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
        5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
        6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
        6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
};

int av_log2(unsigned int v)
{
    int n;

    n = 0;
    if (v & 0xffff0000) {
        v >>= 16;
        n += 16;
    }
    if (v & 0xff00) {
        v >>= 8;
        n += 8;
    }
    n += ff_log2_tab[v];

    return n;
}

static int sync_video_packet(const uint8_t *videobuf_code,size_t len){
  if (len<4)
   return 0;
  int skipped=0;
  while(1)
   {
    if(videobuf_code[0]==0 &&
       videobuf_code[1]==0 &&
       videobuf_code[2]==1) break; // synced
    // shift buffer, drop first byte
    ++skipped;
    videobuf_code++;len--;if (len==0) return 0; //EOF
   }
  //if(skipped) DPRINTF("videobuf: %d bytes skipped  (next: 0x1%02X)\n",skipped,videobuf_code[3]);
  return 0x100|videobuf_code[3];
}

bool decodeMPEGsequenceHeader(bool mpeg2,const unsigned char *hdr,size_t len,TffPictBase &pict,bool *isH264)
{
 int sync=sync_video_packet(hdr,len);
 if((sync&~0x60) == 0x107 && sync != 0x107)  // H.264
  {
   *isH264=true;
   return decodeH264SPS(hdr+2,len,pict);
  }
 *isH264=false;

 len*=8;
 if (!hdr || len<12+12+4) return false;
 GetBitContext gb;
 init_get_bits(&gb, hdr, (int)len);
 int id=get_bits(&gb,32);
 if (id!=0x01b3)
  return false;
 int dx=get_bits(&gb,12),dy=get_bits(&gb,12);
 pict.setSize(dx,dy);
 int aspect_ratio_info=get_bits(&gb, 4);
 if(!mpeg2)
  {
   static const float mpeg1_aspect[16]=
    {
     0.0000f,
     1.0000f,
     0.6735f,
     0.7031f,

     0.7615f,
     0.8055f,
     0.8437f,
     0.8935f,

     0.9157f,
     0.9815f,
     1.0255f,
     1.0695f,

     1.0950f,
     1.1575f,
     1.2015f,
    };
   float aspect1=mpeg1_aspect[aspect_ratio_info];
   if(aspect1!=0.0)
    pict.setSar(Rational(1/aspect1,16384));
   else
    pict.setSar(Rational(1,1));
  }
 else
  {
   /*
   static const float mpeg2_aspect[16]=
    {
     0,
     1.0,
     -3.0/4.0,
     -9.0/16.0,
     -1.0/2.21,
    };
   */
   switch (aspect_ratio_info)
    {
     case 1:pict.setSar(Rational(1,1));break;
     case 2:pict.setDar(Rational(4,3));break;
     case 3:pict.setDar(Rational(16,9));break;
     case 4:pict.setDar(Rational(221,100));break;
     default:pict.setSar(Rational(1,1));break;
    }
   /*
   aspect=mpeg2_aspect[aspect_ratio_info];
   if(aspect>0.0)      aspect_ratio= *width/(aspect**height);
   else if(aspect<0.0) aspect_ratio= -1.0/aspect;
   */
  }
 return true;
}

static inline void decode_hrd_parameters(GetBitContext &gb){
    int cpb_count, i;
    cpb_count = get_ue_golomb(&gb) + 1;
    get_bits(&gb, 4); /* bit_rate_scale */
    get_bits(&gb, 4); /* cpb_size_scale */
    for(i=0; i<cpb_count; i++){
        get_ue_golomb(&gb); /* bit_rate_value_minus1 */
        get_ue_golomb(&gb); /* cpb_size_value_minus1 */
        get_bits1(&gb);     /* cbr_flag */
    }
    get_bits(&gb, 5); /* initial_cpb_removal_delay_length_minus1 */
    get_bits(&gb, 5); /* cpb_removal_delay_length_minus1 */
    get_bits(&gb, 5); /* dpb_output_delay_length_minus1 */
    get_bits(&gb, 5); /* time_offset_length */
}

static void decode_scaling_list(GetBitContext &gb, uint8_t *factors, int size/*, const uint8_t *default_list*/){
    int i, last = 8, next = 8;
static const uint8_t zigzag_scan8x8[64]={
 0+0*8, 1+0*8, 0+1*8, 0+2*8,
 1+1*8, 2+0*8, 3+0*8, 2+1*8,
 1+2*8, 0+3*8, 0+4*8, 1+3*8,
 2+2*8, 3+1*8, 4+0*8, 5+0*8,
 4+1*8, 3+2*8, 2+3*8, 1+4*8,
 0+5*8, 0+6*8, 1+5*8, 2+4*8,
 3+3*8, 4+2*8, 5+1*8, 6+0*8,
 7+0*8, 6+1*8, 5+2*8, 4+3*8,
 3+4*8, 2+5*8, 1+6*8, 0+7*8,
 1+7*8, 2+6*8, 3+5*8, 4+4*8,
 5+3*8, 6+2*8, 7+1*8, 7+2*8,
 6+3*8, 5+4*8, 4+5*8, 3+6*8,
 2+7*8, 3+7*8, 4+6*8, 5+5*8,
 6+4*8, 7+3*8, 7+4*8, 6+5*8,
 5+6*8, 4+7*8, 5+7*8, 6+6*8,
 7+5*8, 7+6*8, 6+7*8, 7+7*8,
};
static const uint8_t zigzag_scan[16]={
 0+0*4, 1+0*4, 0+1*4, 0+2*4,
 1+1*4, 2+0*4, 3+0*4, 2+1*4,
 1+2*4, 0+3*4, 1+3*4, 2+2*4,
 3+1*4, 3+2*4, 2+3*4, 3+3*4,
};

    const uint8_t *scan = size == 16 ? zigzag_scan : zigzag_scan8x8;
    if(!get_bits1(&gb)) /* matrix not written, we use the default one */
        ;//memcpy(factors, default_list, size*sizeof(uint8_t));
    else
    for(i=0;i<size;i++){
        if(next)
            next = (last + get_se_golomb(&gb)) & 0xff;
        if(!i && !next){ /* matrix not written, we use the default one */
            ;//memcpy(factors, default_list, size*sizeof(uint8_t));
            break;
        }
        last = factors[scan[i]] = uint8_t(next ? next : last);
    }
}

bool decodeH264SPS(const unsigned char *hdr,size_t len,TffPictBase &pict)
{
 int sync=sync_video_packet(hdr,len);
 unsigned int tmp;
 if((sync&~0x60) == 0x107 && sync != 0x107)  // H.264
  {
   hdr+=2;len-=2;
  }

    static const int NAL_SPS=7;
    if ((hdr[2]&0x1F)!=NAL_SPS)
     return false;
    int profile_idc, level_idc;
    int sps_id, i;
    struct
     {
      int profile_idc;
      int level_idc;
      int transform_bypass;              ///< qpprime_y_zero_transform_bypass_flag
      int log2_max_frame_num;            ///< log2_max_frame_num_minus4 + 4
      int poc_type;                      ///< pic_order_cnt_type
      int log2_max_poc_lsb;              ///< log2_max_pic_order_cnt_lsb_minus4
      int delta_pic_order_always_zero_flag;
      int offset_for_non_ref_pic;
      int offset_for_top_to_bottom_field;
      int poc_cycle_length;              ///< num_ref_frames_in_pic_order_cnt_cycle
      int ref_frame_count;               ///< num_ref_frames
      int gaps_in_frame_num_allowed_flag;
      int mb_width;                      ///< pic_width_in_mbs_minus1 + 1
      int mb_height;                     ///< pic_height_in_map_units_minus1 + 1
      int frame_mbs_only_flag;
      int mb_aff;                        ///<mb_adaptive_frame_field_flag
      int direct_8x8_inference_flag;
      int crop;                   ///< frame_cropping_flag
      int crop_left;              ///< frame_cropping_rect_left_offset
      int crop_right;             ///< frame_cropping_rect_right_offset
      int crop_top;               ///< frame_cropping_rect_top_offset
      int crop_bottom;            ///< frame_cropping_rect_bottom_offset
      int vui_parameters_present_flag;
      AVRational sar;
      int timing_info_present_flag;
      uint32_t num_units_in_tick;
      uint32_t time_scale;
      int fixed_frame_rate_flag;
      short offset_for_ref_frame[256]; //FIXME dyn aloc?
      int bitstream_restriction_flag;
      int num_reorder_frames;
      int scaling_matrix_present;
      uint8_t scaling_matrix4[6][16];
      uint8_t scaling_matrix8[2][64];
     } _sps,*sps=&_sps;
    GetBitContext gb;init_get_bits(&gb,hdr,(int)len*8);
    skip_bits(&gb,24);

    profile_idc= get_bits(&gb, 8);
    get_bits1(&gb);   //constraint_set0_flag
    get_bits1(&gb);   //constraint_set1_flag
    get_bits1(&gb);   //constraint_set2_flag
    get_bits1(&gb);   //constraint_set3_flag
    get_bits(&gb, 4); // reserved
    level_idc= get_bits(&gb, 8);
    sps_id= get_ue_golomb(&gb);

    sps->profile_idc= profile_idc;
    sps->level_idc= level_idc;

    if(sps->profile_idc >= 100){ //high profile
        if(get_ue_golomb(&gb) == 3) //chroma_format_idc
            get_bits1(&gb);  //residual_color_transform_flag
        get_ue_golomb(&gb);  //bit_depth_luma_minus8
        get_ue_golomb(&gb);  //bit_depth_chroma_minus8
        sps->transform_bypass = get_bits1(&gb);
        if(get_bits1(&gb)){  //seq_scaling_matrix_present_flag
            //decode_scaling_matrices(h, sps, NULL, 1, sps->scaling_matrix4, sps->scaling_matrix8);//av_log(h->s.avctx, AV_LOG_ERROR, "custom scaling matrix not implemented\n");
            if(get_bits1(&gb)){
                sps->scaling_matrix_present = 1;
                uint8_t scaling_matrix4[6][16];
                uint8_t scaling_matrix8[2][64];
                decode_scaling_list(gb,scaling_matrix4[0],16); // Intra, Y
                decode_scaling_list(gb,scaling_matrix4[1],16); // Intra, Cr
                decode_scaling_list(gb,scaling_matrix4[2],16); // Intra, Cb
                decode_scaling_list(gb,scaling_matrix4[3],16); // Inter, Y
                decode_scaling_list(gb,scaling_matrix4[4],16); // Inter, Cr
                decode_scaling_list(gb,scaling_matrix4[5],16); // Inter, Cb
                if(/*is_sps*/1){
                    decode_scaling_list(gb,scaling_matrix8[0],64);  // Intra, Y
                    decode_scaling_list(gb,scaling_matrix8[1],64);  // Inter, Y
                }
            }
        }
    }
    else
        sps->scaling_matrix_present=0;

    sps->log2_max_frame_num= get_ue_golomb(&gb) + 4;
    sps->poc_type= get_ue_golomb(&gb);

    if(sps->poc_type == 0){ //FIXME #define
        sps->log2_max_poc_lsb= get_ue_golomb(&gb) + 4;
    } else if(sps->poc_type == 1){//FIXME #define
        sps->delta_pic_order_always_zero_flag= get_bits1(&gb);
        sps->offset_for_non_ref_pic= get_se_golomb(&gb);
        sps->offset_for_top_to_bottom_field= get_se_golomb(&gb);
        tmp= get_ue_golomb(&gb);

        if(tmp >= sizeof(sps->offset_for_ref_frame) / sizeof(sps->offset_for_ref_frame[0])){
            //DPRINTF(_l("poc_cycle_length overflow %u\n"), tmp);
            return false;
        }
        sps->poc_cycle_length= tmp;

        for(i=0; i<sps->poc_cycle_length; i++)
            sps->offset_for_ref_frame[i]= short(get_se_golomb(&gb));
    }else if(sps->poc_type != 2){
        //DPRINTF(_l("illegal POC type %d\n"), sps->poc_type);
        return false;
    }

    static const int MAX_PICTURE_COUNT=32;
    tmp= get_ue_golomb(&gb);
    if(tmp > MAX_PICTURE_COUNT-2){
        //DPRINTF(_l("too many reference frames\n"));
    }
    sps->ref_frame_count= tmp;
    sps->gaps_in_frame_num_allowed_flag= get_bits1(&gb);
    sps->mb_width= get_ue_golomb(&gb) + 1;
    sps->mb_height= get_ue_golomb(&gb) + 1;
    if((unsigned)sps->mb_width >= INT_MAX/16 || (unsigned)sps->mb_height >= INT_MAX/16 /*||
       avcodec_check_dimensions(NULL, 16*sps->mb_width, 16*sps->mb_height)*/)
        return false;

    sps->frame_mbs_only_flag= get_bits1(&gb);
    if(!sps->frame_mbs_only_flag)
        sps->mb_aff= get_bits1(&gb);
    else
        sps->mb_aff= 0;

    sps->direct_8x8_inference_flag= get_bits1(&gb);

    //if(!sps->direct_8x8_inference_flag && sps->mb_aff)
    //    DPRINTF(_l("MBAFF + !direct_8x8_inference is not implemented\n"));

    sps->crop= get_bits1(&gb);
    if(sps->crop){
        sps->crop_left  = get_ue_golomb(&gb);
        sps->crop_right = get_ue_golomb(&gb);
        sps->crop_top   = get_ue_golomb(&gb);
        sps->crop_bottom= get_ue_golomb(&gb);
        if(sps->crop_left || sps->crop_top){
            //DPRINTF(_l("insane cropping not completely supported, this could look slightly wrong ...\n"));
        }
    }else{
        sps->crop_left  =
        sps->crop_right =
        sps->crop_top   =
        sps->crop_bottom= 0;
    }

    sps->vui_parameters_present_flag= get_bits1(&gb);
    if( sps->vui_parameters_present_flag ) {
        int aspect_ratio_info_present_flag, aspect_ratio_idc;
        int nal_hrd_parameters_present_flag, vcl_hrd_parameters_present_flag;

        aspect_ratio_info_present_flag= get_bits1(&gb);

        if( aspect_ratio_info_present_flag ) {
            aspect_ratio_idc= get_bits(&gb, 8);
            static const int EXTENDED_SAR=255;
            if( aspect_ratio_idc == EXTENDED_SAR ) {
                sps->sar.num= get_bits(&gb, 16);
                sps->sar.den= get_bits(&gb, 16);
            }else if(aspect_ratio_idc < 14){
                static const AVRational pixel_aspect[14]={
                 {0, 1},
                 {1, 1},
                 {12, 11},
                 {10, 11},
                 {16, 11},
                 {40, 33},
                 {24, 11},
                 {20, 11},
                 {32, 11},
                 {80, 33},
                 {18, 11},
                 {15, 11},
                 {64, 33},
                 {160,99},
                };
                sps->sar=  pixel_aspect[aspect_ratio_idc];
            }else{
                //DPRINTF("illegal aspect ratio\n");
                return false;
            }
            if (sps->sar.num && sps->sar.den)
                pict.setSar(sps->sar);
        }else{
            sps->sar.num=
            sps->sar.den= 0;
        }



        if(get_bits1(&gb)){      /* overscan_info_present_flag */
            get_bits1(&gb);      /* overscan_appropriate_flag */
        }

        if(get_bits1(&gb)){      /* video_signal_type_present_flag */
            get_bits(&gb, 3);    /* video_format */
            get_bits1(&gb);      /* video_full_range_flag */
            if(get_bits1(&gb)){  /* colour_description_present_flag */
                get_bits(&gb, 8); /* colour_primaries */
                get_bits(&gb, 8); /* transfer_characteristics */
                get_bits(&gb, 8); /* matrix_coefficients */
            }
        }

        if(get_bits1(&gb)){      /* chroma_location_info_present_flag */
            get_ue_golomb(&gb);  /* chroma_sample_location_type_top_field */
            get_ue_golomb(&gb);  /* chroma_sample_location_type_bottom_field */
        }

        sps->timing_info_present_flag = get_bits1(&gb);
        if(sps->timing_info_present_flag){
            sps->num_units_in_tick = get_bits_long(&gb, 32);
            sps->time_scale = get_bits_long(&gb, 32);
            sps->fixed_frame_rate_flag = get_bits1(&gb);
        }

        nal_hrd_parameters_present_flag = get_bits1(&gb);
        if(nal_hrd_parameters_present_flag)
            decode_hrd_parameters(gb);
        vcl_hrd_parameters_present_flag = get_bits1(&gb);
        if(vcl_hrd_parameters_present_flag)
            decode_hrd_parameters(gb);
        if(nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag)
            get_bits1(&gb);     /* low_delay_hrd_flag */
        get_bits1(&gb);         /* pic_struct_present_flag */

        sps->bitstream_restriction_flag = get_bits1(&gb);
        if(sps->bitstream_restriction_flag){
            get_bits1(&gb);     /* motion_vectors_over_pic_boundaries_flag */
            get_ue_golomb(&gb); /* max_bytes_per_pic_denom */
            get_ue_golomb(&gb); /* max_bits_per_mb_denom */
            get_ue_golomb(&gb); /* log2_max_mv_length_horizontal */
            get_ue_golomb(&gb); /* log2_max_mv_length_vertical */
            sps->num_reorder_frames = get_ue_golomb(&gb);
            get_ue_golomb(&gb); /* max_dec_frame_buffering */
        }
    }
/*
//=============== PPS ================
    skip_bits(&gb,24);
    struct
     {
      int sps_id,pic_order_present,slice_group_count,mb_slice_group_map_type,ref_count[2],cabac,weighted_pred,weighted_bipred_idc,init_qp,init_qs,chroma_qp_index_offset,deblocking_filter_parameters_present,constrained_intra_pred,redundant_pic_cnt_present;
     } _pps, *pps=&_pps;
    pps->sps_id= get_ue_golomb(&gb);
    pps->cabac= get_bits1(&gb);
    pps->pic_order_present= get_bits1(&gb);
    pps->slice_group_count= get_ue_golomb(&gb) + 1;
    if(pps->slice_group_count > 1 ){
        pps->mb_slice_group_map_type= get_ue_golomb(&gb);
        //DPRINTF("FMO not supported\n");
        switch(pps->mb_slice_group_map_type){
        case 0:
#if 0
|   for( i = 0; i <= num_slice_groups_minus1; i++ ) |   |        |
|    run_length[ i ]                                |1  |ue(v)   |
#endif
            break;
        case 2:
#if 0
|   for( i = 0; i < num_slice_groups_minus1; i++ )  |   |        |
|{                                                  |   |        |
|    top_left_mb[ i ]                               |1  |ue(v)   |
|    bottom_right_mb[ i ]                           |1  |ue(v)   |
|   }                                               |   |        |
#endif
            break;
        case 3:
        case 4:
        case 5:
#if 0
|   slice_group_change_direction_flag               |1  |u(1)    |
|   slice_group_change_rate_minus1                  |1  |ue(v)   |
#endif
            break;
        case 6:
#if 0
|   slice_group_id_cnt_minus1                       |1  |ue(v)   |
|   for( i = 0; i <= slice_group_id_cnt_minus1; i++ |   |        |
|)                                                  |   |        |
|    slice_group_id[ i ]                            |1  |u(v)    |
#endif
            break;
        }
    }
    pps->ref_count[0]= get_ue_golomb(&gb) + 1;
    pps->ref_count[1]= get_ue_golomb(&gb) + 1;
    if(pps->ref_count[0] > 32 || pps->ref_count[1] > 32){
        //DPRINTF("reference overflow (pps)\n");
        return false;
    }

    pps->weighted_pred= get_bits1(&gb);
    pps->weighted_bipred_idc= get_bits(&gb, 2);
    pps->init_qp= get_se_golomb(&gb) + 26;
    pps->init_qs= get_se_golomb(&gb) + 26;
    pps->chroma_qp_index_offset= get_se_golomb(&gb);
    pps->deblocking_filter_parameters_present= get_bits1(&gb);
    pps->constrained_intra_pred= get_bits1(&gb);
    pps->redundant_pic_cnt_present = get_bits1(&gb);
*/
    return true;
}
void* memsetd(void *dest,uint32_t c,size_t bytes)
{
#if !defined(__GNUC__) && !defined(WIN64)
 __asm
  {
   mov eax, c
   mov ecx, bytes
   shr ecx, 2
   mov edi, dest
   cld
   rep stosd
  }
 return dest;
#else
 uint32_t *dest2=(uint32_t*)dest;
 for (;bytes>=sizeof(uint32_t);dest2++,bytes-=sizeof(uint32_t))
  *dest2=c;
 return dest2;
#endif
}

void saveFrame(unsigned int num,const unsigned char *buf,size_t len)
{
 char_t flnm[MAX_PATH];tsprintf(flnm,_l("c:\\ffdshow_frame_%06u.raw"),num);
 FILE *f=fopen(flnm,_l("wb"));
 fwrite(buf,1,len,f);
 fclose(f);
}
void dumpBytes(const char_t *flnm,const unsigned char *buf,size_t len)
{
 FILE *f=fopen(flnm,_l("wb"));
 fwrite(buf,1,len,f);
 fflush(f);fclose(f);
}

char_t *readTextFile(const char_t *filename)
{
 int b_error=0;
 char_t *buf;
 TstreamFile fh(filename,false,false);
 if (!fh)
  return NULL;
 b_error|=fh.seek( 0, SEEK_END )<0;
 long i_size;
 b_error|=(i_size=fh.tell())<=0;
 b_error|=fh.seek( 0, SEEK_SET )<0;
 if (b_error)
  return NULL;
 buf=(char_t*)malloc((i_size+2)*sizeof(char_t));
 if (buf==NULL)
  return NULL;
 char_t *buf1=buf;
 while (fh.fgets(buf1,i_size))
  buf1=strchr(buf1,'\0');

 //b_error|=fh.read(buf,1,i_size)!=i_size;
 if (buf[i_size-1]!='\n')
     buf[i_size++]='\n';
 buf[i_size]=0;
 //fclose(fh);
 if (b_error)
  {
   free(buf);
   return NULL;
  }
 return buf;
}

static BOOL CALLBACK getChildWindowsEnum(HWND h,LPARAM lst)
{
 ((THWNDs*)lst)->push_back(h);
 return TRUE;
}
void getChildWindows(HWND h,THWNDs &lst)
{
 lst.clear();
 EnumChildWindows(h,getChildWindowsEnum,LPARAM(&lst));
}

void randomize(void)
{
 srand((unsigned int)time(NULL));
}

struct MPEG4context
{
 int vo_type;
 int aspect_ratio_info;
 int aspected_width,aspected_height;
};
static const uint16_t pixel_aspect[16][2]={
 {0, 0},
 {1, 1},
 {12, 11},
 {10, 11},
 {16, 11},
 {40, 33},
 {0, 0},
 {0, 0},
 {0, 0},
 {0, 0},
 {0, 0},
 {0, 0},
 {0, 0},
 {0, 0},
 {0, 0},
 {0, 0},
};

static int decode_vol_header(MPEG4context *s, GetBitContext *gb)
{
 int /*width, height,*/ vo_ver_id;

 /* vol header */
 skip_bits(gb, 1); /* random access */
 s->vo_type= get_bits(gb, 8);
 if (get_bits1(gb) != 0) { /* is_ol_id */
   vo_ver_id = get_bits(gb, 4); /* vo_ver_id */
   skip_bits(gb, 3); /* vo_priority */
 } else {
   vo_ver_id = 1;
 }
 //printf("vo type:%d\n",s->vo_type);
 s->aspect_ratio_info= get_bits(gb, 4);
	if(s->aspect_ratio_info == FF_ASPECT_EXTENDED){
   s->aspected_width = get_bits(gb, 8); // par_width
   s->aspected_height = get_bits(gb, 8); // par_height
 }else{
   s->aspected_width = pixel_aspect[s->aspect_ratio_info][0];
   s->aspected_height= pixel_aspect[s->aspect_ratio_info][1];
 }
 return 0;
}

bool decodeMPEG4pictureHeader(const unsigned char *hdr,size_t len,TffPictBase &pict)
{
    int startcode, v;
    GetBitContext gb;
    MPEG4context s;
    init_get_bits(&gb, hdr, (int)len*8);

    /* search next start code */
    align_get_bits(&gb);
    startcode = 0xff;
    for(;;) {
        v = get_bits(&gb, 8);
        startcode = ((startcode << 8) | v) & 0xffffffff;

        if(get_bits_count(&gb) >= gb.size_in_bits){
                return false; //end of stream
        }

        if((startcode&0xFFFFFF00) != 0x100)
            continue; //no startcode

        switch(startcode){
        case 0x120:
            if(decode_vol_header(&s, &gb) < 0)
             return false;
            else
             {
              pict.setSar(Rational(s.aspected_width,s.aspected_height));
              return true;
             }
/*
#define VOS_STARTCODE        0x1B0
#define USER_DATA_STARTCODE  0x1B2
#define GOP_STARTCODE        0x1B3
#define VOP_STARTCODE        0x1B6
        case USER_DATA_STARTCODE:
            decode_user_data(s, gb);
            break;
        case GOP_STARTCODE:
            mpeg4_decode_gop_header(s, gb);
            break;
        case VOP_STARTCODE:
            return decode_vop_header(s, gb);
*/
        default:
            break;
        }

        align_get_bits(&gb);
        startcode = 0xff;
    }
}

// encoding
const char_t *encQuantTypes[]=
{
 _l("H.263"),
 _l("MPEG"),
 _l("Modulated"),
 _l("Modulated new"),
 _l("Custom"),
 _l("JVT")
};

const Taspect displayAspects[]=
{
 _l("1:1")   , 1.0f,1.0f,
 _l("4:3")   , 4.0f,3.0f,
 _l("16:9")  ,16.0f,9.0f,
 _l("2.21:1"),2.21f,1.0f,
 NULL,0
};
const Taspect sampleAspects[]=
{
 _l("Square")   , 1.0f, 1.0f,
 _l("4:3 PAL")  ,12.0f,11.0f,
 _l("4:3 NTSC") ,10.0f,11.0f,
 _l("16:9 PAL") ,16.0f,11.0f,
 _l("16:9 NTSC"),40.0f,33.0f,
 NULL,0
};

//======================== YUVcolor ==========================
YUVcolor::YUVcolor(COLORREF rgb,bool vob)
{
 r=GetRValue(rgb),g=GetGValue(rgb),b=GetBValue(rgb);
 if (!vob)
  {
   Y=(uint8_t)((((uint16_t) ((0.299)   * (1L << 8) + 0.5)) * r +
                ((uint16_t) ((0.587)   * (1L << 8) + 0.5)) * g +
                ((uint16_t) ((0.114)   * (1L << 8) + 0.5)) * b) >> 8);
   U=(int8_t)((-((int16_t)  ((0.16874) * (1L << 8) + 0.5)) * r -
                ((int16_t)  ((0.33126) * (1L << 8) + 0.5)) * g +
                ((int16_t)  ((0.50000) * (1L << 8) + 0.5)) * b) >> 9);// + 128;
   V=(int8_t)(( ((int16_t)  ((0.50000) * (1L << 8) + 0.5)) * r -
                ((int16_t)  ((0.41869) * (1L << 8) + 0.5)) * g -
                ((int16_t)  ((0.08131) * (1L << 8) + 0.5)) * b) >> 9);// + 128;
  }
 else
  {
   Y=(uint8_t)((((uint16_t) (( 0.1494 ) * (1L << 8) + 0.5)) * r +
                ((uint16_t) (( 0.6061 ) * (1L << 8) + 0.5)) * g +
                ((uint16_t) (( 0.2445 ) * (1L << 8) + 0.5)) * b) >> 8);
   U=(int8_t) ((((int16_t)  (( 0.6066 ) * (1L << 8) + 0.5)) * r -
                ((int16_t)  (( 0.4322 ) * (1L << 8) + 0.5)) * g -
                ((int16_t)  (( 0.1744 ) * (1L << 8) + 0.5)) * b) >> 8) + 128;
   V=(int8_t) ((((int16_t)  ((-0.08435) * (1L << 8) + 0.5)) * r -
                ((int16_t)  (( 0.3422 ) * (1L << 8) + 0.5)) * g +
                ((int16_t)  (( 0.4266 ) * (1L << 8) + 0.5)) * b) >> 8) + 128;
  }
}

YUVcolorA::YUVcolorA(YUVcolor yuv,unsigned int alpha)
{
 A=alpha;
 Y=(unsigned int)(0.257*yuv.r+0.504*yuv.g+0.098*yuv.b+16.0);
 U=(unsigned int)(-0.148*yuv.r-0.291*yuv.g+0.439*yuv.b+128.0);
 V=(unsigned int)(0.439*yuv.r-0.368*yuv.g-0.071*yuv.b+128.0);
 r=yuv.r;
 g=yuv.g;
 b=yuv.b;
}

YUVcolorA::YUVcolorA(COLORREF rgb,unsigned int alpha)
{
 r=GetRValue(rgb);
 g=GetGValue(rgb);
 b=GetBValue(rgb);
 Y=(unsigned int)(0.257*r+0.504*g+0.098*b+16.0);
 U=(unsigned int)(-0.148*r-0.291*g+0.439*b+128.0);
 V=(unsigned int)(0.439*r-0.368*g-0.071*b+128.0);
 A=alpha;
}
// Copyright (C) 1995,1998,1999 DJ Delorie
static bool isslash(char_t c) {return c=='\\' || c=='/';}
static const char_t *find_slash(const char_t *s)
{
 while (*s)
  {
   if (isslash(*s))
    return s;
   s++;
  }
 return 0;
}
struct Ttoupper
{
 template<class T> inline int operator()(T c) {return tchar_traits<char_t>::toupper((tchar_traits<char_t>::toupper_t)c);}
};
static const char_t *rangematch(const char_t *pattern, char_t test, int nocase)
{
 char_t c, c2;
 int negate, ok;

 if ((negate = (*pattern == '!'))!=NULL)
  ++pattern;

 for (ok = 0; (c = *pattern++) != ']';)
  {
   if (c == 0)
    return 0;/* illegal pattern */
   if (*pattern == '-' && (c2 = pattern[1]) != 0 && c2 != ']')
    {
     if (c <= test && test <= c2)
      ok = 1;
     if (nocase && Ttoupper()(c) <= Ttoupper()(test) && Ttoupper()(test) <= Ttoupper()(c2))
      ok = 1;
     pattern += 2;
    }
   else if (c == test)
    ok = 1;
   else if (nocase && (Ttoupper()(c) == Ttoupper()(test)))
    ok = 1;
  }
 return ok == negate ? NULL : pattern;
}
bool fnmatch(const char_t *pattern, const char_t *string, int flags)
{
 char_t c;
 char_t test;

 for (;;)
  switch ((c = *pattern++))
   {
    case 0:
     return *string == 0 ? true : false;
    case '?':
     if ((test = *string++) == 0 || (isslash(test) && (flags & FNM_PATHNAME)))
      return false;
     break;
    case '*':
     c = *pattern;
     // collapse multiple stars
     while (c=='*')
      c=*++pattern;
     // optimize for pattern with * at end or before /
     if (c == 0)
      {
       if (flags & FNM_PATHNAME)
        return find_slash(string) ? false : true;
       else
        return true;
      }
     else if (isslash(c) && flags & FNM_PATHNAME)
      {
       if ((string = find_slash(string)) == NULL)
        return false;
       break;
      }

     // general case, use recursion
     while ((test = *string) != 0)
      {
       if (fnmatch(pattern, string, flags) == true)
        return true;
       if (isslash(test) && flags & FNM_PATHNAME)
        break;
       ++string;
      }
     return false;
    case '[':
     if ((test = *string++) == 0 || (isslash(test) && flags & FNM_PATHNAME))
      return false;
     if ((pattern = rangematch(pattern, test, flags & FNM_NOCASE)) == NULL)
      return false;
     break;
    case '\\':
     if (!(flags & FNM_NOESCAPE) && pattern[1] && strchr("*?[\\", pattern[1]))
      {
       if ((c = *pattern++) == 0)
        {
         c = '\\';
         --pattern;
        }
       if (c != *string++)
        return false;
       break;
      }
      // FALLTHROUGH
    default:
     if (isslash(c) && isslash(*string))
      {
       string++;
       break;
      }
     if (flags & FNM_NOCASE)
      {
       if (Ttoupper()(c) != Ttoupper()(*string++))
        return false;
      }
     else
      {
       if (c != *string++)
        return false;
      }
     break;
   }
}

template<class tchar> const tchar* stristr(const tchar *haystack,const tchar *needle)
{
 if (!(haystack && needle)) return NULL;

 size_t len=strlen(needle);
 const tchar *p=haystack;
 while (*p != '\0')
  {
   if (_strnicmp(p,needle,len)==0) return p;
   p++;
  }
 return NULL;
}

template<class tchar> const tchar* strnistr(const tchar *haystack,size_t n,const tchar *needle)
{
 if (!(haystack && needle)) return NULL;

 size_t len=strlen(needle);n-=len-1;
 const tchar *p=haystack;
 while (*p && n--)
  {
   if (_strnicmp(p,needle,len)==0) return (tchar*)p;
   p++;
  }
 return NULL;
}

template<class tchar> const tchar* strnstr(const tchar *haystack,size_t n,const tchar *needle)
{
 if (!(haystack && needle)) return NULL;

 size_t len=strlen(needle);n-=len-1;
 const tchar *p=haystack;
 while (*p && n--)
  {
   if (strncmp(p,needle,len)==0) return (tchar*)p;
   p++;
  }
 return NULL;
}

template<class tchar> const void* memnstr(const void *haystack,size_t n,const tchar *needle)
{
 if (!(haystack && needle)) return NULL;

 int step=sizeof(tchar);
 size_t len=strlen(needle)*step;n-=len-step;
 const tchar *p=(const tchar*)haystack;
 while (n-=step)
  {
   if (memcmp(p,needle,len)==0) return p;
   p+=step;
  }
 return NULL;
}

template<class tchar> const tchar *strnchr(const tchar *s,size_t n,int c)
{
 while (n-- && *s)
  if (*s==c)
   return s;
  else
   s++;
 return NULL;
}
template<class char_t> char_t* strrmchar(char_t *s,int c)
{
 char_t *p1=s;
 for (const char_t *p2=p1;*p2;*p1=*p2,p2++,p1+=*p1!=c);
 *p1='\0';
 return s;
}

int countbits(uint32_t x)
{
 static const int numbits[256]=
  {
   0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,
   1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
   1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
   2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
   1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
   2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
   2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
   3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
   1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
   2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
   2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
   3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
   2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
   3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
   3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
   4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8,
  };
 return numbits[((uint8_t*)&x)[0]]+
        numbits[((uint8_t*)&x)[1]]+
        numbits[((uint8_t*)&x)[2]]+
        numbits[((uint8_t*)&x)[3]];
}

void setThreadName(DWORD dwThreadID,LPCSTR szThreadName)
{
#ifdef DEBUG
 struct THREADNAME_INFO
  {
   DWORD dwType;     // must be 0x1000
   LPCSTR szName;    // pointer to name (in user addr space)
   DWORD dwThreadID; // thread ID (-1=caller thread)
   DWORD dwFlags;    // reserved for future use, must be zero
  } info;
 info.dwType=0x1000;
 info.szName=szThreadName;
 info.dwThreadID=dwThreadID;
 info.dwFlags=0;
 __try
  {
   RaiseException(0x406D1388,0,sizeof(info)/sizeof(DWORD),(ULONG_PTR*)&info);
  }
 __except(EXCEPTION_CONTINUE_EXECUTION)
  {
  }
#endif
}

char_t *guid2str(const GUID &riid,char_t *dest,int bufsize)
{
 LPOLESTR str;
 StringFromIID(riid,&str);
 text<char_t>(str, -1, dest, bufsize);
 CoTaskMemFree(str);
 return dest;
}

FOURCC hdr2fourcc(const BITMAPINFOHEADER *hdr,const GUID *subtype)
{
 if (subtype && *subtype==MEDIASUBTYPE_QTRle) return FOURCC_QRLE;
 if (subtype && *subtype==MEDIASUBTYPE_QTRpza) return FOURCC_RPZA;
 if (hdr->biCompression==0)
  {
   if (subtype)
    {
     if      (*subtype==MEDIASUBTYPE_RGB555) return FOURCC_RGB5;
     else if (*subtype==MEDIASUBTYPE_RGB565) return FOURCC_RGB6;
     else if (*subtype==MEDIASUBTYPE_RGB24 ) return FOURCC_RGB2;
     else if (*subtype==MEDIASUBTYPE_RGB32 || *subtype==MEDIASUBTYPE_ARGB32) return FOURCC_RGB3;
    }
   switch (hdr->biBitCount)
    {
     case 1:return FOURCC_PAL1;break;
     case 4:return FOURCC_PAL4;break;
     case 8:return FOURCC_PAL8;break;
     case 15:return FOURCC_RGB5;break;
     case 16:return FOURCC_RGB6;break;
     case 24:return FOURCC_RGB2;break;
     case 32:return FOURCC_RGB3;break;
    }
  }
 return hdr->biCompression;
}

const char_t *fourcc2str(FOURCC fcc,char_t *name,size_t namelength)
{
 switch (fcc)
  {
   case FOURCC_PAL1:ff_strncpy(name,_l("2 colors"),namelength);break;
   case FOURCC_PAL4:ff_strncpy(name,_l("16 colors"),namelength);break;
   case FOURCC_PAL8:ff_strncpy(name,_l("256 colors"),namelength);break;
   case FOURCC_RGB15MPLAYER:
   case FOURCC_RGB5:ff_strncpy(name,_l("RGB555"),namelength);break;
   case FOURCC_RGB16MPLAYER:
   case FOURCC_RGB6:ff_strncpy(name,_l("RGB565"),namelength);break;
   case FOURCC_RGB24MPLAYER:
   case FOURCC_RGB2:ff_strncpy(name,_l("RGB24") ,namelength);break;
   case FOURCC_RGB32MPLAYER:
   case FOURCC_RGB3:ff_strncpy(name,_l("RGB32") ,namelength);break;
   case FOURCC_RLE4:ff_strncpy(name,_l("MSRLE4"),namelength);break;
   case FOURCC_RLE8:ff_strncpy(name,_l("MSRLE8"),namelength);break;
   case FOURCC_1001:ff_strncpy(name,_l("MPEG1") ,namelength);break;
   case FOURCC_1002:ff_strncpy(name,_l("MPEG2") ,namelength);break;
   case FOURCC_BITFIELDS:ff_strncpy(name,_l("bitfields"),namelength);break;
   default:
    {
     char nameA[5];
     memcpy(nameA,&fcc,std::min(namelength,(size_t)4));nameA[std::min(namelength,(size_t)4)]='\0';
     text<char_t>(nameA,name);
     break;
    }
  }
 name[namelength-1]='\0';
 return name;
}

void fixMPEGinAVI(FOURCC &fcc)
{
 FOURCC fccu=FCCupper(fcc);
 if (fccu==FOURCC_1001 || fccu==FOURCC_1002 || fccu==FOURCC_MPG1 || fccu==FOURCC_MPG2)
  fcc=FOURCC_MPEG;
}

HWND createInvisibleWindow(HINSTANCE hi,const char_t *classname,const char_t *windowname,WNDPROC wndproc,void* lparam,ATOM *atom)
{
 if (!atom || !*atom)
  {
   WNDCLASS wndclass;
   wndclass.style        =0;
   wndclass.lpfnWndProc  =wndproc;
   wndclass.cbClsExtra   =0;
   wndclass.cbWndExtra   =0;
   wndclass.hInstance    =hi;
   wndclass.hIcon        =NULL;
   wndclass.hCursor      =NULL;
   wndclass.hbrBackground=NULL;
   wndclass.lpszMenuName =NULL;
   wndclass.lpszClassName=classname;
   ATOM at=RegisterClass(&wndclass);
   if (atom) *atom=at;
  }

 return CreateWindow(classname,
                     windowname,
                     WS_OVERLAPPEDWINDOW,
                     CW_USEDEFAULT,
                     CW_USEDEFAULT,
                     CW_USEDEFAULT,
                     CW_USEDEFAULT,
                     NULL,
                     NULL,
                     hi,
                     lparam);
}

void getCLSIDname(const CLSID &clsid,char_t *buf,size_t buflen)
{
 LPOLESTR sclsidW;
 StringFromIID(clsid,&sclsidW);
 char_t reg[MAX_PATH];
 tsnprintf_s(reg, countof(reg), _TRUNCATE,_l("\\CLSID\\%s"),(const char_t*)text<char_t>(sclsidW));
 CoTaskMemFree(sclsidW);
 TregOpRegRead t(HKEY_CLASSES_ROOT,reg);
 t._REG_OP_S(0,_l(""),buf,buflen,_l(""));
}

const TmultipleInstances multipleInstances[]=
{
 0,_l("no limitations"),
 1,_l("only one - check previous filter only"),
 3,_l("only one - check all previous filters"),
 2,_l("only one - check all filters in graph"),
 4,_l("none - disabled"),
 0,NULL
};

template const char* stristr(const char *haystack,const char *needle);
template const char* strnstr(const char *haystack,size_t n,const char *needle);
template const char* strnistr(const char *haystack,size_t n,const char *needle);
template const char *strnchr(const char *s,size_t n,int c);
template char* strrmchar(char *s,int c);
template void strtok(const char *s,const char *delim,std::vector<DwString<char> > &lst,bool add_empty,size_t max_parts);
template void strtok(const char *s,const char *delim,std::vector<Tstrpart<char> > &lst,bool add_empty,size_t max_parts);
template void strtok(const char *s,const char *delim,ints &lst,bool add_empty,size_t max_parts);
template const void* memnstr(const void *haystachk,size_t n,const char *needle);

template const wchar_t* stristr(const wchar_t *haystack,const wchar_t *needle);
template const wchar_t* strnstr(const wchar_t *haystack,size_t n,const wchar_t *needle);
template const wchar_t* strnistr(const wchar_t *haystack,size_t n,const wchar_t *needle);
template const wchar_t *strnchr(const wchar_t *s,size_t n,int c);
template void strtok(const wchar_t *s,const wchar_t *delim,std::vector<DwString<wchar_t> > &lst,bool add_empty,size_t max_parts);
template void strtok(const wchar_t *s,const wchar_t *delim,std::vector<Tstrpart<wchar_t> > &lst,bool add_empty,size_t max_parts);
template void strtok(const wchar_t *s,const wchar_t *delim,ints &lst,bool add_empty,size_t max_parts);
template wchar_t* strrmchar(wchar_t *s,int c);
template const void* memnstr(const void *haystachk,size_t n,const wchar_t *needle);
