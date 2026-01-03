#ifndef NUMERIC_H
#define NUMERIC_H

#include "iterator.h"

namespace MyTinySTL {

template <class InputIterator, class T>
T accumlate(InputIterator first, InputIterator last, T init) {
  for (; first != last; ++first) {
    init += *first;
  }
  return init;
}

template <class InputIterator, class T, class BinaryOperation>
T accumulate(InputIterator first, InputIterator last, T init,
             BinaryOperation bo) {
  for (; first != last; ++first) {
    init = bo(init, *first);
  }
  return init;
}

template <class InputIterator, class OutputIterator>
OutputIterator adjacent_difference(InputIterator first, InputIterator last,
                                   OutputIterator result) {
  // write nothing
  if (first == last)
    return result;
  *result = *first;
  typedef typename iterator_traits<InputIterator>::value_type ty;
  ty value = *first;
  while (++first != last) {
    ty tmp = *first;
    *++result = tmp - value;
    value = tmp;
  }
  return ++result;
}

template <class InputIterator, class OutputIterator, class BinaryOperation>
OutputIterator adjacent_difference(InputIterator first, InputIterator last,
                                   OutputIterator result, BinaryOperation bo) {
  if (first == last)
    return;
  *result = *first;
  typedef typename iterator_traits<InputIterator>::value_type ty;
  ty value = *first;
  while (++first != last) {
    ty tmp = *first;
    *++result = bo(tmp, value);
    value = tmp;
  }
  return ++result;
}

template <class InputIterator1, class InputIterator2, class T>
T inner_product(InputIterator1 first1, InputIterator1 last1,
                InputIterator2 first2, T init) {
  for (; first1 != last1; first1++, first2++) {
    init = init + (*first1 * *first2);
  }
  return init;
}

template <class InputIterator1, class InputIterator2, class T,
          class BinaryOperation1, class BinaryOperation2>
T inner_product(InputIterator1 first1, InputIterator1 last1,
                InputIterator2 first2, T init, BinaryOperation1 bo1,
                BinaryOperation2 bo2) {
  for (; first1 != last1; first1++, first2++) {
    init = bo1(init, bo2(*first1, *first2));
  }
  return init;
}

template <class InputIterator, class OutputIterator>
OutputIterator partial_sum(InputIterator first, InputIterator last,
                           OutputIterator result) {
  // write nothing
  if (first == last)
    return result;
  *result = *first;
  typedef typename iterator_traits<InputIterator>::value_type ty;
  ty value = *first;
  while (++first != last) {
    value = value + *first;
    *++result = value;
  }
  return ++result;
}

template <class InputIterator, class OutputIterator, class BinaryOperation>
OutputIterator partial_sum(InputIterator first, InputIterator last,
                           OutputIterator result, BinaryOperation bo) {
  // write nothing
  if (first == last)
    return result;
  *result = *first;
  typedef typename iterator_traits<InputIterator>::value_type ty;
  ty value = *first;
  while (++first != last) {
    value = bo(value, *first);
    *++result = value;
  }
  return ++result;
}

} // namespace MyTinySTL

#endif
