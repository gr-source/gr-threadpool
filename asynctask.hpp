#pragma once

class AsyncTask
{
public:
    virtual ~AsyncTask() {}

    virtual void doInBackground() = 0;
};


