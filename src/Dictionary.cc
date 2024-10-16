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
Dictionary::AutoRelease::AutoRelease() {
}
Dictionary::AutoRelease::~AutoRelease() {
    if (!_pInstance) {
        delete _pInstance;
        _pInstance == nullptr;
    }
}

/* 单例实现 */
/* 构造 析构 */
Dictionary::Dictionary() {
}
Dictionary::~Dictionary() {
}

/* 获取实例 */
Dictionary *Dictionary::getInstance() {
    if (_pInstance == nullptr) {
        _pInstance = new Dictionary();
    }
    return _pInstance;
}

/* 初始化 */
void Dictionary::init(const string &dictPath, const string &indexPath) {
    // 字典
    std::ifstream ifsDict(dictPath);
    if (!ifsDict) {
        std::cerr << "dictPath open failed\n";
        exit(-1);
    }
    // 索引
    std::ifstream ifsIndex(indexPath);
    if (!ifsIndex) {
        std::cerr << "indexPath open failed\n";
        exit(-1);
    }

    // 1.词典
    string lineDict;
    while (std::getline(ifsDict, lineDict)) {
        std::istringstream iss(lineDict);
        std::string word;
        std::string feq;
        std::getline(iss, word, ':');
        std::getline(iss, feq);
        _dict[word] = std::stoi(feq);
    }

    // 2.索引
    string lineIndex;

    while (std::getline(ifsIndex, lineIndex)) {
        std::istringstream iss(lineIndex);
        std::string word;
        std::string numbers;

        if (std::getline(iss, word, ':') && std::getline(iss, numbers)) {
            std::istringstream nums(numbers);
            int num;

            // 将数字部分按空格分割，并插入到 set 中
            while (nums >> num) {
                _indexTable[word].insert(num); // 将  插入到 map 的 set 中
            }
        }
    }

    ifsDict.close();
    ifsIndex.close();
}

/* 获取词典 */
unordered_map<string, int> &Dictionary::getDict() {
    return _dict;
}

/* 获取索引表 */
map<string, set<int>> &Dictionary::getIndexTable() {
    return _indexTable;
}

/* 查询中词占字节数 */
size_t nBytesCode(const char ch) {
    if (ch & (1 << 7)) {
        int nBytes = 1;
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
 * 分词
 */
void splitWord(const string &file, vector<string> &candidates, set<int> &index) {
    // 从磁盘中导入候选词
    // 按块读取文件，减少磁盘IO操作
    string buff;           // 当前块
    string incompleteLine; // 不完全的行

    size_t IndexSize = index.size();
    std::ifstream ifs(file);
    if (!ifs) {
        std::cerr << "ifs open is failed\n";
    }

    int currentLine = 1; // 当前行
    while (ifs) {
        buff.resize(CHUNK_SIZE);

        ifs.read(&buff[0], CHUNK_SIZE);
        size_t bytesRead = ifs.gcount();
        buff.resize(bytesRead); // 调整大小，

        // 将缓冲区内容按行分割
        size_t start = 0;
        for (size_t pos = 0; pos < buff.size(); ++pos) {
            if (buff[pos] == '\n') {
                string line = buff.substr(start, pos - start);
                // 处理跨块的行
                if (!incompleteLine.empty()) {
                    line = incompleteLine + line;
                    incompleteLine.clear();
                }

                // 计数当前行
                if (index.count(currentLine) != 0) {
                    std::istringstream iss(line);
                    string word;
                    iss >> word;
                    candidates.push_back(word);
                    --IndexSize;
                }
                ++currentLine;
                if (IndexSize == 0) {
                    return;
                }
                start = pos + 1;
            }
        }

        // 处理未完成的行
        if (start < buff.size()) {
            incompleteLine = buff.substr(start);
        }
    }
}

/* 执行查询 */
vector<string> Dictionary::doQuery(std::string &word) {

    vector<string> candidates; // 候选词集合

    // 1.分割查询词，查找索引
    size_t i = 0;
    string character;       // 分割出的字符
    set<string> characters; // 字符集

    // 索引集
    set<int> index;
    while (i < word.size()) {
        size_t nBytes = nBytesCode(word[i]);
        string character = word.substr(i, nBytes);
        index.insert(_indexTable[character].begin(), _indexTable[character].end());
        i += nBytes; // 根据字节长度移动索引
    }

    // 1.索引
    splitWord("DICT_PATH", candidates, index);

    return candidates;
}

/* 获取词频 */
int Dictionary::getFrequence(const string &word) {
    return _dict[word];
}