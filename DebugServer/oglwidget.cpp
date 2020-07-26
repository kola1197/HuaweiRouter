#include "oglwidget.h"
#include "iostream"
#include <QtOpenGL/qgl.h>
#include <QtOpenGL/QtOpenGL>
#include <GL/glu.h>
#include <GL/gl.h>

OGLWidget::OGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{

}

OGLWidget::~OGLWidget()
{

}

void OGLWidget::initializeGL()
{
    glClearColor(1,1,1,1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
}

void OGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (Ellips e:graph.ellipses)
    {
        drawEllipse(e.x,e.y);
    }
}

void OGLWidget::drawEllipse(float xCenter, float yCenter)
{
    float step = float(2 * M_PI/180);
    //glBegin(GL_LINE_STRIP);
    GLfloat fSizes [2];
    glGetFloatv(GL_LINE_WIDTH_RANGE,fSizes);
    GLfloat fCurrentSize = fSizes[0];
    fCurrentSize+=3.0f;
    glLineWidth(fCurrentSize);
    glBegin(GL_LINE_LOOP);
    glColor3f(1.0, 0.56, 0.0);
    for (float angle = 0; angle < float(2 * M_PI); angle += step)
    {
        const float dx = 50 * cosf(angle);
        const float dy = 25 * sinf(angle);
        glVertex2f(dx + xCenter, dy + yCenter);
    }
    glEnd();
}

void OGLWidget::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluPerspective(45, (float)9/16, 1.0, 1.0);
    gluOrtho2D(0,1600,900,0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //gluLookAt(0,0,5,0,0,0,0,1,0);
}

void OGLWidget::mousePressEvent(QMouseEvent *event)
{
    std::cout<<event->x()<<"::"<<event->y()<<std::endl;
    float x = event->x();
    float y = event->y();
    Ellips *el = graph.getEllipseByPoint(x,y);
    if (el!=NULL)
    {
        std::cout<<"ellips number "<<el->number<<std::endl;
        //el->number=30;
        //std::cout<<el->x<<"*"<<el->y<<std::endl;
        //std::cout<<"ellips number now "<<el->number<<std::endl;
        graph.active = el;
    }
    else{
        std::cout<<"FREE AREA"<<std::endl;
    }
}

void OGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (graph.active!=NULL)
    {
        graph.active->x = event->x();
        graph.active->y = event->y();
        update();
    }
}

void OGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    graph.active = NULL;
    update();
}

void OGLWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (graph.getEllipseByPoint(event->x(),event->y())==NULL)
    {
        graph.addEllips(event->x(),event->y());
    }
}
