#ifndef XDECOE_H
#define XDECOE_H

#include <mutex>
struct AVCodecParameters;
struct AVCodecContext;
struct AVPacket;
struct AVFrame;
extern void XFreePacket(AVPacket **pkt);
extern void XFreeFrame(AVFrame **frame);
class XDecode
{
public:
    XDecode();
    virtual ~XDecode();

    bool isAudio = false;
    // 当前解码到的
    long long pts = 0;

    // 打开解码器 不管成功与否并释放para空间
    virtual bool open(AVCodecParameters *para);

    // 发送到解码线程，不管成功与否清理pkt空间（对象和媒体内容）
    virtual bool send(AVPacket *pkt);
    // 获取解码数据，一次send可能需要多次recv，获取缓冲中的数据send NULL在recv多次
    // 每次复制一份，由调用者释放 av_frame_free
    virtual AVFrame *recv();

    virtual void close();
    virtual void clear();

protected:
    // 解码上下文
    AVCodecContext *ctx = NULL;
    std::mutex mux;
};

#endif // XDECOE_H
