/*************************************************************************
    > File Name: testStrCN.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月12日 星期六 16时59分29秒
    > Mail: JiaZiChunQiu@163.com
    > Title:
    > Content:
 ************************************************************************/

#include <iostream>
#include <string>

/* =============== test =============== */
void test0() {
    std::string str_cn("你好啊\r\n世界哈哈哈哈");
    std::basic_string<char>::size_type i = 0;
    std::string::iterator it = str_cn.begin();
    while( it < str_cn.end() ) {
        if ( *it == '\r' || *it == '\n' ) {
            it = str_cn.erase(it); // 删除字符
        } else {
            ++it;
        }
    }
    std::cout << str_cn << "\n";
}

void test1() {
    std::string str;
    if ( str.empty() ) {
        std::cout << "\"\" is empty\n";
    }
}

// 根据第一个字节判断所占字节数
size_t getByteNum_UTF8(const char byte)  {  
    int byteNum = 0;  
    for (size_t i = 0; i < 6; ++i)  {  
        if (byte & (1 << (7 - i)))  
            ++byteNum;  
        else  
            break;  
    }  
    return byteNum == 0 ? 1 : byteNum; 
}

void test2() {
    std::string str("你");
    int byteNum = getByteNum_UTF8(*str.c_str());
    std::cout << "byteNum is " << byteNum << "\n";
}
/* =============== main =============== */
int main (int argc, char* argv[]) {
    // test0();
    // test1();
    test2();
    return 0;
}
