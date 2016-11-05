#ifndef __DATATRANSFER__
#define __DATATRANSFER__
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include "stdlib.h"
#include "stdio.h"
#include "net_time.h"

typedef struct DT{
    float vol;
    float cur;
    float mh;
    float tmp;
  }DT;

void esp_send(float dat,char *s,char *tm);
int get_switch(char *url);
void apart_num();

#endif
