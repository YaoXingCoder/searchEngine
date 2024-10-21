#ifndef __TCPCONNECTION_HH__
#define __TCPCONNECTION_HH__

#include "InetAddress.h"
#include "Socket.h"
#include "SocketIO.h"

#include <functional>
#include <memory>

class Eventloop;

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
  public:
    using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
    using TcpConnectionCallBack = std::function<void(const TcpConnectionPtr &)>;

  public:
    // 一个连接是不能复制的, 所以禁用隐式转换 TcpConnection tcpCoon = 10; 以及这样的隐式转换
    explicit TcpConnection(int, Eventloop *);
    ~TcpConnection();

  public:
    std::string receive();
    std::string readSys();
    void send(const std::string &);
    bool isColsed(); // 检测客户端是否关闭

    /* 测试 */
    std::string toString(); // 测试 : 输出 本地连接和客户端连接

  private:
    /*
     * 测试类
     * accept 返回的套接字
     * 通过 getpeername() 能够获取到 客户端地址
     * 通过 getsockname() 能够获取到 本地服务器的地址
     */
    InetAddress getPeerAddr();  // 获取客户端连接
    InetAddress getLocalAddr(); // 获取本地服务器连接

  public:
    /*
     * 任务添加和执行
     */
    void setConnectionCallBack(const TcpConnectionCallBack &);
    void setMessageCallBack(const TcpConnectionCallBack &);
    void setClosedCallBack(const TcpConnectionCallBack &);
    void handleConnectionCallBack();
    void handleMessageCallBack();
    void handleClosedCallBack();

  public: // 发送消息给 EventLoop 类
    void sendToLoop(const std::string &msg);

  private:
    /* 打印测试 */
    SocketIO _socketIO; // 数据传输类
    Socket _socket; // 必须定义在 InetAddress 前边, 因为InetAddress初始化时调用函数, 函数中有使用该成员类
    InetAddress _localAddr; // 保存 本地地址
    InetAddress _peerAddr;  // 保存 客户端地址

  private:
    /*
     * 存储回调函数的地址
     * 作为每个
     */
    TcpConnectionCallBack _onConnection;
    TcpConnectionCallBack _onMessage;
    TcpConnectionCallBack _onClosed;

  private:            // V4
    Eventloop *_loop; // 需要访问loop的成员函数, 添加任务到 eventLoop 中
};

#endif
