#include "XAudioThread.h"
#include "XDecode.h"
#include "XAudioPlay.h"
#include "XResample.h"
#include <iostream>
using namespace std;

XAudioThread::XAudioThread()
{
    if ( !res ) res = new XResample();
    if ( !ap ) ap = XAudioPlay::get();
}

XAudioThread::~XAudioThread() {

}

void XAudioThread::run() {
    cout << "XAudioThread::run" << endl;
    unsigned char *pcm = new unsigned char[1024 * 1024 * 10];
    while (!isExit) {
        amux.lock();
        // 没有数据
//        if ( packs.empty() || !decode || !res || !ap ) {
//            amux.unlock();
//            msleep(1);
//            continue;
//        }
//        AVPacket *pkt = packs.front();
//        packs.pop_front();

        if ( isPause ) {
            amux.unlock();
            msleep(5);
            continue;
        }
        AVPacket *pkt = pop();
        bool re = decode->send(pkt);
        if ( !re ) {
            amux.unlock();
            msleep(1);
            continue;
        }
        // 一次send 多次recv
        while (!isExit) {
            AVFrame *frame = decode->recv();
            if ( !frame ) {
                break;
            }
            // 减去缓冲中未播放的时间 毫秒
            pts = decode->pts - ap->getNoPlayMs();
            cout << "audio pts = " << pts << endl;
            // 重采样
            int size = res->resample(frame, pcm);
            // 播放音频
            while (!isExit) {
                if ( size <= 0 ) break;
                // 缓冲未播放完，空间不够
                if ( ap->getFree() < size || isPause ) {
                    msleep(1);
                    continue;
                }
                ap->write(pcm, size);
                break;
            }
        }

        amux.unlock();
    }
    delete []pcm;
    cout << "XAudioThread::run---end" << endl;
}

// 打开，不管成功与否都清理
bool XAudioThread::open(AVCodecParameters *para, int sampleRate, int channels) {
    if ( !para ) {
        return false;
    }
    amux.lock();
    // 清理队列
    clear();
    pts = 0;
    bool re = true;
    if ( !res->open(para, false) ) {
//        amux.unlock();
        cout << "res->open failed" << endl;
//        return false;
        re = false;
    }
    ap->sampleRate = sampleRate;
    ap->channels = channels;
    if ( !ap->open() ) {
//        amux.unlock();
        cout << "ap->open failed" << endl;
//        return false;
        re = false;
    }
    if ( !decode->open(para) ) {
        cout << "audio decode->open failed" << endl;
        re = false;
    }
    amux.unlock();
    cout << "XAudioThread::open success!: " << re << endl;
    return re;
}

void XAudioThread::setPause(bool isPause) {
//    amux.lock();
    this->isPause = isPause;
    if ( ap ) {
        ap->setPause(isPause);
    }
//    amux.unlock();
}

void XAudioThread::clear() {
    XDecodeThread::clear();
    amux.lock();
    if ( ap ) ap->clear();
    amux.unlock();
}

// 停止线程，清理资源
void XAudioThread::close() {
    XDecodeThread::close();
    if ( res ) {
        res->close();
        amux.lock();
        delete res;
        res = NULL;
        amux.unlock();
    }
    if ( ap ) {
        ap->close();
        amux.lock();
        ap = NULL;
        amux.unlock();
    }
}
