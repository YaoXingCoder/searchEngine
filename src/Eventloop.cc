/*************************************************************************
    > File Name: Eventloop.cpp
    > Author: JiaZiChunQiu
    > Created Time: 2024年09月20日 星期五 20时27分40秒
    > Mail: JiaZiChunQiu@163.com
    > Title:
    > Content:
 ************************************************************************/

#include "Eventloop.h"
#include "Acceptor.h"
#include "TcpConnection.h"

#include <errno.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include <iostream>

Eventloop::Eventloop(Acceptor &acceptor, std::size_t evtListCapa)
    : _epfd(createEpollFd()), _evtList(evtListCapa), _isLooping(false), _acceptor(acceptor), _evtfd(createEventFd()),
      _pendings() // 任务容器默认初始化
      ,
      _mutex() // 互斥锁, 默认构造即可
{
    int listenfd = acceptor.getListenFD(); // 获取 server 的 listenfd
    addEpollReadFd(listenfd);              // 将listenfd添加到事件监听
    addEpollReadFd(_evtfd);                // 将 eventfd (进程间通信描述符) 加入监听树
}

/*
 * 析构
 * 关闭epfd套接字
 * 关闭evtfd套接字
 */
Eventloop::~Eventloop() {
    close(_epfd);
    close(_evtfd);
}

/*
 * 循环开始
 */
void Eventloop::loop() {
    _isLooping = true;
    while (_isLooping) {
        waitEpollFd();
    } // 进入循环
}

/*
 * 使循环结束, 变更标志位
 */
void Eventloop::unloop() {
    _isLooping = false;
}

/*
 * 监听事件
 * 处理就绪队列
 */
void Eventloop::waitEpollFd() {
    int nready = 0; // 就绪个数
    do {
        nready = epoll_wait(_epfd, _evtList.data(), _evtList.size(), 3000);
    } while (-1 == nready && errno == EINTR); // 能容忍的异常
    if (nready == -1) {
        perror("epoll_wait");
        return;
    } // 异常
    else if (0 == nready) {
        std::cout << ">> epoll_wait timeout" << std::endl;
    } // 等待事件结束
    else {
        if (_evtList.size() == nready) {
            _evtList.resize(2 * nready); // 同一时间就绪事件达到就绪容器大小, 进行扩容
        }

        // 循环遍历所有就绪事件
        for (int idx = 0; idx < nready; ++idx) {
            int sockfd = _evtList[idx].data.fd;     // 获取到就绪队列中的fd
            int listenfd = _acceptor.getListenFD(); // 获取到 server 的 listenfd

            /*
             * 1.新用户连接, 且为读就绪
             * 2.任务队列就绪
             * 3.已连接客户端消息就绪
             */
            if (listenfd == sockfd) {
                if (EPOLLIN & _evtList[idx].events) {
                    handleNewConnection(); // 处理新连接请求
                }
            } else if (_evtfd == sockfd) {
                // 线程间通讯描述符就绪
                if (EPOLLIN & _evtList[idx].events) {
                    handleRead();        // 接收通讯, 并清空
                    doPendingFunctors(); // 处理任务队列
                }
            } else {
                if (EPOLLIN & _evtList[idx].events) {
                    handleMessage(sockfd); // 处理客户端上的数据的收发
                }
            }
        }
    }
}

/*
 * 处理新连接
 */
void Eventloop::handleNewConnection() {
    int connfd = _acceptor.accept(); // 1.获取与客户端关联的文件描述符
    if (connfd == -1) {
        perror("handleNewConnection");
        return;
    }

    addEpollReadFd(connfd); // 添加套接字到监听事件中

    TcpConnectionPtr conn(new TcpConnection(connfd, this)); // 生成 tcp通信对象, 放入容器中
    _conns[connfd] = conn;                                  // 添加新的TcpConnection到map中

    /*
     * 连接成功后设置任务处理函数
     * 为每个连接类, 都复制一份函数内容(函数地址), 用于执行任务
     */
    conn->setConnectionCallBack(_onConnection);
    conn->setMessageCallBack(_onMessage);
    conn->setClosedCallBack(_onClosed);

    /* 通过TcpConnection对象调用对应函数 */
    conn->handleConnectionCallBack(); // 调用连接时函数
}
/*
 * 处理客户端任务
 */
