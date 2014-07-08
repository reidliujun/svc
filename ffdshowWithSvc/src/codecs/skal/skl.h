/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl.h
 *
 * Base headers and #def. First file to include.
 ********************************************************/

#ifndef _SKL_H_
#define _SKL_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h> // for memcpy()...

//////////////////////////////////////////////////////////
// some global #define
//////////////////////////////////////////////////////////
//
//  TODO: should be in some "config.h"

#ifdef _WINDOWS

#define SKL_EXPORT __declspec(dllexport)

#if defined(_MSC_VER) && (_MSC_VER>=1200)
#define SKL_MSVC6
#else
#define SKL_MSVC5
#endif

#else

#define SKL_EXPORT

#endif

//////////////////////////////////////////////////////////
// Some size-controlled types
//////////////////////////////////////////////////////////
/** @file */

#ifdef _WINDOWS
typedef __int64 SKL_INT64;                /**< type for storing exactly 64bits, signed */
typedef unsigned __int64 SKL_UINT64;      /**< type for storing exactly 64bits, unsigned */
#else
typedef long long int SKL_INT64;          /**< type for storing exactly 64bits, signed */
typedef unsigned long long int SKL_UINT64;/**< type for storing exactly 64bits, unsigned */
#endif

typedef int SKL_INT32;                /**< type for storing exactly 32bits, signed */
typedef unsigned int SKL_UINT32;      /**< type for storing exactly 32bits, unsigned */
typedef short SKL_INT16;              /**< type for storing exactly 16bits, signed */
typedef unsigned short SKL_UINT16;    /**< type for storing exactly 16bits, unsigned */
typedef signed char SKL_INT8;         /**< type for storing exactly 8bits, signed */      // <- Irix-cc *requires* 'signed'!
typedef unsigned char SKL_UINT8;      /**< type for storing exactly 8bits, unsigned */

typedef void *SKL_ANY;                /**< generic pointer type */
typedef unsigned char SKL_BYTE;       /**< byte type (8bits, unsigned) */
typedef char *SKL_STRING;             /**< generic string type */
typedef const char * SKL_CST_STRING;  /**< generic constant string type */

#ifdef SKL_64BITS
typedef SKL_UINT64 SKL_SAFE_INT;      /**< generic integer type capable of safely storing 64bits or 32bits pointers (depending on the platform) */
#else
typedef SKL_UINT32 SKL_SAFE_INT;      /**< generic integer type capable of safely storing 64bits or 32bits pointers (depending on the platform) */
#endif

//////////////////////////////////////////////////////////
// Debug utilities
//////////////////////////////////////////////////////////

#define SKL_NYI abort()

#if 0 //#ifdef __ANSI_CPP__
#define SKL_MK_STRING(x)  "x"
#else
#define SKL_MK_STRING(x)  #x
#endif

  /** When NDEBUG is _not_ defined (debug mode, e.g.), a
      faulty SKL_ASSERT( condition ) will call this function.
      Its main purpose is for you to be able to put
      a breakpoint with a debugger (beside ensuring
      that no assert is violated, of course).
   */
extern void Skl_Do_Assert(SKL_CST_STRING Condition,
                          SKL_CST_STRING File,
                          int Line,
                          SKL_CST_STRING Msg);

#ifndef NDEBUG

  /** Replacement for assert(), calling Skl_Do_Assert() */
#define SKL_ASSERT(EX)                                \
  ((void) ((EX) ||                                    \
   (Skl_Do_Assert( (SKL_CST_STRING)SKL_MK_STRING(EX), \
                   (SKL_CST_STRING)__FILE__, (int)__LINE__, 0), 0)))

#else

  /** Replacement for assert(), calling Skl_Do_Assert() */
#define SKL_ASSERT(a) ((void)0)

#endif

//////////////////////////////////////////////////////////
// Exceptions forward dcl (cf. skl_syst/skl_exception.h)
//////////////////////////////////////////////////////////

