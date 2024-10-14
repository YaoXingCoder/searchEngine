/*************************************************************************
    > File Name: testRss.cc
    > Author: JiaZiChunQiu
    > Created Time: 2024年10月13日 星期日 23时15分00秒
    > Mail: JiaZiChunQiu@163.com
    > Title:
    > Content:
 ************************************************************************/

#include <string>
#include <iostream>
#include <tinyxml2.h>

#define DOC "<doc>  \
                <docid>1</docid>    \
                <title>香港迪士尼2020财政年度灵活营运 积极应对疫情</title>  \
                <link>http:hm.people.com.cn/n1/2021/0518/c42272-32106704.html</link>    \
                <content> 人民网香港5月18日电（记者陈然）香港迪士尼乐园度假区17日公布2020财政年度业绩时表示，面对严峻的新冠肺炎疫情，乐园在整个财年约有60%的时间处于关闭状态，酒店亦仅维持有限度服务；但乐园团队以灵活的营运方式积极应变，在全员“保就业”的同时，乐园年票会员人数更上涨8%，创下历史新高。  香港迪士尼乐园度假区行政总裁莫伟庭当日以视像方式举行记者会。他表示，基于疫情等不利条件，香港迪士尼业绩受到严重打击。截至2020年9月28日年结日，香港迪士尼全年收入下跌76%至14.4亿港元，净亏损达26.6亿港元。  “即使面对重重挑战，乐园仍能灵活应变，以创新方式增加收入，竭力为逾7000名演艺人员‘保就业’。”莫伟庭强调，香港迪士尼未有且无意作出任何裁员行动，管理团队亦将密切评估及管理营运，审慎理财。  据香港迪士尼业绩报告数字显示，2020财政年度乐园入场人次约为170万，较上一年度下跌73%；自2005年开幕以来的累计总入场人次则已超过8500万。  莫伟庭表示，疫情下，乐园度假区着重聚焦香港本地市场，以连串优惠及崭新的游园体验吸引本地宾客光临。在乐园重开后，本地宾客反应正面，入场人次持续回升，乐园年票“奇妙处处通”会员人数更创下2011年该计划推出以来的历史新高，会员人均消费亦录得同比增长。  展望前景，莫伟庭对香港迪士尼的未来感到乐观。作为乐园的全新地标，“奇妙梦想城堡”已于2020年底华丽揭幕，而以新城堡为背景、精心编排的“迪士尼寻梦奇缘”舞台汇演将于今年6月30日隆重登场，届时，汇演将融合迪士尼最有趣、最具人气的角色和故事，带领宾客展开一场有趣、欢乐的音乐梦幻之旅。 </content>  \
            </doc>"

/* =============== test =============== */
void test0() {
    std::string doc(DOC);

    // 1.创建对象
    tinyxml2::XMLDocument parseStr;
    tinyxml2::XMLError error =  parseStr.Parse(doc.c_str(), doc.size());
    if ( error != tinyxml2::XMLError::XML_SUCCESS ) {
        std::cerr << "RssReader parseRss LoadFile error\n";
        std::cerr << "ERROR : " << parseStr.ErrorStr() << "\n";
        return;
    }

    // 3.定位节点 item : item 下有 title link description/content
    tinyxml2::XMLElement * item = parseStr.RootElement();

    // 4 docid 
    tinyxml2::XMLElement * element = item->FirstChildElement("docid");
    int docid = std::stoi(element->GetText());
    // 4 tilte
    element = item->FirstChildElement("title");
    std::string title = element->GetText();
    // 4 link
    element = item->FirstChildElement("link");
    std::string link = element->GetText();
    // 4 content
    element = item->FirstChildElement("content");
    std::string content = element->GetText();

    // 5.输出
    std::cout << docid << "\n";
    std::cout << title << "\n";
    std::cout << link << "\n";
    std::cout << content << "\n";
}
/* =============== main =============== */
int main (int argc, char* argv[]) {
    test0();
    return 0;
}
