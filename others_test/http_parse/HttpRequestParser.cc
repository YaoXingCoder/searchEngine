/*************************************************************************
    > File Name: HttpRequestParser.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月16日 星期三 21时11分14秒
    > Mail: JiaZiChunQiu@163.com
    > Title: HttpRequestParser http请求解析
    > Content:
 ************************************************************************/

#include "HttpRequestParser.h"
#include "http_parser.h"

#include <iostream>
#include <string.h>

HttpRequestParser::HttpRequestParser(const std::string &request) : _strHttpReq(request), _bParsed(false) {
    init(); // 初始化
}

void HttpRequestParser::init() {
    http_parser httpParser;            // 创建解析对象
    http_parser_settings httpSettings; // 创建解析器配置

    // 初使化解析器
    http_parser_init(&httpParser, HTTP_REQUEST); // 设置为请求方式
    // http_parser_settings_init(&httpSettings);

    // 当前实例的指针
    httpParser.data = this; // 必须需要

    // 初始化回调函数
    httpSettings.on_message_begin = onMessageBegin;
    httpSettings.on_headers_complete = onHeaderComplete;
    httpSettings.on_message_complete = onMessageComplete;
    httpSettings.on_url = onURL;
    httpSettings.on_status = onStatus;
    httpSettings.on_header_field = onHeaderField;
    httpSettings.on_header_value = onHeaderValue;
    httpSettings.on_body = onBody;

    // 执行解析过程
    size_t ret = http_parser_execute(&httpParser, &httpSettings, _strHttpReq.data(), _strHttpReq.size());
}

std::string HttpRequestParser::getHttpReq() const {
    return _strHttpReq;
}
bool HttpRequestParser::isParsed() const {
    return _bParsed;
}
std::string HttpRequestParser::getUrl() const {
    return _strUrl;
}
std::string HttpRequestParser::getStatus() const {
    return _strStatus;
}
std::string HttpRequestParser::getBody() const {
    return _strBody;
}
std::map<std::string, std::string> HttpRequestParser::getHeadField() const {
    return _mapHeadField;
}

// 静态回调函数，获取类实例并调用成员函数
int HttpRequestParser::onMessageBegin(http_parser *pParser) {
    return 0;
}
int HttpRequestParser::onHeaderComplete(http_parser *pParser) {
    HttpRequestParser *parser = static_cast<HttpRequestParser *>(pParser->data);
    parser->_bParsed = true;
    return 0;
}
int HttpRequestParser::onMessageComplete(http_parser *pParser) {
    return 0;
}
/* 将数据放入到成员变量中 */
int HttpRequestParser::onURL(http_parser *pParser, const char *at, size_t length) {
    HttpRequestParser *parser = static_cast<HttpRequestParser *>(pParser->data);
    parser->_strUrl.assign(at, length);
    return 0;
}
int HttpRequestParser::onStatus(http_parser *pParser, const char *at, size_t length) {
    HttpRequestParser *parser = static_cast<HttpRequestParser *>(pParser->data);
    parser->_strStatus.assign(at, length);
    return 0;
}
/* 内部自动循环执行 onHeaderField  onHeaderValue , 且总是 field和value依次顺序执行*/
int HttpRequestParser::onHeaderField(http_parser *pParser, const char *at, size_t length) {
    HttpRequestParser *parser = static_cast<HttpRequestParser *>(pParser->data);
    parser->_strFieldTmp.assign(at, length); // key
    return 0;
}
int HttpRequestParser::onHeaderValue(http_parser *pParser, const char *at, size_t length) {
    HttpRequestParser *parser = static_cast<HttpRequestParser *>(pParser->data);
    std::string strValue(at, length);                               // value
    parser->_mapHeadField.insert({parser->_strFieldTmp, strValue}); // key-value
    return 0;
}
int HttpRequestParser::onBody(http_parser *pParser, const char *at, size_t length) {
    HttpRequestParser *parser = static_cast<HttpRequestParser *>(pParser->data);
    if (length > 0 && at != nullptr) {
        parser->_strBody.assign(at, length);
    } else {
        parser->_strBody = "";
        printf("Invalid Body length or pointer.\n");
    }
    return 0;
}