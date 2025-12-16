#ifndef CONSTRUCT_H
#define CONSTRUCT_H

#include "type_traits.h"

namespace MyTinySTL {
template <class T> inline void construct(T *p) {
  // placement new
  new ((void *)p) T();
}

template <class T1, class T2> inline void construct(T1 *p, T2 &value) {
  // placement new
  new ((void *)p) T1(value);
}

template <class T> inline void destroy(T *p) {
  // destroy for placement new
  p->~T();
}

template <class ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last,
                          __false_type) {
  // 这里的 __false_type
  // 不是用来传数据的，它仅仅是一个标签（Tag）
  // 如果传入 __false_type() 对象，就进这个函数（需要循环调用析构）。
  // 如果传入 __true_type() 对象，就进下面那个空函数（啥也不干，优化性能）。
  // 因为函数体内根本不需要用到这个参数（只需要它的类型来决定进哪个门），所以变量名可以直接省略。
  for (; first != last; first++)
    destroy(&*first);
}

template <class ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last,
                          __true_type) {}

template <class ForwardIterator, class T>
inline void _destroy(ForwardIterator first, ForwardIterator last, T *) {
  // 查询 T 类型是 __true_type 还是 __false_type，调用对应的 __destroy_aux
  typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
  __destroy_aux(first, last, trivial_destructor());
}

template <class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
  // value_type(first)： 这是一个辅助函数（SGI STL
  // 风格）。它的作用是返回一个空指针，类型是迭代器所指对象的类型。
  // 它的唯一目的就是帮助编译器推导出 T 是什么。
  // 如果 first 是 int*，它返回 (int*)0
  // 从而调用上面那个带类型指针的函数
  _destroy(first, last, value_type(first));
}

// 全特化 char * 和 wchar_t *
// 不需要再走 value_type 调用了，为了性能优化
template <> inline void destroy(char *, char *) {}

template <> inline void destroy(wchar_t *, wchar_t *) {}
} // namespace MyTinySTL

#endif