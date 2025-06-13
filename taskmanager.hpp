#pragma once

#include "threadpool.hpp"
#include "asynctask.hpp"

#include <mutex>
#include <utility>
#include <vector>
#include <memory>

class TaskManager
{
public:
    template <class T, typename ...Args>
    static void Add(Args &&...args);

    TaskManager() = default;
    ~TaskManager() = default;

    template <class T, typename ...Args>
    void add(Args &&...args);

private:
    static TaskManager s_TaskManager;

    std::vector<std::unique_ptr<AsyncTask>> taskList;

    // std::mutex mutex;

    void update(std::shared_ptr<AsyncTask> task);
};

// static
template <class T, typename ...Args>
inline void TaskManager::Add(Args &&...args)
{
    return s_TaskManager.add<T>(std::forward<Args>(args)...);
}

// default
template <class T, typename ...Args>
inline void TaskManager::add(Args &&...args)
{
    ThreadPool::AddTask(&TaskManager::update, this, std::make_shared<T>(std::forward<Args>(args)...));
}


