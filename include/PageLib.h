#ifndef __PAGE_LIB_H__
#define __PAGE_LIB_H__

#include <vector>
#include <string>
#include <utility>
#include <map>

class DirScanner;
class Configuration;

#define NEW_RIPE_PAGE "./data/newripepage.dat" // 存放 网页库
#define NEW_OFFSET "./data/newoffset.dat" // 存放 网页偏移库
#define INVERT_INDEX "./data/invertIndex.data" // 存放 倒排索引库

#define INIT_CAPACITY 8*1024*1024 // 预留存储空间

class PageLib {
public:
    PageLib(DirScanner & dirScanner); // 构造
    ~PageLib();
public:
    void create(); // 创建网页库, 分析xml文件, 并将内容存储到 _pages 容器中, 同时存储偏移量

    void store(); // 执行下面两个函数
    void storePages(); // 存储 网页库
    void storeOffset();// 存储 位置偏移库
public:
    void showPages();
    void showOffsetLib();
private:
    DirScanner & _dirScanner; // 扫描对象目录的引用
    std::vector<std::string> _pages; // 存放格式化之后的网页的容器
    std::map<std::size_t, std::pair<std::size_t, std::size_t>> _offsetLib; // 存放每篇文档在网页库的位置信息, map有序 id offset length
};

#endif
