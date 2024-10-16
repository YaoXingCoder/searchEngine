/*************************************************************************
    > File Name: PageLibPreprocessor.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月14日 星期一 11时44分45秒
    > Mail: JiaZiChunQiu@163.com
    > Title:  PageLibPreProcessor 网页预处理类 实现
    > Content:
 ************************************************************************/

#include "PageLibPreprocessor.h"
#include "Configuration.h"
#include "SplitTool.h"
#include "WebPage.h"

#include <algorithm>
#include <cmath>
#include <unordered_map>

/* 构造 */
PageLibPreprocessor::PageLibPreprocessor(std::shared_ptr<SplitTool> splitTool, const std::string &confPath)
    : _splitTool(splitTool), _confPath(confPath), _dict_stop(), _pageLib(), _pageLibIdSimhash(), _pageLibSimHash(),
      _offsetLib(), _invertIndexTable() {
    _pageLib.reserve(INIT_SIZE_VEC);             // 预留空间
    _pageLibSimHash.reserve(INIT_SIZE_VEC >> 1); // 预留空间

    readInfoFromFile(); // 读取文件信息到容器

    cutRedundantPages(); // 去重操作, 生成去重后网页库容器

    buildInvertIndexTable(); // 建立倒排索引库
}

/* 析构 */
PageLibPreprocessor::~PageLibPreprocessor() {
}

/*
 * 1.读取配置文件
 * 2.获取配置文件中路径
 * 3.判空并执行读取函数
 */
void PageLibPreprocessor::readInfoFromFile() {
    std::cout << "## PageLibPreprocessor readInfoFromFile() 文件信息读取 开始\n";
    // 1.读取配置文件
    Configuration::getInstance()->setFilePath(_confPath);
    std::map<std::string, std::string> &confMap = Configuration::getInstance()->getConfigMap();

    // 2.获取路径
    std::string offsetLibPath = confMap[OffsetLib_FILENAME]; // 偏移库
    std::string pageLibPath = confMap[PageLib_FILENAME];     // 网页库
    std::string stopWordsEnPath = confMap[STOP_WORDS_EN];
    std::string stopWordsZnPath = confMap[STOP_WORDS_ZN];

    // 3.判断偏移库路径是否为空, 判断网页库路径是否为空
    if (!offsetLibPath.empty()) {
        if (!pageLibPath.empty()) {
            // 3.1 先读取偏移库, 再根据偏移库读取网页库
            readFromOffsetLibFile(offsetLibPath); // 读取偏移库, 存入 _offsetLib 容器
            readFromPageLibFile(pageLibPath);     // 读取网页库, 存入 _pageLib 容器
        } else {
            std::cerr << "pageLib of Path is error\n";
            return;
        }
    } else {
        std::cerr << "offsetLib of Path is error\n";
        return;
    }

    // 4.判空停用词路径, 英文 and 中文
    if (!stopWordsEnPath.empty()) {
        readFromStopWrodsFile(stopWordsEnPath);
    } else {
        std::cerr << "stopWordEnFile of Path is error\n";
    }
    if (!stopWordsZnPath.empty()) {
        readFromStopWrodsFile(stopWordsZnPath);
    } else {
        std::cerr << "stopWordZnFile of Path is error\n";
    }

    std::cout << "## PageLibPreprocessor readInfoFromFile() 文件信息读取 结束\n";
}
/* 读取 偏移库 */
void PageLibPreprocessor::readFromOffsetLibFile(const std::string &filePath) {
    // std::cout << "filePath = " << filePath << "\n";
    // 1.打开文件
    std::ifstream ifs(filePath);
    if (!ifs) {
        std::cerr << "## PageLibPreprocessor readFromOffsetLibFile() ifs is error\n";
        return;
    }

    // 2.遍历文件添加到_offsetLib容器
    std::string line;
    while (std::getline(ifs, line)) {
        std::istringstream iss(line);
        std::size_t pageId;
        std::size_t pageOffset;
        std::size_t pageLength;
        iss >> pageId >> pageOffset >> pageLength;
        _offsetLib.emplace(pageId, std::make_pair(pageOffset, pageLength));
    }

    // 3.关闭资源
    ifs.close();
}
/* 读取 网页库 */
void PageLibPreprocessor::readFromPageLibFile(const std::string &filePath) {
    // 1.打开文件
    std::ifstream ifs(filePath);
    if (!ifs) {
        std::cerr << "PageLibPreprocessor readFromPageLibFile() ifs is error\n";
        return;
    }

    // 2.遍历偏移容器, 读取网页库, 放入容器
    // NOTICE : 因为是直接查找id, 所以结束条件应该为 <=
    int count = 0;
    for (std::size_t i = 1; i <= _offsetLib.size(); ++i) {
        // std::cout << "count is " << ++count << '\n';
        // 2.1 获取偏移量和大小
        std::pair<std::size_t, std::size_t> &pair = _offsetLib[i];

        // 2.2 定位文件位置, 根据长度读取内容
        ifs.seekg(pair.first);
        std::string pageContent(pair.second, '\0');
        ifs.read(&pageContent[0], pair.second);

        // 2.3 将内容放入容器
        _pageLib.emplace_back(WebPage(pageContent, _splitTool)); // 构建
    }

    // 3.关闭资源
    ifs.close();
}
/* 读取 停用词文件 */
void PageLibPreprocessor::readFromStopWrodsFile(const std::string &filePath) {
    // 1.打开文件
    std::ifstream ifs(filePath);
    if (!ifs) {
        std::cerr << "DictProducer buildStopDict() ifs error\n";
        return;
    }

    // 2.循环读取并存储仅结构中
    std::string word;
    while (std::getline(ifs, word)) {
        _dict_stop.emplace(word);
    }
}

