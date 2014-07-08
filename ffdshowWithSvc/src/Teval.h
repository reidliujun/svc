#ifndef _TEVAL_H_
#define _TEVAL_H_

class Teval
{
public:
 struct Tvariable
  {
   const char_t *name;
   const double *valPtr;
  };
private:
 ffstring expr;
 const Tvariable *vars;
 const char_t *c;
 bool getVariable(const char_t *name,double *val);
 void next(void);
 void unknown(char_t *s);
 void syntax(void);
 double constant(void);
 double function(void);
 double term(void);
 static inline double factorial(double v);
 double H(void),G(void),F(void),E(void),C(void),L(void),S(void);
public:
 Teval(const char_t *Ic,const Tvariable *Ivars);
 double operator ()(const char_t* *errPtr);
};

#endif
