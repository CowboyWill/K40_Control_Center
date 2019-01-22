/****************************************************************************

  File: K40 Control Center.ino 

  Program: Chinese Co2 Laser Control Center
 
  Author:
    Will Travis
 
  Date:
    06-25-2018
 
  Version: 
    0.17
  
  See Also:
    See instruction file for hookup information.
  
  Description:
    When modifying my K40, I wanted to have a display with information on 
    different aspects of the K40, so I decided to use the Nextion display.
    It works great with an Arduino that can be used read many sensors
    of data.  This program displays that information and allows interaction
    with buttons and other items...

  Docs:
    Documentation created by NaturalDocs.org.  Documentation style follows
    Arduino standard, https://www.arduino.cc/en/Reference/StyleGuide:

    - Variable names start with a lower-case letter, use uppercase letters as separators and do not use underbars ('_').
    - Function names start with a lower-case letter, use uppercase letters as separators and do not use underbars ('_').
    - Constant names use all capital letters and use underbars ('_') as separators.
    - Using byte, int, unsigned int, etc. vs. int8_t, uint8_t, etc.
    - Avoiding the use of defines as much as possible.
    - Using const instead of define.
 *****************************************************************************/

/****************************************************************************
 * Files: Include files
 *   K40_Control_Center.h - main configuration file
 *   Nextion.h            - Nextion display
 *   FlowMeter            - water flow library
 ****************************************************************************/
#include "K40_Control_Center.h"
#include "Nextion.h"
#include <FlowMeter.h>      // https://github.com/sekdiy/FlowMeter

/****************************************************************************
 * Variables: Global Variables
 *   prev_fault          - Used to display faults only when changing
 *   currentFlowAni      - used to animate flow graphics
 *   celcius             - Celcius = 0, Fahrenheit = 1
 *   caseADC             - used to average Analog to Digital temp readings
 *   waterADC            - used to average Analog to Digital temp readings
 *   tempCount           - used to count readings for NUM_SAMPLES times
 *   last_read_temp      - stores last time thermistor changed
 *   doorOpen            - true if the door is open, false = door is closed
 *   keySwitchOpen       - true if key switch is open, false if key closed
 *   flowHighAlarm       - true if water flow too HIGH
 *   flowLowAlarm        - true if water flow too LOW
 *   waterHighAlarm      - true if water temp too HIGH
 *   waterLowAlarm       - true if water temp too LOW
 *   caseHighAlarm       - true if case temp too HIGH
 *   caseLowAlarm        - true if case temp too LOW
 *   peltierOn           - indicates if peltier is ON
 *   Meter               - Flow Meter class object
 *   last_read_flow      - stores last time water flow changed
 *   last_flow_ani       - stores last time flow animation changed
 *   last_display_update - stores last time display updated
 *   
 ****************************************************************************/
int prev_fault = 1;

byte currentFlowAni = 0;

uint32_t celcius;
unsigned int caseADC = 0;
unsigned int waterADC = 0;
byte tempCount = 0;

unsigned long last_read_temp = millis() - UPDATE_TEMP_DELAY;

byte doorOpen = false;
byte keySwitchOpen = false;
byte flowHighAlarm = false;
byte flowLowAlarm = false;
byte waterHighAlarm = false;
byte waterLowAlarm = false;
byte caseHighAlarm = false;
byte caseLowAlarm = false;
byte peltierOn = false;

FlowMeter Meter = FlowMeter(WATER_FLOW_PIN);
unsigned long last_read_flow = millis() - UPDATE_FLOW_DELAY;
unsigned long last_flow_ani = millis() - FLOW_ANI_DELAY;
unsigned long last_display_update = millis() - DISPLAY_UPDATE_DELAY;

/*****************************************************************************
 * Variables: Nextion components for home page
 * selfcheck_page - Displayed on startup, checks if sensors working
 * celcius_va     - Celcius vs Fahrenheit
 * home           - Home page
 * lights_btn     - Button to turn interior lights on and off
 * assist_btn     - Button to turn air assist on and off
 * pointer_btn    - Button to turn laser pointer on and off
 * exhaust_btn    - Button to turn exhaust fan on and off
 * status_txt     - Text line at bottom of display used to display error MESSAGES
 * water_temp     - Displays water temp (string to display floating point number)
 * case_temp_num  - Displays case temp (integer number)
 ****************************************************************************/
NexPage selfcheck_page = NexPage(0, 0, "selfcheck");
NexVariable celcius_va = NexVariable(0, 2, "vacf");

