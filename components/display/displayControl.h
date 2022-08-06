#ifndef DISPLAYCONTROL_H
#define DISPLAYCONTROL_H

#include <Arduino.h>
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include <LiquidCrystal_I2C.h> 
#include "displayIcons.h"

void lcdUpdate(String status);

#endif