/*
 * ambient.h - Library for sending data to Ambient
 * Created by Takehiko Shimojima, April 21, 2016
 */
#ifndef Ambient3GIM_h
#define Ambient3GIM_h

#include "Arduino.h"
#include <SoftwareSerial.h>

#define AMBIENT_WRITEKEY_SIZE 18
#define AMBIENT_MAX_RETRY 5
#define AMBIENT_DATA_SIZE 24
#define AMBIENT_NUM_PARAMS 11

class Ambient
{
public:

    Ambient(void);

    bool begin(unsigned int channelId, const char * writeKey, SoftwareSerial * s, int dev = 0);
    bool set(int field, char * data);
    bool clear(int field);

    bool send(void);

private:

    SoftwareSerial * s;
    unsigned int channelId;
    char writeKey[AMBIENT_WRITEKEY_SIZE];
    int dev;
    char host[18];
    int port;

    struct {
        int set;
        char item[AMBIENT_DATA_SIZE];
    } data[AMBIENT_NUM_PARAMS];

    bool sendcmd(char * cmd);
};

#endif // Ambient3GIM_h

