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
#include "Ttranslate.h"
#include "Tconfig.h"
#include "inifiles.h"
#include "resource.h"
#include "TfontSettings.h"
#include "Tstream.h"

//====================== Ttranslate::Tfont ==========================
Ttranslate::Tfont::Tfont(void)
{
 typeface[0]='\0';
 pointsize=pointsizeDef;
 italic=italicDef;
 horizontalScale=horizontalScaleDef;
}
void Ttranslate::Tfont::init(Tinifile &ini)
{
 ini.getPrivateProfileString(_l("Font"),Tconfig::winNT?_l("typefaceNT"):_l("typeface9x"),_l(""),typeface,256);
 pointsize=ini.getPrivateProfileInt(_l("Font"),Tconfig::winNT?_l("pointsizeNT"):_l("pointsize9x"),pointsizeDef);
 italic=ini.getPrivateProfileInt(_l("Font"),Tconfig::winNT?_l("italicNT"):_l("italic9x"),italicDef)?TRUE:FALSE;
 lastHorizontalScale=horizontalScale=ini.getPrivateProfileInt(_l("Font"),Tconfig::winNT?_l("horizontalScaleNT"):_l("horizontalScale9x"),horizontalScaleDef);
/*
 char charsetS[256];
 ini.getPrivateProfileString("Font",Tconfig::winNT?"charsetNT":"charset9x","Ansi",charsetS,256);
 charset=TfontSettings::getCharset(charsetS);
*/
}

//====================== Ttranslate::Tstrs ==========================
void Ttranslate::Tstrs::init(Tinifile &ini)
{
 char_t* vals=new char_t[65536 * 4];
 ini.getPrivateProfileSection(_l("-1"),vals,65536 * 4);
 for (char_t *val=vals;*val;val=strchr(strchr(val,'\0')+1,'\0')+1)
  {
   char_t *eq=strchr(val,'=');
   if (!eq) continue;
   eq[0]='\0';
   (*this)[eolnFile2string(val)]=eolnFile2string(eq+1);
  }
 delete vals;
}
const char_t* Ttranslate::Tstrs::translate(const char_t *s) const
{
 const_iterator i=find(s);
 return (i==end())?s:i->second.c_str();
}
void Ttranslate::Tstrs::newTranslation(const ffstring &oldstring,const ffstring &newstring,const char_t *flnm,int encoding)
{
 (*this)[oldstring]=newstring;
 Tinifile ini(flnm,encoding);
 ini.writePrivateProfileString(_l("-1"),eolnString2file(oldstring).c_str(),eolnString2file(newstring).c_str());
}

//===================== Ttranslate::Tdialog =========================
Ttranslate::Tdialog::Tdialog(Tinifile &ini,const char_t *sect)
{
 int dlgId=atoi(sect);
 char_t vals[32768];
 ini.getPrivateProfileSection(sect,vals,32767);
 for (char_t *val=vals;*val;val=strchr(val,'\0')+1)
  {
   char_t *eq=strchr(val,'=');
   if (!eq) continue;
   *eq='\0';
   (*this)[atoi(val)]=eolnFile2string(eq+1);
   val=eq+1;
  }
}
BOOL CALLBACK Ttranslate::Tdialog::transDialogEnum(HWND hwnd,LPARAM lParam)
{
 Tdialog *self=(Tdialog*)lParam;
 char_t className[256];
 GetClassName(hwnd,className,256);
 if (stricmp(className,_l("BUTTON"))==0 || stricmp(className,_l("STATIC"))==0)
  {
   char_t s[256]=_l("");
   GetWindowText(hwnd,s,255);
   int id=GetDlgCtrlID(hwnd);
   (*self)[id]=s;
  }
 return TRUE;
}
Ttranslate::Tdialog::Tdialog(HINSTANCE hi,HWND hwnd)
{
 if (hwnd)
  EnumChildWindows(hwnd,transDialogEnum,LPARAM(this));
}

