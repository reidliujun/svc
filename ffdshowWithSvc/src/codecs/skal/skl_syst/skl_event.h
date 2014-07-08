/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl_event.h
 *
 * base class for event queue
 ********************************************************/

#ifndef _SKL_EVENT_H_
#define _SKL_EVENT_H_

#include "skl.h"
#include "skl_2d/skl_pt.h"

//////////////////////////////////////////////////////////
// SKL_EVENT
//////////////////////////////////////////////////////////

class SKL_EVENT
{
    SKL_DEL_OP(SKL_EVENT)

  public:

    typedef int (*EVENT_HANDLER)(SKL_EVENT *);

    typedef enum {
      NO_EVENT   = 0x00,
      CLICK1     = 0x01,
      CLICK2     = 0x02,
      CLICK3     = 0x04,
      MOVE       = 0x08,
      RELEASE1   = 0x10,
      RELEASE2   = 0x20,
      RELEASE3   = 0x40,
      KEY_PRESS  = 0x80,
      CLICK      = (0x01|0x02|0x04),
      RELEASE    = (0x10|0x20|0x40),
      HAS_DATA   = 0x100
    } EVENT_TYPE;

    typedef enum {
      NO_MODIFIER    = 0x00,
      ALT_MODIFIER   = 0x01,
      CTRL_MODIFIER  = 0x02,
      SHIFT_MODIFIER = 0x04,
      UP_MODIFIER    = 0x08,
      DOWN_MODIFIER  = 0x10,
      LEFT_MODIFIER  = 0x20,
      RIGHT_MODIFIER = 0x40
    } MODIFIER;

    typedef enum {
      ESCAPE=0xff,
      F1=0xfe, F2=0xfd, F3=0xfc, F4=0xfb,
      F5=0xfa, F6=0xf9, F7=0xf8, F8=0xf7,
      F9=0xf6,F10=0xf5,F11=0xf4,F12=0xf3
    } SPECIAL_KEYS;

  protected:

    int       _Type;
    int       _Key, _Modifier;
    SKL_2D_PT _Pos;
    SKL_ANY   _Data;

  public:

    SKL_EVENT() : _Type(NO_EVENT), _Modifier(NO_MODIFIER) {}
    ~SKL_EVENT() {}

    void Reset()                             { _Type=NO_EVENT; _Modifier=0; _Key=0; }

    int Get_Type() const                     { return _Type; }
    int Has_Type(EVENT_TYPE Type) const      { return ((_Type&Type)!=0); }
    int Is_Empty() const                     { return (_Type==NO_EVENT); }
    void Set_Type(EVENT_TYPE Type)           { _Type = Type; }
    void Add_Type(EVENT_TYPE Type)           { _Type |= Type; }
    void Add_Types(int Types)                { _Type |= Types; }
    void Remove_Type(EVENT_TYPE Type)        { _Type &= ~Type; }

    void Set_Data( SKL_ANY Data )            { Add_Type(HAS_DATA); _Data = Data; }
    SKL_ANY Get_Data() const                 { return _Data; }

    void Set_Key(int Key)                    { Add_Type(KEY_PRESS); _Key = Key; }
    int Get_Key() const                      { return _Key; }

    void Add_Modifier(int Modifier)          { _Modifier |= Modifier; }
    int Get_Modifier() const                 { return _Modifier; }
    int Is_Modified_By(int Modifier) const   { return ((_Modifier&Modifier)!=0); }

    void Set_Position(int x, int y)          { _Pos = SKL_2D_PT(x,y); } /* no Type set here.. */
    SKL_2D_PT Get_Position() const           { return _Pos; }
    SKL_2D_PT &Get_Position()                { return _Pos; }
    void Get_Position(int &x, int &y) const  { x = _Pos.x; y = _Pos.y; }
    int Get_x() const                        { return _Pos.x; }
    int Get_y() const                        { return _Pos.y; }
};

//////////////////////////////////////////////////////////

#endif  /* _SKL_EVENT_H_ */
