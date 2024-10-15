/*************************************************************************
    > File Name: test_simhash.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月14日 星期一 22时47分35秒
    > Mail: JiaZiChunQiu@163.com
    > Title:
    > Content:
 ************************************************************************/

#include <iostream>
#include <fstream>

#define LOGGER_LEVEL LL_WARN 

#include "../simhash/Simhasher.hpp"

using namespace simhash;

int main(int argc, char** argv) {
    // 构造simhasher对象，注意尽量只构造一次
    Simhasher simhasher("../dict/jieba.dict.utf8", "../dict/hmm_model.utf8", "../dict/idf.utf8", "../dict/stop_words.utf8");
    string s("我是蓝翔技工拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上总经理，出任CEO，走上人生巅峰。");
    size_t topN = 5; // topN 表示特征的数量
    uint64_t u64 = 0; // u64 用来保存指纹
    simhasher.make(s, topN, u64);
    cout<< "simhash值是: " << u64 << endl;

    string s2("我是蓝翔技工拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上总经理，出任CEO，走上人生巅峰。");
    uint64_t u64_2 = 0;
    simhasher.make(s2, topN, u64_2);
    cout<< "simhash值是: " << u64_2 << endl;

    cout<< "海明距离阈值默认设置为3，则isEqual结果为：" << (Simhasher::isEqual(u64, u64_2)) << endl; 
    

    const char * bin1 = "100010110110";
    const char * bin2 = "110001110011";
    uint64_t u1, u2;
    u1 = Simhasher::binaryStringToUint64(bin1); 
    u2 = Simhasher::binaryStringToUint64(bin2); 
    
    cout<< bin1 << "和" << bin2 << " simhash值的相等判断如下："<<endl;
    cout<< "海明距离阈值默认设置为3，则isEqual结果为：" << (Simhasher::isEqual(u1, u2)) << endl; 
    cout<< "海明距离阈值默认设置为5，则isEqual结果为：" << (Simhasher::isEqual(u1, u2, 5)) << endl; 
    return EXIT_SUCCESS;
}

