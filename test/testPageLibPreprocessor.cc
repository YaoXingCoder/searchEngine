/*************************************************************************
    > File Name: testPageLibPreprocessor.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月14日 星期一 19时34分09秒
    > Mail: JiaZiChunQiu@163.com
    > Title: 网页预处理测试
    > Content:
 ************************************************************************/

#include "Configuration.h"
#include "PageLibPreprocessor.h"
#include "SplitTool.h"
#include "WebPage.h"

#include <iostream>

// 计算时间
#include <chrono> // 包含时间相关的库
#include <thread> // 用于模拟程序运行时间

#define PRE_PROCESSOR_CONF_PAHT "./conf/pageLibPreProcessor.conf"

/* =============== test =============== */
void test0() {
    PageLibPreprocessor pageLibPreprocessor(std::shared_ptr<SplitTool>(new SplitToolCppJieba()),
                                            PRE_PROCESSOR_CONF_PAHT);

    // pageLibPreprocessor.cutRedundantPages(); // 1.去重操作, 生成去重后

    // pageLibPreprocessor.showOffsetLib();  // 1.偏移库
    // pageLibPreprocessor.showPartPageLib(); // 1.网页库
    // pageLibPreprocessor.showPageLibIdSimhash(); // 1.去重后 id 容器
    // pageLibPreprocessor.showPageLibSimhash(); // 1.去重后网页库

    // pageLibPreprocessor.buildInvertIndexTable(); // 2.建立倒排索引库
    // pageLibPreprocessor.showInvertIndexTable(); // 2.测试倒排索引

    pageLibPreprocessor.storeOnDisk(); // 4.存储到磁盘
}

/* =============== main =============== */
int main(int argc, char *argv[]) {
    auto start = std::chrono::high_resolution_clock::now(); // 获取开始时间点

    test0();

    auto end = std::chrono::high_resolution_clock::now();             // 获取结束时间点
    std::chrono::duration<double, std::milli> duration = end - start; // 计算运行时间，单位为毫秒
    std::cout << "Program execution time: " << duration.count() << " ms" << std::endl; // 输出结果
}