NexPage home              = NexPage(1, 0, "home");
NexScrolltext status_txt    = NexScrolltext(1, 4, "status_txt");
NexGauge water_gauge        = NexGauge(1, 6, "water_gauge");
NexText water_temp          = NexText(1, 7, "water_temp");
NexDSButton lights_btn      = NexDSButton(1, 9, "lights_btn");
NexNumber case_temp_num     = NexNumber(1, 13, "case_temp_num");
NexDSButton assist_btn      = NexDSButton(1, 14, "assist_btn");
NexDSButton exhaust_btn     = NexDSButton(1, 15, "exhaust_btn");
NexDSButton pointer_btn     = NexDSButton(1, 16, "pointer_btn");
NexPicture peltier_pic      = NexPicture(1, 17, "peltier_pic");
NexNumber flow_num          = NexNumber(1, 19, "flow_num");
NexPicture flow_ani         = NexPicture(1, 20, "flow_ani");
NexNumber ps_num            = NexNumber(1, 25, "ps_num");

// Variable: nex_listen_list
//   List of all Nextion objects that will return a value, e.g. buttons
NexTouch *nex_listen_list[] = {
  &lights_btn,
  &assist_btn,
  &pointer_btn,
  &exhaust_btn,
  NULL
};

/*****************************************************************************
 * Function: MeterISR
 * define an 'interrupt service handler' (ISR) for Flow Meter interrupt pin
 *****************************************************************************/
void MeterISR() {
  Meter.count();  // count the pulses every time the flow meters triggers an interrupt
}

/*****************************************************************************
 * Function: display_status  
 * Displays message on status line at bottom of Nextion display
 *
 * Argument:
 *   fault - number of message to display from MESSAGES array
 *****************************************************************************/
void display_status(int fault) {
  // 0 = No fault, scroll text
  if (fault == 0) {
    status_txt.Set_scroll_dir(1);       // scroll right to left
    status_txt.Set_scroll_distance(10); // scroll speed
  }else{
    status_txt.Set_scroll_dir(2);       // scroll up to down
    status_txt.Set_scroll_distance(40); // this causes it to flash (height of box)
  }
  status_txt.setText(MESSAGES[fault]);
}

/*****************************************************************************
 * Function: selfCheck
 * 
 * Checks if all systems are running.  
 * Checking off each system on the Nextion display.
 * Will only test systems that do not have Bypass set to true.
 * When complete, will display home page.
 *****************************************************************************/
void selfCheck() { 
  dbSerialPrintln("START Self Check");
  // Read temp and throw away first set of readings
  while (!readThermistors());

  delay(2000); // wait  before moving to home page
  Meter.tick(2000);  // Read flow and throw away first set of readings
  home.show();
  delay(10);

  // turn off Peltier if not permitted
  //  NOT WORKING @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  if (!PERMIT_PELTIER) sendCommand("peltier_pic.vis,0");
  // turn off Case Temp if not permitted
  if (!PERMIT_CASE_TEMP) {
    sendCommand("case_txt.vis,0");
    sendCommand("case_temp_num.vis,0");
    sendCommand("case_cf.vis,0");
  }
  
  dbSerialPrintln("END Self Check");
}

/*****************************************************************************
 * Function: lights_btnPopCallback
 * Turn the LED lights on or off based on the value of the lights button. 
 *****************************************************************************/
void lights_btnPopCallback(void *ptr) {
  uint32_t dual_state;
  // Get the state value of dual state button component.
  lights_btn.getValue(&dual_state);
  digitalWrite(LIGHTS_PIN, dual_state); // Turn on lights
}
/*****************************************************************************
 * Function: assist_btnPopCallback
 * Turn the air assist on or off based on the value of the dual state button.
 *****************************************************************************/
void assist_btnPopCallback(void *ptr)
{
  uint32_t dual_state;
  // Get the state value of dual state button component.
  assist_btn.getValue(&dual_state);
  digitalWrite(ASSIST_PIN, dual_state);  // Turn on or off air assist
}
/*****************************************************************************
 * Function: pointer_btnPopCallback
 * Turn the laser pointer on or off based on the value of the laser button.
 *****************************************************************************/
void pointer_btnPopCallback(void *ptr)
{
  uint32_t dual_state;
  // Get the state value of dual state button component.
  pointer_btn.getValue(&dual_state);
  digitalWrite(POINTER_PIN, dual_state);  // Turn on or off laster pointer
}

