#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include <cstddef>
#include <iostream>


namespace mini_stl
{
    template <typename T>
    class Allocator
    {
    public:
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = std::size_t;

        Allocator() noexcept = default;
        template <typename U> constexpr Allocator(const Allocator<U>&) noexcept {};

        [[nodiscard]]
        pointer allocate(size_type n) {
            if (n == 0) return nullptr;

            if (n > std::size_t(-1) / sizeof(T)) {
                throw std::bad_alloc();
            }

            
            return static_cast<pointer>(::operator new (n * sizeof(T)));

        }

        void deallocate(pointer p, size_type t) noexcept {
            ::operator delete(p);
        };

        void destroy(pointer p) noexcept{
            if (p != nullptr) p->~T();
        }

        template <typename... Args>
        void construct(pointer p, Args&&... args) {

            ::new (static_cast<void*>(p)) T(std::forward<Args>(args)...);
        }
        friend bool operator==(const Allocator&,const Allocator& )  noexcept { return true; } 
        friend bool operator!=(const Allocator&,const Allocator& )  noexcept { return false; } 
    };
} // namespace mini_stl

#endif // ALLOCATOR_HPP