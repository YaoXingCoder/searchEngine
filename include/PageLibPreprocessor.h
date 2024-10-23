#ifndef __PAGE_LIB_PREPROCESSOR_H__
#define __PAGE_LIB_PREPROCESSOR_H__

#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#define CONF_PATH "./conf/pageLibPreProcessor.conf" // 读取用配置文件路径 : 网页库和偏移量 文件名=路径

#define OffsetLib_FILENAME "newoffset.dat" // 用于匹配 : 偏移量文件名
#define PageLib_FILENAME "newripepage.dat" // 用于匹配 : 网页库文件名
#define STOP_WORDS_EN "stop_words_en.txt"  // 用于匹配 : 英文停用词文件名
#define STOP_WORDS_ZN "stop_words_zh.txt"  // 用于匹配 : 中文停用词文件名

#define NewPageLib_PATH "./data/newripepage_pre.dat"    // 存储 : 网页库
#define NewOffsetLib_PATH "./data/newoffsetlib_pre.dat" // 存储 : 索引库
#define InvertIndexTable_PATH "./data/invertIndex.dat"  // 存储 : 倒排索引库存储路径

class SplitTool;
class WebPage;

class PageLibPreprocessor {
  public:
    // 构造
    PageLibPreprocessor(std::shared_ptr<SplitTool> splitTool, const std::string &confPath = CONF_PATH);

    void readInfoFromFile();  // 读取 : 根据配置信息读取网页库和位置偏移库的内容
    void cutRedundantPages(); // 去重 : 对冗余的网页进行去重
    
    void buildInvertIndexTable(); // 生成 : 倒排索引表

    // 将经过预处理后的网页库、位置偏移库、倒排索引写回磁盘
    void storeOnDisk(const std::string &pageLibPath = NewPageLib_PATH,
                     const std::string &offsetLibPath = NewOffsetLib_PATH,
                     const std::string &invertIndexLibPath = InvertIndexTable_PATH);

    /* 测试 */
    void showOffsetLib();        // 偏移库
    void showPartPageLib();      // 网页库
    void showPageLibIdSimhash(); // 去重后 Id
    void showPageLibSimhash();   // 去重后 pageLib
    void showInvertIndexTable(); // 倒排索引库

  private:
    /* 读取 */
    void doProcess();                                        // 预处理
    void readFromOffsetLibFile(const std::string &filePath); // 读取 : 网页偏移库
    void readFromPageLibFile(const std::string &filePath);   // 读取 : 网页库
    void readFromStopWrodsFile(const std::string &filePath); // 读取 : 停用词 词典

    void buildPageLibSimHash(); // 生成 : 去重后 网页库

    /* 存储 */
    void storePageLibAfterSimHash(const std::string &pageLibPath);     // 存储 去重后网页库到磁盘
    void storeOffsetLib(const std::string &offsetLibPath);             // 存储 去重后网页偏移库
    void storeInvertIndexTable(const std::string &invertIndexLibPath); // 存储 倒排索引库

  public:
    ~PageLibPreprocessor(); // 析构

  private:
    std::shared_ptr<SplitTool> _splitTool; // 分词工具
    std::string _confPath;                 // 配置文件路径

    std::unordered_set<std::string> _dict_stop;                                      // 停用词 词典
    std::vector<WebPage> _pageLib;                                                   // 读取文件中网页库
    std::unordered_map<std::size_t, std::pair<std::size_t, std::size_t>> _offsetLib; // 读取文件中网页偏移库

    // 存放去重后 id 和 指纹, 用于临时存放 id 和 smihash生成的指纹
    std::unordered_map<std::size_t, uint64_t> _pageLibIdSimhash;
    std::vector<WebPage> _pageLibSimHash;                                                        // 去重后 网页库
    std::unordered_map<std::string, std::vector<std::pair<std::size_t, double>>> _invertIndexTable; // 倒排索引对象

    static const int INIT_SIZE_VEC = 4 * 1024; // 初始化网页库容器大小
};

#endif
