/*************************************************************************
    > File Name: testPageLib.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月13日 星期日 19时56分00秒
    > Mail: JiaZiChunQiu@163.com
    > Title:
    > Content:
 ************************************************************************/

#include "PageLib.h"
#include "DirScanner.h"
#include "Configuration.h"

#include <iostream>

#define XML_CONF "./conf/xml_path.conf"
// #define YULIAO_ZN_CONF_PATH "./conf/yuliao_zn.conf"

/* =============== test =============== */
void test0() {
    DirScanner dirScanner(XML_CONF);
    // DirScanner dirScanner(XML_CONF, ".txt");
    // dirScanner();
    // std::cout << Configuration::getInstance()->getFilePath() << "\n";
    // dirScanner.showFiles();

    PageLib pageLib(dirScanner);
    // dirScanner.showFiles();
    pageLib.create();
    // pageLib.showPages();
    // pageLib.showOffsetLib();
    pageLib.store();
}
/* =============== main =============== */
int main (int argc, char* argv[]) {
    test0();
    return 0;
}
