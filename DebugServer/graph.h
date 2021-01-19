#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <QObject>
#include <SimulationCore/Messages.h>
#include <mutex>


struct Ellips
{
    float x;
    float y;
    bool connected = false;
    bool ready = false;  // all packets are loaded to Node
    bool serversReady = false;
    int number = -1;
    int maxPacketCount = 0;
    int packetCount = 0;
    int colorStatus = 0; // 0 - orange - default but not connected, 1 - blue - active for movement, 2 - yellow - conected, packets not loaded, 3 - green packets loaded, started //
    int pathLength =- 1;
};

struct Edge
{
    int id;
    int  from;
    int  to;
    float loadFromTo = 0;
    float loadToFrom = 0;
};

struct Packet
{
    int id;
    int from;
    int to;
    int currentPosition = -1;
    bool delivered = false;
    std::chrono::milliseconds timeOnCreation;
    MessageType type = MessageType::PACKET_MESSAGE;
};

enum Algorithms
{
    RANDOM = 0, DRILL = 1, DE_TAIL = 3, LOCAL_FLOW = 4, LOCAL_VOTING = 2, MY_LOCAL_VOTING = 5
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
    bool needReaintTable = false;

    std::vector<int> tableIndexesToUpdate;

    bool addEdge(int number);
    void addEdge(Edge e);

    void addEllips(float x, float y, int num);
    void addEllips(float x, float y);
    void addEllips(Ellips e);

    void deleteActiveEllips();

    std::vector<Packet> packets;
    void addPacket();
    void addPacket(Packet m);
    void addPacketmessage(int _type, int _from, int _to);

    int packetIdCounter = 0;
    float averageTime = 0;
    bool cpuCorrect = true;
    int cpuTermCriticalFrames = 0;
    int cpuLoadCriticalFrames = 0;
    int cpuFrames = 0;
    Algorithms selectedAlgorithm = Algorithms::RANDOM;

    std::mutex packetsToUpdateListMutex;
    void calculatePathLength(int nodeId);
public slots:
    void get_system_message(SystemMessage m);
    void get_system_message(DebugMessage m);
signals:
    void repaint();
    void updateTable();
private:
    std::mutex signalsMutex;
    float dist(float x1, float y1, float x2, float y2);
    int ellipseCounter = 0;
    int edgeCounter = 0;
    void deleteEllips(int number);

    void setPathLength(int nodeId);
};

#endif // GRAPH_H
