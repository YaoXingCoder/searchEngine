Server := SearchEngine

# 目标文件生成
bin/SplitTool.o : src/SplitTool.cc include/SplitTool.h
	g++ -c src/SplitTool.cc -o $@ -g -I include
bin/Configuration.o : src/Configuration.cc include/Configuration.h
	g++ -c src/Configuration.cc -o $@ -g -I include
bin/DirScanner.o : src/DirScanner.cc include/DirScanner.h
	g++ -c src/DirScanner.cc -o $@ -g -I include
bin/DictProducer.o : src/DictProducer.cc include/DictProducer.h
	g++ -c src/DictProducer.cc -o $@ -g -I include

# 测试文件生成

# 测试使用
SPLITTOOL := testSplitTool # 中文分词工具测试
CONFIGURATION := testConfiguration # 配置类测试
DIRSCANNER := testDirScanner # 目录扫描类测试
DICTPRODUCER := testDictProducer # 语料相关测试

$(SPLITTOOL) : src/testSplitTool.cc bin/SplitTool.o
	g++ src/testSplitTool.cc bin/SplitTool.o -o $@ -g -I include

$(CONFIGURATION) : src/testConfiguration.cc bin/Configuration.o
	g++ src/testConfiguration.cc bin/Configuration.o -o $@ -g -I include

$(DIRSCANNER) : src/testDirScanner.cc bin/DirScanner.o bin/Configuration.o
	g++ src/testDirScanner.cc bin/DirScanner.o bin/Configuration.o -o $@ -g -I include

$(DICTPRODUCER) : src/testDictProducer.cc bin/DictProducer.o bin/Configuration.o bin/SplitTool.o
	g++ src/testDictProducer.cc bin/DictProducer.o bin/Configuration.o bin/SplitTool.o -o $@ -g -I include

# 清除文件
cleanAll: cleanTest clean

cleanTest:
	-rm -rf $(SPLITTOOL) $(CONFIGURATION) $(DIRSCANNER)

clean :
	rm -rf bin/SplitTool.o bin/Configuration.o bin/DirScanner.o
