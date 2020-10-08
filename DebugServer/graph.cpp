#include "graph.h"
#include "cmath"
#include "iostream"
#include "fstream"

#include <QFileDialog>
#include <QString>


Graph::Graph(QObject *parent) : QObject(parent)
{
    testInit();
}

Graph& Graph::operator = (const Graph &obj)
{
    ellipses.clear();
    edges.clear();
    packets.clear();
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
    return *this;
}

Graph::Graph(const Graph &obj)
{
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
}

void Graph::testInit()
{
//    addEllips(0.0,0.0);
//    addEllips(200.0,500.0);
//    addEllips(500.0,800.0);
//    addEdge(0);
//    addEdge(1);
//    addEdge(2);
//    addEdge(1);

    //addEllips(800.0,450.0);
    //addEllips(1600.0,900.0);
    //addEllips(2.0,2.0);
    //addEllips(-1.0,-1.0);
    //addEllips(-2.0,-2.0);
}

//template<typename POD>
//std::ostream& serialize(std::ostream& os, std::vector<POD> const& v)
//{
//    // this only works on built in data types (PODs)
//    static_assert(std::is_trivial<POD>::value && std::is_standard_layout<POD>::value,
//        "Can only serialize POD types with this function");

//    auto size = v.size();
//    os.write(reinterpret_cast<char const*>(&size), sizeof(size));
//    os.write(reinterpret_cast<char const*>(v.data()), v.size() * sizeof(POD));
//    return os;
//}

//template<typename POD>
//std::istream& deserialize(std::istream& is, std::vector<POD>& v)
//{
//    static_assert(std::is_trivial<POD>::value && std::is_standard_layout<POD>::value,
//        "Can only deserialize POD types with this function");
//    decltype(v.size()) size;
//    is.read(reinterpret_cast<char*>(&size), sizeof(size));
//    v.resize(size);
//    is.read(reinterpret_cast<char*>(v.data()), v.size() * sizeof(POD));
//    return is;
//}

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
        ofs<<edges[i].from<<" "<<edges[i].to<<".\n";
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
            std::cout<< line<<'\n';
            QString q = QString::fromStdString(line);
            float arr[3];
            int counter = 0;
            QString buff = "";
            if (q[0]!='*')
            {
                for (int i=0;i<q.length();i++)
                {

                    if (q[i].isDigit())
                    {
                        buff += q[i];
                    }
                    else{
                        arr[counter] = buff.toFloat();
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
                    //std::cout<<" "<<arr[0]<<" "<<arr[1]<<std::endl;
                    addEdge((int)arr[0]);
                    addEdge((int)arr[1]);
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
    }
    else
    {
        std::cout<<"unable load file";
    }
}

void Graph::addPacketmessage(int _type, int _from, int _to)
{
    PacketMessage m;
    m.id = packetIdCounter;
    packetIdCounter++;
    if (_type == 0)
    {
        m.type = PacketMessage::DEFAULT_PACKET;
    }
    m.from = _from;
    m.to = _to;
    m.currentPosition = -1;
    packets.push_back(m);
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

void Graph::addPacket(PacketMessage m)
{
    PacketMessage p;
    p.id = m.id;
    p.type = m.type;
    p.from = m.from;
    p.to = m.to;
    packets.push_back(p);
}

Ellips * Graph::getEllipseByNumber(int num)
{
    Ellips *result;
    result = NULL;
    for (int i=0;i<ellipses.size();i++)
    {
        if ( ellipses[i].number == num )
        {
            //std::cout<<ellipses[i].x<<"*"<<ellipses[i].y<<std::endl;
            result = &ellipses[i];
        }
    }
    return result;
}

Ellips * Graph::getEllipseByPoint(int x,int y)
{
    Ellips *result;
    //Ellips q;
    //q.x=-239;
    //q.y=-239;
    result = NULL;
    for (int i=0;i<ellipses.size();i++)
    {
        if ( dist(x,y,ellipses[i].x,ellipses[i].y) + dist(x,y,ellipses[i].x,ellipses[i].y) < 100 )
        {
            std::cout<<ellipses[i].x<<"*"<<ellipses[i].y<<std::endl;
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
    edges.push_back(e);
//    addEdge(e.from);
//    addEdge(e.to);
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
            ellipses.erase(ellipses.begin()+i);
        }
    }
}

void Graph::get_system_message(SystemMessage m)
{
    std::cout<<m.authorNum<<" send system message "<<std::endl;
}

void Graph::get_system_message(DebugMessage m)
{
    if (m.type = DebugMessage::CONNECTION_STATUS)
    {
        Ellips *e = getEllipseByNumber(m.i[0]);
        e->connected = m.i[1]==1;
        e->colorStatus=m.i[1];
        emit repaint();
    }
    if (m.type = DebugMessage::PACKET_STATUS)
    {
        //std::cout<<"GOT PACKET STATUS "<<m.i[0]<<"   "<<m.i[1]<<std::endl;
        for (int i = 0;i < packets.size();i++)
        {
            if (packets[i].id == m.i[0])
            {
                packets[i].currentPosition = m.i[1];
            }
        }
        //emit repaint();
        emit updateTable();
    }
    //std::cout<<m.i[0]<<" send connection status "<<m.i[1]<<std::endl;
}

void Graph::addPacket()
{
    PacketMessage m;
    m.id = packetIdCounter;
    packetIdCounter++;
    m.type = PacketMessage::DEFAULT_PACKET;
    m.from = -1;
    m.to = -1;
    m.currentPosition = -1;
    packets.push_back(m);
    emit repaint();
}
