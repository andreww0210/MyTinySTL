#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "alloc.h"
#include "construct.h"
#include <cstddef>

namespace MyTinySTL {
template <class T> class allocator {
  // clang-format off
public:
  typedef T             value_type;
  typedef T*            pointer;
  typedef const T*      const_pointer;
  typedef T&            reference;
  typedef const T &     const_reference;
  typedef size_t        size_type;
  typedef ptrdiff_t     difference_type;
  // clang-format on
public:
  static T *allocate();
  static T *allocate(size_t n);
  static void deallocate(T *ptr);
  static void deallocate(T *ptr, size_t n);

  static void construct(T *ptr);
  static void construct(T *ptr, const T &x);
  static void destroy(T *ptr);
  static void destroy(T *first, T *last);
};

template <class T> T *allocator<T>::allocate() {
  return static_cast<T *>(alloc::allocate(sizeof(T)));
}

template <class T> T *allocator<T>::allocate(size_t n) {
  if (n == 0)
    return 0;
  return static_cast<T *>(alloc::allocate(n * sizeof(T)));
}

template <class T> void allocator<T>::deallocate(T *ptr) {
  if (ptr == 0)
    return;
  return alloc::deallocate(ptr);
}

template <class T> void allocator<T>::deallocate(T *ptr, size_t n) {
  if (ptr == 0)
    return;
  return alloc::deallocate(ptr, n);
}

template <class T> void allocator<T>::construct(T *ptr) {
  return MyTinySTL::construct(ptr);
}

template <class T> void allocator<T>::construct(T *ptr, const T &x) {
  return MyTinySTL::construct(ptr, x);
}

template <class T> void allocator<T>::destroy(T *ptr) {
  MyTinySTL::destroy(ptr);
}

template <class T> void allocator<T>::destroy(T *first, T *last) {
  MyTinySTL::destroy(first, last);
}
} // namespace MyTinySTL

#endif