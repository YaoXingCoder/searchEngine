/*************************************************************************
    > File Name: tcp_server_select.c
    > Author: JiaZiChunQiu
    > Created Time: 2024年07月24日 星期三 16.11.02
    > Mail: JiaZiChunQiu@163.com
    > Title: 使用epoll实现聊天室功能, 超时未发送消息用户态踢出聊天室
    > Content:
    >   1.socket创建套接字 -> 创建IP地址结构体 -> bind绑定ip与套接字 -> listen转套接字为监听 -> send/recv 接发消息 ->
 close关闭套接字 >   2.添加用户态缓冲, 分别用于接收消息, 发送消息 >   3.初始化监听集合, 将 STDIN_FILENO, recv
 加入监听集合 >   4.使用循环初始化读就绪, 并反复接收 select 返回的读就绪集合 >   5.注意阻塞点: accept, select,
 STDIN_FILENO, recv, send(写就绪, 一般不需要)

    >   6.实现一对多的回复(回显服务), 用户发送消息, 服务器自动回复
    >   7.服务器能够同时对多个用户回复

    >   8.注册epoll的文件描述符, 创建epoll_event结构体, 其中对应值为读事件和fd指向socketfd
    >   9.注册监听事件, 更新epfd中的红黑树, 传输socketfd和event结构体
    >   10.创建epoll_event数组在用户态, 用于epoll_wait传出就绪队列
    >   11.循环中使用epoll_wait持续监听事件, 有就绪队列则返回数量, 并循环遍历用户态的eventsArr结构体数组
    >   12.如果就绪队列中有==socketfd的值, 表示有新客户端连接, 需要判断并accept接收生成新的套接字, 放入epoll当中
    >   13.就绪队列中!=socketfd的为读/写事件, 判断后执行相应逻辑
    >   14.读就绪中如果客户端关闭, 则recv会持续读到0, 判断并执行epoll_ctl对红黑树更新, close关闭套接字

    >   15.添加对每个连接的结构体, 添加最后一次访问时间, 如果与当前时间相差超过一定时间, 则提出聊天室

    >   16.添加结构体, 关联时间戳和套接字, 每次 epoll 后都需要遍历该容器, >10s未发送消息提出
 ************************************************************************/

// #include "http_parser.h"
#include "HttpRequestParser.h"

#include <my_header.h>
#include <stdlib.h>
// socket
#include <sys/socket.h>
// struct sockaddr_in, inet_ntoa
#include <arpa/inet.h>
#include <netinet/in.h>
// setsockopt
#include <sys/types.h>
// epoll
#include <sys/epoll.h>
// close
#include <unistd.h>
// fcntl
#include <fcntl.h>
// time
#include <time.h>

#include <iostream>
#include <map>
#include <string>

/* 每个客户端套接字的结构体, 用于判断超时  */
typedef struct st_conn_info {
    int peerfd;
    time_t lastTime;
} CONN_INFO_T;

#define MAX_EVENTS 100
#define MAX_LINE 1024

// static http_parser *parser; // 申请http_parser大小的内存作为当前请求的parser对象，里面包含了对这次请求的解析信息

// /* 信息接收开始 */
// int on_message_begin(http_parser *_) {
//     (void)_;
//     printf("\n***MESSAGE BEGIN***\n\n");
//     return 0;
// }
// /* 报文头接收完成 */
// int on_headers_complete(http_parser *_) {
//     (void)_;
//     printf("\n***HEADERS COMPLETE***\n\n");
//     return 0;
// }
// /* 消息接收结束 */
// int on_message_complete(http_parser *_) {
//     (void)_;
//     printf("\n***MESSAGE COMPLETE***\n\n");
//     return 0;
// }
// /* 接收到的 url 数据 */
// int on_url(http_parser *_, const char *at, size_t length) {
//     (void)_;
//     printf("Url: %.*s\n", (int)length, at);
//     return 0;
// }
// /* 接收到的 报文头 字段key值 */
// int on_header_field(http_parser *_, const char *at, size_t length) {
//     (void)_;
//     printf("Header field: %.*s\n", (int)length, at);
//     return 0;
// }
// /* 接收到的 报文头 字段value数据 */
// int on_header_value(http_parser *_, const char *at, size_t length) {
//     (void)_;
//     printf("Header value: %.*s\n", (int)length, at);
//     return 0;
// }
// /* 接收到的消息体数据 */
// int on_body(http_parser *_, const char *at, size_t length) {
//     (void)_;
//     printf("Body: %.*s\n", (int)length, at);
//     return 0;
// }

// void parseHttp(char *buf, int len) {
//     http_parser httpParser;
//     http_parser_settings parser_set;

//     // http_parser 的回调函数，需要获取 HEADER 后者 BODY 信息，可以在这里面处理。
//     parser_set.on_message_begin = on_message_begin;
//     parser_set.on_header_field = on_header_field;
//     parser_set.on_header_value = on_header_value;
//     parser_set.on_url = on_url;
//     parser_set.on_body = on_body;
//     parser_set.on_headers_complete = on_headers_complete;
//     parser_set.on_message_complete = on_message_complete;

//     size_t parsed;
//     parser = (http_parser *)malloc(sizeof(http_parser)); // 分配一个http_parser

//     http_parser_init(parser, HTTP_REQUEST);                              // 初始化parser为Request类型
//     parsed = http_parser_execute(parser, &parser_set, buf, strlen(buf)); // 执行解析过程

//     // http_parser_execute(parser, &parser_set, buf, 0); // 信息读取完毕

//     free(parser);
//     parser = NULL;
// }

