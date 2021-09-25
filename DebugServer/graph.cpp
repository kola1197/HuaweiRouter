//
// This class contains graph for simulation and list of packets, it will be sent to all serverNodes, so they will know all topology
//


#include "graph.h"
#include "cmath"
#include "iostream"
#include "fstream"

#include <QFileDialog>
#include <QString>
#include <Utils/sout.h>


Graph::Graph(QObject *parent) : QObject(parent)
{
    testInit();
}

Graph& Graph::operator = (const Graph &obj)
{
    ellipses.clear();
    edges.clear();
    packets.clear();
    edgeCounter = 0;
    ellipseCounter = 0;
    packetIdCounter = 0;
    for (int i=0;i<obj.ellipses.size();i++)
    {
        addEllips(obj.ellipses[i]);
    }
    for (int i=0;i<obj.edges.size();i++)
    {
        addEdge(obj.edges[i]);
    }
    for (int i=0;i<obj.packets.size();i++)
    {
        addPacket(obj.packets[i]);
    }
    this->selectedAlgorithm = obj.selectedAlgorithm;
    return *this;
}

Graph::Graph(const Graph &obj)
{
    ellipses.clear();
    edges.clear();
    packets.clear();
    edgeCounter = 0;
    ellipseCounter = 0;
    packetIdCounter = 0;
    for (int i=0;i<obj.ellipses.size();i++)
    {
        addEllips(obj.ellipses[i]);
    }
    for (int i=0;i<obj.edges.size();i++)
    {
        addEdge(obj.edges[i]);
    }
    for (int i=0;i<obj.packets.size();i++)
    {
        addPacket(obj.packets[i]);
    }
    this->selectedAlgorithm = obj.selectedAlgorithm;
}

void Graph::testInit()
{

}



void Graph::save(QString path)
{
    std::ofstream ofs;
    path+=".grf";
    ofs.open(path.toStdString());
    for (int i=0;i<ellipses.size();i++)
    {
        ofs<<ellipses[i].x<<" "<<ellipses[i].y<<" "<<ellipses[i].number<<".\n";
    }
    ofs<<"*.\n";
    for (int i=0;i<edges.size();i++)
    {
        ofs<<edges[i].from<<" "<<edges[i].to<<" "<<edges[i].toToEdgeData.sendIntervalMS<<" "<<edges[i].toToEdgeData.SendBytesPerInterval<<" "<<edges[i].toToEdgeData.connectionBreakChance
           <<" "<<edges[i].toFromEdgeData.sendIntervalMS<<" "<<edges[i].toFromEdgeData.SendBytesPerInterval<<" "<<edges[i].toFromEdgeData.connectionBreakChance<<".\n";
    }
    ofs<<"*.\n";
    for (int i = 0; i<packets.size();i++)
    {
        ofs<<packets[i].type<<" "<<packets[i].from<<" "<<packets[i].to<<".\n";
    }
    ofs.close();
}

void Graph::load(QString path)
{
    packetIdCounter = 0;
    std::string line;
    std::ifstream in(path.toStdString());
    ellipses.clear();
    edges.clear();
    packets.clear();
    ellipseCounter = 0;
    if (in.is_open())
    {
        int loadPart = 0;       //0 - ellipses, 1 - edges, 2 - PacketMessages
        while( getline(in, line))
        {
            sim::sout<< line<<'\n';
            QString q = QString::fromStdString(line);
            float arr[8];
            for (int i = 2;i<8;i++){
                arr[i] = -1;
            }
            int counter = 0;
            QString buff = "";
            if (q[0]!='*')
            {
                bool belowZero = false;
                for (int j=0;j<6;j++){arr[j] = -1;}
                for (int i=0;i<q.length();i++)
                {
                    if (q[i].isDigit())
                    {
                        buff += q[i];
                    }
                    else if(q[i] == '-'){
                        belowZero = true;
                    }
                    else{
                        arr[counter] = belowZero ? -buff.toFloat() : buff.toFloat();
                        belowZero = false;
                        buff = "";
                        counter++;
                    }
                }
                if (loadPart == 0)
                {
                    addEllips(arr[0],arr[1],arr[2]);
                }
                if (loadPart == 1)
                {
                    Edge edge;
                    edge.from = (int)arr[0];
                    edge.to = (int)arr[1];
                    edge.toToEdgeData.sendIntervalMS = arr[2];
                    edge.toToEdgeData.SendBytesPerInterval = arr[3];
                    edge.toToEdgeData.connectionBreakChance = (int) arr[4];
                    edge.toFromEdgeData.sendIntervalMS = arr[5];
                    edge.toFromEdgeData.SendBytesPerInterval = arr[6];
                    edge.toFromEdgeData.connectionBreakChance = (int) arr[7];
                    addEdge(edge);
                }
                if (loadPart == 2)
                {
                    addPacketmessage(arr[0],arr[1],arr[2]);
                }
            }
            else{
                loadPart++;
            }
        }
        in.close();
        sim::sout<<"File loaded"<<sim::endl;
    }
    else
    {
        sim::sout<<"unable load file"<<sim::endl;
    }
}


