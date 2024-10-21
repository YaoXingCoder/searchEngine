#ifndef __WEBPAGESEARCHER_H__
#define __WEBPAGESEARCHER_H__

#include "TcpConnection.h"
#include "WebPageQuery.h"

#include <memory>
#include <string>

class TcpConnection;

class WebPageSearcher {
  public:
    // using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
    using TcpConnectionPtr = TcpConnection::TcpConnectionPtr;

  public:
    WebPageSearcher(std::string &sought, const TcpConnectionPtr &conn);
    void doQuery(WebPageQuery *webPageQuery); // 执行查询
  private:
    string _sought;         // 查询关键词
    TcpConnectionPtr _conn; // TcpConnection 对象
};

#endif