/* ----------------------------------------------------------------------- */

int main(int argc, char *argv[]) {
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(listenfd, -1, "socket");

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8888);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    printf("serverAddr is %s\n", inet_ntoa(serverAddr.sin_addr));

    int on = 1;
    int ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    ERROR_CHECK(ret, -1, "setsockopt");

    ret = bind(listenfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    ERROR_CHECK(ret, -1, "bind");

    ret = listen(listenfd, MAX_EVENTS);
    ERROR_CHECK(ret, -1, "listen");

    char recvLine[MAX_LINE] = {0};
    char sendLine[MAX_LINE] = {0};

    int epfd = epoll_create1(0);
    printf("epfd is %d\n", epfd);
    ERROR_CHECK(epfd, -1, "epoll_create1");
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = listenfd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);
    ERROR_CHECK(ret, -1, "epoll_ctl");

    struct epoll_event eventArr[MAX_EVENTS] = {0};
    int nready = 0;

    CONN_INFO_T conns[MAX_EVENTS] = {0};

    while (1) {
        nready = epoll_wait(epfd, eventArr, MAX_EVENTS, 5000);
        // if (nready == 0) { printf("Monitoring timeout"); }
        ERROR_CHECK(nready, -1, "epoll_wait");

        for (int i = 0; i < nready; ++i) {
            int rdfd = eventArr[i].data.fd;
            printf("rdfd is %d\n", rdfd);

            if (rdfd == listenfd) {
                struct sockaddr_in clientAddr;
                socklen_t len_clientAddr = sizeof(clientAddr);

                int peerfd = accept(rdfd, (struct sockaddr *)&clientAddr, &len_clientAddr);
                ERROR_CHECK(peerfd, -1, "accept");
                printf("peerfd is %d for client %s:%d has connected.\n", peerfd, inet_ntoa(clientAddr.sin_addr),
                       ntohs(clientAddr.sin_port));

                int flags = fcntl(peerfd, F_GETFL, 0);
                flags |= O_NONBLOCK;
                fcntl(peerfd, F_SETFL, flags);

                // ev.events = EPOLLIN | EPOLLET;
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = peerfd;
                ret = epoll_ctl(epfd, EPOLL_CTL_ADD, peerfd, &ev);
                ERROR_CHECK(ret, -1, "accept epoll_ctl");

                // 新的套接字, 添加到conns数组中
                for (int j = 0; j < MAX_EVENTS; ++j) {
                    if (conns[j].peerfd == 0) {
                        conns[j].peerfd = peerfd;
                        conns[j].lastTime = time(NULL);
                        break;
                    }
                }
            } else {
                if (eventArr[i].events & EPOLLIN) {
                    printf("Handle Read Event\n");
                    memset(recvLine, 0, MAX_LINE);
                    int recvNum = recv(rdfd, recvLine, MAX_LINE, MSG_PEEK);
                    ERROR_CHECK(recvNum, -1, "recv");

                    if (recvNum == 0) {
                        printf("client %d is closed\n", rdfd);
                        ret = epoll_ctl(epfd, EPOLL_CTL_DEL, rdfd, NULL);
                        close(rdfd);

                        for (int j = 0; j < MAX_EVENTS; ++j) {
                            if (conns[j].peerfd == rdfd) {
                                conns[j].peerfd = 0;
                                conns[j].lastTime = 0;
                                break;
                            }
                        }

                        continue;
                    } else if (recvNum > 20) {
                        recvNum = recv(rdfd, recvLine, MAX_LINE, 0);
                        std::string request(recvLine);
                        HttpRequestParser httpRequestParser(request);
                        if (httpRequestParser.isParsed()) {
                            printf("url: [%s] \n", httpRequestParser.getUrl().c_str());
                            printf("status: [%s] \n", httpRequestParser.getStatus().c_str());
                            for (std::pair<const std::string, std::string> &pair : httpRequestParser.getHeadField()) {
                                std::cout << pair.first << " : " << pair.second << "\n";
                            }
                            std::cout << "Body is " << httpRequestParser.getBody() << "\n";
                        }
                        // parseHttp(recvLine, strlen(recvLine));
                        /* printf("client %d said is %s\n", rdfd, recvLine); */
                        for (int j = 0; j < MAX_EVENTS; ++j) {
                            if (conns[j].peerfd != 0 && conns[j].peerfd != rdfd) {
                                int sendNum = send(conns[j].peerfd, recvLine, recvNum, 0);
                                ERROR_CHECK(sendNum, -1, "send");
                            }
                        }

                        for (int j = 0; j < MAX_EVENTS; ++j) {
                            if (conns[j].peerfd != 0 && conns[j].peerfd == rdfd) {
                                conns[j].lastTime = time(NULL);
                                break;
                            }
                        }
                    }
                }
            }
        }
        // 每次 epoll 就绪后, 遍历存放 fd 和 时间戳 的容器, 比较时间戳大于 10s 提出, 删除
        for (int j = 0; j < MAX_EVENTS; ++j) {
            time_t curTime = time(NULL);
            if (conns[j].peerfd != 0 && curTime - conns[j].lastTime > 10) {
                ret = epoll_ctl(epfd, EPOLL_CTL_DEL, conns[j].peerfd, NULL);
                ERROR_CHECK(ret, -1, "time epoll_ctl");
                close(conns[j].peerfd);
                printf("clinet %d is kicked out\n", conns[j].peerfd);
                conns[j].peerfd = 0;
                conns[j].lastTime = 0;
            }
        }
    }

    close(epfd);
    close(listenfd);

    return 0;
}
