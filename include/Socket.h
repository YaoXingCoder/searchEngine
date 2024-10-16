#ifndef __SOCKET_HH__
#define __SOCKET_HH__

#include "NonCopyable.h"

/*
 * 管理网络套接字类
 * 析构自动删除套接字, 所以有关套接字的传递统一采用 Socket 对象传递
 */
class Socket : NonCopyable {
  public:
    Socket();                // 无参
    explicit Socket(int fd); // 有参, 禁止隐式转换
    ~Socket();

  public:
    int getFD() const; // 获取套接字

  private:
    int _fd; //
};

#endif
