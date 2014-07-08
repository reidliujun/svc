#ifndef _RATIONAL_H_
#define _RATIONAL_H_

#include "libavutil/rational.h"

struct Rational :safe_bool<Rational>
{
 int num;
 int den;

 Rational(void):num(0),den(0) {}
 Rational(int Inum,int Iden):num(Inum),den(Iden) {}
 Rational(unsigned int Inum,unsigned int Iden):num(Inum),den(Iden) {}
 Rational(DWORD Inum,DWORD Iden):num(Inum),den(Iden) {}
 Rational(const AVRational &ar):num(ar.num),den(ar.den) {}

 Rational(double d, int max)
  {
   #pragma warning (push)
   #pragma warning (disable:4244)
   int exponent=std::max((int)(log(abs(d)+1e-20)/log(2.0)),0);
   #pragma warning (pop)
   int64_t den64= int64_t(1) << (61 - exponent);
   lavc_reduce(&num, &den, (int64_t)(d * den64 + 0.5), den64, max);
  }
/*
 operator double()
  {
   return num/(double)den;
  }*/
 operator AVRational() const
  {
   AVRational a;
   a.num=num;a.den=den;
   return a;
  }

 Rational operator+(const Rational &c)
  {
   lavc_reduce(&num, &den, num * (int64_t)c.den + c.num * (int64_t)den, den * (int64_t)c.den, INT_MAX);
   return *this;
  }
 Rational operator-(const Rational &c)
  {
   lavc_reduce(&num, &den, num * (int64_t)c.den - c.num * (int64_t)den, den * (int64_t)c.den, INT_MAX);
   return *this;
  }
 Rational& reduce(int max)
  {
   lavc_reduce(&num,&den,num,den,max);
   return *this;
  }
 bool operator!=(const Rational &b) const
  {
   const int64_t tmp= num * (int64_t)b.den - b.num * (int64_t)den;
   if (tmp!=0 ||
       (num==0 && den==0 && (b.num!=0 || b.den!=0)) ||
       (b.num==0 && b.den==0 && (num!=0 || den!=0))) return true;
   else
    return false;
  }
 bool boolean_test() const
  {
   return num || den;
  }
};

#endif
