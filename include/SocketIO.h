#ifndef __SOCKETIO_HH__
#define __SOCKETIO_HH__

/* 最基础的 读 发 任务 */
class SocketIO {
  public:
    explicit SocketIO(int); // 禁止 : SocketIO stIo = 10, 这样的隐式转换
    ~SocketIO();

  public:
    int readn(char *buf, int len);
    int writen(const char *buf, int len);
    int readLine(char *buf, int len);
    int readSys(char *buf, int len);
    int writeSys(const char *buf, int len);

    // 读取 http 请求
    // int readByHttp(char *buf, int len); // 待实现

  private:
    int _fd;
};

#endif
