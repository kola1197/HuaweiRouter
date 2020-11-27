#include "oglwidget.h"
#include "iostream"
#include <QtOpenGL/qgl.h>
#include <QtOpenGL/QtOpenGL>
#include <GL/glu.h>
#include <GL/gl.h>
#include "TextureImage.h"

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
            //std::cout<<"Edge output "<<e.from<<" "<<e.to<<std::endl;
            //std::cout<<"Ellips "<<el1->x<<" "<<el1->y<<" -*- "<<el2->x<<" "<<el2->y<<std::endl;
            std::tuple<float,float,float,float> newCoords = countCoords(el1,el2);
            float x1,x2,y1,y2;
            std::tie(x1,y1,x2,y2) = newCoords;
            drawEdge(x1,y1,x2,y2);
            drawLableCircle(x1,y1,x2,y2, QString::number(e.loadToFrom));
            drawLableCircle(x2,y2,x1,y1, QString::number(e.loadFromTo));

            //drawEdge(el1->x,el1->y,el2->x,el2->y);
        }
    }
}

void OGLWidget::drawLableCircle(float x1, float y1, float x2,float y2, QString edgeUsage)
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
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 20; i++)   {
        glVertex2f ((x + (radius * cos(i * float(2 * M_PI) / 20))), (y + (radius * sin(i * float(2 * M_PI) / 20))) );
    }
    glEnd();
    //QString edgeUsage("100%");
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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"
std::tuple<float,float,float,float> OGLWidget::countCoords(Ellips* el1, Ellips* el2)
{
    float x1 = el1->x;
    float x2 = el2->x;
    float y1 = el1->y;
    float y2 = el2->y;
    if (x1!=x2) {
        float k = (y1 - y2) / (x1 - x2);
        float b = el1->y - el1->x * k;

        float a = el1->x;
        float c = el1->y;
        float resX1 = (a - 4*b*k +4*c*k - sign(el1->x - el2->x)*2*sqrt(-a*a*k*k -2*a*b*k + 2*a*c*k -b*b +2*b*c - c*c + 2500*k*k +625))
                / (4 * k * k +1);
        float resY1 = resX1 * k + b;

        a = el2->x;
        c = el2->y;
        float resX2 = (a - 4*b*k +4*c*k - sign(el2->x - el1->x)*2*sqrt(-a*a*k*k -2*a*b*k + 2*a*c*k -b*b +2*b*c - c*c + 2500*k*k +625))
                      / (4 * k * k +1);
        float resY2 = resX2 * k + b;


        //float resX = (-8*b*k + sign(el2->x - el1->x) * sqrt(64 * k * k * b * b - 4 * ( 4 * (4 * k * k + 1)) * ( 4 * b * b - 50 * 50 )))
        //        /(2*(4*k*k+1));
        //float resX1 = resX + el1->x;
        //float resY1 = k * resX1 + b;
        //float resX2 = resX + el2->x;
        //float resY2 = k * resX2 + b;
        return std::tuple<float,float, float, float>(resX1, resY1, resX2, resY2 );
    } else {
        return std::tuple<float,float, float, float>(el1->x, el1->y + sign(el2->y - el1->y) * 25,el2->x , el2->y + sign(el1->y - el2->y) * 25);
    }
}
#pragma clang diagnostic pop



int OGLWidget::sign(float i)
{
    return i>0 ? 1 : -1;
}

void OGLWidget::renderText(double x, double y, const QString &str, bool bold, QColor color)
{
    // Identify x and y locations to render text within widget
    int height = this->height();
    GLdouble textPosX = 0, textPosY = 0, textPosZ = 0;
    //project(x, y, 0, &textPosX, &textPosY, &textPosZ);
    textPosY = height - textPosY; // y is inverted

    // Retrieve last OpenGL color to use as a font color

    // Render text
    QPainter painter(this);
    //painter.setPen(color);
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

