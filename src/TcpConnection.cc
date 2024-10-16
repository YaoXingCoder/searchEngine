/*************************************************************************
    > File Name: TcpConnection.cpp
    > Author: JiaZiChunQiu
    > Created Time: 2024年09月20日 星期五 16时31分01秒
    > Mail: JiaZiChunQiu@163.com
    > Title: tcp连接类, 该类的套接字都是 accept 返回的用于通讯的套接字
    > Content:
 ************************************************************************/

#include "TcpConnection.h"
#include "Eventloop.h"

#include <iostream>
#include <sstream>

TcpConnection::TcpConnection(int fd, Eventloop *loop)
    : _socketIO(fd), _socket(fd), _localAddr(getLocalAddr()), _peerAddr(getPeerAddr()), _loop(loop) {
}

TcpConnection::~TcpConnection() {
}

std::string TcpConnection::recvive() {
    char buf[BUFSIZ] = {0}; // stdio 中 #define BUFSIZ 8192
    _socketIO.readLine(buf, sizeof(buf));
    return std::string(buf);
}

void TcpConnection::send(const std::string &msg) {
    _socketIO.writen(msg.c_str(), msg.size());
}

/* 返回本地服务器 和 客户端通信地址 */
std::string TcpConnection::toString() {
    std::ostringstream oss;
    oss << _localAddr.getIP() << ":" << _localAddr.getPort() << "---->" << _peerAddr.getIP() << ":"
        << _peerAddr.getPort();
    return oss.str();
}

/* 获取本地服务器的ip/port */
InetAddress TcpConnection::getLocalAddr() {
    struct sockaddr_in localAddr;
    socklen_t localAddr_len = sizeof(localAddr);
    int ret = getsockname(_socket.getFD(), (struct sockaddr *)&localAddr, &localAddr_len);
    if (-1 == ret) {
        perror("getsockname");
    }
    return InetAddress(localAddr);
}
/* 获取客户端ip/port */
InetAddress TcpConnection::getPeerAddr() {
    struct sockaddr_in peerAddr;
    socklen_t peerAddr_len = sizeof(peerAddr);
    int ret = getpeername(_socket.getFD(), (struct sockaddr *)&peerAddr, &peerAddr_len);
    if (-1 == ret) {
        perror("getpeername");
    }
    return InetAddress(peerAddr);
}

/*
 * 检测客户端是否关闭
 * 通过视图接收信息, 判断返回值是否为 0
 */
bool TcpConnection::isColsed() {
    char buf[10];
    int ret = recv(_socket.getFD(), buf, sizeof(buf), MSG_PEEK);
    return ret == 0;
}

/*
 * 回调函数添加
 */
void TcpConnection::setConnectionCallBack(const TcpConnectionCallBack &cb_connection) {
    _onConnection = cb_connection;
}
void TcpConnection::setMessageCallBack(const TcpConnectionCallBack &cb_message) {
    _onMessage = cb_message;
}
void TcpConnection::setClosedCallBack(const TcpConnectionCallBack &cb_closed) {
    _onClosed = cb_closed;
}

/*
 * 回调函数执行
 */
void TcpConnection::handleConnectionCallBack() {
    if (_onConnection) {
        _onConnection(shared_from_this());
    } else {
        std::cout << "_onMessage is nullptr" << std::endl;
    }
}
void TcpConnection::handleMessageCallBack() {
    if (_onMessage) {
        _onMessage(shared_from_this());
    } else {
        std::cout << "_onMessage is nullptr" << std::endl;
    }
}
void TcpConnection::handleClosedCallBack() {
    if (_onClosed) {
        _onClosed(shared_from_this());
    } else {
        std::cout << "_onClosed is nullptr" << std::endl;
    }
}

/*
 * V4 消息处理结束后通知 EventLoop 类
 */
void TcpConnection::sendToLoop(const std::string &msg) {
    if (_loop) {
        _loop->putFunctorsPendings(std::bind(&TcpConnection::send, this, msg));
    }
}
