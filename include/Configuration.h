#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <string>
#include <map>

/* 
 * 配置类 : 单例模式
 * 读取配置信息, 并存储在 map<string, string>
 *
 * 创建内部类, 防止内存泄漏
 * */
class Configuration {
    class AutoRelease{
    public:
        AutoRelease();
        ~AutoRelease();
    };
public:
    // 路径需要根据可执行文件所在位置进行判断
    static Configuration * getInstance(const std::string & filePath = "");
    std::map<std::string, std::string> & getConfigMap();
    void setFilePath(const std::string & filePath); // 重置配置文件路径
private:
    Configuration(const std::string & filePath = "");
    // Configuration();
    ~Configuration();

    void readConfigFile(); // 读取配置信息到数据结构

    Configuration(const Configuration &) = delete;
    Configuration & operator=(const Configuration &) = delete;
private:
    std::string _configFilePath; // 配置文件路径
    std::map<std::string, std::string> _configs; // 配置信息
    static Configuration * _pInstance; // 静态成员变量指针
    static AutoRelease _ar; // 内部类, 作用释放堆空间的单例
};

#endif
