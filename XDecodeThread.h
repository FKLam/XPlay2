#ifndef XDECODETHREAD_H
#define XDECODETHREAD_H
#include <QThread>
#include <mutex>
#include <list>
#include "IVideoCall.h"

class XDecode;
struct AVPacket;

class XDecodeThread: public QThread
{
public:
    XDecodeThread();
    virtual ~XDecodeThread();
    virtual void push(AVPacket *pkt);
    // 取出一帧数据并出栈，如果美誉返回NULL
    virtual AVPacket *pop();
    // 清理队列
    virtual void clear();
    // 清理资源，停止线程
    virtual void close();
protected:
    std::mutex mux;
    // 最大队列
    int maxList = 100;
    bool isExit = false;
    std::list <AVPacket *> packs;
    XDecode *decode = NULL;

};

#endif // XDECODETHREAD_H
