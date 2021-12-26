/****************************************************************************************************************************
  Async_AutoConnect_ESP32_minimal.ino
  For ESP8266 / ESP32 boards
  Built by Khoi Hoang https://github.com/khoih-prog/ESPAsync_WiFiManager
  Licensed under MIT license
 *****************************************************************************************************************************/
#if !(defined(ESP32) )
  #error This code is intended to run on the ESP32 platform! Please check your Tools->Board setting.
#endif
#include <ESPAsync_WiFiManager.h>              //https://github.com/khoih-prog/ESPAsync_WiFiManager
AsyncWebServer webServer(80);
#if !( USING_ESP32_S2 || USING_ESP32_C3 )
DNSServer dnsServer;
#endif

#include <Arduino.h>
#include <U8g2lib.h>

#include <Adafruit_GFX.h>
#include <FastLED.h>
#include <FastLED_NeoMatrix.h>

#define PIN 2

#define SSID_MAX_LEN    32
#define PASS_MAX_LEN    64

// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)


// Example for NeoPixel Shield.  In this application we'd like to use it
// as a 5x8 tall matrix, with the USB port positioned at the top of the
// Arduino.  When held that way, the first pixel is at the top right, and
// lines are arranged in columns, progressive order.  The shield uses
// 800 KHz (v2) pixels that expect GRB color data.
#define mw 8
#define mh 32
#define NUMMATRIX (mw*mh)
#define CHARWIDTH 6

CRGB matrixleds[NUMMATRIX];

U8G2_SSD1306_128X64_NONAME_F_HW_I2C   u8g2(U8G2_R0, 16, 15, 4);

ESPAsync_WiFiManager* wifiManager;

FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(matrixleds, mw, mh, mw/8, 1, 
  NEO_MATRIX_TOP + NEO_MATRIX_RIGHT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG );

const uint16_t colors[] = {
  matrix->Color(255, 0, 0), matrix->Color(0, 255, 0), matrix->Color(0, 0, 255) };

void setup_oled(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* u8g2) {
  // Initialize the graphics library.
  u8g2->begin();
  u8g2->setFont(u8g2_font_6x10_tf);
  u8g2->setFontRefHeightExtendedText();
  u8g2->setDrawColor(1);
  u8g2->setFontPosTop();
  u8g2->setFontDirection(0);

  u8g2->clearBuffer();
}

void setup() {
#if ( USING_ESP32_S2 || USING_ESP32_C3 )
  wifiManager = new ESPAsync_wifiManager(&webServer, NULL, "ChicaGO");
#else
  wifiManager = new ESPAsync_WiFiManager(&webServer, &dnsServer, "ChicaGO");
#endif

  setup_oled(&u8g2);

  wifiManager->setAPStaticIPConfig(IPAddress(192,168,132,1), IPAddress(192,168,132,1), IPAddress(255,255,255,0));
  u8g2.drawStr(0,0, "Connect to ChicaGO");
  u8g2.sendBuffer();
  wifiManager->autoConnect("ChicaGO");
  u8g2.clearBuffer();

  if (WiFi.status() == WL_CONNECTED) { 
    u8g2.drawStr(0,0, "Connected!");
    String IP = WiFi.localIP().toString();
    u8g2.drawStr(0, 15, IP.c_str());
    String SSID = WiFi.SSID();
    u8g2.drawStr(0, 30, SSID.c_str());
  }
  else { 
    u8g2.drawStr(0,0, "Problems connecting.");
  }

  u8g2.sendBuffer();

  FastLED.addLeds<WS2812B,PIN>(matrixleds, NUMMATRIX); 
  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(40);
  matrix->setTextColor(colors[0]);
  matrix->setTextSize(1);
  matrix->setRotation(45);

}

int x    = mw;
int pass = 0;
int len = 0;

const char* message = "UwU OwO";

void loop() {
  matrix->fillScreen(0);
  matrix->setCursor(x, 0);
  matrix->print(F(message));
  len = strlen(message)*CHARWIDTH;
  if(--x < -len) {
    x = matrix->width();
    if(++pass >= 3) pass = 0;
    matrix->setTextColor(colors[pass]);
  }
  matrix->show();
  delay(100);

}
