/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl_rdtsc.h
 *
 * time stamp (INTEL only)
 ********************************************************/

#ifndef _SKL_RDTSC_H_
#define _SKL_RDTSC_H_

#ifdef SKL_USE_ASM    // SKL_USE_RDTSC?!

//////////////////////////////////////////////////////////

extern "C" {

  extern SKL_INT32 Skl_RCount_[16];
  extern SKL_INT32 Skl_Cur_Count_;
  extern SKL_INT32 Skl_Tics_;
  extern SKL_INT32 Skl_EAX_In_;
  extern SKL_INT32 Skl_EBX_In_;
  extern SKL_INT32 Skl_ECX_In_;
  extern SKL_INT32 Skl_EDX_In_;
  extern SKL_INT32 Skl_EDI_In_;
  extern SKL_INT32 Skl_ESI_In_;
  extern SKL_INT32 Skl_EBP_In_;
  extern SKL_INT32 Skl_ESP_In_;
  extern SKL_INT32 Skl_EAX_Out_;
  extern SKL_INT32 Skl_EBX_Out_;
  extern SKL_INT32 Skl_ECX_Out_;
  extern SKL_INT32 Skl_EDX_Out_;
  extern SKL_INT32 Skl_EDI_Out_;
  extern SKL_INT32 Skl_ESI_Out_;
  extern SKL_INT32 Skl_EBP_Out_;
  extern SKL_INT32 Skl_ESP_Out_;
  extern float Skl_f_In_[8];
  extern float Skl_f_Out_[8];
  extern void SKL_RDTSC_0_ASM();
  extern void SKL_RDTSC_1_ASM();
  extern void SKL_RDTSC_2_ASM();
  extern void Skl_Print_Tics( SKL_INT32 Offset );
}

#define SKL_RDTSC_IN  \
  SKL_RDTSC_0_ASM(); \
  for(Skl_Cur_Count_=15;Skl_Cur_Count_>=0;--Skl_Cur_Count_) {\
    SKL_RDTSC_1_ASM()

#define SKL_RDTSC_OUT \
    SKL_RDTSC_2_ASM(); }  \
  Skl_Print_Tics(27)

//////////////////////////////////////////////////////////
#else
//////////////////////////////////////////////////////////

#include "skl_syst/skl_exception.h"
#define SKL_RDTSC_IN
#define SKL_RDTSC_OUT throw SKL_MSG_EXCEPTION( " No ASM timing available for RDTSC." )

//////////////////////////////////////////////////////////

#endif  /* SKL_USE_ASM  */
#endif  /* _SKL_RDTSC_H_ */
