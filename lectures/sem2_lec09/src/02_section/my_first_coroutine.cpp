
#include <experimental/coroutine>
namespace coro = std::experimental;

#include <iostream>
#include <optional>

template <class T>
struct task {

    struct promise_type;
    using coro_handle = coro::coroutine_handle<promise_type>;

    struct promise_type {
        promise_type() { std::cout << "promise_type()\n"; }
        ~promise_type() { std::cout << "~promise_type()\n"; }

        task get_return_object() { return task{coro_handle::from_promise(*this)}; }

        coro::suspend_always initial_suspend() { return {}; }
        coro::suspend_always final_suspend() noexcept { return {}; }

        void return_value(T t) {
            value = std::move(t);
        }
        void unhandled_exception() {
            std::terminate();
        }

        std::optional<T> value;
    };

    T result() {
        while (!handle.done())
            handle.resume();
        return handle.promise().value.value();
    }

    task(coro_handle handle) : handle(std::move(handle)) {}
    task(const task&) = delete;
    task(task&&) = default;

    ~task() {
        handle.destroy();
    }

    coro_handle handle;
};

task<int> answer() {
//    struct Frame {
//        using promise_type = task<int>::promise_type;
//        promise_type promise;
//    };
//    Frame* frame = new Frame{};
//
//    auto& p = frame->promise;
//
//    char* ret = new char[sizeof(task<int>)];
//    new (ret) task<int>{p.get_return_object()};
//
//    try {
//        co_await p.initial_suspend();
//        // function body
//        int a = 0b101010;
//        p.return_value(a);
//    } catch (...) {
//        p.unhandled_exception();
//    }
//
//    co_await p.final_suspend();
    int a = 0b101010;
    co_return a;
}

task<void> range(size_t n) {
    for (size_t i = 0; i < n; ++i) {
        co_yield i; // co_await promise.yield_value(i);
    }
    // promise.return_void()
}

int main() {
    std::cout << "Starting...\n";
    auto task = answer();
    std::cout << "Got task\n";
    int result = task.result();
    std::cout << "Got result: " << result;
}