/*
 * 对冗余的网页进行去重
 * 存储去重后id, 到_pageLibIdSimhash容器
 */
void PageLibPreprocessor::cutRedundantPages() {
    std::cout << "## PageLibPreprocessor cutRedundantPages() 网页库去重 开始\n";
    // 1.构建 simhash
    simhash::Simhasher simhasher(DICT_PATH_SIMHASH, HMM_PATH_SIMHASH, IDF_PATH_SIMHASH, STOP_WORD_PATH);
    size_t topN = 5; // topN 表示特征的数量

    // 2.遍历所有 webPage
    for (WebPage &webPage : _pageLib) {
        // 2.1 生成指纹
        int flag = 1;     // 标志位
        uint64_t u64 = 0; // u64 用来保存指纹
        simhasher.make(webPage.getDocItem().content, topN, u64);

        // 2.2 遍历 比较
        for (std::pair<const std::size_t, uint64_t> &pair : _pageLibIdSimhash) {
            if (simhash::Simhasher::isEqual(u64, pair.second, 5)) {
                flag = 0; // 不添加
                break;
            }
        }

        // 2.3 存储信息
        if (flag) {
            _pageLibIdSimhash.emplace(webPage.getDocId(), u64);
        }
    }

    std::cout << "## PageLibPreprocessor cutRedundantPages() 网页库去重 结束\n";

    // 3.构建去重后网页库
    buildPageLibSimHash();
}

/*
 * 生成新的 pageLib, 去重后
 */
void PageLibPreprocessor::buildPageLibSimHash() {
    std::cout << "## PageLibPreprocessor buildPageLibSimHash() 去重网页库生成 开始\n";

    // 1.遍历 pageLib, 比较 id 生成 new vector
    // 同时更改 docid 为新容器的序列
    size_t newDocId = 1;
    for (int i = 0; i < _pageLib.size(); ++i) {
        if (_pageLibIdSimhash.count(_pageLib[i].getDocId()) != 0) {
            _pageLib[i].setDocId(newDocId);            // 变更为新的 id;
            _pageLib[i].calcTop(_dict_stop);           // 计算词频和 top 词
            _pageLibSimHash.emplace_back(_pageLib[i]); // 添加到新容器
            // _pageLib[i].showTopWords(); // 测试是否生成calcTop
            ++newDocId; // id自增
        }
    }

    std::cout << "## PageLibPreprocessor buildPageLibSimHash() 去重网页库生成 结束\n";
}

/*
 * 创建倒排索引表
 * 1.遍历去重后文章
 */
