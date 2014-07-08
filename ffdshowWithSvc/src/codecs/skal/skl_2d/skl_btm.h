/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl_btm.h
 *
 * basic bitmap class
 ********************************************************/

#ifndef _SKL_BTM_H_
#define _SKL_BTM_H_

#include "skl_2d/skl_cmap.h"

class SKL_FILE;

class SKL_CONVERTER_MAP;
#define SKL_CVRT_SIGNATURE(NAME) void NAME(SKL_BYTE *Dst, int Dst_BpS,        \
                                           const SKL_BYTE *Src, int Src_BpS,  \
                                           int W, int H,                      \
                                           const SKL_UINT32 *Map)
typedef SKL_CVRT_SIGNATURE( (*SKL_CVRT_FUNC) );

//////////////////////////////////////////////////////////
// Bitmaps
//////////////////////////////////////////////////////////

class SKL_BTM
{
  private:

    SKL_MEM_I  *_Mem;
    int         _W, _H, _BpS, _Quantum;
    SKL_BYTE   *_Data;
    size_t      _Data_Size;
    SKL_FORMAT  _Format;
    SKL_CMAP_X *_CMap;
    int         _Owns_CMap;

    void Apply_Func(SKL_BTM &Out, const SKL_CVRT_FUNC Func, const SKL_UINT32 *Map);

    void Set_Data_Size(size_t Size);

  public:

    SKL_BTM(SKL_MEM_I *Mem);
    virtual ~SKL_BTM();
    virtual void Reset();

    void Set(int W, int H, SKL_FORMAT Format, int BpS=0, SKL_CMAP_X *CMap=0);
    void Set_Virtual(int W, int H, SKL_FORMAT Format = 0,
                     SKL_BYTE *Ptr = 0,
                     int BpS=0, const SKL_CMAP_X *CMap=0);

    int Is_Virtual() const { return (_Data_Size==0); }

    void Make_Copy(const SKL_BTM *In);
    void Make_Virtual_Copy(const SKL_BTM *In);

      // color map

    const SKL_CMAP_X &Get_CMap() const     { SKL_ASSERT(_CMap!=0); return *_CMap; }
    const SKL_CMAP_X *Get_CMap_Ptr() const { return _CMap; }
    SKL_CMAP_X *Get_CMap_Ptr()             { return _CMap; }
    int Owns_CMap() const                  { return _Owns_CMap; }
    void Clear_CMap();
    void New_CMap(int Nb=256);
    int Has_CMap() const                   { return (_CMap!=0); }
    int Get_Nb_Colors() const              { return (Has_CMap() ? Get_CMap().Get_Nb_Colors() : 0); }

        // cmap : hard copy

    void Set_CMap(const SKL_CMAP_X &CMap);
    void Set_CMap(const SKL_CMAP &CMap)    { Set_CMap(SKL_CMAP_X(CMap)); }

        // cmap : ref copy

    void Set_CMap(const SKL_CMAP_X *CMap);

        // Do what should with the colormap, once it has been set...

    virtual void Store_CMap() {}

      // accessors

    int Width() const   { return _W; }
    int bWidth() const  { return _W*_Quantum; }
    int Height() const  { return _H; }
    int BpS() const     { return _BpS; }          // BYTE per scanline
    int PpS() const     { return _BpS/_Quantum; } // PIXEL per scanline
    int Quantum() const { return _Quantum; }
    SKL_FORMAT Format()  const { return _Format; }
    int Is_Colormapped() const { return (Format()==SKL_FORMAT::Colormapped()); }
    int Is_Alpha() const       { return (Format()==SKL_FORMAT::Alpha()); }

    virtual SKL_BYTE *Lock() { return _Data; }
    virtual void Unlock() {}
    SKL_BYTE *Lock(const int x, const int y) { return Lock() + y*BpS() + x*Quantum(); }

    SKL_BYTE *Next_Scan(SKL_BYTE *Ptr) const { return Ptr + BpS(); }

    SKL_MEM_I *Get_Mem() const { return _Mem; }

      // misc ops

    void Remap(SKL_BTM &out, SKL_UINT32 Map[256]);
    void Copy_To(SKL_BTM &out);
    void Convert_To(SKL_BTM &Out, SKL_CONVERTER_MAP *Map=0);
    void Clear();

      // debug

    virtual void Print_Infos() const;
};

//////////////////////////////////////////////////////////

#endif /* _SKL_BTM_H_ */
