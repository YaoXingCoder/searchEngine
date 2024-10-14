#ifndef __PAGE_LIB_PREPROCESSOR_H__
#define __PAGE_LIB_PREPROCESSOR_H__

#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <memory>

#define CONF_PATH "./conf/PageLibPre.conf"
#define PageLib_FILENAME "newwripepage.dat"
#define OffsetLib_FILENAME "newoffset.dat"

class SplitTool;
class WebPage;

class PageLibPreprocessor{
public:
    PageLibPreprocessor(std::shared_ptr<SplitTool> splitTool, const std::string & confPath = ""); // 构造
    
    void readInfoFromFile(); // 根据配置信息读取网页库和位置偏移库的内容
    void cutRedundantPages(); // 对冗余的网页进行去重
    void buildInvertIndexTable(); // 创建倒排索引表
    void storeOnDisk(); // 将经过预处理后的网页库、位置偏移库、倒排索引写回磁盘
private:
    void doProcess(); // 预处理
    void readFromPageLibFile(const std::string & filePath); // 读取网页库
    void readFromOffsetLibFile(const std::string & filePath); // 读取网页偏移库
public:
    ~PageLibPreprocessor();
private:
    std::shared_ptr<SplitTool> _splitTool;
    std::string _confPath; // 配置文件路径
    std::vector<WebPage> _pageLib;
    std::unordered_map<std::size_t, std::pair<std::size_t, size_t>> _offsetLib; // 网页偏移库对象
    std::unordered_map<std::string, std::vector<std::pair<size_t, double>>>  _invertIndexTable; // 倒排索引对象
    static const int INIT_SIZE_VEC = 4*1024;
};

#endif
