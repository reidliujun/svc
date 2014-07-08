//  tuple.hpp -----------------------------------------------------

// Copyright (C) 1999, 2000 Jaakko Järvi (jaakko.jarvi@cs.utu.fi)
//
// Permission to copy, use, sell and distribute this software is granted
// provided this copyright notice appears in all copies.
// Permission to modify the code and to distribute modified code is granted
// provided this copyright notice appears in all copies, and a notice
// that the code was modified is included with the copyright notice.
//
// This software is provided "as is" without express or implied warranty,
// and with no claim as to its suitability for any purpose.

// For more information, see http://lambda.cs.utu.fi

// -----------------------------------------------------------------

#ifndef TUPLE_HPP
#define TUPLE_HPP

// ---------------------------------------------------

#define LL_TLAST T10
#define LL_ULAST U10

#define LL_REPEAT0(X) X, X, X, X, X, X, X, X

#define LL_REPEAT1(B, A)\
 B##2 A, B##3 A, B##4 A, B##5 A, B##6 A, B##7 A, B##8 A, B##9 A

#define LL_REPEAT2(B, M, A) B##2 M##2 A, B##3 M##3 A, B##4 M##4 A, \
B##5 M##5 A, B##6 M##6 A, B##7 M##7 A, B##8 M##8 A, B##9 M##9 A

#define LL_REPEAT3(B, M1, M2, A) B##2 M1##2 M2##2 A, B##3 M1##3 M2##3 A,\
B##4 M1##4 M2##4 A, B##5 M1##5 M2##5 A, B##6 M1##6 M2##6 A, \
B##7 M1##7 M2##7 A,B##8 M1##8 M2##8 A, B##9 M1##9 M2##9 A

// some compile time assertion templates ------------------------

namespace std {

namespace tuple_internal {

// -- generate error template, referencing to non-existing members of this
// template is used to produce compilation erros intentionally
template<class T>
class generate_error;

// -- ct_assert<cond>::check leads to a compile time error if cond = false
template <bool Cond> struct ct_assert {
  enum { n = Cond ? 1 : -1 };
  static char check[n];
};

template <bool Cond>
char ct_assert<Cond>::check[n];

} //namespace tuple_internal
// ----------------------------------------------------


// type conversion classes -------------------------------
//   plain_to_c_reference - converts only the plain type to reference to const
//                          identity mapping with all other types
//   plain_to_reference - see above but no const


template<class T>
struct convert {
  typedef const T& plain_to_c_reference;
  typedef T& plain_to_reference;
};

template<class T>
struct convert <const T> {
  typedef const T& plain_to_c_reference;
  typedef const T& plain_to_reference;
};

template<class T>
struct convert <T&> {
  typedef T& plain_to_c_reference;
  typedef T& plain_to_reference;
};

template<class T>
struct convert <const T&> {
  typedef const T& plain_to_c_reference;
  typedef const T& plain_to_reference;
};

template <class T> struct remove_const { typedef T type; };
template <class T> struct remove_const<const T> { typedef T type; };

// -- if construct ------------------------------------------------
// Proposed by Krzysztof Czarnecki and Ulrich Eisenecker

template <bool If, class Then, class Else> struct IF { typedef Then RET; };
template <class Then, class Else> struct IF<false, Then, Else> {
  typedef Else RET;
};
// -----------------------------------------------------------


// reference wrappers --------------------------------------------------

// const_reference_wrapper is used in various functions to  specify
// that a tuple element should be stored as const reference
template<class T> class  const_reference_wrapper;

template <class T> inline const const_reference_wrapper<T> cref(const T& t);

template<class T>
class  const_reference_wrapper {
  const T&  x;
  explicit const_reference_wrapper(const T& t) : x(t) {}
public:
  operator const T&() const { return x; }
  friend const const_reference_wrapper cref<>(const T& t);
};

template <class T>
inline const const_reference_wrapper<T> cref(const T& t) {
  return const_reference_wrapper<T>(t);
}

// reference_wrapper is used to specify that a tuple element should be
// stored as non-const reference
// A disguise for non-const references.
// Can only be created via the ref function, which accepts only
// non-const references. The conversion operator converts the constenss away

template<class T> class reference_wrapper;

template<class T> inline const reference_wrapper<T> ref(T& t);
template<class T> inline const reference_wrapper<T> ref(const T& t);

template<class T>
class reference_wrapper {
  T& x;
  explicit reference_wrapper(T& t) : x(t) {}
public:
  operator T&() const { return x; }

