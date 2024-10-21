/*************************************************************************
    > File Name: Dictionary.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月17日 星期四 00时55分32秒
    > Mail: JiaZiChunQiu@163.com
    > Title: 预热使用单例类
    > Content:
 ************************************************************************/
#include "Dictionary.h"

#include <fstream>
#include <iostream>
#include <sstream>

const size_t CHUNK_SIZE = 4096 * 1024;

/* 静态成员函数初始化  */
Dictionary *Dictionary::_pInstance = nullptr;
Dictionary::AutoRelease Dictionary::_ar;

/* 嵌套类定义 */
Dictionary::AutoRelease::~AutoRelease() {
    if (!_pInstance) {
        delete _pInstance;
        _pInstance == nullptr;
    }
}

/* Dictionary 实现 */
/* 构造 析构 */
Dictionary::Dictionary(const std::string &dictPath, const std::string &indexPath) {
    init(dictPath, indexPath); // 初始化
}

/* 获取实例 */
Dictionary *Dictionary::getInstance(const std::string &dictPath, const std::string &indexPath) {
    if (_pInstance == nullptr) {
        _pInstance = new Dictionary(dictPath, indexPath);
    }
    return _pInstance;
}

/* 初始化 */
void Dictionary::init(const std::string &dictPath, const std::string &indexPath) {
    // 0.打开文件
    std::ifstream ifsDict(dictPath);
    if (!ifsDict) {
        std::cerr << "dictPath open failed\n";
        exit(-1);
    }
    std::ifstream ifsIndex(indexPath);
    if (!ifsIndex) {
        std::cerr << "indexPath open failed\n";
        exit(-1);
    }

    // 1.词典
    std::string lineDict;
    std::string word;
    std::string frequencyStr;
    while (std::getline(ifsDict, word, ':') && std::getline(ifsDict, frequencyStr)) {
        _dict.emplace_back(word, std::stoi(frequencyStr));
    }

    // 2.索引
    std::string lineIndex;
    while (std::getline(ifsIndex, lineIndex)) {
        std::istringstream iss(lineIndex);
        std::string word;
        std::string indexSet;

        if (std::getline(iss, word, ':') && std::getline(iss, indexSet)) {
            std::istringstream issIndexSet(indexSet);
            std::size_t index;
            // 将数字部分按空格分割，并插入到 set 中
            while (issIndexSet >> index) {
                _indexTable[word].insert(index); // 将  插入到 map 的 set 中
            }
        }
    }

    // 3.关闭资源
    ifsDict.close();
    ifsIndex.close();
}

/* 获取词典 */
std::vector<std::pair<std::string, std::size_t>> &Dictionary::getDict() {
    return _dict;
}
/* 获取索引表 */
std::map<std::string, std::set<std::size_t>> &Dictionary::getIndexTable() {
    return _indexTable;
}

/* 查询中词占字节数 */
std::size_t nBytesCode(const char ch) {
    if (ch & (1 << 7)) {
        std::size_t nBytes = 1;
        for (int idx = 0; idx != 6; ++idx) {
            if (ch & (1 << (6 - idx))) {
                ++nBytes;
            } else {
                break;
            }
        }
        return nBytes;
    }
    return 1;
}

/*
 * 执行查询
 * 仅获取候选词的集合
 */
std::vector<std::pair<std::string, std::size_t>> Dictionary::searchCandiFreq(std::string &word) {

    // 0.候选词容器
    std::vector<std::pair<std::string, std::size_t>> candidates; // 词:词频

    // 1.分割查询词，查找索引集合
    size_t i = 0;
    std::set<std::string> characters; // 字符集

    // 2.遍历单字, 建立候选词中所有单字的索引集合
    std::set<std::size_t> chIndexSet;
    for (std::size_t chIdx = 0; chIdx < word.size();) {
        std::size_t byteNum = nBytesCode(word[chIdx]);                                   // 获取字节数
        std::string character = word.substr(chIdx, byteNum);                             // 获取该字符
        chIndexSet.insert(_indexTable[character].begin(), _indexTable[character].end()); // 建立集合
        chIdx += byteNum;                                                                // 下个字符下标的位置
    }

    // 3.根据索引, 在 _dict 查找到 候选词 以及 频率
    for (std::size_t lineNum : chIndexSet) {
        candidates.emplace_back(std::make_pair(_dict[lineNum].first, _dict[lineNum].second));
    }

    // 4.返回
    return candidates;
}

/* 获取词频, vector 容器遍历 */
std::size_t Dictionary::getFrequence(const std::string &word) {
    for (std::pair<std::string, std::size_t> &pair : _dict) {
        if (pair.first == word) {
            return pair.second;
        }
    }
    return 0;
}