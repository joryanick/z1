// z1.ino
//
// GoodPrototyping z1 rev.C1 desktop
// (c) 2013-2018 Jory Anick, jory@goodprototyping.com

// This project compiles as-is in Arduino 1.8+
// Copy the contents of the supplied "arduino libraries" folder
// into your Arduino "libraries" directory

// to speed up compilation & upload:
// in file C:\arduino-1.8.5\hardware\arduino\avr\programmers.txt
//usbtinyisp.name=USBtinyISP
//usbtinyisp.protocol=usbtiny
//usbtinyisp.program.tool=avrdude
//usbtinyisp.program.extra_params=-V -B 1<-- disables verification, speeds up transfer to SCK period 1 usec (3x speedup)

#include "z1.h"

  // for arduino 1.6.3+
  #define prog_char char PROGMEM
  
  const uint8_t SCREENWID2 = SCREENWIDTH/2;
  const uint8_t SCREENHEI2 = SCREENHEIGHT/2;
  
  // keypad variables
  const uint8_t but[4]= {pinBtnBKWD,pinBtnFWD,pinBtnSELECT,pinBtnMENU};
  uint8_t butpressed[4]= {0,0,0,0};
  boolean button_press_enter = false;
  unsigned long gBUTTON_press_time = TV.millis();  
  uint8_t BUTTON_press_time = 200;
  
  // screen mode variables
  uint8_t inScreen = MENUMODE_APP;      
  uint8_t appScreenMode = SCREENMODE_APP;
  const boolean rotateScreens = true;
  const uint8_t rotateScreenSeconds = 30;
  int ScreenModeTimeShowing; // holds number of seconds the screen mode is active (to control scrolling)
  
  // big numbers
  const uint8_t * BigNums[] = {
  bignums_0,     
  bignums_1,
  bignums_2,
  bignums_3,
  bignums_4,
  bignums_5,
  bignums_6,
  bignums_7,
  bignums_8,
  bignums_9};
  
  // days of week
  const char * DaysOfWeek[] = {Sun, Mon, Tue, Wed, Thu, Fri, Sat};
  
  // menu definition /////////////////////////////////////////////////////////////////////////////////  
  Menu top(MENUCONST_ROOT);
  LCDMenu2 Root(top, LCD_rows, LCD_cols);
   
  Menu Item1(CONST_APPNAME);
  Menu Item2(MENUCONST_CLOCK);
  Menu Item3(MENUCONST_SETTINGS);
  Menu Item31(MENUCONST_SETTIME);
  Menu Item32(MENUCONST_SETDATE);
  Menu Item33(MENUCONST_SETTIMEZONE);
  // timezones
  Menu Item331(MENUCONST_G1);
  Menu Item332(MENUCONST_G2);
  Menu Item333(MENUCONST_G3);
  Menu Item334(MENUCONST_G4);
  Menu Item335(MENUCONST_G5);
  Menu Item336(MENUCONST_G6);
  Menu Item337(MENUCONST_G7);
  Menu Item338(MENUCONST_G8);
  Menu Item339(MENUCONST_G9);
  Menu Item3310(MENUCONST_G10);
  Menu Item3311(MENUCONST_G11);
  Menu Item3312(MENUCONST_G12);
  Menu Item3313(MENUCONST_G13);
  Menu Item3314(MENUCONST_G14);
  Menu Item3315(MENUCONST_G15);
  Menu Item3316(MENUCONST_G16);
  Menu Item3317(MENUCONST_G17);
  Menu Item3318(MENUCONST_G18);
  Menu Item3319(MENUCONST_G19);
  Menu Item3320(MENUCONST_G20);
  Menu Item3321(MENUCONST_G21);
  Menu Item3322(MENUCONST_G22);
  Menu Item3323(MENUCONST_G23);
  Menu Item3324(MENUCONST_G24);
  Menu Item3325(MENUCONST_G25);

  Menu Item36(MENUCONST_CLEARALLSETTINGS);    
  Menu Item4(MENUCONST_REBOOT);      
  Menu Item5(MENUCONST_ABOUT);    
  // end of menu code //////////////////////////////////////////////////////////////////////////////////////
  
  // for object editing
  uint8_t timeEditHour;
  uint8_t timeEditMinute;
  uint8_t timeEditSecond;
  uint8_t timeEditCursorPos;
  uint8_t dateEditDay;
  uint8_t dateEditMonth;
  int dateEditYear;
  int RTCBaseYearEdit; // holds current base year in century #
  uint8_t dateEditCursorPos;
   
  // for timers
  unsigned long LastMillisRTCcheck = 0; // last RTC check according to millis()
  uint8_t LastCustomDateUpdateSecFromRTC = 0; // last second the custom date was updated  
  uint8_t LastCustomDateUpdateSec = 0; // last second the screen was updated
  uint8_t  TimeCounter = 0; // for updating the universe every minute
  unsigned long LastRTCCheckMS = 0;
  
  char buf[30]; // for string copying, etc 
  boolean Xor1 = false; // for flicker

  ////////////////////////////////////////////////////////////////////////////////////////////////////

  // STARFIELD
  uint8_t StarField_ViewX = SCREENWID2;
  uint8_t StarField_ViewY = SCREENHEI2;
  uint8_t StarFieldSpeed = 6;
  
  // starfield vars
  struct StarField_pixel3D
  {
    uint8_t X;
    uint8_t Y;
    uint8_t Z;
  };
  
  struct StarField_pixel2D
  {
    uint8_t X;
    uint8_t Y;
  };

  // for starfield / dot scrolling  
  const uint8_t CONST_NUMStars = 50;

  boolean StarField_DidInit;
  StarField_pixel3D StarField_star[CONST_NUMStars];
  StarField_pixel2D StarField_star2d[CONST_NUMStars];

  // SUBS & FUNCTIONS /////////////////////////////////////
  void printBigNum(int x, int y, uint8_t theNum);
  void ShowStarFieldData();
  void StarField_InitStar(int i);
  boolean DotAtThisLocation(uint8_t i);

  void setup()
  {
  int i;
  unsigned long waitTime, timediff;
  
    // prepare lcd
    i = TV.begin(NTSC, SCREENWIDTH, SCREENHEIGHT);
    
    // if TV returns 4 there isn't enough memory for it to begin
    if (i == 4)
    {
      while (true) // don't proceed past this error beeping
      {
        for (i=0; i<2; i++)
        {
          // via internal speaker interface
          tone(pinSPEAKER, 700, 100);
          delay (400);
        }
        delay (3000); 
      }
    }

    TV.clear_screen();
    TV.select_font(font4x6);
    
    // initialize z1 hardware
    z1_init();

    // set up menus
    menuinit();
   
    // pc init (does calcs)
    app_InitializeDefaults();

    // opening screen
    TV.clear_screen();
        
    waitTime = TV.millis();
    timediff=0;
    i=0;

    // stars
    do
    {
      drawStarField(false);
      
      timediff = TV.millis() - waitTime;
      if (timediff > 1000)
      {
        // z1 title
        TV.printPGM(SCREENWID2 + 5, SCREENHEI2 + 5, CONST_APPNAME);
        
        // z1 logo        
        TV.draw_circle(SCREENWID2+6,  SCREENHEI2 - 1, 3, 1, -1); // 3
        TV.draw_circle(SCREENWID2+1,   SCREENHEI2 - 5, 5, 1, -1); // 5
        TV.draw_circle(SCREENWID2-6, SCREENHEI2 - 10, 8, 1, -1); // 8
        
        if ((timediff > 2000) && (i==0))
        {
          TV.printPGM(10, SCREENHEIGHT - 6, CONST_COPYRIGHTSTRING);            
          i=1;
        }
      }
    } while (timediff < 4000);

    TV.clear_screen();

    // scroll screen up
    TVScrollUp();

    // make a start-up sound    
    z1_Beep();

    // end of opening screen
    
    // reset display led
    turnOffRGBLED();

    inScreen = MENUMODE_APP;
    
    // set up the initial UI for the mode (menu/app/settings/etc)    
    InitMode();      
  }

  void InitMode()
  {       
    switch(inScreen)
    {
      case MENUMODE_MENU:
        menudisplay();
        break;
      
      case MENUMODE_APP:
        Update(true);
        break;
      
      case MENUMODE_CLOCK:
        TV.clear_screen();
        ShowTimeBig(SCREENWID2-50, SCREENHEI2-4);
        break;      

      case MENUMODE_SETTIME:
        TV.clear_screen(); 
        showMenuLabel(MENUCONST_SETTIME);

        // set default values        
        setValuesForEdit();

        // show time for edit
        ShowTimeForEditing();
        
        // set cursor to edit hour first
        timeEditCursorPos = 1;
        break;
          
      case MENUMODE_SETDATE:
        TV.clear_screen();
        showMenuLabel(MENUCONST_SETDATE);
        
        // set default values        
        setValuesForEdit();

        // show time for edit
        ShowDateForEditing();
        
        // set cursor to edit hour first
        dateEditCursorPos = 1;
        break;

      case MENUMODE_ABOUT:
        TV.clear_screen();
        showMenuLabel(MENUCONST_ABOUT);
        
        GetAppVerString(buf);
        TV.print(9,28, buf);                         
        TV.printPGM(9,38, CONST_COPYRIGHTSTRING);

        TV.print(9,70, z1_AvailableMemory());
        TV.printPGM(CONST_BYTESFREE);
        break;
        
      case MENUMODE_CLREEPROM:
        // clear eeprom
        TV.clear_screen();
        showMenuLabel(MENUCONST_CLEARALLSETTINGS);        
          
        TV.printPGM(9,28, CONST_ERASING);        
        break;
    }    
  }
  
  void loop()  
  {            
  unsigned long lastLoopMillis;
  boolean isNewSecond = false;
          
    lastLoopMillis = TV.millis();
    
    // maintain separate clock to prevent display errors
    if (lastLoopMillis < LastRTCCheckMS) { LastRTCCheckMS=0; } // 50 day rollover protection
    if (lastLoopMillis - LastRTCCheckMS > 200)
    {
      LastRTCCheckMS = lastLoopMillis;
      updateCustomDate(); // get time from RTC and synchronize our custom date
 
      if (LastCustomDateUpdateSec != CustomDate.sc)
      {          
        // set last update second
        LastCustomDateUpdateSec = CustomDate.sc;
        
        // flag new second
        isNewSecond=true;
         
        // TOGGLE XOR VALUE            
        Xor1 = !Xor1;      
      }            
    }
    
    // visual effect
    if (Xor1 == true) SwirlRGBLED();
          
     // checks to see if user pressed the sleep btn
    if (z1_checkSleepBTN() == true)
      customSleep();

    // check keypad buttons
    menu_buttoncheck();
    
    // only process if the screen is on
    // processing
    switch(inScreen)
    {
      case MENUMODE_MENU:
        menu_functioncheck();
        break;
      
      case MENUMODE_APP:      
        if (appScreenMode == SCREENMODE_STARFIELD)
        {
          drawStarField(true);
        } else
          MoveDotsLeft();
      
        // all other modes run once a second on the timer
        if (isNewSecond==true)
          AppTimer_Timer();    // run the timer
        break;
      
      case MENUMODE_CLOCK:
        // show clock
        if (isNewSecond==true)
        {
          ShowTimeBig(SCREENWID2-50, SCREENHEI2-4);
        }
        break;

      case MENUMODE_SETTIME:
        // set time mode
        break;      
        
      case MENUMODE_SETDATE:
        // set date mode
        break;      
      
      case MENUMODE_ABOUT:
        // about mode  
        break;        
        
      case MENUMODE_CLREEPROM:
         // clear eeprom (settings menu)
         z1_ClearEEPROM();
         z1_makeSound(750,100);
         inScreen = MENUMODE_MENU;
         InitMode();           
         break;
    } // switch
  } // void loop
 
  void updateCustomDate()
  {
  uint8_t secnow;
  
    secnow = z1_GetSecondsFromRTC(); // get seconds from RTC    
    
    if (secnow != LastCustomDateUpdateSecFromRTC)
    {     
      LastCustomDateUpdateSecFromRTC = secnow;
      LastMillisRTCcheck = TV.millis();

      // add the difference between seconds
      int diff = secnow - CustomDate.sc;
      if (diff == 0)
        diff = 1;
      else
        if (diff < 0)
          diff = 60 - abs(diff);
      
      // add to custom date    
      CustomDate.sc = CustomDate.sc + diff;

      // add to screen mode time showing
      ScreenModeTimeShowing = ScreenModeTimeShowing + diff;
      if (ScreenModeTimeShowing > 16384)
        ScreenModeTimeShowing = 1;

      // cascade on minute change
      if (CustomDate.sc>59)
      {
        CustomDate.sc = CustomDate.sc-60;
        CustomDate.mn++;
        if (CustomDate.mn>59)
        {
          CustomDate.mn = 0;
          CustomDate.hr++;
          if (CustomDate.hr>23)
          {
             CustomDate.hr=0;
             CustomDate.dy++;           
             if (CustomDate.dy > DaysInMonth(CustomDate.mo))
             {
               CustomDate.dy=1;
               CustomDate.mo++;
               if (CustomDate.mo > 12)
               {
                 CustomDate.mo=1;
                 CustomDate.yr++;
               }                
             }
          }
        }     
      }  
     }
  }

  void setValuesForEdit()
  {
    // for editing
    timeEditHour   = CustomDate.hr;
    timeEditMinute = CustomDate.mn;
    timeEditSecond = CustomDate.sc;
    dateEditDay    = CustomDate.dy;
    dateEditMonth  = CustomDate.mo;
    dateEditYear   = CustomDate.yr;    
    RTCBaseYearEdit = CustomDate.yr/100;
  }

  void app_InitializeDefaults()
  {
    // set up working values and their defaults
    Time_Zone = z1_GetSettingValue(EEPROM_LOC_TIMEZONEPLUS20, 15) - 20; // DEFAULT IS -5
    
    if ((z1_GetSettingValue(510, 0) == 127) && (z1_GetSettingValue(511, 0) == 127)) // if eeprom was formatted & initialized
      // restore old settings
      // 1999 == (values 7 and 207)
      RTCBaseYear = ( (z1_GetSettingValue(EEPROM_RTC_BASE_YEAR1, 7) * 256) + z1_GetSettingValue(EEPROM_RTC_BASE_YEAR2, 207) );
    else
    {
      TV.printPGM(SCREENWID2-28,SCREENHEI2-3, CONST_CLOCKRESET);

      // default initialization of realtime clock
      RTCBaseYear = 1999;
      z1_SaveNewRTCBaseYear();
      z1_SetRTCTime(12,0,0);     // noon
      z1_SetRTCDate(1, 1, 2010); // jan 1, 2010
    }

    // GET time from RTC
    z1_GetDateAndTimeFromRTC();    
  }
      
  void ShowTime()
  {
  int h;
  char AMPM, FLASHCOLON;
  char * DST;
  char dowchars[4];

    // updates the time on the screen

   // weekday string
   h = Weekday(CustomDate.mo,CustomDate.dy,CustomDate.yr);
   // get day name from progmem
   strcpy_P(dowchars, DaysOfWeek[h]);

    // fix hour   
    h = CustomDate.hr;
    if (h < 12)
    {
      AMPM = 'A';
      if (h==0) h=12;      
    } else {
             h=h-12;            
             if (h==0) h=12;
             AMPM = 'P';
           }
    
    // set time & date string
    
    // FLASH THE SECONDS COLON
    if (Xor1 == true)
      FLASHCOLON = ':';
    else
      FLASHCOLON = ' ';

    snprintf(buf, 30, "%s %02d/%02d/%04d %02d:%02d%c%02d %cM %s", dowchars, CustomDate.mo, CustomDate.dy, CustomDate.yr, h, CustomDate.mn, FLASHCOLON, CustomDate.sc, AMPM, DST);
      
    // update
    TV.print(SCREENWID2 - 60, SCREENHEIGHT-6, buf);
  }
  
  void Update(boolean refreshBG)
  {
    // main update is done here every minute
    switch (appScreenMode)
    {
      case SCREENMODE_APP:
        // UI ///////////////////        
        if (refreshBG == true)
          TV.clear_screen(); // clear background

        ShowTimeBig(SCREENWID2-50, SCREENHEIGHT - 19);        
        SetUpDots();  // set up scrolling dots
        break;
       
      case SCREENMODE_CLOCK:
        // big clock (from menu)
        if (refreshBG == true)
          TV.clear_screen(); // clear background
          
        ShowTimeBig(SCREENWID2-50, SCREENHEI2-4);
        break;
      
      case SCREENMODE_STARFIELD:
        // handled in main loop
        break;          
    }      
  }

  void AppTimer_Timer()
  {        
    switch (appScreenMode)
    {
      case SCREENMODE_APP:
        ShowTimeBig(SCREENWID2-50, SCREENHEIGHT - 19);
        break;
        
      case SCREENMODE_STARFIELD:
        break;        
        
      case SCREENMODE_CLOCK:
        ShowTimeBig(SCREENWID2-50, SCREENHEI2-4);
        break;
    }       
    
    // flip through different modes as a timed event
    if ((rotateScreens == true) && (StarFieldSpeed < 240))
    {        
      TimeCounter++;
      if (TimeCounter == rotateScreenSeconds)
      {
        TimeCounter = 0;
  
        // switch screen mode
        appScreenMode++;
        
        if (appScreenMode > SCREENMODE_STARFIELD)
          appScreenMode = 0;
                
        TVScrollUp();
        StarField_DidInit = false;  // reset starfield
        
        // update
        Update(true);       
        SetUpDots();
        
        ScreenModeTimeShowing = 0;
      }              
    } // rotatescreens
    else
      {
        // new
        TimeCounter++;
        if (TimeCounter == 30)
        {
          TimeCounter = 0;
          Update(true);
        }
      }
  }
  
  ////////////////////////////////////////////////////////////////////// MENU CODE
  void menuinit()
  {
      top.addChild(Item1);           // app
      top.addChild(Item2);           // clock
      top.addChild(Item3);           // settings
      Item3.addChild(Item31);        // set time
      Item3.addChild(Item32);        // set date
      Item3.addChild(Item33);        // set time zone
      
      // time zones      
      Item33.addChild(Item331);
      Item33.addChild(Item332);
      Item33.addChild(Item333);
      Item33.addChild(Item334);
      Item33.addChild(Item335);
      Item33.addChild(Item336);
      Item33.addChild(Item337);
      Item33.addChild(Item338);
      Item33.addChild(Item339);
      Item33.addChild(Item3310);
      Item33.addChild(Item3311);
      Item33.addChild(Item3312);
      Item33.addChild(Item3313);
      Item33.addChild(Item3314);
      Item33.addChild(Item3315);
      Item33.addChild(Item3316);
      Item33.addChild(Item3317);
      Item33.addChild(Item3318);
      Item33.addChild(Item3319);
      Item33.addChild(Item3320);
      Item33.addChild(Item3321);
      Item33.addChild(Item3322);
      Item33.addChild(Item3323);
      Item33.addChild(Item3324);
      Item33.addChild(Item3325);

      Item3.addChild(Item36);        //   Menu Item33("clear all settings"); 
      top.addChild(Item4);           //   reboot
      top.addChild(Item5);           //   about
  }

  void menudisplay()
  {
      Root.display();
  }    
  
  // button actions
  void button(int which)
  {    
    TimeCounter = 0;  // reset mode switcher
    
    switch (inScreen)
    {      
      case MENUMODE_APP:
        switch (which)
        {
          case pinBtnMENU:
            // show menu
            inScreen = MENUMODE_MENU;
            turnOffRGBLED(); // turn off rgb led
            StarField_DidInit = false;  // reset starfield
            StarFieldSpeed = 6; // reset starfield speed (only one accelerator button)
            InitMode();
            break;
    
          case pinBtnBKWD:
            switch(appScreenMode)
            {
              case SCREENMODE_APP:
                break;
                
              case SCREENMODE_STARFIELD:
                if (StarField_ViewX < 254)
                  StarField_ViewX++;
                break;
            }              
            break;
        
          case pinBtnFWD:
            switch(appScreenMode)
            {
              case SCREENMODE_APP:
                break;
                
              case SCREENMODE_STARFIELD:
                if (StarField_ViewX > 0)
                  StarField_ViewX--;
                break;
            }
            break;
          
          case pinBtnSELECT:
            switch(appScreenMode)
            {
              case SCREENMODE_APP:
                break;
                
              case SCREENMODE_STARFIELD:
                if (StarFieldSpeed < 254)
                  StarFieldSpeed++;
            }
            break;
        }
        break;
      
      case MENUMODE_MENU:
        // menu mode  
        switch (which)
        {
          case pinBtnBKWD:
            // UP
            Root.goUp();
            break;
        
          case pinBtnFWD:
            // DOWN
            Root.goDown();
            break;
          
          case pinBtnSELECT:
            // ENTER
            Root.goEnter();
            button_press_enter = true;
            break;
        
          case pinBtnMENU:
            // MENU
            Root.goBack();
            break;
        }
        break;
        
      case MENUMODE_CLOCK:
        // clock mode
        if (which == pinBtnMENU)
        {
            // return to menu
            inScreen = MENUMODE_MENU; // pressing menu should always set the inscreen to menu
            InitMode();
        }
        break;
        
      case MENUMODE_SETTIME:
        // set time mode
        switch (which)
        {
          case pinBtnFWD:
            switch (timeEditCursorPos)
            {
              case 1: // hour
                timeEditHour++;
                if (timeEditHour>23) { timeEditHour = 0; }
                break;
              
              case 2: // minute
                timeEditMinute++;
                if (timeEditMinute>59) { timeEditMinute = 0; }
                break;
  
              case 3: // second
                timeEditSecond++;
                if (timeEditSecond>59) { timeEditSecond = 0; }
                break;
            }
            ShowTimeForEditing();
            break;       

          case pinBtnBKWD:
            switch (timeEditCursorPos)
            {
              case 1: // hour
                if (timeEditHour > 0)
                  timeEditHour--;
                else
                  timeEditHour = 23;
                break;
              
              case 2: // minute
                if (timeEditMinute > 0)
                  timeEditMinute--;
                else
                  timeEditMinute = 59;
                break;
  
              case 3: // second
                if (timeEditSecond > 0)
                  timeEditSecond--;
                else
                  timeEditSecond = 59;
                break;
            }
            ShowTimeForEditing();
            break;
        
          case pinBtnSELECT:
            // advance position (hour edit to minute edit to seconds edit)
            timeEditCursorPos++;
            if (timeEditCursorPos > 3)
            {
                // set time             
                z1_SetRTCTime(timeEditHour, timeEditMinute, timeEditSecond);

                // set the date here using existing values otherwise the read gets botched
                RTCBaseYear = ((uint8_t)(CustomDate.yr/100) * 100)-1; // 18 becomes 1799 (never have year zero)
                z1_SetRTCDate((int) CustomDate.mo, (int) CustomDate.dy, (int) CustomDate.yr);
                z1_GetDateAndTimeFromRTC();
  
               // return to menu
               inScreen = MENUMODE_MENU; // pressing menu should always set the inscreen to menu
               InitMode();
            } 
            break;
        
          case pinBtnMENU:
            // return to menu
            inScreen = MENUMODE_MENU; // pressing menu should always set the inscreen to menu
            InitMode();
            break;
        } // end of settime
        break;
        
      case MENUMODE_SETDATE:
        // set date mode
        switch (which)
        {
          case pinBtnFWD:
            switch (dateEditCursorPos)
            {
              case 1:  // month
                dateEditMonth++;
                if (dateEditMonth > 12) { dateEditMonth = 1; }
                break;
              
              case 2:  // day
                dateEditDay++;
                if (dateEditDay > 31) { dateEditDay = 1; }
                break;
  
              case 3:  // year
                dateEditYear++;
                RTCBaseYearEdit = dateEditYear/100; // (2013 becomes 20, 1888 becomes 18) etc.. century #
                break;
            }
            ShowDateForEditing();
            break;       

          case pinBtnBKWD:
            switch (dateEditCursorPos)
            {
              case 1:  // month
                if (dateEditMonth > 1)
                  dateEditMonth--;
                else
                  dateEditMonth = 12;
                break;
              
              case 2:  // day
                if (dateEditDay > 1)
                  dateEditDay--;
                else
                  dateEditDay = 31;
                break;
  
              case 3:  // year
                if (dateEditYear>0) dateEditYear--;
                RTCBaseYearEdit = dateEditYear/100;
                break;
            }
            ShowDateForEditing();
            break;
        
          case pinBtnSELECT:
            // advance position (month edit to day edit to year edit
            dateEditCursorPos++;
            if (dateEditCursorPos > 3)
            {                                              
                // save new base year
                RTCBaseYear = (RTCBaseYearEdit * 100)-1; // 18 becomes 1799 (never have year zero)
                z1_SaveNewRTCBaseYear();
                
                // set date    
                z1_SetRTCDate((int) dateEditMonth, (int) dateEditDay, (int) dateEditYear);
                z1_GetDateAndTimeFromRTC();
  
               // return to menu           
               inScreen = MENUMODE_MENU; // pressing menu should always set the inscreen to menu
               InitMode();
            } 
            break;
        
          case pinBtnMENU:
            // return to menu
            inScreen = MENUMODE_MENU; // pressing menu should always set the inscreen to menu
            InitMode();
            break;            
        } // end of setdate
        break;
      
      case MENUMODE_ABOUT:
        // about mode
        if(which == pinBtnMENU)
        {
         // return to menu
         inScreen = MENUMODE_MENU; // pressing menu should always set the inscreen to menu
         InitMode();
        }        
        break;
        
      case MENUMODE_CLREEPROM:
        // clear eeprom settings
        // we don't handle buttons, we wait until it's finished clearing
        break;
    }
  }
  
  // button checking
  void menu_buttoncheck()
  {
  int i;
  unsigned long msNow;
  
    msNow = TV.millis();
    for (i=0; i<=3; i++)
    {
      if (digitalRead(but[i]) == LOW)
      {          
        // fix for 50 day rollover
        if (msNow < gBUTTON_press_time)
          gBUTTON_press_time=0;
       
        // check time since last keypress
        if ((msNow - gBUTTON_press_time) >= BUTTON_press_time)
        {                     
          butpressed[i] = 1; // set button pressed
          
          // run button command
          button(but[i]);
          // set last press time
          gBUTTON_press_time = msNow;    
    
          // reduce press time to scroll fast
          if (BUTTON_press_time > 9)
            BUTTON_press_time = BUTTON_press_time - 10;
          else
            BUTTON_press_time = 0;              
        }
      } // digitalRead(but[i]) == LOW
      else {
              if (butpressed[i] == 1)
              {
                // was pressed and now released
                BUTTON_press_time = 200; // reset press time
                butpressed[i] = 0; // reset button pressed status
              }
           }
    }    
  }

  // function checking
  void menu_functioncheck()
  {
  int bb;
  
    if (button_press_enter == true)
    {
      button_press_enter = false;
       
      if (Root.curfuncname == (const __FlashStringHelper*)CONST_APPNAME)
      {
          // back to the app     
          inScreen = MENUMODE_APP; // set app mode          
          InitMode();
      } else if (Root.curfuncname == (const __FlashStringHelper*)MENUCONST_CLOCK)      
      {
        inScreen = MENUMODE_CLOCK; // set clock mode
        InitMode();
      } else if (Root.curfuncname == (const __FlashStringHelper*)MENUCONST_SETTIME)
      {
        inScreen = MENUMODE_SETTIME; // set time mode
        InitMode();
      } else if (Root.curfuncname == (const __FlashStringHelper*)MENUCONST_SETDATE)
      {
        inScreen = MENUMODE_SETDATE; // set date mode
        InitMode();
      } else if (Root.curfuncname == (const __FlashStringHelper*)MENUCONST_REBOOT)
      {
          z1_Reboot(); // reboot the device
      }
      else if (Root.curfuncname == (const __FlashStringHelper*)MENUCONST_ABOUT)
      {
        inScreen = MENUMODE_ABOUT; // set about mode
        InitMode();
      } else if (Root.curfuncname == (const __FlashStringHelper*)MENUCONST_CLEARALLSETTINGS)
      {
         // clear eeprom
        inScreen =  MENUMODE_CLREEPROM;
        InitMode();
      } 
      else if ( (Root.getChar(0) == 'G') && (Root.getChar(1) == 'M') && (Root.getChar(2) == 'T') )
      {
        // time zone setting                             
        // format is 'GMT -XX PLACE'                             
        
        // check for 2nd digit
        if (Root.getChar(6) == ' ')                             
          // no 2nd digit, just get 1st one
          bb = Root.getChar(5) - 48;
        else
          // second digit exists, get 2nd one and add ten
          bb = (Root.getChar(5) - 48) + 10;
        
        // now the sign
        if (Root.getChar(4) == '-')
          bb = bb * -1; 
        
        // bb contains the time zone
        Time_Zone = bb;
        
        // save time zone to EEPROM adjusted by 20 to account for negative values
        z1_SaveSettingValue(EEPROM_LOC_TIMEZONEPLUS20, bb + 20);
        
        // go back to the last settings screen now
        Root.goSettings(Item3);
      }
    } // button = enter
  }

  void showMenuLabel(const prog_char * lbl)
  {   
    // top line in menu
    TV.printPGM(6, 6, lbl);
    TV.draw_line(0, 14, SCREENWIDTH-1, 14, WHITE); 
  }  
  ////////////////////////////////////////////////////////////////////// END OF MENU CODE

  // set time
  void ShowTimeForEditing()
  {
  int a = SCREENWID2 - 50;
  int b = 43;
    
    // hour
    printBigNum(a, b, (int)timeEditHour / 10);
    a = a + 13;
    printBigNum(a, b, (int)timeEditHour % 10);
    a = a + 13;
    
    TV.bitmap(a, b, bignums_COLON);
    a = a + 13;
    
    // minute
    printBigNum(a, b, (int)timeEditMinute / 10);
    a = a + 13;
    printBigNum(a, b, (int)timeEditMinute % 10);
    a = a + 13;
    
    TV.bitmap(a, b, bignums_COLON);
    a = a + 13;
    
    // second
    printBigNum(a, b, (int)timeEditSecond / 10);
    a = a + 13;
    printBigNum(a, b, (int)timeEditSecond % 10);
  }

  // set date
  void ShowDateForEditing()
  {
  int a = SCREENWID2 - 56;
  int b = 43;
  int c;
  int d;
  
    // month
    printBigNum(a, b, (int)dateEditMonth / 10);
    a = a + 13;
    printBigNum(a, b, (int)dateEditMonth % 10);
    a = a + 14;
    
    TV.printPGM(a, b+6, CONST_FWDSLASH);
    a = a + 6;
    
    // day
    printBigNum(a, b, (int)dateEditDay / 10);
    a = a + 13;
    printBigNum(a, b, (int)dateEditDay % 10);
    a = a + 14;
    
    TV.printPGM(a, b+6, CONST_FWDSLASH);
    a = a + 6;
    
    // year
    c = dateEditYear;
    d = c / 1000;
    printBigNum(a, b, d);
    a = a + 13;
    
    c = c - d * 1000;
    d = c / 100;
    printBigNum(a, b, d);
    a = a + 13;
    
    c = c - d * 100;
    d = c / 10;
    printBigNum(a, b, d);    
    a = a + 13;
    
    printBigNum(a, b, (int)c % 10);        
  }
  
  void ShowTimeBig(int x, int y)
  {
  uint8_t h;
  boolean isAM = false;
  static boolean OLDdigit1;
  boolean digit1 = false;
  uint8_t xpos;
  uint8_t ypos;  
  int a;
    
    h = CustomDate.hr;
    if (h < 12)
    {
      isAM = true;
      if (h==0) h=12;
    } else {
             h=h-12;            
             if (h==0) h=12;
             isAM = false;
           } 
    digit1 = (((int) h/10)>0);
    
    if (digit1==false)
      x=x+7;
    
    // clear time display region
    // if digit count changes
    if (digit1 != OLDdigit1)      
      TV.draw_rect(0, y-10, SCREENWIDTH-1, SCREENHEIGHT-(y-10), BLACK, BLACK);

    // back up old digit value
    OLDdigit1 = digit1;
    
    // show large time
    xpos = x;
    ypos = y;    
    if (digit1 == true) 
    {
      printBigNum(xpos, ypos, (int) h/10);
      xpos = xpos + 12;
    }
    printBigNum(xpos, ypos, (int) h % 10);
    xpos = xpos + 11;
              
    TV.bitmap(xpos, ypos-1, bignums_COLON);
    
    xpos = xpos + 10;    
    printBigNum(xpos, ypos, (int) CustomDate.mn / 10);
    xpos = xpos + 12;
    printBigNum(xpos, ypos, (int) CustomDate.mn % 10);
    xpos = xpos + 11;
    
    if (Xor1 == true)
      TV.bitmap(xpos, ypos-1, bignums_COLON);
    else
      TV.draw_rect(xpos+1, ypos-1, 4, 15, BLACK, BLACK);
    
    xpos = xpos + 10;      
    printBigNum(xpos, ypos, (int) CustomDate.sc / 10);
    xpos = xpos + 12;
    printBigNum(xpos, ypos, (int) CustomDate.sc % 10);
    
    xpos = xpos + 11;    
    ypos++;
    if (isAM == true)
      TV.printPGM(xpos, ypos, CONST_AM);
    else
      TV.printPGM(xpos, ypos, CONST_PM);
           
    ypos=ypos+9;
    xpos=xpos+4;

    // weekday name
    h = Weekday(CustomDate.mo, CustomDate.dy, CustomDate.yr);
    TV.printPGM(x, y-10, DaysOfWeek[h]); // get day name from progmem

    // date
    int n = snprintf(buf, 30, "%02d/%02d/%04d", CustomDate.mo, CustomDate.dy, CustomDate.yr);
    
    if (digit1==true)
      a = x + 61;
    else
      a = x + 49;
      
    TV.print(a, y-10, buf);
  }
 
  void printBigNum(int x, int y, uint8_t theNum)
  {
    TV.bitmap(x, y, BigNums[theNum]);
  }