  friend const reference_wrapper ref<>(T& t);
  friend const reference_wrapper ref<>(const T& t);
};

template<class T>
inline const reference_wrapper<T> ref(T& t) { return reference_wrapper<T>(t); }


template <class T>
inline const reference_wrapper<T> ref(const T& t) {
  return tuple_internal::generate_error<T>::const_reference_not_allowed;
}


// -- type conversion templates -------------------------------------
// T1 is the default type, specialisations for
// reference_wrapper, const_reference_wrapper and  array types.
// references to functions require special handling as well

// do not use directly this template directly
// ------------------------------------------------------------------------


// first some helpers
template<class T> struct is_function_reference {
  static const bool test = false;
};

template<class Ret> struct is_function_reference<Ret (...)> {
  static const bool test = true;
};


template<class T1, class T2 = T1>
struct general_type_conversion {
  typedef typename IF<
    is_function_reference<typename remove_const<T1>::type >::test,
                      T1&,
                      T2>::RET type;
};

template<class T, int n, class Any> struct general_type_conversion<T[n], Any> {
  typedef const T (&type)[n];
};

template<class T, int n, class Any>
struct general_type_conversion<const T[n], Any> {
  typedef const T (&type)[n];
};

template<class T, class Any>
struct general_type_conversion<const_reference_wrapper<T>, Any> {
  typedef const T& type;
};
template<class T, class Any>
struct general_type_conversion<const const_reference_wrapper<T>, Any >{
  typedef const T& type;
};

template<class T, class Any>
struct general_type_conversion<reference_wrapper<T>, Any >{
  typedef T& type;
};
template<class T, class Any>
struct general_type_conversion<const reference_wrapper<T>, Any >{
  typedef T& type;
};


// ---------------------------------------------------------------------------
// Use these conversion templates instead of general_type_conversion

// These must be instantiated with plain or const plain types
// from template<class T> foo(const T& t) : convert_to_xxx<const T>::type
// from template<class T> foo(T& t) : convert_to_xxx<T>::type

// The default is plain type -------------------------
// const T -> const T,
// T -> T,
// T[n] -> const T (&) [n]
// references -> compile_time_error
// reference_wrapper<T> -> T&
// const reference_wrapper<T> -> const T&
template<class T>
struct convert_to_plain_by_default {
  typedef typename general_type_conversion<
                     T,
                     T
                   >::type type;
};
template<class T>
struct convert_to_plain_by_default<T&>; // error

// -- nil --------------------------------------------------------
struct nil {};

// a helper function to provide a const nil type temporary
inline const nil cnil() { return nil(); }

// - cons forward declaration -----------------------------------------------
template <class HT, class TT>
struct cons;

// - tuple forward declaration -----------------------------------------------
template <class T1, LL_REPEAT1(class T, = nil), class LL_TLAST = nil> class tuple;

// tuple_length forward declaration
template<class T> struct tuple_length;
// tuple default argument wrappers ---------------------------------------
// Works for const reference and plain types, intentionally not for references


// tuple default argument wrapper ----------------------------------------
template <class T>
struct tuple_default_arg_wrap {
  static T f() { return T(); }
};

template <class T>
struct tuple_default_arg_wrap<T&> {
  static T& f() {
    return tuple_internal::generate_error<T>::no_default_values_for_reference_types;
  }
};
// -----------------------------------------------------------------------

// - cons getters --------------------------------------------------------
// called: tuple_element<3>::get(aTuple)
// -----------------------------------------------------------------------
template< int N >
struct tuple_element {
  template<class RET, class HT, class TT >
  inline static RET get(const cons<HT, TT>& t)
  {
    return tuple_element<N-1>::template get<RET>(t.tail);
  }
  template<class RET, class HT, class TT >
  inline static RET get(cons<HT, TT>& t)
  {
    return tuple_element<N-1>::template get<RET>(t.tail);
  }
};

template<>
struct tuple_element<1> {
  template<class RET, class HT, class TT>
  inline static RET get(const cons<HT, TT>& t)
  {
    return t.head;
  }
  template<class RET, class HT, class TT>
  inline static RET get(cons<HT, TT>& t)
  {
    return t.head;
  }
};

// -cons type accessors ----------------------------------------
// typename tuple_element_type<N,T>::type gets the type of the
// Nth element ot T
// -------------------------------------------------------

template<int N, class T>
struct tuple_element_type
{
  typedef typename T::tail_type Next;
  typedef typename tuple_element_type<N-1, Next>::type type;
};
template<class T>
struct tuple_element_type<1,T>
{
  typedef typename T::head_type type;
};

// -get function templates -----------------------------------------------
// Usage: get<N>(aTuple)

// non-const version, returns a non-const reference
// (unless the element itself is const)