void Ttranslate::Tdialog::translate(int dlgId,HWND hwnd,Tdialog &builtin)
{
 //createTranslationTemplate(hwnd,dlgId);
 THWNDs wnds;
 getChildWindows(hwnd,wnds);
 for (THWNDs::const_iterator w=wnds.begin();w!=wnds.end();w++)
  {
   LONG id=GetWindowLong(*w,GWL_ID);
   if (id>0)
    {
     char_t className[256];
     GetClassName(*w,className,256);
     if (id==IDC_ED_HELP || stricmp(className,_l("BUTTON"))==0 || stricmp(className,_l("STATIC"))==0)
      {
       const char_t *newtext=translate(dlgId,id,NULL,&builtin);
       if (newtext && newtext[0])
        {
         SetDlgItemText(hwnd,id,newtext);
         int exstyle=GetWindowExStyle(*w);
         if (exstyle&WS_EX_ACCEPTFILES)
          {
           HDC dc=GetDC(*w);
           HFONT hf=(HFONT)SendMessage(*w,WM_GETFONT,0,0);
           HFONT oldfont=(HFONT)SelectObject(dc,hf);
           SIZE sz;
           GetTextExtentPoint32(dc,newtext,(int)strlen(newtext),&sz);
           RECT r;
           GetWindowRect(*w,&r);
           SetWindowPos(*w,NULL,0,0,sz.cx+(r.bottom-r.top+1)+2,r.bottom-r.top+1,SWP_NOMOVE|SWP_NOZORDER);
           SelectObject(dc,oldfont);
           ReleaseDC(*w,dc);
          }
        }
      }
    }
  }
}
const char_t* Ttranslate::Tdialog::translate(int dlgId,int item,const char_t *def,Tdialog *builtin)
{
 const_iterator i=find(item);
 if (i==end())
  if (builtin)
   return builtin->translate(dlgId,item,def,NULL);
  else
   return def;
 else
  return i->second.c_str();
}

void Ttranslate::Tdialog::createTranslationTemplate(HWND hwnd,int dlgId)
{
 Tinifile ini(_l("./ffdshow_template"));
 THWNDs wnds;getChildWindows(hwnd,wnds);
 char_t dlgIdS[30];_itoa(dlgId,dlgIdS,10);
 for (THWNDs::const_iterator w=wnds.begin();w!=wnds.end();w++)
  {
   int id=GetWindowLong(*w,GWL_ID);
   char_t classname[256];
   GetClassName(*w,classname,256);
   if (stricmp(classname,_l("BUTTON"))==0 || stricmp(classname,_l("STATIC"))==0)
    {
     char_t caption[256];
     GetWindowText(*w,caption,256);
     if (caption[0])
      {
       char_t ctrlIdS[30];
       ini.writePrivateProfileString(dlgIdS,_itoa(id,ctrlIdS,10),caption);
      }
    }
  }
}

void Ttranslate::Tdialog::newTranslation(int dlgId,int item,const ffstring &newtrans,const char_t *flnm,int encoding)
{
 (*this)[item]=newtrans;
 char_t dialogIdS[20],itemS[20];
 Tinifile ini(flnm,encoding);
 ini.writePrivateProfileString(_itoa(dlgId,dialogIdS,10),_itoa(item,itemS,10),eolnString2file(newtrans).c_str());
}

