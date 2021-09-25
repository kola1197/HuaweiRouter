//
//  Widget for openGL rendering
//

#include "oglwidget.h"
#include "iostream"
#include <QtOpenGL/qgl.h>
#include <QtOpenGL/QtOpenGL>
#include <GL/glu.h>
#include <GL/gl.h>
#include <Utils/sout.h>

OGLWidget::OGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{

}

OGLWidget::~OGLWidget()
{

}

void OGLWidget::initializeGL()
{
    QSurfaceFormat fmt;
    fmt.setSamples(4);
    setFormat(fmt);
    glClearColor(1,1,1,1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_MULTISAMPLE);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

}


void OGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (Ellips e:graph.ellipses)
    {
        drawEllipse(&e);
    }
    for (Edge e:graph.edges)
    {
        Ellips *el1 = graph.getEllipseByNumber(e.from);
        Ellips *el2 = graph.getEllipseByNumber(e.to);
        if (el1!=NULL && el2!=NULL)
        {
            std::tuple<float,float,float,float> newCoords = graph.countEdgeCircleCoords(el1,el2);
            float x1,x2,y1,y2;
            std::tie(x1,y1,x2,y2) = newCoords;
            drawEdge(x1,y1,x2,y2);
        }
    }
    for (int i=0;i<graph.edges.size();i++)
    {
        graph.edgesToUpdateListMutex.lock();
        Ellips *el1 = graph.getEllipseByNumber(graph.edges[i].from);
        Ellips *el2 = graph.getEllipseByNumber(graph.edges[i].to);
        if (el1!=NULL && el2!=NULL)
        {
            std::tuple<float,float,float,float> newCoords = graph.countEdgeCircleCoords(el1,el2);
            float x1,x2,y1,y2;
            std::tie(x1,y1,x2,y2) = newCoords;
            graph.edges[i].toToEdgeData.x = (x1 * 9 + x2) / 10;
            graph.edges[i].toToEdgeData.y = (y1 * 9 + y2) / 10;
            graph.edges[i].toFromEdgeData.x = (x2 * 9 + x1) / 10;
            graph.edges[i].toFromEdgeData.y = (y2 * 9 + y1) / 10;;
            bool toToEdgeDefault = graph.edges[i].toToEdgeData.sendIntervalMS ==-1 && graph.edges[i].toToEdgeData.SendBytesPerInterval ==-1 && graph.edges[i].toToEdgeData.connectionBreakChance ==-1;
            bool toFromEdgeDefault = graph.edges[i].toFromEdgeData.sendIntervalMS ==-1 && graph.edges[i].toFromEdgeData.SendBytesPerInterval ==-1 && graph.edges[i].toFromEdgeData.connectionBreakChance ==-1;
            drawLableCircle(x1,y1,x2,y2, QString::number(graph.edges[i].loadFromTo),toToEdgeDefault);
            drawLableCircle(x2,y2,x1,y1, QString::number(graph.edges[i].loadToFrom),toFromEdgeDefault);
        }
        graph.edgesToUpdateListMutex.unlock();
    }
}

