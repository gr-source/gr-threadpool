#pragma once

#include <condition_variable>
#include <functional>
#include <thread>
#include <vector>
#include <mutex>
#include <list>

class ThreadPool {
public:
    static void Initialize(std::uint32_t maxThread = std::thread::hardware_concurrency());

    static void Shutdown();

    template <typename ...Args>
    static void AddTask(Args &&...args);

    static void Stop();

private:
    static std::list<std::function<void()>> m_task;

    static std::vector<std::thread> m_threads;

    static std::condition_variable m_cv;

    static std::mutex m_mutex;

    static bool m_bStop;

    static void Update();
};

template <typename... Args>
inline void ThreadPool::AddTask(Args &&...args) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_task.emplace_back(std::bind(std::forward<Args>(args)...));
    }
    m_cv.notify_all();
}
