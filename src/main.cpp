// src/main.cpp
#include <iostream>
#include <cassert>
#include "Allocator.hpp"

int main() {

    // Create an Allocator instance for int. Either create an object:
    mini_stl::Allocator<int> alloc;
    // Or, if you wanted a pointer, use:
    // auto* alloc = new mini_stl::Allocator<int>();
    auto mem = alloc.allocate(3);


    return 0;
}