void OGLWidget::drawLableCircle(float x1, float y1, float x2,float y2, QString edgeUsage, bool defaultPerformance)
{
    float x = (x1*9+x2)/10;
    float y = (y1*9+y2)/10;
    int radius = 15;
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // center of circle
    for (int i = 0; i <= 20; i++)   {
        glVertex2f ((x + (radius * cos(i * float(2 * M_PI) / 20))), (y + (radius * sin(i * float(2 * M_PI) / 20))));
    }
    glEnd();
    radius++;
    if (graph.activeEdgeData!= nullptr && graph.activeEdgeData->x == x && graph.activeEdgeData->y == y)
    {
        glColor3f(1.0, 0.0, 0.0);
    }
    else{
        if (defaultPerformance){
            glColor3f(0.0, 0.0, 1.0);
        }
        else {
            glColor3f(0.0, 1.0, 0.0);
        }
    }
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 20; i++)   {
        glVertex2f ((x + (radius * cos(i * float(2 * M_PI) / 20))), (y + (radius * sin(i * float(2 * M_PI) / 20))) );
    }
    glEnd();

    glColor3f(0.0, 0.0, 1.0);
    float xx = (x*7+x2)/8;
    float yy = (y*7+y2)/8;
    float k = (yy - y2)/(xx - x2);
    float b = yy-xx*k;
    float k1 = (tan(45 * 3.14 / 180) + k)/(1 - tan(45 * 3.14 / 180) * k);//tan(120 * 3.14 / 180 + std::atan(k));
    float b1 = yy - xx * k1;
    float xx1 = xx - 30/(sqrt(1+k1*k1)) * (xx<x2 ? 1: -1 ) *( k1 < -1 ? -1 : 1);
    float yy1 = xx1 * k1 + b1;
    float k2 = -1/k;
    float b2 = yy1 - xx1 * k2;
    float n = (b2 - b) / (k - k2);
    float m = k2 * n + b2;
    float xx2 = 2 * n - xx1;
    float yy2 = 2 * m - yy1;

    glBegin(GL_LINE_LOOP);
    glVertex2f(xx, yy);
    glVertex2f(xx1, yy1);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex2f(xx, yy);
    glVertex2f(xx2, yy2);
    glEnd();
    if (edgeUsage == "-100"){
        edgeUsage = "  X";
    }
    renderText(x - 14,y + 5 ,edgeUsage, true, Qt::red);

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
        glColor3f(1, 0.8, 0);
    }
    if (e->colorStatus == 3)
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
    QString ellipseNum = QString::number(e->number);
    renderText(e->x-1,e->y-10,ellipseNum);
    QString ellipseCount = "Packets: " + QString::number(e->packetCount);
    renderText(e->x-24,e->y+0,ellipseCount);
    QString ellipseMaxCount = "MaxPackets: " + QString::number(e->maxPacketCount);
    renderText(e->x-34,e->y+10,ellipseMaxCount);
}

void OGLWidget::renderText(double x, double y, const QString &str, bool bold, QColor color)
{
    // Identify x and y locations to render text within widget
    int height = this->height();
    GLdouble textPosX = 0, textPosY = 0, textPosZ = 0;
    textPosY = height - textPosY; // y is inverted
    // Retrieve last OpenGL color to use as a font color

    // Render text
    QPainter painter(this);
    painter.setPen(color);
    QFont font("Helvetica", 8);
    font.setBold(bold);
    painter.setFont(font);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.drawText(x, y, str);
    painter.end();
}

void OGLWidget::drawEllipse(float xCenter, float yCenter, int color)      //color 0 - default, 1 - active ellipse
{
    float step = float(2 * M_PI/180);
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
    gluOrtho2D(0,this->width(),this->height(),0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void OGLWidget::mousePressEvent(QMouseEvent *event)
{
    if (!screenLocked && event->button() == Qt::LeftButton)
    {
        graph.activeEdgeData = nullptr;
        sim::sout<<event->x()<<"::"<<event->y()<<sim::endl;
        float x = event->x();
        float y = event->y();
        Ellips *el = graph.getEllipseByPoint(x,y);
        if (el!=NULL)
        {
            el->colorStatus=1;
            graph.active = el;
        } else{
            Edge *edge = nullptr;
            bool toFrom = false;
            std::tuple<Edge*,bool> EdgeD = graph.getEdgeByPoint(x,y);
            std::tie(edge,toFrom)  = EdgeD;
            if (edge!=nullptr)
            {
                if (!toFrom ) {
                    sim::sout << "EDGE from " << edge->from << " to " << edge->to << " found" << sim::endl;
                    graph.activeEdgeData = &edge->toToEdgeData;
                }
                else{
                    sim::sout << "EDGE from " << edge->to << " to " << edge->from << " found" << sim::endl;
                    graph.activeEdgeData = &edge->toFromEdgeData;
                }
            }
            else {
            }
        }
    }
    if (event->button() == Qt::RightButton)
    {
        graph.addEdge(-1);
    }
    emit(transmitActiveEdgeChange());
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
                sim::sout<<"TRUE"<<::sim::endl;
                deactivateNewEdge();
            }
        }
    }
}

