#ifndef ALLOC_H
#define ALLOC_H

#include <alloca.h>
#include <cstddef>
#include <cstdlib>
#include <iostream>

namespace MyTinySTL {
#if !defined(_THROW_BAD_ALLOC)
#define _THROW_BAD_ALLOC                                                       \
  std::cerr << "out of memory" << std::endl;                                   \
  exit(1);

class _alloc {
private:
  // oom: out of memory
  static void *oom_malloc(size_t);
  static void (*__oom_handler)();

public:
  static void *allocate(size_t n) {
    void *result = malloc(n);
    if (result == 0)
      result = oom_malloc(n);
    return result;
  }

  static void deallocate(void *p, size_t n) { free(p); }

  // set oom_handle
  static void (*set_malloc_handler(void (*f)()))() {
    void (*old)() = __oom_handler;
    __oom_handler = f;
    return old;
  }
};

void (*_alloc::__oom_handler)() = 0;
void *_alloc::oom_malloc(size_t n) {
  void (*my_malloc_handler)();
  void *result;

  for (;;) {
    my_malloc_handler = __oom_handler;
    if (my_malloc_handler == 0) {
      _THROW_BAD_ALLOC;
    }
    (*my_malloc_handler)();
    result = malloc(n);
    if (result)
      return result;
  }
}
#endif

// alloc 类的核心作用是内存池管理。它的设计目的是为了解决 C++
// 中频繁分配小块内存（Small Objects）时带来的两个主要问题： 内存碎片（Memory
// Fragmentation）： 频繁 malloc/free 不同大小的小块会导致堆内存支离破碎。
// 额外开销（Overhead）： malloc 分配的内存通常带有
// "Cookie"（记录块大小等信息的头部），对于极小的对象（如 4 字节的
// int），这个头部可能比数据本身还大，极其浪费。

// alloc 的核心策略
// 它设定了一个阈值 _MAX_BYTES (128 bytes)：
// 大块内存 (>128 bytes)： 认为直接由系统管理更好，转交给你代码上方的 _alloc
// 类（一级配置器），直接调用 malloc/free。 小块内存 (≤128 bytes)： 由 alloc
// 类（二级配置器）接管。它维护了一个内存池（Memory Pool）和一组自由链表（Free
// Lists）。分配时直接从链表拔下一个节点（仅需指针操作，极快），释放时回收到链表中（不还给操作系统）。

class alloc {
private:
  enum { _ALIGN = 8 };
  enum { _MAX_BYTES = 8 };
  enum { _NFREELISTS = 16 };

private:
  static size_t ROUND_UP(size_t bytes) {
    return ((bytes + _ALIGN - 1) & ~(_ALIGN - 1));
  }

private:
  union obj {
    union obj *free_list_link;
    char data[1];
  };

private:
  static obj *volatile free_list[_NFREELISTS];

  static size_t FREELIST_INDEX(size_t bytes) {
    return ((bytes + _ALIGN + 1) / (_ALIGN - 1));
  }

  static void *refill(size_t n);
  static char *chunk_alloc(size_t size, int &nobj);

