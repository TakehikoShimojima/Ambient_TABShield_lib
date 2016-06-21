#include <SoftwareSerial.h>
#include "Ambient3GIM.h"

SoftwareSerial iemSerial(4,5);
const unsigned long baudrate = 38400;

#define LIMITTIME 35000 // ms (3G module start time)

unsigned int channelId = 100;
const char* writeKey = "...writeKey...";
Ambient ambient;

//============== 3G setup =================
boolean _3Gsetup() {

  pinMode(7,OUTPUT);
  digitalWrite(7,HIGH); delay(100);
  digitalWrite(7,LOW); delay(100);  // 3G shield --> digitalWrite(7,HIGH);
 //------- 3G module begin & connect ---------
  String str="";
  unsigned long tim = millis();
  do{  str=iemSerial.readStringUntil('\n');
  }while(!(str.indexOf("3GIM")>0) && (millis() - tim) <LIMITTIME);
  delay(1000);
  Serial.println(str);
  if( millis() -tim >= LIMITTIME) { 
     return false;
  } else return true;
}

//============================================
void setup() {
  Serial.begin(baudrate);
  iemSerial.begin(baudrate);
  Serial.println(">Ready. \r\n Initilaizing...");
  if( _3Gsetup() ) {
      Serial.println("start");  
  } else { 
      Serial.println(" Connect Error ... Stop"); 
      while(1) { digitalWrite(7,LOW); delay(500); digitalWrite(7,HIGH); delay(500); }; //　エラーアラーム
  } 
  ambient.begin(channelId, writeKey, &iemSerial);
  delay(1000);
}

void loop () {
  char tempbuf[8], lightbuf[8];
  float temp;
  int light;

  light = analogRead(A0); // 空読み
  delay(10);
  light = 0;
  int minl = 1023, maxl = 0;
  for (int i = 0; i < 7; i++) { // センサーを7回読み、
    int l = analogRead(A0);
    light += l;
    if (minl > l) minl = l; // 最小値と
    if (maxl < l) maxl = l; // 最大値を探す
    delay(10);
  }
  light = (light - maxl - minl) / 5; // 最大値と最小値を捨てて平均する
  
  temp = 207.26 - 0.594 * analogRead(A1); // 空読み
  delay(10);
  temp = 0;
  float mint = 100.0, maxt = -100.0;
  for (int i = 0; i < 7; i++) { // センサーを7回読み、
    float t = 207.26 - 0.594 * analogRead(A1);
    temp += t;
    if (mint > t) mint = t; // 最小値と
    if (maxt < t) maxt = t; // 最大値を探す
    delay(10);
  }
  temp = (temp - maxt - mint) / 5; // 最大値と最小値を捨てて平均する

  sprintf(tempbuf, "%2d.%1d", (int)temp, (int)(temp*10)%10);
  sprintf(lightbuf,"%3d", light);
  Serial.print("temp: "); Serial.print(tempbuf);
  Serial.print(",  light: "); Serial.println(lightbuf);

  ambient.set(1, tempbuf);
  ambient.set(2, lightbuf);
  ambient.send();
   
  delay(300000UL);
}

