#ifndef UNINITIALIZAED_H
#define UNINITIALIZAED_H

#include "construct.h"
#include "type_traits.h"

#ifndef USE_CSTRING
#define USE_CSTRING
#include <cstring>
#endif

#include "algobase.h"

namespace MyTinySTL {

// --------------------- //
//  __uninitialized_copy //
// --------------------- //
// [first, last) is the value list
// result is the address of uninitialized obj
template <class InputIterator, class ForwardIterator>
inline ForwardIterator uninitialized_copy(InputIterator first,
                                          InputIterator last,
                                          ForwardIterator result) {
  return __uninitialized_copy_aux(first, last, result, value_type(result));
}

// [first, last) is the value list
// result is the address of uninitialized obj
template <class InputIterator, class ForwardIterator, class T>
inline ForwardIterator __uninitialized_copy(InputIterator first,
                                            InputIterator last,
                                            ForwardIterator result, T *) {
  typedef typename __type_traits<T>::is_POD_type POD_type;
  return __uninitialized_copy_aux(first, last, result, POD_type());
}

// true_type
// [first, last) is the value list
// result is the address of uninitialized obj
template <class InputIterator, class ForwardIterator>
inline ForwardIterator
__uninitialized_copy_aux(InputIterator first, InputIterator last,
                         ForwardIterator result, __true_type) {
  return copy(first, last, result);
}

// false_type
// [first, last) is the value list
// result is the address of uninitialized obj
template <class InputIterator, class ForwardIterator>
inline ForwardIterator
__uninitialized_copy_aux(InputIterator first, InputIterator last,
                         ForwardIterator result, __false_type) {
  ForwardIterator cur = result;
  for (; first != last; ++first, ++cur) {
    MyTinySTL::construct(&*cur, *first);
  }
  return cur;
}

inline char *__uninitialized_copy(const char *first, const char *last,
                                  char *result) {
  memmove(result, first, last - first);
  return result + (last - first);
}

inline wchar_t *__uninitialized_copy(const wchar_t *first, const wchar_t *last,
                                     wchar_t *result) {
  memmove(result, first, sizeof(wchar_t) * (last - first));
  return result + (last - first);
}

// --------------------- //
//  __uninitialized_fill //
// --------------------- //
template <class ForwardIterator, class T>
inline void uninitialized_fill(ForwardIterator first, ForwardIterator last,
                               const T &x) {
  __uninitialized_fill(first, last, x, value_type(first));
}

template <class ForwardIterator, class T, class Ty>
inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last,
                                 const T &x, Ty *) {
  typedef typename __type_traits<Ty>::is_POD_type POD_type;
  __uninitialized_fill_aux(first, last, x, POD_type());
}

template <class ForwardIterator, class T>
inline void __uninitialized_fill_aux(ForwardIterator first,
                                     ForwardIterator last, const T &x,
                                     __true_type) {
  fill(first, last, x);
}

template <class ForwardIterator, class T>
inline void __uninitialized_fill_aux(ForwardIterator first,
                                     ForwardIterator last, const T &x,
                                     __false_type) {
  ForwardIterator cur = first;
  for (; cur != last; cur++) {
    construct(&*cur, x);
  }
}

// ----------------------- //
//  __uninitialized_fill_n //
// ----------------------- //
template <class ForwardIterator, class Size, class T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n,
                                            const T &x) {
  return __uninitialized_fill_n(first, n, x, value_type(first));
}

template <class ForwardIterator, class Size, class T, class Ty>
inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n,
                                              const T &x, Ty *) {
  typedef typename __type_traits<Ty>::is_POD_type POD_type;
  return __uninitialized_fill_n_aux(first, n, x, POD_type());
}

template <class ForwardIterator, class Size, class T>
inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n,
                                                  const T &x, __true_type) {
  return fill_n(first, n, x);
}

template <class ForwardIterator, class Size, class T>
inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n,
                                                  const T &x, __false_type) {
  ForwardIterator cur = first;
  for (; n > 0; --n, cur++) {
    construct(&*cur, x);
  }
  return cur;
}
} // namespace MyTinySTL

#endif