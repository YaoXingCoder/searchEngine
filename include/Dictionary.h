#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

#include <map>
#include <set>
#include <string>
#include <vector>

#define DICTIONARY_DICT_PATH "./data/dict.dat"       // 字典库
#define DICTIONARY_INDEX_PATH "./data/dictIndex.dat" // 索引库

class Dictionary {
    class AutoRelease {
      public:
        ~AutoRelease();
    };

  public:
    static Dictionary *getInstance(const std::string &dictPath = DICTIONARY_DICT_PATH,
                                   const std::string &indexPath = DICTIONARY_INDEX_PATH); // 创建

    std::vector<std::pair<std::string, std::size_t>> &getDict();   // 获取词典
    std::map<std::string, std::set<std::size_t>> &getIndexTable(); // 获取索引表
    std::size_t getFrequence(const std::string &word);             // 查询词频

    std::vector<std::pair<std::string, std::size_t>> searchCandiFreq(std::string &word); // 查找候选词和词频

  private:
    Dictionary(const std::string &dictPath, const std::string &indexPath);

    void init(const std::string &dictPath, const std::string &indexPath); // 初始化:加载文件

    Dictionary(const Dictionary &) = delete; // 禁止拷贝和复制
    Dictionary &operator=(const Dictionary &) = delete;

  private:
    std::vector<std::pair<std::string, std::size_t>> _dict;   // 词典
    std::map<std::string, std::set<std::size_t>> _indexTable; // 索引表

    static Dictionary *_pInstance; // 单例类
    static AutoRelease _ar;        // 内部类, 作用释放堆空间的单例
};
#endif
