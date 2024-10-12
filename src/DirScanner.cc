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

DirScanner::DirScanner(const std::string & confPath)
: _confPath(confPath)
{}
DirScanner::~DirScanner() {}

/* 
 * 读取配置文件, 获取语料的文件目录
 */
void DirScanner::operator()(){
    std::map<std::string, std::string> configs = Configuration::getInstance(_confPath)->getConfigMap();
    for ( std::pair<const std::string, std::string> & pair : configs ) {
        traverse(pair.second);
    }
}

std::vector<std::string> & DirScanner::getFiles() {
    return _files;
}

/* 
 * 递归遍历目录, 并获取每个文件的绝对路径
 * */
void DirScanner::traverse(const std::string & dirName) {
    // 打开指定目录描述符
    DIR * pdir = opendir(dirName.c_str());
    if ( !pdir ) {
        std::cerr << "DirScanner traverse opendir()\n";
        return;
    }

    // 创建目录项
    struct dirent * pdirent;

    // 循环读取
    while ( (pdirent = readdir(pdir)) != NULL ) {
        if ( strcmp(pdirent->d_name, ".") == 0 || strcmp(pdirent->d_name, "..") == 0 ) { continue; } // 遇到. .. 跳过

        /* 
        * filename 以 .txt 结尾添 且是普通文件 才添加到数据结构
        */
        std::string filename(pdirent->d_name);
        if ( (filename.size() >= 4) && (filename.substr(filename.size() - 4) == ".txt") ) {
            if ( pdirent->d_type == DT_REG) {
                std::string filepath = dirName + "/" + filename;
                _files.push_back(filepath); // 添加路径到数据结构中
            }
        }

        // 如果是目录, 递归添加
        if ( pdirent->d_type == DT_DIR ) {
            // 拼凑路径
            char curr_path[512] = {0};
            sprintf(curr_path, "%s/%s", dirName.c_str(), pdirent->d_name);
            traverse(curr_path);
        }
    } 

    // 关闭资源
    closedir(pdir);
}
