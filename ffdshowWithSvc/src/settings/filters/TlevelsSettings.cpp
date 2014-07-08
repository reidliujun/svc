/*
 * Copyright (c) 2003-2006 Milan Cutka
 *
 * curve calculation from The GIMP -- an image manipulation program Copyright (C) 1995 Spencer Kimball and Peter Mattis
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
#include "TlevelsSettings.h"
#include "TimgFilterLevels.h"
#include "Clevels.h"
#include "TffdshowPageDec.h"

const char_t* TlevelsSettings::modes[]=
{
 _l("original"),
 _l("Didee's Ylevels"), //TODO: revert to Didée when unicode
 _l("Didee's YlevelsG"),
 _l("Didee's YlevelsS"),
 _l("Didee's YlevelsC"),
 _l("curve"),
 _l("Seb's BTB&WTW"),
 NULL
};

const TlevelsSettings::CRPoint TlevelsSettings::points[10]={&TlevelsSettings::point0x,&TlevelsSettings::point0y,&TlevelsSettings::point1x,&TlevelsSettings::point1y,&TlevelsSettings::point2x,&TlevelsSettings::point2y,&TlevelsSettings::point3x,&TlevelsSettings::point3y,&TlevelsSettings::point4x,&TlevelsSettings::point4y,&TlevelsSettings::point5x,&TlevelsSettings::point5y,&TlevelsSettings::point6x,&TlevelsSettings::point6y,&TlevelsSettings::point7x,&TlevelsSettings::point7y,&TlevelsSettings::point8x,&TlevelsSettings::point8y,&TlevelsSettings::point9x,&TlevelsSettings::point9y};

void TlevelsSettings::calcMap(unsigned int map[256],int *divisor)
{
 calcMap(map,divisor,inMin,inMax);
}
void TlevelsSettings::calcMap(unsigned int map[256],int *divisor,int inMin,int inMax)
{
 double a=inMin,b=inMax,c=outMin,d=outMax;if (a==b) b+=1;if (c==d) d+=1;
 double gamma=this->gamma/100.0;
 *divisor=inMax-inMin+(inMax==inMin);
 int limitMin=fullY?0:16,limitMax=fullY?255:235;
 switch (mode)
  {
   case 0:
   case 6: // Seb's BTB&WTW
    for (int x=0;x<256;x++) // original
     {
      double i=fullY?x:(x-16)*(255.0/219.0);
      double p=(i-inMin)/(*divisor);
      p=pow(limit(p,0.0,1.0),1/gamma);
      p=p*(outMax-outMin)+outMin;
      map[x]=limit(int(p),limitMin,limitMax);
     }
   case 1:
    for (int x=0;x<256;x++) // Ylevels
     {
      double i=fullY?x:(x-16)*(255.0/219.0);
      double p=pow((i-a)/(b-a),1/gamma)*(d-c)+c;
      if (!fullY) p=p*(219.0/255.0)+16.5;
      map[x]=limit(int(p),limitMin,limitMax);
     }
    break;
   case 2:
    for (int x=0;x<256;x++) // YlevelsG
     // Original script for Avisynth by Didee and Alain2
     // http://forum.doom9.org/showthread.php?p=897854
     //
     // Function YlevelsG_DarkEnh(clip clp, int a, float gamma, int b, int c, int d)
     // {
     //  wicked = "x "+string(a)+" - "+string(b)+" "+string(a)+" - / "+string(d)+" "+string(c)+" - * "+string(c)+" + x 255 / * x "+string(a)+" - "+string(b)+" "+string(a)+" - / 1 "+string(gamma)+" / ^ "+string(d)+" "+string(c)+" - * "+string(c)+" + 1 x 255 / - * +"
     //  Return( clp.mt_lut(Yexpr = wicked, U=2,V=2) )
     // }
     {
      double i=fullY?x:(x-16)*(255.0/219.0);
      double p=((i-a)/(b-a)*(d-c)+c)*i/255+(pow((i-a)/(b-a),1/gamma)*(d-c)+c)*(1-i/255);
      if (!fullY) p=p*(219.0/255.0)+16.5;
      map[x]=limit(int(p),limitMin,limitMax);
     }
    break;
   case 3:
    for (int x=0;x<256;x++) // YlevelsS
     // Original script for Avisynth by Didee and Alain2
     //
     // Function YlevelsS_DarkEnh(clip clp, int a, float gamma, int b, int c, int d)
     // {
     //  wicked = "x "+string(a)+" - "+string(b)+" "+string(a)+" - / "+string(d)+" "+string(c)+" - * "+string(c)+" + x 162.338 / sin * x "+string(a)+" - "+string(b)+" "+string(a)+" - / 1 "+string(gamma)+" / ^ "+string(d)+" "+string(c)+" - * "+string(c)+" + 1 x 162.338 / sin - * +"
     //  Return( clp.mt_lut(Yexpr = wicked, U=2,V=2) )
     // }
     {
      double i=fullY?x:(x-16)*(255.0/219.0);
      double p=((i-a)/(b-a)*(d-c)+c)*sin(i/162.338)+(pow((i-a)/(b-a),1/gamma)*(d-c)+c)*(1-sin(i/162.338));
      if (!fullY) p=p*(219.0/255.0)+16.5;
      map[x]=limit(int(p),limitMin,limitMax);
     }
    break;
   case 4:
    for (int x=0;x<256;x++) // YlevelsC
     // Original script for Avisynth by Didee and Alain2
     //
     // Function YlevelsC_DarkEnh(clip clp, int a, float gamma, int b, int c, int d)
     // {
     //  wicked = "x "+string(a)+" - "+string(b)+" "+string(a)+" - / "+string(d)+" "+string(c)+" - * "+string(c)+" + 1 x 162.338 / cos - * x "+string(a)+" - "+string(b)+" "+string(a)+" - / 1 "+string(gamma)+" / ^ "+string(d)+" "+string(c)+" - * "+string(c)+" + x 162.338 / cos * +"
     //  Return( clp.mt_lut(Yexpr = wicked, U=2,V=2) )
     // }
     {
      double i=fullY?x:(x-16)*(255.0/219.0);
      double p=((i-a)/(b-a)*(d-c)+c)*(1-cos(i/162.338))+(pow((i-a)/(b-a),1/gamma)*(d-c)+c)*cos(i/162.338);
      if (!fullY) p=p*(219.0/255.0)+16.5;
      map[x]=limit(int(p),limitMin,limitMax);
     }
    break;
   case 5:
    calcCurve(map);
    break;
/*
   case 6: // Seb's BTB&WTW
    for (int x=0;x<256;x++)
     {
      //double i=fullY?x:(x-16)*(255.0/219.0);
      double p=(x-inMin)/(*divisor);
      p=pow(limit(p,0.0,1.0),1/gamma);
      p=p*(outMax-outMin)+outMin;
      map[x]=limit(int(p),limitMin,limitMax);
     }
	break;*/
  }
 if(mode!=5 && posterize!=255)
  {
   unsigned int stepd=256/(posterize-1);
   for (unsigned int x=0;x<256;x++)
    map[x]=limit(((map[x]+stepd/2)/stepd)*stepd,0U,255U);
  }
}