/*****************************************************************************
 * Function: exhaust_btnPopCallback
 * Turn the exhaust fan on or off based on the value of the exhaust button.
 *****************************************************************************/ 
void exhaust_btnPopCallback(void *ptr)
{
  uint32_t dual_state;
  // Get the state value of dual state button component.
  exhaust_btn.getValue(&dual_state);
  digitalWrite(EXHAUST_PIN, dual_state);  // Turn on or off exhaust fan
}


/*****************************************************************************
 * Function: readSettings  
 * Reads settings from Nextion 
 *****************************************************************************/
void readSettings() {
  celcius_va.getValue(&celcius);
}

/*****************************************************************************
 * Function: readThermistors
 *   Read a values from both water and case thermistors, 
 *   if it has read multiple values up to NUM_SAMPLES of times then 
 *   calculate average in global variables waterADC and caseADC
 * 
 * Returns:
 *   true if temperature is read NUM_SAMPLES times
 *   false if not all samples have been read
 * 
 * Global Variables:
 *   waterADC       - running total of raw temp reads from water thermistor
 *   caseADC        - running total of raw temp reads from case thermistor
 *   WATER_TEMP_PIN - Arduino pin of water thermistor
 *   CASE_TEMP_PIN  - Arduino pin of case thermistor
 *   tempCount      - count of temperature reading taken, this is compared
 *      to the NUM_SAMPLES before an average is taken
 *****************************************************************************/
bool readThermistors() {
  if (PERMIT_WATER_TEMP) waterADC += analogRead(WATER_TEMP_PIN);
  if (PERMIT_CASE_TEMP)  caseADC += analogRead(CASE_TEMP_PIN);
  tempCount++;
  if (tempCount == NUM_SAMPLES)  {
    tempCount = 0;
    if (PERMIT_WATER_TEMP) waterADC /= NUM_SAMPLES; // get average
    if (PERMIT_CASE_TEMP)  caseADC /= NUM_SAMPLES;  // get average
    return true;
  }else{
    return false;
  }
}

/*****************************************************************************
 * Function: convertToTemp
 *  Converts input from a thermistor analog to Kelvin temperature value.
 * 
 * Parameter:
 *   adc - raw value from analog pin
 * 
 * Return:
 *   temperature - in Kelvin
 * 
 * Global Variables Needed:
 *     INVT0    - ?
 *     INVBETA  - inverse of the thermistor Beta value supplied by manufacturer
 *****************************************************************************/
float convertToTemp(float adc) {
  return (1.00 / (INVT0 + INVBETA * (log(1023.00 / adc - 1.00))));
}

/*****************************************************************************
 * Function: convertKtoCF
 *  Converts convert Kelvin to Celcuis or Fahrenheit
 * 
 * Parameter:
 *   kelvin - kelvin temp
 * 
 * Return:
 *   temperature in Celcius or Fahrenheit (depending on global variable celcius)
 * 
 * Global Variables Needed:
 *     celcius  - true if Celcius, false if Fahrenheit
 *****************************************************************************/
float convertKtoCF(float kelvin) {
  if (celcius == 0) {   // 0=Celsius, 1=Fahrenheit
    return kelvin - 273.15; // convert to Celsius
  }else{
    return (kelvin - 273.15) * 1.8 + 32.0; // convert to Fahrenheit
  }
}

/*****************************************************************************
 * Function: setup
 *  Sets the arduino pins, 
 *  Initializes the Nextion display, 
 *  Runs the Self Check routing.
 *****************************************************************************/
