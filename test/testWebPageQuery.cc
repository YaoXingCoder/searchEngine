/*************************************************************************
    > File Name: testWebPageQuery.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月21日 星期一 14时02分11秒
    > Mail: JiaZiChunQiu@163.com
    > Title: 查询测试
    > Content:
 ************************************************************************/

#include "SplitTool.h"

#include "WebPageQuery.h"

#include <iostream>
#include <memory>

/* =============== test =============== */
void test0() {
    WebPageQuery webPageQuery(std::shared_ptr<SplitTool>(new SplitToolCppJieba()));
    // webPageQuery.doQuery(
    //     "图为位于深圳的前海深港青年梦工厂。不少香港青年在这里实现创新创业梦想。　　（资料图片）　　作为2020年香港特区政"
    //     "府施政报告中推出的重点措施之一，香港“大湾区青年就业计划”自今年1月初接受申请以来获得热烈反响。记者从香港特区政"
    //     "府新闻网了解到，截至4月30日，“大湾区青年就业计划”共收到321家企业提供的2394个职位空缺，多于原本预计的2000个，其"
    //     "中1267个是创新科技类职位，也多于预计的700个。香港特区行政长官林郑月娥呼吁香港青年把握机遇，踊跃申请，在积极融"
    //     "入国家发展大局的同时，为人生开拓更广阔出路。　　　　找到合适位置　　如何搭上大湾区发展的高速列车，找到一个合适"
    //     "的位置，是当下香港年轻人思考的问题。“香港社会一些年轻人，对于内地的发展不是完全了解，对于大湾区生活就业情况，"
    //     "他们也不太掌握。所以怎么能吸引他们到大湾区内地城市工作是一个挑战。”林郑月娥透露，推出“大湾区青年就业计划”的初"
    //     "衷，是希望鼓励青年人参与大湾区建设，把握近水楼台的大好机遇，为自己、为香港谋求更好的发展，同时亲身体验内地的发"
    //     "展，深入了解内地的生活和文化。　　该计划的运作方式，是请在香港和大湾区内地城市均有业务的企业，聘请香港的大学毕"
    //     "业生，派驻他们到大湾区内地城市工作，每月薪资不少于1."
    //     "8万港元。港府会补贴参与计划的企业每人每月1万港元，最长18个月，最多共补贴3."
    //     "6亿港元。林郑月娥在接受媒体访问时说，她觉得特区政府“这个钱花得很有价值”。“让香港青年在内地慢慢住下来，了解内地"
    //     "，交一些朋友，认可在大湾区就业是一个好选择。");

    std::string webPage = webPageQuery.doQueryTest("大湾区青年就业计划");
    std::cout << webPage << "\n";

    /* 测试容器 */
    // webPageQuery.showPageLib();
    // webPageQuery.showOffsetLib();
    // webPageQuery.showInvertIndexLib();
}
/* =============== main =============== */
int main(int argc, char *argv[]) {
    test0();
    return 0;
}
