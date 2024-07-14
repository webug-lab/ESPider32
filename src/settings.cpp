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