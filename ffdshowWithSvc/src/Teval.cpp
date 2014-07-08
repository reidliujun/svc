/*
 * Copyright (c) 2005,2006 Milan Cutka
 * based on e, a tiny expression evaluator by Dimitromanolakis Apostolos
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "Teval.h"

Teval::Teval(const char_t *Ic,const Tvariable *Ivars):expr(Ic),vars(Ivars)
{
 expr.ConvertToLowerCase();
 expr=stringreplace(expr,_l(" and "),_l("&&"),rfReplaceAll);
 expr=stringreplace(expr,_l(" or "),_l("||"),rfReplaceAll);
 expr=stringreplace(expr,_l(" "),_l(""),rfReplaceAll);
 expr=stringreplace(expr,_l(","),_l("."),rfReplaceAll);
}

double Teval::operator ()(const char_t* *errPtr)
{
 c=expr.c_str();
 try
  {
   double res=S();
   if (errPtr) *errPtr=NULL;
   return res;
  }
 catch(const char_t *err)
  {
   if (errPtr) *errPtr=err;
   return HUGE_VAL;
  }
 catch(...)
  {
   if (errPtr) *errPtr=_l("unknown");
   return HUGE_VAL;
  }
}

void Teval::next(void)
{
 c++;
}
void Teval::unknown(char_t *s)
{
 throw "unknown function";
}
void Teval::syntax(void)
{
 throw "syntax error";
}
double Teval::constant(void)
{
 double r = 0;

 while(*c >= '0' && *c <= '9')
  {
   r = 10*r + (*c-'0');
   next();
  }

 if(*c == '.')
  {
   double p = 0.1;
   next();

   while(*c >= '0' && *c <= '9')
    {
     r += p * (*c-'0'); p /= 10;
     next();
    }
  }

 if(*c == 'e' || *c == 'E')
  {
   double m = 1;

   next();
   if(*c == '-')
    {
     m = -m; next();
    }
   else if(*c == '+')
    next();
   r *= pow(10.0,m*term());
  }
 return r;
}

bool Teval::getVariable(const char_t *name,double *val)
{
 if      (strcmp(name,_l("pi"))==0) {*val=M_PI;return true;}
 else if (strcmp(name,_l("e") )==0) {*val=M_E;return true;}
 else
  {
   for (const Tvariable *var=vars;var && var->name;var++)
    if (strcmp(var->name,name)==0)
     {
      *val=*var->valPtr;
      return true;
     }
   return false;
  }
}

double Teval::function(void)
{
 char_t f[20], *p;
 double v;

 p = f;
 while (p-f < 19 && *c >= 'a' && *c <= 'z')
  {
   *p++ = *c;
   next();
  }

 *p = 0;

 double var;
 if (getVariable(f,&var)) return var;

 v = term();

 if      (strcmp(f,_l("abs")   )==0) return fabs(v);
 else if (strcmp(f,_l("fabs")  )==0) return fabs(v);
 else if (strcmp(f,_l("floor") )==0) return floor(v);
 else if (strcmp(f,_l("ceil")  )==0) return ceil(v);
 else if (strcmp(f,_l("sqrt")  )==0) return sqrt(v);
 else if (strcmp(f,_l("exp")   )==0) return exp(v);

 else if (strcmp(f,_l("sin")   )==0) return sin(v);
 else if (strcmp(f,_l("cos")   )==0) return cos(v);
 else if (strcmp(f,_l("tan")   )==0) return tan(v);
 else if (strcmp(f,_l("asin")  )==0) return asin(v);
 else if (strcmp(f,_l("acos")  )==0) return acos(v);
 else if (strcmp(f,_l("atan")  )==0) return atan(v);

 else if (strcmp(f,_l("sinh")  )==0) return sinh(v);
 else if (strcmp(f,_l("cosh")  )==0) return cosh(v);
 else if (strcmp(f,_l("tanh")  )==0) return tanh(v);

 else if (strcmp(f,_l("ln")    )==0) return log(v);
 else if (strcmp(f,_l("log")   )==0) return log(v)/log(2.0);
 else unknown(f);
 return 0;
}
double Teval::term(void)
{
 /*double m = 1;*/
 const int m = 1;

 /*
   if(*c == '-') { m = -m; next(); }
   else
   if(*c == '+') next();
 */

 if(*c=='(' || *c=='[')
  {
   double r;

   next();
   r = L();
   if(*c != ')' && *c !=']') syntax();

   next();
   return m * r;
  }
 else if((*c >= '0' && *c <= '9') || *c == '.')
  return m * constant();
 else if(*c >= 'a' && *c <= 'z')
  return m * function();
 else
  return 0;
}
inline double Teval::factorial(double v)
{
 double i, r = 1;

 for(i=2;i<=v;i++)
  r *= i;
 return r;
}
double Teval::H(void)
{
 double r = term();

 if(*c == '!')
  {
   next();
   r = factorial(r);
  }
 return r;
}
double Teval::G(void)
{
 double q, r = H();

 while(*c == '^')
  {
   next();
   q = G();
   r = pow(r,q);
  }

 return r;
}
double Teval::F(void)
{
 double r = G();

 while(1)
  {
   if(*c=='*') { next(); r *= G(); }
   else if(*c=='/') { next(); r /= G(); }
   else if(*c=='%') { next(); r = fmod(r,G()); }
   else break;
  }

 return r;
}
double Teval::E(void)
{
 double r = F();

 while(1)
  {
   if(*c=='+') { next(); r += F(); }
   else if(*c=='-') { next(); r -= F(); }
   else break;
  }

 return r;
}
double Teval::C(void)
{
 double r = E();

 while(1)
  {
   if (*c=='=')
    {
     next();
     r=r==E()?1:0;
    }
   else if (*c=='<')
    {
     next();
     if (*c=='>')
      {
       next();
       r=r!=E()?1:0;
      }
     else if (*c=='=')
      {
       next();
       r=r<=E()?1:0;
      }
     else
      r = r<E()?1:0;
    }
   else if(*c=='>')
    {
     next();
     if (*c=='=')
      {
       next();
       r = r>=E()?1:0;
      }
     else
      r = r>E()?1:0;
    }
   else break;
  }

 return r;
}
double Teval::L(void)
{
 double r = C();

 while(1)
  {
   if(*c=='&' && *(c+1)=='&') { next();next(); r = r && C(); }
   else if(*c=='|' && *(c+1)=='|') { next();next(); r = r || C(); }
   else break;
  }

 return r;
}

double Teval::S(void)
{
 double r = L();

 if(*c != 0) syntax();
 return r;
}
