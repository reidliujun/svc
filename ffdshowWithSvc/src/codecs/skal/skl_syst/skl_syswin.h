/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl_syswin.h
 *
 * Windows-specific system calls
 ********************************************************/

#ifndef _SKL_SYSWIN_H_
#define _SKL_SYSWIN_H_

#ifdef _WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "skl_syst/skl_event.h"
#include "skl_2d/skl_btm.h"

//////////////////////////////////////////////////////////

class SKL_WIN_SYSTEM
{
    SKL_DEL_OP(SKL_WIN_SYSTEM)

  private:

    enum { QUEUE_SIZE = 32 };
    char            _Queue[QUEUE_SIZE];
    int             _Head, _Tail;
    HWND            _Win;
    SKL_WIN_SYSTEM *_Next;
    int             _Fullscreen;
    int             _Win_Shown;
    int             _Is_Active;
    int             _W, _H;

    int Flush_Messages(int Stop_On_Event);

    static SKL_WIN_SYSTEM *_List;
    static SKL_WIN_SYSTEM *_Last;
    static HINSTANCE       _hInst;
    static SKL_BTM         _Display;
    static HWND            _App_Win;

    static HWND Init_Window(HINSTANCE HInst);

  public:

    SKL_WIN_SYSTEM();
    ~SKL_WIN_SYSTEM();
    void Kill();

    HWND Get_Win() const { return _Win; }
    int Is_Shown() const { return (_Win_Shown!=0); }

    int Init_Window();
    int Init_Window(int W, int H, SKL_CST_STRING Name=0,
                    int Show_Me=1);
    void Adjust_Window( int W, int H, SKL_CST_STRING Name=0,
                        int Show_Me=1 );
    void Destroy_Window();
    void Show_Window();
    void Hide_Window();

    void Print_Msg(SKL_CST_STRING Msg, int x=-1, int y=-1) const;
    void Clear() const;

    void Set_Name(SKL_CST_STRING Name);
    void Set_Position(int x, int y);

    void Get_Win_Infos(int &x, int &y, int &Width, int &Height) const;
    void Print_Infos() const;

    int Is_Active() const        { return _Is_Active; }
    void Activate(int Is_Active) { _Is_Active = Is_Active; }

    int Has_Key() const;
    int Get_Key();

    int Queue_Empty() const { return (_Head==_Tail); }
    char Dequeue();
    void Enqueue(char c);

    static SKL_WIN_SYSTEM *Search(HWND w);

    static int Init(HINSTANCE h);
    static void Finish();
    static void Destroy_All_Windows();

    static int Init_Main_Display(SKL_BTM &Dsp);
    static const SKL_BTM &Get_Display() { return _Display; }

    static HWND App_Win() { return _App_Win; }
};

//////////////////////////////////////////////////////////

#endif  /* _WINDOWS */
#endif  /* _SKL_SYSWIN_H_ */
