/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl_cpu_specs.h
 *
 *  CPU's caps
 ********************************************************/

#ifndef _SKL_CPU_SPECS_H_
#define _SKL_CPU_SPECS_H_

//////////////////////////////////////////////////////////
// Implementation choices

  /** @file */
  /** @enum SKL_CPU_FEATURE
     This enumeration select the CPU instruction set low-level
     functions should use, if available.
     @see Skl_Detect_CPU_Feature
     @see SKL_MP4_ENC::Set_CPU
     @see SKL_MP4_DEC::Set_CPU
    */
typedef enum {
  SKL_CPU_C    = 0, /**< Use C version only */
  SKL_CPU_X86  = 1, /**< Use x86 instruction set */
  SKL_CPU_MMX  = 2, /**< Use MMX instruction set */
  SKL_CPU_SSE  = 3, /**< Use SSE instruction set */
  SKL_CPU_SSE2 = 4, /**< Use SSE2 instruction set */
  SKL_CPU_REF  = 5, /**< Use Reference (slow) implementation */
  SKL_CPU_ALT  = 6, /**< Alternative implementation, for tests. */
  SKL_CPU_LAST = 7,

  SKL_CPU_DETECT = 7  /**< System will attempt to autodetect available CPU features */

} SKL_CPU_FEATURE;

extern "C" {

  /** After autoprobing, this function returns the most efficient
      available feature.
    */
extern SKL_CPU_FEATURE Skl_Detect_CPU_Feature();

typedef void (*SKL_DSP_SWITCH)();
extern void Skl_Switch_MMX();
extern void Skl_Switch_None();
extern SKL_DSP_SWITCH Skl_Get_Switch(SKL_CPU_FEATURE Cpu);

#ifdef SKL_USE_ASM

#ifdef __LINUX__
#define SKL_EMMS asm( "emms" : : )
#else
#define SKL_EMMS _asm emms
#endif

#else

#define SKL_EMMS

#endif  /* SKL_USE_ASM */
}

//////////////////////////////////////////////////////////
#ifdef SKL_USE_ASM

extern "C" {
extern int Skl_Detect_CPUID();
extern int Skl_Detect_MMX();
extern int Skl_Detect_SSE();
extern int Skl_Detect_SSE2();
extern int Skl_Detect_3DNOW();
extern void Skl_Get_CPUID(char Id[12+1]);
extern void Skl_Get_ModelID(char Id[48+1]);
}

#endif  /* SKL_USE_ASM */

//////////////////////////////////////////////////////////

class SKL_CPU_SPECS
{
  private:

    static int  _SType;
    static int  _SFlags;
    static char _Vendor[12+1];
    static char _Model[48+1];

    int _Type;
    int _Flags;

  public:

    enum {
      SKL_INTEL   = 0,
      SKL_AMD     = 1,
      SKL_K6      = 2,
      SKL_ATHLON  = 3,
      SKL_CYRIX   = 4,

      SKL_SGI     = 5,
      SKL_SPARC   = 6,
      SKL_ALPHA   = 7,
      SKL_HP      = 8,
      SKL_UNKNOWN = 9
    };
    enum { SKL_CPUID  =  1,
           SKL_MMX    =  2,
           SKL_SSE    =  4,
           SKL_SSE2   =  8,
           SKL_3DNOW  = 16,
           SKL_3DNOW2 = 32,
           SKL_LAST   = 64
    };

    SKL_CPU_SPECS(int Ignore_Feature=0);
    static void Reset();
    static void Detect();
    static int Check_Types();
    static void Test_Float();

    int Get_Flags() const   { return _Flags; }
    int Get_Type() const    { return _Type; }
    int Is_x86() const      { return (_Type==SKL_INTEL)||(_Type==SKL_AMD); }

    int Has_MMX() const    { return (_Flags&SKL_MMX)!=0; }
    int Has_SSE() const    { return (_Flags&SKL_SSE)!=0; }
    int Has_SSE2() const   { return (_Flags&SKL_SSE2)!=0; }
    int Has_3DNow() const  { return (_Flags&SKL_3DNOW)!=0; }
    int Has_3DNow2() const { return (_Flags&SKL_3DNOW2)!=0; }

    void Print_Infos() const;
};

//////////////////////////////////////////////////////////

#endif  /* _SKL_CPU_SPECS_H_ */
