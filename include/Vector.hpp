#ifndef MINI_STL_VECTOR_HPP
#define MINI_STL_VECTOR_HPP

#include <iostream>
#include "Allocator.hpp"

namespace mini_stl
{
    template <typename T>
    class Vector
    {
    public:
        using value_type = T;
        using size_type = std::size_t;
        using reference = T &;
        using const_reference = const T &;
        using pointer = T *;
        using const_pointer = const T *;

        using iterator = pointer;
        using const_iterator = const_pointer;

    private:
        iterator start_;
        iterator finish_; // last contructed object
        iterator end_of_storage_;
        Allocator<T> allocator;

    public:
        Vector() noexcept : start_(nullptr), finish_(nullptr), end_of_storage_(nullptr)
        {
        }

        explicit Vector(size_type count, const_reference value = {})
        {
            if (count == 0) {
                *this = Vector();
                return;
            }

            start_ = allocator.allocate(count);
            end_of_storage_ = start_ + count;
            finish_ = start_;
            for (iterator it = start_; it < end_of_storage_; it++)
            {
                allocator.construct(it, value);
            }
            finish_ = end_of_storage_;
        }
        size_type capacity() const noexcept { return end_of_storage_ - start_; };
        size_type size() const noexcept { return finish_ - start_; };
        bool empty() const noexcept { return finish_ == start_; };
        ~Vector() noexcept
        {
            if (start_)
            {
                for (iterator it = start_; it < finish_; ++it)
                {
                    allocator.destroy(it);
                }
                allocator.deallocate(start_);
            }
        };
        void swap(Vector &other_vector) noexcept
        {
            std::swap(start_, other_vector.start_);
            std::swap(finish_, other_vector.finish_);
            std::swap(end_of_storage_, other_vector.end_of_storage_);
            std::swap(allocator, other_vector.allocator);
        }
        void clear() noexcept
        {
            for (iterator it = start_; it < finish_; it++)
                allocator.destroy(it);

            finish_ = start_;
        }
        iterator begin() noexcept
        {
            return start_;
        }
        iterator end() noexcept
        {
            return finish_;
        }
        const_iterator begin() const noexcept
        {
            return start_;
        }
        const_iterator end() const noexcept
        {
            return finish_;
        }
        void reserve(size_type new_cap)
        {
            if (capacity() >= new_cap)
                return;

            iterator new_start_ = allocator.allocate(new_cap);
            iterator new_finish_ = new_start_;
            iterator new_end_if_storage = new_start_ + new_cap;

            for (iterator it = start_; it < finish_; it++)
            {
                allocator.construct(new_finish_, std::move((*it)));
                new_finish_ += 1;
            }

            for (iterator it = start_; it < finish_; it++)
                allocator.destroy(it);
            allocator.deallocate(start_);

            start_ = new_start_;
            finish_ = new_finish_;
            end_of_storage_ = new_end_if_storage;
        }
        // lvalue overload: copy from const reference (do not std::move a const&)
        void push_back(const_reference val) {
            if (capacity() == 0) {
                reserve(1);
            } else if (capacity() == size()) // it's full
            {
                reserve(capacity() + (capacity() / 2) + 1);
            }
            allocator.construct(finish_, val);
            finish_++;
        }

        // rvalue overload: move when caller provides an rvalue
        void push_back(value_type&& val) {
            if (capacity() == 0) {
                reserve(1);
            } else if (capacity() == size()) // it's full
            {
                reserve(capacity() + (capacity() / 2) + 1);
            }
            allocator.construct(finish_, std::move(val));
            finish_++;
        }
        void pop_back() noexcept{
            if (size() == 0) return;

            allocator.destroy(--finish_);
        }
        // Move
        Vector(Vector&& other) noexcept : start_(other.start_), finish_(other.finish_), end_of_storage_(other.end_of_storage_){
            other.start_ = nullptr;
            other.finish_ = nullptr;
            other.end_of_storage_ = nullptr;
        }
        // Copy
        Vector(const Vector& other) {
            if (other.empty())  {
                *this = Vector();
                return;
            }
            start_ = allocator.allocate(other.size());
            finish_ = start_;
            
            for (size_type it = 0; it < other.size(); it++) {
                allocator.construct(start_ + it, other[it]);
                finish_++;
            }
            
            end_of_storage_ = start_ + other.size();

        }
        Vector& operator=(const Vector& other) {

        }
        Vector& operator=(Vector&& other) {

        }
        reference operator[](size_type pos) {
            return *(start_+pos);
        }
        const_reference operator[](size_type pos) const {
            return *(start_+pos);
        }
        
    };

} // namespace mini_stl

#endif