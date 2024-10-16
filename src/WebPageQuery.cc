/*************************************************************************
    > File Name: WebPageQuery.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月17日 星期四 02时25分03秒
    > Mail: JiaZiChunQiu@163.com
    > Title:
    > Content:
 ************************************************************************/

#include "WebPageQuery.h"
// #include "../nlohmann/json.hpp"
#include "/home/coder/searchEngine/nlohmann/json.hpp"
#include "RssReader.h"
#include "SplitTool.h"

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::istringstream;
using std::map;
using std::ostringstream;

const char *const PAGELIB = "/home/coder/searchEngine/data/newoffsetlib_pre.dat";     // 去重网页库
const char *const OFFSETLIB = "/home/coder/searchEngine/data/newoffsetlib_pre.dat";   // 偏移库
const char *const INVERTINDEXTABLE = "/home/coder/searchEngine/data/invertIndex.dat"; // 倒排索引库

WebPageQuery::WebPageQuery(std::shared_ptr<SplitTool> splitTool) : _splitTool(splitTool) {
    loadLibrary();
}

// 执行查询，返回结果
std::string WebPageQuery::doQuery(const std::string &str) {
    std::string result;
    if (_invertIndexTable.count(str) != 0) {
        // 1.表示str是一个词，在倒排索引库中可以查询到
        set<pair<int, double>> res = _invertIndexTable[str];
        // 2.把docid和权重放到优先队列
        storeToQueue(res);
        result = createJson();
    } else {
        // 倒排索引库中没有查询到
        // 1.分词&去停用词
        std::vector<std::string> query_words = _splitTool->cut(str); // 获得一个个查询词
        /* // 2.计算查询词的基准向量 */
        /* vector<double> Base = getQueryWordsWeightVector(query_words); */
        // 得到的基准向量与查询词顺序一致
        // 3.取每个词的倒排索引集，求交集
        std::vector<std::pair<int, std::vector<double>>> intersection;
        bool ret = executeQuery(query_words, intersection);
        if (ret) // true取到交集，存入优先队列
        {
            result = createJson();
        } else // false没有取到交集，查询失败
        {
            result = returnNoAnswer();
        }
    }

    return result;
}

void WebPageQuery::loadLibrary() // 加载库文件
{
    // 偏移库
    ifstream off(OFFSETLIB);
    if (!off.is_open()) {
        cerr << OFFSETLIB << "open failed\n";
        exit(-1);
    }
    std::string line;
    while (std::getline(off, line)) {
        int docid;
        int startPos;
        int endPos;
        istringstream iss(line);
        iss >> docid >> startPos >> endPos;

        _offsetLib[docid] = {startPos, endPos};
    }
    off.close();

    // 倒排索引库
    ifstream dex(INVERTINDEXTABLE);
    if (!dex.is_open()) {
        cerr << "Failed to open file: " << INVERTINDEXTABLE << "\n";
        return;
    }

    // 解析 JSON 文件
    nlohmann::json js;
    // dex >> js;

    // 遍历 JSON 并填充到 _invertIndexTable
    std::string lineInvertIndex;
    while (std::getline(dex, lineInvertIndex)) {
        std::istringstream iss(lineInvertIndex);
        std::string word;
        iss >> word;
        double weight;
        int docId;
        std::set<pair<int, double>> entrySet;
        while (iss >> docId) {
            iss >> weight;
            entrySet.insert({docId, weight});
        }
        _invertIndexTable[word] = entrySet;
    }

    // for (auto &[key, value] : js.items()) {
    //     set<pair<int, double>> entrySet;

    //     // 遍历 key 对应的数组，填充到 set 中
    //     for (auto &pairArr : value) {
    //         if (pairArr.is_array() && pairArr.size() == 2) {
    //             int docId = pairArr[0].get<int>();
    //             double weight = pairArr[1].get<double>();
    //             entrySet.insert({docId, weight});
    //         }
    //     }
    //     // 将 set 填入到 _invertIndexTable 中
    //     _invertIndexTable[key] = entrySet;
    // }

    dex.close(); // 关闭资源

    // 测试一把
    for (auto a : _offsetLib) {
        cout << a.first << " " << a.second.first << " " << a.second.second << "\n";
    }

    for (auto b : _invertIndexTable) {
        cout << b.first << ": \n";
        for (auto c : b.second) {
            cout << "\t";
            cout << "[" << c.first << " : " << c.second << "] ";
            cout << "\n";
        }
        cout << "\n";
    }
}

void WebPageQuery::storeToQueue(set<pair<int, double>> &res) {
    for (auto &ele : res) {
        _resultQue.push(ele);
    }
}

