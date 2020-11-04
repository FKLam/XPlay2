#include "XAudioPlay.h"
#include <QAudioFormat>
#include <QAudioOutput>
#include <mutex>

class CXAudioPlay : public XAudioPlay {
public:
    QAudioOutput *output = NULL;
    QIODevice *io = NULL;
    std::mutex mux;
    // 打开音频播放
    virtual bool open() {
        close();
        QAudioFormat fmt;
        fmt.setSampleRate(sampleRate);
        fmt.setSampleSize(sampleSize);
        fmt.setChannelCount(channels);
        fmt.setCodec("audio/pcm");
        fmt.setByteOrder(QAudioFormat::LittleEndian);
        fmt.setSampleType(QAudioFormat::UnSignedInt);
        mux.lock();
        output = new QAudioOutput(fmt);
        io = output->start();   // 开始播放
        mux.unlock();
        if ( io ) {
            return true;
        }
        return false;
    }

    // 播放音频
    virtual bool write(const unsigned char *data, int datasize) {
        if ( !data || datasize <= 0 ) {
            return false;
        }
        mux.lock();
        if ( !output || !io ) {
            mux.unlock();
            return false;
        }
        int size = io->write((char *)data, datasize);
        mux.unlock();
        if ( datasize != size ) {
            return false;
        }
        return true;
    }

    void setPause(bool isPause) {
        mux.lock();
        if ( !output ) {
            mux.unlock();
            return;
        }
        if ( isPause ) {
            output->suspend();
        } else {
            output->resume();
        }
        mux.unlock();
    }

    virtual int getFree() {
        mux.lock();
        if ( !output ) {
            mux.unlock();
            return 0;
        }
        int free = output->bytesFree();
        mux.unlock();
        return free;
    }
    // 返回缓冲中还没有播放的时间 毫秒
    virtual long long getNoPlayMs() {
        mux.lock();
        if ( !output ) {
            mux.unlock();
            return 0;
        }
        long long pts = 0;
        // 还未播放的字节数
        double size = output->bufferSize() - output->bytesFree();
        // 一秒音频字节大小
        double secSize = sampleRate * (sampleSize / 8) * channels;
        if ( secSize <= 0 ) {
            pts = 0;
        } else {
            pts = size / secSize * 1000;
        }
        mux.unlock();
        return pts;
    }

    virtual void clear() {
        mux.lock();
        if ( io ) io->reset();
        mux.unlock();
    }

    virtual void close() {
        mux.lock();
        if ( io ) {
           io->close();
           delete io;
           io = NULL;
        }
        if ( output ) {
            output->stop();
            delete output;
            output = NULL;
        }

        mux.unlock();
    }
};

XAudioPlay::XAudioPlay()
{

}

XAudioPlay::~XAudioPlay() {

}

XAudioPlay * XAudioPlay::get() {
    static CXAudioPlay play;

    return &play;
}
