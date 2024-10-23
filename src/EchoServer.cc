/*************************************************************************
    > File Name: EchoServer.cpp
    > Author: JiaZiChunQiu
    > Created Time: 2024年09月23日 星期一 09时02分58秒
    > Mail: JiaZiChunQiu@163.com
    > Title: Echo 封装类
    > Content:
 ************************************************************************/

#include "EchoServer.h"
#include "Dictionary.h"     // 字典
#include "KeyRecommander.h" // 候选词
#include "SplitTool.h"      // 分词工具
#include "TcpConnection.h"  // conn 需要使用该类的成员函数
#include "ThreadPool.h"
#include "WebPageQuery.h"

#include <iostream>
#include <memory>

#define SUGGEST "suggest"
#define SEARCH "search"

static Dictionary *dictionary = nullptr;
static std::unique_ptr<WebPageQuery> webPageQuery = nullptr;

/*
 * std::size_t threadNum : 线程数 = 3
 * std::size_t queSize : 阻塞队列大小 = 10
 * const std::string &ip : ip
 * unsigned short port : port
 */
EchoServer::EchoServer(std::size_t threadNum, std::size_t queSize, const std::string &ip, unsigned short port)
    : _pool(threadNum, queSize), _server(ip, port) {
}

EchoServer::~EchoServer() {
}

void EchoServer::start() {
    _pool.start(); // 线程池启动
    _server.setAllCallBack(std::bind(&EchoServer::OnConnection, this, std::placeholders::_1),
                           std::bind(&EchoServer::OnMessage, this, std::placeholders::_1),
                           std::bind(&EchoServer::OnClosed, this, std::placeholders::_1));

    // 字典类加载
    dictionary = Dictionary::getInstance();
    std::cout << "dictionary success\n";

    webPageQuery.reset(new WebPageQuery(std::shared_ptr<SplitTool>(new SplitToolCppJieba())));
    std::cout << "webPageQuery success\n";

    _server.start(); // 服务器启动
}

void EchoServer::stop() {
    _pool.stop();
    _server.stop();
}

void EchoServer::OnConnection(const TcpConnectionPtr &conn) {
    std::cout << conn->toString() << " has connected" << std::endl;
}

void EchoServer::OnMessage(const TcpConnectionPtr &conn) {
    std::string msg = conn->readSys();
    std::cout << ">> server received message is " << msg << "\n";
    MyTask task(msg, conn);                            // 创建任务对象
    _pool.addTask(std::bind(&MyTask::process, &task)); // 向任务添加到阻塞队列中
}

void EchoServer::OnClosed(const TcpConnectionPtr &conn) {
    std::cout << conn->toString() << " is closed" << std::endl;
}

/*
 * MyTask 子线程任务类
 */
// MyTask::MyTask(const std::string &msg, const TcpConnectionPtr &conn) : _msg(msg), _conn(conn), _httpReqParser(_msg) {
MyTask::MyTask(const std::string &msg, const TcpConnectionPtr &conn) : _msg(msg), _conn(conn) {
}

MyTask::~MyTask() {
}

void MyTask::process() {
    // 1.测试候选词
    // KeyRecommander keyRecommander(_msg, _conn);
    // keyRecommander.executeQuery();
    // keyRecommander.response();

    // 2.测试网页查询
    // webPageQuery->doQuery(_msg, _conn);
}

// std::string url = _httpReqParser.getUrl();
// std::cout << "url = " << _httpReqParser.getUrl() << "\n";
// std::string path = _httpReqParser.getQueryPath(); // 路径
// std::cout << "path = " << _httpReqParser.getQueryPath() << "\n";
// std::string sentence = _httpReqParser.getQueryStr(); // 句子
// std::cout << "sentence = " << _httpReqParser.getQueryStr() << "\n";

/* http 协议 */
// std::cout << _httpReqParser.getBody() << "\n";
// std::string strHttpRes; // 响应报文
// if (path == SUGGEST) {
//     strHttpRes += "HTTP/1.1 200 OK\r\n";
//     strHttpRes += "Cache-Control: private\r\n";
//     strHttpRes += "Content-Type: application/json; charset=utf-8\r\n";
//     strHttpRes += "Server: Microsoft-IIS/8.0\r\n";
//     strHttpRes += "X-AspNetMvc-Version: 5.2\r\n";
//     strHttpRes += "X-AspNet-Version: 4.0.30319\r\n";
//     strHttpRes += "X-Powered-By: ASP.NET\r\n";
//     strHttpRes += "Date: Sat, 24 Oct 2020 02:45:29 GMT\r\n";
//     strHttpRes += "Content-Length: 30\r\n";
//     strHttpRes += "\r\n";
//     strHttpRes += "{\"suggest\":\"SUGGEST\",\"Msg\":\"OK\"}";
//     _conn->sendToLoop(strHttpRes); // 处理结束
// } else if (path == SEARCH) {
//     strHttpRes += "HTTP/1.1 200 OK\r\n";
//     strHttpRes += "Cache-Control: private\r\n";
//     strHttpRes += "Content-Type: application/json; charset=utf-8\r\n";
//     strHttpRes += "Server: Microsoft-IIS/8.0\r\n";
//     strHttpRes += "X-AspNetMvc-Version: 5.2\r\n";
//     strHttpRes += "X-AspNet-Version: 4.0.30319\r\n";
//     strHttpRes += "X-Powered-By: ASP.NET\r\n";
//     strHttpRes += "Date: Sat, 24 Oct 2020 02:45:29 GMT\r\n";
//     strHttpRes += "Content-Length: 30\r\n";
//     strHttpRes += "\r\n";
//     strHttpRes += "{\"search\":\"SEARCH\",\"Msg\":\"OK\"}";
//     _conn->sendToLoop(strHttpRes); // 处理结束
// } else {
// _conn->sendToLoop(_httpReqParser.getHttpReq());
// }