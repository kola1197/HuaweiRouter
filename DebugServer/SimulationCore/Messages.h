//
// Created by nickolay on 10.06.2020.
//

#ifndef DRONEAPP_MESSAGES_H
#define DRONEAPP_MESSAGES_H

#include <string>
#include <chrono>

#define int64 int64_t

//new generation
enum Priority {
    LOW=0, MEDIUM=1, HIGH=2
};

enum MessageType {MESSAGE=0,PACKET_MESSAGE=1, PING_MESSAGE=2, TEST_MESSAGE=3, SYSTEM_MESSAGE=4, DEBUG_MESSAGE=5, NODE_LOAD_MESSAGE = 6};

struct HarbingerMessage {                                           //sends before other messages, to set resiver to it
    //enum MessageType {MESSAGE=0,PACKET_MESSAGE=1, PING_MESSAGE=2, TEST_MESSAGE=3, SYSTEM_MESSAGE=4, DEBUG_MESSAGE=5};
    MessageType type;
    int code;
};

struct Message
{
    //enum MessageType {MESSAGE=0,PACKET_MESSAGE=1, PING_MESSAGE=2, TEST_MESSAGE=3, SYSTEM_MESSAGE=4, DEBUG_MESSAGE=5};
    Priority priority = Priority::MEDIUM;
    int id;
    int to;
    int from;
    std::chrono::milliseconds timeOnCreation;
    std::chrono::milliseconds deliveredToThisNode;
    MessageType type = MessageType::MESSAGE;
};

struct PacketMessage:Message{
    int firstCheckSum = 0;
    Priority priority = Priority::MEDIUM;
    int currentPosition;
    bool delivered;
    char uselessData [23900] = {0};
    int prevposition = -1;
    std::chrono::milliseconds timeOnCreation;
    MessageType type = MessageType::PACKET_MESSAGE;
    int checkSum = 0;
};

//enum Function {PING_MESSAGE, TEST_MESSAGE, SYSTEM_MESSAGE, DEBUG_MESSAGE, PACKET_MESSAGE};
struct SystemMessage:Message {
    enum Function {TEXT_ALLERT, START_SIMULATION_FLAG, SERVERS_READY, DEBUG_SERVER_READY};
    char text[200];
    int i[8];
    int authorNum;
    Function function;
    MessageType type = MessageType::SYSTEM_MESSAGE;
};

struct DebugMessage:Message{
    enum Function {CONNECTION_STATUS, PACKET_STATUS, PACKET_STATUS_DELIVERED, PACKET_COUNT_STATUS, EDGES_USAGE_STATUS};
    //char text[200];
    int i[200];
    Function function;
    std::chrono::milliseconds deliveringTime;
    MessageType type = MessageType::DEBUG_MESSAGE;
};

struct PingMessage:Message{
    int64 time[2];
    MessageType type = MessageType::PING_MESSAGE;
};

struct TestMessage:Message{
    char text[200];
    MessageType type = MessageType::TEST_MESSAGE;
    int checkCode = 0;
};

struct NodeLoadMessage:Message{
    float load;
    MessageType type = MessageType::NODE_LOAD_MESSAGE;
    Priority priority = Priority::HIGH;
};


class Messages {
public:
    static bool getMessageTypeByName(std::string name,MessageType * type) {
        if (name == typeid(SystemMessage).name())
        {
            *type = MessageType::SYSTEM_MESSAGE;
            return true;
        }
        if (name == typeid(DebugMessage).name())
        {
            *type = MessageType::DEBUG_MESSAGE;
            return true;
        }
        if (name == typeid(PingMessage).name())
        {
            *type = MessageType::PING_MESSAGE;
            return true;
        }
        if (name == typeid(TestMessage).name())
        {
            *type = MessageType::TEST_MESSAGE;
            return true;
        }
        if (name == typeid(PacketMessage).name())
        {
            *type = MessageType::PACKET_MESSAGE;
            return true;
        }
        return false;
    }
};
//class Messages {

//};


#endif //DRONEAPP_MESSAGES_H