int Graph::sign(float i)
{
    return i>0 ? 1 : -1;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"
std::tuple<float,float,float,float> Graph::countEdgeCircleCoords(Ellips* el1, Ellips* el2)
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

        return std::tuple<float,float, float, float>(resX1, resY1, resX2, resY2 );
    } else {
        return std::tuple<float,float, float, float>(el1->x, el1->y + sign(el2->y - el1->y) * 25,el2->x , el2->y + sign(el1->y - el2->y) * 25);
    }
}
#pragma clang diagnostic pop


void Graph::addPacketmessage(int _type, int _from, int _to)
{
    Packet m;
    m.id = packetIdCounter;
    packetIdCounter++;
    m.from = _from;
    m.to = _to;
    m.currentPosition = -1;
    m.delivered = false;
    packets.push_back(m);
    packetsToUpdateListMutex.lock();
    tableIndexesToUpdate.push_back(packets.size()-1);
    packetsToUpdateListMutex.unlock();
}

void Graph::addEllips(float x,float y)
{
    addEllips(x,y,ellipseCounter);
}

void Graph::addEllips(Ellips e)
{
    addEllips(e.x,e.y,e.number);
}

void Graph::addEllips(float x,float y, int num)
{
    Ellips d;
    d.x = x;
    d.y = y;
    d.number = num;
    if (num >= ellipseCounter)
    {
        ellipseCounter = num + 1;
    }
    ellipses.push_back(d);
}

void Graph::addPacket(Packet m)
{
    Packet p;
    p.id = m.id;
    p.from = m.from;
    p.to = m.to;
    p.delivered = m.delivered;
    p.timeOnCreation = m.timeOnCreation;
    packets.push_back(p);
    packetsToUpdateListMutex.lock();
    tableIndexesToUpdate.push_back(packets.size()-1);
    packetsToUpdateListMutex.unlock();
}

Ellips * Graph::getEllipseByNumber(int num)
{
    Ellips *result;
    result = NULL;
    for (int i=0;i<ellipses.size();i++)
    {
        if ( ellipses[i].number == num )
        {
            result = &ellipses[i];
        }
    }
    return result;
}

std::tuple<Edge*, bool> Graph::getEdgeByPoint(int x,int y)
{
    Edge * resultEdge = nullptr;
    bool from;
    for (auto & edge : edges)
    {
        if (dist(x, y, edge.toFromEdgeData.x, edge.toFromEdgeData.y) < 15 )
        {
            from = true;
            sim::sout<<"Edge "<<edge.to<<" to "<<edge.from<<sim::endl;
            resultEdge = &edge;
        }
        if (dist(x, y, edge.toToEdgeData.x, edge.toToEdgeData.y) < 15 )
        {
            from = false;
            sim::sout<<"Edge "<<edge.from<<" to "<<edge.to<<sim::endl;
            resultEdge = &edge;
        }
    }
    std::tuple<Edge*,bool> result {resultEdge,from};
    return result;
}


Ellips * Graph::getEllipseByPoint(int x,int y)
{
    Ellips *result;
    result = NULL;
    for (int i=0;i<ellipses.size();i++)
    {
        if ( dist(x,y,ellipses[i].x,ellipses[i].y) + dist(x,y,ellipses[i].x,ellipses[i].y) < 100 )
        {
            sim::sout<<ellipses[i].x<<"*"<<ellipses[i].y<<sim::endl;
            result = &ellipses[i];
        }
    }
    return result;
}

float Graph::dist(float x1,float y1,float x2,float y2)
{
    float result = std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
    return result;
}

void Graph::addEdge(Edge e)
{
    e.id = edgeCounter;
    edgeCounter++;
    edges.push_back(e);
}

bool Graph::addEdge(int number)
{
    bool result = false;
    if (activeNumberForEdge == -1)
    {
        activeNumberForEdge = number;
    }
    else{
        if (number != -1)
        {
            Edge e;
            e.id = edgeCounter;
            edgeCounter++;
            e.from = activeNumberForEdge;
            e.to = number;
            edges.push_back(e);
            activeNumberForEdge = -1;
            result = true;
        }
        else
        {
            activeNumberForEdge = -1;
        }
    }
    return result;
}

void Graph::deleteActiveEllips()
{
    deleteEllips(activeNumberForEdge);
}

