#ifndef __DICT_PRODUCER_H__
#define __DICT_PRODUCER_H__

#include <set>
#include <map>
#include <vector>
#include <utility>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>

#define DEST_DICT "./data/dict.dat" // 存放 词典文件路径
#define DEST_INDEX "./data/dictIndex.dat" // 存放 词典索引文件路径

#define VECT_SIZE 8*1024*1024 // 预留存储空间大小

class SplitTool;

/* 配置文件路径 */
typedef struct{
    std::string confPathZN; // 中文
    std::string confPathEN; // 英文路径
    std::string confPathSTOP; // 停用词路径
}ConfsPath;

class DictProducer {
public:
    DictProducer(const ConfsPath & confsPath, std::shared_ptr<SplitTool> splitTool); // splitTool 中英文处理
    ~DictProducer();
public:
    /* 存储 */ 
    void storeDict(const std::string & filePath = DEST_DICT); // 字典 保存
    void storeIndex(const std::string & filePath = DEST_INDEX); // 索引 保存

    /* TODO:添加  */
    void pushDict(const std::string & word); // 存储某个单词

    /* 测试 */
    void showFiles() const; // 查看所有需要处理的文件路径
    void showDict() const; // 查看生成词典
    void showIndex() const; // 查看生成索引
    void storeVecFromMap(); // 将 map 放入 vector, 无用

private:
    /* 读取配置  */
    void readFromConfigEN(const std::string & confPathEN); // 英文 从 conf 读取路径到 数据结构
    void readFromConfigZN(const std::string & confPathZN); // 中文 从 conf 读取路径到 数据结构
    void readFromConfigSTOP(const std::string & confPathStop); // 停用词

    /* 字典 */
    void buildEnDict(); // 创建英文词典
    void buildCnDict(); // 创建中文词典
    void buildStopDict(); // 创建停用词词典
    // 不用停用词
    void buildEnDictNoStop();
    void buildCnDictNoStop();

    void buildDictStopWrods(); // 生成字典, 是否使用停用词

    /* 索引 */
    void buildIndex(); // 创建索引

private:
    void dealLine(std::string & line); // 英文单行处理
    size_t getByteNum_UTF8(const char ch); // 根据首地址获取单字所长字节数

private:
    std::vector<std::string> _filesEN; // 英文, 语料库文件的绝对路径集合
    std::vector<std::string> _filesZN; // 中文, 语料库文件的绝对路径集合
    std::vector<std::string> _filesSTOP; // 停用词语料库 路径 集合

    std::vector<std::pair<const std::string, int>> _dict; // 词频统计, 暂时用在map存到vector, 暂时无用
    std::unordered_map<std::string, int> _dict_map; // 用于临时存储, 去重
    std::map<std::string, std::set<int>> _index; // 单词在 vector 词典中出现的位置(下标)
    std::unordered_set<std::string> _dict_stop; // 停用词 词典
    
    std::shared_ptr<SplitTool> _splitTool; // 分词工具
};

#endif
