/*************************************************************************
    > File Name: testDictionary.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月17日 星期四 16时30分37秒
    > Mail: JiaZiChunQiu@163.com
    > Title: 在线词典库测试
    > Content:
 ************************************************************************/

#include "Dictionary.h"

#include <iostream>
#include <utility>
#include <string>

/* =============== test =============== */
void test0() {
    Dictionary * dict =  Dictionary::getInstance();
    /* for ( std::pair<std::string, std::size_t> & pair : dict->getDict() ) { */
    /*     std::cout << "word : " <<  pair.first << ", freq is " << pair.second << "\n"; */
    /* } */

    /* for ( std::pair<const std::string, std::set<std::size_t>> & pair : dict->getIndexTable() ) { */
    /*     std::cout << "word : " << pair.first << ", index is "; */
    /*     for ( std::size_t index : pair.second ) { */
    /*         std::cout << index << " "; */
    /*     } */
        /* std::cout << "\n"; */
    /* } */
    std::string word;
    std::cin >> word;
    for ( std::pair<std::string, std::size_t> & pair : dict->searchCandiFreq(word) ) {
        std::cout << "word is " << pair.first << ", freq is " << pair.second << "\n";
    }
}
/* =============== main =============== */
int main (int argc, char* argv[]) {
    test0();
    return 0;
}