//===================== Ttranslate::Tlanglist =======================
Ttranslate::Tlanglist::Tlanglist(const char_t *Ipth):pth(Ipth)
{
 char_t lngMask[MAX_PATH];
 _makepath_s(lngMask, countof(lngMask), NULL, pth, _l("languages\\ffdshow*.*"), _l("*"));
 strings files;
 findFiles(lngMask,files,false);
 for (strings::iterator i=files.begin();i!=files.end();i++)
  {
   char_t ext[MAX_PATH];extractfileext(i->c_str(),ext);strlwr(ext);
   char_t name[MAX_PATH];extractfilenameWOext(i->c_str(),name);
   char_t lang[MAX_PATH];extractfileext(name,lang);
   if (lang[0]!='\0')
    *strchr(name,'.')='\0';
   insert(std::make_pair((const char_t*)ext,(const char_t*)lang));
  }
}
void Ttranslate::Tlanglist::getLangName(const_iterator &lang,char_t *buf,size_t buflen)
{
 char_t ext[MAX_PATH];
 if (!lang->second)
  ff_strncpy(ext, lang->first.c_str(), countof(ext));
 else
  tsnprintf_s(ext, countof(ext), _TRUNCATE, _l("%s.%s"), lang->second.c_str(), lang->first.c_str());
 char_t flnm[MAX_PATH];
 _makepath_s(flnm, countof(flnm), NULL, pth, _l("languages\\ffdshow"), ext);
 TstreamFile f(flnm,false,false);
 buf[0]='\0';
 if (f)
  {
   char_t line[256];f.fgets(line,256);
   if (line[0]!='[')
    {
     ff_strncpy(buf,line,buflen);
     buf[buflen-1]='\0';
     char_t *eoln=strchr(buf,'\n');
     if (eoln) *eoln='\0';
    }
  }
 if (buf[0]=='\0') ff_strncpy(buf,lang->first.c_str(),buflen);
 //if (f) fclose(f);
}

//=========================== Ttranslate =================================
Ttranslate::Ttranslate(HINSTANCE Ihi,const char_t *Ipth):hi(Ihi),pth(Ipth),langlist(Ipth),refcount(0),initfont(true)
{
 curlang[0]='\0';
 translateMode=0;
 translateEncoding=Tstream::ENC_ASCII;
}

void Ttranslate::init(const char_t *newlang,int ItranslateMode)
{
 if (strcmp(curlang,newlang)==0) return;
 ff_strncpy(curlang, newlang ? newlang : _l(""), countof(curlang));
 done();
 if (curlang[0]=='\0') return;
 Tlanglist::const_iterator langId=langlist.find(newlang);
 _makepath_s(curflnm, countof(curflnm), NULL, pth, _l("languages\\ffdshow"),
   langId==langlist.end() || !langId->second ?
     curlang :
     ffstring(langId->second+_l(".")+ffstring(curlang)).c_str());
 char_t sects[10240];
 Tinifile ini(curflnm);
 ini.getPrivateProfileSectionNames(sects, countof(sects));
 for (char_t *sect=sects;*sect;sect=strchr(sect,'\0')+1)
  if (strcmp(sect,_l("-1"))==0)
   strs.init(ini);
  else if (initfont && strnicmp(sect,_l("font"),4)==0)
   {
    font.init(ini);
    initfont=false;
   }
  else
   {
    int dlgId=atoi(sect);
    if (dlgId) dlgs[dlgId]=Tdialog(ini,sect);
   }
 translateMode=ItranslateMode;
 translateEncoding=ini.encoding;
}
void Ttranslate::done(void)
{
 dlgs.clear();
 strs.clear();
}

void Ttranslate::translate(HWND hwnd,int dlgId)
{
 if (dlgsBuiltin[dlgId].empty())
  dlgsBuiltin[dlgId]=Tdialog(hi,hwnd);
 dlgs[dlgId].translate(dlgId,hwnd,dlgsBuiltin[dlgId]);
}

const char_t* Ttranslate::translate(HWND hwnd,int dlgId,int item,const char_t *def) const
{
 if (dlgsBuiltin[dlgId].empty())
  dlgsBuiltin[dlgId]=Tdialog(hi,hwnd);
 return dlgs[dlgId].translate(dlgId,item,def,&dlgsBuiltin[dlgId]);
}

const char_t* Ttranslate::translate(int dlgId)
{
again:
 const char_t *res=dlgs[dlgId].translate(dlgId,0,NULL,NULL);
 if (!res)
  {
   char_t dlgname[1024];
   LoadString(hi,dlgId,dlgname,(int)countof(dlgname));
   dlgs[dlgId][0]=dlgname;
   goto again;
  }
 return res;
}

