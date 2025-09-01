/*
 *  Simple HTTP get webclient test
 */

#include "WiFiS3.h"
#include <ArduinoRS485.h> // the ArduinoDMX library depends on ArduinoRS485
#include <ArduinoDMX.h>

#include <WiFiUdp.h>
#include <elapsedMillis.h>

#define DEBUG_SERIAL_WAIT true

const char* ssid     = "WPSAud2";
const char* password = "Lamplights";
int status = WL_IDLE_STATUS;

unsigned int localPort = 4444; 
uint8_t packetBuffer[1024];
WiFiUDP Udp;

#if defined(ESP8266)
#define PIN        3
#else
#define PIN 14
#define PWM0       15
#define PWM1       0
#define PWM2       4
#endif

#define DEBUG_PRINT false

uint16_t dmxStartAddr = 1;
bool ledState = false;
bool blink = false;

void connect()
{
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  status = WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    digitalWrite(LED_BUILTIN, blink);
    blink = !blink;
    delay(500);

    Serial.print(".");
  }

  delay(10000);

  printWifiStatus();
}

void setup() {
  Serial.begin(115200);
  while(!Serial && DEBUG_SERIAL_WAIT){}

  pinMode(LED_BUILTIN, OUTPUT);

  delay(100);

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
    {
      digitalWrite(LED_BUILTIN, blink);
      blink = !blink;
      delay(100);
    }
  }

  // Pin 1 TX
  // Pin 8 DE
  // Pin 7 RE
  if (!DMX.begin()) {
    Serial.println("Failed to initialize DMX!");
    while (true)
    {
      digitalWrite(LED_BUILTIN, blink);
      blink = !blink;
      delay(200);
    }; // wait for ever
  }

  // We start by connecting to a WiFi network
  connect();
  Udp.begin(localPort);
}

int value = 0;

elapsedMillis lastPrintTime = 0;
elapsedMillis lastAddrPrintTime = 0;
elapsedMillis ledBlink = 0;

uint8_t val = 0;

bool okToPrint = false;
void loop() {
  
  if(WiFi.status() != WL_CONNECTED)
  {
    WiFi.disconnect();
    connect();
  }

 if (Udp.parsePacket()) {
    int len = Udp.read(packetBuffer, 1024);
    if (len > 0) 
    {
        okToPrint = false;
        if(lastPrintTime > 1000 )
        {
          okToPrint = true;
          lastPrintTime = 0;
        }
        
        if( okToPrint && DEBUG_PRINT)
        {
          Serial.print("UDP Rx Len: ");
          Serial.println(len);
        }

        if( ledBlink > 50 )
        {
          digitalWrite(LED_BUILTIN, ledState);  // turn the LED on (HIGH is the voltage level)
          ledState = !ledState;
          ledBlink = 0;
        }

      /*
      Serial.print("len: ");
      Serial.println(len);
    
      for( int i=0; i < 32 && i < len; ++i)
      { 
        Serial.print(packetBuffer[i]);
        Serial.print(" ");
      }
      Serial.println("");
      */

      uint8_t newval = packetBuffer[1];

      if( val != newval )
      {
        Serial.println(newval);
      }
      val = newval;

      DMX.beginTransmission();
      for( uint16_t i=1; i < 4; ++i )
      {
        DMX.write(i,packetBuffer[i]);
      }
      DMX.endTransmission();
    }
  }
}

void printWifiStatus() {
/* -------------------------------------------------------------------------- */
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}