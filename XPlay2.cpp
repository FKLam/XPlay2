#include "XPlay2.h"
#include "ui_XPlay2.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include "XDemuxThread.h"

static XDemuxThread dt;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    dt.start();
    startTimer(40);
}

Widget::~Widget()
{
    dt.close();
    delete ui;
}

void Widget::openFile() {
    // 选择文件
    QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择视频文件"));
    qDebug() << name;
    if ( name.isEmpty() ) return;
    this->setWindowTitle(name);
    bool re = dt.open(name.toLocal8Bit(), ui->video);
    if ( !re ) {
        QMessageBox::information(0, "Error", "open file failed!");
        return;
    }
    setPause(dt.isPause);
}

// 播放或者暂停点击
void Widget::playOrPause() {
    bool isPause = !dt.isPause;
    setPause(isPause);
    dt.setPause(isPause);
}


void Widget::sliderPress() {
    isSliderPress = true;
}

void Widget::sliderRelease() {
    isSliderPress = false;
    double pos = 0.0;
    pos = ui->playPos->value() / (double)ui->playPos->maximum();
    dt.seek(pos);
}

// 定时器 滑动条显示
void Widget::timerEvent(QTimerEvent *e) {
    if ( isSliderPress ) return;
    long long total = dt.totalMs;
    if ( total > 0 ) {
        double pos = (double)dt.pts / (double)total;
        int v = ui->playPos->maximum() * pos;
        ui->playPos->setValue(v);
    }
}

// 窗口尺寸变化
void Widget::resizeEvent(QResizeEvent *e) {
    ui->playPos->move(50, this->height() - 100);
    ui->playPos->resize(this->width() - 100, ui->playPos->height());
    ui->openFile->move(100, this->height() - 150);
    ui->isPlay->move(ui->openFile->x() + ui->openFile->width() + 10, ui->openFile->y());
    ui->video->resize(this->size());
}

// 双击全屏
void Widget::mouseDoubleClickEvent(QMouseEvent *e) {
    if ( isFullScreen() ) {
        this->showNormal();
    } else {
        this->showFullScreen();
    }
}

void Widget::setPause(bool isPause) {
    if ( isPause ) {
        ui->isPlay->setText("播 放");
    } else {
        ui->isPlay->setText("暂 停");
    }
}
