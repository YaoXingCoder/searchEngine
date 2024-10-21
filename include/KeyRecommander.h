#ifndef __KEY_RECOMMANDER_H__
#define __KEY_RECOMMANDER_H__

#include "TcpConnection.h"

#include <queue>
#include <string>
#include <utility>
#include <vector>

#define TOP_MAX 10

/* 候选词类 */
class CandidateResult {
  public:
    CandidateResult(const std::string &word, std::size_t freq, std::size_t distance);

    /* get 函数 */
    std::string getWord() const;
    std::size_t getFreq() const;
    std::size_t getDistance() const;

  private:
    std::string _word;     // 单词
    std::size_t _freq;     // 词频
    std::size_t _distance; // 目标词与该词的编辑距离
};

/* 重构比较运算符 */
bool operator<(const CandidateResult &lhs, const CandidateResult &rhs);

/* 关键词推荐类 */
class KeyRecommander {
  public:
    using TcpConnectionPtr = TcpConnection::TcpConnectionPtr; // 别名

  public:
    KeyRecommander(const std::string &queryWord, const TcpConnectionPtr &conn);
    void executeQuery(); // 执行查询
    void response();     // 响应客户端请求

  private:
    void statistic(std::vector<std::pair<std::string, std::size_t>> &candidateVec);
    std::size_t minEditDist(const std::string &rhs); // 计算最小编辑距离

  private:
    std::string _queryWord;                                                        // 等待查询单词
    TcpConnectionPtr _conn;                                                        // 与客户端连接的套接字
    std::priority_queue<CandidateResult, std::vector<CandidateResult>> _resultQue; // 推荐词集合
};

#endif
