/// z1_toolbox.ino
//
// GoodPrototyping z1 rev.C1 desktop
// (c) 2013-2018 Jory Anick, jory@goodprototyping.com

#include <Arduino.h>
#include <avr/sleep.h>
#include <EEPROM.h>
#include <Wire.h> // for rtc

// for video display
#include <TVout.h>
#include <font4x6.h>
#include <video_gen.h>

#include "z1_toolbox.h"

  // composite TVout video display
  TVout TV;
  DateType CustomDate; // date and time from z1
  
  // RGB LED variables
  RGBLEDFmt RGBLED[2];
  
  // RTC variables
  int RTCBaseYear;            // base year for RTC (DS1307 RTC only supports 100yrs)
  boolean RTC_State_ON;       // real-time clock ON/OFF state

  int Time_Zone;              // time zone in hours from GMT

  uint8_t decToBcd(uint8_t val);
  uint8_t bcdToDec(uint8_t val);
  
void z1_init()
{        
  // keypad
  // enable internal pull-up resistors for menu buttons
  pinMode(pinBtnFWD, INPUT);
  pinMode(pinBtnBKWD, INPUT);
  pinMode(pinBtnSELECT, INPUT);
  pinMode(pinBtnMENU, INPUT);
  digitalWrite(pinBtnFWD, PULL_UP);
  digitalWrite(pinBtnBKWD, PULL_UP); 
  digitalWrite(pinBtnSELECT, PULL_UP);   
  digitalWrite(pinBtnMENU, PULL_UP);      

  // RGB LED
  pinMode(pinLEDR, OUTPUT);
  pinMode(pinLEDG, OUTPUT);
  pinMode(pinLEDB, OUTPUT);
  
  analogWrite(pinLEDR,3);
  analogWrite(pinLEDG,3);
  analogWrite(pinLEDB,1);
  
  // init battery sensor
  pinMode(pinBATSENSOR, INPUT); // battery sensor
  
  // init button to sleep/wake
  z1_InitSleepBTN();
}

int z1_AvailableMemory()
{
extern unsigned long __bss_end;
extern void *__brkval;

  int freeValue;
  freeValue = 0;

  if ((unsigned long)__brkval == 0)
    freeValue = ((unsigned long)&freeValue) - ((unsigned long)&__bss_end);
  else
    freeValue = ((unsigned long)&freeValue) - ((unsigned long)__brkval);

  return freeValue;
}

// registry code /////////////////////////////////////////////////////////////  
void z1_CheckEEPROMFormatted()
{
  // clear eeprom if format bit is not set
 if ((EEPROM.read(510) != 127) || (EEPROM.read(511) != 127))
   z1_ClearEEPROM();
}

void z1_ClearEEPROM()
{
int i;
  
  // reset all 512 bytes
  for (i = 0; i < 512; i++)
    EEPROM.write(i, 255); // write all FF's to every memory block
  
  // set formatted
  EEPROM.write(510, 127);
  EEPROM.write(511, 127);
}

void z1_SaveSettingValue(int settingNum, uint8_t theValue)
{      
    EEPROM.write(settingNum,theValue);
}

// fetches an entry from the saved eeprom or returns the default if no entry (value was 0)
uint8_t z1_GetSettingValue(int settingNum, uint8_t defaultRtn)
{
uint8_t a;
  
    a = EEPROM.read(settingNum);
    if (a == 255)
      return (defaultRtn);
    else
      return (a);
}
// end of registry code ///////////////////////////////////////////////////

int DaysInMonth(int mo)
{
  switch (mo)
  {
    case 1: case 3: case 5: case 7: case 8: case 10: case 12:
      // 31 days      
      return(31);
      break;
      
    case 4: case 6: case 9: case 11:
      // 30 days
      return(30);
      break;
    
    case 2: // feb
      // 28 or 29 days, check for leap year        
      if ((CustomDate.yr % 4) == 0)
      {
        if ((CustomDate.yr % 100) == 0)
        {
          if ((CustomDate.yr % 400) > 0)
            return(29);
          else
              return(29);
        }
      }
      return(28);
      break;
  }
}      

