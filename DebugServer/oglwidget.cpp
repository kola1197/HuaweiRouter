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
        drawEllipse(&e);
//        if (e.number != graph.activeNumberForEdge)
//        {
//            //drawEllipse(e.x,e.y, 0);
//            drawEllipse(&e);
//        }
//        else
//        {
//            drawEllipse(&e);
//            //drawEllipse(e.x,e.y, 1);
//        }
    }
    for (Edge e:graph.edges)
    {
        Ellips *el1 = graph.getEllipseByNumber(e.from);
        Ellips *el2 = graph.getEllipseByNumber(e.to);
        if (el1!=NULL && el2!=NULL)
        {
            std::cout<<"Edge output "<<e.from<<" "<<e.to<<std::endl;
            std::cout<<"Ellips "<<el1->x<<" "<<el1->y<<" -*- "<<el2->x<<" "<<el2->y<<std::endl;
            drawEdge(el1->x,el1->y,el2->x,el2->y);
        }
    }
}

void OGLWidget::drawEdge(float x1, float y1, float x2,float y2)
{
    GLfloat fSizes [2];
    glGetFloatv(GL_LINE_WIDTH_RANGE,fSizes);
    GLfloat fCurrentSize = fSizes[0];
    fCurrentSize+=3.0f;
    glLineWidth(fCurrentSize);
    glBegin(GL_LINE_LOOP);
    glColor3f(0.0, 0.0, 1.0);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void OGLWidget::drawEllipse(Ellips *e)      //color 0 - default, 1 - active ellipse
{
    float step = float(2 * M_PI/180);
    //glBegin(GL_LINE_STRIP);
    GLfloat fSizes [2];
    glGetFloatv(GL_LINE_WIDTH_RANGE,fSizes);
    GLfloat fCurrentSize = fSizes[0];
    fCurrentSize+=3.0f;
    glLineWidth(fCurrentSize);
    glBegin(GL_LINE_LOOP);
    if (e->colorStatus == 0)
    {
        glColor3f(1.0, 0.56, 0.0);
    }
    if (e->colorStatus == 1)
    {
        glColor3f(0.5, 0.3, 0.5);
    }
    if (e->colorStatus == 2)
    {
        glColor3f(0, 0.8, 0);
    }
    for (float angle = 0; angle < float(2 * M_PI); angle += step)
    {
        const float dx = 50 * cosf(angle);
        const float dy = 25 * sinf(angle);
        glVertex2f(dx + e->x, dy + e->y);
    }
    glEnd();
}

void OGLWidget::drawEllipse(float xCenter, float yCenter, int color)      //color 0 - default, 1 - active ellipse
{
    float step = float(2 * M_PI/180);
    //glBegin(GL_LINE_STRIP);
    GLfloat fSizes [2];
    glGetFloatv(GL_LINE_WIDTH_RANGE,fSizes);
    GLfloat fCurrentSize = fSizes[0];
    fCurrentSize+=3.0f;
    glLineWidth(fCurrentSize);
    glBegin(GL_LINE_LOOP);
    if (color == 0)
    {
        glColor3f(1.0, 0.56, 0.0);
    }
    if (color == 1)
    {
        glColor3f(0.5, 0.3, 0.5);
    }
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
    gluOrtho2D(0,this->width(),this->height(),0);
    //gluOrtho2D(0,1600,900,0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //gluLookAt(0,0,5,0,0,0,0,1,0);
}

void OGLWidget::mousePressEvent(QMouseEvent *event)
{
    if (!screenLocked && event->button() == Qt::LeftButton)
    {
        std::cout<<event->x()<<"::"<<event->y()<<std::endl;
        float x = event->x();
        float y = event->y();
        Ellips *el = graph.getEllipseByPoint(x,y);
        if (el!=NULL)
        {
            QString q="";
            q+="x  : "+QString::number(el->x)+"\n";
            q+="y  : "+QString::number(el->y)+"\n";
            q+="num: "+QString::number(el->number)+"\n";
            emit transmit_info(q);
            std::cout<<"ellips number "<<el->number<<std::endl;
            el->colorStatus=1;
            //el->number=30;
            //std::cout<<el->x<<"*"<<el->y<<std::endl;
            //std::cout<<"ellips number now "<<el->number<<std::endl;
            graph.active = el;
        }
        else{
            std::cout<<"FREE AREA"<<std::endl;
        }
    }
    if (event->button() == Qt::RightButton)
    {
        graph.addEdge(-1);
    }
}

void OGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!screenLocked && graph.active!=NULL)
    {
        graph.active->x = event->x();
        graph.active->y = event->y();
        update();
    }
}

void OGLWidget::deactivate()
{
    if (graph.active != NULL)
    {
        graph.active->colorStatus = 0;
    }
    graph.active = NULL;
    update();
}

void OGLWidget::deactivateNewEdge()
{
    for (int i = 0; i < graph.ellipses.size(); i++)
    {
        graph.ellipses[i].colorStatus = 0;
    }
}

void OGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!screenLocked)
    {
        deactivate();
        //std::cout<<"release"<<std::endl;
        //graph.addEdge(-1);
    }
}

void OGLWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (!screenLocked)
    {
        Ellips *el = graph.getEllipseByPoint(event->x(),event->y());
        if (el == NULL)
        {
            graph.addEllips(event->x(),event->y());
            graph.addEdge(-1);
            graph.addEdge(-1);
            deactivate();
            deactivateNewEdge();
        }
        else
        {
            el->colorStatus = 1;
            if (graph.addEdge(el->number))
            {
                std::cout<<"TRUE"<<::std::endl;
                deactivateNewEdge();
            }
            //graph.active = el;
        }
    }
}

