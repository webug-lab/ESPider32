#include "globals.h"
#include <EEPROM.h>
#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include "esp32-hal-psram.h"

// Public Globals Variables
int prog_handler;
int rotation;
int IrTx;
int IrRx;
int RfTx;
int RfRx;
bool sdcardMounted;
bool wifiConnected;
bool BLEConnected;
bool returnToMenu;
char timeStr[10];
time_t localTime;
struct tm* timeInfo;
bool clock_set = true;

String ssid;
String pwd;
std::vector<std::pair<std::string, std::function<void()>>> options;
const int bufSize = 4096;
uint8_t buff[bufSize] = {0};

// Protected global variables
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
TFT_eSprite sprite = TFT_eSprite(&tft);
TFT_eSprite draw = TFT_eSprite(&tft);

#if defined(CARDPUTER)
Keyboard_Class Keyboard = Keyboard_Class();
#endif

#include "mykeyboard.h"
#include "display.h"
#include "webInterface.h"
#include "sd_functions.h"
#include "wifi_common.h"
#include "settings.h"
#include "Wire.h"
#include "sniffer.h"

#ifdef CARDPUTER
#include "led_control.h"
#endif

char16_t FGCOLOR = TFT_WHITE;

void setup() {
  Serial.begin(115200);

  log_d("Total heap: %d", ESP.getHeapSize());
  log_d("Free heap: %d", ESP.getFreeHeap());
  if (psramInit()) log_d("PSRAM Started");
  if (psramFound()) log_d("PSRAM Found");
  else log_d("PSRAM Not Found");
  log_d("Total PSRAM: %d", ESP.getPsramSize());
  log_d("Free PSRAM: %d", ESP.getFreePsram());

  prog_handler = 0;
  sdcardMounted = false;
  wifiConnected = false;
  BLEConnected = false;

  Keyboard.begin();
  pinMode(0, INPUT);
  pinMode(10, INPUT);

  tft.init();
  rotation = gsetRotation();
  tft.setRotation(rotation);
  resetTftDisplay();

  #if defined(BACKLIGHT)
  pinMode(BACKLIGHT, OUTPUT);
  #endif

  getBrightness();
  gsetIrTxPin();
  gsetIrRxPin();
  gsetRfTxPin();
  gsetRfRxPin();
  readFGCOLORFromEEPROM();

  int i = millis();
  tft.setTextColor(FGCOLOR, TFT_BLACK);
  tft.setTextSize(3);
  tft.println("\nESPider32");
  tft.setTextSize(2);
  tft.println("// webug\n");
  tft.println("Version " + String(webug_VERSION));

  delay(1000);

  if (!LittleFS.begin(true)) {
    LittleFS.format();
    LittleFS.begin();
  }

  while (millis() < i + 1000) {
    #if defined(CARDPUTER)
    Keyboard.update();
    if (Keyboard.isPressed()) {
      tft.fillScreen(TFT_BLACK);
      delay(10);
      return;
    }
    #endif
  }
  delay(200);
}

void loop() {
  bool redraw = true;
  int index = 0;
  int opt = 3;
  tft.fillRect(0, 0, WIDTH, HEIGHT, BGCOLOR);

  while (1) {
    if (returnToMenu) {
      returnToMenu = false;
      tft.fillScreen(BGCOLOR);
      redraw = true;
    }
    if (redraw) {
      drawMainMenu(index);
      redraw = false;
      delay(200);
    }
    if (checkPrevPress()) {
      index = (index == 0) ? opt - 1 : index - 1;
      redraw = true;
    }
    if (checkNextPress()) {
      index = (index + 1) % opt;
      redraw = true;
    }
    if (checkSelPress()) {
      switch (index) {
        case 0: // WiFi
          if (!wifiConnected) {
            options = {
              {"Sniffer", [=]() { sniffer_setup(); }},
              {"Connect WiFi", [=]() { wifiConnectMenu(); }},
              {"WiFi AP", [=]() { wifiConnectMenu(true); }}
            };
          } else {
            options = {
              {"Disconnect", [=]() { wifiDisconnect(); }}
            };
          }
          options.push_back({"Main Menu", [=]() { backToMenu(); }});
          delay(200);
          loopOptions(options, false, true, "WiFi");
          break;
        case 1: // Servers
          options = {
            {"Web Server", [=]() { loopOptionsWebUi(); }}
          };
          options.push_back({"Main Menu", [=]() { backToMenu(); }});
          delay(200);
          loopOptions(options, false, true, "Servers");
          break;
        case 2: // Settings
          options = {
            {"Brightness", [=]() { setBrightnessMenu(); }},
            {"Color", [=]() { setUIColor(); }}
          };
          options.push_back({"Main Menu", [=]() { backToMenu(); }});
          delay(200);
          loopOptions(options, false, true, "Settings");
          break;
      }
      redraw = true;
    }
  }
}