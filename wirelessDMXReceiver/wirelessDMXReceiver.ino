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
#include <elapsedMillis.h>

#define DEBUG_SERIAL_WAIT false

const char* ssid     = "WPSAud2";
const char* password = "Lamplights";

unsigned int localPort = 4444; 
uint8_t packetBuffer[1024];
WiFiUDP Udp;

#if defined(ESP8266)
#define PIN        3
#define PINID0     2
#define PINID1     13
#define PINID2     14
#define BOARDLED   LED_BUILTIN
#else
#define PIN 14
#define PINID0     25
#define PINID1     26
#define PINID2     27
#define PWM0       15
#define PWM1       0
#define PWM2       4
#define BOARDLED   2
#endif

#define DEBUG_PRINT false

#define NUMPIXELS 27 // Popular NeoPixel ring size
//Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
CRGB leds[NUMPIXELS];

uint16_t dmxStartAddr = 1;
bool ledState = false;

void connect()
{
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  FastLED.clear();
  FastLED.show();  

  bool blink = false;
  while (WiFi.status() != WL_CONNECTED) {

    leds[0] = CRGB(blink ? 32 : 0, 0, 0);
    FastLED.show();  
    blink = !blink;

    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  while(!Serial && DEBUG_SERIAL_WAIT){}

  pinMode(BOARDLED, OUTPUT);

  delay(100);
  FastLED.addLeds<WS2812B, PIN, GRB>(leds, NUMPIXELS);  // GRB ordering is typical
  FastLED.setBrightness(127);

  pinMode(PINID0,INPUT_PULLUP);
  pinMode(PINID1,INPUT_PULLUP);
  pinMode(PINID2,INPUT_PULLUP);
  pinMode(PWM0,OUTPUT);
  pinMode(PWM1,OUTPUT);
  pinMode(PWM2,OUTPUT);

  // We start by connecting to a WiFi network
  connect();
  Udp.begin(localPort);
}

int value = 0;

elapsedMillis lastPrintTime = 0;
elapsedMillis lastAddrPrintTime = 0;
elapsedMillis ledBlink = 0;

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
          digitalWrite(BOARDLED, ledState);  // turn the LED on (HIGH is the voltage level)
          ledState = !ledState;
          ledBlink = 0;
        }
      /*
      Serial.print("dmxStartAddr: ");
      Serial.println(dmxStartAddr);
      Serial.print("len: ");
      Serial.println(len);
    
      for( int i=0; i < 32 && i < len; ++i)
      { 
        Serial.print(packetBuffer[i]);
        Serial.print(" ");
      }
      Serial.println("");
      */

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

    }
  }
}