/////////////////////// starfield code
  void drawStarField(boolean ShowData)
  {
  int i;
  
    // handle initialization
    if (StarField_DidInit == false)
    {
      StarField_Init();
      StarField_DidInit = true; // set initialized
    }

    StarField_MoveStars();
        
    // clear the starfield area
    TV.draw_rect(3, 4, SCREENWIDTH-10, SCREENHEIGHT-18 , BLACK, BLACK);

    // show data on starfield
    if (ShowData == true)
      ShowStarFieldData();
    
    for (i=0; i<CONST_NUMStars; i++)
    {    
      if ( (StarField_star2d[i].X > 3) && (StarField_star2d[i].Y > 4) )
      {
        if ( (StarField_star2d[i].X < SCREENWIDTH-8) && (StarField_star2d[i].Y < SCREENHEIGHT-18) )
        {
            // little star
            if (( i % 25) == 0)
            {
              TV.draw_line(StarField_star2d[i].X, StarField_star2d[i].Y, StarField_star2d[i].X+3, StarField_star2d[i].Y, WHITE);
              TV.draw_line(StarField_star2d[i].X+1, StarField_star2d[i].Y-1, StarField_star2d[i].X+1, StarField_star2d[i].Y+1, WHITE);
            } else
              {
                // single dot
                TV.set_pixel(StarField_star2d[i].X, StarField_star2d[i].Y, WHITE);
              }
        }
      }
    }   
  }

  void StarField_Init()
  {
  int i;

    for (i=0; i<CONST_NUMStars; i++)
      StarField_InitStar(i);
    
    // center view origin
    StarField_ViewX = SCREENWID2;
    StarField_ViewY = (SCREENHEIGHT-13) / 2;
    
    TV.clear_screen();
  }

  void StarField_InitStar(int i)
  {
    // make random 3D position
    StarField_star[i].X = random(SCREENWID2 - (SCREENWID2/3), SCREENWID2 + (SCREENWID2/3));
    StarField_star[i].Y = random(SCREENHEI2 - (SCREENHEI2/3), SCREENHEI2 + (SCREENHEI2/3));
    StarField_star[i].Z = random(2, 40); // min & max star distance
  }
  
  // move the stars inwards
  void StarField_MoveStars()
  {
  int i;
  
    for (i=0; i<CONST_NUMStars; i++)
    {  
      StarField_star[i].Z = StarField_star[i].Z - StarFieldSpeed; // speed the stars and camera move at

      // is star close?
      if (StarField_star[i].Z < 20)
        StarField_InitStar(i);  // reset star position

      // 3D Conversion
      StarField_star2d[i].X = StarField_ViewX - (256 * (SCREENWID2 - StarField_star[i].X)) / StarField_star[i].Z; // 60 = screen width / 2
      StarField_star2d[i].Y = StarField_ViewY - (256 * (((SCREENHEIGHT-13)/2) - StarField_star[i].Y)) / StarField_star[i].Z; // 48 = screen height / 2
    }  
  }
  
  void ShowStarFieldData()
  {
  static int lastA;
  static int lastB;
  static float MS, lastMS;
  int a, b, c, h;  
  char AMPM, FLASHCOLON;

    // draw navigation orbs    
    a = random(-3,3);
    b = random(-3,3);
    c = random(-1,1);
    lastA = (lastA + a)/2;
    lastB = (lastB + b)/2;    
    TV.draw_circle(SCREENWID2, SCREENHEI2, 5, WHITE, -1);
    TV.draw_circle(SCREENWID2+lastA, SCREENHEI2+lastB, 15+c, WHITE, -1);
    
    // draw X Y Z axis
    TV.draw_line(2, SCREENHEIGHT-13, SCREENWIDTH-8, SCREENHEIGHT-13, WHITE); // to lower right (X axis)
    TV.printPGM(SCREENWIDTH-6, SCREENHEIGHT-15 , (const prog_char *) F("X"));

    TV.draw_line(2, 0, 2, SCREENHEIGHT-13, WHITE); // to middle top (Y axis)
    TV.printPGM(1, 0, (const prog_char *) F("Y"));

    TV.draw_line(2, SCREENHEIGHT-13, SCREENWID2, SCREENHEI2, WHITE); // to top right (Z axis)
    TV.printPGM(SCREENWID2-1, SCREENHEI2-2, (const prog_char *) F("Z"));

    // show time & date
    if (Xor1 == true)
      FLASHCOLON = ':';
    else
      FLASHCOLON = ' ';

    h = CustomDate.hr;
    if (h < 12)
    {
      AMPM = 'A';
      if (h==0) h=12;      
    } else {
             h=h-12;            
             if (h==0) h=12;
             AMPM = 'P';
           }  
    snprintf(buf, 30, "%02d:%02d%c%02d %cM %02d/%02d/%04d ", h, CustomDate.mn, FLASHCOLON, CustomDate.sc, AMPM, CustomDate.mo, CustomDate.dy, CustomDate.yr);
    
    TV.print(0,SCREENHEIGHT - 7, buf);
    
    // tray of notifications (updates every 500ms)
    MS = millis();
    if (MS-lastMS > 500)
    {
      // show flashing object (no real purpose here but could be activated by a sensor)
      if (Xor1 == true)
          TV.printPGM(SCREENWIDTH - 11, SCREENHEIGHT - 7, (const prog_char *)F("*"));
        else
          TV.draw_rect(SCREENWIDTH - 11, SCREENHEIGHT - 7, 8, 8, BLACK, BLACK);
      
      lastMS = MS;
    }       
  }
  // end of starfield code

