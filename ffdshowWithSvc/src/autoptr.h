#ifndef _AUTOPTR_H_
#define _AUTOPTR_H_

template<class T> struct autoptr :safe_bool< autoptr<T> >
{
private:
 T *ptr;
public:
 autoptr(void):ptr(NULL) {}
 autoptr(T* newptr):ptr(NULL)
  {
   assign(newptr);
  }
 ~autoptr()
  {
   release();
  }
 void assign(T* newptr)
  {
   ptr=newptr;
  }
 void release(void)
  {
   if (ptr)
    {
     delete ptr;
     ptr=NULL;
    }
  }
 T* operator =(T* newptr)
  {
   assign(newptr);
   return ptr;
  }
 T& operator *(void) const {return *ptr;}
 T* operator ->(void) const {return ptr;}
 bool boolean_test() const {return !!ptr;}
};

#endif
