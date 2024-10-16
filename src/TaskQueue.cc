/*************************************************************************
    > File Name: taskQueue.cpp
    > Author: JiaZiChunQiu
    > Created Time: 2024年09月14日 星期六 23时28分29秒
    > Mail: JiaZiChunQiu@163.com
    > Title: 线程池的阻塞队列实现
    > Content:
 ************************************************************************/

#include "TaskQueue.h"

/* 
 * 构造
 * const std::size_t capacity : 阻塞队列大小
 */
TaskQueue::TaskQueue(const std::size_t capacity)
    : _capacity(capacity), _que(), _mutex(), _notFull(), _notEmpty(), _flag(true) {
}
TaskQueue::~TaskQueue() {
}

bool TaskQueue::full() const {
    return _que.size() == _capacity;
}
bool TaskQueue::empty() const {
    return _que.empty();
}
void TaskQueue::push(Element &&ele) {
    std::unique_lock<std::mutex> ul(_mutex); // 上锁

    while (full()) {
        _notFull.wait(ul);
    } // 判满, 等待唤醒

    _que.push(std::move(ele)); // 放入任务

    _notEmpty.notify_one(); // 队列不空, 唤醒
}

void TaskQueue::push(const Element &ele) {
    std::unique_lock<std::mutex> ul(_mutex); // 上锁

    while (full()) {
        _notFull.wait(ul);
    } // 判满, 等待唤醒

    _que.push(ele); // 放入任务

    _notEmpty.notify_one(); // 队列不空, 唤醒
}

TaskQueue::Element TaskQueue::pop() {
    std::unique_lock<std::mutex> ul(_mutex); // 上锁

    while (empty() && _flag) {
        _notEmpty.wait(ul);
    } // 持续判空, 等待唤醒

    /* 标志位false, 不再取出任务 */
    if (_flag) {
        Element tmp = _que.front(); // 存储临时任务
        _que.pop();                 // 取出任务

        _notFull.notify_one(); // 队列不满, 唤醒
        return tmp;            // 返回任务
    } else {
        return nullptr;
    }
}

void TaskQueue::wakeup() {
    _flag = false;          // 标志位false
    _notEmpty.notify_all(); // 唤醒所有等待取任务的条件
}
