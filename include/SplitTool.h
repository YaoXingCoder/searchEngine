#ifndef __SPLIT_TOOL_H__
#define __SPLIT_TOOL_H__

#include <vector>
#include <string>

#include "../simhash/cppjieba/Jieba.hpp"

const char* const DICT_PATH = "./dict/jieba.dict.utf8";
const char* const HMM_PATH = "./dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "./dict/user.dict.utf8";
const char* const IDF_PATH = "./dict/idf.utf8";
const char* const STOP_WORD_PATH = "./dict/stop_words.utf8";

/* 
    抽象类
    处理字符的工具类的基类
 */
class SplitTool {
public:
    virtual std::vector<std::string> cut(const std::string & sentence) = 0;
    virtual ~SplitTool() {};
};

/*
    派生类
*/
class SplitToolCppJieba : public SplitTool {
public:
    SplitToolCppJieba(const std::string& dict_path = DICT_PATH, 
        const std::string& model_path = HMM_PATH,
        const std::string& user_dict_path = USER_DICT_PATH, 
        const std::string& idfPath = IDF_PATH, 
        const std::string& stopWordPath = STOP_WORD_PATH,
        bool hmm = true);
    ~SplitToolCppJieba();
public:
    std::vector<std::string> cut(const std::string & sentence);
private:
    cppjieba::Jieba _jieba; // jieba 对象
    bool _hmm;
};

#endif
