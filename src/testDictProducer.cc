/*************************************************************************
    > File Name: testDictProducer.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月12日 星期六 15时23分09秒
    > Mail: JiaZiChunQiu@163.com
    > Title:
    > Content:
 ************************************************************************/

#include "DictProducer.h"
#include "SplitTool.h"

#include <iostream>

#define YULIAO_EN_CONF_PATH "./conf/yuliao_en.conf"
#define YULIAO_ZN_CONF_PATH "./conf/yuliao_zn.conf"

/* =============== test =============== */
void test0() {
    // 单参数英文
    DictProducer dictPro(YULIAO_EN_CONF_PATH);
    dictPro.showFiles();
    dictPro.buildEnDict();
    dictPro.storeVecFromMap();
    dictPro.showDict();
}

void test1() {
    // 两个参数能够处理中文
    std::shared_ptr<SplitTool> splitTool(new SplitToolCppJieba());
    DictProducer dictPro(YULIAO_ZN_CONF_PATH, splitTool);
    dictPro.showFiles();
    dictPro.buildCnDict();
    dictPro.storeVecFromMap();
    dictPro.showDict();
}

void test2() {
    // 三个参数处理中英文
    std::shared_ptr<SplitTool> splitTool(new SplitToolCppJieba());
    DictProducer dictPro(YULIAO_EN_CONF_PATH, YULIAO_ZN_CONF_PATH, splitTool);
    dictPro.showFiles();
    
    dictPro.buildEnDict();
    dictPro.buildCnDict();
    dictPro.buildIndex();

    dictPro.storeDict();
    dictPro.storeIndex();
}

/* =============== main =============== */
int main (int argc, char* argv[]) {
    // test0();
    // test1();
    test2();
    return 0;
}
