#include "XVideoThread.h"
#include "XDecode.h"
#include <iostream>
using namespace std;

XVideoThread::XVideoThread()
{

}

XVideoThread::~XVideoThread()
{
}

void XVideoThread::run() {
    cout << "XVideoThread::run" << endl;
    while (!isExit) {
        vmux.lock();
        if ( isPause ) {
            vmux.unlock();
            msleep(5);
            continue;
        }
        // 音视频同步
        if ( synpts > 0 && synpts < decode->pts ) {
            vmux.unlock();
            msleep(1);
            continue;
        }
//        // 没有数据
//        if ( packs.empty() || !decode ) {
//            vmux.unlock();
//            msleep(1);
//            continue;
//        }

//        AVPacket *pkt = packs.front();
//        packs.pop_front();

        AVPacket *pkt = pop();

        bool re = decode->send(pkt);
        if ( !re ) {
            vmux.unlock();
            msleep(1);
            continue;
        }
        // 一次send 多次recv
        while (!isExit) {
            AVFrame *frame = decode->recv();
            if ( !frame ) {
                break;
            }
            // 显示视频
            if ( call ) {
                call->repaint(frame);
            }
        }

        vmux.unlock();
    }
    cout << "XVideoThread::run---end" << endl;
}

// 打开，不管成功与否都清理
bool XVideoThread::open(AVCodecParameters *para, IVideoCall *call, int width, int height) {
    if ( !para ) {
        return false;
    }
    vmux.lock();
    // 清理队列
    clear();
    synpts = 0;
    // 初始化显示窗口
    this->call = call;
    if ( call ) {
        call->init(width, height);
    }
    vmux.unlock();
    int re = true;
    if ( !decode->open(para) ) {
        cout << "video decode->open failed" << endl;
        re = false;
    }
    cout << "XVideoThread::open success!: " << re << endl;
    return re;
}

void XVideoThread::setPause(bool isPause) {
    vmux.lock();
    this->isPause = isPause;
    vmux.unlock();
}

// 解码pts，如果接收到的解码数据pts > seekPts，return true，并且显示画面
bool XVideoThread::repaintPts(AVPacket *pkt, long long seekPts) {
    vmux.lock();
    bool re = decode->send(pkt);
    if ( !re ) {
        vmux.unlock();
        return true;
    } // 表示结束解码
    AVFrame *frame = decode->recv();
    if ( !frame ) {
        vmux.unlock();
        return false;
    }
    if ( decode->pts >= seekPts ) {
        if ( call ) {
            call->repaint(frame);
        }
        vmux.unlock();
        return true;
    }
    XFreeFrame(&frame);
    vmux.unlock();
    return false;
}


