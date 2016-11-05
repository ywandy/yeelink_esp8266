#ifndef __NETIME__
#define __NETIME__

#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include "stdlib.h"
#include "stdio.h"


typedef struct TIMEDATA{
    int val_year,val_month,val_day,val_hour,val_min,val_sec;
  }TIMEDATA;
  
#define SECONDS_IN_DAY          86400
#define START_YEAR              1970
#define TIME_ZONE               +8
void init_udp_time();
unsigned long sendNTPpacket(IPAddress& address);
void printDate(uint32_t timeStamp);
boolean isLeapYear(unsigned int year) ;
void time_get_net();
#endif

