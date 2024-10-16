#ifndef __EVENTLOOP_HH__
#define __EVENTLOOP_HH__

#include <sys/epoll.h>

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

/* 前向声明 */
class Acceptor;
class TcpConnection;

#define EPOLL_EVENT_MAX 1024

class Eventloop {
  public:
    // 连接存放在 map 的结构中, 但连接应该唯一, 对象语义, 且后序需要使用, 所以采用 shared_ptr 方便管理,
    // 且不会发生复制和赋值;
    using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
    using TcpConnectionCallBack = std::function<void(const TcpConnectionPtr &)>; // Tcp 回调函数类型
    using Functor = std::function<void()>;                                       // 任务队列中的函数类型

  public:
    Eventloop(Acceptor &, std::size_t evtListCapa = EPOLL_EVENT_MAX);
    ~Eventloop();

  public:
    void loop();   // 循环开始
    void unloop(); // 结束循环监听

  private:
    void waitEpollFd();         // 等待时间就绪
    void handleNewConnection(); // 处理新连接
    void handleMessage(int);    // 监听就绪后, 处理消息

    int createEpollFd();      // 创建 epoll 内核结构, 返回 fd
    void addEpollReadFd(int); // 添加套接字到监听结构(红黑树)
    void delEpollReadFd(int); // 删除套接字从监听结构

  public: // 回调函数设置
    void setConnectionCallBack(TcpConnectionCallBack &&);
    void setMessageCallBack(TcpConnectionCallBack &&);
    void setClosedCallBack(TcpConnectionCallBack &&);

  public:                                 // V4
    int createEventFd();                  // 创建线程间通讯套接字
    void doPendingFunctors();             // 遍历并执行任务, 清空任务容器
    void putFunctorsPendings(Functor &&); // 存放任务到_pendings

  private:
    void handleRead(); // 封装read, 读取_evtfd套接字
    void wakeup();     // 封装write, 写入内容, 用做通信

  private:                                    // V1 构建循环监听
    int _epfd;                                // 监听结构文件描述符
    std::vector<struct epoll_event> _evtList; // 事件就绪队列
    bool _isLooping;                          // 循环标志
    Acceptor &_acceptor;                      // 服务器相关操作的对象, 唯一, 使用该类的 accept 函数
    std::map<int, TcpConnectionPtr> _conns;   // 文件描述符 int <--> 连接对象 TcpConnection : 键值对

  private:                               // V2 添加自定义函数，通过回调函数实现
    TcpConnectionCallBack _onConnection; // 保存 回调函数内容
    TcpConnectionCallBack _onMessage;
    TcpConnectionCallBack _onClosed;

  private:                          // V4 添加线程池, 用于线程间任务和通信
    int _evtfd;                     // 线程间通信套接字 eventfd
    std::vector<Functor> _pendings; // 存放任务容器
    std::mutex _mutex;              // 互斥锁, 存放和读取任务会产生冲突
};

#endif
