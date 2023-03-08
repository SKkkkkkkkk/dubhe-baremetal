// #include <cstddef>
// #include <vector>
// void std::__throw_out_of_range(char const* msg) {
// 	while(1) __asm__ volatile("");
// }


extern "C" void __dso_handle() { while(1); }

// extern "C" void __cxa_pure_virtual() { while(1); }

#include <cstdlib> 
#include <new> 

void* operator new(size_t size) noexcept 
{ 
    return malloc(size); 
} 

void operator delete(void *p) noexcept 
{ 
    free(p); 
} 

void* operator new[](size_t size) noexcept 
{ 
    return operator new(size); // Same as regular new
} 

void operator delete[](void *p) noexcept 
{ 
    operator delete(p); // Same as regular delete
} 

void* operator new(size_t size, std::nothrow_t) noexcept 
{ 
    return operator new(size); // Same as regular new 
} 

void operator delete(void *p,  std::nothrow_t) noexcept 
{ 
    operator delete(p); // Same as regular delete
} 

void* operator new[](size_t size, std::nothrow_t) noexcept 
{ 
    return operator new(size); // Same as regular new
} 

void operator delete[](void *p,  std::nothrow_t) noexcept 
{ 
    operator delete(p); // Same as regular delete
}