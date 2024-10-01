#include <iostream>

#include "threadpool.hpp"

int main(int, char**) {
    ThreadPool::Initialize();

    for (int i=0;i<1000;++i) {
        ThreadPool::AddTask([i] {
            std::cout << "Task: " << i << std::endl;
        });
    }

    std::string ch;
    std::cin >> ch;

    ThreadPool::Shutdown();
}