std::vector<double> WebPageQuery::getQueryWordsWeightVector(
    const std::vector<std::string> &queryWords) // 计算查询词的权重值
{
    // 1.计算TF:词频
    std::map<std::string, int> TF;
    for (auto &word : queryWords) {
        ++TF[word];
    }
    // 2.计算IDF： TF中词的DF是1
    std::map<std::string, double> IDF;
    for (auto &word : TF) {
        IDF[word.first] = log(1) / log(2);
    }
    // 3.求权重
    // 计录权重的平方和
    double w2_score = 0;
    std::map<std::string, double> W;
    for (auto &word : TF) {
        W[word.first] = word.second * IDF[word.first];
        w2_score += (W[word.first]) * (W[word.first]);
    }

    // 4.归一化
    std::map<std::string, double> W1;
    for (auto &word : W) {
        W1[word.first] = word.second / sqrt(w2_score);
    }

    std::vector<double> res;
    // 为求顺序与word顺序一致
    for (auto &word : queryWords) {
        res.push_back(W1[word]);
    }

    return res;
}

bool WebPageQuery::executeQuery(const std::vector<std::string> &queryWords,
                                std::vector<std::pair<int, std::vector<double>>> &resultVec) // 执行查询
{
    if (queryWords.empty()) {
        return false;
    }

    set<int> resultInter;
    // 1.首先查询每个word的docid集合
    // // 遍历查询词库
    for (size_t i = 0; i < queryWords.size(); ++i) {
        set<int> wordTmp;
        // 查找该词的docid集合
        auto it = _invertIndexTable.find(queryWords[i]);
        if (it != _invertIndexTable.end()) // 找到了
        {
            for (auto ele : it->second) {
                // 获得该单词的docid
                wordTmp.insert(ele.first);
            }
        } else // 没有找到
        {
            return false;
        }

        // 求交集
        // 第一次之间插入
        if (i == 0) {
            resultInter = wordTmp;
        } else {
            set<int> resultTmp;

            // 2.求交集
            for (auto ele : resultInter) {
                if (wordTmp.find(ele) != wordTmp.end()) // 找到了,说明结果中有
                {
                    resultTmp.insert(ele);
                }
            }

            resultInter.swap(resultTmp);
            // 已空
            if (resultInter.empty()) {
                return false;
            }
        }
    }

    // 3.获得结果

    // 遍历交集set. docid
    for (auto res : resultInter) // 获得交集中的一个docid
    {
        // 按照输入查询词的顺序进行构建结果
        std::vector<double> wTmp; // 临时权重
        for (size_t i = 0; i < queryWords.size(); ++i) {
            std::string word = queryWords[i];
            set<pair<int, double>> indexSet = _invertIndexTable[word];

            for (auto ele : indexSet) // 查找该词中的docid的权重
            {
                if (ele.first == res) {
                    wTmp.push_back(ele.second);
                    break;
                }
            }
        }
        resultVec.push_back({res, wTmp});
    }

    // 4.余弦相似算法计算 得到每一个docid 的cos*
    // 获得基准向量
    std::vector<double> Base = getQueryWordsWeightVector(queryWords);
    // 保存一定会用到的基准向量平方根
    double Base_square_root = 0;
    double Base_square = 0;
    for (size_t i = 0; i < Base.size(); ++i) {
        Base_square += Base[i] * Base[i];
    }
    Base_square_root = sqrt(Base_square);

    // 余弦值
    for (size_t i = 0; i < resultVec.size(); ++i) {
        double cos = 0;
        double doc_square_root = 0;
        double sum = 0;
        double square_sum = 0;
        std::vector<double> tmp = resultVec[i].second;
        for (size_t i = 0; i < tmp.size(); ++i) {
            square_sum += tmp[i] * tmp[i];
            sum += Base[i] * tmp[i];
        }
        doc_square_root = sqrt(square_sum);
        cos = sum / (Base_square_root * doc_square_root);

        // 存入优先队列
        _resultQue.push({resultVec[i].first, cos});
    }

    return true;
}

/* 创建Json */
std::string WebPageQuery::createJson() {
    // 取10篇doc
    // 如果不满10篇，取整个优先队列
    ifstream ifs(PAGELIB);
    if (!ifs.is_open()) {
        cerr << PAGELIB << " open failed\n";
    }
    ostringstream oss;

    int loop = 0;
    if (_resultQue.size() > 10) {
        loop = 10;
    } else {
        loop = _resultQue.size();
    }

    for (int i = 0; i < loop; ++i) {
        pair<int, double> tmp = _resultQue.top();
        int startPos = _offsetLib[tmp.first].first;
        int endPos = _offsetLib[tmp.first].second;

        ifs.seekg(startPos, std::ios::beg);
        int numBytes = endPos - startPos;
        char *buffer = new char[numBytes + 1];
        // 读取
        ifs.read(buffer, numBytes);
        buffer[numBytes] = '\0';

        oss << buffer;
        delete[] buffer;
        _resultQue.pop();
    }
    ifs.close();

    FileProcessor file;
    std::string result = oss.str();
    result = file.xmlToJson(result);

    return result;
}

std::string WebPageQuery::returnNoAnswer() {
    std::string result = "未查询到相关网页\n";
    FileProcessor file;
    result = file.xmlToJson(result);

    return result;
}
