/*************************************************************************
    > File Name: PageLibPreprocessor.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月14日 星期一 11时44分45秒
    > Mail: JiaZiChunQiu@163.com
    > Title:  PageLibPreProcessor 网页预处理类 实现
    > Content:
 ************************************************************************/

#include "Configuration.h"
#include "SplitTool.h"
#include "WebPage.h"
#include "PageLibPreprocessor.h"

/* 构造 */
PageLibPreprocessor::PageLibPreprocessor(std::shared_ptr<SplitTool> splitTool, const std::string & confPath)
: _splitTool(splitTool)
, _confPath(confPath)
, _pageLib()
, _offsetLib()
, _invertIndexTable()
{
    _pageLib.reserve(INIT_SIZE_VEC); // 预留空间
}

/* 析构 */
PageLibPreprocessor::~PageLibPreprocessor() {}

/* 根据配置信息读取网页库和移动偏移库的内容 */
void PageLibPreprocessor::readInfoFromFile() {
    // 1.读取配置文件
    Configuration::getInstance()->setFilePath(_confPath);
    std::map<std::string, std::string> confMap = Configuration::getInstance()->getConfigMap();
    
    // 2.遍历偏移库文件, 并存入 _offsetLib 容器
    std::string offsetLibPath = confMap[OffsetLib_FILENAME];
    std::string pageLibPath = confMap[PageLib_FILENAME];

    // 2.1 判断偏移库路径是否为空
    if ( !offsetLibPath.empty() ) { 
        readFromOffsetLibFile(offsetLibPath); // 2.1.1 读取偏移库, 存入 _offsetLib 容器
        // 2.2 判断网页库路径是否为空
        if ( !pageLibPath.empty() ) {
            readFromPageLibFile(pageLibPath); // 2.2.1 读取网页库, 存入 _pageLib 容器
        } else {
            std::cerr << "pageLib of Path is error\n"; return;
        }
    } else { 
        std::cerr << "pageLib of Path is error\n"; return; 
    }
}

/* 读取偏移库 */
void PageLibPreprocessor::readFromOffsetLibFile(const std::string & filePath) {
    // std::cout << "filePath = " << filePath << "\n";
    // 1.打开文件
    std::ifstream ifs(filePath);
    if ( !ifs ) {
        std::cerr << "PageLibPreprocessor readFromOffsetLibFile() ifs is error\n";
        return;
    }

    // 2.遍历文件添加到_offsetLib容器
    std::string line;
    while( std::getline(ifs, line) ) {
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

/* 读取网页库 */
void PageLibPreprocessor::readFromPageLibFile(const std::string & filePath) {
    // 1.打开文件
    std::ifstream ifs(filePath);
    if ( !ifs ) {
        std::cerr << "PageLibPreprocessor readFromPageLibFile() ifs is error\n";
        return;
    }

    // 2.遍历偏移容器, 读取网页库, 放入容器
    // NOTICE : 因为是直接查找id, 所以结束条件应该为 <=
    int count = 0;
    for(std::size_t i = 1; i <= _offsetLib.size(); ++i) {
        // std::cout << "count is " << ++count << '\n';
        // 2.1 获取偏移量和大小
        std::pair<std::size_t, std::size_t> & pair = _offsetLib[i];

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

// 对冗余的网页进行去重
void PageLibPreprocessor::cutRedundantPages() {

}

// 创建倒排索引表
void PageLibPreprocessor::buildInvertIndexTable(){} 

// 将经过预处理后的网页库、位置偏移库、倒排索引写回磁盘
void PageLibPreprocessor::storeOnDisk(){} 


/* 
* 测试
*/
/* 测试偏移库 */
void PageLibPreprocessor::showOffsetLib() {
    // 1.判空
    if ( _offsetLib.empty() ) {
        std::cerr << "PageLibPreprocessor showOffsetLib() _offsetLib is empty\n";
        return;
    }

    std::cout << "_offsetLib Size is " << _offsetLib.size() << "\n";

    // 2.遍历
    // for ( std::pair<const std::size_t, std::pair<std::size_t, size_t>> & pair : _offsetLib ) {
    //     std::cout << pair.first << " " << pair.second.first << " " << pair.second.second << "\n";
    // }
}

/* 测试网页库 */
void PageLibPreprocessor::showPartPageLib() {
    // 1.判空
    if ( _pageLib.empty() ) {
        std::cerr << "PageLibPreprocessor showPartPageLib() _pageLib is empty\n";
        return;
    }

    // std::cout << "_pageLib Size is " << _pageLib.size() << "\n";

    // 2.遍历
    for ( WebPage & webPage : _pageLib ) {
        webPage.showDocItem();
        // std::cout << "docid is " << webPage.getDocId() << "\n";
        ::putchar('\n');
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