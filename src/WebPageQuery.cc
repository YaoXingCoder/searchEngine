/*************************************************************************
    > File Name: WebPageQuery.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月17日 星期四 02时25分03秒
    > Mail: JiaZiChunQiu@163.com
    > Title: 网页查询库
    > Content:
 ************************************************************************/

#include "Configuration.h"
#include "SplitTool.h"

#include "WebPageQuery.h"

#include "../nlohmann/json.hpp"

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

SimilarityDegree::SimilarityDegree(std::size_t docid, double cosineVal) : _docid(docid), _cosineVal(cosineVal) {
}

bool operator<(const SimilarityDegree &lhs, const SimilarityDegree &rhs) {
    if (lhs._cosineVal != rhs._cosineVal) {
        return lhs._cosineVal < rhs._cosineVal;
    } else {
        return lhs._docid < rhs._docid;
    }
}

WebPageQuery::WebPageQuery(std::shared_ptr<SplitTool> splitTool) : _splitTool(splitTool) {
    loadLibrary();
}

/* 加载库文件 */
void WebPageQuery::loadLibrary() {
    /*
     * 0.读取配置文件
     */
    Configuration::getInstance()->setFilePath(CONF_PATH);
    std::map<std::string, std::string> &fileNameMap = Configuration::getInstance()->getConfigMap();
    std::string pageLibPath = fileNameMap[PAGELIB_NAME];
    std::string offsetLibPath = fileNameMap[OFFSETLIB_NAME];
    std::string invertIndexPath = fileNameMap[INVERTINDEX_NAME];
    std::string stopWordsEnPath = fileNameMap[STOPWORDS_EN];
    std::string stopWordsZhPaht = fileNameMap[STOPWORDS_ZN];

    /* 1.网页库 */
    if (pageLibPath.empty()) {
        std::cerr << "WebPageQuery loadLibrary() pageLibPath is empty\n";
    }
    std::ifstream ifsPageLib(pageLibPath);
    if (!ifsPageLib) {
        std::cerr << "WebPageQuery loadLibrary() ifsPageLib is error\n";
    }

    /*
     * 2.偏移库
     * 网页库需要根据偏移库读取
     */
    if (offsetLibPath.empty()) {
        std::cerr << "WebPageQuery loadLibrary() offsetLibPath is empty\n";
    }
    std::ifstream ifsOffsetLib(offsetLibPath);
    if (!ifsOffsetLib) {
        std::cerr << "WebPageQuery loadLibrary() ifsOffsetLib is error\n";
    }
    // 2.1 按行读, 读取 docid, docOffset, docSize
    std::string offsetLine;
    while (std::getline(ifsOffsetLib, offsetLine)) {
        std::istringstream iss(offsetLine);
        std::size_t docid;
        std::size_t docOffset;
        std::size_t docSize;
        iss >> docid >> docOffset >> docSize;

        // 2.2 重置网页库文件内指针, 读取内容到 _pageLib
        ifsPageLib.seekg(docOffset);
        std::string doc(docSize, '\0');
        ifsPageLib.read(&doc[0], docSize);
        _pageLib.emplace_back(WebPage(doc, _splitTool));

        // 2.3 偏移库信息存入 _offsetLib
        _offsetLib[docid] = std::make_pair(docOffset, docSize);
    }

    /*
     * 3.倒排索引
     */
    // 3.1 打开文件
    if (invertIndexPath.empty()) {
        std::cerr << "WebPageQuery loadLibrary() invertIndexPath is empty\n";
    }
    std::ifstream ifsInvertIndex(invertIndexPath);
    if (!ifsInvertIndex) {
        std::cerr << "WebPageQuery loadLibrary() ifsInvertIndex is error\n";
    }
    // 3.2 按行读取, 存入 _invertIndexLib
    std::string invertIndexLine;
    while (std::getline(ifsInvertIndex, invertIndexLine)) {
        std::istringstream iss(invertIndexLine);
        std::string word;
        iss >> word;

        std::size_t docid;
        double weight;
        while (iss >> docid) {
            iss >> weight;
            _invertIndexLib[word].emplace_back(docid, weight); // 存入容器中
        }
    }

    /*
     * 4.停用词集合
     */
    // 4.1 英文
    if (stopWordsEnPath.empty()) {
        std::cerr << "WebPageQuery loadLibrary() invertIndexPath is empty\n";
    }
    std::ifstream ifsStopWordsEn(stopWordsEnPath);
    if (!ifsStopWordsEn) {
        std::cerr << "WebPageQuery loadLibrary() ifsStopWordsEn is error\n";
    }
    std::string word;
    while (std::getline(ifsStopWordsEn, word)) {
        _stopWords.emplace(word);
    }

    // 4.2 中文
    if (stopWordsZhPaht.empty()) {
        std::cerr << "WebPageQuery loadLibrary() invertIndexPath is empty\n";
    }
    std::ifstream ifsStopWordsZh(stopWordsEnPath);
    if (!ifsStopWordsZh) {
        std::cerr << "WebPageQuery loadLibrary() ifsStopWordsZh is error\n";
    }
    while (std::getline(ifsStopWordsZh, word)) {
        _stopWords.emplace(word);
    }

    // 4.关闭资源
    ifsPageLib.close();
    ifsOffsetLib.close();
    ifsInvertIndex.close();
    ifsStopWordsEn.close();
    ifsStopWordsZh.close();
}

