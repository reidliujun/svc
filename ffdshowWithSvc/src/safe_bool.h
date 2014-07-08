#ifndef _SAFE_BOOL_H_
#define _SAFE_BOOL_H_

// copied from the The Safe Bool Idiom article by Bjorn Karlsson
// http://www.artima.com/cppsource/safebool3.html

class safe_bool_base
{
public: //protected
 typedef void (safe_bool_base::*bool_type)() const;
 void this_type_does_not_support_comparisons() const {}

 safe_bool_base() {}
 safe_bool_base(const safe_bool_base&) {}
 safe_bool_base& operator=(const safe_bool_base&) {return *this;}
 ~safe_bool_base() {}
};

template <typename T> class safe_bool : public safe_bool_base
{
public:
 operator bool_type() const
  {
   return (static_cast<const T*>(this))->boolean_test() ? &safe_bool_base::this_type_does_not_support_comparisons : 0;
  }
protected:
  ~safe_bool() {}
};

template <typename T, typename U> void operator==(const safe_bool<T>& lhs,const safe_bool<U>& rhs)
{
 lhs.this_type_does_not_support_comparisons();
 return false;
}

template <typename T,typename U> void operator!=(const safe_bool<T>& lhs,const safe_bool<U>& rhs)
{
 lhs.this_type_does_not_support_comparisons();
 return false;
}

#endif
