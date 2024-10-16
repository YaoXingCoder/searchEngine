#ifndef __WEBPAGEQUERY_H__
#define __WEBPAGEQUERY_H__

#include "WebPage.h"

class SplitTool;

#include <queue>
#include <set>
#include <unordered_map>

using std::pair;
using std::priority_queue;
using std::set;
using std::unordered_map;

struct Compare {
    bool operator()(const pair<int, double> &lhs, const pair<int, double> &rhs) {
        return lhs.second < rhs.second;
    }
};

class WebPageQuery {
  public:
    WebPageQuery(std::shared_ptr<SplitTool> splitTool); // 接受一个工具类
    std::string doQuery(const std::string &str); // 执行查询，返回结果

  private:
    void loadLibrary();                                                                        // 加载库文件
    void storeToQueue(set<pair<int, double>> &res);                                            // 存入优先队列
    std::vector<double> getQueryWordsWeightVector(const std::vector<std::string> &queryWords); // 计算查询词的权重值
    bool executeQuery(const std::vector<std::string> &queryWords,
                      std::vector<pair<int, std::vector<double>>> &resultVec); // 执行查询
    std::string createJson();
    std::string returnNoAnswer();

  private:
    std::shared_ptr<SplitTool> _splitTool;                                                 // 分词工具
    unordered_map<int, WebPage> _pageLib;                                                  // 网页库
    unordered_map<int, pair<int, int>> _offsetLib;                                         // 偏移库
    unordered_map<std::string, set<pair<int, double>>> _invertIndexTable;                  // 倒排索引表
    priority_queue<pair<int, double>, std::vector<pair<int, double>>, Compare> _resultQue; // 保存候选词的优先级队列
};

#endif
