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
#include <locale.h>
#include "TfontSettings.h"
#include "ffdshow_constants.h"
#include "reg.h"

//=========================================== TfontSettings ==========================================
const TfontSettings::Tweigth TfontSettings::weights[]=
{
 _l("thin")      ,FW_THIN,
 _l("extralight"),FW_EXTRALIGHT,
 _l("light")     ,FW_LIGHT,
 _l("normal")    ,FW_NORMAL,
 _l("medium")    ,FW_MEDIUM,
 _l("semibold")  ,FW_SEMIBOLD,
 _l("bold")      ,FW_BOLD,
 _l("extrabold") ,FW_EXTRABOLD,
 _l("heavy")     ,FW_HEAVY,
 NULL
};

const char_t* TfontSettings::shadowModes[]=
{
 _l("Glowing shadow"),
 _l("Gradient shadow"),
 _l("Classic shadow"),
 _l("Disabled"),
 NULL
};

const int TfontSettings::charsets[]=
{
 ANSI_CHARSET,DEFAULT_CHARSET,SYMBOL_CHARSET,SHIFTJIS_CHARSET,HANGUL_CHARSET,GB2312_CHARSET,CHINESEBIG5_CHARSET,OEM_CHARSET,JOHAB_CHARSET,HEBREW_CHARSET,ARABIC_CHARSET,GREEK_CHARSET,TURKISH_CHARSET,VIETNAMESE_CHARSET,THAI_CHARSET,EASTEUROPE_CHARSET,RUSSIAN_CHARSET,MAC_CHARSET,BALTIC_CHARSET,-1
};
const char_t* TfontSettings::getCharset(int i)
{
 switch (i)
  {
   case ANSI_CHARSET       :return _l("ANSI");
   case DEFAULT_CHARSET    :return _l("Default");
   case SYMBOL_CHARSET     :return _l("Symbol");
   case SHIFTJIS_CHARSET   :return _l("ShiftJIS");
   case HANGUL_CHARSET     :return _l("Hangul");
   case GB2312_CHARSET     :return _l("GB2312");
   case CHINESEBIG5_CHARSET:return _l("Chinese");
   case OEM_CHARSET        :return _l("OEM");
   case JOHAB_CHARSET      :return _l("Johab");
   case HEBREW_CHARSET     :return _l("Hebrew");
   case ARABIC_CHARSET     :return _l("Arabic");
   case GREEK_CHARSET      :return _l("Greek");
   case TURKISH_CHARSET    :return _l("Turkish");
   case VIETNAMESE_CHARSET :return _l("Vietnamese");
   case THAI_CHARSET       :return _l("Thai");
   case EASTEUROPE_CHARSET :return _l("Easteurope");
   case RUSSIAN_CHARSET    :return _l("Cyrillic");
   case MAC_CHARSET        :return _l("Mac");
   case BALTIC_CHARSET     :return _l("Baltic");
   default                 :return _l("unknown");
  }
}
int TfontSettings::getCharset(const char_t *name)
{
 if      (stricmp(name,_l("Default"))==0) return DEFAULT_CHARSET;
 else if (stricmp(name,_l("Symbol"))==0) return SYMBOL_CHARSET;
 else if (stricmp(name,_l("Shiftjis"))==0) return SHIFTJIS_CHARSET;
 else if (stricmp(name,_l("Hangul"))==0) return HANGUL_CHARSET;
 else if (stricmp(name,_l("Gb2312"))==0) return GB2312_CHARSET;
 else if (stricmp(name,_l("Chinese"))==0) return CHINESEBIG5_CHARSET;
 else if (stricmp(name,_l("OEM"))==0) return OEM_CHARSET;
 else if (stricmp(name,_l("Johab"))==0) return JOHAB_CHARSET;
 else if (stricmp(name,_l("Hebrew"))==0) return HEBREW_CHARSET;
 else if (stricmp(name,_l("Arabic"))==0) return ARABIC_CHARSET;
 else if (stricmp(name,_l("Greek"))==0) return GREEK_CHARSET;
 else if (stricmp(name,_l("Turkish"))==0) return TURKISH_CHARSET;
 else if (stricmp(name,_l("Vietnamese"))==0) return VIETNAMESE_CHARSET;
 else if (stricmp(name,_l("Thai"))==0) return THAI_CHARSET;
 else if (stricmp(name,_l("Easteurope"))==0) return EASTEUROPE_CHARSET;
 else if (stricmp(name,_l("Russian"))==0 || stricmp(name,_l("Cyrillic"))==0) return RUSSIAN_CHARSET;
 else if (stricmp(name,_l("Mac"))==0) return MAC_CHARSET;
 else if (stricmp(name,_l("Baltic"))==0) return BALTIC_CHARSET;
 else return ANSI_CHARSET;
}

void TfontSettings::reg_op(TregOp &t)
{
 Toptions::reg_op(t);
}