void PageLibPreprocessor::buildInvertIndexTable() {
    std::cout << "## PageLibPreprocessor buildInvertIndexTable() 倒排索引生成开始\n";
    std::size_t n = _pageLibSimHash.size(); // n : 总文档数
    // int count = 0;
    for (WebPage &webPageOut : _pageLibSimHash) {
        // 1.临时容器, 存储一篇文章的单词的所有权重
        std::unordered_map<std::string, double> pageWeights;

        // 2.每个文章中的单词和相应词频, 循环结束则一篇中所有的单词权重统计完毕
        // int wordCount = 0;
        for (std::string &wortTopOut : webPageOut.getTopWords()) {
            std::size_t tf = webPageOut.getWordsMap()[wortTopOut]; // TF : 在一篇文章中出现频次

            // 3.统计该词在在所有文章中出现的文章数
            std::size_t df = 0;
            for (WebPage &webPageIn : _pageLibSimHash) {
                // 4.查找每篇文章的前二十词频的单词是否有该单词
                for (std::string &wortTopIn : webPageIn.getTopWords()) {
                    if (wortTopOut == wortTopIn) {
                        ++df;
                        break;
                    }
                }
            }

            // 5.计算 IDF
            double idf = std::log2((n / (df + 1)) + 1);

            // 6.计算 W 权重
            double weight = tf * idf;

            // 7.放入集合
            pageWeights.emplace(wortTopOut, weight);

            // std::cout << "\r" << "当前文章id 为 "  << webPage.getDocId() << ", 已处理单词个数为 " << ++wordCount;
        }
        // std::cout << '\n';

        // 8.当前文章的所有权重归一化
        double l2Norm = 0.0;
        for (std::pair<const std::string, double> &pair : pageWeights) {
            l2Norm += pow(pair.second, 2);
        }
        l2Norm = std::sqrt(l2Norm);

        // 9.遍历当前文章权重集合, 计算权重, 存入倒序索引容器
        std::size_t docid = webPageOut.getDocId(); // 当前文章 id
        for (std::pair<const std::string, double> &pair : pageWeights) {
            double weight = pair.second / l2Norm;                                 // 取出单个权重, 归一化
            _invertIndexTable[pair.first].emplace(webPageOut.getDocId(), weight); // 插入容器
            // std::set<std::pair<std::size_t, double>>{std::make_pair(webPage.getDocId(), weight)};
        }

        // std::cout << '\r' << "## PageLibPreprocessor buildInvertIndexTable() 已处理 " << ++count << " 篇文章";
    }
    // putchar('\n');

    std::cout << "## PageLibPreprocessor buildInvertIndexTable() 倒排索引生成结束\n";
}

// 将经过预处理后的网页库、位置偏移库、倒排索引写回磁盘
void PageLibPreprocessor::storeOnDisk(const std::string &pageLibPath, const std::string &offsetLibPath,
                                      const std::string &invertIndexLibPath) {
    if (!pageLibPath.empty()) {
        storePageLibAfterSimHash(pageLibPath);
    } else {
        std::cerr << "PageLibPreprocessor storeOnDisk() pageLibPath is nullpty\n";
        return;
    }

    if (!pageLibPath.empty()) {
        storeOffsetLib(offsetLibPath);
    } else {
        std::cerr << "PageLibPreprocessor storeOnDisk() offsetLibPath is nullpty\n";
        return;
    }

    if (!pageLibPath.empty()) {
        storeInvertIndexTable(invertIndexLibPath);
    } else {
        std::cerr << "PageLibPreprocessor storeOnDisk() invertIndexLibPath is nullpty\n";
        return;
    }
}

/* 存储 去重后网页库到磁盘 */
void PageLibPreprocessor::storePageLibAfterSimHash(const std::string &pageLibPath) {
    // 1.打开文件, 默认清空|创建
    std::ofstream ofs(pageLibPath);
    if (!ofs) {
        std::cerr << "PageLibPreprocessor storePageLibAfterSimHash() ofs is error\n";
        return;
    }

    // 2.遍历去重后 WebPage, 并存入磁盘中
    for (WebPage &webPage : _pageLibSimHash) {
        ofs << webPage.getDoc();
    }

    // 3.关闭资源
    ofs.close();
}
/*
 * 存储 去重后网页偏移库
 * 偏移量写入给定文件
 */
void PageLibPreprocessor::storeOffsetLib(const std::string &offsetLibPath) {
    // 1.打开给定存放文件
    std::ofstream ofs(offsetLibPath);
    if (!ofs) {
        std::cerr << "PageLibPreprocessor storeOffsetLib() ofs is error\n";
        return;
    }

    // 2.遍历存储, 计算偏移量写入, 长度
    std::size_t offset = 0;
    for (WebPage &webPage : _pageLibSimHash) {
        ofs << webPage.getDocId() << " " << offset << " " << webPage.getDoc().size() << "\r\n";
        offset += webPage.getDoc().size();
    }

    // 3.关闭资源
    ofs.close();
}
/* 存储 倒排索引库 */
void PageLibPreprocessor::storeInvertIndexTable(const std::string &invertIndexLibPath) {
    // 1.打开给定存放文件
    std::ofstream ofs(invertIndexLibPath);
    if (!ofs) {
        std::cerr << "PageLibPreprocessor storeInvertIndexTable() ofs is error\n";
        return;
    }

    // 2.遍历容器, 输出到文件
    for (std::pair<const std::string, std::set<std::pair<size_t, double>>> &pair : _invertIndexTable) {
        ofs << pair.first;
        for (const std::pair<std::size_t, double> &idW : pair.second) {
            ofs << " " << idW.first << " " << idW.second;
        }
        ofs << "\r\n";
    }

    // 3.关闭资源
    ofs.close();
}

/*
 * 测试
 */
