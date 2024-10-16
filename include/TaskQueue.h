#ifndef __TASKQUEUE_HH__
#define __TASKQUEUE_HH__

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>

class TaskQueue {
  public:
    using Element = std::function<void()>;
    TaskQueue(const std::size_t capacity = 10);
    ~TaskQueue();

  public:
    bool full() const;
    bool empty() const;
    void push(Element &&);
    void push(const Element &);
    Element pop();
    void wakeup();

  private:
    std::size_t _capacity; // 队列大小
    std::queue<Element> _que; // 队列容器
    std::mutex _mutex; // 互斥锁
    std::condition_variable _notFull; // 条件变量
    std::condition_variable _notEmpty; // 条件变量
    bool _flag; // 退出标位置, false退出
};

#endif