void Eventloop::handleMessage(int connfd) {
    std::map<int, TcpConnectionPtr>::iterator it = _conns.find(connfd); // 从容器中找到连接
    if (it != _conns.end()) {
        bool flag = it->second->isColsed(); // 连接是否关闭
        if (flag) {
            it->second->handleClosedCallBack(); // 连接关闭处理函数
            delEpollReadFd(connfd);             // 将连接从监听树中移除
            _conns.erase(it);                   // 删除map中的连接对象
        } else {
            it->second->handleMessageCallBack(); // 已连接客户端处理函数
            /* std::string msg = it->second->recvive(); */
            /* it->second->send(msg); // 回应客户端消息 */
        }
    } else {
        std::cout << "Connection does not exist." << std::endl;
        return;
    }
}

/*
 * 创建 epollfd 底层结构
 */
int Eventloop::createEpollFd() {
    return epoll_create1(0);
}
/* 添加读就绪监听事件 */
void Eventloop::addEpollReadFd(int sockfd) {
    struct epoll_event evt;
    evt.data.fd = sockfd;
    evt.events = EPOLLIN;

    int ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, sockfd, &evt);
    if (ret == -1) {
        perror("epoll_ctl add");
        return;
    }
}
/* 删除读就绪事件 */
void Eventloop::delEpollReadFd(int sockfd) {
    int ret = epoll_ctl(_epfd, EPOLL_CTL_DEL, sockfd, nullptr);
    if (ret == -1) {
        perror("epoll_ctl del");
        return;
    }
}

/*
 * 回调函数添加
 */
void Eventloop::setConnectionCallBack(TcpConnectionCallBack &&cb_connection) {
    _onConnection = std::move(cb_connection);
}
void Eventloop::setMessageCallBack(TcpConnectionCallBack &&cb_message) {
    _onMessage = std::move(cb_message);
}
void Eventloop::setClosedCallBack(TcpConnectionCallBack &&cb_closed) {
    _onClosed = std::move(cb_closed);
}

/*
 * V4 添加线程与任务容器
 * 三个线程用于线程间通讯, 阻塞和唤醒
 */
int Eventloop::createEventFd() {
    int evtfd = eventfd(10, 0);
    if (evtfd < 0) {
        perror("createEventFd");
        return -1;
    }
    return evtfd;
}
void Eventloop::handleRead() {
    uint64_t one = 1;
    ssize_t ret = read(_evtfd, &one, sizeof(one));
    if (ret != sizeof(uint64_t)) {
        perror("handleRead");
        return;
    }
}
void Eventloop::wakeup() {
    uint64_t one = 1;
    ssize_t ret = write(_evtfd, &one, sizeof(one));
    if (ret != sizeof(uint64_t)) {
        perror("wakeup");
        return;
    }
}

/* 执行线程处理完后放入的任务
 * 由主线程来完成
 */
void Eventloop::doPendingFunctors() {
    std::vector<Functor> tmp;
    {
        std::lock_guard<std::mutex> lg(_mutex); // 锁
        tmp.swap(_pendings);
    }
    for (Functor &fc : tmp) {
        fc();
    }
}

void Eventloop::putFunctorsPendings(Functor &&callBack) {
    {
        std::lock_guard<std::mutex> lg(_mutex);
        _pendings.push_back(std::move(callBack)); // 将线程处理完的任务存放如主线程的任务容器中
    }
    wakeup(); // 线程将会执行到这里结束, 同时唤醒主线程用于发送处理完的任务
}
