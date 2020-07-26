#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <QObject>



struct Ellips
{
    float x;
    float y;
    int number = -1;
};

class Graph: public QObject
{
Q_OBJECT

public:
    explicit Graph(QObject *parent = nullptr);
    std::vector<Ellips> ellipses;
    void testInit();
    void addEllips(float x, float y);
    Ellips *getEllipseByPoint(int x, int y);
    Ellips *active = NULL;
    void save(QString path);
    void load(QString path);
private:
    float dist(float x1, float y1, float x2, float y2);
    int counter = 0;
};

#endif // GRAPH_H
