#include "threadpool.hpp"
#include <iostream>

std::list<std::function<void()>> ThreadPool::m_task;

std::vector<std::thread> ThreadPool::m_threads;

std::condition_variable ThreadPool::m_cv;

std::mutex ThreadPool::m_mutex;

bool ThreadPool::m_bStop = false;

void ThreadPool::Initialize(std::uint32_t maxThread)
{
    for (std::uint32_t i=0;i<maxThread;i++)
    {
        m_threads.emplace_back(&ThreadPool::Update);
    }
}

void ThreadPool::Shutdown()
{
    Stop();

    for (auto &thread : m_threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

void ThreadPool::Stop()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_bStop = true;
    }
    m_cv.notify_all();
}

void ThreadPool::Update()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [] {
            return m_bStop || !m_task.empty();
        });

        if (m_bStop)
        {
            break;
        }

        auto temp = std::move(m_task);
        lock.unlock();

        try
        {
            for (auto &task : temp)
                task();
        } catch (const std::exception &e)
        {
            std::cerr << "ThreadPool: " << e.what() << std::endl;
        }

        lock.lock();
    }
}