  template<int N, class HT, class TT>
  inline typename convert<
                    typename tuple_element_type<N, cons<HT, TT> >::type
                  >::plain_to_reference
  get(cons<HT, TT>& c) {
    return tuple_element<N>::template get<
             typename convert<
               typename tuple_element_type<N, cons<HT, TT>
             >::type
           >::plain_to_reference>(c);
  }

// const version, returns a const reference to element
  template<int N, class HT, class TT>
  inline typename convert<
                    typename tuple_element_type<N, cons<HT, TT>
                  >::type>::plain_to_c_reference
  get(const cons<HT, TT>& c) {
    return tuple_element<N>::template get<
             typename convert<
               typename tuple_element_type<N, cons<HT, TT>
             >::type
           >::plain_to_c_reference>(c);
  }


// -- the cons template  --------------------------------------------------
template <class HT, class TT>
struct cons {
  typedef HT head_type;
  typedef TT tail_type;
  HT head;
  TT tail;

  template <class T1, LL_REPEAT1(class T,), class LL_TLAST>
  cons( T1& t1, LL_REPEAT2( T, & t,), LL_TLAST& tlast)
  : head (t1) , tail (LL_REPEAT1(t,), tlast, cnil()) {}

   template <class HT2, class TT2>
   cons( const cons<HT2, TT2>& u ) : head(u.head), tail(u.tail) {}

  template <class HT2, class TT2>
  cons& operator=( const cons<HT2, TT2>& u ) {
    head=u.head; tail=u.tail; return *this;
  }

  // must define assignment operator explicitely, implicit version is illformed if HT is a reference
  cons& operator=(const cons& u) { head = u.head; tail = u.tail; return *this; }


  template <class T1, class T2>
  cons& operator=( const std::pair<T1, T2>& u ) {
    tuple_internal::ct_assert<(tuple_length<cons>::value == 2)>(); // check_length = 2
    head = u.first; tail.head = u.second; return *this;
  }

  // get member functions (non-const and const)
  template <int N>
  typename convert<
             typename tuple_element_type<N, cons>::type
           >::plain_to_reference
  get() {
    return std::get<N>(*this); // delegate to non-member get
  }

  template <int N>
  typename convert<
             typename tuple_element_type<N, cons>::type
           >::plain_to_c_reference
  get() const {
    return std::get<N>(*this); // delegate to non-member get
  }


};

template <class HT>
struct cons<HT, nil> {

  typedef HT head_type;
  typedef nil tail_type;

  head_type head;

  template<class T1>
  cons( T1& t1, LL_REPEAT0(const nil&), const nil&)
  : head (t1) {}

  template <class HT2>
  cons( const cons<HT2, nil>& u ) : head(u.head) {}

  template <class HT2>
  cons& operator=(const cons<HT2, nil>& u ) {  head = u.head; return *this; }

  // must define assignment operator explicitely, implicit version is illformed if HT is a reference
  cons& operator=(const cons& u) { head = u.head; return *this; }

  template <int N>
  typename convert<
             typename tuple_element_type<N, cons>::type
           >::plain_to_reference
  get() {
    return std::get<N>(*this);
  }

