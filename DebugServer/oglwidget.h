#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include "graph.h"
#include "TextureImage.h"

#include <QWidget>
#include <QOpenGLWidget>

class OGLWidget : public QOpenGLWidget
 {
    Q_OBJECT
    void drawEllipse(float xCenter, float yCenter, int color);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
public:
    OGLWidget(QWidget *parent = 0);
    ~OGLWidget();
    Graph graph;
    void mousePressEvent(QMouseEvent *event);
    void drawEdge(float x1, float y1, float x2, float y2);
    bool screenLocked = false;    //we cannot change our map after start
    void drawEllipse(Ellips *e);
    void deactivate();
    void deactivateNewEdge();
    oglFont *glFont;
    void renderText(double x, double y, double z, const QString &str, const QFont &font);

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    std::tuple<float, float, float, float> countCoords(Ellips *el1, Ellips *el2);
    int sign(float i);

signals:
    void transmit_info(QString);

};

#endif // OGLWIDGET_H
