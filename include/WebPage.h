#ifndef __WEB_PAGE_H__
#define __WEB_PAGE_H__

#include <string>
#include <vector>
#include <map>
#include <set>

class Configuration;
class SplitTool;

/* 处理后 文档格式 */
typedef struct {
    std::size_t id;
    std::string title;
    std::string url;
    std::string content;
}DocItem;

/* WebPage 类 */
class WebPage {
public:
    WebPage(std::string & doc, SplitTool * splitTool);

    std::size_t getDocId(); // 获取 _docId
    DocItem & getDocItem(); // 获取文档结构, 引用减少copy
    std::string getDoc(); // 获取文档
    std::map<std::string, int> & getDict(); // 获取文档的词频统计

    void processDoc(const std::string & doc); // 对文档进行格式化处理
    void calcTopK(std::vector<std::string> & wordsVec, std::set<std::string> & stopWordSet, int k = TOPK_NUMBER); // 获取文档的topk词集

    // 友元
    // friend bool operator==(const WebPage & lhs, const WebPage & rhs); // 判断两篇文章是否相等
    // friend bool operator<(const WebPage & lhs, const WebPage & rhs); // 对文档 DocId 进行排序

private:
    void parseXML(const std::string & doc);
    void parseContent(const std::string & content, std::set<std::string> & stopWordSet);

public:
    ~WebPage();
    
    /* 测试 */
    void showTopWords();
    void showWordMap();
    void showDocItem();
private:
    std::string _doc; // 整个文档, 包含xml, 原汁原味的
    /* std::size_t _docId; // 文档id */
    /* std::string _docTitle; */
    /* std::string _docUrl; */
    /* std::string _docContent; */
    DocItem _docItem;
    std::vector<std::string> _topWords; // 词频最高的前20个词
    std::map<std::string, int> _wordsMap; // 保存每篇文档的所有词语和词频, 不包括停用词

    SplitTool * _splitTool; // jieba 工具类
                            //
    const static int TOPK_NUMBER = 20; // 词频最高的前20词
};

#endif
