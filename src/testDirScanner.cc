/*************************************************************************
    > File Name: testDirScanner.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月12日 星期六 11时26分01秒
    > Mail: JiaZiChunQiu@163.com
    > Title:
    > Content:
 ************************************************************************/

#include "DirScanner.h"

#include <iostream>

#define YULIAO_CONF_PATH "./conf/yuliao2.conf"

/* =============== test =============== */
void test0() {
    DirScanner dirScanner(YULIAO_CONF_PATH);
    dirScanner();
    std::vector<std::string> files = dirScanner.getFiles();
    for ( std::string & file : files ) {
        std::cout << "filepath : " << file << "\n";
    }
}
/* =============== main =============== */
int main (int argc, char* argv[]) {
    test0();
    return 0;
}
