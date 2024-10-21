/*************************************************************************
    > File Name: KeyRecommander.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月18日 星期五 14时15分05秒
    > Mail: JiaZiChunQiu@163.com
    > Title: 关键词推荐类定义
    > Content:
    >   CandidateResult 结果类
    >   KeyRecommander 查询, 响应类
    >   json 序列化
 ************************************************************************/

#include "KeyRecommander.h"
#include "../nlohmann/json.hpp"
#include "Dictionary.h"

CandidateResult::CandidateResult(const std::string &word, std::size_t freq, std::size_t distance)
    : _word(word), _freq(freq), _distance(distance) {
}

std::string CandidateResult::getWord() const {
    return _word;
}

std::size_t CandidateResult::getFreq() const {
    return _freq;
}

std::size_t CandidateResult::getDistance() const {
    return _distance;
}

/* ---------------------------- bool operator< --------------------------- */
bool operator<(const CandidateResult &lhs, const CandidateResult &rhs) {
    if (lhs.getDistance() != rhs.getDistance()) {
        return lhs.getDistance() > rhs.getDistance();
    } else if (lhs.getFreq() != rhs.getFreq()) {
        return lhs.getFreq() > lhs.getFreq();
    } else {
        return lhs.getWord() > rhs.getWord();
    }
}

/* ---------------------------- KeyRecommander --------------------------- */

KeyRecommander::KeyRecommander(const std::string &queryWord, const TcpConnectionPtr &conn)
    : _queryWord(queryWord), _conn(conn), _resultQue() {
}

/* 执行查询 */
void KeyRecommander::executeQuery() {
    // 1.获取候选词和词频集合
    std::vector<std::pair<std::string, std::size_t>> candidateVec =
        Dictionary::getInstance()->searchCandiFreq(_queryWord);

    // 2.计算
    statistic(candidateVec);
}

/* 区分中英文 */
static size_t nBytesCode(const char ch) {
    if (ch & (1 << 7)) {
        int nBytes = 1;
        for (int idx = 0; idx != 6; ++idx) {
            if (ch & (1 << (6 - idx))) {
                ++nBytes;
            } else
                break;
        }
        return nBytes;
    }
    return 1;
}

/* 计算字符串长度,  */
static std::size_t length(const std::string &str) {
    std::size_t ilen = 0;
    for (std::size_t idx = 0; idx != str.size();) {
        int nBytes = nBytesCode(str[idx]);
        idx += nBytes;
        ++ilen;
    }
    return ilen;
}

/* 计算三个数的最小数 */
static std::size_t triple_min(const int &a, const int &b, const int &c) {
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

/* 计算最小编辑距离 */
std::size_t KeyRecommander::minEditDist(const std::string &other) {
    // 1.计算字符串长度, 因为中英文所长字节同步, 不能str.size()计算
    std::size_t querySize = length(_queryWord);
    std::size_t otherSize = length(other);

    // 2.创建二维数组, 并初始化
    std::size_t editDist[querySize + 1][otherSize + 1]; // +1 表示从空串到完整字符串的编辑距离
    for (int idx = 0; idx <= querySize; ++idx) {
        editDist[idx][0] = idx;
    }
    for (int idx = 0; idx <= otherSize; ++idx) {
        editDist[0][idx] = idx;
    }

    // 3.最小编辑距离:动态规划
    std::string subQueryStr, subOtherStr;
    for (std::size_t dist_row = 1, query_i = 0; dist_row <= querySize; ++dist_row) {
        // 1.计算行的当前字符所占长字节数
        std::size_t RowbyteChNum = nBytesCode(_queryWord[query_i]);
        subQueryStr = _queryWord.substr(query_i, RowbyteChNum); // 获取当前字符
        query_i += RowbyteChNum;                                // for循环后有自动++, 所以这里需减一

        // 2.遍历列, 计算行变为列需要编辑的距离
        for (std::size_t dist_col = 1, other_i = 0; dist_col <= otherSize; ++dist_col) {
            // 3.获取列的当前字符所占字节数
            std::size_t ColByteChNum = nBytesCode(other[other_i]);
            subOtherStr = other.substr(other_i, ColByteChNum);
            other_i += ColByteChNum;

            /*
             * 4.判断 行的字符 与 列的字符 是否相等
             * 相等 : 则等于左上角的值
             * 不相等 : 上 左 坐上, 取最小值 +1
             */
            if (subQueryStr == subOtherStr) {
                editDist[dist_row][dist_col] = editDist[dist_row - 1][dist_col - 1];
            } else {
                editDist[dist_row][dist_col] =
                    triple_min(editDist[dist_row][dist_col - 1] + 1, editDist[dist_row - 1][dist_col] + 1,
                               editDist[dist_row - 1][dist_col - 1] + 1);
            }
        }
    }

    // 5.二位数组的最后即为所求值
    return editDist[querySize][otherSize];
}

/* 计算 */
void KeyRecommander::statistic(std::vector<std::pair<std::string, std::size_t>> &candidateVec) {
    // 1.遍历候选词集
    for (std::pair<std::string, std::size_t> &word_freq : candidateVec) {
        // 2.计算编辑距离
        std::size_t eidtDistance = minEditDist(word_freq.first);

        // 3.存入容器
        _resultQue.push(CandidateResult(word_freq.first, word_freq.second, eidtDistance));
    }
}

/*
 * 响应客户端请求
 * 发送给客户端的数据要采用 JSON 数据格式封包
 */
void KeyRecommander::response() {
    // 1.创建json对象
    nlohmann::json resultQueJson;

    // 2.遍历结果集
    std::size_t topStr = 0;
    while (!_resultQue.empty()) {
        std::string candidate = _resultQue.top().getWord();
        std::size_t eidtDistance = _resultQue.top().getDistance();
        nlohmann::json wordDistJson;
        wordDistJson[candidate] = eidtDistance;

        resultQueJson.push_back(wordDistJson);
        _resultQue.pop();

        if (topStr == TOP_MAX) {
            break;
        }
        ++topStr;
    }

    // 3.序列化
    std::string msg = resultQueJson.dump();

    // 4.添加给主线程队列
    _conn->sendToLoop(msg);
}