void setup(void) {
  ///////////////////////////// PINS /////////////////////////////////////////
  // Input pins
  pinMode(KEY_PIN, INPUT_PULLUP);
  pinMode(DOOR_PIN, INPUT_PULLUP);
  pinMode(WATER_FLOW_PIN, INPUT_PULLUP);
    
  // Output pins
  pinMode(LIGHTS_PIN, OUTPUT);
  digitalWrite(LIGHTS_PIN, LOW);
  pinMode(INTERLOCK_PIN, OUTPUT);
  digitalWrite(INTERLOCK_PIN, LOW);
  pinMode(ASSIST_PIN, OUTPUT);
  digitalWrite(ASSIST_PIN, LOW);
  pinMode(EXHAUST_PIN, OUTPUT);
  digitalWrite(EXHAUST_PIN, LOW);
  pinMode(POINTER_PIN, OUTPUT);
  digitalWrite(POINTER_PIN, LOW);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  pinMode(PELTIER_PIN, OUTPUT);
  digitalWrite(PELTIER_PIN, LOW);

  ////////////////////////////// SETUP NEXTION ///////////////////////////////
  delay(20); // allow nextion to initialize
  nexInit();
  delay(10); // allow nextion to initialize

  // Register pop callback function for each button component
  lights_btn.attachPop(lights_btnPopCallback, &lights_btn);
  assist_btn.attachPop(assist_btnPopCallback, &assist_btn);
  pointer_btn.attachPop(pointer_btnPopCallback, &pointer_btn);
  exhaust_btn.attachPop(exhaust_btnPopCallback, &exhaust_btn);

  ////////////////////////////// FLOW METER //////////////////////////////////
  // enable a call to the 'interrupt service handler' (ISR) on every rising edge at the interrupt pin
  attachInterrupt(FLOW_INTERRUPT, MeterISR, RISING);
  Meter.reset();   // reset meter variables to 0

  ////////////// Run a self check of the sytem, page 0 of Nextion ////////////
  selfCheck();

  dbSerialPrintln("setup done");
}

