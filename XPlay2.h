#ifndef XPLAY2_H
#define XPLAY2_H

#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    // 定时器滑动条
    void timerEvent(QTimerEvent *e);

    // 窗口尺寸变化
    void resizeEvent(QResizeEvent *e);

    // 双击全屏
    void mouseDoubleClickEvent(QMouseEvent *e);

    void setPause(bool isPause);
    bool isSliderPress = false;

public slots:
    void openFile();
    void playOrPause();
    void sliderPress();
    void sliderRelease();

private:
    Ui::Widget *ui;
};

#endif // XPLAY2_H
