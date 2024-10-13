#ifndef __DIR_SCANNER_H__
#define __DIR_SCANNER_H__

#include <vector>
#include <string>

class DirScanner {
public:
    DirScanner(const std::string & confPath); // 构造, 传入 conf 配置路径
    DirScanner(const std::string & confPath, const std::string & suffix); // 指定文件后缀
    DirScanner();

    void setConfPath(const std::string & confPath);
    void setSuffix(const std::string & suffix);

    void operator()(); // 创建 Configuration 类, 循环读取路径并执行 traverse, 同时存储路径到 _files 中

    std::vector<std::string> & getFiles(); // 获取 _files 容器
public:
    void showFiles(); // 测试 : 遍历容器
    ~DirScanner();
private:
    void traverseAll(const std::string & dirName); // 获取某目录文件下的所有文件
    void traverseSuffix(const std::string & dirName); // 遍历指定后缀文件
private:
    std::vector<std::string> _files; // 存放每个xml文件的绝对路径
    std::string _confPath; // 配置路径
    std::string _suffix; // 指定后缀
};

#endif