/* 执行查询, 返回结果 */
void WebPageQuery::doQuery(const std::string &sentence, const TcpConnectionPtr &conn) {
    // 0.去除'\n'
    std::string str(sentence);
    for (int idx = 0; idx < str.size(); ++idx) {
        if (str[idx] == '\n') {
            str.erase(idx, 1);
        }
    }
    // 1.分词
    std::vector<std::string> words = _splitTool->cut(str);

    // 2.去除停用词, 并计算词频
    std::vector<std::string> wordsNoStop;
    for (std::string &word : words) {
        if (_stopWords.count(word) == 0) {
            wordsNoStop.emplace_back(word);
        }
    }

    // 3.根据分词查询倒排索引表, 遍历每个词有关的文章, 获取到有关文章的所有集合
    std::vector<std::set<std::size_t>> wordDocidSets; // 所有的单词的对应的所有集合
    for (std::string &word : wordsNoStop) {
        std::set<std::size_t> setDocidPerWord;
        for (const std::pair<std::size_t, double> &pair : _invertIndexLib[word]) {
            setDocidPerWord.emplace(pair.first);
        }
        if (setDocidPerWord.empty()) {
            continue;
        }
        wordDocidSets.emplace_back(setDocidPerWord);
    }
    int count = 1;
    for (std::set<std::size_t> &wordDocidSet : wordDocidSets) {
        std::cout << "setNum is " << count << " : ";
        for (std::size_t num : wordDocidSet) {
            std::cout << num << " ";
        }
        ++count;
        putchar('\n');
    }
    std::cout << "wordDocidSets size is " << wordDocidSets.size() << "\n";

    // 4.对这些集合进行取交集运算
    std::set<std::size_t> intersectSet = intersectSets(wordDocidSets);
    std::cout << "intersectSet size is " << intersectSet.size() << "\n";

    // 5.计算句子中各词的权重值
    std::unordered_map<std::string, double> wordsWeight = getQueryWordsWeightVector(wordsNoStop);
    std::cout << "wordsWeight size is " << wordsWeight.size() << "\n";

    // 4.执行查询
    if (executeQuery(intersectSet, wordsWeight)) {
        conn->sendToLoop(createJson());
    } else {
        conn->sendToLoop(returnNoAnswer());
    }
}

