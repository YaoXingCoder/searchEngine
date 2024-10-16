/*************************************************************************
    > File Name: TestEchoServer.cpp
    > Author: JiaZiChunQiu
    > Created Time: 2024年09月23日 星期一 09时19分50秒
    > Mail: JiaZiChunQiu@163.com
    > Title: 启动服务器
    > Content:
 ************************************************************************/

#include "EchoServer.h"

#include <iostream>

/* =============== test =============== */
void test0() {
    EchoServer echo(4, 10, "127.0.0.1", 8888);
    echo.start();
}
/* =============== main =============== */
int main(int argc, char *argv[]) {
    test0();
    return 0;
}
