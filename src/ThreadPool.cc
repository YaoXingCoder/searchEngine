/*************************************************************************
    > File Name: threadPool.cpp
    > Author: JiaZiChunQiu
    > Created Time: 2024年09月15日 星期日 09时08分34秒
    > Mail: JiaZiChunQiu@163.com
    > Title: 线程池
    > Content:
 ************************************************************************/

#include "ThreadPool.h"

#include <iostream>

/* 构造 
 * const std::size_t threadNum : 线程数 = 3
 * const std::size_t queSize : 阻塞队列大小 = 10
 */
ThreadPool::ThreadPool(const std::size_t threadNum, const std::size_t queSize)
    : _threadNum(threadNum), _threads(), _isExit(false), _taskQue(queSize), _queSize(queSize) {
}

/* 析构 */
ThreadPool::~ThreadPool() {
    if (!_isExit) {
        stop();
    }
}

/* 循环常见线程 */
void ThreadPool::start() {
    for (int i = 0; i < _threadNum; ++i) {
        _threads.emplace_back(std::thread([this]() { this->doTask(); }));
    }
}

/* 停止 */
void ThreadPool::stop() {
    // 1.主线程等待队列为null, (等待子线程清空阻塞队列)
    while (!_taskQue.empty()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    } 

    _isExit = true;    // 标志位更新
    _taskQue.wakeup(); // 唤醒所有阻塞线程

    // 等待所有线程结束
    for (std::thread &th : _threads) {
        th.join();
    } 
}
void ThreadPool::addTask(Task &&taskcb) {
    if (taskcb) {
        _taskQue.push(std::move(taskcb));
    }
}
void ThreadPool::addTask(const Task &taskcb) {
    if (taskcb) {
        _taskQue.push(taskcb);
    }
}

ThreadPool::Task ThreadPool::getTask() {
    return _taskQue.pop();
}

void ThreadPool::doTask() {
    while (!_isExit) {
        Task taskcb = getTask();
        if (taskcb) {
            taskcb();
        } else {
            std::cerr << "ptask is null" << std::endl;
        }
    }
    std::cout << "end of thread is " << std::this_thread::get_id() << std::endl;
}
