#include "globals.h"
#include "settings.h"
#include "display.h"  // calling loopOptions(options, true);
#include "wifi_common.h"
#include "mykeyboard.h"
#include <EEPROM.h>


/*********************************************************************
**  Function: setBrightness
**  save brightness value into EEPROM
**********************************************************************/
void setBrightness(int bright, bool save) {
  if(bright>100) bright=100;
  #if !defined(STICK_C_PLUS)
  int bl = MINBRIGHT + round(((255 - MINBRIGHT) * bright/100 ));
  analogWrite(BACKLIGHT, bl);
  #else
  axp192.ScreenBreath(bright);
  #endif
  EEPROM.begin(EEPROMSIZE); // open eeprom
  EEPROM.write(2, bright); //set the byte
  EEPROM.commit(); // Store data to EEPROM
  EEPROM.end(); // Free EEPROM memory
}

/*********************************************************************
**  Function: getBrightness
**  save brightness value into EEPROM
**********************************************************************/
void getBrightness() {
  EEPROM.begin(EEPROMSIZE);
  int bright = EEPROM.read(2);
  EEPROM.end(); // Free EEPROM memory
  
  #if !defined(STICK_C_PLUS)
  int bl = bright;
  analogWrite(BACKLIGHT, bl);
  #else
  axp192.ScreenBreath(bright);
  #endif
  setBrightness(bright);
}

/*********************************************************************
**  Function: gsetRotation
**  get orientation from EEPROM
**********************************************************************/
int gsetRotation(bool set){
  EEPROM.begin(EEPROMSIZE);
  int getRot = EEPROM.read(0);
  int result = ROTATION;

  if(getRot==1 && set) result = 3;
  else if(getRot==3 && set) result = 1;
  else if(getRot<=3) result = getRot;
  else {
    set=true;
    result = ROTATION;
  }

  if(set) {
    rotation = result;
    tft.setRotation(result);
    EEPROM.write(0, result);    // Left rotation
    EEPROM.commit();
  }
  EEPROM.end(); // Free EEPROM memory
  returnToMenu=true;
  return result;
}


/*********************************************************************
**  Function: setBrightnessMenu
**  Handles Menu to set brightness
**********************************************************************/
void setBrightnessMenu() {
  options = {
    {" 20 %", [=]() { setBrightness(20); }},
    {" 40 %", [=]() { setBrightness(40); }},
    {" 60 %", [=]() { setBrightness(60); }},
    {" 80 %", [=]() { setBrightness(80); }},
    {" 100 %", [=]() { setBrightness(100); }},
  };
  delay(200);
  loopOptions(options, true);
  delay(200);
}

void setUIColor(){
  EEPROM.begin(EEPROMSIZE);
  options = {
    {"Red",   [&]() { FGCOLOR=TFT_RED; EEPROM.write(5,2);EEPROM.commit(); }},
    {"Green",   [&]() { FGCOLOR=TFT_DARKGREEN; EEPROM.write(5,3);EEPROM.commit(); }},
    {"Blue",  [&]() { FGCOLOR=TFT_BLUE; EEPROM.write(5,4);EEPROM.commit(); }},
    {"Purple",  [&]() { FGCOLOR=TFT_PURPLE; EEPROM.write(5,0);EEPROM.commit(); }},
    {"White",  [&]() { FGCOLOR=TFT_WHITE; EEPROM.write(5,1);EEPROM.commit(); }},
    {"Yellow",   [&]() { FGCOLOR=TFT_YELLOW; EEPROM.write(5,5);EEPROM.commit(); }},
    {"Orange",   [&]() { FGCOLOR=TFT_ORANGE; EEPROM.write(5,6);EEPROM.commit(); }},
  };
  delay(200);
  loopOptions(options);
  tft.setTextColor(TFT_BLACK, FGCOLOR);
  EEPROM.end();
}

/*********************************************************************
**  Function: gsetIrTxPin
**  get or set IR Pin from EEPROM
**********************************************************************/
int gsetIrTxPin(bool set){
  EEPROM.begin(EEPROMSIZE);
  int result = EEPROM.read(6);
  if(result>50) result = LED;
  if(set) {
    options = {
      {"Default", [&]() { result = LED; }},
      {"M5 IR Mod", [&]() { result = GROVE_SDA; }},
    #ifndef CARDPUTER
      {"G26",     [&]() { result=26; }},
      {"G25",     [&]() { result=25; }},
      {"G0",     [&]() { result=0; }},
    #endif
      {"Groove W", [&]() { result = GROVE_SCL; }},
      {"Groove Y", [&]() { result = GROVE_SDA; }},

    };
    delay(200);
    loopOptions(options);
    delay(200);
    EEPROM.write(6, result);
    EEPROM.commit();
  }
  EEPROM.end();
  returnToMenu=true;
  IrTx = result;
  return result;
}

/*********************************************************************
**  Function: gsetIrRxPin
**  get or set IR Rx Pin from EEPROM
**********************************************************************/
int gsetIrRxPin(bool set){
  EEPROM.begin(EEPROMSIZE);
  int result = EEPROM.read(63);
  if(result>36) result = GROVE_SCL;
  if(set) {
    options = {
      {"M5 IR Mod", [&]() { result = GROVE_SCL; }},
    #ifndef CARDPUTER
      {"G26",     [&]() { result=26; }},
      {"G25",     [&]() { result=25; }},
      {"G0",     [&]() { result=0; }},
    #endif
      {"Groove W", [&]() { result = GROVE_SCL; }},
      {"Groove Y", [&]() { result = GROVE_SDA; }},

    };
    delay(200);
    loopOptions(options);
    delay(200);
    EEPROM.write(63, result);
    EEPROM.commit();
  }
  EEPROM.end();
  returnToMenu=true;
  IrRx = result;
  return result;
}

/*********************************************************************
**  Function: gsetRfTxPin
**  get or set RF Tx Pin from EEPROM
**********************************************************************/
int gsetRfTxPin(bool set){
  EEPROM.begin(EEPROMSIZE);
  int result = EEPROM.read(7);
  if(result>36) result = GROVE_SDA;
  if(set) {
    options = {
      {"Default TX", [&]() { result = GROVE_SDA; }},
    #ifndef CARDPUTER
      {"G26",     [&]() { result=26; }},
      {"G25",     [&]() { result=25; }},
      {"G0",     [&]() { result=0; }},
    #endif
    };
    delay(200);
    loopOptions(options);
    delay(200);
    EEPROM.write(7, result);    // Left rotation
    EEPROM.commit();
  }
  EEPROM.end();
  returnToMenu=true;
  RfTx = result;
  return result;
}
/*********************************************************************
**  Function: gsetRfRxPin
**  get or set FR Rx Pin from EEPROM
**********************************************************************/
int gsetRfRxPin(bool set){
  EEPROM.begin(EEPROMSIZE);
  int result = EEPROM.read(7);
  if(result>36) result = GROVE_SCL;
  if(set) {
    options = {
      {"Default RX", [&]() { result = GROVE_SCL; }},
    #ifndef CARDPUTER
      {"G26",     [&]() { result=26; }},
      {"G25",     [&]() { result=25; }},
      {"G0",     [&]() { result=0; }},
    #endif
    };
    delay(200);
    loopOptions(options);
    delay(200);
    EEPROM.write(7, result);    // Left rotation
    EEPROM.commit();
  }
  EEPROM.end();
  returnToMenu=true;
  RfRx = result;
  return result;
}
