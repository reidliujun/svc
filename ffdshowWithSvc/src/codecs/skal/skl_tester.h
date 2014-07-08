/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl_tester.h
 *
 *   Useful macros for tests
 *
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "skl.h"
#include "skl_syst/skl_checker.h"
#include "skl_syst/skl_exception.h"

//////////////////////////////////////////////////////////

#define SHOW_FLT_ERROR_OFF  Skl_Show_Flt_Error=0
#define SHOW_FLT_ERROR_ON  Skl_Show_Flt_Error=1

#define TEST_FUNC(F) void F() {       \
  printf( "// -- " );           \
  printf( SKL_MK_STRING(F) );   \
  printf( " --\n" );

#define END_FUNC   /*printf( "// --\n" );*/ }


#define SKL_T_START \
  int Skl_Check_n=0; if (argc>1) Skl_Check_n=atoi(argv[1]); \
  try {                                                     \
    printf( "/////////// start ///////////\n" )       \

#define SKL_TEST(n,f) if (Skl_Check_n==(n)||(Skl_Check_n==0)) { f(); }

#define SKL_T_END   \
    printf( "//////////// end ////////////\n" );          \
  } catch( const SKL_EXCEPTION &e ) {                           \
    printf( "ERROR: Caught: "); e.Print(); printf( "\n");       \
    Skl_Nb_Error++;                                             \
  }                                                             \
  Skl_Check_Print()

#define SKL_T_RETURN  return (Skl_Nb_Error+Skl_Nb_Flt_Error)

#define SKL_TRY  try {

#define SKL_CATCH                     \
    printf( "ERROR: Line %d: Exception was not raised!!\n", __LINE__ ); \
    Skl_Nb_Error++;                   \
  } catch( const SKL_EXCEPTION &e ) { \
      printf( "Ok. Caught:" );        \
      e.Print();                      \
      printf("\n");                   \
      Skl_Nb_Check++;                 \
  }

//////////////////////////////////////////////////////////
// various checking functions
//////////////////////////////////////////////////////////

// -- check bool true

#define CHECK(EX) do { Skl_Nb_Check++; \
  if (!(EX)) Skl_PError_Bool(__LINE__, SKL_MK_STRING(EX) ); \
} while(0)

// -- check strings

#define CHECKSTR(EX1, EX2) do { Skl_Nb_Check++; \
  SKL_CST_STRING ex1 = (SKL_CST_STRING)(EX1);  SKL_CST_STRING ex2 = (SKL_CST_STRING)(EX2);  \
  if (strcmp(ex1,ex2)!=0) { Skl_PError_Str(__LINE__, ex1, SKL_MK_STRING(EX1), ex2, SKL_MK_STRING(EX2)); } \
} while(0)


// -- check 32/64b pointers

#define CHECKPTR(EX1, EX2) do { Skl_Nb_Check++; \
  SKL_ANY ex1 = (SKL_ANY)(EX1);  SKL_ANY ex2 = (SKL_ANY)(EX2);  \
  if (ex1!=ex2) { Skl_PError_Ptr(__LINE__, ex1, SKL_MK_STRING(EX1), ex2, SKL_MK_STRING(EX2)); } \
} while(0)

// -- check int

#define CHECKI(EX1, EX2) do { Skl_Nb_Check++; \
  int ex1 = (int)(EX1);  int ex2 = (int)(EX2);  \
  if (ex1!=ex2) { Skl_PError_Int(__LINE__, ex1, SKL_MK_STRING(EX1), ex2, SKL_MK_STRING(EX2)); } \
} while(0)

// -- check unsigned int

#define CHECKUI(EX1, EX2) do { Skl_Nb_Check++; \
  SKL_UINT32 ex1 = (SKL_UINT32)(EX1); SKL_UINT32 ex2 = (SKL_UINT32)(EX2);  \
  if (ex1!=ex2) { Skl_PError_UInt(__LINE__, ex1, SKL_MK_STRING(EX1), ex2, SKL_MK_STRING(EX2)); } \
} while(0)


// -- check equality for class with operator==

#define CHECKEQ(EX1, EX2) do { Skl_Nb_Check++; \
  SKL_BOOL Eq = ((EX1)==(EX2)); \
  if (!Eq) { Skl_PError_Eq( __LINE__, SKL_MK_STRING(EX1), SKL_MK_STRING(EX2)); } \
} while(0)


// -- check equality for class with operator!=

#define CHECKNEQ(EX1, EX2) do { Skl_Nb_Check++; \
  SKL_BOOL nEq = ((EX1)!=(EX2)); \
  if (nEq) { Skl_PError_Eq( __LINE__, SKL_MK_STRING(EX1), SKL_MK_STRING(EX2)); } \
} while(0)

// -- check float with default epsilon

#define CHECKF_EPS 1.0e-6f
#define CHECKF(EX1, EX2) do { Skl_Nb_Check++; \
  float ex1 = (float)(EX1);  float ex2 = (float)(EX2);  float d = (float)fabs(ex2-ex1); \
  if (d>CHECKF_EPS) { Skl_PError_Float(__LINE__, SKL_MK_STRING(EX1), ex1, SKL_MK_STRING(EX2), ex2, d, CHECKF_EPS ); } \
} while(0)

// -- check float with epsilon

#define CHECKFEPS(EX1, EX2, EPS) do { Skl_Nb_Check++; \
  float ex1 = (float)(EX1);  float ex2 = (float)(EX2);  float d = (float)fabs(ex2-ex1); \
  if (d>EPS) { Skl_PError_Float(__LINE__, SKL_MK_STRING(EX1), ex1, SKL_MK_STRING(EX2), ex2, d, EPS ); } \
} while(0)

// -- check int with epsilon

#define CHECKIEPS(EX1, EX2, EPS) do { Skl_Nb_Check++; \
  SKL_UINT32 ex1 = (SKL_UINT32)EX1;  SKL_UINT32 ex2 = (SKL_UINT32)EX2;  SKL_UINT32 d = abs(ex2-ex1); \
  if (d>EPS) { Skl_PError_Int_Eps(__LINE__, SKL_MK_STRING(EX1), ex1, SKL_MK_STRING(EX2), ex2, d, EPS ); } \
} while(0)


//////////////////////////////////////////////////////////
