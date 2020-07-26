#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include "graph.h"

#include <QWidget>
#include <QOpenGLWidget>

class OGLWidget : public QOpenGLWidget
 {
    Q_OBJECT
    void drawEllipse(float xCenter, float yCenter);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
public:
    OGLWidget(QWidget *parent = 0);
    ~OGLWidget();
    Graph graph;
    void mousePressEvent(QMouseEvent *event);
protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
};

#endif // OGLWIDGET_H