uint8_t Weekday(uint8_t mo, uint8_t dy, int yr)
{
uint16_t mm;
uint16_t yy;
uint8_t a;

  // Calculate day of the week    
  mm = mo;
  yy = yr;
  if (mm < 3)
  {
    mm += 12;
    yy -= 1;
  }
  
  a = (dy + (2 * mm) + (6 * (mm + 1)/10) + yy + (yy/4) - (yy/100) + (yy/400) + 1) % 7;
  return(a);
}

//// rtc functions ///////////////////////////////////////////////////

// use the wire library to read via i2c
uint8_t inline ReadRTCByte(uint8_t adr)
{
 uint8_t data;
 
  Wire.beginTransmission(0x68);
  Wire.write(adr);
  Wire.endTransmission();
 
  Wire.requestFrom(0x68,1);
  while (Wire.available()) data=Wire.read();

  return data;
}

// write a byte to the clock
void inline WriteRTCByte(uint8_t adr, uint8_t data)
{
  Wire.beginTransmission(0x68);
  Wire.write(adr);
  Wire.write(decToBcd(data));
  Wire.endTransmission();
} 

// retrieve a byte from the clock
uint8_t inline GetRTCData(uint8_t adr, uint8_t validbits)
{
uint8_t data;
 
  // read using wire library
  data = ReadRTCByte(adr);

  // adjust the return value depending
  // on the number of valid bits    
  data=data & 0xff >> (8-validbits);
  return(bcdToDec(data));
}

// convert normal decimal numbers to binary coded decimal
uint8_t decToBcd(uint8_t val)
{
  return ((val/10*16) + (val%10));
}

// convert binary coded decimal to normal decimal numbers
uint8_t bcdToDec(uint8_t val)
{
  return ((val/16*10) + (val%16));
}

uint8_t inline z1_GetSecondsFromRTC()
{
int a;  

  if (RTC_State_ON == false)
    z1_StartRTC();
    
  a = (int)GetRTCData(RTC_SECOND,7);

  // sanity bounds check
  if ((a<0) || (a > 59))
    a=0;   

  z1_StopRTC();

  return(a);
}

void inline z1_GetTimeFromRTC()
{ 
  if (RTC_State_ON == false)
    z1_StartRTC();
  
  // time    
  CustomDate.hr  = GetRTCData(RTC_HOUR,6);
  CustomDate.mn  = GetRTCData(RTC_MINUTE,7);
  CustomDate.sc  = GetRTCData(RTC_SECOND,7); 
  
  z1_StopRTC();
}

void inline z1_GetDateAndTimeFromRTC()
{ 
  if (RTC_State_ON == false)
    z1_StartRTC();
  
  // time    
  CustomDate.hr  = GetRTCData(RTC_HOUR,6);
  CustomDate.mn  = GetRTCData(RTC_MINUTE,7);
  CustomDate.sc  = GetRTCData(RTC_SECOND,7); 
  
  // date
  CustomDate.yr  = RTCBaseYear + (int)GetRTCData(RTC_YEAR,8);
  CustomDate.mo  = GetRTCData(RTC_MONTH,5);
  CustomDate.dy  = GetRTCData(RTC_DAY,6);

  z1_StopRTC();
}

void inline z1_SetRTCTime(uint8_t h, uint8_t m, uint8_t s)
{
  if (RTC_State_ON == false)
    z1_StartRTC();
  
  WriteRTCByte(RTC_SECOND, s);
  WriteRTCByte(RTC_MINUTE, m);
  WriteRTCByte(RTC_HOUR, h);
  
  z1_StopRTC();
}

void inline z1_SetRTCDate(uint8_t mo, uint8_t d, int y)
{
  if (RTC_State_ON == false)
    z1_StartRTC();

  WriteRTCByte(RTC_YEAR, (uint8_t) y-RTCBaseYear);
  WriteRTCByte(RTC_MONTH, mo);
  WriteRTCByte(RTC_DAY, d);
  
  z1_StopRTC();
}

void z1_SaveNewRTCBaseYear()
{
int a;

  // initialize & format if necessary
  z1_CheckEEPROMFormatted();
  
  // save new RTC BASE YEAR to eeprom
  a = RTCBaseYear / 256;
  z1_SaveSettingValue(EEPROM_RTC_BASE_YEAR1, a);
  
  a = RTCBaseYear - (a * 256);
  z1_SaveSettingValue(EEPROM_RTC_BASE_YEAR2, a); 
}