TfontSettings::TfontSettings(TintStrColl *Icoll):Toptions(Icoll)
{
 memset(name,0,sizeof(name));
}

void TfontSettings::getDefaultStr(int id,char_t *buf,size_t buflen)
{
 if (id==IDFF_OSDfontName || id==IDFF_fontName)
  {
   LCID lcid = GetUserDefaultLCID();
   if (lcid == 1041 /* Japanese */)
    { // Arial does not have Japanese characters.
#ifdef UNICODE
     // I just want to avoid typing kanji directly for encoding reason. GCC does not compile UNICODE source file.
     wchar_t msgothic[]={0xff2d, 0xff33, 0x0020, 0xff30, 0x30b4, 0x30b7, 0x30c3, 0x30af, 0x0000}; // MS P Gothic
     strcpy(buf,msgothic);
#else
     unsigned char msgothic[]={0x82, 0x6c, 0x82, 0x72, 0x20, 0x82, 0x6f, 0x83, 0x53, 0x83, 0x56, 0x83, 0x62, 0x83, 0x4e, 0x00}; // MS P Gothic
     strcpy(buf,(const char*)msgothic);
#endif
    }
   else
    strcpy(buf,_l("Arial"));
  }
}

//========================================= TfontSettingsOSD =========================================
TfontSettingsOSD::TfontSettingsOSD(TintStrColl *Icoll):TfontSettings(Icoll)
{
 autosize=0;
 sizeA=50;
 split=0;
 autosizeVideoWindow=0;

 static const TintOptionT<TfontSettings> iopts[]=
  {
   IDFF_OSDfontCharset       ,&TfontSettings::charset        ,1,1,_l(""),0,
     _l("OSDfontCharset"), DEFAULT_CHARSET,
   IDFF_OSDfontSize          ,&TfontSettings::sizeP          ,2,255,_l(""),0,
     _l("OSDfontSize"), 17,
   IDFF_OSDfontWeight        ,&TfontSettings::weight         ,0,900,_l(""),0,
     _l("OSDfontWeight"), FW_NORMAL,
   IDFF_OSDfontColor         ,&TfontSettings::color          ,1,1,_l(""),0,
     _l("OSDfontColor"), RGB(110,220,0),
   IDFF_OSDfontOutlineColor  ,&TfontSettings::outlineColor   ,1,1,_l(""),1,
     _l("OSDfontOutlineColor"), RGB(0,0,0),
   IDFF_OSDfontShadowColor   ,&TfontSettings::shadowColor    ,1,1,_l(""),1,
     _l("OSDfontShadowColor"), RGB(0,0,0),
   IDFF_OSDfontOutlineWidth  ,&TfontSettings::outlineWidth   ,0,16,_l(""),1,
     _l("OSDfontOutlineWidth"), 1,
   IDFF_OSDfontOpaqueBox     ,&TfontSettings::opaqueBox      ,0,0,_l(""),1,
     _l("OSDfontOpaqueBox"), 0,
   IDFF_OSDfontSpacing       ,&TfontSettings::spacing        ,-10,10,_l(""),0,
     _l("OSDfontSpacing"), 0,
   IDFF_OSDfontXscale        ,&TfontSettings::xscale         ,10,300,_l(""),0,
     _l("OSDfontXscale"), 100,
   IDFF_OSDfontYscale        ,&TfontSettings::yscale         ,10,300,_l(""),0,
     _l("OSDfontYscale"), 100,
   IDFF_OSDfontAspectAuto    ,&TfontSettings::aspectAuto     ,0,0,_l(""),0,
     _l("OSDfontAspectAuto"), 1,
   IDFF_OSDfontFast          ,&TfontSettings::fast           ,0,0,_l(""),0,
     _l("OSDfontFast"), 0,
   IDFF_OSDfontBodyAlpha     ,&TfontSettings::bodyAlpha      ,0,256,_l(""),1,
     _l("OSDfontBodyAlpha"), 256,
   IDFF_OSDfontOutlineAlpha  ,&TfontSettings::outlineAlpha   ,0,256,_l(""),1,
     _l("OSDfontOutlineAlpha"), 256,
   IDFF_OSDfontShadowAlpha   ,&TfontSettings::shadowAlpha    ,0,256,_l(""),1,
     _l("OSDfontShadowAlpha"), 128,
   IDFF_OSDfontShadowSize    ,&TfontSettings::shadowSize     ,0,50,_l(""),1,
     _l("OSDfontShadowSize"), 8,
   IDFF_OSDfontShadowMode    ,&TfontSettings::shadowMode     ,0,3,_l(""),1,
     _l("OSDfontShadowMode"), 3,
   IDFF_OSDfontBlur          ,&TfontSettings::blur           ,0,0,_l(""),1,
     _l("OSDfontBlur"), 0,
   0
  };
 addOptions(iopts);
 static const TstrOption sopts[]=
  {
   IDFF_OSDfontName,(TstrVal)&TfontSettings::name,LF_FACESIZE,0,_l(""),0,
     _l("OSDfontName"), NULL,
   0
  };
 addOptions(sopts);
 static const TcreateParamList2<TfontSettings::Tweigth> listOSDfontWeigth(weights,&Tweigth::name);setParamList(IDFF_OSDfontWeight,&listOSDfontWeigth);
}