bool Ttranslate::isCustomFont(void)
{
 initfont=false;
 return font.typeface[0]!='\0' || font.pointsize!=font.pointsizeDef || font.italic!=font.italicDef;
}
DLGTEMPLATE *Ttranslate::translateDialogTemplate(int dialogId) const
{
 HRSRC dlgrsr=FindResource(hi,MAKEINTRESOURCE(dialogId),RT_DIALOG);
 HGLOBAL dlgglb=LoadResource(hi,dlgrsr);
 const WCHAR *orig=(WCHAR*)LockResource(dlgglb);
 WCHAR *trans0=(WCHAR*)LocalAlloc(LPTR,300000),*p=trans0;
 *p++=*orig++; // dlgver
 WCHAR signature=*p++=*orig++; // signature
 if (signature==0xffff)
  {
   *p++=*orig++; // LOWORD (helpId)
   *p++=*orig++; // HIWORD (helpId)
   *p++=*orig++; // LOWORD (lExtendedStyle);
   *p++=*orig++; // HIWORD (lExtendedStyle);
   DWORD style=*(DWORD*)orig;
   *p++=*orig++; // LOWORD (lStyle);
   *p++=*orig++; // HIWORD (lStyle);
   int numitems=*p++=*orig++; // NumberOfItems
   *p++=*orig++; // x
   *p++=*orig++; // y
   *p++=*orig++; // cx
   *p++=*orig++; // cy
   *p++=*orig++; // Menu
   *p++=*orig++; // Class
   while ((*p++=*orig++)!=NULL)
    ;
   if (style&(DS_SETFONT|DS_SHELLFONT))
    {
     *p++=WCHAR(font.pointsize);orig++; // pointsize
     *p++=*orig++; // weight
     BYTE *pb=(BYTE*)p,*origb=(BYTE*)orig;
     pb[0]=font.italic; // italic,
     pb[1]=origb[1];//font.charset; // charset
     p++;orig++;
     if (!font.typeface[0])
      while ((*p++=*orig++)!=NULL)
       ;
     else
      {
       int nchar=nCopyAnsiToWideChar(p,text<char_t>(font.typeface));
       p+=nchar;
       while (*orig++)
        ;
      }
    }
   for (int i=0;i<numitems;i++)
    {
     p=lpwAlign(p);orig=lpwAlign(orig);
     *p++=*orig++; // LOWORD(DWORD helpID);
     *p++=*orig++; // HIWORD(DWORD helpID);
     *p++=*orig++; // LOWORD(DWORD exStyle);
     *p++=*orig++; // HIWORD(DWORD exStyle);
     *p++=*orig++; // LOWORD(DWORD style);
     *p++=*orig++; // HIWORD(DWORD style);
     *p++=*orig++; // short x;
     *p++=*orig++; // short y;
     *p++=*orig++; // short cx;
     *p++=*orig++; // short cy;
     *p++=*orig++; // WORD id;
     WCHAR windowclass=*p++=*orig++; // sz_Or_Ord windowClass;
     if (windowclass!=0xffff)
      for (;(windowclass=*p++=*orig++)!=NULL;)
       ;
     else
      *p++=*orig++;
     WCHAR title=*p++=*orig++; // sz_Or_Ord title;
     if (title!=0xffff)
      for (;(title=*p++=*orig++)!=NULL;)
       ;
     else
      *p++=*orig++;
     int extracount=*p++=*orig++; // WORD extraCount;
     if (extracount>0)
      {
       p=lpwAlign(p);orig=lpwAlign(orig);
       BYTE *pb=(BYTE*)p,*origb=(BYTE*)orig;
       for (int j=0;j<extracount;j++)
        pb[j]=origb[j];
       p=lpwAlign(p+roundDiv(extracount,2));orig=lpwAlign(orig+roundDiv(extracount,2));
      }
    }
  }
 return (DLGTEMPLATE*)trans0;
}

int Ttranslate::lastHorizontalScale=Tfont::horizontalScaleDef;
