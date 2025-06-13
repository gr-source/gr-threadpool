#include <iostream>
#include <mutex>

#include "threadpool.hpp"

static std::mutex mutex;

int sum = 0;

// Não é seguro usar std::cout, printa valores as vezes incorretos, por isso não printei id
// Output: 499500
static void print(int id)
{
    {
        std::lock_guard<std::mutex> lock(mutex);
        sum += id;
    }

    std::cout << "SUM: "<< sum << std::endl;
}

int main(int, char**)
{
    ThreadPool::Initialize();

    for (int i=0;i<1000;++i)
    {
        ThreadPool::AddTask(&print, i);
    }

    std::string ch;
    std::cin >> ch;

    ThreadPool::Shutdown();
}
