#ifndef __WENPAGE_QUERY_H__
#define __WENPAGE_QUERY_H__

#include "TcpConnection.h"
#include "WebPage.h"

#include <memory>
#include <queue>
#include <set>
#include <unordered_map>
#include <utility>
#include <vector>

// 需要读取的文件名字
#define CONF_PATH "./conf/webPageQuery.conf" // 配置文件路径

#define PAGELIB_NAME "newripepage.dat"     // 去重网页库
#define OFFSETLIB_NAME "newoffsetlib.dat"  // 偏移库
#define INVERTINDEX_NAME "invertIndex.dat" // 倒排索引库

#define STOPWORDS_EN "stop_words_en.txt" // 英文停用词
#define STOPWORDS_ZN "stop_words_zh.txt" // 中文停用词

class SplitTool;

struct SimilarityDegree {
    SimilarityDegree(std::size_t docid, double cosineVal = 0.0);
    std::size_t _docid;
    double _cosineVal;
};

bool operator<(const SimilarityDegree &lhs, const SimilarityDegree &rhs);

class WebPageQuery {
  public:
    using TcpConnectionPtr = TcpConnection::TcpConnectionPtr;

  public:
    WebPageQuery(std::shared_ptr<SplitTool> splitTool);
    ~WebPageQuery() = default;

    void doQuery(const std::string &sentence, const TcpConnectionPtr &conn); // 执行查询, 返回结果

  public:
    void showPageLib();
    void showOffsetLib();
    void showInvertIndexLib();
    void showStopSet();

  private:
    void loadLibrary();                                                                  // 加载库文件
    std::set<std::size_t> intersectSets(const std::vector<std::set<std::size_t>> &sets); // 对所有文章进行交集计算
    std::unordered_map<std::string, double> getQueryWordsWeightVector(
        std::vector<std::string> &queryWords); // 计算分词后每个词的权重值
    bool executeQuery(const std::set<std::size_t> &intersect,
                      std::unordered_map<std::string, double> &wordsWeight); // 执行查询
    std::string createJson();                                                // 创建Json格式返回
    std::string returnNoAnswer();                                            // 无符合文章返回没有

  private:
    std::shared_ptr<SplitTool> _splitTool;      // 分词工具
    std::unordered_set<std::string> _stopWords; // 停用词集合

    std::vector<WebPage> _pageLib;                                                                // 去重网页库
    std::unordered_map<std::size_t, std::pair<std::size_t, std::size_t>> _offsetLib;              // 偏移库
    std::unordered_map<std::string, std::vector<std::pair<std::size_t, double>>> _invertIndexLib; // 倒排索引库

    std::priority_queue<SimilarityDegree, std::vector<SimilarityDegree>> _similarQueue;
};

#endif