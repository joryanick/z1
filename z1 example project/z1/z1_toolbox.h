// z1_toolbox.h
//
// GoodPrototyping z1 rev.C1 desktop
// (c) 2013-2018 Jory Anick, jory@goodprototyping.com

#ifndef z1toolbox_h
#define z1toolbox_h

#include "LCDMenu2.h"   // menu system

  #define DEBUG    false
  #define PULL_UP  HIGH 
  
  // =CONSTANTS=====================================
  
  #define pinRTCPOWER                           A2  // power pin to RTC
  #define pinBATSENSOR                          A3  // battery sensor via a voltage divider of 2 x 10k resistors
  
  const uint8_t pinLEDR                      =  4;  // red   LED
  const uint8_t pinLEDG                      =  5;  // green LED
  const uint8_t pinLEDB                      =  6;  // blue  LED

  const uint8_t pinRESET                     =  30; // tied to RESET PIN
  const uint8_t pinSPEAKER                   =  10; // speaker
  const uint8_t pinSLEEPBTN                  =  3;  // sleep btn
  
  const uint8_t pinBtnMENU                   =  22; // menu  
  const uint8_t pinBtnFWD                    =  24; // forwards
  const uint8_t pinBtnBKWD                   =  26; // backwards
  const uint8_t pinBtnSELECT                 =  28; // select
  
  // eeprom save positions
  const uint8_t EEPROM_LOC_TIMEZONEPLUS20    =  0; // Time Zone + 20 (to handle negatives)
  const uint8_t EEPROM_RTC_BASE_YEAR1        =  1; // (RTC Baseyear / 256) and 255
  const uint8_t EEPROM_RTC_BASE_YEAR2        =  2; // (RTC Baseyear and 255)

  // for DS1307 RTC (DO NOT ADJUST)
  const uint8_t RTC_YEAR                     =  6;
  const uint8_t RTC_MONTH                    =  5;
  const uint8_t RTC_DAY                      =  4;
  const uint8_t RTC_HOUR                     =  2;
  const uint8_t RTC_MINUTE                   =  1;
  const uint8_t RTC_SECOND                   =  0;
  
  struct DateType
  {
   int yr;
   uint8_t mo;
   uint8_t dy;
   uint8_t hr;
   uint8_t mn;
   uint8_t sc;
  };

  extern TVout TV;  
  extern int   Time_Zone;     // time zone value
  extern int   RTCBaseYear;
  
  // rgb led variables
  struct RGBLEDFmt
  {
    uint8_t val;
    uint8_t dir;
  };
  
// =SUBS & FUNCTIONS================================
  
  // hardware initialization
  void    z1_init();
  
  // rtc functions
  void    z1_StartRTC();
  void    z1_StopRTC();
  void    z1_SaveNewRTCBaseYear();
  void    z1_SetRTCDate(uint8_t, uint8_t, int);
  void    z1_SetRTCTime(uint8_t, uint8_t, uint8_t);
  void    z1_GetTimeFromRTC();
  void    z1_GetDateAndTimeFromRTC();  
  uint8_t z1_GetSecondsFromRTC();
    
  // sound functions
  void    z1_makeSound(int, int);
  void    z1_Beep();
  
  void    z1_makeSoundAsync(int);
  void    z1_soundOff();

  // sleep button functions
  void    z1_InitSleepBTN();
  boolean z1_checkSleepBTN();
  void    z1_Sleep();

  // eeprom functions
  void    z1_CheckEEPROMFormatted();
  void    z1_ClearEEPROM();
  void    z1_SaveSettingValue(int, uint8_t);
  uint8_t z1_GetSettingValue(int, uint8_t);

  // reboot function
  void    z1_Reboot();

  // debugging functions (displays to TVout)
  void    z1_DebugPrint(char *msg);
  void    z1_DebugPrint(float, byte);
  void    z1_DebugPrint(int f);

  // get free memory in bytes
  int     z1_AvailableMemory();
  
  // date functions
  int     DaysInMonth(int);
  uint8_t Weekday(uint8_t, uint8_t, int);

  void    GetAppVerString(char *); 
  
#endif
// end of z1_toolbox.h
