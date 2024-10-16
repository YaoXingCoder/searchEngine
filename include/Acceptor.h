#ifndef __ACCEPTOR_HH__
#define __ACCEPTOR_HH__

#include "InetAddress.h"
#include "Socket.h"

#define LISTEN_MAX 128

class Acceptor {
  public:
    Acceptor(const std::string &, unsigned short);
    ~Acceptor();

  public:
    void ready(); 
    int accept();
    int getListenFD();

  private:
    void setReuseAddr();
    void setReusePort();
    void bind();
    void listen();

  private:
    Socket _sock;
    InetAddress _addr;
};

#endif
