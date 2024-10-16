#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

using std::map;
using std::pair;
using std::set;
using std::string;
using std::unordered_map;
using std::vector;

#define DICTIONARY_DICT_PATH "./data/dict.dat" // 词频库
#define DICT_INDEX_PATH "./data/dictIndex.dat" // 索引库

class Dictionary {
    class AutoRelease {
      public:
        AutoRelease();
        ~AutoRelease();
    };

  public:
    static Dictionary *getInstance(); // 创建
    void init(const string &dictPath = DICTIONARY_DICT_PATH,
              const string &indexPath = DICT_INDEX_PATH); // 初始化 : 加载

    unordered_map<string, int> &getDict();  // 获取词典
    map<string, set<int>> &getIndexTable(); // 获取索引表
    vector<string> doQuery(string &word);   // 执行查询
    int getFrequence(const string &word);   // 查询词频

  private:
    Dictionary();
    ~Dictionary();

    Dictionary(const Dictionary &) = delete;
    Dictionary &operator=(const Dictionary &) = delete;

  private:
    std::unordered_map<string, int> _dict; // 词典
    map<string, set<int>> _indexTable;     // 索引表

    static Dictionary *_pInstance; // 单例类
    static AutoRelease _ar;        // 内部类, 作用释放堆空间的单例
};
#endif
