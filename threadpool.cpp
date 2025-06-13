#include "threadpool.hpp"
#include <iostream>
#include <mutex>
#include <thread>

ThreadPool ThreadPool::s_ThreadPool(std::thread::hardware_concurrency());

std::mutex ThreadPool::m_mutex;

// static
bool ThreadPool::IsMainThread()
{
    return s_ThreadPool.isMainThread();
}

// private
ThreadPool::ThreadPool(std::uint32_t maxThread) : m_bStop(false), m_available(0), m_mainThreadID(std::this_thread::get_id())
{
    for (std::uint32_t i=0;i<maxThread;i++)
    {
        m_threads.emplace_back(&ThreadPool::update, this);
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [&] { return m_available == 0 && m_task.empty(); });
    }

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_bStop = true;
    }

    m_cv.notify_all();

    for (auto &thread : m_threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

bool ThreadPool::isMainThread() const
{
    return std::this_thread::get_id() == m_mainThreadID;
}

void ThreadPool::update()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, std::bind(&ThreadPool::wait_for, this));

        if (m_bStop)
        {
            break;
        }

        {
            m_available++;
            m_cv.notify_all();
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
        {
            m_available--;
            m_cv.notify_all();
        }
    }
}

bool ThreadPool::wait_for()
{
    return m_bStop || !m_task.empty();
}