void TlevelsSettings::calcCurve(unsigned int dp[256])
{
 for (int x=0;x<this->*points[0][0];x++)
  dp[x]=this->*points[0][1];
 for (int i=0;i<numPoints-1;i++)
  {
   int p1 = i==0?i:i-1;
   int p2 = i;
   int p3 = i+1;
   int p4 = i==numPoints-2?numPoints-1:i+2;
   plot_curve(p1,p2,p3,p4,dp);
  }
 for (int x=this->*points[numPoints-1][0];x<256;x++)
  dp[x]=this->*points[numPoints-1][1];
 for (int i=0;i<numPoints;i++)
  dp[this->*points[i][0]]=this->*points[i][1];
}

void TlevelsSettings::plot_curve(int p1,int p2,int p3,int p4,unsigned int curve[256])
{
 typedef double CRMatrix[4][4];
 static const CRMatrix CR_basis=
  {
   { -0.5,  1.5, -1.5,  0.5 },
   {  1.0, -2.5,  2.0, -0.5 },
   { -0.5,  0.0,  0.5,  0.0 },
   {  0.0,  1.0,  0.0,  0.0 },
  };
 struct curves_CR_compose
  {
   void operator ()(const CRMatrix &a, const CRMatrix &b, CRMatrix &ab)
    {
     for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
       ab[i][j] = (a[i][0] * b[0][j] +
                   a[i][1] * b[1][j] +
                   a[i][2] * b[2][j] +
                   a[i][3] * b[3][j]);
    }
  };

 CRMatrix geometry;
 CRMatrix tmp1, tmp2;
 CRMatrix deltas;
 double  x, dx, dx2, dx3;
 double  y, dy, dy2, dy3;
 double  d, d2, d3;
 int     lastx, lasty;
 int   newx, newy;
 int     i;

 /* construct the geometry matrix from the segment */
 for (i = 0; i < 4; i++)
  {
   geometry[i][2] = 0;
   geometry[i][3] = 0;
  }

 for (i = 0; i < 2; i++)
  {
   geometry[0][i] = this->*points[p1][i];
   geometry[1][i] = this->*points[p2][i];
   geometry[2][i] = this->*points[p3][i];
   geometry[3][i] = this->*points[p4][i];
  }

 /* subdivide the curve */
 static const int CURVES_SUBDIVIDE=1000;

 d = 1.0 / CURVES_SUBDIVIDE;
 d2 = d * d;
 d3 = d * d * d;

 /* construct a temporary matrix for determining the forward
  * differencing deltas
  */
 tmp2[0][0] = 0;       tmp2[0][1] = 0;       tmp2[0][2] = 0;  tmp2[0][3] = 1;
 tmp2[1][0] = d3;      tmp2[1][1] = d2;      tmp2[1][2] = d;  tmp2[1][3] = 0;
 tmp2[2][0] = 6 * d3;  tmp2[2][1] = 2 * d2;  tmp2[2][2] = 0;  tmp2[2][3] = 0;
 tmp2[3][0] = 6 * d3;  tmp2[3][1] = 0;       tmp2[3][2] = 0;  tmp2[3][3] = 0;

 /* compose the basis and geometry matrices */
 curves_CR_compose()(CR_basis, geometry, tmp1);

 /* compose the above results to get the deltas matrix */
 curves_CR_compose()(tmp2, tmp1, deltas);

 /* extract the x deltas */
 x   = deltas[0][0];
 dx  = deltas[1][0];
 dx2 = deltas[2][0];
 dx3 = deltas[3][0];

 /* extract the y deltas */
 y   = deltas[0][1];
 dy  = deltas[1][1];
 dy2 = deltas[2][1];
 dy3 = deltas[3][1];

 lastx = limit_uint8((int)x);
 lasty = limit_uint8((int)y);

 curve[lastx] = lasty;

 /* loop over the curve */
 for (i = 0; i < CURVES_SUBDIVIDE; i++)
  {
   /* increment the x values */
   x += dx;
   dx += dx2;
   dx2 += dx3;

   /* increment the y values */
   y += dy;
   dy += dy2;
   dy2 += dy3;

   newx = limit_uint8 (int (x));
   newy = limit_uint8 (int (y));

   /* if this point is different than the last one...then draw it */
   if ((lastx != newx) || (lasty != newy))
    curve[newx] = newy;

   lastx = newx;
   lasty = newy;
  }
}

