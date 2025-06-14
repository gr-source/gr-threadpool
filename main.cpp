#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

#include "taskmanager.hpp"
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

class ExampleTask : public AsyncTask
{
public:
    ExampleTask(int id) : id(id)
    {
        if (ThreadPool::IsMainThread())
        {
            std::cout << "ExampleTask-C: Main" << std::endl;
        } else
        {
            std::cout << "ExampleTask-C: No Main" << std::endl;
        }
    }

    ~ExampleTask()
    {
        // std::lock_guard<std::mutex> lock(mutex);
        if (ThreadPool::IsMainThread())
        {
            std::cout << "ExampleTask-D: Main" << std::endl;
        } else
        {
            std::cout << "ExampleTask-D: No Main" << std::endl;
        }
    }

    // Assegure que os dados compartilhados estejam na safe thread, std::cout não é seguro
    void doInBackground() override
    {
        std::this_thread::sleep_for(std::chrono::seconds(10));

        {
            std::lock_guard<std::mutex> lock(mutex);
            sum += id;

            // std::cout << "ExampleTask: " << sum << std::endl;
        }  
    }

private:
    int id;
};

int main(int, char**)
{
    /* resultado deve ser: 499500 */
    // for (int i=0;i<1000;++i)
        // TaskManager::Add<ExampleTask>(10);
    
    /* resultado deve ser: 499500 */
    // for (int i=0;i<1000;++i)
    {
        // ThreadPool::AddTask(&print, i);
    }
    // */
    //
    std::cout << "Create Task" << std::endl;
    auto info = TaskManager::Add<ExampleTask>(10);
    
    std::cout << "Wai for task" << std::endl;
    info->wait();
    
    std::cout << "Task completed" << std::endl;

}
