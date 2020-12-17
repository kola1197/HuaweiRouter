//
// Created by nickolay on 10.06.2020.
//

#ifndef DRONEAPP_MESSAGES_H
#define DRONEAPP_MESSAGES_H

#include <string>
#include <chrono>

#define int64 int64_t

struct HarbingerMessage {                                           //sends before other messages, to set resiver to it
    enum Type {PING_MESSAGE, TEST_MESSAGE, SYSTEM_MESSAGE, DEBUG_MESSAGE, PACKET_MESSAGE};
    Type type;
    int code;
};

//struct MessageWithImage {                                           //get image from vehicle
//    enum Type {LEFT_IMAGE, RIGHT_IMAGE};
//    char text[200];
//    int i;
//    int height;
//    int width;
//    int dataSize;
//    //cv::Mat image;
//    uchar imData [240400];                                                  //230400   //2352000
//    Type type;
//};

//struct MessageWithGrayImage {                                           //get image from vehicle
//    enum Type {LEFT_IMAGE, RIGHT_IMAGE};
//    char text[200];
//    int i;
//    int height;
//    int width;
//    int dataSize;
//    //cv::Mat image;
//    uchar imData [100000];                                               //100000   //230400   //2352000
//    Type type;
//};

struct SystemMessage {
    enum Type {TEXT_ALLERT, START_SIMULATION_FLAG, SERVERS_READY, DEBUG_SERVER_READY};
    char text[200];
    int i[8];
    int authorNum;
    Type type;
};

struct DebugMessage{
    enum Type {CONNECTION_STATUS, PACKET_STATUS, PACKET_STATUS_DELIVERED, PACKET_COUNT_STATUS, EDGES_USAGE_STATUS};
    //char text[200];
    int i[200];
    Type type;
    std::chrono::milliseconds deliveringTime;
};

struct PingMessage{
    int64 time[2];
};

struct TestMessage{
    char text[200];
    //std::string testTexst;
};



//new generation
enum Priority {
    LOW, MEDIUM, HIGH
};

struct PacketMessage{
    enum Type {DEFAULT_PACKET=0};
    Priority priority = Priority::MEDIUM;
    int from;
    int to;
    int id;
    int currentPosition;
    bool delivered;
    char uselessData [239000];
    int checkSum = 0;
    std::chrono::milliseconds timeOnCreation;
    Type type;
};

/*struct Message
{
    Priority priority = Priority::MEDIUM;
    int id;
    int to;
    int from;
    std::chrono::milliseconds timeOnCreation;
    std::chrono::milliseconds deliveredToThisNode;
};

struct PMessage:Message
{

};*/
//enum Type {PING_MESSAGE, TEST_MESSAGE, SYSTEM_MESSAGE, DEBUG_MESSAGE, PACKET_MESSAGE};

class Messages {
public:
    static bool getMessageTypeByName(std::string name,HarbingerMessage::Type * type) {
        if (name == typeid(SystemMessage).name())
        {
            *type = HarbingerMessage::Type::SYSTEM_MESSAGE;
            return true;
        }
        if (name == typeid(DebugMessage).name())
        {
            *type = HarbingerMessage::Type::DEBUG_MESSAGE;
            return true;
        }
        if (name == typeid(PingMessage).name())
        {
            *type = HarbingerMessage::Type::PING_MESSAGE;
            return true;
        }
        if (name == typeid(TestMessage).name())
        {
            *type = HarbingerMessage::Type::TEST_MESSAGE;
            return true;
        }
        if (name == typeid(PacketMessage).name())
        {
            *type = HarbingerMessage::Type::PACKET_MESSAGE;
            return true;
        }
        return false;
    }
};
//class Messages {

//};


#endif //DRONEAPP_MESSAGES_H
