/*************************************************************************
    > File Name: InetAddress.cpp
    > Author: JiaZiChunQiu
    > Created Time: 2024年09月20日 星期五 10时19分11秒
    > Mail: JiaZiChunQiu@163.com
    > Title: ip/port地址类 实现
    > Content:
 ************************************************************************/

#include "InetAddress.h"
/* #include <iostream> */

/* 构造 : 存放指定ip和端口 */
InetAddress::InetAddress(const std::string &ip, unsigned short port) {
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = inet_addr(ip.c_str());
    _addr.sin_port = htons(port);
}

/* 拷贝构造 */
InetAddress::InetAddress(const struct sockaddr_in &addr) : _addr(addr) {
}

/* 拷贝构造 */
InetAddress::InetAddress(const InetAddress &other) : _addr(other._addr) {
}

/* 析构 */
InetAddress::~InetAddress() {
}

/* get 函数 */
std::string InetAddress::getIP() {
    char ip_str[INET_ADDRSTRLEN];
    return std::string(inet_ntop(AF_INET, &_addr.sin_addr, ip_str, INET_ADDRSTRLEN));
}

unsigned short InetAddress::getPort() {
    return ntohs(_addr.sin_port);
}

struct sockaddr_in *InetAddress::getInetAddressPtr() {
    return &_addr;
}

/* 测试 */
/* int main() { */
/*     InetAddress inet_addr("127.0.0.1", 8888); */
/*     std::cout << inet_addr.getInetAddressPtr() << std::endl; */
/*     std::cout << inet_addr.getIP() << std::endl; */
/*     std::cout << inet_addr.getPort() << std::endl; */
/*     return 0; */
/* } */
