#ifndef UNORDERED_MAP_HPP

#define UNORDERED_MAP_HPP

#include "Allocator.hpp"
#include <iostream>

namespace mini_stl
{
    template <typename Key, typename T, typename Hash = std::hash<Key>>
    class unordered_map
    {
    public:
        using key_type = Key;
        using mapped_type = T;
        using value_type = std::pair<const Key, T>;
        using size_type = std::size_t;

        using hasher = Hash;
        using allocator_type = Allocator<value_type>;

        struct Node
        {
            value_type data;
            Node *next;
        }

        class iterator
        {
            Node *ptr_;
            unordered_map *map;
            size_type bucket_idx_;
            void advance_to_next()
            {
                if (!map)
                    return;

                ptr_ = ptr_->next;
                if (!ptr_)
                {
                    for (size_type i = bucket_idx_; i < map->bucket_count_; i++)
                    {
                        if (map->buckets_[i])
                        {
                            ptr_ = map->buckets_[i];
                            bucket_idx_ = i;
                            return;
                        }
                    }
                    bucket_idx_ = map->bucket_count_;
                }
            }

        public:
            explicit iterator(Node *ptr_, unordered_map *map, size_type idx = 0) : ptr_(ptr_), map(map), bucket_idx_(idx)
            {
            }
            using value_type = value_type;
            using reference = value_type &;
            using pointer = value_type *;

            reference operator*() { return ptr_->data; }
            pointer operator->() { return &(ptr_->data); }
            iterator &operator++()
            {
                advance_to_next();
                return *this;
            }
            iterator operator++(int)
            {
                advance_to_next();
                return *this;
            }

            bool operator==(const iterator other) const
            {
                return ptr_ == other.ptr_;
            }
            bool operator!=(const iterator other) const
            {
                return !(*this == other);
            }
        };

    private:
        Node **buckets_;

        size_type bucket_count_;
        float max_load_factor_;
        size_type size_;
        hasher hash_fn_;
        allocator_type allocator;

        size_type get_bucket_index(const key_type &key) const
        {
            return hash_fn_(key) % bucket_count_;
        }

    public:
        explicit unordered_map(size_type bucket_count = 16, const Hash &hash = Hash{}) : bucket_count_(bucket_count), size_(0), hash_fn_(hash), max_load_factor_(1.0f)
        {
            if (bucket_count_ == 0)
                bucket_count_ = 1;
            buckets_ = new Node *[bucket_count_](); // list of null pointers
        }

        ~unordered_map() noexcept
        {
            clear_nodes();
            delete[] buckets_;
        }
        void clear_nodes() noexcept
        {
            for (size_type i = 0; i < bucket_count_; i++)
            {
                Node *curr = buckets_[i];
                while (curr)
                {
                    Node *next = curr->next;
                    allocator.destroy(curr);
                    allocator.deallocate(curr, 1);
                    curr = next;
                }
                buckets_[i] = nullptr;
            }
            size_ = 0;
            return;
        }

        std::pair<iterator, bool> insert(const value_type &value_type)
        {
            size_type idx = get_bucket_index(value_type.first);

            for (Node *curr = buckets_[idx]; curr; curr = curr->next)
            {
                if (value_type.first == curr->data.first)
                    return {iterator(curr, this, idx), false};
            }

            Node *new_node = allocator.allocate(1);

            allocator.construct(new_node, value_type, nullptr);
            new_node->next = buckets_[idx];
            buckets_[idx] = new_node;
            size_++;
            check_load_factor();

            return {iterator(new_node, this, idx), true};
        }
        float load_factor()
        {
            return static_cast<float>(size_) / bucket_count_;
        }
        void check_load_factor()
        {
            if (load_factor() > max_load_factor_)
            {
                rehash(bucket_count_ * 2);
            }
        }
        void rehash(size_type new_bucket_count)
        {
            if (new_bucket_count < bucket_count_)
                return;
            Node **new_buckets = new Node *[new_bucket_count]();

            for (size_type i = 0; i < bucket_count_; i++)
            {
                Node *curr = buckets_[i];

                while (curr)
                {
                    Node *temp = curr->next;

                    size_type new_idx = hash_fn_(curr->data.first) % new_bucket_count;

                    curr->next = new_buckets[new_idx];
                    new_buckets[new_idx] = curr;

                    curr = temp;
                }
            }
            delete[] buckets_;
            buckets_ = new_buckets;
            bucket_count_ = new_bucket_count;
        }
        value_type &operator[](const key_type &key)
        {
            auto res = insert(value_type(key, mapped_type{}));
            return res.first->second;
        }
        size_type erase(const key_type &key)
        {
            size_type idx = get_bucket_index(key);
            Node *curr = buckets_[idx];
            Node *prev = nullptr;

            while (curr)
            {
                if (curr->data.first == key)
                {
                    if (prev)
                        prev->next = curr->next;
                    else
                        buckets_[idx] = curr;

                    allocator.destroy(curr);
                    allocator.deallocate(curr, 1);
                    size_--;
                    return 1;
                }
                prev = curr;
                curr = curr->next;
            }

            return 0;
        }
    };
} // namespace mini_stl

#endif