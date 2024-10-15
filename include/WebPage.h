#ifndef __WEB_PAGE_H__
#define __WEB_PAGE_H__

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

class Configuration;
class SplitTool;

// xml 的标签内容
#define DOCID "docid"
#define TITLE "title"
#define URL "link"
#define CONTENT "content"

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
    WebPage(std::string & doc, std::shared_ptr<SplitTool> splitTool);

    // get 函数
    std::size_t getDocId() const; // 获取 _docId
    DocItem & getDocItem(); // 获取文档结构, 引用减少copy
    std::string & getDoc(); // 获取文档
    std::unordered_map<std::string, int> & getDict(); // 获取文档的词频统计
    std::unordered_map<std::string, int> & getWordsMap(); // 获取单词和词频
    std::vector<std::string> & getTopWords();

    // set 函数
    void setDocId(std::size_t newDocId);

    // 流程 函数
    void processDoc(const std::string & doc); // 对文档进行格式化处理

    // 获取TopTwenty词, 到类中_topWords容器
    void calcTop( std::unordered_set<std::string> & stopWordSet);
    void calcTopNoStop();

    // 获取topK词, 到给定的容器中
    void calcTopK(std::vector<std::string> & wordsVec
    , std::unordered_set<std::string> & stopWordSet
    , int k = TOPK_NUMBER); // 获取文档的topk词集
    void calcTopKNoStop(std::vector<std::string> & wordsVec
    , int k = TOPK_NUMBER); // 获取文档的topk词集, 无停用词

    // 友元
    friend bool operator==(const WebPage & lhs, const WebPage & rhs); // 判断两篇文章是否相等
    friend bool operator<(const WebPage & lhs, const WebPage & rhs); // 对文档 DocId 进行排序

private:
    void parseXML(const std::string & doc); // 分析XML格式字符串

    void parseContent(const std::string & content, std::unordered_set<std::string> & stopWordSet); // 单词和词频, 去除停用词
    void parseContentNoStop(const std::string & content); // 无停用词


public:
    ~WebPage();
    
    /* 测试 */
    void showTopWords();
    void showWordMap();
    void showDocItem();

private:
    std::string _doc; // 整个文档, 包含xml, 原汁原味的
    DocItem _docItem; // page 结构

    std::vector<std::string> _topWords; // 词频最高的前20个词
    std::unordered_map<std::string, int> _wordsMap; // 保存每篇文档的所有词语和词频, 包括停用词

    std::shared_ptr<SplitTool> _splitTool; // jieba 工具类
                            
    const static int TOPK_NUMBER = 20; // 词频最高的前20词
};

#endif
