# 编译链接选项
COM_OP := -g -I include 


# 编译 : 目标文件生成
# 模块一
bin/SplitTool.o : src/SplitTool.cc include/SplitTool.h
	$(CXX) -c src/SplitTool.cc -o $@ $(COM_OP)
bin/Configuration.o : src/Configuration.cc include/Configuration.h
	$(CXX) -c src/Configuration.cc -o $@ $(COM_OP)
bin/DictProducer.o : src/DictProducer.cc include/DictProducer.h
	$(CXX) -c src/DictProducer.cc -o $@ $(COM_OP)
# 模块二
bin/DirScanner.o : src/DirScanner.cc include/DirScanner.h
	$(CXX) -c src/DirScanner.cc -o $@ $(COM_OP)
bin/RssReader.o : src/RssReader.cc include/RssReader.h 
	$(CXX) -c src/RssReader.cc -o $@ $(COM_OP)
bin/PageLib.o : src/PageLib.cc include/PageLib.h
	$(CXX) -c src/PageLib.cc -o $@ $(COM_OP)
bin/WebPage.o : src/WebPage.cc include/WebPage.h
	$(CXX) -c src/WebPage.cc -o $@ $(COM_OP)


# 测试 链接目标文件
# 模块一
SPLITTOOL := testSplitTool # 中文分词工具测试
CONFIGURATION := testConfiguration # 配置类测试
DIRSCANNER := testDirScanner # 目录扫描类测试
DICTPRODUCER := testDictProducer # 语料相关测试

$(SPLITTOOL) : src/testSplitTool.cc bin/SplitTool.o
	$(CXX) src/testSplitTool.cc bin/SplitTool.o -o $@ $(COM_OP)

$(CONFIGURATION) : src/testConfiguration.cc bin/Configuration.o
	$(CXX) src/testConfiguration.cc bin/Configuration.o -o $@ $(COM_OP)

$(DICTPRODUCER) : src/testDictProducer.cc bin/DictProducer.o bin/Configuration.o bin/SplitTool.o
	$(CXX) src/testDictProducer.cc bin/DictProducer.o bin/Configuration.o bin/SplitTool.o -o $@ $(COM_OP)

# 模块二
RSSREADER := testRssReader # xml文件解析测试
PAGELIB := testPageLib # 网页库测试
WEBPAGE := testWebPage # 网页解析测试

$(DIRSCANNER) : src/testDirScanner.cc bin/DirScanner.o bin/Configuration.o
	$(CXX) src/testDirScanner.cc bin/DirScanner.o bin/Configuration.o -o $@ $(COM_OP)

$(RSSREADER) : src/testRssReader.cc bin/RssReader.o # 用到 -tinyxml2 库
	$(CXX) src/testRssReader.cc bin/RssReader.o -o $@ $(COM_OP) -ltinyxml2

$(PAGELIB) : src/testPageLib.cc bin/PageLib.o bin/Configuration.o bin/DirScanner.o bin/RssReader.o # 用到 -tinyxml2 库
	$(CXX) src/testPageLib.cc bin/PageLib.o bin/Configuration.o bin/DirScanner.o bin/RssReader.o -o $@ $(COM_OP) -ltinyxml2

$(WEBPAGE) : src/testWebPage.cc bin/WebPage.o bin/Configuration.o bin/SplitTool.o
	$(CXX) src/testWebPage.cc bin/WebPage.o bin/Configuration.o bin/SplitTool.o -o $@ $(COM_OP) -ltinyxml2


# 清除文件
cleanAll: cleanTest clean
cleanTest:
	-rm -rf $(SPLITTOOL) $(CONFIGURATION) $(DICTPRODUCER) $(DIRSCANNER) $(RSSREADER) $(PAGELIB) $(WEBPAGE)
clean :
	rm -rf bin/SplitTool.o bin/Configuration.o bin/DictProducer.o bin/DirScanner.o bin/RssReader.o bin/PageLib.o bin/WebPage.o