/* 测试:偏移库容器 */
void PageLibPreprocessor::showOffsetLib() {
    // 1.判空
    if (_offsetLib.empty()) {
        std::cerr << "PageLibPreprocessor showOffsetLib() _offsetLib is empty\n";
        return;
    }

    std::cout << "_offsetLib Size is " << _offsetLib.size() << "\n";

    // 2.遍历
    // for ( std::pair<const std::size_t, std::pair<std::size_t, size_t>> & pair : _offsetLib ) {
    //     std::cout << pair.first << " " << pair.second.first << " " << pair.second.second << "\n";
    // }
}
/* 测试:网页库容器 */
void PageLibPreprocessor::showPartPageLib() {
    // 1.判空
    if (_pageLib.empty()) {
        std::cerr << "PageLibPreprocessor showPartPageLib() _pageLib is empty\n";
        return;
    }

    std::cout << "_pageLib Size is " << _pageLib.size() << "\n";

    // 2.遍历
    for (WebPage &webPage : _pageLib) {
        // webPage.showDocItem();
        // std::cout << "docid is " << webPage.getDocId() << "\n";
        // ::putchar('\n');
    }
}
/* 测试:去重后容器大小 */
void PageLibPreprocessor::showPageLibIdSimhash() {
    // 1.判空
    if (_pageLibIdSimhash.empty()) {
        std::cerr << "PageLibPreprocessor showPageLibIdSimhash() _pageLibIdSimhash is empty\n";
        return;
    }

    std::cout << "_pageLibIdSimhash Size is " << _pageLibIdSimhash.size() << "\n";
}
/* 测试:去重后pageLib容器 */
void PageLibPreprocessor::showPageLibSimhash() {
    // 1.判空
    if (_pageLibSimHash.empty()) {
        std::cerr << "PageLibPreprocessor showPageLibSimhash() _pageLibSimHash is empty\n";
        return;
    }

    // 2.大小
    std::cout << "_pageLibSimHash Size is " << _pageLibSimHash.size() << "\n";

    // 3.遍历
    // for ( WebPage & webPage : _pageLibSimHash ) {
    //     webPage.showDocItem();
    //     // std::cout << "docid is " << webPage.getDocId() << "\n";
    //     ::putchar('\n');
    // }
}
/* 测试: 倒排索引 */
void PageLibPreprocessor::showInvertIndexTable() {
    // 1.判空
    if (_invertIndexTable.empty()) {
        std::cerr << "PageLibPreprocessor showInvertIndexTable() _invertIndexTable is empty\n";
        return;
    }

    // 2.大小
    std::cout << "_invertIndexTable Size is " << _invertIndexTable.size() << "\n";

    // 3.遍历
    for (std::pair<const std::string, std::set<std::pair<size_t, double>>> &pair : _invertIndexTable) {
        std::cout << "word is " << pair.first;
        for (const std::pair<std::size_t, double> &idW : pair.second) {
            std::cout << " " << std::to_string(idW.first) << " " << std::to_string(idW.second) << "\t";
        }
        std::cout << '\n';
    }
}

/* 根据偏移量读取文本内容测试 */
// void PageLibPreprocessor::readFromPageLibFile(const std::string & filePath) {
//     // 1.打开文件
//     std::ifstream ifs(filePath);
//     if ( !ifs ) {
//         std::cerr << "PageLibPreprocessor readFromPageLibFile() ifs is error\n";
//         return;
//     }

//     // 2.遍历偏移容器, 读取网页库, 放入容器
//     // for(std::size_t i = 1; i < _offsetLib.size(); ++i) {
//     for(std::size_t i = 1; i < 2; ++i) {
//         std::pair<std::size_t, std::size_t> & pair = _offsetLib[i];
//         // 2.1 定位文件位置, 根据长度读取内容
//         ifs.seekg(pair.first);
//         std::string pageContent(pair.second, '\0');
// std::string pageContent;
// pageContent.resize(pair.second);
//         ifs.read(&pageContent[0], pair.second);

//         std::cout << "pair Size = " << pair.second << "\n";
//         std::cout << "pageContent Size = " << pageContent.size() << "\n";
//         std::cout << pageContent;
//     }

//     // 3.关闭资源
//     ifs.close();
// }

/* 已丢弃排序去重策略  */
// void PageLibPreprocessor::cutRedundantPages() {
//     // 1.排序 : 根据文档 id 进行排序
//     std::sort(_pageLib.begin(), _pageLib.end());

//     // 2.unique : 取出重复元素, 返回值为指向容器最后重复元素位置的迭代器指针
//     std::vector<WebPage>::iterator itLast = std::unique(_pageLib.begin(), _pageLib.end());

//     // 3.去除重复元素
//     _pageLib.erase(itLast, _pageLib.end());
// }