void loop() {
  /////////////// Check each object to see if any were triggered /////////////
  nexLoop(nex_listen_list);

  /* current_fault:
      0=ok, 1=interlock, 2=key off, 
      3=flow rate upper limit, 4=flow rate lower limit
      5=water temp upper limit, 6=water temp lower limit, 
      7=case temp upper limit, 8=case temp lower limit
  */
  int current_fault = 0; // Set to no faults detected, 0=ok

  //////////////////// Check interlocks if set to true ///////////////////////
  if (PERMIT_INTERLOCK) {
    // Check if interlock switch is open
    if (digitalRead(DOOR_PIN) == DOOR_OPEN) {
      doorOpen = true; //interlock(s) opened
    }else{
      doorOpen = false;
    }
  }
  ///////////////////// Check key switch if set to true //////////////////////
  if (PERMIT_KEY) {
    // Check if key switch is locked
    if (digitalRead(KEY_PIN) == LOCKED) {
      keySwitchOpen = true; //key not turned on
    }else{
      keySwitchOpen = false;
    }
  }
  /////////////////////////// Read Temperatures //////////////////////////////
  if (PERMIT_WATER_TEMP || PERMIT_CASE_TEMP) {
    // Read termistor(s) every UPDATE_TEMP_DELAY ms
    if ((millis() - last_read_temp) > UPDATE_TEMP_DELAY) {
      // read termistors reading every NUM_SAMPLES times, if true convert to temp
      if (readThermistors()) {
        if (PERMIT_WATER_TEMP) {
          float Temp = convertToTemp(waterADC);

          (Temp > WATER_TEMP_UPPER_LIMIT) ? waterHighAlarm = true : waterHighAlarm = false;
          (Temp < WATER_TEMP_LOWER_LIMIT) ? waterLowAlarm = true : waterLowAlarm = false;

          // Update Peltier on or off
          if (PERMIT_PELTIER) {
            // Check if need to turn Peltier on or off
            if (Temp >= WATER_TEMP_UPPER_PELTIER) {
              if (!peltierOn) {
                digitalWrite(PELTIER_PIN, HIGH); // Turn on Peltier 
                peltier_pic.setPic(PELTIER_ON); // show Peltier ON picture
                peltierOn = true;
              }
            }
            if (Temp <= WATER_TEMP_LOWER_PELTIER) {
              if (peltierOn) {
                digitalWrite(PELTIER_PIN, LOW); // Turn off Peltier 
                peltier_pic.setPic(PELTIER_OFF);// show Peltier OFF picture
                peltierOn = false;
              }
            }
          }

          ////////////////////// Update Water Gauge //////////////////////////
          // Gauge has a range of -31(329) to 211
          // 0 = straight left,  -1 = 359, 90 = straight up
          // Good range = 50deg, Hot Range = 130
          // 80 degrees between each range (-31 to 211 = 240)
          // Map command converts one range of numbers to another
          //    map(value, fromLow, fromHigh, toLow, toHigh)
          int gauge_val = map(Temp, 
                              WATER_TEMP_LOWER_LIMIT-WATER_TEMP_LIMIT_DIFF, 
                              WATER_TEMP_UPPER_LIMIT+WATER_TEMP_LIMIT_DIFF,
                              -31, 211);
          // Check if value a negative number, if so 360 - value (360+negative value)
          if (gauge_val < 0) {
            gauge_val+=360;
          }
          dbSerialPrintln("Temp="+String(Temp)+"  gauge_val=" + String(gauge_val));
          // water_gauge.setValue(gauge_val);
          water_gauge.setValue(gauge_val);

          // Display water temp value on display
          Temp = convertKtoCF(Temp);
          water_temp.setFloatText(Temp, 1);
        }

        if (PERMIT_CASE_TEMP) {
          float Temp = convertToTemp(caseADC);

          (Temp > CASE_TEMP_UPPER_LIMIT) ? caseHighAlarm = true : caseHighAlarm = false;
          (Temp < CASE_TEMP_LOWER_LIMIT) ? caseLowAlarm = true : caseLowAlarm = false;
          
          Temp = convertKtoCF(Temp); // Display case temp value on display
          case_temp_num.setValue((int)Temp);
        }
      }
    last_read_temp = millis();
    }
  }

  /////////////////////////// Read Water Flow //////////////////////////////
  if (PERMIT_FLOW) {
    // Read flow termistor(s) every UPDATE_TEMP_DELAY ms
    if ((millis() - last_read_flow) > UPDATE_FLOW_DELAY) {
      // process the (possibly) counted ticks
      Meter.tick(UPDATE_FLOW_DELAY);

      // output some measurement result
      dbSerialPrintln("Currently " + String(Meter.getCurrentFlowrate()) + " l/min, " + String(Meter.getTotalVolume())+ " l total.");

      // Update nextion display
      float flowRate = Meter.getCurrentFlowrate();
      (flowRate > FLOW_RATE_UPPER_LIMIT) ? flowHighAlarm = true : flowHighAlarm = false;
      (flowRate < FLOW_RATE_LOWER_LIMIT) ? flowLowAlarm = true : flowLowAlarm = false;

      //display the flow rate number (integer)
      flow_num.setValue((int)flowRate);

      last_read_flow = millis();
    }

    // Display the flow animation
    if ( (millis() - last_flow_ani) > FLOW_ANI_DELAY) {
      // Only animate if flowRate > 0
      if (Meter.getCurrentFlowrate() > 0.0) { 
        flow_ani.setPic(currentFlowAni+ANI_START_PIC);
        currentFlowAni++;
        if (currentFlowAni == ANI_PICS) currentFlowAni = 0;
      }
      last_flow_ani = millis();
    }

  }

  /////////////////////// Display rest of items //////////////////////////////
  if ( (millis() - last_display_update) > DISPLAY_UPDATE_DELAY) {
    // Update Power percentage from Power potentometer
    int powerVal = analogRead(POWER_PIN);  // read the power pot value
    dbSerialPrint("Power =" + String(powerVal));
    powerVal = map(powerVal, 0, 1023, 0, 100);  // convert analog value to %
    dbSerialPrintln("  Percentage=" + String(powerVal));
    ps_num.setValue(powerVal);

    // Update Power Current
    // ...
    last_display_update = millis();
  }

  // check if error condition and set fault indicator
  if (doorOpen)       current_fault = 1; //interlock opened
  if (keySwitchOpen)  current_fault = 2; //key not turned on
  if (flowHighAlarm)  current_fault = 3; //flow too high
  if (flowLowAlarm)   current_fault = 4; //flow too low
  if (waterHighAlarm) current_fault = 5; //water temp too high
  if (waterLowAlarm)  current_fault = 6; //water temp too low

  /////////////// Update status text only if status changed //////////////////
  if (current_fault != prev_fault) {
    //enable or disable the interlock output on any error
    if (current_fault != 0) {
      digitalWrite(INTERLOCK_PIN, LOW); // Turn off interlock relay (disabling laser)
    }else{
      digitalWrite(INTERLOCK_PIN, HIGH); // Turn on interlock relay (enabling laser)
    }

    // Sound buzzer for critial errors
    if (current_fault > 2) {
      digitalWrite(BUZZER_PIN, ALARM_ON); // turn alarm on
    }else{
      digitalWrite(BUZZER_PIN, ALARM_OFF); // turn off alarm
    }

    // Update status text display
    display_status(current_fault);
    prev_fault = current_fault;
  }
}

/* 
  dbSerialPrint("keySwitchOpen=");  // DEBUG 
  dbSerialPrintln(keySwitchOpen);   // DEBUG
          or
  dbSerialPrintln("keySwitchOpen=" + String(keySwitchOpen));   // DEBUG
*/

/*  I T E M S  T O  F I X 
  Turn off visibility for objects not used
*/