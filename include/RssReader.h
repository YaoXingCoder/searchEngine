#ifndef __RSS_XML_H__
#define __RSS_XML_H__

#include <string>
#include <vector>

/*
 * Rss内容
 */
struct RssItem {
  public:
    RssItem();
    RssItem(const std::string &, const std::string &, const std::string &); // 构造
    RssItem(const RssItem &);                                               // 移动
    std::size_t size();
    ~RssItem();

  public:
    std::string title;       // 标题
    std::string link;        // 链接
    std::string description; // 内容
};

/*
 * Rss 解析类
 * */
class RssReader {
  public:
    RssReader(const int capacity = _initSize); // 构造
    ~RssReader();

  public:
    void parseRss(const std::string &xmlFilePath); // tinyxml2解析xml文件, 并存入容器
    void parseRss(const std::string &xmlFilePath, std::vector<RssItem> &rssVec); // tinyxml2解析xml文件, 并存入给定容器
    void dump(const std::string &savePath);                                      // 遍历容器存入文件中
    std::vector<RssItem> &getRssVec();

  public:
    void showRssVec(); // 测试 : 遍历输出容器

  private:
    void dealText(std::string &text);

  private:
    std::vector<RssItem> _rssVec;    // 解析后存放Rss的容器
    static const int _initSize = 32; // 预留空间给容器
};

class FileProcessor {
  public:
    FileProcessor();
    void process(std::vector<std::string> &files, const std::string &filename); // 处理网页
    std::string xmlToJson(std::string &doc);

  private:
    std::string _titleFeature;
};

#endif
