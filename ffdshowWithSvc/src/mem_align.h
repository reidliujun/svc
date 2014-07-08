#ifndef _MEM_ALIGN_H_
#define _MEM_ALIGN_H_

void *aligned_malloc(size_t size,size_t alignment=0);
void *aligned_calloc(size_t size1,size_t size2,size_t alignment=0);
void *aligned_calloc3(size_t size1,size_t size2,size_t size3=0,size_t alignment=0);
void *aligned_realloc(void *memblock,size_t size,size_t alignment=0);
void aligned_free(void *mem_ptr);

#ifdef __cplusplus

template <class T> class aligned_allocator;

template <> class aligned_allocator<void>
{
public:
 typedef void*       pointer;
 typedef const void* const_pointer;
 typedef void  value_type;
 template <class U> struct rebind { typedef aligned_allocator<U> other; };
};

template <class T> class aligned_allocator
{
public:
 typedef T value_type;
 typedef size_t size_type;
 typedef ptrdiff_t difference_type;

 typedef T* pointer;
 typedef const T* const_pointer;

 typedef T& reference;
 typedef const T& const_reference;

 pointer address(reference r) const { return &r; }
 const_pointer address(const_reference r) const { return &r; }

 aligned_allocator() throw(){}
 template <class U> aligned_allocator(const aligned_allocator<U>& ) throw() {}
 ~aligned_allocator() throw(){}

 //Space for n Ts
 pointer allocate(size_type n, typename aligned_allocator<void>::const_pointer = 0)
  {
   return (T*)aligned_malloc(n*sizeof(T));
  }
 void deallocate(pointer p, size_type)
  {
   aligned_free(p);
  }

 //Use placement new to engage the constructor
 void construct(pointer p, const T& val) { new((void*)p) T(val); }
 void destroy(pointer p){ ((T*)p)->~T(); }

 size_type max_size() const throw() {return size_t(-1)/sizeof(T);}
 template<class U> struct rebind { typedef aligned_allocator<U> other; };
};

#endif

#endif /* _MEM_ALIGN_H_ */
