/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl_utils.h
 *
 *  Header for utilities library
 ***************************************************/

#ifndef _SKL_UTILS_H_
#define _SKL_UTILS_H_

#include "skl.h"

//////////////////////////////////////////////////////////

class SKL_BTM;
class SKL_CMAP_X;
class SKL_FILE;
class SKL_FILE_I;
class SKL_SPRITE;
class SKL_WINDOW;
class SKL_COLOR;

//////////////////////////////////////////////////////////
// Some raisonable default drivers
//////////////////////////////////////////////////////////

class SKL_VIDEO_I;

#ifdef SKL_USE_X11
#define SKL_SYSTEM_VIDEO_DRIVER SKL_X11_VIDEO_I
#define SKL_SYSTEM_VIDEO_LIB "libskl_drv_video"
#endif

#ifdef SKL_USE_SVGA
#ifndef SKL_SYSTEM_VIDEO_DRIVER
#define SKL_SYSTEM_VIDEO_DRIVER SKL_SVGA_VIDEO_I
#define SKL_SYSTEM_VIDEO_LIB "libskl_drv_video"
#endif
#endif

#ifdef _WINDOWS
#define SKL_SYSTEM_VIDEO_DRIVER SKL_DDRAW_VIDEO_I
#define SKL_SYSTEM_VIDEO_LIB "skl_drv_ddraw"
#endif

//////////////////////////////////////////////////////////
// -- sku_show.cpp
//////////////////////////////////////////////////////////

#include "skl_2d/skl_cmap.h"

SKL_WINDOW *Sku_Get_Video(int Width, int Height, SKL_FORMAT Fmt);
extern void Sku_Show_Pic(SKL_BTM *Pic);
extern void Sku_Show_CMap(SKL_BTM *Pic);
extern int  Sku_Get_Key(SKL_WINDOW *Win=0);
extern int Sku_Get_Key_Mouse(int &x, int &y, int &Key, SKL_WINDOW *Win=0);
extern int  Sku_Wait_Key(SKL_WINDOW *Win=0);
extern void Sku_Show_Close();

//////////////////////////////////////////////////////////

extern void Sku_Save_PPM(SKL_CST_STRING Name, SKL_BTM &Btm, int Use_PGM=0);
extern void Sku_Dump_Pic(SKL_BTM *Pic);

//////////////////////////////////////////////////////////

#endif  /* _SKL_UTILS_H_ */
