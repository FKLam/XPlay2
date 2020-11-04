#include "XSlider.h"

XSlider::XSlider(QWidget *parent) : QSlider(parent)
{

}

XSlider::~XSlider() {

}

void XSlider::mousePressEvent(QMouseEvent *e) {
    double pos = e->pos().x() / (double)width();
    setValue(pos * this->maximum());
    // 原有事件处理
//    QSlider::mousePressEvent(e);
    QSlider::sliderReleased();
}
