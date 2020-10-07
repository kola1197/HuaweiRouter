//
// Created by nickolay on 10.06.2020.
//

#ifndef DRONEAPP_MESSAGES_H
#define DRONEAPP_MESSAGES_H

#include <string>

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
    enum Type {TEXT_ALLERT};
    char text[200];
    int i[8];
    int authorNum;
    Type type;
};

struct DebugMessage{
    enum Type {CONNECTION_STATUS, PACKET_STATUS};
    char text[200];
    int i[8];
    Type type;
};

struct PingMessage{
    int64 time[2];
};

struct TestMessage{
    char text[200];
    //std::string testTexst;
};

struct PacketMessage{
    enum Type {DEFAULT_PACKET=0};
    int from;
    int to;
    int id;
    int currentPosition;
    Type type;
};

//class Messages {

//};


#endif //DRONEAPP_MESSAGES_H
