/*
 * Copyright (c) 2003-2006 Milan Cutka
 * subtitles fixing code from SubRip by MJQ (subrip@divx.pl)
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
#include "TsubtitleProps.h"
#include "TsubtitlesSettings.h"
#include "rational.h"
#include "TfontManager.h"
#include <locale.h>

void TSubtitleProps::reset(void)
{
 wrapStyle=-1;
 refResX=refResY=0;
 bold=-1;
 italic=underline=strikeout=false;
 isColor=false;
 isPos=false;
 size=0;
 fontname[0]='\0';
 encoding=-1;
 spacing=INT_MIN;
 scaleX=scaleY=-1;
 alignment=-1;
 marginR=marginL=marginV=marginTop=marginBottom=-1;
 borderStyle=-1;
 outlineWidth=shadowDepth=-1;
 color=SecondaryColour=TertiaryColour=0xffffff;
 OutlineColour=ShadowColour=0;
 colorA=SecondaryColourA=TertiaryColourA=OutlineColourA=256;
 ShadowColourA=128;
 blur=0;
 version=-1;
 m_ascent64=-1;
 m_descent64=-1;
 tStart=tStop=fadeT1=fadeT2=fadeT3=fadeT4=REFTIME_INVALID;
 isFad=0;
 karaokeMode = KARAOKE_NONE;
 karaokeStart = 0;
 karaokeDuration = 0;
 karaokeNewWord = false;
 extendedTags=0;
}

HGDIOBJ TSubtitleProps::toGdiFont(HDC hdc, LOGFONT &lf,const TfontSettings &fontSettings,unsigned int dx,unsigned int dy,unsigned int clipdy,const Rational& sar, TfontManager *fontManager) const
{
 toLOGFONT(lf,fontSettings,dx,dy,clipdy,sar);
 HFONT font=fontManager->getFont(lf);
 HGDIOBJ old=SelectObject(hdc,font);
 fix_size(lf,hdc,fontManager);
 return old;
}

void TSubtitleProps::toLOGFONT(LOGFONT &lf,const TfontSettings &fontSettings,unsigned int dx,unsigned int dy,unsigned int clipdy,const Rational& sar) const
{
 memset(&lf,0,sizeof(lf));
 lf.lfHeight=(LONG)limit(size?size:fontSettings.getSize(dx,dy),3U,255U)*4;
 if (refResY && dy)
  lf.lfHeight=(clipdy ? clipdy : dy)*lf.lfHeight/refResY;
 int yscale=get_yscale(fontSettings.yscale,sar,fontSettings.aspectAuto,fontSettings.overrideScale);
 lf.lfHeight=lf.lfHeight*yscale/100;
 lf.lfWidth=0;
 if (bold==-1)
  lf.lfWeight=fontSettings.weight;
 else if (bold==0)
  lf.lfWeight=0;
 else
  lf.lfWeight=700;
 lf.lfItalic=italic;
 lf.lfUnderline=underline;
 lf.lfStrikeOut=strikeout;
 lf.lfCharSet=BYTE(encoding!=-1?encoding:fontSettings.charset);
 lf.lfOutPrecision=OUT_TT_PRECIS;
 lf.lfClipPrecision=CLIP_DEFAULT_PRECIS;
 lf.lfQuality=ANTIALIASED_QUALITY;
 lf.lfPitchAndFamily=DEFAULT_PITCH|FF_DONTCARE;
 ff_strncpy(lf.lfFaceName,fontname[0]?fontname:fontSettings.name,LF_FACESIZE);
}

void TSubtitleProps::fix_size(LOGFONT &lf, HDC hdc, TfontManager *fontManager) const
{
 // for ASS compatibility.
 // vsfilter multiple 64 to lfHeight when it rasterizes the font.
 // ffdshow multiple 4. This is not compatible, so here we want to correct.
 TEXTMETRIC tm4, tm64;
 if (GetTextMetrics(hdc,&tm4))
  {
   lf.lfHeight*=16;
   HFONT font=fontManager->getFont(lf);
   SelectObject(hdc,font);
   GetTextMetrics(hdc,&tm64);
   double r4=(double)(tm4.tmHeight-tm4.tmInternalLeading)*16/lf.lfHeight;
   double r64=(double)(tm64.tmHeight-tm64.tmInternalLeading)/lf.lfHeight;
   m_ascent64=(tm64.tmAscent + 4) >> 3;
   m_descent64=(tm64.tmDescent + 4) >> 3;
   lf.lfHeight=LONG((double)lf.lfHeight*r64/r4/16);
   font=fontManager->getFont(lf);
   SelectObject(hdc,font);
  }
 else
  {
   m_ascent64=int(lf.lfHeight*2*0.8);
   m_descent64=int(lf.lfHeight*2-m_ascent64);
  }
}

int TSubtitleProps::get_spacing(unsigned int dy,unsigned int clipdy) const
{
 if (refResY)
  return int(spacing*4.0*(clipdy ? clipdy : dy)/refResY);
 else
  return int(spacing*4.0); 
}

int TSubtitleProps::get_marginR(unsigned int screenWidth) const
{
 // called only for SSA/ASS/ASS2
 int result;
 if (isPos)
  {
   switch (alignment)
    {
     case 3: // right(SSA)
     case 7:
     case 11:
      result=refResX-posx;
      break;
     case 2: // center(SSA)
     case 6:
     case 10:
      result=refResX-posx*2;
      break;
     default:
      result=0;
      break;
    }
  }
 else if (marginR>=0)
  result=marginR;
 else return 0;

 if (result<0) result=0;
 if (refResX>0)
  return result*screenWidth/refResX;
 else
  return result;
}
int TSubtitleProps::get_marginL(unsigned int screenWidth) const
{
 // called only for SSA/ASS/ASS2
 int result;
 if (isPos)
  {
   switch (alignment)
    {
     case 1: // left(SSA)
     case 5:
     case 9:
      result=posx;
      break;
     case 3: // right(SSA)
     case 7:
     case 11:
      result=0;
      break;
     case 2: // center(SSA)
     case 6:
     case 10:
     default:
      result=posx*2-refResX;
      break;
    }
  }
 else if (marginL>=0)
  result=marginL;
 else return 0;

 if (result<0) result=0;
 if (refResX>0)
  return result*screenWidth/refResX;
 else
  return result;
}
int TSubtitleProps::get_marginTop(unsigned int screenHeight) const
{
 int result;
 if (isPos)
  {
   switch (alignment)
    {
     case 5: // SSA top
     case 6:
     case 7:
      result=posy;
      break;
     case 9: // SSA mid
     case 10:
     case 11:
      result=posy*2-refResY;
      break;
     case 1: // SSA bottom
     case 2:
     case 3:
     default:
      result=0;
      break;
    }
  }
 else if (marginTop>0)
  result=marginTop; //ASS
 else if (marginV>0)
  result=marginV; // SSA
 else return 0;

 if (result<0) result=0;
 if (refResY>0)
  return result*screenHeight/refResY;
 else
  return result;
}
int TSubtitleProps::get_marginBottom(unsigned int screenHeight) const
{
 int result;
 if (isPos)
  {
   switch (alignment)
    {
     case 5: // SSA top
     case 6:
     case 7:
      result=0;
      break;
     case 9: // SSA mid
     case 10:
     case 11:
      result=refResY-posy*2;
      break;
     case 1: // SSA bottom
     case 2:
     case 3:
     default:
      result=refResY-posy;
      break;
    }
  }
 else if (marginBottom>0)
  result=marginBottom; //ASS
 else if (marginV>0)
  result=marginV; // SSA
 else return 0;

 if (result<0) result=0;
 if (refResY>0)
  return result*screenHeight/refResY;
 else
  return result;
}

int TSubtitleProps::alignASS2SSA(int align)
{
 switch (align)
  {
   case 1:
   case 2:
   case 3:
    return align;
   case 4:
   case 5:
   case 6:
    return align+5;
   case 7:
   case 8:
   case 9:
    return align-2;
  }
 return align;
}

int TSubtitleProps::get_xscale(int Ixscale,const Rational& sar,int aspectAuto,int overrideScale) const
{
 int result;
 if (scaleX==-1 || overrideScale)
  result=Ixscale;
 else
  result=scaleX;
 if ((aspectAuto) && sar.num>sar.den)
  result=result*sar.den/sar.num;
 return result;
}

int TSubtitleProps::get_yscale(int Iyscale,const Rational& sar,int aspectAuto,int overrideScale) const
{
 int result;
 if (scaleY==-1 || overrideScale)
  result=Iyscale;
 else
  result=scaleY;
 if ((aspectAuto) && sar.num<sar.den)
  result=result*sar.num/sar.den;
 return result;
}
