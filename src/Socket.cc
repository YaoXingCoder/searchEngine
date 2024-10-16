/*************************************************************************
    > File Name: Socket.cpp
    > Author: JiaZiChunQiu
    > Created Time: 2024年09月20日 星期五 09时41分01秒
    > Mail: JiaZiChunQiu@163.com
    > Title: Socket 实现
    > Content:
 ************************************************************************/

#include "Socket.h"

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

/* #include <iostream> */

/* 无参构造 默认创建一个 TCP 网络套接字 */
Socket::Socket() {
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd < 0) {
        perror("socket");
        return;
    }
}

/* 有参构造 初始化成员变量 */
Socket::Socket(int fd) : _fd(fd) {
}

/* 关闭资源 */
Socket::~Socket() {
    close(_fd);
}

/* 获取到套接字 */
int Socket::getFD() const {
    return _fd;
}
