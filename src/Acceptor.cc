/*************************************************************************
    > File Name: Acceptor.cpp
    > Author: JiaZiChunQiu
    > Created Time: 2024年09月20日 星期五 16时06分49秒
    > Mail: JiaZiChunQiu@163.com
    > Title: 连接器类
    > Content:
    >   将服务器的所有基本操作全部封装到该类中, 包括: 地址复用、端口复用、bind、listen、accept 等。
 ************************************************************************/

#include "Acceptor.h"

#include <sys/socket.h>
#include <sys/types.h>

/* 给定 ip/port */
Acceptor::Acceptor(const std::string &ip, unsigned short port) : _sock(), _addr(ip, port) {
    ready();
}

Acceptor::~Acceptor() {
}

/* 按给定处理 */
void Acceptor::ready() {
    setReuseAddr();
    setReusePort();
    bind();
    listen();
}

/* 地址复用 */
void Acceptor::setReuseAddr() {
    int opt = 1;
    int ret = setsockopt(_sock.getFD(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (ret) {
        perror("setsockopt SO_REUSEADDR");
        return;
    }
}

/* 端口复用 */
void Acceptor::setReusePort() {
    int opt = 1;
    int ret = setsockopt(_sock.getFD(), SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    if (ret) {
        perror("setsockopt SO_REUSEPORT");
        return;
    }
}

/* bind 套接字 绑定 ip/port */
void Acceptor::bind() {
    int ret = ::bind(_sock.getFD(), (struct sockaddr *)_addr.getInetAddressPtr(), sizeof(struct sockaddr));
    if (-1 == ret) {
        perror("bind()");
        return;
    }
}

/* listent 在指定端口开启网络监听 */
void Acceptor::listen() {
    int ret = ::listen(_sock.getFD(), LISTEN_MAX);
    if (-1 == ret) {
        perror("listen()");
        return;
    }
}

/* accept 获取已连接客户端 并生成新的通信套接字返回 */
int Acceptor::accept() {
    int connfd = ::accept(_sock.getFD(), nullptr, nullptr);
    if (-1 == connfd) {
        perror("accept");
        return -1;
    }
    return connfd;
}

/* 获取 用于监听网络的 套接字 */
int Acceptor::getListenFD() {
    return _sock.getFD();
}
