#pragma once

#include "heap_4_infinitime.h"
#include <memory>
#include <string>

template <typename T> class infini_allocator : public std::allocator<T> {
public:
  typedef size_t size_type;
  typedef T *pointer;
  typedef const T *const_pointer;

  template <typename _Tp1> struct rebind {
    typedef infini_allocator<_Tp1> other;
  };

  pointer allocate(size_type n, const void *hint = 0) {
    return (pointer)pvPortMalloc(n * sizeof(T));
  }

  void deallocate(pointer p, size_type n) {
    return vPortFree(p);
  }

  infini_allocator() throw() : std::allocator<T>() {   
  }

  infini_allocator(const infini_allocator &a) throw() : std::allocator<T>(a) {}
  template <class U>
  infini_allocator(const infini_allocator<U> &a) throw() : std::allocator<T>(a) {}
  ~infini_allocator() throw() {}
};

static void reset_mempool()
{
    vPortDeinit();
}

template <typename T>
class pool_unique_ptr {
 public:
  pool_unique_ptr() = default;
  pool_unique_ptr(T* ptr) : ptr_(ptr) {}

  ~pool_unique_ptr() { vPortFree(ptr_); }

  pool_unique_ptr(const pool_unique_ptr& other) = delete;

  pool_unique_ptr(pool_unique_ptr&& other) noexcept
      : ptr_(other.release()) {}
  
  // generalized move ctor
  template <typename U>
  pool_unique_ptr(pool_unique_ptr<U>&& other) noexcept
      : ptr_(other.release()) {}

  pool_unique_ptr& operator=(const pool_unique_ptr& other) = delete;

  pool_unique_ptr& operator=(pool_unique_ptr&& other) noexcept {
    pool_unique_ptr(std::move(other)).swap(*this);
    return *this;
  }

  void reset(T* ptr) noexcept {
    vPortFree(ptr_);
    ptr_ = ptr;
  }

  T* release() noexcept {
    auto old_ptr = ptr_;
    ptr_ = nullptr;
    return old_ptr;
  }

  void swap(pool_unique_ptr& other) noexcept {
    using std::swap;
    swap(ptr_, other.ptr_);
  }

  T& operator*() const noexcept { return *ptr_; }
  T* operator->() const noexcept { return ptr_; }
  T* get() const noexcept { return ptr_; }
  explicit operator bool() { return ptr_ != nullptr; }

 private:
  T* ptr_ = nullptr;
};


template<typename T, typename... Args>
static pool_unique_ptr<T> pool_make_unique(Args&&... args){

    infini_allocator<T> allocator;
    void* p = allocator.allocate(1);

    T* t = new(p) T(std::forward< Args >(args)...);
    return pool_unique_ptr<T>(t);
}

typedef std::basic_string<char, std::char_traits<char>, infini_allocator<char>> IString;

