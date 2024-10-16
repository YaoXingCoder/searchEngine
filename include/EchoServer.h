#ifndef __ECHOSERVER_HH__
#define __ECHOSERVER_HH__


#include "HttpRequestParser.h"
#include "TcpServer.h"
#include "ThreadPool.h"

/*
 * 封装类
 */
class EchoServer {
  public:
    using TcpConnectionPtr = TcpServer::TcpConnectionPtr;

  public:
    EchoServer(std::size_t threadNum, std::size_t queSize, const std::string &ip, unsigned short port);
    ~EchoServer();

  public:
    void start();
    void stop();
    void OnConnection(const TcpConnectionPtr &);
    void OnMessage(const TcpConnectionPtr &);
    void OnClosed(const TcpConnectionPtr &);

  private:
    ThreadPool _pool;  // 线程池对象
    TcpServer _server; // 服务器对象
};

/*
 * 子线程任务类
 */
class MyTask {
  public:
    using TcpConnectionPtr = TcpServer::TcpConnectionPtr;

  public:
    MyTask(const std::string &msg, const TcpConnectionPtr &conn);
    ~MyTask();

  public:
    void process();

  private:
    std::string _msg;                 // 待处理任务
    TcpConnectionPtr _conn;           // tcp 连接
    HttpRequestParser _httpReqParser; // 解析http请求
};

#endif
