/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl_pt.h
 *
 *  2D points and rectangles
 ********************************************************/

#ifndef _SKL_PT_H_
#define _SKL_PT_H_

#include "skl.h"

//////////////////////////////////////////////////////////
// SKL_2D_PT / SKL_2D_RECT
//////////////////////////////////////////////////////////

struct SKL_2D_PT {

  int x, y;

  SKL_2D_PT(int xo=0, int yo=0) : x(xo), y(yo) {}

  void operator+=(const SKL_2D_PT &Pt) { x+=Pt.x; y+=Pt.y; }
  void operator-=(const SKL_2D_PT &Pt) { x-=Pt.x; y-=Pt.y; }

  SKL_2D_PT operator+(const SKL_2D_PT &Pt) const { return SKL_2D_PT(x+Pt.x, y+Pt.y); }
  SKL_2D_PT operator-(const SKL_2D_PT &Pt) const { return SKL_2D_PT(x-Pt.x, y-Pt.y); }

  int operator<(const SKL_2D_PT &Pt)  const { return (x==Pt.x) ? (y<Pt.y) : (x<Pt.x); }
  int operator<=(const SKL_2D_PT &Pt) const { return (x<=Pt.x && y<=Pt.y); }
  int operator>(const SKL_2D_PT &Pt)  const { return (x==Pt.x) ? (y>Pt.y) : (x>Pt.x); }
  int operator>=(const SKL_2D_PT &Pt) const { return (x>=Pt.x && y>=Pt.y); }

  int operator==(const SKL_2D_PT &Pt) const { return (x==Pt.x && y==Pt.y); }
};

struct SKL_2D_RECT {

  SKL_2D_PT UL; // Up-Left corner
  SKL_2D_PT BR; // Bottom-Right corner

  SKL_2D_RECT() {}
  SKL_2D_RECT(int xo, int yo, int sx, int sy)
    : UL(xo,yo), BR(xo+sx, yo+sy) { SKL_ASSERT(sx>=0 && sy>=0); }
  SKL_2D_RECT(const SKL_2D_PT &ul, const SKL_2D_PT &size)
    : UL(ul), BR(ul+size) { SKL_ASSERT(size.x>=0 && size.y>=0); }

  SKL_2D_PT Get_Size() const { return (BR-UL); }
  SKL_2D_PT Get_Position() const { return UL; }
  void Set_Size(const SKL_2D_PT &S) { BR = UL+S; }
  void Move(const SKL_2D_PT &T) { SKL_2D_PT Size = Get_Size(); UL += T; BR = UL+Size; }

  void operator+=(const SKL_2D_PT &T) { UL+=T; BR+=T; }
  void operator-=(const SKL_2D_PT &T) { UL-=T; BR-=T; }

  SKL_2D_RECT operator+(const SKL_2D_PT &T) const { return SKL_2D_RECT(UL+T, Get_Size()); }
  SKL_2D_RECT operator-(const SKL_2D_PT &T) const { return SKL_2D_RECT(UL-T, Get_Size()); }

  int operator==(const SKL_2D_RECT &R) const { return (UL==R.UL && BR==R.BR); }

  int Is_Left(const SKL_2D_PT &Pt) const  { return (Pt.x<UL.x); }
  int Is_Right(const SKL_2D_PT &Pt) const { return (Pt.x>=BR.x); }
  int Is_Above(const SKL_2D_PT &Pt) const { return (Pt.y<UL.y); }
  int Is_Below(const SKL_2D_PT &Pt) const { return (Pt.y>=BR.y); }

  int Is_Inside(const SKL_2D_PT &Pt) const { return (Pt>=UL && Pt<BR); }
};

//////////////////////////////////////////////////////////

#endif  /* _SKL_PT_H_ */
