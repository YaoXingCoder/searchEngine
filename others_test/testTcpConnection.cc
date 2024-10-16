/*************************************************************************
    > File Name: TestTcpConnection.cpp
    > Author: JiaZiChunQiu
    > Created Time: 2024年09月20日 星期五 16时46分19秒
    > Mail: JiaZiChunQiu@163.com
    > Title: 测试类
    > Content:
 ************************************************************************/

#include "Acceptor.h"
#include "Eventloop.h"
#include "TcpConnection.h"
#include "TcpServer.h"

#include "ThreadPool.h"

#include <functional>
#include <iostream>

using TcpConnectionPtr = Eventloop::TcpConnectionPtr;

ThreadPool *p_pool = nullptr;

class MyTask {
  public:
    MyTask(const std::string &msg, const TcpConnectionPtr &con) : _msg(msg), _con(con) {
    }
    ~MyTask() {
    }

  public:
    // 子线程的任务
    void process() {

        /* 
         * 进行任务的处理 
         * 此处是子线程进行处理
        */

        _con->sendToLoop(_msg); // 处理结束后, 放入 eventLoop 中, 主线程做最后处理
    }

  private:
    std::string _msg;      // 需要处理的信息
    TcpConnectionPtr _con; // 一个tcp连接
};

/* 连接成功后任务 */
void onConnection(const TcpConnectionPtr &conn) {
    std::cout << "onConnection is running" << std::endl;
    std::cout << conn->toString() << " has connected" << std::endl;
}

/* 接收消息后任务 */
void onMessage(const TcpConnectionPtr &conn) {
    std::string msg = conn->receive();
    std::cout << ">> sever received : " << msg << std::endl;
    /* conn->send(msg); */
    MyTask myTask(msg, conn); // 创建任务对象
    p_pool->addTask(std::bind(&MyTask::process, &myTask)); // 向任务添加到阻塞队列中
}

void onClosed(const TcpConnectionPtr &conn) {
    std::cout << conn->toString() << " has closed" << std::endl;
}

/* =============== test =============== */
#if 0
/* V1 没有时间循环监听 */
void test0() {
    Acceptor acceptor("127.0.0.1", 8888);
    acceptor.ready(); // 服务端启动

    TcpConnection con(acceptor.accept()); // 等待 accept 获取连接
    
    std::cout << con.toString() << std::endl; // 调试输出

    while(1) {
        std::cout << ">> recv msg from client : " << con.recvive() << std::endl; // 接收客户端信息
        con.send("hello, client"); // 客户端返回信息
    }
}
#endif

/* 添加事件监听 */
void test1() {
    Acceptor acceptor("127.0.0.1", 8888);
    /* acceptor.ready(); */

    Eventloop evloop(acceptor);
    evloop.loop();
}

void test2() {
    Acceptor acceptor("127.0.0.1", 8888);

    Eventloop evloop(acceptor);
    evloop.setConnectionCallBack(std::move(onConnection));
    evloop.setMessageCallBack(std::move(onMessage));
    evloop.setClosedCallBack(std::move(onClosed));
    evloop.loop();
}

void test3() {
    TcpServer server("127.0.0.1", 8888);
    server.setAllCallBack(std::move(onConnection), std::move(onMessage), std::move(onClosed));
    server.start();
}

void test4() {
    ThreadPool pool;
    pool.start();
    p_pool = &pool;

    TcpServer server("127.0.0.1", 8888);
    server.setAllCallBack(std::move(onConnection), std::move(onMessage), std::move(onClosed));
    server.start();
}

/* =============== main =============== */
int main(int argc, char *argv[]) {
    /* test0(); */
    /* test1(); */
    /* test2(); */
    /* test3(); */
    test4();
    return 0;
}
