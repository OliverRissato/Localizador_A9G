#include <Arduino.h>
#include <HardwareSerial.h>
#include "GPS.h"
HardwareSerial Sim800L(2); 

// put function declarations here:
void UpdateSerial();
GPS_t* g_gps;

void setup() 
{
  g_gps = GPS_Init();

  // Serial.begin(115200);
  // Serial.println("Inicializando A9G");
  Sim800L.begin(115200);
  Sim800L.println("AT");
  delay(500);
  // UpdateSerial();

  Sim800L.println("AT+RST=1"); //Software Reset
  delay(5000);
  // UpdateSerial();


  Sim800L.println("AT+GPS=0"); //Disable GPS function
  delay(500);
  // UpdateSerial();

  Sim800L.println("AT+CGATT=1"); //Attach network, this command is necessary if the Internet is needed.
  delay(500);
  // UpdateSerial();

  Sim800L.println("AT+CGDCONT=1,\"IP\",\"CMNET\""); //Set PDP parameter
  delay(500);
  // UpdateSerial();

  Sim800L.println("AT+CGACT=1,1"); //Activate PDP, can access to Internet when activated properly 
  delay(500);
  // UpdateSerial();

  Sim800L.println("AT+GPS=1"); //Enable GPS function
  delay(500);
  // UpdateSerial();

  Sim800L.println("AT+GPSRD=30"); //Output a NEMA message from AT serial every 5 seconds 
  delay(500);
  // UpdateSerial();
}

void loop() {
  // put your main code here, to run repeatedly:
  while(Sim800L.available())   
  { 
    String read_data = Sim800L.readStringUntil('\n');
    if(GPS_SetGGA(g_gps, read_data.c_str()))
    {
      double latitude, longitude;
      char http_url[200];

      latitude = GPS_GetLatitude(g_gps);
      longitude = GPS_GetLongitude(g_gps);

      sprintf(http_url, "AT+HTTPGET=\"https://api.thingspeak.com/update?api_key=FRHNMS2Y95UJ92FU&field3=%f&field4=%f\"", latitude, longitude);
    
      // Serial.println(http_url);
      Sim800L.println(http_url);
    }  

  }


}

// void UpdateSerial()
// {
//    while (Sim800L.available()) 
//   {
//        Serial.println(Sim800L.readString());
//   }
// }

