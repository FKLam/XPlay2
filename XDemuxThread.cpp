#include "XDemuxThread.h"
#include "XDemux.h"
#include "XVideoThread.h"
#include "XAudioThread.h"
#include <iostream>
using namespace std;

XDemuxThread::XDemuxThread()
{

}

XDemuxThread::~XDemuxThread() {
    isExit = true;
    wait();
}

bool XDemuxThread::open(const char *url, IVideoCall *call) {
    if ( !url || url[0] == '\n' ) {
        return false;
    }
    mux.lock();
    if ( !demux ) demux = new XDemux();
    if ( !vt ) vt = new XVideoThread();
    if ( !at ) at = new XAudioThread();
    bool re = demux->open(url);
    if ( !re ) {
        mux.unlock();
        cout << " demux->open(url) failed!" << endl;
        return false;
    }
    // 启动当前线程
    QThread::start();
    if ( vt ) vt->start();
    if ( at ) at->start();

    // 打开视频解码器和处理线程
    re = vt->open(demux->copyVParam(), call, demux->width, demux->height);
    if ( !re ) {
        cout << "vt->open failed!" << endl;
    }
    // 打开音频解码器和处理线程
    re = at->open(demux->copyAParam(), demux->sampleRate, demux->channels);
    if ( !re ) {
        cout << "at->open failed!" << endl;
    }
    totalMs = demux->totalMs;
    mux.unlock();
    cout << "XDemuxThread::open success! " << re << endl;
    return re;
}

// 启动所有线程
void XDemuxThread::start() {
//    mux.lock();
//    if ( !demux ) demux = new XDemux();
//    if ( !vt ) vt = new XVideoThread();
//    if ( !at ) at = new XAudioThread();
//    // 启动当前线程
//    QThread::start();
//    if ( vt ) vt->start();
//    if ( at ) at->start();
//    mux.unlock();
}

void XDemuxThread::setPause(bool isPause) {
    mux.lock();
    this->isPause = isPause;
    if ( at ) at->setPause(isPause);
    if ( vt ) vt->setPause(isPause);
    mux.unlock();
}

void XDemuxThread::run() {
    while ( !isExit ) {
        mux.lock();

        if ( isPause ) {
            mux.unlock();
            msleep(5);
            continue;
        }
        if ( !demux ) {
            mux.unlock();
            msleep(5);
            continue;
        }
        // 音视频同步
        if ( vt && at ) {
            pts = at->pts;
            vt->synpts = at->pts;
        }
        AVPacket *pkt = demux->read();
        if ( !pkt ) {
            mux.unlock();
            msleep(5);
            continue;
        }
        if ( demux->isAudio(pkt) ) {
            // 音频
            if ( at ) {
                at->push(pkt);
            }
        } else {
            // 视频
            if ( vt ) {
                vt->push(pkt);
            }
        }
        mux.unlock();
        msleep(1);
    }
}

void XDemuxThread::seek(double pos) {
    // 清理缓冲
    clear();
    mux.lock();
    bool status = this->isPause;
    mux.unlock();

    // 暂停
    setPause(true);

    mux.lock();
    if ( demux )
        demux->seek(pos);
    // 实际要显示的位置pts
    long long seekPts = pos * demux->totalMs;
    while (!isExit) {
        AVPacket *pkt = demux->readVideo();
        if ( !pkt ) break;

        bool re = vt->repaintPts(pkt, seekPts);
        // 如果解码到seekPts
        if ( re ) {
            this->pts = seekPts;
            break;
        }
    }
    mux.unlock();
    // 恢复拖动前的状态
    if ( !status ) {
        setPause(false);
    }
}

void XDemuxThread::clear() {
    mux.lock();
    if ( demux ) demux->clear();
    if ( vt ) vt->clear();
    if ( at ) at->clear();
    mux.unlock();
}

// 关闭线程，清理资源
void XDemuxThread::close() {
    isExit = true;
    wait();
    if ( vt ) vt->close();
    if ( at ) at->close();
    mux.lock();
    delete vt;
    delete at;
    vt = NULL;
    at = NULL;
    mux.unlock();
}
