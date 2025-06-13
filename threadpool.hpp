#pragma once

#include <condition_variable>
#include <functional>
#include <thread>
#include <utility>
#include <vector>
#include <mutex>
#include <list>

class ThreadPool
{
public:
    template <typename ...Args>
    static void AddTask(Args &&...args);

private:
    static ThreadPool s_ThreadPool;

    std::list<std::function<void()>> m_task;

    std::vector<std::thread> m_threads;

    std::condition_variable m_cv;

    static std::mutex m_mutex;

    bool m_bStop;

    std::uint8_t m_available;

    ThreadPool(std::uint32_t maxThread);
    ~ThreadPool();

    template <typename ...Args>
    void add(Args &&...args);

    void update();

    bool wait_for();
};

template <typename... Args>
inline void ThreadPool::AddTask(Args &&...args)
{
    return s_ThreadPool.add(std::forward<Args>(args)...);
}

// private
template <typename ...Args>
inline void ThreadPool::add(Args &&...args)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_task.emplace_back(std::bind(std::forward<Args>(args)...));
    }
    m_cv.notify_one();
}
