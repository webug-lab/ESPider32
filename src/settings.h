#include <EEPROM.h>

void setBrightness(int bright, bool save = true);

void getBrightness();

int gsetRotation(bool set = false);

void setBrightnessMenu();

void setUIColor();

int gsetIrTxPin(bool set = false);

int gsetIrRxPin(bool set = false);

int gsetRfTxPin(bool set = false);

int gsetRfRxPin(bool set = false);
