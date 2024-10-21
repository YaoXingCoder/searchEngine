#ifndef __HTTP_REQUEST_PARSER_H__
#define __HTTP_REQUEST_PARSER_H__

class http_parser; // 前向声明, 不用再include

#include <map>
#include <string>

class HttpRequestParser {
  public:
    /* 需要解析的字符串 */
    HttpRequestParser(const std::string &request);
    HttpRequestParser(const char *request);

  public:
    /* get 函数 */
    bool isParsed() const;
    std::string getHttpReq() const;
    std::string getUrl() const;
    std::string getStatus() const;
    std::string getBody() const;
    std::map<std::string, std::string> getHeadField() const;
    std::string getQueryPath();
    std::string getQueryStr();

    void init(); // 初始化回调函数 和 对象的设置
  private:
    /* 静态成员函数 用于解析的函数 */
    static int onMessageBegin(http_parser *pParser);    // 数据开始接受
    static int onHeaderComplete(http_parser *pParser);  // 报文头接收完毕
    static int onMessageComplete(http_parser *pParser); // 数据接收完毕

    /* 解析后获取 */
    static int onURL(http_parser *pParser, const char *at, size_t length);         // url 路径
    static int onStatus(http_parser *pParser, const char *at, size_t length);      // 状态码
    static int onHeaderField(http_parser *pParser, const char *at, size_t length); // 请求头key
    static int onHeaderValue(http_parser *pParser, const char *at, size_t length); // 请求头value
    static int onBody(http_parser *pParser, const char *at, size_t length);        // 请求体

  private:
    std::string _strHttpReq;                          // 需要解析的字符串
    bool _bParsed;                                    // 如果有解析成功, 会在函数中置为 true
    std::map<std::string, std::string> _mapHeadField; // 存放头部字段的map类, map有序
    std::string _strUrl;                              // url
    std::string _strStatus;                           // 状态
    std::string _strBody;                             // 请求体
    std::string _strFieldTmp;                         // 报文头字段
};

#endif