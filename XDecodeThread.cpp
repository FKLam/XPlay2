#include "XDecodeThread.h"
#include "XDecode.h"
#include <iostream>
using namespace std;

XDecodeThread::XDecodeThread()
{
    // 打开解码器
    if ( !decode ) {
        decode = new XDecode();
    }
}

XDecodeThread::~XDecodeThread()
{
    // 等待线程退出
    isExit = true;
    wait();
}

void XDecodeThread::push(AVPacket *pkt) {
    cout << "XDecodeThread::push" << endl;
    if ( !pkt ) return;
    // 阻塞
    while (!isExit) {
        mux.lock();
        if ( packs.size() < maxList ) {
            packs.push_back(pkt);
            mux.unlock();
            break;
        }
        mux.unlock();
        msleep(1);
    }
}

// 取出一帧数据并出栈，如果美誉返回NULL
AVPacket * XDecodeThread::pop() {
    mux.lock();
    if ( packs.empty() ) {
        mux.unlock();
        return NULL;
    }
    AVPacket *pkt = packs.front();
    packs.pop_front();
    mux.unlock();
    return pkt;
}

// 清理队列
void XDecodeThread::clear() {
    mux.lock();
    decode->clear();
    while (!packs.empty()) {
        AVPacket *pkt = packs.front();
        XFreePacket(&pkt);
        packs.pop_front();
    }
    mux.unlock();
}

// 清理资源，停止线程
void XDecodeThread::close() {
    clear();
    isExit = true;
    wait();
    mux.lock();
    decode->close();
    delete decode;
    decode = NULL;
    mux.unlock();
}
