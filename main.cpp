
#include "ThreadPool.h"
#include <iostream>
#include <chrono>

int main() {
    ThreadPool pool(4);

    std::cout << "Pending tasks: " << pool.pendingTasks() << "\n";

    auto future1 = pool.enqueue([]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return 42;
        });

    auto future2 = pool.enqueue([](int a, int b) {
        return a + b;
        }, 10, 20);

    std::cout << "Pending tasks: " << pool.pendingTasks() << "\n";

    std::cout << "Result 1: " << future1.get() << "\n";
    std::cout << "Result 2: " << future2.get() << "\n";

    pool.shutdown();

    std::cout << "Is stopped: " << pool.isStopped() << "\n";

    return 0;
}