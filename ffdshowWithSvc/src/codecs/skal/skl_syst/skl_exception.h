/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl_exception.h
 *
 * exception classes
 ********************************************************/

#ifndef _SKL_EXCEPTION_H_
#define _SKL_EXCEPTION_H_

//////////////////////////////////////////////////////////
//
// This comment was for old compilers, horny with exceptions
// (for instance, egcs-2.91.57 is really sensible to this mess):
//

// -- We embed exception throwing into a global func
// (at the expense of some compiler's warning about
//  some return being unreachable...).
// All SKL_EXCEPTION must remain a pure virtual
// object (only containing the _vptr).
// Do NOT even think of making the Skl_Throw take
// a "const SKL_EXCEPTION e". And re-read the Stroustrup
// to find out why :((
// Every *_EXCEPTION should only consist of a vtbl pointer.
// It'll be copied by Skl_Throw(), and then re-thrown...
//
//extern void Skl_Throw( const SKL_EXCEPTION &e );
//
//

//
// But now, we have modern compilers and use something like:
//

//#define Skl_Throw(e) throw( (const SKL_EXCEPTION &)(e) )
//#define Skl_Throw(e) throw( (e) )
extern void Skl_Throw( const SKL_EXCEPTION &e );

//////////////////////////////////////////////////////////

class SKL_MSG_EXCEPTION : public SKL_EXCEPTION {
  public:
    SKL_MSG_EXCEPTION(SKL_CST_STRING Msg, ...);
    virtual ~SKL_MSG_EXCEPTION() {}
};

class SKL_MEM_EXCEPTION : public SKL_EXCEPTION {
  private:
    static size_t _Size;
  public:
    SKL_MEM_EXCEPTION(SKL_CST_STRING Msg, int Size=0);
    virtual ~SKL_MEM_EXCEPTION() {}
    virtual void Print() const;
};

class SKL_ASSERT_EXCEPTION : public SKL_EXCEPTION {
  private:
    static char _Text[MAX_LEN];
    static char _File[MAX_LEN];
    static int  _Line;
  public:
    SKL_ASSERT_EXCEPTION(SKL_CST_STRING Msg,
                         SKL_CST_STRING Text,
                         SKL_CST_STRING File,
                         int Line);
    virtual ~SKL_ASSERT_EXCEPTION() {}
    virtual void Print() const;
};

//////////////////////////////////////////////////////////

#endif  /* _SKL_EXCEPTION_H_ */