// dots /////////////////////////////////////////////////////////////////////////
void SetUpDots()
{
uint8_t i;
  
  // create a sane pixel map to start
  for (i=0; i<CONST_NUMStars; i++)
  {
    while (true)
    {
      StarField_star[i].X = random(3, SCREENWIDTH-1);
      StarField_star[i].Y = random(1, SCREENHEIGHT-2);

      // we have a valid pixel (no matches to other pixels)      
      if (DotAtThisLocation(i) == false)
        break;
    }

    // get current pixel value at new location
    StarField_star[i].Z = TV.get_pixel(StarField_star[i].X, StarField_star[i].Y);      
  }
}

void inline SetNewDotPos(int i)
{
  // set new position & search for duplicate
  StarField_star[i].X = SCREENWIDTH-1;
  while (true)
  {    
    // random y
    StarField_star[i].Y = random(1, SCREENHEIGHT-1);

    if (DotAtThisLocation(i) == false)
      break; 
  }
}

boolean DotAtThisLocation(uint8_t i)
{
boolean found;
uint8_t z;

  // search for dupe
  found = false;
  for (z=0; z<CONST_NUMStars; z++)
  {
    if (z != i)
    {
      if (StarField_star[z].X == StarField_star[i].X)
      {
        if (StarField_star[z].Y == StarField_star[i].Y)
        {
          // dupe
          found = true;
          break;
        }
      }
    }
  }  
  return (found);
}

