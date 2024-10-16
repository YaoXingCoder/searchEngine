#ifndef __SOCKETIO_HH__
#define __SOCKETIO_HH__

/* 最基础的 读 发 任务 */
class SocketIO {
  public:
    explicit SocketIO(int); // 禁止 : SocketIO stIo = 10, 这样的隐式转换
    ~SocketIO();

  public:
    int readn(char *, int);
    int writen(const char *, int);
    int readLine(char *, int);

  private:
    int _fd;
};

#endif
