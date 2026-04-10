#include <iostream>
#include <cassert>
#include "Allocator.hpp"
struct TestObj
{
    int id;
    TestObj(int id) : id(id)
    {
        std::cout << "Constructed TestObj " << id << std::endl;
    };
    ~TestObj() {
        std::cout << "Destructed TestObj " << id << std::endl;
    };
};

int main()
{
    mini_stl::Allocator<TestObj> alloc_TestObj;

    TestObj* mem =  alloc_TestObj.allocate(3);

    alloc_TestObj.construct(mem, 1);
    alloc_TestObj.construct(mem+1, 2);
    alloc_TestObj.construct(mem+2, 3);

    assert(mem->id == 1);
    assert((mem + 1)->id == 2);
    assert((mem + 2)->id == 3);


    alloc_TestObj.destroy(mem);
    alloc_TestObj.destroy(mem+1);
    alloc_TestObj.destroy(mem+2);

    alloc_TestObj.deallocate(mem, 3);


    std::cout << "Allocator tests passed!\n";

    return 0;
}