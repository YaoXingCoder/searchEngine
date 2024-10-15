/*************************************************************************
    > File Name: Dictionary.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月11日 星期五 20时33分53秒
    > Mail: JiaZiChunQiu@163.com
    > Title: Dictory 实现
    > Content:
    >   英文清晰:
    >       把所有的非字母、非数字转成空格
    >       大写字母转小写
 ************************************************************************/

#include "DictProducer.h"
#include "SplitTool.h"
#include "Configuration.h"

#include <fstream>
#include <cctype>
#include <sstream>
#include <algorithm> // for std::copy
#include <iterator>  // for std::back_inserter

/* 构造 */
DictProducer::DictProducer(const ConfsPath & confsPath, std::shared_ptr<SplitTool> splitTool) 
: _filesEN()
, _filesZN()
, _filesSTOP()
, _dict()
, _dict_map()
, _index()
, _dict_stop()
, _splitTool(splitTool)
{
    _dict.reserve(VECT_SIZE); // 预留空间

    // 判断配置路径是否为空
    if ( !confsPath.confPathEN.empty() ) {
        readFromConfigEN(confsPath.confPathEN);
    }
    if ( !confsPath.confPathZN.empty() ) {
        readFromConfigZN(confsPath.confPathZN);
    }
    if ( !confsPath.confPathSTOP.empty() ) {
        readFromConfigSTOP(confsPath.confPathSTOP);
    }

    buildDictStopWrods(); // 生成字典
    buildIndex(); // 生成索引
}

/* 析构 */ 
DictProducer::~DictProducer() {}

/* 生成字典是否使用停用词 */
void DictProducer::buildDictStopWrods() {
    if ( !_filesSTOP.empty() ) {
        buildStopDict();
        if (  !_filesEN.empty() ) { buildEnDict(); }
        else { std::cerr << " _filesEN is empty.\n"; }

        if ( !_filesZN.empty() ) { buildCnDict(); }
        else { std::cerr << " _filesZN is empty.\n"; }
    } else {
        std::cerr << " _filesSTOP is empty.\n";

        if (  !_filesEN.empty() ) { buildEnDictNoStop(); }
        else { std::cerr << " _filesEN is empty.\n"; }
        
        if ( !_filesZN.empty() ) { buildCnDictNoStop(); }
        else { std::cerr << " _filesZN is empty.\n"; }
    }
}

/* 英文路径读取 */
void DictProducer::readFromConfigEN(const std::string & confPathEN) {
    // 1.读取配置文件中信息
    Configuration::getInstance()->setFilePath(confPathEN);
    std::map<std::string, std::string> & map_conf = Configuration::getInstance()->getConfigMap();

    // 2.存入容器, 判断配置文件的内容是否为空
    if ( !map_conf.empty() ) {
        for ( std::pair<const std::string, std::string> & pair_kv : map_conf ) {
            _filesEN.push_back(pair_kv.second);
        }
    } else {
        std::cerr << "The File of configEN may be empty\n";
        return;
    }
}
/* 中文路径读取 */
void DictProducer::readFromConfigZN(const std::string & confPathZN){
    // 1.读取配置文件中信息
    Configuration::getInstance()->setFilePath(confPathZN);
    std::map<std::string, std::string> & map_conf = Configuration::getInstance()->getConfigMap();

    // 2.存入容器, 判断配置文件的内容是否为空
    if ( !map_conf.empty() ) {
        for ( std::pair<const std::string, std::string> & pair_kv : map_conf ) {
            _filesZN.push_back(pair_kv.second);
        }
    } else {
        std::cerr << "The File of configZN may be empty\n";
        return;
    }
}
/* 停用词路径读取 */
void DictProducer::readFromConfigSTOP(const std::string & confPathStop){
    // 1.读取配置文件中信息
    Configuration::getInstance()->setFilePath(confPathStop);
    std::map<std::string, std::string> & map_conf = Configuration::getInstance()->getConfigMap();

    // 2.存入容器, 判断配置文件的内容是否为空
    if ( !map_conf.empty() ) {
        for ( std::pair<const std::string, std::string> & pair_kv : map_conf ) {
            _filesSTOP.push_back(pair_kv.second);
        }
    } else {
        std::cerr << "The File of configSTOP may be empty\n";
        return;
    }
}

