/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl_destroy.h
 *
 * "automatically destroyed objects"
 ********************************************************/

#ifndef _SKL_DESTROY_H_
#define _SKL_DESTROY_H_

//////////////////////////////////////////////////////////

class SKL_DESTROYABLE {
  private:
    SKL_DESTROYABLE *_Next_Destroyable;
    void Add_Me() const;
    void Remove_Me();
  protected:
    SKL_DESTROYABLE();
  public:
    virtual void Suicide();
    virtual ~SKL_DESTROYABLE();
    static void Kill_All();
};

//////////////////////////////////////////////////////////

#endif   /* _SKL_DESTROY_H_ */
