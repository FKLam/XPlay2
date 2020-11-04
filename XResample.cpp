#include "XResample.h"
#include <iostream>
using namespace std;

extern "C" {
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
}

XResample::XResample()
{

}

XResample::~XResample()
{

}

// 输出参数和输入参数一致，除了采样格式，输出为S16
bool XResample::open(AVCodecParameters *para, bool isClearPara)
{
    if ( !para ) {
        return false;
    }
    mux.lock();
    actx = swr_alloc_set_opts(actx,
                       av_get_default_channel_layout(2),
                       (AVSampleFormat)outFormat,
                       para->sample_rate,
                       av_get_default_channel_layout(para->channels),
                       (AVSampleFormat)para->format,
                       para->sample_rate,
                       0, 0);
    int re = swr_init(actx);
    if ( isClearPara ) {
        avcodec_parameters_free(&para);
    }
    mux.unlock();
    if ( re != 0 ) {
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        cout << "swr_init failed!: " << buf << endl;
        return false;
    }
    return true;
}

// 返回重采样后大小
int XResample::resample(AVFrame *indata, unsigned char *data) {
    if ( !indata ) {
        return -1;
    }
    if ( !data ) {
        av_frame_free(&indata);
        return -1;
    }
    uint8_t *out[2] = { 0 };
    out[0] = data;
    int re = swr_convert(actx, out, indata->nb_samples,
                         (const uint8_t **)indata->data, indata->nb_samples);
    int outSize = re * indata->channels * av_get_bytes_per_sample((AVSampleFormat)outFormat);
    av_frame_free(&indata);
    if ( re <= 0 ) {
        return re;
    }
    return outSize;
}

void XResample::close()
{
    mux.lock();

    mux.unlock();
}