  template <int N>
  typename convert<
             typename tuple_element_type<N, cons>::type
           >::plain_to_c_reference
  get() const {
    return std::get<N>(*this);
  }
};

// templates for finding out the length of the tuple -------------------

template<class HT, class TT> struct tuple_length<cons<HT, TT> > {
  enum { value = 1 + tuple_length<TT>::value };
};

template<class HT> struct tuple_length<cons<HT, nil> > {
  enum { value = 1 };
};

// If K=tuple type, allows to write tuple_length<K> instead of
// tuple_lenght<typename K::inherited>
template <class T1, LL_REPEAT1(class T,), class LL_TLAST>
struct tuple_length<tuple<T1, LL_REPEAT1(T,), LL_TLAST> >
{
  enum { value = tuple_length<
                   typename tuple<T1, LL_REPEAT1(T,), LL_TLAST>::inherited
                 >::value };
};


// Tuple to cons mapper --------------------------------------------------
template <class T1, LL_REPEAT1(class T,), class LL_TLAST>
struct map_tuple_to_cons
{
  typedef cons<T1,
               typename map_tuple_to_cons<LL_REPEAT1(T,), LL_TLAST, nil>::type
              > type;
};

template <class T1>
struct map_tuple_to_cons<T1, LL_REPEAT0(nil), nil>
{
  typedef cons<T1, nil> type;
};

// -------------------------------------------------------------------
// -- tuple ------------------------------------------------------
template <class T1, LL_REPEAT1(class T,), class LL_TLAST>
class tuple : public map_tuple_to_cons<T1, LL_REPEAT1(T,), LL_TLAST>::type
{
public:
  typedef typename map_tuple_to_cons<T1, LL_REPEAT1(T,), LL_TLAST>::type inherited;

  explicit tuple(typename convert<T1>::plain_to_c_reference t1
          = tuple_default_arg_wrap<T1>::f(),
        LL_REPEAT3(typename convert<T,>::plain_to_c_reference t,
          = tuple_default_arg_wrap<T,>::f()),
        typename convert<LL_TLAST>::plain_to_c_reference tlast
          = tuple_default_arg_wrap<LL_TLAST>::f())
        : inherited(t1, LL_REPEAT1(t,), tlast) {}

  template<class U1, class U2>
  tuple( const cons<U1, U2>& p) : inherited(p) {}

  template <class U1, LL_REPEAT1(class U,), class LL_ULAST>
  tuple& operator=(const tuple<U1, LL_REPEAT1(U,), LL_ULAST>& k) {
    inherited::operator=(k);
    return *this;
  }

