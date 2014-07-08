#ifndef _TTIMER_H_
#define _TTIMER_H_

#include "ffdebug.h"

class Ttimer
{
private:
 const char_t *name;
 int64_t t,t1;
 double freq;
 #ifndef __GNUC__
 static int64_t read_counter(void)
  {
   int64_t ts;
   uint32_t ts1, ts2;
   __asm
    {
     rdtsc
     mov ts1, eax
     mov ts2, edx
    }
   ts = ((uint64_t) ts2 << 32) | ((uint64_t) ts1);
   return ts;
 }
 #else
 static int64_t read_counter(void)
  {
   int64_t ts;
   uint32_t ts1, ts2;
   __asm__ __volatile__("rdtsc\n\t":"=a"(ts1), "=d"(ts2));
   ts = ((uint64_t) ts2 << 32) | ((uint64_t) ts1);
   return ts;
  }
 #endif
 static double get_freq(void)
  {
   int64_t x, y;
   int32_t i;

   i = time(NULL);
   while (i == time(NULL));

   x = read_counter();
   i++;

   while (i == time(NULL));
   y = read_counter();
   return (double) (y - x) / 1000.;
  }
public:
 Ttimer(const char_t *Iname)
  {
   //freq=get_freq();
   name=Iname;
   t=0;
  }
 ~Ttimer()
  {
   DPRINTF(_l("timer %s: %12I64i"),name,t);
  }
 void start(void)
  {
   t1=read_counter();
  }
 void stop(void)
  {
   t+=read_counter()-t1;
  }
};
class TtimerAuto
{
private:
 Ttimer *t;
 bool own;
public:
 TtimerAuto(const char_t *name):t(new Ttimer(name)),own(true)
  {
   t->start();
  }
 TtimerAuto(Ttimer &It):t(&It),own(false)
  {
   t->start();
  }
 ~TtimerAuto()
  {
   t->stop();
   if (own)
    delete t;
  }
};

#endif
