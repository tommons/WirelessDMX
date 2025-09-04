/*
 *  Simple HTTP get webclient test
 */

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include <WiFiUdp.h>

#include <Arduino.h>
#include <esp_dmx.h>

#include <elapsedMillis.h>

#define DEBUG_SERIAL_WAIT true
#define LED_BUILTIN 2

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

int transmitPin = 17;
int receivePin = 16;
int enablePin = 21;
dmx_port_t dmxPort = 1;
byte data[DMX_PACKET_SIZE];

void connect()
{
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  status = WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    digitalWrite(LED_BUILTIN, blink);
    blink = !blink;
    delay(500);

    Serial.print(".");
  }

  printWifiStatus();
}

void setup() {
  Serial.begin(115200);
  while(!Serial && DEBUG_SERIAL_WAIT){}

  pinMode(LED_BUILTIN, OUTPUT);

  delay(100);

  dmx_config_t config = DMX_CONFIG_DEFAULT;
  dmx_personality_t personalities[] = {};
  int personality_count = 0;
  dmx_driver_install(dmxPort, &config, personalities, personality_count);
  dmx_set_pin(dmxPort, transmitPin, receivePin, enablePin);

  // We start by connecting to a WiFi network
  connect();
  Udp.begin(localPort);
}

int value = 0;

elapsedMillis lastPrintTime = 0;
elapsedMillis lastAddrPrintTime = 0;
elapsedMillis ledBlink = 0;
elapsedMillis procTime = 0;
uint32_t count = 0;

uint8_t val = 0;

bool okToPrint = false;
void loop() {

  if(WiFi.status() != WL_CONNECTED)
  {
    WiFi.disconnect();
    connect();
  }

 int len = Udp.parsePacket();
 if (len) 
 {
    len = Udp.read(packetBuffer, len);
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

      
      for( uint16_t i=1; i < 513; ++i )
      {
        data[i] = packetBuffer[i];
      }
      dmx_write(dmxPort, data, DMX_PACKET_SIZE);
      dmx_send_num(dmxPort, DMX_PACKET_SIZE);
      dmx_wait_sent(dmxPort, DMX_TIMEOUT_TICK);

    }
    ++count;

    if( procTime > 1000)
    {
      Serial.print("Proc: ");
      Serial.print(" count: ");
      Serial.print(count);
      Serial.print(" proc: ");
      Serial.print(procTime);
      Serial.print(" " );
      Serial.println( procTime / count);

      count = 0;
      procTime = 0;
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