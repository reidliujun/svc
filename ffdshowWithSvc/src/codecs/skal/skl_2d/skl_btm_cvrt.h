/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl_btm_cvrt.h
 *
 * convertion between bitmaps format and colorspaces
 ********************************************************/

#ifndef _SKL_BTM_CVRT_H_
#define _SKL_BTM_CVRT_H_

#include "skl_2d/skl_btm.h"

//////////////////////////////////////////////////////////
// converters
//////////////////////////////////////////////////////////

extern "C"
SKL_CVRT_FUNC Skl_Get_Cvrt_Convert_Ops(int In_Quantum, int Out_Quantum);
extern "C"
SKL_CVRT_FUNC Skl_Get_Cvrt_Copy_Ops(int Quant);

//////////////////////////////////////////////////////////
// SKL_CONVERTER_MAP
//////////////////////////////////////////////////////////

class SKL_CONVERTER_MAP
{
  private:
    SKL_FORMAT _In, _Out;
    SKL_UINT32 _Map[4][256];  // 4k

    SKL_CVRT_FUNC _Func;

      // yes. private. Use Get_Or_Make_Map() (+Dispose()) instead
    SKL_CONVERTER_MAP(SKL_FORMAT In, SKL_FORMAT Out);

    void Init_Colormapper();
    void Init_Converter();
    void Init_Copier();

    static void Make_Convert_Table(SKL_FORMAT In, SKL_FORMAT Out,
                                   SKL_UINT32 *M1, SKL_UINT32 *M2,
                                   SKL_UINT32 *M3, SKL_UINT32 *M4);

  public:
    static SKL_CONVERTER_MAP *Get_Or_Make_Map(SKL_FORMAT In, SKL_FORMAT Out);
    ~SKL_CONVERTER_MAP() {}

    void Dispose();

    SKL_UINT32 *Get_Map()             { return _Map[0]; }
    SKL_CVRT_FUNC Get_Func()    const { return _Func; }
    SKL_FORMAT Get_In_Format()  const { return _In; }
    SKL_FORMAT Get_Out_Format() const { return _Out; }

    void Convert_And_Store_CMap(SKL_BTM &Dst, const SKL_CMAP_X &CMap);
    void Store_RGB_Cube( SKL_BTM *Dst );

    void Print_Infos() const;
};

//////////////////////////////////////////////////////////
// SKL_CONVERTER
//////////////////////////////////////////////////////////

class SKL_BTM_CVRT : public SKL_BTM
{
  private:
    SKL_BTM           *_Dst;
    SKL_CONVERTER_MAP *_Map;

  protected:
    SKL_BTM *Get_Dst() const { return _Dst; }

  public:

    SKL_BTM_CVRT(SKL_MEM_I *Mem, SKL_FORMAT Fmt, SKL_BTM &Dst);
    virtual ~SKL_BTM_CVRT();
    virtual void Kill();

    virtual void Store_CMap();
    virtual void Unlock() { Convert_To(*Get_Dst(), _Map); }

    virtual void Print_Infos() const;
};

//////////////////////////////////////////////////////////

#endif /* _SKL_BTM_CVRT_H_ */
