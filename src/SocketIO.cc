/*************************************************************************
    > File Name: SocketIO.cpp
    > Author: JiaZiChunQiu
    > Created Time: 2024年09月20日 星期五 14时42分17秒
    > Mail: JiaZiChunQiu@163.com
    > Title: 读法类, 向给定的套接字中的读取和发送数据, 通知指定长度
    > Content:
 ************************************************************************/

#include "SocketIO.h"

#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

SocketIO::SocketIO(int fd) : _fd(fd) {
}

SocketIO::~SocketIO() {
}

/* 系统调用 */
int SocketIO::readSys(char *buf, int len) {
    return read(_fd, buf, len);
}
int SocketIO::writeSys(const char *buf, int len) {
    return write(_fd, buf, len);
}

int write(int fd, char *buf, int len);

/* 读取指定字节数的字符串
 * parameter char *buf : 字符串首地址, 传入传出
 * parameter len : 指定字节数
 * return : 返回已读取字节数
 * */
int SocketIO::readn(char *buf, int len) {
    int total = 0;    // 已读取的字节数
    int left = len;   // 剩余字节数
    int ret = 0;      // 异常记录
    char *pstr = buf; // 字符指针, 存放读取字符

    // 1.循环 read
    while (left > 0) {
        ret = read(_fd, pstr, left);

        // 2.读取结果判错
        if (-1 == ret && errno == EINTR) {
            continue; // 能容忍错误, 由信号中断
        } else if (-1 == ret) {
            perror("readn");
            return -1;
        } else if (ret == 0) {
            break;
        } else {
            total += ret;
            left -= ret;
            pstr += ret;
        }
    }

    return total;
}

/* 发送指定字节数的字符串
 * parameter char *buf : 字符串首地址, 传入参数
 * parameter len : 指定字节数
 * return : 返回已发送字节数
 * */
int SocketIO::writen(const char *buf, int len) {
    int total = 0;
    int left = len;
    int ret = 0;
    const char *pstr = buf;

    // 1.循环 write
    while (left > 0) {
        ret = write(_fd, pstr, left);

        // 2.判错
        if (-1 == ret && errno == EINTR) {
            continue; // 能容忍错误
        } else if (-1 == ret) {
            perror("writen");
            return -1;
        } else if (ret == -1 && errno == EPIPE) {
            perror("has closed");
            break;
        } else if (0 == ret) {
            break;
        } else {
            total += ret;
            pstr += ret;
            left -= ret;
        }
    }

    return total;
}

/*
 * 按给定的标识符为结束标志
 * parameter char *buf : 字符串首地址, 传入传出
 * parameter len : 指定字节数
 * return : 返回已读取字节数
 * 思路 : 拷贝, 判断, 读取
 */
int SocketIO::readLine(char *buf, int len) {
    int left = len - 1; // 流最后一位, 添加 '\0'
    int total = 0;
    int ret = 0;
    char *pstr = buf;

    // 1.循环读取
    while (left > 0) {
        ret = recv(_fd, pstr, left, MSG_PEEK); // MSG_PEEK : 从缓冲区拷贝内容

        // 2.判断
        if (-1 == ret && errno == EINTR) {
            continue; // 能容忍中断
        } else if (-1 == ret) {
            perror("readLine");
            return -1;
        } else if (ret == 0) {
            break;
        } else {

            // 3.循环判断拷贝的内容是否是给定的结束符(\r\n)
            // for (int idx = 0; idx < ret - 1; ++idx) {
            //     if ((pstr[idx] == '\r') && (pstr[idx + 1] == '\n')) {
            //         int str_offset = idx + 2; // 多一位存放 '\0', 字符串结束标志
            //         readn(pstr, str_offset);
            //         pstr += str_offset; // 移动偏移量
            //         *pstr = '\0';
            //         return total + str_offset; // 返回已读取字节数
            //     }
            // }

            for (int idx = 0; idx < ret; ++idx) {
                // 检查当前字符和下一个字符是否为 '\r' 和 '\n'
                if ((pstr[idx] == '\r') && (idx + 1 < ret) && (pstr[idx + 1] == '\n')) {
                    int str_offset = idx + 2; // 包括 '\r' 和 '\n'
                    readn(pstr, str_offset);
                    pstr += str_offset;
                    *pstr = '\0'; // C风格字符串以 '\0' 结尾
                    return total + str_offset;
                }
            }

            // for (int idx = 0; idx < ret; ++idx) {
            //     if (pstr[idx] == '\n') {
            //         int str_offset = idx + 1;
            //         readn(pstr, str_offset);
            //         pstr += str_offset;
            //         *pstr = '\0'; // C风格字符串以'\0'结尾
            //         return total + str_offset;
            //     }
            // }

            // 4.无给定结束标志, 正常读取
            readn(pstr, ret); // 读取len长度内容
            total += ret;     // 计算已读取字节数
            pstr += ret;      // 移动字符指针
            left -= ret;      // 计算仍需读取字节数
        }
    }

    *pstr = '\n'; // 正常结束, 添加结束 '\0'
    return total;
}

/*
 * 读取 http 请求, use http-parser-2.9.4
 *
 */