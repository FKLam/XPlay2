#include "XPlay2.h"
#include <QApplication>
#include <iostream>
using namespace std;
#include "XDemux.h"
#include "XDecode.h"
#include "ui_XPlay2.h"
#include <QThread>
#include "XPlayVideoWidget.h"
#include "XResample.h"
#include "XAudioPlay.h"
#include "XAudioThread.h"
#include "XVideoThread.h"
#include "XDemuxThread.h"

//class TestThread : public QThread
//{
//public:
//    XDemux demux;
////    XDecode vdecode;
////    XDecode adecode;
//    XPlayVideoWidget *video;
////    XResample resample;
////    XAudioThread at;
////    XVideoThread vt;
//    void init() {
//        // 测试XDemux
////        char *pathUrl = "/Users/amglfk/Desktop/killer.mp4";
//    //    pathUrl = "rtmp://live.hkstv.hk.lxdns.com/live/hks";
////        cout << "demux.open = " << demux.open(pathUrl) << endl;
//        //cout << "demux.copyVParam = " << demux.copyVParam() << endl;
//        //cout << "demux.copyAParam = " << demux.copyAParam() << endl;
////        cout << "demux.seek = " << demux.seek(0.95) << endl;

//        ////////////////////////////////////////
//        // 解码测试
//        // cout << "vdecode.open = " << vdecode.open(demux.copyVParam()) << endl;

////        cout << "adecode.open = " << adecode.open(demux.copyAParam()) << endl;

////        cout << "resample.open = " << resample.open(demux.copyAParam()) << endl;

////        XAudioPlay::get()->channels = demux.channels;
////        XAudioPlay::get()->sampleRate = demux.sampleRate;
////        cout << "XAudioPlay::get()->open() = " << XAudioPlay::get()->open() << endl;

////        cout << "at.open = " << at.open(demux.copyAParam(), demux.sampleRate, demux.channels) << endl;
////        cout << "vt.open = " << vt.open(demux.copyVParam(), video, demux.width, demux.height) << endl;
////        at.start();
////        vt.start();
//    }

//    unsigned char *pcm = new unsigned char[1024 * 1024];
//    void run() {
//        for (;;) {
//            AVPacket *pkt = demux.read();
//            if ( !pkt ) {
//                break;
//            }
//            if ( demux.isAudio(pkt) ) {
//                // 音频
////                at.push(pkt);
//                /*
//                adecode.send(pkt);
//                AVFrame *frame = adecode.recv();
//                int len = resample.resample(frame, pcm);
//                cout << "resample.resample = " << resample.resample(frame, pcm) << endl;
//                while (len > 0) {
//                    if (XAudioPlay::get()->getFree() >= len ) {
//                       XAudioPlay::get()->write(pcm, len);
//                       break;
//                    }
//                    msleep(1);
//                }*/

//            } else {
//                // 视频
////                vdecode.send(pkt);
////                AVFrame *frame = vdecode.recv();
////                cout << "Video: " << frame << endl;
////                video->repaint(frame);
////                msleep(40);
////                vt.push(pkt);
//            }
//        }
//    }
//};


int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);

    // 初始化显示


    /*
*/

//    TestThread tt;


    QApplication a(argc, argv);
    Widget w;
    w.show();

//    Ui_Widget *temp = (Ui_Widget *)w.ui;
//    temp->video->init(tt.demux.width, tt.demux.height);
//    tt.video = temp->video;

//    tt.init();

//    tt.start();

//    char *pathUrl = "/Users/amglfk/Desktop/killer.mp4";
//    XDemuxThread dt;
//    dt.open(pathUrl, temp->video);
//    dt.start();

    return a.exec();
}
