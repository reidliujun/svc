/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl_dyn_load.h
 *
 * dynamic loading of libraries
 * + basic Factory (No RTTI. Use with care.)
 ********************************************************/

#ifndef _SKL_DYN_LOAD_H_
#define _SKL_DYN_LOAD_H_

#include "skl.h"

//////////////////////////////////////////////////////////
// SKL_DYN_LOADER -- dynamic module loading
// TODO : versioning control is not done yet
//////////////////////////////////////////////////////////

class SKL_DYN_LOADER
{
  private:
    int            _Sym_Count;
    SKL_ANY        _Handle;
    SKL_CST_STRING _Module;
    SKL_INT32      _Version;

  public:
    SKL_DYN_LOADER() : _Sym_Count(0), _Handle(0), _Module(0), _Version(0) {}
    SKL_DYN_LOADER(SKL_CST_STRING Module, const SKL_INT32 Version=0, int Load_Now=0);
    ~SKL_DYN_LOADER() { Unload(); }

    SKL_INT32 Get_Version() const { return _Version; }
    int Ok() const                { return (_Handle!=0); }

    void Set_Target(SKL_CST_STRING Module, const SKL_INT32 Version);

    int Load();
    SKL_ANY Load_Symbol(SKL_CST_STRING Symbol);
    void Unload();
};

//////////////////////////////////////////////////////////
// SKL_SYMBOL_MAPPER -- Proxy
//////////////////////////////////////////////////////////

class SKL_SYMBOL_MAPPER
{
  private:
    SKL_DYN_LOADER      &_Lib;
    SKL_ANY            (*_New)();
    SKL_CST_STRING const _Symb;
    void Map();
    void Unmap();

  public:
    SKL_SYMBOL_MAPPER(SKL_DYN_LOADER &Lib, SKL_CST_STRING Symb)
      : _Lib(Lib), _New(0), _Symb(Symb) {}
    ~SKL_SYMBOL_MAPPER()   { Unmap(); }
    int Is_Mapped() const  { return (_New!=0); }
    SKL_ANY Build()        { if (!Is_Mapped()) Map();
                             return _New(); }
    void Dispose()         { Unmap(); }
};

//////////////////////////////////////////////////////////
//   User side
// Public macros to use "SYMBOL" dynamically.
// "Automagic" Dynamic Loading is switched on when
// SKL_USE_DYN_LOAD is defined.
// Otherwise, you can use a .dll or a static linkage.
//////////////////////////////////////////////////////////

// workaround for strange pre-processors behaviour
#define SKL_CAT(A,B)  (A ## B)
#define SKL_CAT_STR(A,B)  SKL_MK_STRING(A ## B)

#ifdef SKL_USE_DYN_LOAD

// -- Public macros to use "SYMBOL" dynamically.

#define SKL_DECLARE_DYN_LIBRARY(DYN_LIB_NAME) \
      extern SKL_DYN_LOADER DYN_LIB_NAME

#define SKL_DECLARE_DYN_OBJECT(OBJ) \
      extern SKL_SYMBOL_MAPPER SKL_CAT(OBJ, _Builder)

#define SKL_DYN_LIBRARY(DYN_LIB_NAME, MOD, VERSION) \
      SKL_DYN_LOADER DYN_LIB_NAME(MOD,VERSION)

#define SKL_DYN_OBJECT(OBJ, DYN_LIB_NAME) \
      SKL_SYMBOL_MAPPER SKL_CAT(OBJ, _Builder) ( DYN_LIB_NAME, SKL_CAT_STR( OBJ, _Build ) )

#define SKL_DYN_INSTANCE(OBJ, INST) \
      ((INST*) SKL_CAT(OBJ, _Builder) . Build())

#define SKL_DYN_LIBRARY_UNLOAD(LIB) \
      LIB.Unload()

#define SKL_LOAD_DYN_SYMBOL(DYN_LIB_NAME, SYMB) DYN_LIB_NAME.Load_Symbol(#SYMB)

//////////////////////////////////////////////////////////
#else   // no dll. simply return the static builder
//////////////////////////////////////////////////////////


#define SKL_DECLARE_DYN_LIBRARY(DYN_LIB_NAME) \
    extern void * DYN_LIB_NAME /* just to keep compiler happy */

#define SKL_DECLARE_DYN_OBJECT(OBJ) \
    extern "C" SKL_EXPORT SKL_ANY SKL_CAT(OBJ, _Build)()

#define SKL_DYN_LIBRARY(NAME, MOD, VERSION)

#define SKL_DYN_OBJECT(OBJ, DYN_LIB_NAME)

#define SKL_DYN_INSTANCE(OBJ, INST) \
    ((INST*) SKL_CAT(OBJ, _Build) ())

#define SKL_DYN_LIBRARY_UNLOAD(LIB)

#define SKL_LOAD_DYN_SYMBOL(DYN_LIB_NAME, SYMB) SYMB

#endif  /* !SKL_USE_DYN_LOAD */



//////////////////////////////////////////////////////////
//  Implementor side:
// Factory macros. Must be used once in the module
// where 'SYMBOL' is implemented.
//////////////////////////////////////////////////////////

// -- Factory
#define SKL_DYN_FACTORY(SYMBOL)              \
extern "C" SKL_EXPORT SKL_ANY SYMBOL ## _Build(); \
extern "C" SKL_EXPORT SKL_ANY SYMBOL ## _Build()

// -- Default factory
#define SKL_DFLT_DYN_FACTORY(SYMBOL) \
SKL_DYN_FACTORY(SYMBOL) { return (SKL_ANY)::new SYMBOL; }

//////////////////////////////////////////////////////////

#endif   /* _SKL_DYN_LOAD_H_ */
