#include "taskmanager.hpp"

// private
TaskManager TaskManager::s_TaskManager;

// public


// private
void TaskManager::update(std::shared_ptr<AsyncTask> task)
{
    task->doInBackground();
}

