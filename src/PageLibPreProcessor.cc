/*************************************************************************
    > File Name: PageLibPreProcessor.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月14日 星期一 11时44分45秒
    > Mail: JiaZiChunQiu@163.com
    > Title:  PageLibPreProcessor 网页预处理类 实现
    > Content:
 ************************************************************************/

#include "Configuration.h"
#include "SplitTool.h"
#include "WebPage.h"
#include "PageLibPreProcessor.h"

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
    
    // 2.遍历判断获取文件名, 进行相应的文件读取和存入容器
    for( std::pair<std::string ,std::string> & pair : confMap ){
        if ( pair.first == PageLib_FILENAME ) {
    
        }
    }
    // 2.遍历偏移库文件, 并存入 _offsetLib 容器

    // 3.读取网页库并存入 _pageLib 容器
}

void PageLibPreprocessor:
