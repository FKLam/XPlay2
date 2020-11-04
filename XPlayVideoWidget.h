#ifndef XPLAYVIDEOWIDGET_H
#define XPLAYVIDEOWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <mutex>
#include "IVideoCall.h"
struct AVFrame;

class XPlayVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions, public IVideoCall
{
    Q_OBJECT

public:
    virtual void init(int width, int height);
    XPlayVideoWidget(QWidget *parent);
    ~XPlayVideoWidget();
    // 不管成功与否，都要释放frame
    virtual void repaint(AVFrame *frame);

protected:
    // 初始化gl
    void initializeGL();
    // 窗口尺寸变化
    void resizeGL(int width, int height);
    // 刷新显示
    void paintGL();
private:
    std::mutex mux;
    // shader程序
    QOpenGLShaderProgram *program;
    // shader中yuv变量地址
    GLuint unis[3] = { 0 };
    // opengl的texture地址
    GLuint texs[3] = { 0 };
    // 材质内存空间
    unsigned char *datas[3] = { 0 };

    int width = 240;
    int height = 128;
};

#endif // XPLAYVIDEOWIDGET_H
