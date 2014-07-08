/*
 * Copyright (c) 2004-2006 Milan Cutka
 * Based of Logoaway plugin filter for VirtualDub by Krzysztof Wojdon
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
#include "TimgFilterLogoaway.h"
#include "TlogoawaySettings.h"

//===================================== TimgFilterLogoaway::Tplane =====================================
inline unsigned char* TimgFilterLogoaway::Tplane::getPixelAddr(int x,int y)
{
 return dst+stride2*(y>>shiftY)+(x>>shiftX);
}

unsigned char* TimgFilterLogoaway::Tplane::getCornerAddr(const TlogoawaySettings *cfg,int corner)
{
 switch (corner)
  {
   case TlogoawaySettings::NW:
    return getPixelAddr(cfg->x,cfg->y);
   case TlogoawaySettings::NE:
    return getPixelAddr(cfg->x+cfg->dx-1,cfg->y);
   case TlogoawaySettings::SW:
    return getPixelAddr(cfg->x,cfg->y+cfg->dy-1);
   case TlogoawaySettings::SE:
    return getPixelAddr(cfg->x+cfg->dx-1,cfg->y+cfg->dy-1);
   default:
    return NULL;
  }
}

void TimgFilterLogoaway::Tplane::selectPoints(const TlogoawaySettings *cfg,
                                              int bmode,
                                              unsigned char **s,unsigned char **e,
                                              int psd,int ped,
                                              int pso,int peo,
                                              int psi,int pei
                                             )
{
 switch (bmode)
  {
   case TlogoawaySettings::BM_DIRECT:
    *s=getCornerAddr(cfg,psd);
    *e=getCornerAddr(cfg,ped);
    break;
   case TlogoawaySettings::BM_OPPOSITE:
    *s=getCornerAddr(cfg,pso);
    *e=getCornerAddr(cfg,peo);
    break;
   case TlogoawaySettings::BM_INTERPOLATE:
    *s=getCornerAddr(cfg,psi);
    *e=getCornerAddr(cfg,pei);
    break;
   default:
    *s=*e=NULL;
    break;
  }
}

void TimgFilterLogoaway::Tplane::calcGradient(const unsigned char *s,const unsigned char *e,int len,FIX *r,FIX *rd)
{
 *r=*s<<16;
 *rd=(FIX(*e)-FIX(*s))<<16;
 *rd/=len-1;
}

void TimgFilterLogoaway::Tplane::createBorderH(unsigned char *border,const unsigned char *s,const unsigned char *e,int width,int bmode)
{
 int i;
 FIX rt,rd;

 switch (bmode)
  {
   case TlogoawaySettings::BM_INTERPOLATE:
    calcGradient(s,e,width,
                 &rt,
                 &rd);
    for (i=0;i<width;i++)
     {
      *border++=uint8_t(rt>>16);
      rt+=rd;
     }
    break;
   case TlogoawaySettings::BM_DIRECT:
   case TlogoawaySettings::BM_OPPOSITE:
    memcpy(border,s,width);
    break;
  }
}

void TimgFilterLogoaway::Tplane::createBorderV(unsigned char *border,const unsigned char *s,const unsigned char *e,int h,stride_t pitch,int bmode)
{
 int i;

 switch (bmode)
  {
   case TlogoawaySettings::BM_INTERPOLATE:
    {
     FIX rt,rd;
     calcGradient(s,e,h,
                  &rt,
                  &rd);
     for (i=0;i<h;i++)
      {
       *border++=uint8_t(rt>>16);
       rt+=rd;
      }
     break;
    }
   case TlogoawaySettings::BM_DIRECT:
   case TlogoawaySettings::BM_OPPOSITE:
    for (i=0;i<h;i++)
     {
      *border++=*s;
      s+=pitch;
     }
    break;
  }
}

void TimgFilterLogoaway::Tplane::createBorder(const TlogoawaySettings *cfg,int side,int bmode)
{
 unsigned char *s,*e;

 switch (side)
  {
   case TlogoawaySettings::NORTH:
    selectPoints(cfg,bmode,&s,&e,
                 TlogoawaySettings::NW,TlogoawaySettings::NE,
                 TlogoawaySettings::SW,TlogoawaySettings::SE,
                 cfg->pointnw,cfg->pointne);
    createBorderH(bordn,s,e,cfg->dx>>shiftX,bmode);
    break;
   case TlogoawaySettings::EAST:
    selectPoints(cfg,bmode,&s,&e,
                 TlogoawaySettings::NE,TlogoawaySettings::SE,
                 TlogoawaySettings::NW,TlogoawaySettings::SW,
                 cfg->pointne,cfg->pointse);
    createBorderV(borde,s,e,cfg->dy>>shiftY,stride2,bmode);
    break;
   case TlogoawaySettings::SOUTH:
    selectPoints(cfg,bmode,&s,&e,
                 TlogoawaySettings::SW,TlogoawaySettings::SE,
                 TlogoawaySettings::NW,TlogoawaySettings::NE,
                 cfg->pointsw,cfg->pointse);
    createBorderH(bords,s,e,cfg->dx>>shiftX,bmode);
    break;
   case TlogoawaySettings::WEST:
    selectPoints(cfg,bmode,&s,&e,
                 TlogoawaySettings::NW,TlogoawaySettings::SW,
                 TlogoawaySettings::NE,TlogoawaySettings::SE,
                 cfg->pointnw,cfg->pointsw);
    createBorderV(bordw,s,e,cfg->dy>>shiftY,stride2,bmode);
    break;
  }
}

void TimgFilterLogoaway::Tplane::saveLogoBorder(int border)
{
 int i;

 switch( border )
  {
   case TlogoawaySettings::NORTH:
    memcpy(logotempdata,bordn,w);
    break;
   case TlogoawaySettings::SOUTH:
    {
     unsigned char *buffer=logotempdata+logotempstride*(h-1);
     memcpy(buffer,bords,w);
     break;
    }
   case TlogoawaySettings::WEST:
    {
     unsigned char *buffer=logotempdata;
     for (i=0;i<h;i++)
      buffer[logotempstride*i]=bordw[i];
     break;
    }
   case TlogoawaySettings::EAST:
    {
     unsigned char *buffer=logotempdata;
     for (i=0;i<h;i++)
      buffer[logotempstride*i+w-1]=borde[i];
     break;
    }
  }
}

void TimgFilterLogoaway::Tplane::setAverageVH(const TlogoawaySettings *cfg)
{
 int i,j;
 int vstart,vend;
 int hstart,hend;
 FIX px,py;

 // Precalculate vertical (north->south) gradient parameters:
 // For each column: starting R,G,B and per-row deltas R,G,B
 for(i=0;i<w;i++)
  calcGradient(&bordn[i],&bords[i],h,
               &vt[i],
               &vd[i]);

 // CALCULATE RECTANGLE START AND END
 // BM_INTERPOLATE overwrites border
 // other methods do not - just leave border alone.

 if(cfg->bordn_mode==TlogoawaySettings::BM_INTERPOLATE)
  vstart=0;
 else
  vstart=1;

 if(cfg->bords_mode==TlogoawaySettings::BM_INTERPOLATE)
  vend=h;
 else
  vend=h-1;

 if(cfg->bordw_mode==TlogoawaySettings::BM_INTERPOLATE)
  hstart=0;
 else
  hstart=1;

 if(cfg->borde_mode==TlogoawaySettings::BM_INTERPOLATE)
  hend=w;
 else
  hend=w-1;

 unsigned char *pRow=logotempdata;

 // If north border unused
 // we need to skip this border with
 // one empty vertical gradient step
 if (vstart==1)
  {
   pRow+=logotempstride; // fix added on
   for (i=0;i<w;i++)
    vt[i]+=vd[i];
  }

 // Now fill logo area by walking each scanline
 // step by step through the logo h
 for (i=vstart;i<vend;i++)
  {
   int r,rd;
   // Horizontal gradient for this row
   calcGradient(&bordw[i],&borde[i],w,&r,&rd);

   if (hstart==1) // First column step skip
    r+=rd;

   // Make whole logo row
   for (j=hstart;j<hend;j++)
    {
     // Update vertical gradient current component
     // Next row we will need next vertical step
     vt[j]+=vd[j];
     // Make weighted H gradient component
     px=r*(10-cfg->vhweight);
     // Make weighted V gradient component
     py=vt[j]*cfg->vhweight;
     // Calc
     pRow[j]=uint8_t(((px+py)/10)>>16);
     // Next pixel, one gradient step
     r+=rd;
    }
    // Get next row start Real UP->DOWN, not VirtualDub down->up
   pRow+=logotempstride;
  }
}

void TimgFilterLogoaway::Tplane::blurLogotemp(bool useparambitmap)
{
 unsigned char *rowp, *row, *rown;
 unsigned char *rowbkup;

 rowp   =(unsigned char*)_alloca(w);
 rowbkup=(unsigned char*)_alloca(w);

 memcpy(rowp,logotempdata,w);

 row=logotempdata+logotempstride;
 rown=row+logotempstride;

 for (int i=1;i<=h-2;i++)
  {
   // 1 ... X+
   memcpy(rowbkup,row,w);
   for (int j=1;j<=w-2;j++)
    if (!useparambitmap ||
       (parambitmapdata[i*parambitmapstride+j]>SHAPEMAP_NOCHANGE &&
        parambitmapdata[i*parambitmapstride+j]<SHAPEMAP_CONTOUR))
      {
       int r=rowp[j-1]+
             rowp[j+1]+
             (row[j]<<2)+
             rown[j-1]+
             rown[j+1];
       row[j]=uint8_t(r/8);
      }
   row=rown;
   rown=rown+logotempstride;
   memcpy(rowp,rowbkup,w);
  }
}

void TimgFilterLogoaway::Tplane::calcUweWeightTable(int w,int h,int power)
{
 double e = 1.0 + (0.3 * power);
 int x;
 for (x=0;x<w;x++)
  for (int y=0;y<h;y++)
   if(x+y!=0)
    {
     double d=pow(sqrt(double(x*x+y*y)),e);
     uwetable[x+y*w]=1.0/d;
    }
   else
    uwetable[x+y*w]=1.0;

 for (x=1;x<w-1;x++)
  for (int y=1;y<h-1;y++)
   {
    double weightsum=0;
    for (int bx=0;bx<w;bx++)
     {
      weightsum+=uwetable[abs(bx-x)+y*w];
      weightsum+=uwetable[abs(bx-x)+abs(h-1-y)*w];
     }
    for (int by=1;by<h-1;by++)
     {
      weightsum+=uwetable[x+abs(by-y)*w];
      weightsum+=uwetable[abs(w-1-x)+abs(by-y)*w];
     }
    uweweightsum[y*w+x]=weightsum;
   }
}

void TimgFilterLogoaway::Tplane::uwe(const TlogoawaySettings *cfg)
{
 if (!uwetable)
  {
   uwetable=(double*)aligned_malloc(w*h*sizeof(double));
   uweweightsum=(double*)aligned_malloc(w*h*sizeof(double));
   calcUweWeightTable(w,h,cfg->blur);
  }

 for (int x=1;x<w-1;x++)
  for (int y=1;y<h-1;y++)
   {
    double r=0;
    const unsigned char *lineN=bordn,*lineS=bords;
    for (int bx=0;bx<w;bx++)
     {
      r+=lineN[bx]*uwetable[abs(bx-x)+y*w];
      r+=lineS[bx]*uwetable[abs(bx-x)+abs(h-1-y)*w];
     }
    const unsigned char *lineW=bordw,*lineE=borde;
    for (int by=1;by<h-1;by++)
     {
      r+=lineW[by]*uwetable[x+abs(by-y)*w];
      r+=lineE[by]*uwetable[abs(w-1-x)+abs(by-y)*w];
     }
    logotempdata[y*logotempstride+x]=uint8_t(r/uweweightsum[y*w+x]);
   }
}

void TimgFilterLogoaway::Tplane::setSolid(const TlogoawaySettings *cfg)
{
 for (int y=0;y<cfg->dy>>shiftY;y++)
  memset(logotempdata+y*logotempstride,solidcolor,cfg->dx>>shiftX);
}

void TimgFilterLogoaway::Tplane::loadLogo(const TlogoawaySettings *cfg,bool withborderN,bool withborderE,bool withborderS,bool withborderW)
{
 int Vstart, Vend;
 int Hstart, Hend;
 const unsigned char *buffer=logotempdata;

 if (withborderN)
  Vstart=0;
 else
  {
   Vstart=1;
   buffer+=logotempstride;
  }

 if (withborderS)
  Vend=h;
 else
  Vend=h-1;

 if (withborderW)
  Hstart=0;
 else
  Hstart=1;

 if (withborderE)
  Hend=w;
 else
  Hend=w-1;

 unsigned char *p=getPixelAddr(cfg->x,cfg->y);
 for (int i=Vstart;i<Vend;i++)
  {
   memcpy(p+Hstart,buffer+(withborderW==false?1:0),Hend-Hstart);
   buffer+=logotempstride;
   p+=stride2;
  }
}

void TimgFilterLogoaway::Tplane::init(const TlogoawaySettings *cfg)
{
 bordn=(unsigned char*)aligned_malloc(w);
 bords=(unsigned char*)aligned_malloc(w);
 borde=(unsigned char*)aligned_malloc(h);
 bordw=(unsigned char*)aligned_malloc(h);
 vd=(FIX*)aligned_malloc(sizeof(FIX)*(w));
 vt=(FIX*)aligned_malloc(sizeof(FIX)*(w));
 if (parambitmapdata && (cfg->mode==TlogoawaySettings::MODE_SHAPEXY || cfg->mode==TlogoawaySettings::MODE_SHAPEUGLARM))
  {
   shapexy_cn=(SHAPEXY_VCNDEF*)aligned_malloc(w*h*sizeof(SHAPEXY_VCNDEF));
   for (int i=0;i<w*h;i++)
    {
     shapexy_cn[i].up   =0;
     shapexy_cn[i].left =0;
     shapexy_cn[i].right=w-1;
     shapexy_cn[i].down =h-1;
    }
   calcShapeXY_VContourNeighbours(cfg);
  }
}

void TimgFilterLogoaway::Tplane::done(void)
{
 if (bordn) aligned_free(bordn);bordn=NULL;
 if (bords) aligned_free(bords);bords=NULL;
 if (borde) aligned_free(borde);borde=NULL;
 if (bordw) aligned_free(bordw);bordw=NULL;
 if (vd) aligned_free(vd);vd=NULL;
 if (vt) aligned_free(vt);vt=NULL;
 if (uwetable) aligned_free(uwetable);uwetable=NULL;
 if (uweweightsum) aligned_free(uweweightsum);uweweightsum=NULL;
 if (shapexy_cn) aligned_free(shapexy_cn);shapexy_cn=NULL;
}

void TimgFilterLogoaway::Tplane::calcShapeXY_VContourNeighbours(const TlogoawaySettings *cfg)
{
 SHAPEXY_VCNDEF *vcn=shapexy_cn;
 //****************************************************************
 // CALCULATE VERTICAL CONTOUR NEIGHBOURS
 for( int x=1;x<w-1;x++)
  {
   // find top neighbours
   int lastcn=0;
   for(int y=1;y<h-1;y++)
    {
     unsigned char b=parambitmapdata[y*parambitmapstride+x];
     if (b>=SHAPEMAP_CONTOUR)
      lastcn=y;
     else if (b<=SHAPEMAP_NOCHANGE)
      continue;
     else
      vcn[y*w+x].up=lastcn;
    }

   // find bottom neighbours
   lastcn=h-1;
   for (int y=h-2;y>0;y--)
    {
     unsigned char b=parambitmapdata[y*parambitmapstride+x];
     if (b>=SHAPEMAP_CONTOUR)
      lastcn=y;
     else if (b<=SHAPEMAP_NOCHANGE)
      continue;
     else
      vcn[y*w+x].down=lastcn;
    }
  }
 //****************************************************************
 // CALCULATE HORIZONTAL CONTOUR NEIGHBOURS
 for(int y=1;y<h-1;y++)
  {
   // find left neighbours
   int lastcn = 0;
   for(int x=1;x<w-1; x++)
    {
     unsigned char b=parambitmapdata[y*parambitmapstride+x];
     if (b>=SHAPEMAP_CONTOUR)
      lastcn = x;
     else if (b<=SHAPEMAP_NOCHANGE)
      continue;
     else
      vcn[ y*w+x].left=lastcn;
    }

   // find bottom neighbours
   lastcn=w-1;
   for(int x=w-2;x>0;x--)
    {
     unsigned char b=parambitmapdata[ y*parambitmapstride+x ];
     if (b>=SHAPEMAP_CONTOUR)
      lastcn=x;
     else if (b<=SHAPEMAP_NOCHANGE)
      continue;
     else
      vcn[y*w+x].right=lastcn;
    }
  }
}

unsigned char TimgFilterLogoaway::Tplane::mix2Pixels32(unsigned char a,unsigned char b,FIX wa,FIX wb)
{
 return (unsigned char)((a*wa + b*wb)>>16);
}

void TimgFilterLogoaway::Tplane::renderShapeXY(const TlogoawaySettings *cfg)
{
 const unsigned char *modeparambitmap=parambitmapdata+parambitmapstride;
 for (int y=1;y<h-1;y++,modeparambitmap+=parambitmapstride)
  for (int x=1;x<w-1;x++)
   if(modeparambitmap[x]>SHAPEMAP_NOCHANGE &&
      modeparambitmap[x]<SHAPEMAP_CONTOUR)
    {
     // HORIZONTAL GRADIENT
     int d1 = x - shapexy_cn[y*w+x].left;
     int d2 = shapexy_cn[y*w+x].right - x;

     unsigned char p1 = logotempdata[ y*logotempstride + shapexy_cn[y*w+x].left  ];
     unsigned char p2 = logotempdata[ y*logotempstride + shapexy_cn[y*w+x].right ];

     unsigned char hp = mix2Pixels32( p1, p2, (d2<<16)/(d1+d2), (d1<<16)/(d1+d2) );

     // VERTICAL GRADIENT
     d1 = y - shapexy_cn[y*w+x].up;
     d2 = shapexy_cn[y*w+x].down - y;

     p1 = logotempdata[ logotempstride * shapexy_cn[y*w+x].up   + x ];
     p2 = logotempdata[ logotempstride * shapexy_cn[y*w+x].down + x ];

     unsigned char vp = mix2Pixels32( p1, p2, (d2<<16)/(d1+d2), (d1<<16)/(d1+d2) );

     // MIX HORIZONTAL & VERTICAL into DEST. PIXEL
     // Mix based on XY slider value (0-10)

     logotempdata[logotempstride*y+x] = mix2Pixels32( hp, vp,
                                6553 * (10-cfg->vhweight),  // 0.1x
                                6553 * cfg->vhweight );     // 0.1x

     //logotemp[p] = p1;
    }
}

void TimgFilterLogoaway::Tplane::renderShapeUwe(const TlogoawaySettings *cfg)
{
 if (!uwetable)
  {
   uwetable=(double*)aligned_malloc(w*h*sizeof(double));
   uweweightsum=(double*)aligned_malloc(w*h*sizeof(double));
   calcUweWeightTable(w,h,cfg->blur);
  }

 int radius=3*(cfg->vhweight+1);
 const unsigned char *modeparambitmap=parambitmapdata+parambitmapstride;
 for (int y=1;y<h-1;y++,modeparambitmap+=parambitmapstride)
  for (int x=1;x<w-1;x++)
   if(modeparambitmap[x]>SHAPEMAP_NOCHANGE &&
      modeparambitmap[x]<SHAPEMAP_CONTOUR)
    {
     int lb=x-radius;
     int rb=x+radius;
     int ub=y-radius;
     int db=y+radius;

     if (lb<0)   lb=0;
     if (rb>w-1) lb=w-1;

     if (ub<0)   ub=0;
     if (db>h-1) db=h-1;

     double weightsum=0.0;
     double b=0.0;

     for (int sy=ub;sy<=db;sy++)
      for (int sx=lb;sx<=rb;sx++)
       if (parambitmapdata[sy*parambitmapstride+sx]>=SHAPEMAP_CONTOUR)
        {
         double weight = uwetable[abs(y-sy)*w+abs(x-sx)];
         b+=logotempdata[sy*logotempstride+sx]*w;
         weightsum+=weight;
        }
     int bi = (int) (b/weightsum);
     logotempdata[y*logotempstride+x]=(unsigned char)bi;
    }
}

void TimgFilterLogoaway::Tplane::process(const TlogoawaySettings *cfg)
{
 createBorder(cfg,TlogoawaySettings::NORTH,cfg->bordn_mode);
 createBorder(cfg,TlogoawaySettings::EAST ,cfg->borde_mode);
 createBorder(cfg,TlogoawaySettings::SOUTH,cfg->bords_mode);
 createBorder(cfg,TlogoawaySettings::WEST ,cfg->bordw_mode);

 saveLogoBorder(TlogoawaySettings::NORTH);
 saveLogoBorder(TlogoawaySettings::SOUTH);
 saveLogoBorder(TlogoawaySettings::EAST );
 saveLogoBorder(TlogoawaySettings::WEST );

 switch (cfg->mode)
  {
   case TlogoawaySettings::MODE_XY:
    setAverageVH(cfg);
    for (int i=1;i<=cfg->blur;i++)
     blurLogotemp(false);
    break;
   case TlogoawaySettings::MODE_UGLARM:
    uwe(cfg);
    break;
   case TlogoawaySettings::MODE_SOLIDFILL:
    setSolid(cfg);
    break;
   case TlogoawaySettings::MODE_SHAPEXY:
    if (parambitmapdata)
     {
      renderShapeXY(cfg);
      for (int i=1;i<=cfg->blur;i++)
       blurLogotemp(true);
      break;
     }
    else
     return;
    case TlogoawaySettings::MODE_SHAPEUGLARM:
    if (parambitmapdata)
     {
      renderShapeUwe(cfg);
      for (int i=1;i<=cfg->blur;i++)
       blurLogotemp(true);
      break;
     }
    else
     return;
  }

 loadLogo(cfg,
          cfg->bordn_mode==TlogoawaySettings::BM_INTERPOLATE,
          cfg->borde_mode==TlogoawaySettings::BM_INTERPOLATE,
          cfg->bords_mode==TlogoawaySettings::BM_INTERPOLATE,
          cfg->bordw_mode==TlogoawaySettings::BM_INTERPOLATE);
}

//========================================= TimgFilterLogoaway =========================================
TimgFilterLogoaway::TimgFilterLogoaway(IffdshowBase *Ideci,Tfilters *Iparent):TimgFilter(Ideci,Iparent)
{
 oldLumaOnly=oldBlur=oldMode=-1;oldparambitmap[0]='\0';
 parambitmap=NULL;
}
void TimgFilterLogoaway::done(void)
{
 logotempbuf.clear();
 parambitmapbuf.clear();
 if (parambitmap) delete parambitmap;parambitmap=NULL;
 for (int i=0;i<3;i++)
  plane[i].done();
}
void TimgFilterLogoaway::onSizeChange(void)
{
 done();
}

bool TimgFilterLogoaway::is(const TffPictBase &pict,const TfilterSettingsVideo *cfg0)
{
 const TlogoawaySettings *cfg=(const TlogoawaySettings*)cfg0;
 if (super::is(pict,cfg) && cfg->dx>=4 && cfg->dy>=4)
  {
   Trect pictRect=pict.getRect(cfg->full,cfg->half);
   return (unsigned int)cfg->x<=pictRect.dx-1 && (unsigned int)cfg->y<=pictRect.dy-1 &&
          (unsigned int)cfg->x+cfg->dx<=pictRect.dx && (unsigned int)cfg->y+cfg->dy<=pictRect.dy;
  }
 else
  return false;
}

HRESULT TimgFilterLogoaway::process(TfilterQueue::iterator it,TffPict &pict,const TfilterSettingsVideo *cfg0)
{
 if (is(pict,cfg0))
  {
   const TlogoawaySettings *cfg=(const TlogoawaySettings*)cfg0;
   init(pict,cfg->full,cfg->half);
   bool cspChanged;
   if (cfg->lumaonly)
    cspChanged=getCurNext(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,COPYMODE_DEF,&plane[0].dst,NULL,NULL,NULL);
   else
    cspChanged=getCurNext(FF_CSPS_MASK_YUV_PLANAR,pict,cfg->full,COPYMODE_DEF,&plane[0].dst,&plane[1].dst,&plane[2].dst,NULL);

   if (cspChanged || logotemp.rectFull.dx!=(unsigned int)cfg->dx || logotemp.rectFull.dy!=(unsigned int)cfg->dy || !plane[0] || oldLumaOnly!=cfg->lumaonly || oldBlur!=cfg->blur || oldMode!=cfg->mode || stricmp(oldparambitmap,cfg->parambitmap)!=0)
    {
     oldLumaOnly=cfg->lumaonly;oldBlur=cfg->blur;oldMode=cfg->mode;
     ff_strncpy(oldparambitmap, cfg->parambitmap, countof(oldparambitmap));
     done();
     if (cfg->mode==TlogoawaySettings::MODE_SHAPEXY || cfg->mode==TlogoawaySettings::MODE_SHAPEUGLARM)
      {
       parambitmap=new TffPict(csp2,cfg->parambitmap,parambitmapbuf,deci);
       if (parambitmap->rectFull.dx!=(unsigned int)cfg->dx || parambitmap->rectFull.dy!=(unsigned int)cfg->dy)
        {
         delete parambitmap;
         parambitmap=NULL;
        }
      }
     for (int i=0;i<(cfg->lumaonly?1:3);i++)
      {
       plane[i].shiftX=pict.cspInfo.shiftX[i];plane[i].shiftY=pict.cspInfo.shiftY[i];
       plane[i].w=cfg->dx>>plane[i].shiftX;plane[i].h=cfg->dy>>plane[i].shiftY;
       if (parambitmap)
        {
         plane[i].parambitmapdata=parambitmap->data[i];
         plane[i].parambitmapstride=parambitmap->stride[i];
        }
       else
        {
         plane[i].parambitmapdata=NULL;
         plane[i].parambitmapstride=0;
        }
       plane[i].init(cfg);
      }
    }

   Trect tempR(cfg->x,cfg->y,cfg->dx,cfg->dy);
   logotemp.copyFrom(pict,logotempbuf,&tempR);

   YUVcolor yuv(cfg->solidcolor);
   for (int i=0;i<(cfg->lumaonly?1:3);i++)
    {
     plane[i].stride2=stride2[i];
     plane[i].logotempdata=logotemp.data[i];plane[i].logotempstride=logotemp.stride[i];
     plane[i].solidcolor=(i==0?yuv.Y:(i==1?yuv.V+128:yuv.U+128));
     plane[i].process(cfg);
    }
  }
 return parent->deliverSample(++it,pict);
}
