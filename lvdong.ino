//#include <FastLED.h>
#include "OneButton.h"
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
//wifi设置
#ifndef STASSID
#define STASSID "AKAISAKURA"//wifi名字
#define STAPSK  "ren123456"//wifi密码
#endif
#define PIN              5                      
#define buttonPin   4       
#define NUMPIXELS      64                        
int localPort = 6001;      
int   maxBrightness = 250;
#include <NTPClient.h>
Adafruit_NeoPixel pad = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, 1, 1, PIN,
                            NEO_MATRIX_TOP  + NEO_TILE_LEFT  + NEO_MATRIX_COLUMNS   + NEO_MATRIX_PROGRESSIVE +
                            NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
                            NEO_GRB + NEO_KHZ800);
//CRGB leds[NUMPIXELS];
int x = matrix.width();
int pass = 0;
int Mode = 1;
uint8_t color_index = 0;
int scroll_limit = 30; // each text character is 6 pixels wide
char incomingPacket[1024]; //buffer to hold incoming packet,
String  H;
String  M;
String timeText = "Time";
WiFiUDP udp;
NTPClient timeClient(udp, "ntp1.aliyun.com", 60 * 60 * 8, 30 * 60 * 1000);
OneButton button(buttonPin, true);
void setup() {
  Serial.begin(115200);
  button.attachClick(modeChange);
  // FastLED.addLeds<WS2812, PIN, GRB>(leds, NUMPIXELS).setCorrection(TypicalLEDStrip);
  matrix.begin();

  matrix.setTextWrap(false);
  matrix.setBrightness(maxBrightness);
  //matrix.setTextColor(colors[0]);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("UDP server on port %d\n", localPort);
  udp.begin(localPort);
  pad.begin();
  pad.setBrightness(50);
  ArduinoOTA.begin();
}