  static char *start_free;
  static char *end_free;
  static size_t heap_size;

public:
  static void *allocate(size_t n);
  static void deallocate(void *p, size_t n);
  static void *reallocate(void *p, size_t old_size, size_t new_size);
};

char *alloc::start_free = 0;
char *alloc::end_free = 0;
size_t alloc::heap_size = 0;
alloc::obj *volatile alloc::free_list[_NFREELISTS] = {0, 0, 0, 0, 0, 0, 0, 0,
                                                      0, 0, 0, 0, 0, 0, 0, 0};

void *alloc::allocate(size_t n) {
  obj *volatile *my_free_list;
  obj *result;

  if (n > (size_t)_MAX_BYTES)
    return _alloc::allocate(n);

  my_free_list = free_list + FREELIST_INDEX(n);
  result = *my_free_list;
  if (result == 0) {
    void *r = refill(ROUND_UP(n));
    return r;
  }

  *my_free_list = result->free_list_link;
  return result;
}

void alloc::deallocate(void *p, size_t n) {
  obj *q = (obj *)p;
  obj *volatile *my_free_list;

  if (n > (size_t)_MAX_BYTES) {
    _alloc::deallocate(p, n);
    return;
  }

  my_free_list = free_list + FREELIST_INDEX(n);
  q->free_list_link = *my_free_list;
  *my_free_list = q;
}

void *alloc::reallocate(void *p, size_t old_size, size_t new_size) {
  deallocate(p, old_size);
  return allocate(new_size);
}

void *alloc::refill(size_t n) {
  int nobj = 20;

  char *c = chunk_alloc(n, nobj);
  if (nobj == 1)
    return c;
  
  obj *volatile *my_free_list;
  obj *result;
  obj *cur_obj, *nex_obj;

  my_free_list = free_list + FREELIST_INDEX(n);
  result = (obj *)c;
  *my_free_list = nex_obj = (obj *)(c + n);
  for (int i = 1; i < nobj - 1; i++) {
    cur_obj = nex_obj;
    nex_obj = (obj *)((char *)nex_obj + n);
    cur_obj->free_list_link = nex_obj; 
  }
  cur_obj->free_list_link = 0;
  return result;
}

char *alloc::chunk_alloc(size_t size, int &nobj) {
  char *result;

  size_t need_bytes = size * nobj;
  size_t pool_bytes = end_free - start_free;

  if (pool_bytes >= need_bytes) {
    result = start_free;
    start_free += need_bytes;
    return result;
  }
  // at least one
  else if (pool_bytes >= size) {
    nobj = pool_bytes / size;
    need_bytes = size * nobj;
    result = start_free;
    start_free += need_bytes;
    return result;
  }

  else {
    if (pool_bytes > 0) {
      obj *volatile *my_free_list = free_list + FREELIST_INDEX(pool_bytes);
      // what happend if pool_bytes < 4(size of obj)
      // 这也是 STL 设计中最严谨的地方——对齐（Alignment）保证。
      // 入口保证：所有的内存申请请求 allocate(n)，第一步就是调用
      // ROUND_UP(n)。这意味着 n 永远是 8 的倍数（8, 16, 24...）。
      // 移动保证：start_free 指针的每一次移动（增加），都是加上 need_bytes。而
      // need_bytes = size * nobj，既然 size 是 8 的倍数，那么 need_bytes
      // 也一定是 8 的倍数。
      // 结果：start_free 永远落在 8 的倍数边界上，end_free（它是某次 malloc
      // 的结果加上一大块 8 的倍数）也是如此。
      // 推论： pool_bytes = end_free - start_free。 两个“8
      // 的倍数”相减，结果必然还是 8 的倍数。
      ((obj*)start_free)->free_list_link = *my_free_list;
      *my_free_list = (obj*)start_free;
      // no need to start_free += pool_bytes
    }

    size_t bytes_to_get = 2 * need_bytes + ROUND_UP(heap_size >> 4);
    start_free = (char *)malloc(bytes_to_get);
    if (start_free == 0) {
      // heap memory is ran out
      for (int i = size; i < _MAX_BYTES; i += _ALIGN) {
        obj *volatile *my_free_list, *p;
        my_free_list = free_list + FREELIST_INDEX(i);
        p = *my_free_list;
        if (p != 0) {
          *my_free_list = p->free_list_link;
          start_free = (char *)p;
          end_free = start_free + i;
          // alloc again
          return chunk_alloc(size, nobj);
        }

        end_free = 0;
        start_free = (char *)_alloc::allocate(bytes_to_get);
      }
    }

    end_free = start_free + bytes_to_get;
    heap_size += bytes_to_get;
    return chunk_alloc(size, nobj);
  }  
}

} // namespace MyTinySTL

#endif