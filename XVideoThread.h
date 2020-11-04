#ifndef XVIDEOTHREAD_H
#define XVIDEOTHREAD_H
#include "IVideoCall.h"
#include "XDecodeThread.h"
#include <mutex>

struct AVCodecParameters;
class XAudioPlay;
struct AVPacket;

// 解码和显示视频

class XVideoThread: public XDecodeThread
{
public:
    XVideoThread();
    ~XVideoThread();

    void run();
    // 打开，不管成功与否都清理
    virtual bool open(AVCodecParameters *para, IVideoCall *call, int width, int height);
    void setPause(bool isPause);
    bool isPause = false;

    // 解码pts，如果接收到的解码数据pts > seekPts，return true，并且显示画面
    virtual bool repaintPts(AVPacket *pkt, long long seekPts);

    // 同步时间由外部传入
    long long synpts = 0;

protected:
    XAudioPlay *ap = NULL;
    IVideoCall *call = NULL;
    long long pts = 0;
    std::mutex vmux;
};

#endif // XVIDEOTHREAD_H
