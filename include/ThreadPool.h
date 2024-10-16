#ifndef __THREADPOOL_HH__
#define __THREADPOOL_HH__

#include "TaskQueue.h"

#include <functional>
#include <thread>
#include <vector>

class ThreadPool {
  public:
    using Task = std::function<void()>;
    ThreadPool(const std::size_t threadNum = 3, const std::size_t queSize = 10);
    ~ThreadPool();

  public:
    void start();
    void stop();
    void addTask(Task &&);
    void addTask(const Task &);
    Task getTask();
    void doTask();

  private:
    std::size_t _threadNum;
    std::vector<std::thread> _threads;
    bool _isExit;
    TaskQueue _taskQue;
    std::size_t _queSize;
};

#endif