/* 
* 创建停用词词典, 存入容器
*/
void DictProducer::buildStopDict() {
    for( std::string & filePath : _filesSTOP ) {
        // 1.打开文件
        std::ifstream ifs(filePath);
        if ( !ifs ) { 
            std::cerr << "DictProducer buildStopDict() ifs error\n";
            continue;
        }

        // 2.循环读取并存储仅结构中
        std::string word;
        while ( std::getline( ifs, word) ) {
            _dict_stop.emplace(word);
        }
    }
}

/* 
* 创建英文词典 
*/
void DictProducer::dealLine(std::string & line) {
    if ( line.empty() ) { return; } // 判空
    for ( int i = 0; i < line.size(); ++i ) {
        if ( !std::isalpha(line[i]) ) {
            line[i] = ' '; // 非字符串 替换为 空格' '
        } else {
            line[i] = std::tolower(line[i]); // 转换为小写
        }
    }
}
void DictProducer::buildEnDict() {
    
    for ( std::string & filePath : _filesEN) {
        // 1.打开文件
        std::ifstream ifs(filePath);
        if ( !ifs ) { 
            std::cerr << "DictProducer buildEnDict() ifs error\n";
            continue;
        }

        // 2.循环读取并分析
        std::string line;
        while ( std::getline(ifs, line) ) {
            dealLine(line); // 2.1 清洗

            // 2.2 处理单词
            std::istringstream iss(line);
            std::string word;
            while ( iss >> word ) {
                if ( _dict_stop.count(word) == 0 ) {
                    ++_dict_map[word]; // 去重存储
                }
            }
        }

        // 3.关闭资源
        ifs.close();
    }
}
void DictProducer::buildEnDictNoStop() {
    for ( std::string & filePath : _filesEN) {
        // 1.打开文件
        std::ifstream ifs(filePath);
        if ( !ifs ) { 
            std::cerr << "DictProducer buildEnDict() ifs error\n";
            return;
        }

        // 2.循环读取并分析
        std::string line;
        while ( std::getline(ifs, line) ) {
            dealLine(line); // 2.1 清洗

            // 2.2 处理单词
            std::istringstream iss(line);
            std::string word;
            while ( iss >> word ) { ++_dict_map[word]; } // 去重存储
        }

        // 3.关闭资源
        ifs.close();
    }
}


/* 
* 创建中文词典
* 根据首字节获取单个字所长字节数
*/
size_t DictProducer::getByteNum_UTF8(const char ch) {
    int byteNum = 0;  
    for (size_t i = 0; i < 6; ++i)  {  
        if (ch & (1 << (7 - i))) ++byteNum;  
        else { break; }
    }  
    return byteNum == 0 ? 1 : byteNum;
}
void DictProducer::buildCnDict() {
    for ( std::string & filePath : _filesZN) {
        // 1.打开文件
        std::ifstream ifs(filePath);
        if ( !ifs ) { 
            std::cerr << "DictProducer buildCnDict() ifs error\n";
            continue;
        }

        // 2.读取文件所有内容, 并读取到 string
        ifs.seekg(0, std::ios::end);
        int fileSize = ifs.tellg();
        std::string fileContext(fileSize, '\0');
        ifs.seekg(0, std::ios::beg);
        ifs.read(&fileContext[0], fileSize);

        // 3.处理中文所有的\r\n
        std::basic_string<char>::size_type i = 0;
        std::string::iterator it = fileContext.begin();
        while( it < fileContext.end() ) {
            if ( *it == '\r' || *it == '\n' ) {
                it = fileContext.erase(it); // 删除字符
            } else {
                ++it;
            }
        }

        // 4.jieba 分词
        std::vector<std::string> words;
        words = _splitTool->cut(fileContext);

        // 5.去重存储, 且判断是否是汉字
        for ( std::string & word : words ) {
            if ( (_dict_stop.count(word) == 0) && (getByteNum_UTF8(word[0]) == 3) ) {
                ++_dict_map[word];
            }
        }

        // 6.关闭资源
        ifs.close();
    }
}

