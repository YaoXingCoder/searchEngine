# 编译链接选项
COM_OP := -g -I include
Server := Server # 服务器

# 服务器启动
# bin/WebPageSearcher.o bin/WebPageQuery.o   bin/RssReader.o bin/HttpRequestParser.o
$(Server) : src/Server.cc \
			bin/EchoServer.o \
			bin/WebPageQuery.o bin/WebPage.o bin/Configuration.o bin/SplitTool.o \
			bin/KeyRecommander.o bin/Dictionary.o \
			bin/TcpServer.o bin/Eventloop.o bin/TcpConnection.o bin/Acceptor.o \
			bin/SocketIO.o bin/InetAddress.o bin/Socket.o \
			bin/ThreadPool.o bin/TaskQueue.o
	$(CXX) $^ -o $@ $(COM_OP) -ltinyxml2

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
bin/PageLibPreprocessor.o : src/PageLibPreprocessor.cc include/PageLibPreprocessor.h
	$(CXX) -c src/PageLibPreprocessor.cc -o $@ $(COM_OP)
# 模块三 服务端事务回调
bin/Socket.o : src/Socket.cc include/Socket.h
	$(CXX) -c src/Socket.cc -o $@ $(COM_OP)
bin/InetAddress.o : src/InetAddress.cc include/InetAddress.h
	$(CXX) -c src/InetAddress.cc -o $@ $(COM_OP)
bin/SocketIO.o : src/SocketIO.cc include/SocketIO.h
	$(CXX) -c src/SocketIO.cc -o $@ $(COM_OP)
bin/Acceptor.o : src/Acceptor.cc include/Acceptor.h
	$(CXX) -c src/Acceptor.cc -o $@ $(COM_OP)
bin/TcpConnection.o : src/TcpConnection.cc include/TcpConnection.h
	$(CXX) -c src/TcpConnection.cc -o $@ $(COM_OP)
bin/Eventloop.o : src/Eventloop.cc include/Eventloop.h
	$(CXX) -c src/Eventloop.cc -o $@ $(COM_OP)
bin/TcpServer.o : src/TcpServer.cc include/TcpServer.h
	$(CXX) -c src/TcpServer.cc -o $@ $(COM_OP)
# 线程池
bin/TaskQueue.o : src/TaskQueue.cc include/TaskQueue.h
	$(CXX) -c src/TaskQueue.cc -o $@ $(COM_OP)
bin/ThreadPool.o : src/ThreadPool.cc include/ThreadPool.h
	$(CXX) -c src/ThreadPool.cc -o $@ $(COM_OP)
# 封装线程池与服务端
# 候选词
bin/Dictionary.o : src/Dictionary.cc include/Dictionary.h
	$(CXX) -c src/Dictionary.cc -o $@ $(COM_OP)
bin/KeyRecommander.o : src/KeyRecommander.cc include/KeyRecommander.h
	$(CXX) -c src/KeyRecommander.cc -o $@ $(COM_OP)
# 网页查询
bin/WebPageQuery.o : src/WebPageQuery.cc include/WebPageQuery.h
	$(CXX) -c src/WebPageQuery.cc -o $@ $(COM_OP)

# bin/http_parser.o : src/http_parser.c include/http_parser.h
# 	$(CXX) -c src/http_parser.c -o $@ $(COM_OP)
# bin/HttpRequestParser.o : src/HttpRequestParser.cc include/HttpRequestParser.h
# 	$(CXX) -c src/HttpRequestParser.cc -o $@ $(COM_OP)

# bin/WebPageSearcher.o : src/WebPageSearcher.cc include/WebPageSearcher.h
# 	$(CXX) -c src/WebPageSearcher.cc -o $@ $(COM_OP)

bin/EchoServer.o : src/EchoServer.cc include/EchoServer.h
	$(CXX) -c src/EchoServer.cc -o $@ $(COM_OP)


# 测试 链接目标文件
# 模块一
SPLITTOOL := testSplitTool # 中文分词工具测试
CONFIGURATION := testConfiguration # 配置类测试
DIRSCANNER := testDirScanner # 目录扫描类测试
DICTPRODUCER := testDictProducer # 语料相关测试

$(SPLITTOOL) : test/testSplitTool.cc bin/SplitTool.o
	$(CXX) test/testSplitTool.cc bin/SplitTool.o -o $@ $(COM_OP)

