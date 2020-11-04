#ifndef XVIDEOWIDGET_H
#define XVIDEOWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>

class XVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    XVideoWidget(QWidget *parent);
    ~XVideoWidget();

protected:
    void initializeGL();                            // 初始化gl
    void paintGL();                                 // 刷新显示
    void resizeGL(int width, int height);           // 窗口尺寸变化

private:
    // shader程序
    QGLShaderProgram program;
    // 从shader中yuv变量地址
    GLuint unis[3] = { 0 };
    // opengl的texture地址
    GLuint texs[3] = { 0 };

    // 材质的内存空间
    unsigned char *datas[3] = { 0 };

    int width = 240;
    int height = 128;
};

#endif // XVIDEOWIDGET_H