const TfilterIDFF TlevelsSettings::idffs=
{
 /*name*/      _l("Levels"),
 /*id*/        IDFF_filterLevels,
 /*is*/        IDFF_isLevels,
 /*order*/     IDFF_orderLevels,
 /*show*/      IDFF_showLevels,
 /*full*/      IDFF_fullLevels,
 /*half*/      IDFF_halfLevels,
 /*dlgId*/     IDD_LEVELS,
};

TlevelsSettings::TlevelsSettings(TintStrColl *Icoll,TfilterIDFFs *filters):TfilterSettingsVideo(sizeof(*this),Icoll,filters,&idffs)
{
 static const TintOptionT<TlevelsSettings> iopts[]=
  {
   IDFF_isLevels       ,&TlevelsSettings::is      ,0,0,_l(""),1,
     _l("isLevels"),0,
   IDFF_showLevels     ,&TlevelsSettings::show    ,0,0,_l(""),1,
     _l("showLevels"),1,
   IDFF_orderLevels    ,&TlevelsSettings::order   ,1,1,_l(""),1,
     _l("orderLevels"),0,
   IDFF_fullLevels     ,&TlevelsSettings::full    ,0,0,_l(""),1,
     _l("fullLevels"),0,
   IDFF_halfLevels     ,&TlevelsSettings::half    ,0,0,_l(""),1,
     _l("halfLevels"),0,
   IDFF_levelsMode     ,&TlevelsSettings::mode    ,0,6,_l(""),1,
     _l("levelsMode"),0,
   IDFF_levelsGamma    ,&TlevelsSettings::gamma   ,1,400,_l(""),1,
     _l("levelsGamma"),100,
   IDFF_levelsPosterize,&TlevelsSettings::posterize,2,255,_l(""),1,
     _l("levelsPosterize"),255,
   IDFF_levelsYmaxDelta,&TlevelsSettings::YmaxDelta,1,255,_l(""),1,
     _l("YmaxDelta"),20,
   IDFF_levelsYthreshold,&TlevelsSettings::Ythreshold,1,255,_l(""),1,
     _l("Ythreshold"),10,
   IDFF_levelsYtemporal,&TlevelsSettings::Ytemporal,1,10,_l(""),1,
     _l("Ytemporal"),3,
   IDFF_levelsInMin    ,&TlevelsSettings::inMin   ,-3,-3,_l(""),1,
     _l("levelsInMin"),0,
   IDFF_levelsInMax    ,&TlevelsSettings::inMax   ,-3,-3,_l(""),1,
     _l("levelsInMax"),255,
   IDFF_levelsOutMin   ,&TlevelsSettings::outMin  ,-3,-3,_l(""),1,
     _l("levelsOutMin"),0,
   IDFF_levelsOutMax   ,&TlevelsSettings::outMax  ,-3,-3,_l(""),1,
     _l("levelsOutMax"),255,
   IDFF_levelsOnlyLuma ,&TlevelsSettings::onlyLuma,0,0,_l(""),1,
     _l("levelsOnlyLuma"),0,
   IDFF_levelsFullY    ,&TlevelsSettings::fullY   ,0,0,_l(""),1,
     _l("levelsFullY"),0,
   IDFF_levelsInAuto   ,&TlevelsSettings::inAuto  ,0,0,_l(""),1,
     _l("levelsInAuto"),0,
   IDFF_levelsNumPoints,&TlevelsSettings::numPoints ,2,10,_l(""),1,
     _l("levelsNumPoints"),2,
   IDFF_levelsPoint0x  ,&TlevelsSettings::point0x   ,0,255,_l(""),1,
     _l("levelsPoint0x"),0,
   IDFF_levelsPoint0y  ,&TlevelsSettings::point0y   ,0,255,_l(""),1,
     _l("levelsPoint0y"),0,
   IDFF_levelsPoint1x  ,&TlevelsSettings::point1x   ,0,255,_l(""),1,
     _l("levelsPoint1x"),255,
   IDFF_levelsPoint1y  ,&TlevelsSettings::point1y   ,0,255,_l(""),1,
     _l("levelsPoint1y"),255,
   IDFF_levelsPoint2x  ,&TlevelsSettings::point2x   ,0,255,_l(""),1,
     _l("levelsPoint2x"),0,
   IDFF_levelsPoint2y  ,&TlevelsSettings::point2y   ,0,255,_l(""),1,
     _l("levelsPoint2y"),0,
   IDFF_levelsPoint3x  ,&TlevelsSettings::point3x   ,0,255,_l(""),1,
     _l("levelsPoint3x"),0,
   IDFF_levelsPoint3y  ,&TlevelsSettings::point3y   ,0,255,_l(""),1,
     _l("levelsPoint3y"),0,
   IDFF_levelsPoint4x  ,&TlevelsSettings::point4x   ,0,255,_l(""),1,
     _l("levelsPoint4x"),0,
   IDFF_levelsPoint4y  ,&TlevelsSettings::point4y   ,0,255,_l(""),1,
     _l("levelsPoint4y"),0,
   IDFF_levelsPoint5x  ,&TlevelsSettings::point5x   ,0,255,_l(""),1,
     _l("levelsPoint5x"),0,
   IDFF_levelsPoint5y  ,&TlevelsSettings::point5y   ,0,255,_l(""),1,
     _l("levelsPoint5y"),0,
   IDFF_levelsPoint6x  ,&TlevelsSettings::point6x   ,0,255,_l(""),1,
     _l("levelsPoint6x"),0,
   IDFF_levelsPoint6y  ,&TlevelsSettings::point6y   ,0,255,_l(""),1,
     _l("levelsPoint6y"),0,
   IDFF_levelsPoint7x  ,&TlevelsSettings::point7x   ,0,255,_l(""),1,
     _l("levelsPoint7x"),0,
   IDFF_levelsPoint7y  ,&TlevelsSettings::point7y   ,0,255,_l(""),1,
     _l("levelsPoint7y"),0,
   IDFF_levelsPoint8x  ,&TlevelsSettings::point8x   ,0,255,_l(""),1,
     _l("levelsPoint8x"),0,
   IDFF_levelsPoint8y  ,&TlevelsSettings::point8y   ,0,255,_l(""),1,
     _l("levelsPoint8y"),0,
   IDFF_levelsPoint9x  ,&TlevelsSettings::point9x   ,0,255,_l(""),1,
     _l("levelsPoint9x"),0,
   IDFF_levelsPoint9y  ,&TlevelsSettings::point9y   ,0,255,_l(""),1,
     _l("levelsPoint9y"),0,
   0
  };
 addOptions(iopts);

 static const TcreateParamList1 listMode(modes);setParamList(IDFF_levelsMode,&listMode);
}

