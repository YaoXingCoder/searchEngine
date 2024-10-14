/*************************************************************************
    > File Name: DisScanner.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月12日 星期六 10时18分25秒
    > Mail: JiaZiChunQiu@163.com
    > Title: 目录扫描类 -- 递归扫描 定义
    > Content:
 ************************************************************************/

#include "DirScanner.h"
#include "Configuration.h"

#include <dirent.h>
#include <sys/types.h>
#include <string.h>

#include <utility>
#include <iostream>

/*
* 构造
*/
DirScanner::DirScanner() 
: _confPath()
, _suffix()
{}

DirScanner::DirScanner(const std::string & confPath)
: _confPath(confPath)
, _suffix()
{}

DirScanner::DirScanner(const std::string & confPath, const std::string & suffix)
: _confPath(confPath)
, _suffix(suffix)
{}

DirScanner::~DirScanner() {}

/* 设置配置文件路径 */
void DirScanner::setConfPath(const std::string & confPath) {
    if ( confPath.empty() ) {
        std::cerr << "DirScanner setConfPath() is null\b";
        return;
    }
    _files.clear();
    _confPath = confPath;
}

/* 设置文件后缀 */
void DirScanner::setSuffix(const std::string & suffix) {
    if ( suffix.empty() ) {
        std::cerr << " DirScanner setSuffix() is null\n";
        return;
    }
    _suffix = suffix;
}

/* 
 * 读取配置文件, 获取语料的文件目录
 */
void DirScanner::operator()() {
    // 0.路径判空
    if ( _confPath.empty() ) { 
        std::cerr << "DirScanner _confPath is empty\n";
        return;
    }
    
    // 1.读取配置路径
    Configuration::getInstance()->setFilePath(_confPath);
    std::map<std::string, std::string> & configs = Configuration::getInstance()->getConfigMap();

    // 2.判断路径配置文件是否为空
    if ( configs.empty() ) {
        std::cerr << "DirScanner operator() configFile is empty\n";
        return;
    }

    // 3.判断是否有指定文件后缀
    if ( _suffix.empty() ) {
        for ( std::pair<const std::string, std::string> & pair : configs ) {
            // std::cout << "DirScanner read from Configuration is "  << pair.second << "\n";
            if (  !pair.second.empty() ) { traverseAll(pair.second); }
        }
    } else {
        for ( std::pair<const std::string, std::string> & pair : configs ) {
            // std::cout << "DirScanner read from Configuration is "  << pair.second << "\n";
            if (  !pair.second.empty() ) { traverseSuffix(pair.second); }
        }
    }
}

/* 获取存放文件路径的容器 */
std::vector<std::string> & DirScanner::getFiles() {
    return _files;
}

/* 
 * 递归遍历目录, 并获取每个文件的绝对路径
 * 所有文件
 * */
void DirScanner::traverseAll(const std::string & dirName) {
    // std::cout << "DirScanner traverse() dirName is "  << dirName << "\n";
    // 打开指定目录描述符
    DIR * pdir = opendir(dirName.c_str());
    if ( !pdir ) {
        std::cerr << "DirScanner traverseAll opendir() error\n";
        return;
    }

    // 创建目录项
    struct dirent * pdirent;

    // 循环读取
    while ( (pdirent = readdir(pdir)) != NULL ) {
        if ( strcmp(pdirent->d_name, ".") == 0 || strcmp(pdirent->d_name, "..") == 0 ) { continue; } // 遇到. .. 跳过

        /* 
        * 是普通文件 才添加到数据结构
        */
        std::string filename(pdirent->d_name);
        if ( pdirent->d_type == DT_REG) {
            std::string filepath = dirName + "/" + filename;
            _files.push_back(filepath); // 添加路径到数据结构中
        }

        // 如果是目录, 递归添加
        if ( pdirent->d_type == DT_DIR ) {
            // 拼凑路径
            char curr_path[512] = {0};
            sprintf(curr_path, "%s/%s", dirName.c_str(), pdirent->d_name);
            traverseAll(curr_path);
        }
    } 
    // 关闭资源
    closedir(pdir);
}

/* 指定文件后缀遍历 */
void DirScanner::traverseSuffix(const std::string & dirName) {
    // std::cout << "DirScanner traverse() dirName is "  << dirName << "\n";
    // 1.打开指定目录描述符
    DIR * pdir = opendir(dirName.c_str());
    if ( !pdir ) {
        std::cerr << "DirScanner traverseSuffix opendir() error\n";
        return;
    }

    // 2.创建目录项
    struct dirent * pdirent;

    // 3.循环读取
    while ( (pdirent = readdir(pdir)) != NULL ) {
        if ( strcmp(pdirent->d_name, ".") == 0 || strcmp(pdirent->d_name, "..") == 0 ) { continue; } // 遇到. .. 跳过

        // 3.1.filename 以 suffix 结尾添 且是普通文件 才添加到数据结构
        std::string filename(pdirent->d_name);
        if ( (filename.size() >= _suffix.size()) && (filename.substr(filename.size() - _suffix.size()) == _suffix) ) {
            if ( pdirent->d_type == DT_REG) {
                std::string filepath = dirName + "/" + filename;
                _files.push_back(filepath); // 添加路径到数据结构中
            }
        }

        // 3.2.如果是目录, 递归添加
        if ( pdirent->d_type == DT_DIR ) {
            // 拼凑路径
            char curr_path[512] = {0};
            sprintf(curr_path, "%s/%s", dirName.c_str(), pdirent->d_name);
            traverseSuffix(curr_path);
        }
    } 

    // 4.关闭资源
    closedir(pdir);
}

void DirScanner::showFiles() {
    if ( _files.empty() ) {
        std::cerr << "DirScanner _files is empty\n";
        return;
    }

    for ( std::string & path : _files ) {
        std::cout << "file path is " << path << "\n";
    }
}
