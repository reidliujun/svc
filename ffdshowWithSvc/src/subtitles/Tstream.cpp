/*
 * Copyright (c) 2004-2006 Milan Cutka
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
#include "Tstream.h"
#include "Tdll.h"
#include "../unrar/dll.hpp"

//====================================== Tstream ======================================
void Tstream::detectUnicode(void)
{
 encoding=ENC_ASCII;unicode_offset=0;
 if (length()>=2)
  {
   long pos=tell();
   WORD w;
   size_t readed=read(&w,1,2);
   if (w==0xfeff)
    {
     encoding=ENC_LE16;
     unicode_offset=2;
    }
   else if (w==0xfffe)
    {
     encoding=ENC_BE16;
     unicode_offset=2;
    }
   else if (w==0xbbef && length()>=3)
    {
     BYTE b;
     if (read(&b,1,1)==1 && b==0xbf)
      {
       encoding=ENC_UTF8;
       unicode_offset=3;
      }
    }
   seek(pos,SEEK_SET);
  }
}

char* Tstream::ugets(char *buf0,int len) const
{
 char *buf=buf0;
 bool eof=true,wasr=false;
 switch (encoding)
  {
   case ENC_UTF8:
    {
     wchar_t unicodeBuf[MAX_SUBTITLE_LENGTH+1];
     char srcBuf[MAX_SUBTITLE_LENGTH*2+1];
     unicodeBuf[MAX_SUBTITLE_LENGTH]=0;srcBuf[MAX_SUBTITLE_LENGTH]=0;

     // read a line
     int count=0;
     char u1,u2,u3,u4;
     while (count<=MAX_SUBTITLE_LENGTH-3 && read(&u1,1,sizeof(u1))==sizeof(u1))
      {
       eof=false;
       if(!(u1&0x80)) // 0xxxxxxx
        srcBuf[count++]=u1;
       else if ((u1&0xe0)==0xc0) // 110xxxxx 10xxxxxx
        {
         srcBuf[count++]=u1;
         if (read(&u2,1,sizeof(u2))==sizeof(u2))
          srcBuf[count++]=u2;
         else
          break;
        }
       else if ((u1&0xf0)==0xe0) // 1110xxxx 10xxxxxx 10xxxxxx
        {
         srcBuf[count++]=u1;
         if (read(&u2,1,sizeof(u2))==sizeof(u2))
          srcBuf[count++]=u2;
         else
          break;
         if (read(&u3,1,sizeof(u3))==sizeof(u3))
          srcBuf[count++]=u3;
         else
          break;
        }
       else if ((u1&0xf8)==0xf0) // 1111xxxx 10xxxxxx 10xxxxxx 10xxxxxx
        {
         srcBuf[count++]=u1;
         if (read(&u2,1,sizeof(u2))==sizeof(u2))
          srcBuf[count++]=u2;
         else
          break;
         if (read(&u3,1,sizeof(u3))==sizeof(u3))
          srcBuf[count++]=u3;
         else
          break;
         if (read(&u4,1,sizeof(u4))==sizeof(u4))
          srcBuf[count++]=u4;
         else
          break;
        }
       if (u1=='\r')
        {
         if (!crln)
          count--;
         wasr=true;
         continue;
        }
       if (u1=='\n')
        {
         if (utod &&!wasr)
          srcBuf[--count]='\r';
         break;
        }
      }
     if (eof) break;
     srcBuf[count]=0;
     int unicodeLen=MultiByteToWideChar(CP_UTF8,0,srcBuf,-1,unicodeBuf,MAX_SUBTITLE_LENGTH*2);
     int ansiLen=WideCharToMultiByte(CP_ACP,0,unicodeBuf,unicodeLen,srcBuf,MAX_SUBTITLE_LENGTH*2,NULL,NULL);
     memcpy(buf0,srcBuf,ansiLen<len ? ansiLen : len);
     break;
    }
   case ENC_LE16:
    {
     WCHAR w;
     while (read(&w,1,sizeof(w))==sizeof(w))
      {
       eof=false;
       if (buf-buf0>len) break;
       char c='?';
       if (!(w&0xff00))
        c=char(w&0xff);
       else
        {
         char cA[10];
         c=*unicode16toAnsi(&w,1,cA);
        }
       if (c=='\r') {if (crln) *buf++=c;wasr=true;continue;}
       if (c=='\n') {if (utod &&!wasr) *buf++='\r';if (crln) *buf++=c;break;}
       *buf++=c;
      }
     *buf='\0';
     break;
    }
   case ENC_BE16:
    {
     WCHAR w;
     while (read(&w,1,sizeof(w))==sizeof(w))
      {
       eof=false;
       char c='?';
       if (!(w&0xff))
        c=char(w>>8);
       else
        {
         bswap(w);
         char cA[10];
         c=*unicode16toAnsi(&w,1,cA);
        }
       if (c=='\r') {if (crln) *buf++=c;wasr=true;continue;}
       if (c=='\n') {if (utod &&!wasr) *buf++='\r';if (crln) *buf++=c;break;}
       *buf++=c;
      }
     *buf='\0';
     break;
    }
  }
 return eof?NULL:buf0;
}
int Tstream::ugetc(void) const
{
 return EOF;
}

wchar_t* Tstream::fgets(wchar_t *buf0,int len) const
{
 wchar_t *buf=buf0;
 bool eof=true,wasr=false;
 switch (encoding)
  {
   case ENC_ASCII:
    {
     char w;
     while (read(&w,1,sizeof(w))==sizeof(w))
      {
       eof=false;
       if (buf-buf0>len) break;
       text<wchar_t>(&w, 1, buf, 1);//nCopyAnsiToWideChar(buf,&w,1);
       if (w=='\r') {if (crln) *buf++=w;wasr=true;continue;}
       if (w=='\n') {if (utod &&!wasr) *buf++='\r';if (crln) *buf++=w;break;}
       buf++;//*buf++=w;
      }
     *buf='\0';
     break;
    }
   case ENC_LE16:
   case ENC_BE16:
    {
     wchar_t w;
     while (read(&w,1,sizeof(w))==sizeof(w))
      {
       if (encoding==ENC_BE16) bswap(w);
       eof=false;
       if (buf-buf0>len) break;
       if (w=='\r') {if (crln) *buf++=w;wasr=true;continue;}
       if (w=='\n') {if (utod &&!wasr) *buf++='\r';if (crln) *buf++=w;break;}
       *buf++=w;
      }
     *buf='\0';
     break;
    }
   case ENC_UTF8:
    {
     wchar_t unicodeBuf[MAX_SUBTITLE_LENGTH+1];
     char srcBuf[MAX_SUBTITLE_LENGTH+1];
     unicodeBuf[MAX_SUBTITLE_LENGTH]=0;srcBuf[MAX_SUBTITLE_LENGTH]=0;

     // read a line
     int count=0;
     char u1,u2,u3,u4;
     while (count<=MAX_SUBTITLE_LENGTH-3 && read(&u1,1,sizeof(u1))==sizeof(u1))
      {
       eof=false;
       if(!(u1&0x80)) // 0xxxxxxx
        srcBuf[count++]=u1;
       else if ((u1&0xe0)==0xc0) // 110xxxxx 10xxxxxx
        {
         srcBuf[count++]=u1;
         if (read(&u2,1,sizeof(u2))==sizeof(u2))
          srcBuf[count++]=u2;
         else
          break;
        }
       else if ((u1&0xf0)==0xe0) // 1110xxxx 10xxxxxx 10xxxxxx
        {
         srcBuf[count++]=u1;
         if (read(&u2,1,sizeof(u2))==sizeof(u2))
          srcBuf[count++]=u2;
         else
          break;
         if (read(&u3,1,sizeof(u3))==sizeof(u3))
          srcBuf[count++]=u3;
         else
          break;
        }
       else if ((u1&0xf8)==0xf0) // 1111xxxx 10xxxxxx 10xxxxxx 10xxxxxx
        {
         srcBuf[count++]=u1;
         if (read(&u2,1,sizeof(u2))==sizeof(u2))
          srcBuf[count++]=u2;
         else
          break;
         if (read(&u3,1,sizeof(u3))==sizeof(u3))
          srcBuf[count++]=u3;
         else
          break;
         if (read(&u4,1,sizeof(u4))==sizeof(u4))
          srcBuf[count++]=u4;
         else
          break;
        }
       if (u1=='\r')
        {
         if (!crln)
          count--;
         wasr=true;
         continue;
        }
       if (u1=='\n')
        {
         if (utod &&!wasr)
          srcBuf[--count]='\r';
         break;
        }
      }
     if (eof) break;
     srcBuf[count]=0;
     int unicodeLen=MultiByteToWideChar(CP_UTF8,0,srcBuf,-1,unicodeBuf,MAX_SUBTITLE_LENGTH);
     ff_strncpy(buf0,unicodeBuf,unicodeLen<len ? unicodeLen : len);
     break;
    }
  }
 return eof?NULL:buf0;
}

template<class tchar> int Tstream::printf(const tchar *fmt,...)
{
 va_list valist;
 va_start(valist, fmt);
 tchar buf[1024];
 int len=_vsnprintf(buf,1023,fmt,valist); //_vsctprintf
 if (len>0)
  fputs(buf);
 va_end(valist);
 return len;
}
template int Tstream::printf(const char* fmt,...);
template int Tstream::printf(const wchar_t* fmt,...);

//=================================== TstreamFile =====================================
TstreamFile::TstreamFile(const char_t *flnm,bool binary,bool write,ENCODING Iencoding):ownf(true),Tstream(Iencoding)
{
 static const char_t *modes[]={_l("rt"),_l("rb"),_l("wt"),_l("wb")};
 f=fopen(flnm,modes[(binary || (write && encoding!=ENC_ASCII)?1:0)+(write?2:0)]);
 if (!f) return;
 if (encoding==ENC_AUTODETECT && !write && !binary)
  detectUnicode();
 else if (write && !binary)
  switch (encoding)
   {
    case ENC_LE16:
     {
      static const uint8_t marker[]={0xff,0xfe};
      fwrite(marker,1,2,f);
      break;
     }
    case ENC_UTF8:
     {
      static const uint8_t marker[]={0xbf,0xbb,0xef};
      fwrite(marker,1,3,f);
      break;
     }
   }
 else
  encoding=ENC_ASCII;
}

int TstreamFile::fputs(const char *buf)
{
 if (encoding==ENC_LE16)
  return fputs(text<wchar_t>(buf));
 else
  return ::fputs(buf,f);
}
wint_t TstreamFile::fputs(const wchar_t *buf)
{
 if (encoding==ENC_ASCII)
  return (wint_t)fputs(text<char>(buf));
 else
  {
   while (*buf)
    {
     if (*buf=='\n')
      {
       static const wchar_t d=0x0d;
       fwrite(&d,1,2,f);
      }
     fwrite(buf,1,2,f);
     buf++;
    }
   return 1;
  }
}

//==================================== TstreamMem =====================================
char* TstreamMem::fgets0(char *buf,int len) const
{
 int c=0;
 char *cs=buf;
 bool wasr=false;
 while (len-->0 && (c=getc())!=EOF)
  {
   if (c=='\r') {if (crln) *cs++=(char)c;wasr=true;continue;}
   if (c=='\n') {if (utod &&!wasr) *cs++='\r';if (crln) *cs++=(char)c;break;}
   *cs++=(char)c;
  }
 if (c==EOF && cs==buf)
  return NULL;
 *cs='\0';
 return buf;
}

//==================================== TstreamRAR =====================================
int CALLBACK TstreamRAR::callback(UINT msg,LONG_PTR UserData,LONG_PTR P1,LONG P2)
{
 if (msg==UCM_PROCESSDATA)
  {
   TstreamRAR *self=(TstreamRAR*)UserData;
   memcpy(self->buf+self->bufpos,(void*)P1,P2);
   self->bufpos+=P2;
  }
 return 1;
}

const char_t* TstreamRAR::dllname=_l("ff_unrar.dll");
TstreamRAR::TstreamRAR(const char_t *rarflnm,const char_t *flnm,const Tconfig *config)
{
 buf=NULL;bufpos=NULL;
 Tdll *unrar=new Tdll(_l("unrar.dll"),config);
 if (!unrar->ok)
  {
   delete unrar;
   unrar=new Tdll(dllname,config);
  }
 typedef HANDLE (PASCAL *RAROpenArchiveEx)(struct RAROpenArchiveDataEx *ArchiveData);
 typedef int    (PASCAL* RARCloseArchive)(HANDLE hArcData);
 typedef int    (PASCAL* RARReadHeaderEx)(HANDLE hArcData,struct RARHeaderDataEx *HeaderData);
 typedef int    (PASCAL* RARProcessFile)(HANDLE hArcData,int Operation,char *DestPath,char *DestName);
 typedef void   (PASCAL* RARSetCallback64)(HANDLE hArcData,UNRARCALLBACK64 Callback,LONG_PTR UserData);
 RAROpenArchiveEx OpenArchiveEx;unrar->loadFunction(OpenArchiveEx,"RAROpenArchiveEx");
 RARCloseArchive CloseArchive;unrar->loadFunction(CloseArchive, "RARCloseArchive");
 RARReadHeaderEx ReadHeaderEx;unrar->loadFunction(ReadHeaderEx, "RARReadHeaderEx");
 RARProcessFile ProcessFile;unrar->loadFunction(ProcessFile, "RARProcessFile");
 RARSetCallback64 SetCallback64;unrar->loadFunction(SetCallback64, "RARSetCallback");
 if (!unrar->ok)
  {
   delete unrar;
   return;
  }
 RAROpenArchiveDataEx ArchiveDataEx;
 memset(&ArchiveDataEx,0,sizeof(ArchiveDataEx));
 #ifdef UNICODE
 ArchiveDataEx.ArcNameW=rarflnm;
 #else
 ArchiveDataEx.ArcName=rarflnm;
 #endif
 ArchiveDataEx.OpenMode=RAR_OM_EXTRACT;
 ArchiveDataEx.CmtBuf=0;
 HANDLE hrar=OpenArchiveEx(&ArchiveDataEx);
 if(!hrar)
  {
   delete unrar;
   return;
  }
 SetCallback64(hrar,callback,LONG_PTR(this));
 RARHeaderDataEx HeaderDataEx;
 HeaderDataEx.CmtBuf=NULL;
 char_t ext[MAX_PATH];
 _splitpath_s(flnm,NULL,0,NULL,0,NULL,0,ext,MAX_PATH);
 char_t extpattern[MAX_PATH];
 _makepath_s(extpattern,MAX_PATH,NULL,NULL,_l("*"),ext);
 size_t bufsize=0;
 while(ReadHeaderEx(hrar, &HeaderDataEx) == 0)
  {
   const char_t *FileName;
   #ifdef UNICODE
    FileName=HeaderDataEx.FileNameW;
   #else
    FileName=HeaderDataEx.FileName;
   #endif
   if (stricmp(FileName,flnm)==0 || fnmatch(extpattern,FileName))
    {
     buf=(unsigned char*)malloc(bufsize=HeaderDataEx.UnpSize);
     if (ProcessFile(hrar,RAR_TEST,NULL,NULL)!=0)
      {
       free(buf);buf=NULL;
       CloseArchive(hrar);
       delete unrar;
       return;
      }
     bufpos=0;
     break;
    }
   ProcessFile(hrar,RAR_SKIP,NULL,NULL);
  }
 CloseArchive(hrar);
 delete unrar;
 setMem(buf,bufsize);
}
