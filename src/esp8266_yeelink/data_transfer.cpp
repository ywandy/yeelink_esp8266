#include "data_transfer.h"
DT mydt;
extern String comdata;
void esp_send(float dat,char *s,char *tm){
    char str[86],flt[6];
    int code;
    HTTPClient http;
    dtostrf(dat,4,2,flt);
    //Serial.println(flt);
    sprintf(str,"{\"timestamp\":\"%s\",\"value\":%s}",tm,flt);
    //Serial.println(str);
    code = http.begin(s);
    http.addHeader("U-ApiKey","74c4601f0870e71ed6db8ec6f4741b33",1,1);
    code = http.sendRequest("POST",str);
    if(code==200){
        //Serial.println("success");
      }else{
       // Serial.print("failed! code:");
        //Serial.println(code);
        //Serial.print("cause:");
        //Serial.println(http.getString());              
      }
    //Serial.println(code);
    http.end();
    delay(100);
  }

int get_switch(char *url){
    int light_status = 0;
    HTTPClient http;
    int code = 0;
    String msg = "";
    code = http.begin(url);
    http.addHeader("U-ApiKey","74c4601f0870e71ed6db8ec6f4741b33",1,1);
    code = http.sendRequest("GET");
    if(code==200){
      //  Serial.println("success");
        msg = http.getString();
       // Serial.println(msg);     
        if(msg[msg.length()-2]=='0'){
          light_status = 0;
          }else if(msg[msg.length()-2]=='1'){
            light_status = 1;
            }
        return light_status;    
      }else{
      //  Serial.print("failed! code:");
       // Serial.println(code);
       // Serial.print("cause:");
         msg = http.getString();
        //Serial.println(msg);              
      }
   // Serial.println(code);
    http.end();
     return -1;     
  }


  void apart_num(){
    int sum = 0;
    int loop = 0;
    String inString = "";
    while(comdata[loop]!='i'){
      if(loop == 6){
          break;
        }
       if(isDigit(comdata[loop])||comdata[loop]=='.'){
          inString += comdata[loop];
        }
        loop++;
      }
      mydt.cur = inString.toFloat();
      inString = "";
      while(comdata[loop]!='v'){
      if(loop == 13){
          break;
        }
       if(isDigit(comdata[loop])||comdata[loop]=='.'){
          inString += comdata[loop];
        }
        loop++;
      }
      mydt.vol = inString.toFloat();
      inString = "";
      while(comdata[loop]!='m'){
      if(loop == 19){
          break;
        }
       if(isDigit(comdata[loop])||comdata[loop]=='.'){
          inString += comdata[loop];
        }
        loop++;
      }
      mydt.mh = inString.toFloat();
      inString = "";
      while(comdata[loop]!='t'){
      if(loop == 25){
          break;
        }
       if(isDigit(comdata[loop])||comdata[loop]=='.'){
          inString += comdata[loop];
        }
        loop++;
      }
      mydt.tmp = inString.toFloat();
      inString = "";
     /* Serial.print("vol:");
      Serial.print(mydt.vol);
      Serial.print("cur:");
      Serial.print(mydt.cur);
      Serial.print("mh:");
      Serial.print(mydt.mh);
      Serial.print("tmp:");
      Serial.print(mydt.tmp);
      Serial.print("\n");*/
  }