void DictProducer::buildCnDictNoStop() {
    for ( std::string & filePath : _filesZN) {
        // 1.打开文件
        std::ifstream ifs(filePath);
        if ( !ifs ) { 
            std::cerr << "DictProducer buildCnDict() ifs error\n";
            continue;
        }

        // 2.读取文件所有内容, 并读取到 string
        ifs.seekg(0, std::ios::end);
        int fileSize = ifs.tellg();
        std::string fileContext(fileSize, '\0');
        ifs.seekg(0, std::ios::beg);
        ifs.read(&fileContext[0], fileSize);

        // 3.处理中文所有的\r\n
        std::basic_string<char>::size_type i = 0;
        std::string::iterator it = fileContext.begin();
        while( it < fileContext.end() ) {
            if ( *it == '\r' || *it == '\n' ) {
                it = fileContext.erase(it); // 删除字符
            } else {
                ++it;
            }
        }

        // 4.jieba 分词
        std::vector<std::string> words;
        words = _splitTool->cut(fileContext);

        // 5.去重存储, 且判断是否是汉字
        for ( std::string & word : words ) {
            if ( getByteNum_UTF8(word[0]) == 3 ) { ++_dict_map[word]; }
        }

        // 6.关闭资源
        ifs.close();
    }
}

/* 
* 创建索引
* 遍历 _dict, 同时记录下标
*/
void DictProducer::buildIndex() {
    int i = 0; // 记录行号, 也是下标
    for ( std::pair<const std::string, int> & pair : _dict_map ) {
        std::string word = pair.first;
        size_t charNums = word.size()/getByteNum_UTF8(word[0]); // 计算出 word 中有多少个单字/字母
        /* 根据单字数, 遍历该单词 */
        for ( size_t idx = 0, n = 0; n != charNums; ++n  ) {
            size_t byteNum = getByteNum_UTF8(word[idx]); // 获取单字的 字节数
            std::string subWord = word.substr(idx, byteNum); // 切割: 根据字节数切割获取 单字

            _index[subWord].insert(i); // 索引结构中插入, 并将行号记录其中, i是该单词/汉字的行号
            idx += byteNum; // 光标移动到下个单字
        }
        ++i;
    }
}


/* 
* 存储
*/
void DictProducer::storeDict(const std::string & filePath) {
    // 打开文件
    std::ofstream ofs(filePath);
    if ( !ofs ) {
        std::cerr << "DictProducer storeDict() ifs error\n";
        return;
    }

    // 遍历 _dict_map 存储
    for ( std::pair<const std::string, int> & pair : _dict_map ) {
        ofs << pair.first << ":" << pair.second << "\r\n";
    }

    // 关闭资源
    ofs.close();
}


void DictProducer::storeIndex(const std::string & filePath) {
    // 打开文件
    std::ofstream ofs(filePath);
    if ( !ofs ) {
        std::cerr << "DictProducer storeDict() ifs error\n";
        return;
    }

    // 遍历 _dict_map 存储
    for ( std::pair<const std::string, std::set<int>> & pair : _index ) {
        ofs << pair.first << ":";
        for ( int idx : pair.second ) { 
            ofs << idx << " ";
        }
        ofs << "\r\n";
    }

    // 关闭资源
    ofs.close();
}

void DictProducer::pushDict(const std::string & word) {

}

/* 将临时的 map 存储到 vector */
void DictProducer::storeVecFromMap() {
    for ( std::pair<const std::string, int> & pair : _dict_map ) {
        _dict.push_back(pair);
    }
}

/* 测试 : files 绝对文件路径 */
void DictProducer::showFiles() const {
    if ( !_filesEN.empty() ) {
        for (std::string path : _filesEN) {
            std::cout << "EN file path is " << path << "\n";
        }
    }
    if (  !_filesZN.empty() ) {
        for (std::string path : _filesZN) {
            std::cout << "ZN file path is " << path << "\n";
        }
    }
    if ( !_filesSTOP.empty() ) {
        for (std::string path : _filesSTOP) {
            std::cout << "STOP file path is " << path << "\n";
        }
    }
}

/* 测试 : 打印词典 */
void DictProducer::showDict() const {
    for (std::pair<const std::string, int> pair : _dict_map ) {
        std::cout << "word is " << pair.first << ", frequency is " << pair.second << "\n";
    }
}

/* 测试 : 打印索引 */
void DictProducer::showIndex() const {
    for (std::pair<const std::string, std::set<int>> pair : _index ) {
        std::cout << "word is " << pair.first << ", index is " << pair.second << "\n";
    }
}