#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <QObject>
#include <SimulationCore/Messages.h>


struct Ellips
{
    float x;
    float y;
    bool connected = false;
    bool ready = false;  // all packets are loaded to Node
    bool serversReady = false;
    int number = -1;
    int packetCount = 0;
    int colorStatus = 0; // 0 - orange - default but not connected, 1 - blue - active for movement, 2 - yellow - conected, packets not loaded, 3 - green packets loaded, started //
};

struct Edge
{
    int id;
    int  from;
    int  to;
};

class Graph: public QObject
{
Q_OBJECT

public:
    explicit Graph(QObject *parent = nullptr);
    Graph(const Graph &obj);
    Graph& operator = (const Graph &obj);

    std::vector<Ellips> ellipses;
    std::vector<Edge> edges;
    void testInit();
    Ellips *getEllipseByPoint(int x, int y);
    Ellips *active = NULL;
    void save(QString path);
    void load(QString path);
    Ellips *getEllipseByNumber(int num);
    int activeNumberForEdge = -1;

    bool addEdge(int number);
    void addEdge(Edge e);

    void addEllips(float x, float y, int num);
    void addEllips(float x, float y);
    void addEllips(Ellips e);

    void deleteActiveEllips();

    std::vector<PacketMessage> packets;
    void addPacket();
    void addPacket(PacketMessage m);
    void addPacketmessage(int _type, int _from, int _to);
    int packetIdCounter = 0;
public slots:
    void get_system_message(SystemMessage m);
    void get_system_message(DebugMessage m);
signals:
    void repaint();
    void updateTable();
private:
    float dist(float x1, float y1, float x2, float y2);
    int ellipseCounter = 0;
    int edgeCounter = 0;
    void deleteEllips(int number);
};

#endif // GRAPH_H
