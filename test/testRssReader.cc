/*************************************************************************
    > File Name: testRssReader.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月13日 星期日 16时32分52秒
    > Mail: JiaZiChunQiu@163.com
    > Title: xml 文件解析测试
    > Content:
 ************************************************************************/

#include <RssReader.h>

#include <iostream>

#define XML_PATH "./resource/web_rmw/auto.xml"

/* =============== test =============== */
void test0() {
    RssReader rssReader;
    rssReader.parseRss(XML_PATH);
    // rssReader.showRssVec();
    rssReader.dump("./data/testRss.dat");
}

/* =============== main =============== */
int main (int argc, char* argv[]) {
    test0();
    return 0;
}
