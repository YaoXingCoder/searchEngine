#ifndef __DIR_SCANNER_H__
#define __DIR_SCANNER_H__

#include <vector>
#include <string>

class DirScanner {
public:
    void operator()();
    std::vector<std::string> & getFiles();
    void traverse(const std::string & dirName); // 获取某目录文件下的所有文件
public:
    DirScanner(const std::string & confPath);
    ~DirScanner();
private:
    std::vector<std::string> _files; // 存放每个语料文件的绝对路径
    std::string _confPath;
};

#endif
