#include <Arduino.h>
#include <HardwareSerial.h>

#include "GPS.h"
HardwareSerial Sim800L(2); 

SemaphoreHandle_t xSerialSemaphore;

void TaskSerialRead( void *pvParameters );
void TaskHttpWrite( void *pvParameters );

// put function declarations here:
void UpdateSerial();
GPS g_gps;
size_t read_flag = 0;
char http_url[150];

void setup() 
{
  Serial.begin(115200);
  Serial.println("Inicializando A9G");

  delay(500);

  Sim800L.begin(115200);
  Sim800L.println("AT");
  delay(500);
  UpdateSerial();

  Sim800L.println("AT+RST=1"); //Software Reset
  delay(3000);
  UpdateSerial();


  Sim800L.println("AT+GPS=0"); //Disable GPS function
  delay(500);
  UpdateSerial();

  Sim800L.println("AT+CGATT=1"); //Attach network, this command is necessary if the Internet is needed.
  delay(500);
  UpdateSerial();

  Sim800L.println("AT+CGDCONT=1,\"IP\",\"CMNET\""); //Set PDP parameter
  delay(500);
  UpdateSerial();

  Sim800L.println("AT+CGACT=1,1"); //Activate PDP, can access to Internet when activated properly 
  delay(500);
  UpdateSerial();

  Sim800L.println("AT+GPS=1"); //Enable GPS function
  delay(500);
  UpdateSerial();

  Sim800L.println("AT+GPSRD=30"); //Output a NEMA message from AT serial every 5 seconds 
  delay(500);
  UpdateSerial();

  if ( xSerialSemaphore == NULL )  // Check to confirm that the Serial Semaphore has not already been created.
  {
    xSerialSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore we will use to manage the Serial Port
    if ( ( xSerialSemaphore ) != NULL )
      xSemaphoreGive( ( xSerialSemaphore ) );  // Make the Serial Port available for use, by "Giving" the Semaphore.
  }

  xTaskCreate(
    TaskSerialRead
    ,  "SerialRead"  // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL //Parameters for the task
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL ); //Task Handle

  xTaskCreate(
    TaskHttpWrite
    ,  "HTTPWrite" // A name just for humans
    ,  128  // Stack size
    ,  NULL //Parameters for the task
    ,  1  // Priority
    ,  NULL ); //Task Handle

}

void loop() {


}

void UpdateSerial()
{
   while (Sim800L.available()) 
  {
       Serial.println(Sim800L.readString());
  }
}


/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskSerialRead( void *pvParameters __attribute__((unused)) )  // This is a Task.
{
  for (;;) // A Task shall never return or exit.
  {

    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      
      String read_data = Sim800L.readStringUntil('\n');
      Serial.println(read_data);
      if(g_gps.SetGGA(read_data.c_str()))
      {
        Serial.println(read_data);
      }

      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }

    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}

void TaskHttpWrite( void *pvParameters __attribute__((unused)) )  // This is a Task.
{

  for (;;)
  {
    // See if we can obtain or "Take" the Serial Semaphore.
    // If the semaphore is not available, wait 5 ticks of the Scheduler to see if it becomes free.
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      
      double latitude, longitude;

      latitude = g_gps.GetLatitude();
      longitude = g_gps.GetLongitude();

      sprintf(http_url, "AT+HTTPGET=\"https://api.thingspeak.com/update?api_key=FRHNMS2Y95UJ92FU&field3=%f&field4=%f\"", latitude, longitude);
    
      Serial.println(http_url);
      Sim800L.println(http_url);


      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }

    vTaskDelay(2000);  // one tick delay (15ms) in between reads for stability
  }
}