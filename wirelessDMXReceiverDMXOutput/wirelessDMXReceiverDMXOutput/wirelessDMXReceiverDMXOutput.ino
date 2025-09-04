/*
 *  Simple HTTP get webclient test
 */

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include <WiFiUdp.h>
#include <FastLED.h>

#include <Arduino.h>
#include <esp_dmx.h>

#include <elapsedMillis.h>

#define DEBUG_SERIAL_WAIT false

const char* ssid     = "WPSAud2";
const char* password = "Lamplights";
int status = WL_IDLE_STATUS;

unsigned int localPort = 4444; 
uint8_t packetBuffer[1024];
WiFiUDP Udp;

#define PIN 14
#define PINID0     25
#define PINID1     26
#define PINID2     27
#define PWM0       15
#define PWM1       0
#define PWM2       4
#define LED_BUILTIN 2

#define DEBUG_PRINT false
#define FAST_LED_ENABLE false

#define NUMPIXELS 27 // Popular NeoPixel ring size
//Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
CRGB leds[NUMPIXELS];

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

  #if FAST_LED_ENABLE
  FastLED.clear();
  FastLED.show();  
  #endif

  while (WiFi.status() != WL_CONNECTED) {

    digitalWrite(LED_BUILTIN, blink);
    leds[0] = CRGB(blink ? 32 : 0, 0, 0);

    #if FAST_LED_ENABLE
    FastLED.show();  
    #endif

    blink = !blink;
    delay(500);

    Serial.print(".");
  }

  printWifiStatus();
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  while(!Serial && DEBUG_SERIAL_WAIT)
  {
    digitalWrite(LED_BUILTIN, blink);
    blink = !blink;
    delay(100);
  }

  pinMode(LED_BUILTIN, OUTPUT);

  delay(100);

  #if FAST_LED_ENABLE
  FastLED.addLeds<WS2812B, PIN, GRB>(leds, NUMPIXELS);  // GRB ordering is typical
  FastLED.setBrightness(127);
  #endif

  pinMode(PINID0,INPUT_PULLUP);
  pinMode(PINID1,INPUT_PULLUP);
  pinMode(PINID2,INPUT_PULLUP);
  pinMode(PWM0,OUTPUT);
  pinMode(PWM1,OUTPUT);
  pinMode(PWM2,OUTPUT);

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
  uint16_t id0 = !digitalRead(PINID0);
  uint16_t id1 = !digitalRead(PINID1);
  uint16_t id2 = !digitalRead(PINID2);

  // use bit 2 to switch between base IDs
  uint16_t idBase = 1;
  if( id2 == 1 )
  {
    idBase = 353; 
  }
  
  // increment bits 0:1 by 3 addresses and add base
  // b00 = 0
  // b01 = 3
  // b10 = 6
  uint16_t id = 3*( (id1 << 1) + (id0 << 0) ) + idBase;

  dmxStartAddr = id;

  if( lastAddrPrintTime > 1000 && DEBUG_PRINT )
  {
    Serial.print("id0 ");
    Serial.print(id0);
    Serial.print(" id1 ");
    Serial.print(id1);
    Serial.print(" id2 ");
    Serial.print(id2);
    Serial.print(" idBase ");
    Serial.print(idBase);   
    Serial.print(" id ");
    Serial.print(id);
    Serial.print(" dmxStartAddr ");
    Serial.print(dmxStartAddr);
    Serial.println("");
    lastAddrPrintTime = 0;
  }

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

      #if FAST_LED_ENABLE
      // Doesn't appear to be compatible with DMX output
      FastLED.clear();
      for(int i=0; i<NUMPIXELS; i++) 
      { // For each pixel...
        // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
        // Here we're using a moderately bright green color:
        leds[i] = CRGB( packetBuffer[dmxStartAddr+0],
                        packetBuffer[dmxStartAddr+1],
                        packetBuffer[dmxStartAddr+2] );
      }

      FastLED.show();  

      analogWrite(PWM0,packetBuffer[dmxStartAddr+0]);
      analogWrite(PWM1,packetBuffer[dmxStartAddr+1]);
      analogWrite(PWM2,packetBuffer[dmxStartAddr+2]);
      #endif

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