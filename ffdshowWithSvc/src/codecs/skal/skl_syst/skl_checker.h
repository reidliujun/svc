/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl_checker.h
 *
 * Managment and report of check errors
 ********************************************************/

#ifndef _SKL_CHECKER_H_
#define _SKL_CHECKER_H_

//////////////////////////////////////////////////////////

extern int Skl_Nb_Check, Skl_Nb_Error, Skl_Nb_Flt_Error;
extern int Skl_Show_Flt_Error;

extern void Skl_Check_Print();

extern void Skl_PError_Bool(int Line, const char *What);
extern void Skl_PError_Str(int Line, SKL_CST_STRING v1, const char *What1, SKL_CST_STRING v2, const char *What2);
extern void Skl_PError_Int(int Line, int v1, const char *What1, int v2, const char *What2);
extern void Skl_PError_Ptr(int Line, SKL_ANY v1, const char *What1, SKL_ANY v2, const char *What2);
extern void Skl_PError_UInt(int Line, SKL_UINT32 v1, const char *What1, SKL_UINT32 v2, const char *What2);
extern void Skl_PError_Eq(int Line, const char *What1, const char *What2);
extern void Skl_PError_Float(int Line, const char *What1, float v1, const char *What2, float v2, float delta, float eps);
extern void Skl_PError_Int_Eps(int Line, const char *What1, int v1, const char *What2, int v2, int delta, int eps);

//////////////////////////////////////////////////////////

#endif  /* _SKL_CHECKER_H_ */
