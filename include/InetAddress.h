#ifndef __InetAddress_HH__
#define __InetAddress_HH__

#include <arpa/inet.h>
#include <string>

/*
 * 地址管理类
 * 默认为Ipv4
 */
class InetAddress {
  public:
    InetAddress(const std::string &, unsigned short);
    InetAddress(const struct sockaddr_in &);
    InetAddress(const InetAddress &);
    ~InetAddress();

  public:
    std::string getIP();
    unsigned short getPort();
    struct sockaddr_in *getInetAddressPtr(); // 获取地址指针

  private:
    struct sockaddr_in _addr;
};

#endif
