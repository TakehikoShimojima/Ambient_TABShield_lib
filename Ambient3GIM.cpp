/*
 * ambient.h - Library for sending data to Ambient
 * Created by Takehiko Shimojima, April 21, 2016
 */
#include "Ambient3GIM.h"
#define LIMITTIME 35000 // ms (3G module start time)

#define AMBIENT_DEBUG 0

#if AMBIENT_DEBUG
#define DBG(...) { Serial.print(__VA_ARGS__); }
#define ERR(...) { Serial.print(__VA_ARGS__); }
#else
#define DBG(...)
#define ERR(...)
#endif /* AMBIENT_DBG */

const char* AMBIENT_HOST = "54.65.206.59";
int AMBIENT_PORT = 80;
const char* AMBIENT_HOST_DEV = "192.168.0.8";
int AMBIENT_PORT_DEV = 4567;

const char * ambient_keys[] = {"\$\"d1\$\":\$\"", "\$\"d2\$\":\$\"", "\$\"d3\$\":\$\"", "\$\"d4\$\":\$\"", "\$\"d5\$\":\$\"", "\$\"d6\$\":\$\"", "\$\"d7\$\":\$\"", "\$\"d8\$\":\$\"", "\$\"lat\$\":\$\"", "\$\"lng\$\":\$\"", "\$\"created\$\":\$\""};

Ambient::Ambient() {
}

bool
Ambient::begin(unsigned int channelId, const char * writeKey, SoftwareSerial * s, int dev) {
    this->channelId = channelId;

    if (sizeof(writeKey) > AMBIENT_WRITEKEY_SIZE) {
        ERR("writeKey length > AMBIENT_WRITEKEY_SIZE");
        return false;
    }
    strcpy(this->writeKey, writeKey);

    if(NULL == s) {
        ERR("Socket Pointer is NULL, open a socket.");
        return false;
    }
    this->s = s;
    this->dev = dev;
    if (dev) {
        strcpy(this->host, AMBIENT_HOST_DEV);
        this->port = AMBIENT_PORT_DEV;
    } else {
        strcpy(this->host, AMBIENT_HOST);
        this->port = AMBIENT_PORT;
    }
    for (int i = 0; i < AMBIENT_NUM_PARAMS; i++) {
        this->data[i].set = false;
    }
        return true;
}

bool
Ambient::set(int field, char * data) {
    --field;
    if (field < 0 || field >= AMBIENT_NUM_PARAMS) {
        return false;
    }
    if (strlen(data) > AMBIENT_DATA_SIZE) {
        return false;
    }
    this->data[field].set = true;
    strcpy(this->data[field].item, data);

    return true;
}

bool
Ambient::clear(int field) {
    --field;
    if (field < 0 || field >= AMBIENT_NUM_PARAMS) {
        return false;
    }
    this->data[field].set = false;

    return true;
}

bool
Ambient::sendcmd(char *cmd) {
    String rstr = "";
    unsigned long tim = millis();
    DBG(cmd);DBG("\r\n");
    this->s->println(cmd);
    do {
        while (!this->s->available() && (millis() - tim < 30000)) {
            ;
        }
        rstr = this->s->readStringUntil('\n');
        DBG(rstr);
    } while (rstr.indexOf("$W") < 0);
    char ch;
    do {
        ch = this->s->read();
        if (0x20 <= ch && ch < 0x80) {
            DBG(ch);
        }
    } while (this->s->available());
    DBG("\r\n");
    return (rstr.indexOf("=OK") > 0);
}

bool
Ambient::send() {
    char buf[256];

    sprintf(buf, "$WP http://%s:%d/api/v2/channels/%d/data ", this->host, this->port, this->channelId);

    strcat(buf, "\"{\$\"writeKey\$\":\$\"");
    strcat(buf, this->writeKey);
    strcat(buf, "\$\",");

    for (int i = 0; i < AMBIENT_NUM_PARAMS; i++) {
        if (this->data[i].set) {
            strcat(buf, ambient_keys[i]);
            strcat(buf, this->data[i].item);
            strcat(buf, "\$\",");
        }
    }
    buf[strlen(buf) - 1] = '\0';
    strcat(buf, "}$r$n\"");
    
    strcat(buf, "\"Content-Type: application/json$r$n\"");
    sendcmd(buf);

    delay(200);

    for (int i = 0; i < AMBIENT_NUM_PARAMS; i++) {
        this->data[i].set = false;
    }

    return true;
}