//========================================= TfontSettingsSub =========================================
TfontSettingsSub::TfontSettingsSub(TintStrColl *Icoll):TfontSettings(Icoll)
{
 static const Toptions::TintOptionT<TfontSettings> iopts[]=
  {
   IDFF_fontCharset            ,&TfontSettings::charset            ,1,1,_l(""),1,
     _l("fontCharset"), DEFAULT_CHARSET,
   IDFF_fontAutosize           ,&TfontSettings::autosize           ,0,0,_l(""),1,
     _l("fontAutosize"), 0,
   IDFF_fontAutosizeVideoWindow,&TfontSettings::autosizeVideoWindow,0,0,_l(""),1,
     _l("fontAutosizeVideoWindow"), 0,
   IDFF_fontSizeP              ,&TfontSettings::sizeP              ,2,255,_l(""),1,
     _l("fontSize"), 26,
   IDFF_fontSizeA              ,&TfontSettings::sizeA              ,1,100,_l(""),1,
     _l("fontSizeA"), 60,
   IDFF_fontWeight             ,&TfontSettings::weight             ,0,900,_l(""),1,
     _l("fontWeight"), FW_BOLD,
   IDFF_fontColor              ,&TfontSettings::color              ,1,1,_l(""),1,
     _l("fontColor"), RGB(255,255,255),
   IDFF_fontOutlineColor       ,&TfontSettings::outlineColor       ,1,1,_l(""),1,
     _l("fontOutlineColor"), RGB(0,0,0),
   IDFF_fontShadowColor        ,&TfontSettings::shadowColor        ,1,1,_l(""),1,
     _l("fontShadowColor"), RGB(0,0,0),
   IDFF_fontOutlineWidth       ,&TfontSettings::outlineWidth       ,0,16,_l(""),1,
     _l("fontOutlineWidth"), 2,
   IDFF_fontOpaqueBox          ,&TfontSettings::opaqueBox          ,0,0,_l(""),1,
     _l("fontOpaqueBox"), 0,
   IDFF_fontSpacing            ,&TfontSettings::spacing            ,-10,10,_l(""),1,
     _l("fontSpacing"), 0,
   IDFF_fontSplitting          ,&TfontSettings::split              ,0,0,_l(""),1,
     _l("fontSplitting"), 1,
   IDFF_fontXscale             ,&TfontSettings::xscale             ,10,300,_l(""),1,
     _l("fontXscale"), 100,
   IDFF_fontYscale             ,&TfontSettings::yscale             ,10,300,_l(""),1,
     _l("fontYscale"), 100,
   IDFF_fontOverrideScale      ,&TfontSettings::overrideScale      ,0,0,_l(""),1,
     _l("fontOverrideScale"), 0,
   IDFF_fontAspectAuto         ,&TfontSettings::aspectAuto         ,0,0,_l(""),1,
     _l("fontAspectAuto"), 1,
   IDFF_fontFast               ,&TfontSettings::fast               ,0,0,_l(""),1,
     _l("fontFast"), 0,
   IDFF_fontBodyAlpha          ,&TfontSettings::bodyAlpha          ,0,256,_l(""),1,
     _l("fontBodyAlpha"), 256,
   IDFF_fontOutlineAlpha       ,&TfontSettings::outlineAlpha       ,0,256,_l(""),1,
     _l("fontOutlineAlpha"), 256,
   IDFF_fontShadowAlpha        ,&TfontSettings::shadowAlpha        ,0,256,_l(""),1,
     _l("fontShadowAlpha"), 128,
   IDFF_fontShadowSize          ,&TfontSettings::shadowSize        ,0,50,_l(""),1,
     _l("fontShadowSize"), 8,
   IDFF_fontShadowMode         ,&TfontSettings::shadowMode         ,0,3,_l(""),1,
     _l("fontShadowMode"), 1, // Default shadow mode to gradient (best rendering)
   IDFF_fontBlur               ,&TfontSettings::blur               ,0,0,_l(""),1,
     _l("fontBlur"), 0,
   0
  };
 addOptions(iopts);
 static const Toptions::TstrOption sopts[]=
  {
   IDFF_fontName               ,(TstrVal)&TfontSettings::name      ,LF_FACESIZE,0,_l(""),1,
     _l("fontName"), NULL,
   0
  };
 addOptions(sopts);

 static const TcreateParamList2<TfontSettings::Tweigth> listFontWeigth(weights,&Tweigth::name);setParamList(IDFF_fontWeight,&listFontWeigth);
}

void TfontSettingsSub::reg_op(TregOp &t)
{
 TfontSettings::reg_op(t);
 if (weight>900) weight=900;
}
