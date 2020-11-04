#ifndef XRESAMPLE_H
#define XRESAMPLE_H

#include <mutex>
struct AVCodecParameters;
struct SwrContext;
struct AVFrame;

class XResample
{
public:
    // 输出参数和输入参数一致，除了采样格式，输出为S16
    virtual bool open(AVCodecParameters *para, bool isClearPara);
    virtual void close();
    // 返回重采样后大小
    virtual int resample(AVFrame *indata, unsigned char *data);
    XResample();
    virtual ~XResample();

protected:
    std::mutex mux;
    SwrContext *actx = NULL;
    // AV_SAMPLE_FMT_S16
    int outFormat = 1;
};

#endif // XRESAMPLE_H
