#include "XDemux.h"
#include <iostream>
using namespace std;

extern "C" {
    #include <libavformat/avformat.h>
    #include <libavutil/avutil.h>
    #include <libavdevice/avdevice.h>
}

static double r2d(AVRational r) {
    return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}

XDemux::XDemux()
{
    static bool isFirst = true;
    static std::mutex dmux;
    dmux.lock();
    if ( isFirst ) {
        // 初始化封装库
        // 初始化网络库（可以打开rtsp，rtmp，http 协议的流媒体视频）
        avformat_network_init();
        isFirst = false;
    }
    dmux.unlock();
}

XDemux::~XDemux()
{

}

// 打开媒体文件，或者流媒体 rtmp http rstp
bool XDemux::open(const char *url) {
    // 参数设置
    AVDictionary *opts = NULL;
    // 设置rtsp流 tcp协议打开
    av_dict_set(&opts, "rtsp_transport", "tcp", 0);

    // 网络延时时间
    av_dict_set(&opts, "max_delay", "500", 0);

    mux.lock();
    // 解封装上下文
    ic = NULL;
    int re = avformat_open_input(
                &ic,
                url,
                0,          // 0 表示自动选择解封装器
                &opts      // 参数设置，比如rtsp的延时时间
                );
    if ( re != 0 ) {
        mux.unlock();
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        cout << "open " << url << " failed!: " << buf << endl;
        return false;
    }
    cout << "open " << url << " success!" << endl;
    // 获取流信息
    re = avformat_find_stream_info(ic, 0);
    // 总时长，毫秒
    totalMs = ic->duration / (AV_TIME_BASE / 1000);
    cout << "totalMs = " << totalMs << endl;

    // 打印视频流详细信息
    av_dump_format(ic, 0, url, 0);

    // 音视频索引 读取时区分音视频
    videoStream = 0;
    audioStream = 1;

    cout << "=================================================" << endl;
    // 获取视频流
    videoStream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    AVStream *as = ic->streams[videoStream];
    cout << "codec_id = " << as->codecpar->codec_id << endl;
    cout << "format = " << as->codecpar->format << endl;
    // 视频 AVMEDIA_TYPE_VIDEO
    cout << videoStream << " 视频信息 " << endl;
    cout << "width = " << as->codecpar->width << endl;
    cout << "height = " << as->codecpar->height << endl;
    // 帧率 fps 分数转换
    cout << "video fps = " << r2d(as->avg_frame_rate) << endl;
    width = as->codecpar->width;
    height = as->codecpar->height;

    cout << "=================================================" << endl;
    // 获取音频流
    audioStream = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    as = ic->streams[audioStream];
    sampleRate = as->codecpar->sample_rate;
    channels = as->codecpar->channels;
    cout << "codec_id = " << as->codecpar->codec_id << endl;
    cout << "format = " << as->codecpar->format << endl;
    cout << audioStream << " 音频信息 " << endl;
    cout << "sample_rate = " << as->codecpar->sample_rate << endl;
    // AVSampleFormat
    cout << "channels = " << as->codecpar->channels << endl;
    // 一帧数据？？单通道样本数
    cout << "frame_size = " << as->codecpar->frame_size << endl;
    // 1024 * 2 * 2 = 4096 fps = sample_rate / frame_size
    mux.unlock();

    return true;
}

// 空间需要调用者释放，释放AVPacket对象空间，和数据空间 av_packet_free
AVPacket * XDemux::read() {
    mux.lock();
    if ( !ic ) {        // 容错
        mux.unlock();
        return NULL;
    }
    AVPacket *pkt = av_packet_alloc();
    int re = av_read_frame(ic, pkt);
    if ( re != 0 ) {
        mux.unlock();
        av_packet_free(&pkt);
        return NULL;
    }
    // pts 转换为毫秒
    pkt->pts = pkt->pts * (1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
    pkt->dts = pkt->dts * (1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));

    mux.unlock();
    return pkt;
}

// 只读视频，音频丢弃空间释放
AVPacket *XDemux::readVideo() {
    mux.lock();
    if ( !ic ) {        // 容错
        mux.unlock();
        return NULL;
    }
    mux.unlock();
    // 防止阻塞
    AVPacket *pkt = NULL;
    for (int i = 0; i < 20; i++ ) {
        pkt = read();
        if ( !pkt ) {
            break;
        }
        if ( pkt->stream_index == videoStream ) {
            break;
        }
        av_packet_free(&pkt);
    }
    return pkt;
}

// 获取视频参数 返回的空间需要清理 avcodec_parameters_free
AVCodecParameters * XDemux::copyVParam() {
    mux.lock();
    if ( !ic ) {        // 容错
        mux.unlock();
        return NULL;
    }

    AVCodecParameters *pa = avcodec_parameters_alloc();
    avcodec_parameters_copy(pa, ic->streams[videoStream]->codecpar);

    mux.unlock();
    return pa;
}

// 获取音频参数 返回的空间需要清理 avcodec_parameters_free
AVCodecParameters * XDemux::copyAParam() {
    mux.lock();
    if ( !ic ) {        // 容错
        mux.unlock();
        return NULL;
    }

    AVCodecParameters *pa = avcodec_parameters_alloc();
    avcodec_parameters_copy(pa, ic->streams[audioStream]->codecpar);

    mux.unlock();
    return pa;
}

// seek 位置 pos 0.0 ～ 1.0
bool XDemux::seek(double pos) {
    mux.lock();
    if ( !ic ) {        // 容错
        mux.unlock();
        return false;
    }
    // 清理读取缓存
    avformat_flush(ic);

    long long seekPos = 0;
    seekPos = ic->streams[videoStream]->duration * pos;
    int re = av_seek_frame(ic, videoStream, seekPos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
    mux.unlock();
    if ( re < 0 ) {
        return false;
    }
    return true;
}

bool XDemux::isAudio(AVPacket *pkt) {
    if ( !pkt ) {
        return false;
    }
    if ( pkt->stream_index == videoStream ) {
        return false;
    }
    return true;
}

// 清空读取缓存
void XDemux::clear() {
    mux.lock();
    if ( !ic ) {        // 容错
        mux.unlock();
        return;
    }
    // 清理读取缓存
    avformat_flush(ic);
    mux.unlock();
}

void XDemux::close() {
    mux.lock();
    if ( !ic ) {        // 容错
        mux.unlock();
        return;
    }
    avformat_close_input(&ic);
    totalMs = 0;
    mux.unlock();
}