void Graph::deleteEllips(int number)
{
    for (int i=0;i<ellipses.size();i++)
    {
        if (ellipses[i].number == number)
        {
            sim::sout<<"Ellips "<<ellipses[i].number<<"deleted"<<sim::endl;
            ellipses.erase(ellipses.begin()+i);
        }
    }
    int counter = 0;
    bool found = true;
    while (found) {
        found = false;
        for (int j = 0; j < edges.size(); j++) {
            if (edges[j].to == number || edges[j].from == number) {
                if (!found) {
                    edges.erase(edges.begin() + j);
                    found = true;
                    counter ++;
                }
            }
        }
    }
    activeNumberForEdge = -1;
}

void Graph::get_system_message(SystemMessage m)
{
    sim::sout<<m.authorNum<<" send system message "<<sim::endl;
}

void Graph::get_system_message(DebugMessage m)
{
    if (m.checksum == 239239239) {
        signalsMutex.lock();
        if (m.function == DebugMessage::CONNECTION_STATUS) {
            Ellips *e = getEllipseByNumber(m.i[0]);
            e->connected = m.i[1] == 1;
            e->colorStatus = m.i[1];
            emit repaint();
        }
        packetsToUpdateListMutex.lock();
        if (m.function == DebugMessage::PACKET_STATUS) {
            for (int i = 0; i < packets.size(); i++) {
                if (packets[i].id == m.i[0] && !packets[i].delivered) {
                    packets[i].currentPosition = m.i[1];
                    tableIndexesToUpdate.push_back(i);
                }
            }
        }
        if (m.function == DebugMessage::PACKET_STATUS_DELIVERED) {
            for (int i = 0; i < packets.size(); i++) {
                if (packets[i].id == m.i[0]) {
                    packets[i].currentPosition = m.i[1];
                    packets[i].delivered = true;
                    packets[i].timeOnCreation = m.deliveringTime;
                }
            }
            emit updateTable();
        }
        if (m.function == DebugMessage::PACKET_COUNT_STATUS) {
            Ellips *e = getEllipseByNumber(m.i[0]);
            e->packetCount = m.i[1];
            e->maxPacketCount = m.i[2];
            emit repaint();
        }
        packetsToUpdateListMutex.unlock();
        if (m.function == DebugMessage::EDGES_USAGE_STATUS) {
            edgesToUpdateListMutex.lock();
            int count = m.i[0];
            for (int j = 0; j < count; j++) {
                for (int i = 0; i < edges.size(); i++) {
                    if (edges[i].id == m.i[j * 3 + 1]) {
                        if (edges[i].from == m.i[j * 3 + 2]) {
                            edges[i].loadFromTo = m.i[j * 3 + 3];
                            edges[i].maxLoadFromTo = edges[i].maxLoadFromTo > edges[i].loadFromTo ? edges[i].maxLoadFromTo : edges[i].loadFromTo;
                        }
                        if (edges[i].to == m.i[j * 3 + 2]) {
                            edges[i].loadToFrom = m.i[j * 3 + 3];
                            edges[i].maxLoadToFrom = edges[i].maxLoadToFrom > edges[i].loadToFrom ? edges[i].maxLoadToFrom : edges[i].loadToFrom;
                        }
                    }
                }
            }
            edgesToUpdateListMutex.unlock();
            emit repaint();
        }
        signalsMutex.unlock();
    }
    else {
        sim::sout<<"DebugMessage checksum error"<<sim::endl;
    }
}

void Graph::addPacket()
{
    Packet m;
    m.id = packetIdCounter;
    packetIdCounter++;
    m.from = -1;
    m.to = -1;
    m.currentPosition = -1;
    m.timeOnCreation;
    m.delivered = false;
    packets.push_back(m);
    packetsToUpdateListMutex.lock();
    tableIndexesToUpdate.push_back(packets.size()-1);
    packetsToUpdateListMutex.unlock();
}

void Graph::calculatePathLength(int nodeId)
{
    for (int i=0; i < ellipses.size();i++)
    {
        ellipses[i].pathLength = 88888;
    }
    Ellips *e = getEllipseByNumber(nodeId);
    e->pathLength=0;
    setPathLength(nodeId);
}

void Graph::setPathLength(int nodeId)
{
    Ellips *e = getEllipseByNumber(nodeId);
    for (int i=0; i< edges.size();i++)
    {
        if (edges[i].from == nodeId)
        {
            Ellips *ee = getEllipseByNumber(edges[i].to);
            if (ee->pathLength > e->pathLength+1)
            {
                ee->pathLength = e->pathLength+1;
                setPathLength(ee->number);
            }
        }
        if (edges[i].to == nodeId)
        {
            Ellips *ee = getEllipseByNumber(edges[i].from);
            if (ee->pathLength > e->pathLength+1)
            {
                ee->pathLength = e->pathLength+1;
                setPathLength(ee->number);
            }
        }
    }
}