unsigned long drop_time;
int old_list[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int new_list[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int drop_list[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned long  time_list[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void modeChange()
{
  if (++Mode > 2)Mode = 0;
  Serial.println("按键切换模式");
  Serial.println(Mode);

  pad.clear();
  pad.show();
  int old_list[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int drop_list[8] = {0, 0, 0, 0, 0, 0, 0, 0};

}





void specturm(int c, int pre_trig, int  trig)//音乐频谱显示
{
  if (c % 2 != 0)
  {

    if (trig > pre_trig)
    {
      for (int i = c * 8 + pre_trig; i < c * 8 + trig + 1; i++)
      {

        pad.setPixelColor(i,  pad.Color(35 * (i - c * 8), 0,  250 - 35 * (i - c * 8)));
        pad.show();
        // delay((i - c * 8) * 2);

      }
    }
    if (trig < pre_trig)
    {
      for (int i = c * 8 + pre_trig; i > c * 8 + trig - 1; i--)
      {

        pad.setPixelColor(i,  pad.Color(0, 0, 0));
        pad.show();
        //delay((7 - (i - c * 8)) * 2);
      }
    }
  }

  if (c % 2 == 0)
  {
    if (trig > pre_trig)
    {
      for (int i = (c + 1) * 8 - 1 - pre_trig; i > ((c + 1) * 8 - 1) - trig - 1; i--)
      {
        pad.setPixelColor(i,  pad.Color(250 - 35 * (i - c * 8), 0,  35 * (i - c * 8)));
        pad.show();
        //delay(((c + 1) * 8 - 1 - i) * 2);

      }
    }
    if (trig < pre_trig)
    {
      for (int i = (c + 1) * 8 - 1 - pre_trig; i < ((c + 1) * 8 - 1) - trig + 1; i++)
      {
        pad.setPixelColor(i, pad.Color(0, 0, 0));

        pad.show();
        // delay((7 - ((c + 1) * 8 - 1 - i)) * 2);
      }
    }
  }

  if ( drop_list[c] >= trig )//下落效果
  {
    time_list[c] = millis();

    if (c % 2 != 0)
    {

      if (drop_list[c] < 7)
      {
        pad.setPixelColor(c * 8 + drop_list[c] + 1, pad.Color(0, 0, 0));
      }
      pad.setPixelColor(c * 8 + drop_list[c], pad.Color(0, 200, 128));
    }

    if (c % 2 == 0)
    {
      if (drop_list[c] < 7)
      {
        pad.setPixelColor((c + 1) * 8 - 1 - drop_list[c] - 1, pad.Color(0, 0, 0));
        pad.show();//
      }
      pad.setPixelColor((c + 1) * 8 - 1 - drop_list[c], pad.Color(0, 250, 128));
      //pad.show();//
    }

    drop_list[c] = drop_list[c] - 1;

    pad.show();

  }


}

//Matrix
int pos[8] = {random(0, 16), random(0, 16), random(0, 16), random(0, 16), random(0, 16), random(0, 16), random(0, 16), random(0, 16)};

uint32_t myRGBColorPalette[16] = { pad.Color(0, 250, 250, 250),  pad.Color(0, 210, 0, 230),  pad.Color(0, 210, 0, 200),  pad.Color(0, 150, 0, 150),
                                   pad.Color(0, 150, 50, 100),  pad.Color(0, 120, 0, 80),  pad.Color(0, 50, 0, 50),  pad.Color(0, 20, 10, 20),
                                   pad.Color(0, 250, 250, 250),  pad.Color(0, 210, 0, 230),  pad.Color(0, 210, 0, 200),  pad.Color(0, 150, 0, 150),
                                   pad.Color(0, 150, 50, 100),  pad.Color(0, 120, 0, 80),  pad.Color(0, 50, 0, 50),  pad.Color(0, 20, 10, 20)
                                 };




void Matrix()
{

  for (int y = 0; y < 8; y++)
  {
    for (int x = 0; x < 8; x++)
    {
      int P = pos[x] + y;
      if (P > 15)P = P - 16;


      if (x % 2 == 0) pad.setPixelColor(x * 8 + y, myRGBColorPalette[P]);
      if (x % 2 != 0)pad.setPixelColor((x + 1) * 8 - y - 1, myRGBColorPalette[P]);
    }
  }
  for (int c = 0; c < 8; c++)
  {
    if (--pos[c] < 0)pos[c] = 15;
  }
  pad.show();
  delay(100);
}




unsigned long lastTime;
void showTime()
{
  timeClient.update();
  H = timeClient.getFormattedTime().substring(0, 2);
  M = timeClient.getFormattedTime().substring(3, 5);
  timeText = H + ":" + M;
  if ( millis() - lastTime > 100)
  {
    lastTime = millis();
    matrix.fillScreen(1);
    matrix.setCursor(x, 1);
    matrix.print(timeText);
    //matrix.print(H + M);
    if (--x < -30) {

      x = matrix.width();

      //if (++pass >= 3) pass = 0;

      matrix.setTextColor(color_wheel(color_index));
      color_index += max(1, 256 / scroll_limit);
    }
    matrix.show();
  }

}
void showVu() {
  ArduinoOTA.handle();
  int packetSize = udp.parsePacket();//获得解析包
  Serial.println(packetSize);

  //Serial.println(udp.remoteIP().toString().substring(0, 9));

  if (packetSize) //解析包不为空
  {
    //Serial.printf("收到来自远程IP：%s（远程端口：%d）的数据包字节数：%d\n", udp.remoteIP().toString().c_str(), udp.remotePort(), packetSize);

    int len = udp.read(incomingPacket, 255);
    //Serial.println(incomingPacket);
    incomingPacket[len] = 0;

    if (len > 0)
    {

      String data = String(incomingPacket);

      for (int k = 0; k < 8; k++)
      {
        new_list[k] = data.substring((k + 1) * 3 - 1, (k + 1) * 3).toInt();
      }
      maxBrightness = data.substring(24, 25).toInt() * 100 + data.substring(25, 26).toInt() * 10 + data.substring(26, 27).toInt();

      pad.setBrightness(maxBrightness);
      matrix.setBrightness(maxBrightness);
     



    }
    for (int k = 0; k < 8; k++)
    {
      if (new_list[k]  > drop_list[k])
      {
        drop_list[k] = new_list[k];
      }
      specturm(k, int(old_list[k]), int(new_list[k]));
    }
    for (int s = 0; s < 8; s++)
    {
      old_list[s] = new_list[s];
    }
  }
  udp.stop();
  udp.begin(localPort);
  timeClient.update();
}




uint16_t color_wheel(uint8_t pos) {
  pos = 255 - pos;
  if (pos < 85) {
    //  return ((uint32_t)(255 - pos * 3) << 16) | ((uint32_t)(0) << 8) | (pos * 3);
    return matrix.Color((uint16_t)(255 - pos * 3), 0, (pos * 3));
  } else if (pos < 170) {
    pos -= 85;
    //  return ((uint32_t)(0) << 16) | ((uint32_t)(pos * 3) << 8) | (255 - pos * 3);
    return matrix.Color(0, (uint32_t)(pos * 3), (255 - pos * 3));
  } else {
    pos -= 170;
    //  return ((uint32_t)(pos * 3) << 16) | ((uint32_t)(255 - pos * 3) << 8) | (0);
    return matrix.Color((uint16_t)(pos * 3), (uint32_t)(255 - pos * 3), 0);
  }
}
void loop()
{
  ArduinoOTA.handle();
  button.tick();
  
  switch (Mode)
  {
    case 0:
      Matrix();
      break;
    case 1:
      showVu();
      break;
    case 2:
      Matrix();
      break;
  }
}
