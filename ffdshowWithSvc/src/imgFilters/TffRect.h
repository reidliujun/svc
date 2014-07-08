#ifndef _TFFRECT_H_
#define _TFFRECT_H_

#include "rational.h"
#include "Crect.h"

struct Trect
{
 Trect(void):sar(1,1) {x=y=dx=dy=0;}
 Trect(const RECT &r):sar(1,1) {x=r.left;y=r.top;dx=r.right-r.left+1;dy=r.bottom-r.top+1;}
 Trect(const Trect &Ir,const Rational &Isar):sar(1,1) {x=Ir.x;y=Ir.y;dx=Ir.dx;dy=Ir.dy;}
 Trect(unsigned int Ix,unsigned int Iy,unsigned int Idx,unsigned int Idy):x(Ix),y(Iy),dx(Idx),dy(Idy),sar(1,1) {}
 Trect(unsigned int Ix,unsigned int Iy,unsigned int Idx,unsigned int Idy,unsigned int sarx,unsigned int sary):x(Ix),y(Iy),dx(Idx),dy(Idy),sar(sarx,sary) {}
 Trect(unsigned int Ix,unsigned int Iy,unsigned int Idx,unsigned int Idy,const Rational &Isar):x(Ix),y(Iy),dx(Idx),dy(Idy),sar(Isar) {}
 unsigned int x,y,dx,dy;
 Rational sar;
 const Rational dar(void) const
  {
   return (sar.num && sar.den && dx && dy)?Rational(sar.num*dx,sar.den*dy).reduce(32768):Rational(dx,dy);
  }
 void setDar(const Rational &dar)
  {
   sar=(dar.num && dar.den)?Rational((dy*dar.num)/double(dx*dar.den),32768):Rational(1,1);
  }
 operator RECT(void)
  {
   RECT ret={x,y,dx-1,dy-1};
   return ret;
  }
 operator CRect(void)
  {
   return CRect(CPoint(x,y),CSize(dx,dy));
  }
 static void calcNewSizeAspect(const Trect &in,int a1,int a2,Trect &out)
  {
   if ((in.dx << 16) / in.dy > ((unsigned int)a1 << 16) / (unsigned int)a2)
    {
     out.dy = a2 * in.dx / a1;
     out.dx= in.dx;
    }
   else
    {
     out.dx = a1 * in.dy / a2;
     out.dy = in.dy;
    }
   if (out.dx&3) out.dx=(out.dx/4+1)*4;
   if (out.dy&1) out.dy=(out.dy/2+1)*2;
  }
};
inline bool operator !=(const Trect &r1,const Trect &r2)
{
 return (r1.x!=r2.x) || (r1.y!=r2.y) || (r1.dx!=r2.dx) || (r1.dy!=r2.dy);
}
inline bool operator ==(const Trect &r1,const Trect &r2)
{
 return (r1.x==r2.x) && (r1.y==r2.y) && (r1.dx==r2.dx) && (r1.dy==r2.dy);
}

#endif
