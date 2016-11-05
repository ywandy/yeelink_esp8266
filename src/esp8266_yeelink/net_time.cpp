#include "net_time.h"
WiFiUDP udp;
TIMEDATA timedata;
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
extern char time_iso[16] = {0};
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
static int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
extern unsigned int localPort = 4444;      // local port to listen for UDP packets
IPAddress timeServerIP(120,24,166,46); // time.nist.gov NTP server address

void init_udp_time(){
  udp.begin(localPort);
 // Serial.print("Local UDP port: ");
 // Serial.println(udp.localPort());
  time_get_net(); //获取网络时间
  }

unsigned long sendNTPpacket(IPAddress& address)
{
  //Serial.println("sending NTP packet...");
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;   
  packetBuffer[1] = 0;     
  packetBuffer[2] = 6;     
  packetBuffer[3] = 0xEC;  
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52; //UDP报文
  udp.beginPacket(address, 123); //123 为NTP端口
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

void printDate(uint32_t timeStamp) {
  unsigned int year = START_YEAR;
  while(1) {
    uint32_t seconds;
    if(isLeapYear(year)) seconds = SECONDS_IN_DAY * 366;
    else seconds = SECONDS_IN_DAY * 365;
    if(timeStamp >= seconds) {
      timeStamp -= seconds;
      year++;
    } else break;
  }
  unsigned int month = 0;
  while(1) {    
    uint32_t seconds = SECONDS_IN_DAY * days_in_month[month];
    if(isLeapYear(year) && month == 1) seconds = SECONDS_IN_DAY * 29;
    if(timeStamp >= seconds) {
      timeStamp -= seconds;
      month++;
    } else break;
  }  
  month++;
  unsigned int day = 1;
  while(1) {
    if(timeStamp >= SECONDS_IN_DAY) {
      timeStamp -= SECONDS_IN_DAY;
      day++;
    } else break;
  }  
  unsigned int hour = timeStamp / 3600;
  unsigned int minute = (timeStamp - (uint32_t)hour * 3600) / 60;
  unsigned int second = (timeStamp - (uint32_t)hour * 3600) - minute * 60;
  //Serial.println("Current date and time:");
 // if(day < 10) Serial.print("0");
//  Serial.print(day);
  timedata.val_day = day;
//  Serial.print("/");
 // if(month < 10) Serial.print("0");
 // Serial.print(month);
  timedata.val_month = month;
 // Serial.print("/");  
  timedata.val_year = year;
 // Serial.println(year);
  //if(hour < 10) Serial.print("0");
  timedata.val_hour = hour;
 // Serial.print(hour);
//  Serial.print(":");
 // if(minute < 10) Serial.print("0");
  timedata.val_min = minute;
 // Serial.print(minute);
//  Serial.print(":");
  //if(second < 10) Serial.print("0");
  timedata.val_sec = second;
 // Serial.println(second);
 // Serial.println();
}

boolean isLeapYear(unsigned int year) {
  return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

void time_get_net(){
  //char timestr[30] = {0};
    //get a random server from the pool
  //WiFi.hostByName(ntpServerName, timeServerIP); 
  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(2000);
  int cb = udp.parsePacket();
  if (!cb) {
    //Serial.println("no packet yet");
  }
  else {
   // Serial.print("packet received, length=");
    //Serial.println(cb);
    udp.read(packetBuffer, NTP_PACKET_SIZE); //读取udp包到缓存
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]); //从40位置读取4字节的字
    unsigned long secsSince1900 = highWord << 16 | lowWord; //组合4个字（从1900开始计算的时间）
   /* Serial.print("Seconds since Jan 1 1900 = " );
    Serial.println(secsSince1900);
    Serial.print("Unix time = ");*/
    const unsigned long seventyYears = 2208988800UL;
    unsigned long epoch = secsSince1900 - seventyYears;  //转换UNIX时间戳 自从1970/1/1,（2208988800S）
  //  Serial.println(epoch); //unix时间戳
    //Serial.print("The UTC time is "); 
    printDate(epoch + 3600 * TIME_ZONE);
    sprintf(time_iso,"%d-%d-%dT%d:%d:%d",timedata.val_year,timedata.val_month,timedata.val_day,timedata.val_hour,timedata.val_min,timedata.val_sec);
   // Serial.println(time_iso);
}
}
