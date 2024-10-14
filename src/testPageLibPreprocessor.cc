/*************************************************************************
    > File Name: testPageLibPreprocessor.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月14日 星期一 19时34分09秒
    > Mail: JiaZiChunQiu@163.com
    > Title: 网页预处理测试
    > Content:
 ************************************************************************/

#include "Configuration.h"
#include "SplitTool.h"
#include "WebPage.h"
#include "PageLibPreprocessor.h"

#include <iostream>

#define PRE_PROCESSOR_CONF_PAHT "./conf/pageLibPreProcessor.conf"

/* =============== test =============== */
void test0() {
    PageLibPreprocessor pageLibPreprocessor(std::shared_ptr<SplitTool>(new SplitToolCppJieba()), PRE_PROCESSOR_CONF_PAHT);
    pageLibPreprocessor.readInfoFromFile();
    // pageLibPreprocessor.showOffsetLib(); 
    pageLibPreprocessor.showPartPageLib();
}

/* =============== main =============== */
int main (int argc, char* argv[]) {
    test0();
    return 0;
}
