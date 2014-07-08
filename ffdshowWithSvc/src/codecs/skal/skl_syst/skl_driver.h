/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl_driver.h
 *
 *  basic driver interface
 ********************************************************/

#ifndef _SKL_DRIVER_H_
#define _SKL_DRIVER_H_

#include "skl.h"

//////////////////////////////////////////////////////////

class SKL_DRIVER_I
{
    SKL_DEL_OP(SKL_DRIVER_I)

  private:

    SKL_CST_STRING _Name;

  protected:

    int _Ok;
    void Set_Ok()     { _Ok = 1; }
    void Set_Not_Ok() { _Ok = 0; }

  public:

    SKL_DRIVER_I(SKL_CST_STRING Name=0);
    virtual ~SKL_DRIVER_I();

    int Ok() const                     { return (_Ok!=0); }
    SKL_CST_STRING Get_Name() const    { return _Name; }
    void Set_Name(SKL_CST_STRING Name) { _Name = Name; }

     // returns true if driver is 'physically' (hardware)
     // ok (although maybe not initialized).
    virtual int Ping() = 0;

    virtual void Stand_By();
    virtual void Wake_Up();

    virtual void Print_Infos() const;   // debug
};

//////////////////////////////////////////////////////////

#endif  /* _SKL_DRIVER_H_ */
