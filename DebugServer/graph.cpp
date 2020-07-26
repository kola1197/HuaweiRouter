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

void Graph::testInit()
{
    addEllips(0.0,0.0);
    addEllips(200.0,500.0);
    addEllips(500.0,800.0);
    //addEllips(800.0,450.0);
    //addEllips(1600.0,900.0);
    //addEllips(2.0,2.0);
    //addEllips(-1.0,-1.0);
    //addEllips(-2.0,-2.0);
}

template<typename POD>
std::ostream& serialize(std::ostream& os, std::vector<POD> const& v)
{
    // this only works on built in data types (PODs)
    static_assert(std::is_trivial<POD>::value && std::is_standard_layout<POD>::value,
        "Can only serialize POD types with this function");

    auto size = v.size();
    os.write(reinterpret_cast<char const*>(&size), sizeof(size));
    os.write(reinterpret_cast<char const*>(v.data()), v.size() * sizeof(POD));
    return os;
}

template<typename POD>
std::istream& deserialize(std::istream& is, std::vector<POD>& v)
{
    static_assert(std::is_trivial<POD>::value && std::is_standard_layout<POD>::value,
        "Can only deserialize POD types with this function");
    decltype(v.size()) size;
    is.read(reinterpret_cast<char*>(&size), sizeof(size));
    v.resize(size);
    is.read(reinterpret_cast<char*>(v.data()), v.size() * sizeof(POD));
    return is;
}

void Graph::save(QString path)
{
    std::ofstream ofs;
    ofs.open(path.toStdString());
    for (int i=0;i<ellipses.size();i++)
    {
        ofs<<ellipses[i].x<<" "<<ellipses[i].y<<" "<<ellipses[i].number<<".\n";
    }
    ofs.close();
}

void Graph::load(QString path)
{
    std::string line;
    std::ifstream in(path.toStdString());
    ellipses.clear();
    if (in.is_open())
    {
        while( getline(in, line))
        {
            std::cout<< line<<'\n';
            QString q = QString::fromStdString(line);
            float arr[3];
            int counter = 0;
            QString buff = "";
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
            std::cout<<arr[0]<<" "<<arr[1]<<" "<<arr[2]<<std::endl;
            addEllips(arr[0],arr[1]);
        }
        in.close();
    }
    else
    {
        std::cout<<"unable load file";
    }
}

void Graph::addEllips(float x,float y)
{
    Ellips d;
    d.x = x;
    d.y = y;
    d.number = counter;
    counter++;
    ellipses.push_back(d);
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