$(CONFIGURATION) : test/testConfiguration.cc bin/Configuration.o
	$(CXX) test/testConfiguration.cc bin/Configuration.o -o $@ $(COM_OP)

$(DICTPRODUCER) : test/testDictProducer.cc bin/DictProducer.o bin/Configuration.o bin/SplitTool.o
	$(CXX) test/testDictProducer.cc bin/DictProducer.o bin/Configuration.o bin/SplitTool.o -o $@ $(COM_OP)

# 模块二
RSSREADER := testRssReader # xml文件解析测试
PAGELIB := testPageLib # 网页库测试
WEBPAGE := testWebPage # 网页解析测试
PAGELIBPREPROCESSOR := testPageLibPreprocessor # 网页预处理测试

$(DIRSCANNER) : test/testDirScanner.cc bin/DirScanner.o bin/Configuration.o
	$(CXX) test/testDirScanner.cc bin/DirScanner.o bin/Configuration.o -o $@ $(COM_OP)

$(RSSREADER) : test/testRssReader.cc bin/RssReader.o # 用到 -tinyxml2 库
	$(CXX) test/testRssReader.cc bin/RssReader.o -o $@ $(COM_OP) -ltinyxml2

$(PAGELIB) : test/testPageLib.cc bin/PageLib.o bin/Configuration.o bin/DirScanner.o bin/RssReader.o # 用到 -tinyxml2 库
	$(CXX) test/testPageLib.cc bin/PageLib.o bin/Configuration.o bin/DirScanner.o bin/RssReader.o -o $@ $(COM_OP) -ltinyxml2

$(WEBPAGE) : test/testWebPage.cc bin/WebPage.o bin/Configuration.o bin/SplitTool.o
	$(CXX) test/testWebPage.cc bin/WebPage.o bin/Configuration.o bin/SplitTool.o -o $@ $(COM_OP) -ltinyxml2

$(PAGELIBPREPROCESSOR) : test/testPageLibPreprocessor.cc bin/PageLibPreprocessor.o bin/WebPage.o bin/SplitTool.o bin/Configuration.o
	$(CXX) test/testPageLibPreprocessor.cc bin/PageLibPreprocessor.o bin/WebPage.o bin/SplitTool.o bin/Configuration.o -o $@ $(COM_OP) -ltinyxml2

# 在线
DICTIONARY := testDictionary # 词典加载
WEBPAGEQUERY := testWebPageQuery # 网页加载, 处理

$(DICTIONARY) : test/testDictionary.cc bin/Dictionary.o
	$(CXX) test/testDictionary.cc bin/Dictionary.o -o $@ $(COM_OP)

$(WEBPAGEQUERY) : test/testWebPageQuery.cc bin/WebPageQuery.o bin/WebPage.o bin/Configuration.o bin/SplitTool.o
	$(CXX) test/testWebPageQuery.cc bin/WebPageQuery.o bin/WebPage.o bin/Configuration.o bin/SplitTool.o -o $@ $(COM_OP) -ltinyxml2

# 清除文件
cleanAll: cleanTest clean cleanServer
# 服务器
cleanServer: 
	-rm -rf $(Server) \
			bin/EchoServer.o \
			bin/WebPageQuery.o bin/WebPage.o bin/Configuration.o bin/SplitTool.o \
			bin/KeyRecommander.o bin/Dictionary.o \
			bin/TcpServer.o bin/Eventloop.o bin/TcpConnection.o bin/Acceptor.o \
			bin/SocketIO.o bin/InetAddress.o bin/Socket.o \
			bin/ThreadPool.o bin/TaskQueue.o
# 测试文件
cleanTest:
	-rm -rf $(SPLITTOOL) $(CONFIGURATION) $(DICTPRODUCER) $(DIRSCANNER) $(RSSREADER) $(PAGELIB) $(WEBPAGE) $(PAGELIBPREPROCESSOR) \
			$(DICTIONARY) $(WEBPAGEQUERY)
# 模块1/2
clean :
	rm -rf bin/SplitTool.o bin/Configuration.o bin/DictProducer.o \
	bin/DirScanner.o bin/RssReader.o bin/PageLib.o bin/WebPage.o bin/PageLibPreprocessor.o