/* 遍历倒排索引, 查找到各次的文章和在其中的权重,  */
std::unordered_map<std::string, double> WebPageQuery::getQueryWordsWeightVector(std::vector<std::string> &queryWords) {
    // 1.计算 TF
    std::unordered_map<std::string, std::size_t> wordFreq; // 存储word和在句子中的频率
    for (std::string &word : queryWords) {
        ++wordFreq[word];
    }

    // 2.计算 DF : 所有文章
    std::unordered_map<std::string, double> pageWeights; // 临时容器, 存储句子中词的所有权重
    for (std::pair<const std::string, std::size_t> &pair : wordFreq) {
        std::size_t n = _pageLib.size();
        std::size_t tf = pair.second;
        std::size_t df = 0;
        for (WebPage &webPage : _pageLib) {
            if (webPage.getWordsMap().count(pair.first) != 0) {
                ++df;
            }
        }
        double idf = std::log2((n / (df + 1)) + 1); // 计算 IDF
        double weight = tf * idf;                   // 计算每个词的权重
        pageWeights.emplace(pair.first, weight);    // 放入权重集合
    } // 结束, 句子中所有词, 计算结束

    // 3.计算归一化中的基地
    double l2Norm = 0.0;
    for (std::pair<const std::string, double> &pair : pageWeights) {
        l2Norm += pow(pair.second, 2);
    }
    l2Norm = std::sqrt(l2Norm);

    // 4.归一化, 存入容器(单词-权重)
    std::unordered_map<std::string, double> wordWeightMap;
    for (std::pair<const std::string, double> &pair : pageWeights) {
        double weight = pair.second / l2Norm; // 取出单个权重, 归一化
        wordWeightMap[pair.first] = weight;
    }

    // 5.返回权重集合
    return wordWeightMap;
}

// 对集合队列进行取交集运算
std::set<std::size_t> WebPageQuery::intersectSets(const std::vector<std::set<std::size_t>> &sets) {
    if (sets.empty()) {
        return {}; // 返回空集
    }

    // 1.用第一个进行初始化
    std::set<std::size_t> intersection = sets[0];

    // 2.遍历后续的 sets
    for (std::size_t idx = 1; idx < sets.size(); ++idx) {
        std::set<std::size_t> currenSet = sets[idx];
        std::set<std::size_t> tempIntersection;

        // 遍历当前的集合
        for (std::size_t element : intersection) {
            // 如果原容器中和现容器中都有, 则为交集
            if (currenSet.find(element) != currenSet.end()) {
                tempIntersection.insert(element);
            }
        }

        // 更新交集
        intersection = tempIntersection;

        // 如果交集为空, 则提前返回
        if (intersection.empty()) {
            break;
        }
    }

    return intersection;
}

/*
 * 执行查询: vector<pair<size_t, vector<double>>> docid - 集合
 * intersect 文章的交集
 * wordsWeight 句子分词后的单词和权重
 */
bool WebPageQuery::executeQuery(const std::set<std::size_t> &intersect,
                                std::unordered_map<std::string, double> &wordsWeight) {

    std::vector<std::pair<std::string, double>> wordVec;
    for (std::pair<const std::string, double> &pair : wordsWeight) {
        wordVec.emplace_back(pair.first, pair.second);
    }

    std::vector<std::size_t> intersectVec;
    for (std::size_t docid : intersect) {
        intersectVec.emplace_back(docid);
    }

    // 1.构建二维数组
    std::size_t rowLength = intersectVec.size();
    std::size_t colLength = wordVec.size();
    double weightArray[rowLength + 1][colLength] = {0};

    // 2.基向量初始化
    for (int col = 0; col < colLength; ++col) {
        weightArray[0][col] = wordVec[col].second;
    }

    // 3.填充二维数组的权重
    for (int row = 1; row <= rowLength; ++row) {
        for (int col = 0; col < colLength; ++col) {
            for (const std::pair<std::size_t, double> &pair : _invertIndexLib[wordVec[col].first]) {
                if (intersectVec[row - 1] == pair.first) {
                    weightArray[row][col] = pair.second;
                }
            }
        }
    }

    // 4.使用余弦相似度算法, 遍历数组计算
    double normalWeight1 = 0.0;
    for (int col = 0; col < colLength; ++col) {
        normalWeight1 += std::pow(weightArray[0][col], 2);
    }
    normalWeight1 = std::sqrt(normalWeight1);                                             // 基向量的分母
    std::cout << "WebPageQuery::executeQuery normalWeight1 is " << normalWeight1 << "\n"; // 0

    for (int row = 1; row <= rowLength; ++row) {
        // 计算获取到分子分母
        double sumWeight = 0.0;
        double normalWeight2 = 0.0;

        // 计算分母的其他向量的分母
        for (int col = 0; col < colLength; ++col) {
            normalWeight2 += std::pow(weightArray[row][col], 2);
        }
        normalWeight2 = std::sqrt(normalWeight2);
        std::cout << "WebPageQuery::executeQuery normalWeight2 is " << normalWeight2 << "\n"; // 0

        // 计算分子
        for (int col = 0; col < colLength; ++col) {
            sumWeight += weightArray[0][col] * weightArray[row][col];
        }

        // 计算cos值, 并放入优先队列
        double cosineVal = sumWeight / (normalWeight1 * normalWeight2);
        std::cout << "WebPageQuery::executeQuery() cosineVal is " << cosineVal << "\n";
        _similarQueue.emplace(intersectVec[row - 1], cosineVal);
    }

    if (_similarQueue.empty()) {
        return false;
    }
    return true;
}

