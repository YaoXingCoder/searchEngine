/*************************************************************************
    > File Name: PageLib.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月12日 星期六 23时13分11秒
    > Mail: JiaZiChunQiu@163.com
    > Title: 网页库实现
    > Content:
 ************************************************************************/

#include "PageLib.h"
#include "Configuration.h"
#include "DirScanner.h"
#include "RssReader.h"

#include <sstream>
#include <fstream>
#include <iostream>

PageLib::PageLib(DirScanner & dirScanner)
: _dirScanner(dirScanner)
, _pages()
, _offsetLib()
{
    _pages.reserve(_initCapacity); // 预留空间
    create(); // 创建
}

/* 析构 */
PageLib::~PageLib() {}

/* 处理文本 删除 '\t' '\n' ' ' */
void PageLib::dealText(std::string & text) {
    // 1.判空
    // if ( text.empty() ) { 
    //     std::cerr << "PageLib dealText() text is empty()\n";
    //     return;
    // }

    // 2.处理
    std::string::iterator it = text.begin();
    while ( it < text.end() ) {
        if ( *it == '\n' || *it == '\t' || *it == ' ' || *it == '\r') {
            it = text.erase(it);
        } else {
            ++it;
        }
    }
}

/* 构建网页库和偏移库到容器 */
void PageLib::create() {
    // 1.执行扫面对象, 将路径放入临时的容器
    _dirScanner();
    std::vector<std::string> pathVec = _dirScanner.getFiles();

    // 2.遍历容器, 解析 xml 文件
    RssReader rssReader;
    std::size_t offset = 0; // 记录偏移量
    std::size_t docid = 1; // 记录行号

    // 2.每次循环一个网页 xml 文件
    for ( std::string & path : pathVec) {
        // 2.1 解析文件, 并存入(添加)给定容器中
        std::vector<RssItem> rssVec;
        rssReader.parseRss(path, rssVec);
        
        // 2.2 存放网页文件
        for ( RssItem & rssItem : rssVec ) {
            std::ostringstream oss;
            // 2.3 每次判空后再写入
            oss << "<doc>" << "\r\n";
            oss << "\t" << "<docid>" << std::to_string(docid) << "</docid>" << "\r\n";

            if ( !rssItem.title.empty() ) { 
                dealText(rssItem.title);
                oss << "\t" << "<title>" << rssItem.title << "</title>" << "\r\n"; 
            } else { 
                oss << "\t" << "<title>" << "title is empty" << "</title>" << "\r\n"; 
            }

            if ( !rssItem.link.empty() ) { 
                dealText(rssItem.link);
                oss << "\t" << "<link>" << rssItem.link << "</link>" << "\r\n"; 
            } else { 
                oss << "\t" << "<link>" << "link is empty" << "</link>" << "\r\n"; 
            }

            if ( !rssItem.description.empty() ) { 
                dealText(rssItem.description);
                oss << "\t" << "<content>" << rssItem.description << "</content>" << "\r\n"; 
            } else { 
                oss << "\t" << "<content>" << "content is empty" << "</content>" << "\r\n"; 
            }

            oss << "</doc>" << "\r\n";

            // 2.4 每个网页放入容器中
            _pages.emplace_back(oss.str()); // 将内容放入容器

            // 2.5 存放每个网页文件偏移量
            _offsetLib.emplace(docid, std::make_pair(offset, oss.str().size()));

            // 2.6 累计偏移量
            offset += oss.str().size();

            // 2.7 不忘记 docid++
            ++docid; // id自增
        }
    }
}

/* 存储容器到给定文件  */
void PageLib::store(const std::string & pagesLibPath, const std::string & offsetLibPath) {
    storePages(pagesLibPath);
    storeOffset(offsetLibPath);
}

/* 网页数据写入给定文件 */
void PageLib::storePages(const std::string & pagesLibPath) {
    // 1.打开给定存放文件
    std::ofstream ofs(pagesLibPath);
    if ( !ofs ) {
        std::cerr << "PageLib storePages() ofs error\n";
        return;
    }

    // 2.内容存放进文件
    for ( std::string & page : _pages ) {
        ofs << page;
    }

    // 3.关闭资源
    ofs.close();
}


/* 偏移量写入给定文件 */
void PageLib::storeOffset(const std::string & offsetLibPath) {
    // 1.打开给定存放文件
    std::ofstream ofs(offsetLibPath);
    if ( !ofs ) {
        std::cerr << "PageLib storeOffset() ofs error\n";
        return;
    }

    // 2.内容存放进文件
    for ( std::pair<const std::size_t, std::pair<std::size_t, std::size_t>> & pair_offset : _offsetLib ) {
        ofs << pair_offset.first << " " << pair_offset.second.first << " " << pair_offset.second.second << "\r\n";
    }

    // 3.关闭资源
    ofs.close();
}

/*
* 测试
*/
/* 测试网页数据容器  */ 
void PageLib::showPages() {
    if ( _pages.empty() ) { 
        std::cerr << "PageLib pages is nullptr\n";
        return;
    }
    for ( std::string & page : _pages ) {
        std::cout << page;
    }
}

/* 测试偏移量容器 */
void PageLib::showOffsetLib() {
    if ( _offsetLib.empty() ) {
        std::cerr << "PageLib offsetLib is nullptr\n";
        return;
    }
    for ( std::pair<const std::size_t, std::pair<std::size_t, std::size_t>> & pair_offset : _offsetLib ) {
        std::cout << pair_offset.first << " " << pair_offset.second.first << " " << pair_offset.second.second << "\r\n";
    }
}