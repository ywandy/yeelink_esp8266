#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include "FS.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "ESP8266WebServer.h"
#include "WiFiUdp.h"
#include "stdlib.h"
#include "stdio.h"
#include "net_time.h"
#include "data_transfer.h"

#define vol_Sensor "http://api.yeelink.net/v1.0/device/350544/sensor/393430/datapoints"
#define cur_Sensor "http://api.yeelink.net/v1.0/device/350544/sensor/393429/datapoints"
#define mh_Sensor "http://api.yeelink.net/v1.0/device/350544/sensor/394877/datapoints"
#define tmp_Sensor "http://api.yeelink.net/v1.0/device/350544/sensor/394878/datapoints"
extern char wifiname[40] = {0};
extern char wifipass[40] = {0};
ESP8266WebServer server(80);
extern char time_iso[16]; //时间戳全局
extern unsigned int localPort;
extern String comdata = ""; //接受串口数据
int max_con_timeout = 10; //wifi 连接最大尝试次数
unsigned long tim_task1,tim_task2 = 0;
int red_led = 0;
int green_led = 0;
//结构体定义区域
extern DT mydt;
extern TIMEDATA timedata;


bool loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }
  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }
  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());
  if (!json.success()) {
    Serial.println("Failed to parse config file");
    return false;
  }
  sprintf(wifiname,json["wifi"]);
  sprintf(wifipass,json["pass"]);//字符串转格式
  return true;
}


bool saveConfig() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["wifi"] = wifiname;
  json["pass"] = wifipass;
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }
  json.printTo(configFile);
  Serial.println("esp8266 is rebooting!");
  delay(2000);
   //ESP.restart();
  return true;
}



void system_check(){
   Serial.println("last reset reason:");
   Serial.println(ESP.getResetReason());
   Serial.print("flash size: ");
   Serial.println(ESP.getFlashChipSize());
   Serial.print("flash real_size: ");
   Serial.println(ESP.getFlashChipRealSize());
   Serial.print("flash speed: ");
   Serial.println(ESP.getFlashChipSpeed());
  }

void led_setup(){
    pinMode(13,OUTPUT);
    pinMode(14,OUTPUT);
  }

void handleRoot() {
  String msg;
 // char wifiname[40],wifipass[40];
  if (server.hasArg("WIFI_NAME") && server.hasArg("WIFI_PASS")){
      //String header = "HTTP/1.1 301 OK\r\nSet-Cookie: ESPSESSIONID=1\r\nLocation: /\r\nCache-Control: no-cache\r\n\r\n";
     //server.sendContent(header);
      sprintf(wifiname,"%s",server.arg("WIFI_NAME").c_str());
      sprintf(wifipass,"%s",server.arg("WIFI_PASS").c_str());
      Serial.print("wifi-name:");
      Serial.println(wifiname);
      Serial.print("wifi-pass:");
      Serial.println(wifipass);
      
      server.send(200, "text/html", "<h1>Wifi config had been saved and esp is rebooting.....</h1>");
      saveConfig();
      delay(3000);
    }
  String content = "<html><body><form action='/' method='POST'>Enter the wifi NAME && PASS<br>";
  content += "User:<input type='text' name='WIFI_NAME' placeholder='wifi_name'><br>";
  content += "Password:<input type='password' name='WIFI_PASS' placeholder='password'><br>";
  content += "<input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "<br>";
  content += "You also can go <a href='/inline'>here</a></body></html>";
  server.send(200, "text/html", content);
 // server.send(200, "text/html", "<h1>You are connected</h1>");
}


void wifi_init(){
   int count = 0;
  loadConfig();
  WiFi.softAP("esp8266_set","0123456789"); //创建软AP
  WiFi.begin(wifiname, wifipass); //连接WIFI
  while (WiFi.status() != WL_CONNECTED)
  {
    count ++;
    delay(500);
    if(count==max_con_timeout){
      Serial.println("wifi connect failed");
      break;}
  }
  server.on("/", handleRoot);
  server.begin();
  init_udp_time(); //配置网络时钟服务器
  }

 
void setup() {
  SPIFFS.begin();
  led_setup();
  Serial.begin(115200);
  wifi_init();
  delay(2000);
}



void loop() {
  server.handleClient();  
  if(abs(tim_task1-millis())>1000){
      tim_task1 = millis();
      Serial.println(get_switch("http://api.yeelink.net/v1.0/device/351192/sensor/395134/datapoints"));
      if(get_switch("http://api.yeelink.net/v1.0/device/351192/sensor/395134/datapoints"))
        digitalWrite(14,HIGH);
      else 
        digitalWrite(14,LOW);
      //digitalWrite(13,get_switch("http://api.yeelink.net/v1.0/device/351192/sensor/395134/datapoints"));
    }   
  while (Serial.available() > 0)  
    {
        comdata += char(Serial.read());
        delay(5);
    }
    if (comdata.length() > 0)
    {
        //Serial.println(comdata);
        apart_num(); //对数据进行分离
        time_get_net();
        delay(100);
        esp_send(mydt.vol,vol_Sensor,time_iso);
        esp_send(mydt.cur,cur_Sensor,time_iso);
        esp_send(mydt.mh,mh_Sensor,time_iso);
        esp_send(mydt.tmp,tmp_Sensor,time_iso);
        comdata = "";
    }
}
