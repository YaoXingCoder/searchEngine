/*************************************************************************
    > File Name: WebPage.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月13日 星期日 22时41分17秒
    > Mail: JiaZiChunQiu@163.com
    > Title: WebPage 网页类
    > Content:
 ************************************************************************/

#include "WebPage.h"
#include "SplitTool.h"
#include "Configuration.h"

#include <tinyxml2.h>
#include <iostream>
#include <utility>
#include <algorithm>

/* 构造 */ 
WebPage::WebPage(std::string & doc, std::shared_ptr<SplitTool> splitTool)
: _doc(doc)
, _docItem()
, _topWords()
, _wordsMap()
, _splitTool(splitTool)
{
    // _topWords.reserve(TOPK_NUMBER); // 预留空间

    processDoc(doc);
}

/* 析构 */
WebPage::~WebPage() {}

/* 成员变量获取 */
std::size_t WebPage::getDocId() {
    return _docItem.id;
}
DocItem & WebPage::getDocItem() {
    return _docItem;
}
std::string WebPage::getDoc() {
    return _doc;
}
std::map<std::string, int> & WebPage::getDict() {
    return _wordsMap;
}

/* 文档处理 */
void WebPage::processDoc(const std::string & doc) {
    // 1.处理 xml
    parseXML(doc);
}

/* 处理XML格式的字符串 */
void WebPage::parseXML(const std::string & doc) {
    // 1.创建对象
    tinyxml2::XMLDocument parseStr;
    tinyxml2::XMLError error =  parseStr.Parse(doc.c_str(), doc.size());
    if ( error != tinyxml2::XMLError::XML_SUCCESS ) {
        std::cerr << "RssReader parseRss LoadFile error\n";
        std::cerr << "ERROR : " << parseStr.ErrorStr() << "\n";
        return;
    }

    // 2.定位节点 item : item 下有 title link content
    tinyxml2::XMLElement * item = parseStr.RootElement();

    // 3.根据节点提取出内容
    // 3.1 docid
    std::size_t docid = std::stoi(item->FirstChildElement(DOCID)->GetText());
    _docItem.id = docid;
    // 3.2 tilte
    std::string title = item->FirstChildElement(TITLE)->GetText();
    if ( !title.empty() ) { _docItem.title = title; }
    else { _docItem.title = "title is empty"; }
    // 3.3 link
    std::string url = item->FirstChildElement(URL)->GetText();
    if ( !url.empty() ) { _docItem.url = url; }
    else { _docItem.url = "url is empty"; }
    // 3.4 content
    std::string content = item->FirstChildElement(CONTENT)->GetText();
    if ( !content.empty() ) { _docItem.content = content; }
    else { _docItem.content = "content is empty"; }
}

/* 获取文档的topk词集 */
void WebPage::calcTopK(std::vector<std::string> & wordsVec, std::set<std::string> & stopWordSet, int k) {
    // 1.处理 content 内容 : 分词->排序->放入容器
    parseContent(_docItem.content, stopWordSet);

    // 2.根据参数获取k个top词, 并放入给定的wordsVec中
    for ( int i = 0; i < k && i < _topWords.size(); ++i ) {
        wordsVec.push_back(_topWords[i]);
    }
}

/* 处理 content 内容, 不包括停用词 */
void WebPage::parseContent(const std::string & content, std::set<std::string> & stopWordSet) {
    // 1.临时容器, 保存处理后的词
    std::vector<std::string> words;
    words = _splitTool->cut(content);

    // 2.获取所有词和词频
    for ( std::string & word : words ) {
        if ( stopWordSet.count(word) == 0 ) { ++_wordsMap[word]; }
    }

    // 3.临时vector<pair<string,int>>存储map, 同时对vector进行降序排序
    std::vector<std::pair<std::string, int>> wordsVec(_wordsMap.begin(), _wordsMap.end());
    std::sort(wordsVec.begin(), wordsVec.end(), [](const std::pair<std::string, int> & lhs, const std::pair<std::string, int> & rhs){
                return lhs.second > rhs.second;
              });

    // 4.获取临时vector中文前20个, 并放入容器
    for ( int i = 0; i < TOPK_NUMBER && i < wordsVec.size(); ++i ) {
        _topWords.push_back(wordsVec[i].first);
    }
}

/* 测试 */
void WebPage::showDocItem() {
    std::cout << "docId is " << _docItem.id << "\n";
    std::cout << "docTitle is " << _docItem.title << "\n";
    std::cout << "docUrl is " << _docItem.url << "\n";
    std::cout << "docContent is " << _docItem.content << "\n";
}

void WebPage::showWordMap() {
    if ( _wordsMap.empty() ) { 
        std::cerr << "WebPage showWordMap() _wordsMap is empty\n";
        return;
    }

    for( std::pair<const std::string, int> & pair : _wordsMap ) {
        std::cout << "word is " << pair.first << ", frequency is " << std::to_string(pair.second) << "\n";
    }
}

void WebPage::showTopWords() {
    if ( _topWords.empty() ) {
        std::cerr << "WebPage showTopWords() _topWords is empty\n";
        return;
    }

    for ( std::string & word : _topWords ) {
        std::cout << "Top 20 word is " << word << "\n";
    }
}


/* 丢弃代码, Expired  */
/* OLD : 处理XML格式的字符串 */
// void WebPage::parseXML(const std::string & doc) {
//     // 1.创建对象
//     tinyxml2::XMLDocument parseStr;
//     tinyxml2::XMLError error =  parseStr.Parse(doc.c_str(), doc.size());
//     if ( error != tinyxml2::XMLError::XML_SUCCESS ) {
//         std::cerr << "RssReader parseRss LoadFile error\n";
//         std::cerr << "ERROR : " << parseStr.ErrorStr() << "\n";
//         return;
//     }

//     // 2.定位节点 item : item 下有 title link content
//     tinyxml2::XMLElement * item = parseStr.RootElement();

//     // 3.根据节点提取出内容
//     // 3.1.docid
//     /* tinyxml2::XMLElement * element = item->FirstChildElement("docid"); */
//     _docItem.id = std::stoi(item->FirstChildElement("docid")->GetText());
//     // 3.2.tilte
//     /* element = item->FirstChildElement("title"); */
//     /* _docItem.title = element->GetText(); */
//    _docItem.title = item->FirstChildElement("title")->GetText();
//     // 3.3.link
//     /* element = item->FirstChildElement("link"); */
//     /* _docItem.url link = element->GetText(); */
//     _docItem.url = item->FirstChildElement("link")->GetText();
//     // 3.4.content
//     /* element = item->FirstChildElement("content"); */
//     /* _docItem.content = element->GetText(); */
//     _docItem.content = item->FirstChildElement("content")->GetText();
// }