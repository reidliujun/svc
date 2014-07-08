/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl_cmap.h
 *
 * cmap utilities
 ********************************************************/

#ifndef _SKL_CMAP_H_
#define _SKL_CMAP_H_

#include "skl_2d/skl_color.h"

//////////////////////////////////////////////////////////
// Pixel format
//////////////////////////////////////////////////////////

class SKL_FORMAT {

  private:

    SKL_UINT32 _Bits;
    enum { BGR_ORDER = 0x8000000 };

  public:

    enum { DFLT = 0x10000,
           COLORMAPPED=0x10000, ALPHA_FMT=0x18000 };

    SKL_FORMAT(SKL_UINT32 f=0) : _Bits(f) {}
    SKL_FORMAT( SKL_ARGB R_msk, SKL_ARGB G_msk,
                SKL_ARGB B_msk, SKL_ARGB A_msk, int Depth=0 );

    operator SKL_UINT32() const  { return _Bits; }

    SKL_UINT32 Bits(int i) const { SKL_ASSERT(i>=0 && i<4);
                                   return (_Bits>>(4*i))&0xf; }
    SKL_UINT32 Bits() const { return (_Bits&0xffff); }
    int Raw_Depth() const   { return (_Bits>>16)&0xf; }
    int Compute_Depth() const;
    int Depth() const;
    void Set_Depth(int d)   { _Bits = Bits() | ((d&0xf)<<16); }

    int Is_BGR() const { return (_Bits & BGR_ORDER); }
    int Is_Compatible_With(SKL_FORMAT Fmt) const {
		  return ( (_Bits&0xf0fff)==(Fmt._Bits&0xf0fff) ); }

    static SKL_FORMAT Colormapped() { return COLORMAPPED; }
    static SKL_FORMAT Alpha() { return ALPHA_FMT; }

    void Print_Infos() const; // debug
};

//////////////////////////////////////////////////////////
// SKL_FORMAT_SHIFT
//////////////////////////////////////////////////////////

class SKL_FORMAT_SHIFT {

  private:

    enum { FIXED = 32 };
    SKL_INT32 _Shift[4];  // a=3, r=2, g=1, b=0
    SKL_ARGB  _Mask[4];

    void Store_Mask_And_Shift(const SKL_FORMAT f);

  public:

    SKL_FORMAT_SHIFT(SKL_FORMAT f) { Store_Mask_And_Shift(f); }
    int Shift(int i)         const { return _Shift[i]; }
    SKL_ARGB Mask(int i)     const { return _Mask[i]; }

    inline SKL_ARGB Unpack(SKL_UINT32 c) const { // unpack to SKL_COLOR
      return (((c&_Mask[0])<<_Shift[0])>>24) |
             (((c&_Mask[1])<<_Shift[1])>>16) |
             (((c&_Mask[2])<<_Shift[2])>> 8);
    }
    inline SKL_UINT32 Pack(SKL_ARGB c) const {
      return (((c<<24)>>_Shift[0])&_Mask[0]) |
             (((c<<16)>>_Shift[1])&_Mask[1]) |
             (((c<< 8)>>_Shift[2])&_Mask[2]);
    }
    inline SKL_ARGB UnpackA(SKL_UINT32 c) const { // unpack with Alpha
      return (((c&_Mask[0])<<_Shift[0])>>24) |
             (((c&_Mask[1])<<_Shift[1])>>16) |
             (((c&_Mask[2])<<_Shift[2])>> 8) |
             (((c&_Mask[3])<<_Shift[3])>> 0);
    }
    inline SKL_UINT32 PackA(SKL_ARGB c) const {
      return (((c<<24)>>_Shift[0])&_Mask[0]) |
             (((c<<16)>>_Shift[1])&_Mask[1]) |
             (((c<< 8)>>_Shift[2])&_Mask[2]) |
             (((c<< 0)>>_Shift[3])&_Mask[3]);
    }

    SKL_ARGB All_Mask() const { return _Mask[0] | _Mask[1] | _Mask[2] | _Mask[3]; }


    void Print_Infos() const;         // debug
    void Print_Col(SKL_UINT32 c) const;
};

//////////////////////////////////////////////////////////
//  SKL_CMAP_X
// class for exchanging color maps.
// colors are in 0x48888 format
//////////////////////////////////////////////////////////

class SKL_CMAP;

class SKL_CMAP_X
{
  protected:

    int _Nb;
    SKL_COLOR _Map[256];

  public:

    SKL_CMAP_X(int Nb=256) : _Nb(Nb) { SKL_ASSERT(Nb>=0 && Nb<=256);
                                       SKL_BZERO(_Map, _Nb*sizeof(_Map[0])); }
    SKL_CMAP_X(const SKL_CMAP &);
    SKL_CMAP_X(const SKL_CMAP_X &In);

    int Get_Nb_Colors() const           { return _Nb; }
    void Set_Nb_Colors(int Nb)          { _Nb = Nb; }
    const SKL_COLOR *Get_Colors() const { return _Map; }
    const SKL_COLOR &operator[](int i) const { SKL_ASSERT(i>=0 && i<_Nb);
                                                return _Map[i]; }
    SKL_COLOR &operator[](int i) { SKL_ASSERT(i>=0 && i<_Nb);
                                    return _Map[i]; }

      // in skl_cmap_mix.cpp

    void Ramp(SKL_COLOR c1, SKL_COLOR c2, int Start=0, int End=256);
    void RGB_Cube( SKL_FORMAT Fmt );
};

//////////////////////////////////////////////////////////
//  SKL_CMAP
// -- formatted CMAPs
//////////////////////////////////////////////////////////

class SKL_CMAP
{
  private:

    int _Nb;
    SKL_UINT32 _Map[256]; // <- not necessarly ARGB_8888...
    SKL_FORMAT _Format;

  public:

    SKL_CMAP(const SKL_FORMAT fmt, int Nb=256);
    SKL_CMAP(const SKL_CMAP_X &In, SKL_FORMAT fmt );
    SKL_CMAP(const SKL_CMAP &In, SKL_FORMAT fmt=0 );

    int Get_Nb_Colors() const            { return _Nb; }
    const SKL_UINT32 *Get_Colors() const { return _Map; }
    SKL_UINT32 &operator[] (int i)       { SKL_ASSERT(i>=0 && i<_Nb);
                                           return _Map[i]; }
    SKL_UINT32 operator[] (int i) const  { SKL_ASSERT(i>=0 && i<_Nb);
                                           return _Map[i]; }

    SKL_FORMAT Get_Format() const   { return _Format; }
    void Set_Format(SKL_FORMAT fmt) { _Format = fmt; }

    void Ramp(SKL_COLOR c1, SKL_COLOR c2, int Start=0, int End=256);
};

//////////////////////////////////////////////////////////

#endif    /* _SKL_CMAP_H_ */
