/*************************************************************************
    > File Name: Configuration.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月11日 星期五 23时44分52秒
    > Mail: JiaZiChunQiu@163.com
    > Title: 配置文件类实现
    > Content:
    > 单例模式实现
    > 读取配置文件并存储到数据结构中
 ************************************************************************/

#include "Configuration.h"

#include <fstream>
#include <iostream>

/* 静态成员函数初始化  */
Configuration * Configuration::_pInstance = nullptr;
Configuration::AutoRelease Configuration::_ar;

/* 嵌套类定义 */
Configuration::AutoRelease::AutoRelease() {}
Configuration::AutoRelease::~AutoRelease() {
    if ( !_pInstance ) {
        delete _pInstance;
        _pInstance == nullptr;
    }
}

/* Configuration 类函数定义 */
Configuration::Configuration(const std::string & filePath)
: _configFilePath(filePath)
, _configs()
{}

/* 析构函数 */
Configuration::~Configuration(){}

/* 获取实例  */
Configuration * Configuration::getInstance(const std::string & filePath) {
    if( !_pInstance ) {
        if ( filePath.empty() ) {
            _pInstance = new Configuration;
        } else {
            _pInstance = new Configuration(filePath);
        }
    }
    return _pInstance;
}

void Configuration::setFilePath(const std::string & filePath) {
    if ( filePath.empty() ) {
        std::cerr << "setFilePath is empty\n";
        return;
    }
    if ( !_configs.empty() ) {
        _configs.clear(); // 每次重新设置路径都需清空
    }
    _configFilePath = filePath;
}

std::string Configuration::getFilePath() {
    if ( _configFilePath.empty() ) {
        return "_configFilePath id empty";
    } else {
        return _configFilePath;
    }
}

void Configuration::readConfigFile() {
    // 0.配置路径是否空
    if ( _configFilePath.empty() ) {
        std::cerr << "In readConfigFile the _configFilePath is empty\n";
        return;
    }

    // 1.关联文件
    std::ifstream ifs(_configFilePath);
    if ( !ifs ){
        std::cerr << "Configuration.cc Configuration ifs error\n";
        return;
    }

    // 2.分析并存储
    std::string key;
    std::string val;
    while ( std::getline(ifs, key, '=') && std::getline(ifs, val) ) {
        if ( key[0] == '#' ) { continue; } // 注释忽略
        // std::cout << "key = " << key << ", value = " << val << "\n";
        _configs[key] = val;
    }

    // 3.关闭资源
    ifs.close();
}

std::map<std::string, std::string> & Configuration::getConfigMap() {
    readConfigFile();
    return _configs;
}