class SKL_EXCEPTION {
  protected:
    enum { MAX_LEN=2048 };
  public:
    static char _Msg[MAX_LEN];
  public:
    SKL_EXCEPTION(SKL_CST_STRING Msg=0);
    virtual ~SKL_EXCEPTION() {}
    virtual void Print() const;
  	static SKL_CST_STRING Get_Message() { return _Msg; }
};

//////////////////////////////////////////////////////////
// -- Memory managment interface
// Default implementation uses malloc() / free()
//////////////////////////////////////////////////////////

  /** Instance of this class should provide interface for memory allocation
      and disallocation. The default implementation uses malloc() / free()
      low-level calls.<br>
      A new-with-placement operator is defined, taking the memory pool
      as placement data. You can then use: new (Mem) MY_CLASS to allocate
      MY_CLASS on the SKL_MEM_I *Mem memory pool. <br>
      For convenience, a default global static instance of SKL_MEM_I
      is available as: SKL_MEM_I *SKL_MEM.<br>
    */
class SKL_MEM_I
{
  public:

    SKL_MEM_I();
    virtual ~SKL_MEM_I();

      /** This function should return a pointer to a freshly allocation
          memory segment of at least 'Size' size. It returns 0
          if allocation was not possible.
          @param Size the desired size to be allocated.
        */
    virtual SKL_ANY New(const size_t Size);

      /** This function should disallocate the memory segment pointed to
          by the 'p' parameter, and of size 'Size'.
          @param p Pointer to the memory segment. This pointer should have
          been obtained by a call to New() member function.
          @param Size size of the memory to be freed. This size *must* match
          the size that was passed to the corresponding New() call as argument.
        */
    virtual void Delete(const SKL_ANY p, size_t Size);

      /** This function re-allocates, without copying its content, a memory
          segment of size 'Old_Size' referenced by 'Ptr'. The new size is
          passed a 'Size' argument.
        */
    void Realloc(SKL_ANY &Ptr,
                  const size_t Size,         // if Old_Size>0 -> "Grow" to Size
                  const size_t Old_Size=0 ); // else "Shrink" to Size
};

extern SKL_MEM_I * const SKL_MEM;

//////////////////////////////////////////////////////////
// New/Delete operators (what a mess!)
//////////////////////////////////////////////////////////

  /** Overloaded new-with-placement operator, using SKL_MEM_I.
      @see SKL_MEM_I */
inline void *operator new (size_t s, SKL_MEM_I *m) {
  if (!s) return (void*)s;  // wow!   (this avoids some warning msg)
  if (m==0) return malloc(s);
  return m->New(s);
}

#if !defined(SKL_MSVC5)
inline void *operator new[] (size_t s, SKL_MEM_I *m) {
  if (!s) return (void*)s;  // re-wow!
  if (m==0) return malloc(s);
  return m->New(s);
}

#if defined(SKL_MSVC6)
  // Hmm... There seems to be a bug => I *must*
  // redefine a '::new []' whenever I define a 'new (Mem) []'
inline void *operator new[] (size_t s) {
  if (!s) return (void*)s;  // re-wow!
  return malloc(s);
}
inline void operator delete(void *Ptr) { if (Ptr) free(Ptr); }
inline void operator delete [] (void *Ptr) { if (Ptr) free(Ptr); }
#endif

#endif

#if defined(SKL_MSVC6)

inline void operator delete(void *Ptr, SKL_MEM_I *m) {}

#define SKL_DEL_OP(T) \
public:               \
 /* inline void operator delete(void *Ptr, SKL_MEM_I *m) {} */\
private:

#else

#define SKL_DEL_OP(T)
#endif

//////////////////////////////////////////////////////////
// System dependant useful macros
//////////////////////////////////////////////////////////

  // system dependant...
#define SKL_MEMMOVE(dst,src,size) memmove((void*)(dst),(void*)(src),(size_t)(size))
#define SKL_MEMCPY(dst,src,size) memcpy((void*)(dst),(void*)(src),(size_t)(size))
#define SKL_BZERO(ptr,size) memset((void*)(ptr),0,(size_t)(size))
#define SKL_MEMSET(ptr,c,size) memset((void*)(ptr),(unsigned char)(c),(size_t)(size))

//////////////////////////////////////////////////////////

#endif   /* _SKL_H_ */
