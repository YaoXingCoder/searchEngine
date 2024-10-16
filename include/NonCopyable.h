#ifndef __NonCopyable_HH__
#define __NonCopyable_HH__

/*
 * 禁止复制拷贝类
 * 原理 : 派生类没有显示实现拷贝和复制时, 编译器会自动生成默认, 同时也会调用基类的, 但基类删除, 则无法自动生成
 */
class NonCopyable {
  protected:
    NonCopyable() {
    }
    ~NonCopyable() {
    }
    NonCopyable(const NonCopyable &) = delete;
    NonCopyable &operator=(const NonCopyable &) = delete;
};

#endif
