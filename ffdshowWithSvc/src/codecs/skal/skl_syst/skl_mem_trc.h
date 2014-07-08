/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl_mem_trc.h
 *
 * Tracing memory module
 ********************************************************/

#ifndef _SKL_MEM_TRC_H_
#define _SKL_MEM_TRC_H_

//////////////////////////////////////////////////////////
// SKL_MEM_TRC_I -- tracing memory allocator
//////////////////////////////////////////////////////////

class SKL_MEM_TRC_I : public SKL_MEM_I
{
  private:
    enum { MAX_SLOT = 1000 };
    int _Allocated, _Max;
    int _Last_Slot, _Max_Slots;
    struct SLOT { SKL_ANY _Ptr; size_t _Size; } *_Slots;
    int _Verbose;
    void Reset();
    int Grow();

  public:
    SKL_MEM_TRC_I(int Verbose=0);
    virtual ~SKL_MEM_TRC_I();

    virtual SKL_ANY New(const size_t s);
    virtual void Delete(const SKL_ANY p, size_t Size);

    void Print_Infos() const;
    void Set_Verbose(int Level=1);      // 0: all quiet
                                        // 1: report errors
                                        // 2: print all
};

//////////////////////////////////////////////////////////

#endif  /* _SKL_MEM_TRC_H_ */
