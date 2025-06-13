#include "taskmanager.hpp"

// private
TaskManager TaskManager::s_TaskManager;

// public


// private
void TaskManager::update(std::shared_ptr<AsyncTask> task, std::shared_ptr<TaskInfo> info)
{
    task->doInBackground();

    info->finish();
}

