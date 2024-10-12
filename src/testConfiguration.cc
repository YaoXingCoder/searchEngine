/*************************************************************************
    > File Name: testConfiguration.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月12日 星期六 09时43分45秒
    > Mail: JiaZiChunQiu@163.com
    > Title:
    > Content:
 ************************************************************************/

#include "Configuration.h"

#include <iostream>
#include <utility>
#include <string>
#include <map>

#define CONFIG_FILE_PATH "./conf/yuliao_zn.conf"

#define CONFIG_STOP_PATH "./conf/yuliao_stop.conf"

/* =============== test =============== */
void test0() {
    Configuration::getInstance()->setFilePath(CONFIG_STOP_PATH);
    std::map<std::string, std::string> conf_map = Configuration::getInstance()->getConfigMap();
    for ( std::pair<const std::string, std::string> & pair : conf_map){
        std::cout << "key = " << pair.first << ", value = " << pair.second << "\n";
    }
}

void test1() {
    std::map<std::string, std::string> conf_map = Configuration::getInstance(CONFIG_FILE_PATH)->getConfigMap();
    for ( std::pair<const std::string, std::string> & pair : conf_map){
        std::cout << "key = " << pair.first << ", value = " << pair.second << "\n";
    }
    Configuration::getInstance()->setFilePath(CONFIG_STOP_PATH);
    conf_map.clear();
    conf_map = Configuration::getInstance()->getConfigMap();
    for ( std::pair<const std::string, std::string> & pair : conf_map){
        std::cout << "key = " << pair.first << ", value = " << pair.second << "\n";
    }
}

/* =============== main =============== */
int main (int argc, char* argv[]) {
    // test0();
    test1();
    return 0;
}
