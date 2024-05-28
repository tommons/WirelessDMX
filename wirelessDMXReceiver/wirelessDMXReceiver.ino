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
#else
#define PIN 14
#define PINID0     2
#define PINID1     13
#endif

#define NUMPIXELS 20 // Popular NeoPixel ring size
//Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
CRGB leds[NUMPIXELS];

uint8_t dmxStartAddr = 1;
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

  pinMode(LED_BUILTIN, OUTPUT);

  delay(100);
  FastLED.addLeds<WS2812B, PIN, GRB>(leds, NUMPIXELS);  // GRB ordering is typical
  FastLED.setBrightness(127);

  pinMode(PINID0,INPUT_PULLUP);
  pinMode(PINID1,INPUT_PULLUP);

  // We start by connecting to a WiFi network
  connect();
  Udp.begin(localPort);
}

int value = 0;
elapsedMillis lastPrintTime = 0;
bool okToPrint = false;
void loop() {
  
  int id0 = digitalRead(PINID0);
  int id1 = digitalRead(PINID1);

  if( id0 == 1 && id1 == 1 ) // 00
  {
    dmxStartAddr = 1;
  }
  else if( id0 == 1 && id1 == 0 ) // 01
  {
    dmxStartAddr = 4;
  }
  else if( id0 == 0 && id1 == 1 ) // 10
  {
    dmxStartAddr = 7;
  }
  else if( id0 == 0 && id1 == 0 ) // 11
  {
    dmxStartAddr = 10;
  }

/*
  Serial.print("id0 ");
  Serial.print(id0);
  Serial.print(" id1 ");
  Serial.print(id1);
  Serial.println("");
*/

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
        
        if( okToPrint )
        {
          Serial.print("UDP Rx Len: ");
          Serial.println(len);
        }

        digitalWrite(LED_BUILTIN, ledState);  // turn the LED on (HIGH is the voltage level)
        ledState = !ledState;
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
    }
  }
}