void TlevelsSettings::getMinMax(int id,int &min,int &max)
{
 switch (id)
  {
   case IDFF_levelsInMin :min=0;max=inMax-1;break;
   case IDFF_levelsInMax :min=inMin+1;max=255;break;
   case IDFF_levelsOutMin:min=0;max=outMax-1;break;
   case IDFF_levelsOutMax:min=outMin+1;max=255;break;
  }
}

void TlevelsSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if (show)
  queueFilter<TimgFilterLevels>(filtersorder,filters,queue);
}
void TlevelsSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TlevelsPage>(&idffs);
}

const int* TlevelsSettings::getResets(unsigned int pageId)
{
 static const int idResets[]={
  IDFF_levelsMode,IDFF_levelsOnlyLuma,IDFF_levelsFullY,
  IDFF_levelsInMin,IDFF_levelsInMax,IDFF_levelsInAuto,IDFF_levelsOutMin,IDFF_levelsOutMax,IDFF_levelsGamma,IDFF_levelsPosterize,
  IDFF_levelsYmaxDelta,IDFF_levelsYthreshold,IDFF_levelsYtemporal,
  IDFF_levelsNumPoints,IDFF_levelsPoint0x,IDFF_levelsPoint0y,IDFF_levelsPoint1x,IDFF_levelsPoint1y,IDFF_levelsPoint2x,IDFF_levelsPoint2y,IDFF_levelsPoint3x,IDFF_levelsPoint3y,IDFF_levelsPoint4x,IDFF_levelsPoint4y,IDFF_levelsPoint5x,IDFF_levelsPoint5y,IDFF_levelsPoint6x,IDFF_levelsPoint6y,IDFF_levelsPoint7x,IDFF_levelsPoint7y,IDFF_levelsPoint8x,IDFF_levelsPoint8y,IDFF_levelsPoint9x,IDFF_levelsPoint9y,
  0};
 return idResets;
}

bool TlevelsSettings::getTip(unsigned int pageId,char_t *tipS,size_t len)
{
 tsnprintf_s(tipS, len, _TRUNCATE, _l("%s\nInput min: %i, input max: %i, output min:%i, output max:%i\nGamma correction: %5.2f"),modes[mode],inMin,inMax,outMin,outMax,gamma/100.0f);
 if (posterize!=255)
  strncatf(tipS,len,_l("\nPosterizing to %i levels"),posterize);
 return true;
}
