#ifndef XAUDIOTHREAD_H
#define XAUDIOTHREAD_H
#include <mutex>
#include "XDecodeThread.h"

struct AVCodecParameters;
class XAudioPlay;
class XResample;
struct AVPacket;

class XAudioThread: public XDecodeThread
{
public:
    // 当前音频播放的pts
    long long pts = 0;
    XAudioThread();
    virtual ~XAudioThread();

    void run();
    // 打开，不管成功与否都清理
    virtual bool open(AVCodecParameters *para, int sampleRate, int channels);
    // 停止线程，清理资源
    virtual void close();
    virtual void clear();
    void setPause(bool isPause);
    bool isPause = false;

protected:
    XAudioPlay *ap = NULL;
    XResample *res = NULL;
    std::mutex amux;
};

#endif // XAUDIOTHREAD_H
