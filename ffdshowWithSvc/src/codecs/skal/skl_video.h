/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl_video.h
 *
 * video drivers
 ********************************************************/

#ifndef _SKL_VIDEO_H_
#define _SKL_VIDEO_H_

#ifndef SKL_NO_VIDEO

#include "skl_syst/skl_driver.h"
#include "skl_2d/skl_window.h"
#include "skl_syst/skl_destroy.h"
#include "skl_syst/skl_event.h"

//////////////////////////////////////////////////////////

class SKL_VIDEO_I : public SKL_DRIVER_I, private SKL_DESTROYABLE
{
  private:

    SKL_MEM_I           *_Mem;
    SKL_BTM            **_Modes;
    int                  _Nb_Modes;
    int                  _Max_Modes;
    int                  _Fullscreen_Mode_Nb;
    SKL_WINDOW          *_Fullscreen_Mode;
    SKL_WINDOW          *_Windows;

    void Close_All_Windows();
    int Is_Better_Than(const SKL_BTM * const a, const SKL_BTM * const b) const;
    SKL_WINDOW *Set_Fullscreen_Mode_Internal(int Mode);

    void Add_Mode(int i, SKL_BTM *Mode);

  protected:

    void Add_Mode(SKL_BTM*);
    void Clear_Modes();

  public:

      // capabilities

    typedef enum {
      NONE           = 0,
      HAS_FULLSCREEN = 1,
      HAS_BACKBUFFER = 2
    } CAPS;
    CAPS _Caps;

      // options

    // yes. i know. This is against OO rules to make subclasses'
    // specific stuff (params) "bubble up" to base class. But
    // compared to the price of having to make public such uninteresting
    // implementation code as, say, the DDraw video driver...
    // Or: would you like all X11's stuff to be included just to
    // have a Use_Shm() method handy? Well...
    // So, each sub-implementation will plug its own params needs
    // in this enum. Period.

    typedef enum {

      USE_DISPLAY,
      USE_SHM,
      USE_DGA,

      USE_SAFE_SVGALIB,

      VSYNC,
      DITHER,
      REFRESH_RATE,

      HIDE_FULLSCREEN,
      SHOW_FULLSCREEN

    } PARAM;

  protected:

      // to be implemented by subclasses

    virtual int Set_Param(PARAM opt, SKL_CST_STRING SValue, int IValue) = 0;
    virtual int Get_Param(PARAM opt, SKL_CST_STRING *SValue, int *IValue) const = 0;

    virtual SKL_WINDOW *Set_Fullscreen_Mode(const SKL_BTM *Mode, int Show=1) = 0;
    virtual void Shutdown_Fullscreen() = 0;

    virtual SKL_WINDOW *Open_Window(int W, int H, SKL_FORMAT Fmt,
                                    int With_Backbuffer=0,
                                    int Show=1) = 0;
    virtual int Needs_Conversion(const SKL_WINDOW *Win,
                                 SKL_FORMAT Fmt) const;
  public:

    SKL_VIDEO_I(SKL_MEM_I *Mem, SKL_CST_STRING Name=0);
    virtual ~SKL_VIDEO_I();

    void Print_Modes_Infos() const;
    void Print_Windows_Infos() const;
    virtual void Print_Infos() const;
    virtual void Hide(SKL_WINDOW &m) { m.Hide(); }
    virtual void Show(SKL_WINDOW &m) { m.Show(); }

    SKL_MEM_I *Get_Mem() const { return _Mem; }

    int Set_String_Param(PARAM Opt, SKL_CST_STRING value);
    int Set_Int_Param(PARAM Opt, int value);
    SKL_CST_STRING Get_String_Param(PARAM Opt) const;
    int Get_Int_Param(PARAM Opt) const;
    int Has_Fullscreen() const { return (_Caps&HAS_FULLSCREEN)!=0; }
    int Has_Backbuffer() const { return (_Caps&HAS_BACKBUFFER)!=0; }

      // fullscreen modes
      // Mode number -1 means 'text mode' (no fullscreen mode)

    int Get_Nb_Modes() const { return _Nb_Modes; }
    const SKL_BTM *Get_Mode_Infos(int Nb) const;
    int Search_Best_Mode(int w, int h, SKL_FORMAT fmt) const;

      // API for setting fullscreen mode
    int Is_Fullscreen() const { return (_Fullscreen_Mode!=0); }
    int Get_Fullscreen_Mode_Nb() const { return _Fullscreen_Mode_Nb; }
    SKL_WINDOW *Get_Fullscreen_Mode() const { return _Fullscreen_Mode; }
    SKL_WINDOW *Fullscreen_Mode(int Mode_Nb);
    SKL_WINDOW *Fullscreen_Mode(int Width,
                                int Height,
                                SKL_FORMAT Fmt);
    void Close_Fullscreen();

      // API for creating window

    SKL_WINDOW *Create_Window(int W, int H,
                              SKL_FORMAT fmt,
                              int With_Backbuffer=0,
                              int Show=1);
};

//////////////////////////////////////////////////////////

#endif  /* SKL_NO_VIDEO */
#endif  /* _SKL_VIDEO_H_ */
