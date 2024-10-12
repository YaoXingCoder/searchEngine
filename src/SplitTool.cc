/*************************************************************************
    > File Name: SplitTool.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月11日 星期五 21时34分53秒
    > Mail: JiaZiChunQiu@163.com
    > Title: SplitTool 工具集
    > Content:
    >   实现派生类Jieba工具, 能够对中文句子进行拆分
 ************************************************************************/

#include "SplitTool.h"

SplitToolCppJieba::SplitToolCppJieba(const std::string& dict_path, 
        const std::string& model_path,
        const std::string& user_dict_path, 
        const std::string& idfPath, 
        const std::string& stopWordPath,
        bool hmm)
: _jieba(dict_path, model_path, user_dict_path, idfPath, stopWordPath)
, _hmm(hmm)
{}

SplitToolCppJieba::~SplitToolCppJieba() { }

std::vector<std::string> SplitToolCppJieba::cut(const std::string & sentence) {
    std::vector<std::string> words;
    _jieba.Cut(sentence, words, _hmm);
    return words;
}