void MoveDotsLeft()
{
uint8_t i;

  // only process if the array was initialized (0 is not allowed)
  if (StarField_star[0].X == 0)
    return;
      
  for (i=0; i<CONST_NUMStars; i++)
  {
    // restore old pixel
    TV.set_pixel(StarField_star[i].X, StarField_star[i].Y, StarField_star[i].Z);
    
    if (StarField_star[i].X <= 2)
      SetNewDotPos(i);
    else
    {
      // now scroll this dot at two velocities
      if ((StarField_star[i].Y & 1) == 0) // odd by 1 pixel
        StarField_star[i].X--;
      else
        StarField_star[i].X-=2; // even by 2 pixels     
  
      if (DotAtThisLocation(i) == true)
        SetNewDotPos(i);
    }

    // get pixel value (it will be the 'old' value when scrolling)
    StarField_star[i].Z = TV.get_pixel(StarField_star[i].X, StarField_star[i].Y);
  
    // draw the dot
    TV.set_pixel(StarField_star[i].X, StarField_star[i].Y, WHITE);
  }
}
// end of dots code ///////////////////////////////

void turnOffRGBLED()
{
  analogWrite(pinLEDR, 0);
  analogWrite(pinLEDG, 0);
  analogWrite(pinLEDB, 0);
  
  for (int i=0;i<3;i++)
  {
    RGBLED[i].val=0;
    RGBLED[i].dir=0;
  }
}

