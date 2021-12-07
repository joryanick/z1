// z1.h
//
// GoodPrototyping z1 rev.C1 desktop
// (c) 2013-2018 Jory Anick, jory@goodprototyping.com

#ifndef z1_h
#define z1_h

#include <Arduino.h>

// for sleep btn
#include <avr/sleep.h>

// for video display
#include <TVout.h>
#include <font4x6.h>
#include <video_gen.h>

// for PROGMEM storage
#include <inttypes.h>
#include <avr/pgmspace.h>

#include "z1_toolbox.h"  // z1 toolbox functions

  extern TVout TV;
  extern RGBLEDFmt RGBLED[2];
  extern DateType CustomDate;
  
  const uint8_t VERSION_NUMBER               =  103;
  
  const uint8_t SCREENWIDTH                  =  136;
  const uint8_t SCREENHEIGHT                 =  96;

  // menu lcd settings
  const uint8_t LCD_rows                     =  8;
  const uint8_t LCD_cols                     =  20;
  
  // screen modes
  const uint8_t SCREENMODE_APP               =  0;
  const uint8_t SCREENMODE_CLOCK             =  1; 
  const uint8_t SCREENMODE_STARFIELD         =  2;
  
  // menu modes
  const uint8_t MENUMODE_MENU                =  0;  // Menu Mode
  const uint8_t MENUMODE_APP                 =  1;  // App mode
  const uint8_t MENUMODE_CLOCK               =  2;  // Clock Mode
  const uint8_t MENUMODE_SETTIME             =  3;  // Set Time Mode
  const uint8_t MENUMODE_SETDATE             =  4;  // Set Date Mode
  const uint8_t MENUMODE_SETTZONE            =  5;  // Set Time Zone  
  const uint8_t MENUMODE_ABOUT               =  6;  // About Mode  
  const uint8_t MENUMODE_CLREEPROM           =  7;  // Clear EEPROM
  
  // UI Portion
  const prog_char CONST_APPNAME[] PROGMEM = "z1";
  const prog_char CONST_COPYRIGHTSTRING[] PROGMEM = "(c) 2013 GOODPROTOTYPING.COM";
  const prog_char CONST_ERASING[] PROGMEM = "ERASING...";
  const prog_char CONST_CLOCKRESET[] PROGMEM = "CLOCK WAS RESET";
  
  const prog_char CONST_PERIOD[] PROGMEM = ".";
  const prog_char CONST_FWDSLASH[] PROGMEM = "/";
  const prog_char CONST_BYTESFREE[] PROGMEM = " BYTES FREE";
  const prog_char CONST_AM[] PROGMEM = " AM";
  const prog_char CONST_PM[] PROGMEM = " PM";
  
  const prog_char MENUCONST_ROOT[] PROGMEM = "Root";
  const prog_char MENUCONST_CLOCK[] PROGMEM = "CLOCK";
  const prog_char MENUCONST_SETTINGS[] PROGMEM = "SETTINGS";
  const prog_char MENUCONST_SETTIME[] PROGMEM = "SET TIME";
  const prog_char MENUCONST_SETDATE[] PROGMEM = "SET DATE";
  const prog_char MENUCONST_REBOOT[] PROGMEM = "REBOOT";
  const prog_char MENUCONST_ABOUT[] PROGMEM = "ABOUT";  
  const prog_char MENUCONST_CLEARALLSETTINGS[] PROGMEM = "CLEAR SETTINGS";
  const prog_char MENUCONST_SETTIMEZONE[] PROGMEM = "SET TIME ZONE";

  const prog_char MENUCONST_G1[] PROGMEM = "GMT -12 ENIWETOK";
  const prog_char MENUCONST_G2[] PROGMEM = "GMT -11 SAMOA";
  const prog_char MENUCONST_G3[] PROGMEM = "GMT -10 HAWAII";
  const prog_char MENUCONST_G4[] PROGMEM = "GMT -9  ALASKA";
  const prog_char MENUCONST_G5[] PROGMEM = "GMT -8  PACIFIC";
  const prog_char MENUCONST_G6[] PROGMEM = "GMT -7  MOUNTAIN";
  const prog_char MENUCONST_G7[] PROGMEM = "GMT -6  CENTRAL";
  const prog_char MENUCONST_G8[] PROGMEM = "GMT -5  EASTERN";
  const prog_char MENUCONST_G9[] PROGMEM = "GMT -4  ATLANTIC";
  const prog_char MENUCONST_G10[] PROGMEM = "GMT -3  BUENOS AIRES";
  const prog_char MENUCONST_G11[] PROGMEM = "GMT -2  MID ATLANTIC";
  const prog_char MENUCONST_G12[] PROGMEM = "GMT -1  CAPE VERDE";
  const prog_char MENUCONST_G13[] PROGMEM = "GMT +0  LONDON";
  const prog_char MENUCONST_G14[] PROGMEM = "GMT +1  PARIS";
  const prog_char MENUCONST_G15[] PROGMEM = "GMT +2  S AFRICA";
  const prog_char MENUCONST_G16[] PROGMEM = "GMT +3  MOSCOW";
  const prog_char MENUCONST_G17[] PROGMEM = "GMT +4  ABU DHABI";
  const prog_char MENUCONST_G18[] PROGMEM = "GMT +5  KARACHI";
  const prog_char MENUCONST_G19[] PROGMEM = "GMT +6  DHAKA";
  const prog_char MENUCONST_G20[] PROGMEM = "GMT +7  BANGKOK";
  const prog_char MENUCONST_G21[] PROGMEM = "GMT +8  HONG KONG";
  const prog_char MENUCONST_G22[] PROGMEM = "GMT +9  TOKYO";
  const prog_char MENUCONST_G23[] PROGMEM = "GMT +10 GUAM";
  const prog_char MENUCONST_G24[] PROGMEM = "GMT +11 SOLOMONS";
  const prog_char MENUCONST_G25[] PROGMEM = "GMT +12 FIJI";   

  const uint8_t bignums_COLON[] PROGMEM = {
    11, 15, // WIDTH, HEIGHT
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x00,0x1C,0x00,0x00,0x00,0x00,0x00,0x1C,0x00,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
    
  const uint8_t bignums_0[] PROGMEM = {
    11, 15, // WIDTH, HEIGHT
  0x1F,0x00,0x7F,0xC0,0x71,0xC0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0x71,0xE0,0x7F,0xC0,0x1F,0x00 };

  const uint8_t bignums_1[] PROGMEM = {
    11,15,
  0x7E,0x00,0x7E,0x00,0x0E,0x00,0x0E,0x00,0x0E,0x00,0x0E,0x00,0x0E,0x00,0x0E,0x00,0x0E,0x00,0x0E,0x00,0x0E,0x00,0x0E,0x00,0x0E,0x00,0x0E,0x00,0x0E,0x00 };
  
  const uint8_t bignums_2[] PROGMEM = {
    11,15,
  0x1F,0x00,0x7F,0xC0,0x71,0xC0,0xE0,0xE0,0xE0,0xE0,0x00,0xE0,0x01,0xE0,0x03,0xC0,0x07,0x80,0x0F,0x00,0x1E,0x00,0x3C,0x00,0x70,0x00,0xFF,0xE0,0xFF,0xE0 };

  const uint8_t bignums_3[] PROGMEM = {
    11,15,
  0x1F,0x00,0x3F,0x80,0x79,0xC0,0x71,0xC0,0x01,0xC0,0x0F,0x80,0x0F,0xC0,0x01,0xC0,0x00,0xE0,0x00,0xE0,0xE0,0xE0,0xE0,0xE0,0x71,0xC0,0x7F,0xC0,0x1F,0x00 };

  const uint8_t bignums_4[] PROGMEM = {
    11,15,
  0x01,0xC0,0x03,0xC0,0x07,0xC0,0x07,0xC0,0x0F,0xC0,0x1D,0xC0,0x3D,0xC0,0x39,0xC0,0x71,0xC0,0xF1,0xC0,0xFF,0xE0,0xFF,0xE0,0x01,0xC0,0x01,0xC0,0x01,0xC0 };  

  const uint8_t bignums_5[] PROGMEM = {
    11,15,
  0x3F,0xC0,0x3F,0xC0,0x30,0x00,0x70,0x00,0x70,0x00,0x7F,0x00,0x7F,0xC0,0x71,0xC0,0x00,0xE0,0x00,0xE0,0xE0,0xE0,0xE0,0xE0,0xF1,0xC0,0x7F,0xC0,0x3F,0x00 };

  const uint8_t bignums_6[] PROGMEM = {
    11,15,
  0x07,0x80,0x0F,0x00,0x0E,0x00,0x1C,0x00,0x1C,0x00,0x3F,0x80,0x7F,0xC0,0x71,0xC0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0x71,0xC0,0x7F,0xC0,0x1F,0x00 };
    
  const uint8_t bignums_7[] PROGMEM = {
    11,15,
  0x7F,0xC0,0x7F,0xC0,0x01,0xC0,0x03,0xC0,0x03,0x80,0x03,0x80,0x07,0x00,0x07,0x00,0x0E,0x00,0x0E,0x00,0x1E,0x00,0x1C,0x00,0x1C,0x00,0x38,0x00,0x38,0x00 };

  const uint8_t bignums_8[] PROGMEM = {
    11,15,
  0x1E,0x00,0x3F,0x00,0x73,0x80,0x73,0x80,0x73,0x80,0x73,0x80,0x3F,0x00,0x7F,0x80,0xF3,0xC0,0xE1,0xC0,0xE1,0xC0,0xE1,0xC0,0xF3,0xC0,0x7F,0x80,0x3F,0x00 };

  const uint8_t bignums_9[] PROGMEM = {
    11,15,
  0x1F,0x00,0x7F,0xC0,0x71,0xC0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0x71,0xC0,0x7F,0xC0,0x3F,0x80,0x07,0x00,0x0F,0x00,0x0E,0x00,0x1E,0x00,0x3C,0x00 };
  
  // days of week
  const prog_char Sun[] PROGMEM = "SUN";   const prog_char Mon[] PROGMEM = "MON";
  const prog_char Tue[] PROGMEM = "TUE";   const prog_char Wed[] PROGMEM = "WED";
  const prog_char Thu[] PROGMEM = "THU";   const prog_char Fri[] PROGMEM = "FRI";
  const prog_char Sat[] PROGMEM = "SAT";
  
 #endif
 // end of z1.h
