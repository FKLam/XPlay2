#include "XDecode.h"
#include <iostream>
using namespace std;
extern "C" {
#include <libavcodec/avcodec.h>
}


XDecode::XDecode()
{

}

XDecode::~XDecode()
{

}

// 打开解码器 不管成功与否并释放para空间
bool XDecode::open(AVCodecParameters *para) {
    if ( !para ) {
        return false;
    }
    ////////////////////////////////////////////////
    // 视频解码器打开
    // 找到解码器
    AVCodec *codec = avcodec_find_decoder(para->codec_id);
    if ( !codec ) {
        avcodec_parameters_free(&para);
        cout << "can't find the codec id " << para->codec_id << endl;
        return false;
    }
    cout << "find the AVCodec " << para->codec_id << endl;

    mux.lock();
    ctx = avcodec_alloc_context3(codec);

    // 配置解码器上下文参数
    avcodec_parameters_to_context(ctx, para);

    avcodec_parameters_free(&para);
    // 八线程解码
    ctx->thread_count = 8;

    int re = avcodec_open2(ctx, 0, 0);
    if ( re != 0 ) {
        avcodec_free_context(&ctx);
        mux.unlock();
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        cout << "avcodec_open2 failed :" << buf << endl;
        return false;
    }
    mux.unlock();
    cout << "avcodec_open2 success!" << endl;
    return true;
}

void XDecode::close() {
    mux.lock();
    if ( ctx ) {
        avcodec_close(ctx);
        avcodec_free_context(&ctx);
    }
    mux.unlock();
}

void XDecode::clear() {
    mux.lock();
    // 清理解码缓冲
    if ( ctx ) {
        avcodec_flush_buffers(ctx);
    }
    pts = 0;
    mux.unlock();
}

// 发送到解码线程，不管成功与否清理pkt空间（对象和媒体内容）
bool XDecode::send(AVPacket *pkt) {
    if ( !pkt || pkt->size <= 0 || !pkt->data ) {
        return false;
    }
    mux.lock();
    if ( !ctx ) {
        mux.unlock();
        return false;
    }
    int re = avcodec_send_packet(ctx, pkt);
    mux.unlock();
    av_packet_free(&pkt);
    if ( re != 0 ) {
        return false;
    }
    return true;
}

// 获取解码数据，一次send可能需要多次recv，获取缓冲中的数据send NULL在recv多次
// 每次复制一份，由调用者释放 av_frame_free
AVFrame * XDecode::recv() {
    mux.lock();
    if ( !ctx ) {
        mux.unlock();
        return NULL;
    }
    AVFrame *frame = av_frame_alloc();
    int re = avcodec_receive_frame(ctx, frame);
    mux.unlock();
    if ( re != 0 ) {
        av_frame_free(&frame);
        return NULL;
    }
    cout << "frame->linesize[0] = " << frame->linesize[0] << endl;
    pts = frame->pts;
    return frame;
}

void XFreePacket(AVPacket **pkt) {
    if ( !pkt || !(*pkt) ) {
        return;
    }
    av_packet_free(pkt);
}

void XFreeFrame(AVFrame **frame) {
    if ( !frame || !(*frame) ) {
        return;
    }
    av_frame_free(frame);
}
