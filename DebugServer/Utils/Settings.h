//
// Created by nickolay on 28.09.2020.
//
#pragma ones
#ifndef HUAWEIROUTER_SETTINGS_H
#define HUAWEIROUTER_SETTINGS_H
class Settings

{
    static int DebugFirstPortNum;
    static int ConnectionsFirstPortNum;
    static float SendBytesPerInterval;
    static float sendIntervalMS;
    static float limitForDeTail;
    static int alpha;
    static float localFowConnectionLoadLimit;
    static int connectionBreakChance;
public:
    static int getDebugFirstPortNum()
    {
        return DebugFirstPortNum;
    };
    static int getConnectionsFirstPortNum()
    {
        return ConnectionsFirstPortNum;
    };
    static void setDebugFirstPortNum(int i)
    {
        DebugFirstPortNum = i;
    }
    static void setConnectionsFirstPortNum(int i)
    {
        ConnectionsFirstPortNum = i;
    }
    static float getSendBytesPerInterval()
    {
        return SendBytesPerInterval;
    }
    static void setSendBytesPerInterval(float i)
    {
        SendBytesPerInterval = i;
    }
    static float getsendIntervalMS()
    {
        return sendIntervalMS;
    }
    static void setsendIntervalMS(float i)
    {
        sendIntervalMS = i;
    }
    static void setlimitForDeTail(float i)
    {
        limitForDeTail = i;
    }
    static float getLimitForDeTail()
    {
        return limitForDeTail;
    }
    static int getAlpha()
    {
        return alpha;
    };
    static void setAlpha(int i)
    {
        alpha = i;
    }
    static float getLocalFowConnectionLoadLimit()
    {
        return localFowConnectionLoadLimit;
    };
    static void setLocalFowConnectionLoadLimit(float i)
    {
        localFowConnectionLoadLimit = i;
    }

    static int getConnectionBreakChance()
    {
        return connectionBreakChance;
    };
    static void setConnectionBreakChance(int i)
    {
        connectionBreakChance = i;
    }

};







#endif //HUAWEIROUTER_SETTINGS_H
