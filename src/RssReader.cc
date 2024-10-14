/*************************************************************************
    > File Name: RssReader.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月13日 星期日 14时37分31秒
    > Mail: JiaZiChunQiu@163.com
    > Title: xml 文件解析类
    > Content:
    >   构建解析后的类型结构
    >   通过路径读取xml文件
    >   读取文件, 并存储在容器中
    >   遍历容器生成存储文件
 ************************************************************************/

#include "RssReader.h"

#include <tinyxml2.h>

#include <iostream>
#include <fstream>
#include <regex>

/* 
 * 文件结构类
 */
RssItem::RssItem() {}

RssItem::RssItem(const std::string & str_title, const std::string & str_link, const std::string & str_description)
: title(str_title)
, link(str_link)
, description(str_description)
{}

RssItem::RssItem(const RssItem & other)
: title(std::move(other.title))
, link(std::move(other.link))
, description(std::move(other.description))
{}

RssItem::~RssItem() {}

/* 结构体大小 */
std::size_t RssItem::size() {
    std::size_t rssItemSize = title.size() + title.size() + description.size();
    return rssItemSize;
}

/* 将整个将 */

/*
 * Rss构造解析
 */
RssReader::RssReader(const int capacity) 
: _rssVec()
{
    _rssVec.reserve(capacity); // 预留空间
}

RssReader::~RssReader() {} // 析构

/* 处理文本 删除 '\t' '\n' ' ' */
void RssReader::dealText(std::string & text) {
    // 1.判空
    if ( text.empty() ) { 
        // std::cerr << "PageLib dealText() text is empty()\n";
        return;
    }

    // 2.处理
    std::string::iterator it = text.begin();
    while ( it < text.end() ) {
        if ( *it == '\n' || *it == '\t' || *it == ' ' || *it == '\r') {
            it = text.erase(it);
        } else {
            ++it;
        }
    }
}

void RssReader::parseRss(const std::string & xmlFilePath) {
    // 1.关联xml文件
    tinyxml2::XMLDocument doc; // 创建结构对象
    tinyxml2::XMLError error = doc.LoadFile(xmlFilePath.c_str()); // 加载xml文件
    if ( error != tinyxml2::XMLError::XML_SUCCESS ) {
        std::cerr << "RssReader parseRss LoadFile error\n";
        std::cerr << "ERROR : " << doc.ErrorStr() << "\n";
        return;
    }

    // 2.正则表达式 : 处理html的标签
    // std::string regexHtml("<[^>]+>");
    std::regex html_regex("<[^>]+>");

    // 3.定位节点 item : item 下有 title link description/content
    tinyxml2::XMLElement * item = doc.RootElement()->FirstChildElement("channel")->FirstChildElement("item");

    // 4.循环遍历所有的item节点
    while (item) {
        RssItem rssItem;

        // 4.1 tilte 节点
        tinyxml2::XMLElement * element = item->FirstChildElement("title");
        rssItem.title = element->GetText();

        // 4.2 link 节点
        element = item->FirstChildElement("link");
        rssItem.link = element->GetText();

        // 4.3 description 节点, 需要处理 html 格式, 同时判断子节点的名字
        if ( (element = item->FirstChildElement("description")) != nullptr ) {
            std::string description = std::regex_replace(element->GetText(), html_regex, "");
            dealText(description);
            rssItem.description = description;
        } else if ( (element = item->FirstChildElement("content")) != nullptr ) {
            std::string description = std::regex_replace(element->GetText(), html_regex, "");
            dealText(description);
            rssItem.description = description;
        } else {
            rssItem.description = "content is empty";
        }
        
        // 4.4 存入容器
        _rssVec.push_back(rssItem);

        // 4.5 寻找兄弟标签
        item = item->NextSiblingElement("item");
    }
}

/* 存入给定的容器 */
void RssReader::parseRss(const std::string & xmlFilePath, std::vector<RssItem> & rssVec) {
    // 1.关联xml文件
    tinyxml2::XMLDocument doc; // 创建结构对象
    tinyxml2::XMLError error = doc.LoadFile(xmlFilePath.c_str()); // 加载xml文件
    if ( error != tinyxml2::XMLError::XML_SUCCESS ) {
        std::cerr << "RssReader parseRss LoadFile error\n";
        std::cerr << "ERROR : " << doc.ErrorStr() << "\n";
        return;
    }

    // 2.正则表达式 : 处理html的标签
    std::regex html_regex("<[^>]+>");

    // 3.定位节点 item : item 下有 title link description/content
    tinyxml2::XMLElement * item = doc.RootElement()->FirstChildElement("channel")->FirstChildElement("item");

    // 4.循环遍历所有的item节点
    while (item) {
        // 4.0 临时结构体, 临时存放内容
        RssItem rssItem;

        // 4.1 tilte 节点
        tinyxml2::XMLElement * element = item->FirstChildElement("title");
        rssItem.title = element->GetText();

        // 4.2 link 节点
        element = item->FirstChildElement("link");
        rssItem.link = element->GetText();

        // 4.3 description 节点, 需要处理 html 格式, 同时判断子节点的名字
        if ( (element = item->FirstChildElement("description")) != nullptr ) {
            std::string description = std::regex_replace(element->GetText(), html_regex, "");
            dealText(description);
            rssItem.description = description;
        } else if ( (element = item->FirstChildElement("content")) != nullptr ) {
            std::string description = std::regex_replace(element->GetText(), html_regex, "");
            dealText(description);
            rssItem.description = description;
            rssItem.description = "content is empty";
        }
        
        // 4.4 存入容器
        rssVec.push_back(rssItem);

        // 4.5 寻找兄弟标签
        item = item->NextSiblingElement("item");
    }
}

/* 获取容器 */
std::vector<RssItem> & RssReader::getRssVec() {
    return _rssVec;
}

/* 测试 : 遍历输出容器内容 */
void RssReader::showRssVec() {
    for ( RssItem & rssItem : _rssVec ) {
        std::cout << "<doc>" << "\r\n";
        std::cout << "\t" << "<docid>" << 1 << "</docid>" << "\r\n";
        std::cout << "\t" << "<title>" << rssItem.title << "</title>" << "\r\n";
        std::cout << "\t" << "<link>" << rssItem.link << "</link>" << "\r\n";
        std::cout << "\t" << "<content>" << rssItem.description << "</content>" << "\r\n";
        std::cout << "</doc>" << "\r\n";
    }
}

void RssReader::dump(const std::string & savePath) {
    // 1.打开文件
    std::ofstream ofs(savePath);
    if( !ofs ) {
        std::cerr << "RssReader dump() error\n";
        return;
    }

    // 2.遍历输出到文件
    size_t docid = 1;
    for ( RssItem & rssItem : _rssVec ) {
        ofs << "<doc>" << docid << "\r\n";
        ofs << "\t" << "<docid>" << docid << "</docid>" << "\r\n";
        ofs << "\t" << "<title>" << rssItem.title << "</title>" << "\r\n";
        ofs << "\t" << "<link>" << rssItem.link << "</link>" << "\r\n";
        ofs << "\t" << "<content>" << rssItem.description << "</content>" << "\r\n";
        ofs << "</doc>" << "\r\n";
        ++docid;
    }

    // 关闭资源
    ofs.close();
}