std::string WebPageQuery::createJson() {
    nlohmann::json returnJson;

    // 遍历优先级队列, 后去id制成json格式
    while (!_similarQueue.empty()) {
        SimilarityDegree similar = _similarQueue.top();
        std::string docid = std::to_string(similar._docid);
        returnJson[docid] = _pageLib[similar._docid - 1].getDocItem().content;
        _similarQueue.pop();
    }

    std::string returnStr = returnJson.dump();
    return returnStr;
}

std::string WebPageQuery::returnNoAnswer() {
    return "Non-conforming article";
}

/*
 * 测试容器
 */
void WebPageQuery::showPageLib() {
    if (_pageLib.empty()) {
        std::cerr << "WebPageQuery showPageLib() _pageLib is empty\n";
        return;
    }
    std::cout << "_pageLib size is " << _pageLib.size() << "\n";

    // for (WebPage &webPage : _pageLib) {
    //     std::cout << webPage.getDoc() << "\n";
    // }
}
void WebPageQuery::showOffsetLib() {
    if (_offsetLib.empty()) {
        std::cerr << "WebPageQuery showOffsetLib() _offsetLib is empty\n";
        return;
    }
    std::cout << "_offsetLib size is " << _offsetLib.size() << "\n";
    // for (std::pair<const std::size_t, std::pair<std::size_t, std::size_t>> &pair : _offsetLib) {
    //     std::cout << pair.first << " " << pair.second.first << " " << pair.second.second << "\n";
    // }
}
void WebPageQuery::showInvertIndexLib() {
    if (_invertIndexLib.empty()) {
        std::cerr << "WebPageQuery showInvertIndexLib() _invertIndexLib is empty\n";
        return;
    }
    std::cout << "_invertIndexLib size is " << _invertIndexLib.size() << "\n";
    for (std::pair<const std::string, std::vector<std::pair<std::size_t, double>>> &pair : _invertIndexLib) {
        std::cout << pair.first << " : ";
        for (const std::pair<std::size_t, double> &idW : pair.second) {
            std::cout << idW.first << " " << idW.second;
        }
        putchar('\n');
    }
}
void WebPageQuery::showStopSet() {
    if (_stopWords.empty()) {
        std::cerr << "WebPageQuery showStopSet() _stopWords is empty\n";
        return;
    }
    std::cout << "_stopWords size is " << _stopWords.size() << "\n";
}

// int count = 0;
// for (const std::set<std::size_t> &wordSet : wordDocidSets) {
//     ++count;
//     std::cout << count << " set is ";
//     for (std::size_t docid : wordSet) {
//         std::cout << docid << " ";
//     }
//     putchar('\n');
// }