  template <class U1, class U2>
  tuple& operator=(const std::pair<U1, U2>& k) {
    tuple_internal::ct_assert<(tuple_length<tuple>::value == 2)>();// check_length = 2
    this->head = k.first;
    this->tail.head =  k.second;
    return *this;
  }

};

// -make_tuple function templates -----------------------------------
template<class T1>
tuple<typename convert_to_plain_by_default<T1>::type> inline make_tuple(const T1& t1) {
  return tuple<typename convert_to_plain_by_default<T1>::type> (t1);
}
template<class T1, class T2>
tuple<typename convert_to_plain_by_default<T1>::type, typename convert_to_plain_by_default<T2>::type> inline make_tuple(const T1& t1, const T2& t2) {
  return tuple<typename convert_to_plain_by_default<T1>::type, typename convert_to_plain_by_default<T2>::type> (t1, t2);
}
template<class T1, class T2, class T3>
tuple<typename convert_to_plain_by_default<T1>::type, typename convert_to_plain_by_default<T2>::type, typename convert_to_plain_by_default<T3>::type> inline make_tuple(const T1& t1, const T2& t2, const T3& t3) {
  return tuple<typename convert_to_plain_by_default<T1>::type, typename convert_to_plain_by_default<T2>::type, typename convert_to_plain_by_default<T3>::type> (t1, t2, t3);
}
template<class T1, class T2, class T3, class T4>
tuple<typename convert_to_plain_by_default<T1>::type, typename convert_to_plain_by_default<T2>::type, typename convert_to_plain_by_default<T3>::type, typename convert_to_plain_by_default<T4>::type> inline make_tuple(const T1& t1, const T2& t2, const T3& t3, const T4& t4) {
  return tuple<typename convert_to_plain_by_default<T1>::type, typename convert_to_plain_by_default<T2>::type, typename convert_to_plain_by_default<T3>::type, typename convert_to_plain_by_default<T4>::type> (t1, t2, t3, t4);
}
template<class T1, class T2, class T3, class T4, class T5>
tuple<typename convert_to_plain_by_default<T1>::type, typename convert_to_plain_by_default<T2>::type, typename convert_to_plain_by_default<T3>::type, typename convert_to_plain_by_default<T4>::type, typename convert_to_plain_by_default<T5>::type> inline make_tuple(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5) {
  return tuple<typename convert_to_plain_by_default<T1>::type, typename convert_to_plain_by_default<T2>::type, typename convert_to_plain_by_default<T3>::type, typename convert_to_plain_by_default<T4>::type, typename convert_to_plain_by_default<T5>::type> (t1, t2, t3, t4, t5);
}
template<class T1, class T2, class T3, class T4, class T5, class T6>
tuple<typename convert_to_plain_by_default<T1>::type, typename convert_to_plain_by_default<T2>::type, typename convert_to_plain_by_default<T3>::type, typename convert_to_plain_by_default<T4>::type, typename convert_to_plain_by_default<T5>::type, typename convert_to_plain_by_default<T6>::type> inline make_tuple(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6) {
  return tuple<typename convert_to_plain_by_default<T1>::type, typename convert_to_plain_by_default<T2>::type, typename convert_to_plain_by_default<T3>::type, typename convert_to_plain_by_default<T4>::type, typename convert_to_plain_by_default<T5>::type, typename convert_to_plain_by_default<T6>::type> (t1, t2, t3, t4, t5, t6);
}
template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
tuple<typename convert_to_plain_by_default<T1>::type, typename convert_to_plain_by_default<T2>::type, typename convert_to_plain_by_default<T3>::type, typename convert_to_plain_by_default<T4>::type, typename convert_to_plain_by_default<T5>::type, typename convert_to_plain_by_default<T6>::type, typename convert_to_plain_by_default<T7>::type> inline make_tuple(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7) {
  return tuple<typename convert_to_plain_by_default<T1>::type, typename convert_to_plain_by_default<T2>::type, typename convert_to_plain_by_default<T3>::type, typename convert_to_plain_by_default<T4>::type, typename convert_to_plain_by_default<T5>::type, typename convert_to_plain_by_default<T6>::type, typename convert_to_plain_by_default<T7>::type> (t1, t2, t3, t4, t5, t6, t7);
}
template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
tuple<typename convert_to_plain_by_default<T1>::type, typename convert_to_plain_by_default<T2>::type, typename convert_to_plain_by_default<T3>::type, typename convert_to_plain_by_default<T4>::type, typename convert_to_plain_by_default<T5>::type, typename convert_to_plain_by_default<T6>::type, typename convert_to_plain_by_default<T7>::type, typename convert_to_plain_by_default<T8>::type> inline make_tuple(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8) {
  return tuple<typename convert_to_plain_by_default<T1>::type, typename convert_to_plain_by_default<T2>::type, typename convert_to_plain_by_default<T3>::type, typename convert_to_plain_by_default<T4>::type, typename convert_to_plain_by_default<T5>::type, typename convert_to_plain_by_default<T6>::type, typename convert_to_plain_by_default<T7>::type, typename convert_to_plain_by_default<T8>::type> (t1, t2, t3, t4, t5, t6, t7, t8);
}
template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
tuple<typename convert_to_plain_by_default<T1>::type, typename convert_to_plain_by_default<T2>::type, typename convert_to_plain_by_default<T3>::type, typename convert_to_plain_by_default<T4>::type, typename convert_to_plain_by_default<T5>::type, typename convert_to_plain_by_default<T6>::type, typename convert_to_plain_by_default<T7>::type, typename convert_to_plain_by_default<T8>::type, typename convert_to_plain_by_default<T9>::type> inline make_tuple(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9) {
  return tuple<typename convert_to_plain_by_default<T1>::type, typename convert_to_plain_by_default<T2>::type, typename convert_to_plain_by_default<T3>::type, typename convert_to_plain_by_default<T4>::type, typename convert_to_plain_by_default<T5>::type, typename convert_to_plain_by_default<T6>::type, typename convert_to_plain_by_default<T7>::type, typename convert_to_plain_by_default<T8>::type, typename convert_to_plain_by_default<T9>::type> (t1, t2, t3, t4, t5, t6, t7, t8, t9);
}
template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
tuple<typename convert_to_plain_by_default<T1>::type, typename convert_to_plain_by_default<T2>::type, typename convert_to_plain_by_default<T3>::type, typename convert_to_plain_by_default<T4>::type, typename convert_to_plain_by_default<T5>::type, typename convert_to_plain_by_default<T6>::type, typename convert_to_plain_by_default<T7>::type, typename convert_to_plain_by_default<T8>::type, typename convert_to_plain_by_default<T9>::type, typename convert_to_plain_by_default<T10>::type> inline make_tuple(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T10& t10) {
  return tuple<typename convert_to_plain_by_default<T1>::type, typename convert_to_plain_by_default<T2>::type, typename convert_to_plain_by_default<T3>::type, typename convert_to_plain_by_default<T4>::type, typename convert_to_plain_by_default<T5>::type, typename convert_to_plain_by_default<T6>::type, typename convert_to_plain_by_default<T7>::type, typename convert_to_plain_by_default<T8>::type, typename convert_to_plain_by_default<T9>::type, typename convert_to_plain_by_default<T10>::type> (t1, t2, t3, t4, t5, t6, t7, t8, t9, t10);
}

// Tie function templates -------------------------------------------------
template<class T1>
tuple<T1&> inline tie(T1& t1) {
  return tuple<T1&> (t1);
}
template<class T1, class T2>
tuple<T1&, T2&> inline tie(T1& t1, T2& t2) {
  return tuple<T1&, T2&> (t1, t2);
}
template<class T1, class T2, class T3>
tuple<T1&, T2&, T3&> inline tie(T1& t1, T2& t2, T3& t3) {
  return tuple<T1&, T2&, T3&> (t1, t2, t3);
}
template<class T1, class T2, class T3, class T4>
tuple<T1&, T2&, T3&, T4&> inline tie(T1& t1, T2& t2, T3& t3, T4& t4) {
  return tuple<T1&, T2&, T3&, T4&> (t1, t2, t3, t4);
}
template<class T1, class T2, class T3, class T4, class T5>
tuple<T1&, T2&, T3&, T4&, T5&> inline tie(T1& t1, T2& t2, T3& t3, T4& t4, T5& t5) {
  return tuple<T1&, T2&, T3&, T4&, T5&> (t1, t2, t3, t4, t5);
}
template<class T1, class T2, class T3, class T4, class T5, class T6>
tuple<T1&, T2&, T3&, T4&, T5&, T6&> inline tie(T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6) {
  return tuple<T1&, T2&, T3&, T4&, T5&, T6&> (t1, t2, t3, t4, t5, t6);
}
template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
tuple<T1&, T2&, T3&, T4&, T5&, T6&, T7&> inline tie(T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7) {
  return tuple<T1&, T2&, T3&, T4&, T5&, T6&, T7&> (t1, t2, t3, t4, t5, t6, t7);
}
template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
tuple<T1&, T2&, T3&, T4&, T5&, T6&, T7&, T8&> inline tie(T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8) {
  return tuple<T1&, T2&, T3&, T4&, T5&, T6&, T7&, T8&> (t1, t2, t3, t4, t5, t6, t7, t8);
}
template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
tuple<T1&, T2&, T3&, T4&, T5&, T6&, T7&, T8&, T9&> inline tie(T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9) {
  return tuple<T1&, T2&, T3&, T4&, T5&, T6&, T7&, T8&, T9&> (t1, t2, t3, t4, t5, t6, t7, t8, t9);
}
template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
tuple<T1&, T2&, T3&, T4&, T5&, T6&, T7&, T8&, T9&, T10&> inline tie(T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9, T10& t10) {
  return tuple<T1&, T2&, T3&, T4&, T5&, T6&, T7&, T8&, T9&, T10&> (t1, t2, t3, t4, t5, t6, t7, t8, t9, t10);
}

} //namespace std

#undef LL_TLAST
#undef LL_ULAST
#undef LL_REPEAT0
#undef LL_REPEAT1
#undef LL_REPEAT2
#undef LL_REPEAT3

#endif
