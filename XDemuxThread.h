#ifndef XDEMUXTHREAD_H
#define XDEMUXTHREAD_H
#include <QThread>
#include "IVideoCall.h"
#include <mutex>

class XDemux;
class XVideoThread;
class XAudioThread;

class XDemuxThread: public QThread
{
public:
    XDemuxThread();
    virtual ~XDemuxThread();

    // 启动所有线程
    virtual void start();
    virtual void run();
    // 创建对象并打开
    virtual bool open(const char *url, IVideoCall *call);
    // 关闭线程，清理资源
    virtual void close();
    virtual void clear();
    void setPause(bool isPause);
    bool isPause = false;

    virtual void seek(double pos);

    long long pts = 0;
    long long totalMs = 0;

protected:
    XDemux *demux = NULL;
    XVideoThread *vt = NULL;
    XAudioThread *at = NULL;
    std::mutex mux;
    bool isExit = false;
};

#endif // XDEMUXTHREAD_H
