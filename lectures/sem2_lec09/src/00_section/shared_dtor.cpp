#include <iostream>
#include <vector>
#include <memory>

struct T {
    ~T() { std::cout << 1; }
};
struct D : T {
    ~D() { std::cout << 2; }
};


template <class T>
struct SharedPtr {
    using Deleter = void(*)(T*);

    template <class U>
    explicit SharedPtr(U* ptr)
    : src(ptr)
    , deleter([](T* t){ delete static_cast<U*>(t); }){}

    ~SharedPtr() {
        deleter(src);
    }

    T* src;
    Deleter deleter;
};


int main() {
//    std::vector<std::shared_ptr<T>> vs;
//    vs.emplace_back(new D);
    // вызовется ли деструктор D

    SharedPtr<T> t(new D);

    return 0;
}

