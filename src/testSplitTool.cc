/*************************************************************************
    > File Name: testSplitTool.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月11日 星期五 21时48分03秒
    > Mail: JiaZiChunQiu@163.com
    > Title: jieba 测试
    > Content:
 ************************************************************************/

#include "SplitTool.h"

#include <iostream>

/* =============== test =============== */
void test0() {
    SplitTool *splitTool = new SplitToolCppJieba();
    std::vector<std::string> words; //将用来存储每个单词
    // std::string s = "这部专著的面世， 使“山重水复”的朱自清研究平添活力，步入“柳暗花明”的新境界。";
    std::string s = "Hello World, it's time to do something. and to do some interesting more";
    words = splitTool->cut(s);
    for ( auto & word : words) {
        std::cout << word << "\n";
    }
}
/* =============== main =============== */
int main (int argc, char* argv[]) {
    test0();
    return 0;
}
