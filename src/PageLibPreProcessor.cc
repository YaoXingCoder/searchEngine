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
PageLibPreprocessor::PageLibPreprocessor(std::shared_ptr<SplitTool> splitTool, const std::string & confPath);
: _splitTool(splitTool)
, _pageLib()
, _offsetLib()
, _invertIndexTable()
{
    _pageLib.reserve(INIT_SIZE_VEC); // 预留空间
}

/* 析构 */
PageLibPreprocessor::~PageLibPreprocessor() {}


