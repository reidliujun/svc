/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl_window.h
 *
 * basic window class
 ********************************************************/

#ifndef _SKL_WINDOW_H_
#define _SKL_WINDOW_H_

#include "skl_2d/skl_btm.h"
#include "skl_2d/skl_btm_cvrt.h"
#include "skl_syst/skl_event.h"

//////////////////////////////////////////////////////////
// SKL_WINDOW
//////////////////////////////////////////////////////////

class SKL_WINDOW : public SKL_BTM
{
    SKL_DEL_OP(SKL_WINDOW)

  protected:

    int            _Xo, _Yo;
    SKL_CST_STRING _Name;
    int            _Show;
    SKL_EVENT      _Event;
    SKL_WINDOW    *_Next;

  public:

    SKL_WINDOW(SKL_MEM_I* Mem, SKL_CST_STRING Name=0);
    virtual ~SKL_WINDOW();

    const SKL_EVENT *Get_Event();
    void Refresh();

    virtual void Set_Position(int Xo, int Yo)         { _Xo=Xo;  _Yo=Yo; }
    virtual void Get_Position(int &Xo, int &Yo) const {  Xo=_Xo; Yo=_Yo; }
    virtual int Get_X_Position()                const { return _Xo; }
    virtual int Get_Y_Position()                const { return _Yo; }
    virtual void Set_Name(SKL_CST_STRING Name)        { _Name = Name; }
    virtual SKL_CST_STRING Get_Name()           const { return _Name; }
    virtual void Hide()                               { _Show = 0; }
    virtual void Show()                               { _Show = 1; }
    virtual int Is_Visible() const                    { return (_Show==1); }
    virtual void Get_Event(SKL_EVENT &Event)          { }
    virtual void Print_Infos() const;

    SKL_WINDOW *Get_Next() const { return _Next; }
    void Set_Next(SKL_WINDOW *W) { _Next = W; }
};

//////////////////////////////////////////////////////////
// SKL_WINDOW_CVRT
//////////////////////////////////////////////////////////

class SKL_WINDOW_CVRT : public SKL_WINDOW
{
    SKL_DEL_OP(SKL_WINDOW_CVRT)

  private:

    SKL_WINDOW        *_Dst;
    SKL_CONVERTER_MAP *_Map;

  protected:

    SKL_WINDOW *Get_Dst() const { return _Dst; }

  public:

    SKL_WINDOW_CVRT(SKL_MEM_I *Mem, SKL_FORMAT In_Format, SKL_WINDOW &Dst);
    virtual ~SKL_WINDOW_CVRT();

    virtual void Store_CMap();
    virtual void Unlock() { Convert_To(*Get_Dst(), _Map); }


      // relay toward the real window

    virtual void Set_Position(int Xo, int Yo)  { _Dst->Set_Position(Xo, Yo); }
    virtual void Get_Position(int &Xo, int &Yo) const  { _Dst->Get_Position(Xo, Yo); }
    virtual int Get_X_Position() const         { return _Dst->Get_X_Position(); }
    virtual int Get_Y_Position() const         { return _Dst->Get_Y_Position(); }
    virtual void Set_Name(SKL_CST_STRING Name) { SKL_WINDOW::Set_Name(Name); _Dst->Set_Name(Name); }
    virtual SKL_CST_STRING Get_Name() const    { return _Dst->Get_Name(); }
    virtual void Get_Event(SKL_EVENT &Event)   { _Dst->Get_Event(Event); }
    virtual void Hide()                        { _Dst->Hide(); }
    virtual void Show()                        { _Dst->Show(); }
    virtual int Is_Visible() const             { return _Dst->Is_Visible(); }
    virtual void Print_Infos() const;
};

//////////////////////////////////////////////////////////

#endif  /* _SKL_WINDOW_H_ */
