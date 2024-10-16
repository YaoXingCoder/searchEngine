#ifndef __TCPSERVER_HH__
#define __TCPSERVER_HH__

#include "Acceptor.h"
#include "Eventloop.h"

class TcpServer {
  public:
    using TcpConnectionPtr = Eventloop::TcpConnectionPtr;
    using TcpConnectionCallBack = Eventloop::TcpConnectionCallBack;

  public:
    TcpServer(const std::string &ip, unsigned short port, std::size_t evtCapa = EPOLL_EVENT_MAX);
    ~TcpServer();

  public:
    void start();
    void stop();
    void setAllCallBack(TcpConnectionCallBack &&cb_connection, TcpConnectionCallBack &&cb_message,
                        TcpConnectionCallBack &&cb_closed);

  private:
    Acceptor _acceptor; // 创建服务器
    Eventloop _loop;
};

#endif
