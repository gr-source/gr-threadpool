#pragma once

#include "threadpool.hpp"
#include "asynctask.hpp"

#include <condition_variable>
#include <mutex>
#include <utility>
#include <vector>
#include <memory>

struct TaskInfo
{
    bool bFinish;
    std::mutex mutex;
    std::condition_variable cv;

    TaskInfo() : bFinish(false) {}

    void wait()
    {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this] { return bFinish; });
    }

    void finish()
    {
        std::lock_guard<std::mutex> lock(mutex);
        bFinish = true;

        cv.notify_one();
    }
};

class TaskManager
{
public:
    template <class T, typename ...Args>
    static std::shared_ptr<TaskInfo> Add(Args &&...args);

    TaskManager() = default;
    ~TaskManager() = default;

    template <class T, typename ...Args>
    std::shared_ptr<TaskInfo> add(Args &&...args);

private:
    static TaskManager s_TaskManager;

    std::vector<std::unique_ptr<AsyncTask>> taskList;

    void update(std::shared_ptr<AsyncTask> task, std::shared_ptr<TaskInfo> info);
};

// static
template <class T, typename ...Args>
inline std::shared_ptr<TaskInfo> TaskManager::Add(Args &&...args)
{
    return s_TaskManager.add<T>(std::forward<Args>(args)...);
}

// default
template <class T, typename ...Args>
inline std::shared_ptr<TaskInfo> TaskManager::add(Args &&...args)
{
    auto info = std::make_shared<TaskInfo>();

    ThreadPool::AddTask(&TaskManager::update, this, std::make_shared<T>(std::forward<Args>(args)...), info);
    return info;
}