void SwirlRGBLED()
{
int b;
uint8_t a;

  // swirl RGB LED
    
  // green + red
  for (int i=0; i<3; i++)
  {
    if (RGBLED[i].dir==0)
      b = RGBLED[i].val + random(1,2);
    else
      b = RGBLED[i].val - random(1,2);
  
    if (b<1)
    {
      RGBLED[i].dir = 0;
      b = 1;
    }
    else
    {
      if (b>128)
      {
        RGBLED[i].dir = 1;
        b = 128;
      }
    }
    
    RGBLED[i].val = b;
  }
   
  analogWrite(pinLEDR, RGBLED[0].val / 8); // red
  analogWrite(pinLEDG, RGBLED[1].val / 8); // green
  analogWrite(pinLEDB, RGBLED[2].val / 8); // blue
}

void TVScrollUp()
{
    // scroll up
    TV.draw_line(0, SCREENHEIGHT - 1, SCREENWIDTH-1, SCREENHEIGHT - 1, WHITE); // for effect
    for (int i=0; i < SCREENWIDTH/4; i++)
    {
      TV.shift(4, 0); // shift(uint8_t distance, uint8_t direction) 
      if ((i % 3)==0)
        TV.delay(1);
    }
}

void customSleep()
{
  TVScrollUp(); // effect
  
  // disable hardware
  z1_StopRTC();
  TV.end();
  
  // turn off LED
  turnOffRGBLED();
  
  // delay for debounce
  delay(1000);
  
  // put the z1 to sleep (ultra-low power mode)
  z1_Sleep();   
   
  // back from sleep
   
  // re-enable hardware
  
  TV.clear_screen();
  TV.begin(NTSC, SCREENWIDTH, SCREENHEIGHT);  
  TV.delay(1000);

  // GET time from RTC
  z1_GetDateAndTimeFromRTC();
  
  // reset variables for initial display
  inScreen = MENUMODE_APP;
  appScreenMode = 0;
  TimeCounter = 0;
  
  TVScrollUp(); // effect
  
  // init UI
  InitMode();
}
// end of z1.ino