void inline z1_StopRTC()
{
  // drop power to chip
  pinMode (pinRTCPOWER, INPUT);  
  // turn off pull-up
  digitalWrite (pinRTCPOWER, LOW);

  // turn off I2C
  // according to: http://arduino.cc/forum/index.php/topic,89182.0.html
  TWCR &= ~(_BV(TWEN) | _BV(TWIE) | _BV(TWEA));
  
  RTC_State_ON = false;
}

void inline z1_StartRTC()
{
  pinMode (pinRTCPOWER, OUTPUT);
  digitalWrite (pinRTCPOWER, HIGH);
  
  // wait for the device to initialize
  TV.delay(2);
  
  Wire.begin(); // for rtc  
  RTC_State_ON = true;
}
// end rtc ///////////////////////////////////////////////////////////////////

// misc functions
void inline z1_Reboot()
{
  // assumes pin is tied to RST
  analogWrite(pinRESET, LOW);
}

void inline GetAppVerString(char * s)
{
char buffer[30];

  // fetch from progmem
  strcpy_P(buffer, CONST_APPNAME);
    
  int n = snprintf(s, 30, "%s V%02d", buffer, VERSION_NUMBER);  
}

// end of misc functions ///////////////////////

// debugging functions
void z1_DebugPrint(char *msg)
{ 
  TV.draw_rect(0, 0, SCREENWIDTH-1, 9, BLACK, BLACK);
  TV.print(0,0,msg);
  TV.delay(2000);
}

char *ftoa(char *a, float f, int precision)
{
  long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};
  
  char *ret = a;
  long intpart = (long)f;
  itoa(intpart, a, 10);
  while (*a != '\0') a++;
  *a++ = '.';
  long decimal = abs((long)((f - intpart) * p[precision]));
  itoa(decimal, a, 10);
  return ret;
}

void z1_DebugPrint(float val, byte precision)
{
  ftoa(buf, val, precision);
  z1_DebugPrint(buf);
}

void z1_DebugPrint(int f)
{  
    int n = sprintf(buf, "%d", f);
    z1_DebugPrint((char *) buf);
}
// end of debugging functions

// sound function
void z1_makeSound(int freq, int duration)
{
  TV.tone(freq);
  
  unsigned long sndWait = TV.millis() + duration;
  while (TV.millis() < sndWait)
  {
    TV.delay(1);
  };
  
  TV.noTone();
}

void z1_makeSoundAsync(int freq)
{
  TV.tone(freq);
}

void z1_soundOff()
{
  TV.noTone();
}

void z1_Beep()
{
  z1_makeSound(900,100);
}
// end of sound function

///////////// sleep functions
void z1_InitSleepBTN()
{
    pinMode(pinSLEEPBTN, INPUT);
    digitalWrite (pinSLEEPBTN, HIGH);  // enable pull-up
}

boolean z1_checkSleepBTN()
{
  if (digitalRead(pinSLEEPBTN) == LOW)
    return(true);
  else
    return(false);
}

void z1_Sleep()
{
  // low-power code courtesy of Nick Gammon, http://gammon.com.au/forum/?id=11497
   
  // disable ADC
  byte old_ADCSRA = ADCSRA;
  ADCSRA = 0;  

  // clear various "reset" flags
  MCUSR = 0;     
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
  sleep_enable();

  noInterrupts();

  attachInterrupt(1, z1_wakeUpNow, LOW); // set wake-up pin
  
  interrupts();
  sleep_cpu(); // here the device is actually put to sleep
 
  // we wake in z1_wakeUpNow() then return to this next line:
  ADCSRA = old_ADCSRA; // re-enable ADC conversion

 // back to app where start values are re-initialized 
}

void z1_wakeUpNow()
{
  // we wake up here
  sleep_disable(); // first thing after waking from sleep
  
  // detach hardware wake button
  detachInterrupt(1);   
}
// end of sleep btn code /////////////////////////////////////////////

// end of z1_toolbox.ino
