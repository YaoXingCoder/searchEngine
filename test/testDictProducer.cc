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
#define CONFIG_STOP_PATH "./conf/yuliao_stop.conf" // 停用词配置文件路径

/* =============== test =============== */
void test0() {
    // 单参数英文
    ConfsPath confsPath;
    confsPath.confPathEN = YULIAO_EN_CONF_PATH;
    
    DictProducer dictPro( confsPath, std::shared_ptr<SplitTool>(new SplitToolCppJieba()) );
    dictPro.showFiles(); // 显示文件路径
    // dictPro.showIndex();
    // dictPro.showDict();
}

void test1() {
    ConfsPath confsPath;
    confsPath.confPathEN = YULIAO_EN_CONF_PATH;
    confsPath.confPathZN = YULIAO_ZN_CONF_PATH;
    confsPath.confPathSTOP = CONFIG_STOP_PATH;
    
    DictProducer dictPro( confsPath, std::shared_ptr<SplitTool>(new SplitToolCppJieba()) );
    // dictPro.showFiles(); // 显示文件路径
    dictPro.storeDict();
    dictPro.storeIndex();
}

/* =============== main =============== */
int main (int argc, char* argv[]) {
    // test0();
    test1();
    return 0;
}
