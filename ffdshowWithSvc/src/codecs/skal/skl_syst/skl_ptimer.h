/********************************************************
 * Some code. Copyright (C) 2003 by Pascal Massimino.   *
 * All Rights Reserved.      (http://skal.planet-d.net) *
 * For Educational/Academic use ONLY. See 'LICENSE.TXT'.*
 ********************************************************/
/*
 * skl_ptimer.h
 *
 * poor man's timer & stats
 ********************************************************/

#ifndef _SKL_PTIMER_H_
#define _SKL_PTIMER_H_

#include <time.h>

//////////////////////////////////////////////////////////
// Timing
//////////////////////////////////////////////////////////

class SKL_PTIMER
{
  private:
    clock_t _Start, _Delta;
    long    _Count;
    enum { TICKS = CLOCKS_PER_SEC };

  public:
    SKL_PTIMER() { Reset(); }

    void operator++()      { _Count++; }
    void operator++(int)   { _Count++; }
    void operator--()      { _Count--; }
    void operator--(int)   { _Count--; }
    long Get_Count() const { return _Count; }

    void Elapsed_s(SKL_CST_STRING s=0, SKL_CST_STRING e=0);
    void Elapsed_FPS(SKL_CST_STRING s=0, SKL_CST_STRING e=0);

    void Start() { _Start = (long)clock(); _Delta=0; }
    float Stop() { _Delta = clock() - _Start; return Get_Sec(); }
    void Reset() { Start(); _Count = 0; }

    float Sec() const      { return 1.0f*(clock() - _Start); }
    float mSec() const     { return 1000.0f*(clock() - _Start); }
    float Get_Sec() const  { return 1.0f*_Delta/TICKS; }
    float Get_mSec() const { return 1000.0f*_Delta/TICKS; }

    void Wait(float ms) const;
};

//////////////////////////////////////////////////////////
// statistics / profiling
//  Warning! This class rely on destructor of statics
// to print infos. Don't use heartlightedly in Dll...
//////////////////////////////////////////////////////////

class SKL_CALL_STAT
{
  private:
    enum { TICKS = CLOCKS_PER_SEC, MAX_STR=128 };

    char           _Name[MAX_STR+1];
    clock_t        _Run_Time, _Cur;
    SKL_UINT32     _dTicksA, _dTicksD;
    SKL_UINT32     _TicksA, _TicksD;
    int            _Calls, _tCalls;
    SKL_CALL_STAT *_Next;

    int Ok() const { return (0!=_Calls || 0!=_tCalls); }

  public:
    SKL_CALL_STAT(SKL_CST_STRING Name);
    ~SKL_CALL_STAT() {}
    void Set_Name(SKL_CST_STRING Name);
    void Print_Stats();
    SKL_CALL_STAT *Get_Next() const { return _Next; }

    void Record();
    void Pause();
    void Record_Ticks();
    void Pause_Ticks();

    static SKL_CALL_STAT *Search(SKL_CST_STRING Name);
    static void Record(SKL_CST_STRING Name) { SKL_CALL_STAT::Search(Name)->Record(); }
    static void Pause(SKL_CST_STRING Name) { SKL_CALL_STAT::Search(Name)->Pause(); }
    static void Record_Ticks(SKL_CST_STRING Name) { SKL_CALL_STAT::Search(Name)->Record_Ticks(); }
    static void Pause_Ticks(SKL_CST_STRING Name) { SKL_CALL_STAT::Search(Name)->Pause_Ticks(); }
};

  // -- simple easy-to-use wrapper

class SKL_STAT
{
  private:
    SKL_CALL_STAT *_Stat;

  public:
    SKL_STAT(SKL_CST_STRING Name)
      : _Stat(SKL_CALL_STAT::Search(Name)) { _Stat->Record(); }
    ~SKL_STAT()         { _Stat->Pause(); }
    void Record()       { _Stat->Record(); }
    void Pause()        { _Stat->Pause(); }
    void Record_Ticks() { _Stat->Record_Ticks(); }
    void Pause_Ticks()  { _Stat->Pause_Ticks(); }
};

//////////////////////////////////////////////////////////

#endif  /* _SKL_PTIMER_H_ */
