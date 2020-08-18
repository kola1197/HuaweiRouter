#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <QObject>



struct Ellips
{
    float x;
    float y;
    int number = -1;
    int colorStatus = 0; // 0 - orange - default but not connected, 1 - blue - active for movement, 2 - green - conected
};

struct Edge
{
    float from;
    float to;
};

class Graph: public QObject
{
Q_OBJECT

public:
    explicit Graph(QObject *parent = nullptr);
    std::vector<Ellips> ellipses;
    std::vector<Edge> edges;
    void testInit();
    void addEllips(float x, float y);
    Ellips *getEllipseByPoint(int x, int y);
    Ellips *active = NULL;
    void save(QString path);
    void load(QString path);
    bool addEdge(int number);
    Ellips *getEllipseByNumber(int num);
    int activeNumberForEdge = -1;
    void addEllips(float x, float y, int num);
    void deleteActiveEllips();
private:
    float dist(float x1, float y1, float x2, float y2);
    int ellipseCounter = 0;
    void deleteEllips(int number);
};

#endif // GRAPH_H
