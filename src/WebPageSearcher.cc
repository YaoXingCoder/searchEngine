/*************************************************************************
    > File Name: WebPageSearcher.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月17日 星期四 03时48分39秒
    > Mail: JiaZiChunQiu@163.com
    > Title: 网页查询
    > Content:
 ************************************************************************/

#include "WebPageSearcher.h"

#include <iostream>
#include <string>

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::string;

WebPageSearcher::WebPageSearcher(string &sought, const TcpConnectionPtr &conn) : _sought(sought), _conn(conn) {
}

// 执行查询
void WebPageSearcher::doQuery(WebPageQuery *webPageQuery) {
    // 使用WebPageQuery类的功能查询
    string result = webPageQuery->doQuery(_sought);
    _conn->sendToLoop(result);
}
