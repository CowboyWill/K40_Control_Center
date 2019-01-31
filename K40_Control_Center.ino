/****************************************************************************

  File: K40 Control Center.ino 

  Program: Chinese Co2 Laser Control Center
 
  Author:
    Will Travis
 
  Date:
    01/27/2019
 
  Version: 
    0.18
  
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
 *   prevFault         - Used to display faults only when changing
 *   currentFlowAni    - used to animate flow graphics
 *   fahrenheit        - Celcius = 0, Fahrenheit = 1
 *   caseADC           - used to average Analog to Digital temp readings
 *   waterADC          - used to average Analog to Digital temp readings
 *   tempCount         - used to count readings for NUM_SAMPLES times
 *   lastReadTemp      - stores last time thermistor changed
 *   doorOpen          - true if the door is open, false = door is closed
 *   keySwitchOpen     - true if key switch is open, false if key closed
 *   flowHighAlarm     - true if water flow too HIGH
 *   flowLowAlarm      - true if water flow too LOW
 *   waterHighAlarm    - true if water temp too HIGH
 *   waterLowAlarm     - true if water temp too LOW
 *   caseHighAlarm     - true if case temp too HIGH
 *   caseLowAlarm      - true if case temp too LOW
 *   peltierOn         - indicates if peltier is ON
 *   Meter             - Flow Meter class object
 *   lastReadFlow      - stores last time water flow changed
 *   lastFlowAni       - stores last time flow animation changed
 *   lastDisplayUpdate - stores last time display updated
 *   laserOn           - true = laser turned on, false = laser turned off
 *   airAssistAuto     - true = air assist set to auto, false = AA set to manual
 ****************************************************************************/
int prevFault = 1;

byte currentFlowAni = 0;

uint32_t fahrenheit;
unsigned int caseADC = 0;
unsigned int waterADC = 0;
byte tempCount = 0;

unsigned long lastReadTemp = millis() - UPDATE_TEMP_DELAY;

byte doorOpen = false;
byte keySwitchOpen = false;
byte flowHighAlarm = false;
byte flowLowAlarm = false;
byte waterHighAlarm = false;
byte waterLowAlarm = false;
byte caseHighAlarm = false;
byte caseLowAlarm = false;
byte peltierOn = false;
byte laserOn = false;
uint32_t airAssistAuto = false;

FlowMeter Meter = FlowMeter(WATER_FLOW_PIN);
unsigned long lastReadFlow = millis() - UPDATE_FLOW_DELAY;
unsigned long lastFlowAni = millis() - FLOW_ANI_DELAY;
unsigned long lastDisplayUpdate = millis() - DISPLAY_UPDATE_DELAY;

/*****************************************************************************
 * Variables: Nextion components for homePage page
 * selfcheckPage - Displayed on startup, checks if sensors working
 * celciusVar    - Celcius vs Fahrenheit
 * airAssistVar  - Air Assist Auto (0) or Manual (1)
 * homePage      - home page
 * statusTxt     - Text line at bottom of display used to display error MESSAGES
 * waterGauge    - Water Temp Gauge
 * waterTemp     - Displays water temp (string to display floating point number)
 * lightsBtn     - Button to turn interior lights on and off
 * caseTempNum   - Displays case temp (integer number)
 * assistBtn     - Button to turn air assist on and off
 * exhaustBtn    - Button to turn exhaust fan on and off
 * pointerBtn    - Button to turn laser pointer on and off
 * peltierPic    - Peltier picture number (on or off)
 * flowNum       - Water Flow number
 * flowAni       - Water Flow Animation picture number
 * psNum         - Power Setting number
 * poNum         - Power Output number
 ****************************************************************************/
NexPage homePage        = NexPage(1, 0, "home");
NexVariable celciusVar     = NexVariable(1, 28, "vacf");
NexVariable airAssistVar   = NexVariable(1, 31, "vaAA");
NexVariable laserHours     = NexVariable(1, 29, "vahused");
NexScrolltext statusTxt    = NexScrolltext(1, 4, "status_txt");
NexGauge waterGauge        = NexGauge(1, 6, "water_gauge");
NexText waterTemp          = NexText(1, 7, "water_temp");
NexDSButton lightsBtn      = NexDSButton(1, 9, "lights_btn");
NexNumber caseTempNum      = NexNumber(1, 13, "case_temp_num");
NexDSButton assistBtn      = NexDSButton(1, 14, "assist_btn");
NexDSButton exhaustBtn     = NexDSButton(1, 15, "exhaust_btn");
NexDSButton pointerBtn     = NexDSButton(1, 16, "pointer_btn");
NexPicture peltierPic      = NexPicture(1, 17, "peltier_pic");
NexNumber flowNum          = NexNumber(1, 19, "flow_num");
NexPicture flowAni         = NexPicture(1, 20, "flow_ani");
NexNumber psNum            = NexNumber(1, 25, "ps_num");
NexNumber poNum            = NexNumber(1, 22, "po_num");

// Variable: nex_listen_list
//   List of all Nextion objects that will return a value, e.g. buttons
NexTouch *nex_listen_list[] = {
  &lightsBtn,
  &assistBtn,
  &pointerBtn,
  &exhaustBtn,
  &celciusVar,
  &airAssistVar,
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
    statusTxt.Set_scroll_dir(1);       // scroll right to left
    statusTxt.Set_scroll_distance(10); // scroll speed
  }else{
    statusTxt.Set_scroll_dir(2);       // scroll up to down
    statusTxt.Set_scroll_distance(40); // this causes it to flash (height of box)
  }
  statusTxt.setText(MESSAGES[fault]);
}

/*****************************************************************************
 * Function: lightsBtnPopCallback
 * Turn the LED lights on or off based on the value of the lights button. 
 *****************************************************************************/
void lightsBtnPopCallback(void *ptr) {
  uint32_t dual_state;
  // Get the state value of dual state button component.
  lightsBtn.getValue(&dual_state);
  digitalWrite(LIGHTS_PIN, dual_state); // Turn on lights
}
/*****************************************************************************
 * Function: assistBtnPopCallback
 * Turn the air assist on or off based on the value of the dual state button.
 *****************************************************************************/
void assistBtnPopCallback(void *ptr)
{
  uint32_t dual_state;
  // Get the state value of dual state button component.
  assistBtn.getValue(&dual_state);
  digitalWrite(ASSIST_PIN, dual_state);  // Turn on or off air assist
}
/*****************************************************************************
 * Function: pointerBtnPopCallback
 * Turn the laser pointer on or off based on the value of the laser button.
 *****************************************************************************/
void pointerBtnPopCallback(void *ptr)
{
  uint32_t dual_state;
  // Get the state value of dual state button component.
  pointerBtn.getValue(&dual_state);
  digitalWrite(POINTER_PIN, dual_state);  // Turn on or off laster pointer
}

/*****************************************************************************
 * Function: exhaustBtnPopCallback
 * Turn the exhaust fan on or off based on the value of the exhaust button.
 *****************************************************************************/ 
void exhaustBtnPopCallback(void *ptr)
{
  uint32_t dual_state;
  // Get the state value of dual state button component.
  exhaustBtn.getValue(&dual_state);
  digitalWrite(EXHAUST_PIN, dual_state);  // Turn on or off exhaust fan
}
/*****************************************************************************
 * Function: airAssistVarPopCallback
 * Air Assist set to Auto or Manual in settings page
 *****************************************************************************/
void airAssistVarPopCallback(void *ptr) {
  // Get the state value of airAssist variable.
  airAssistVar.getValue(&airAssistAuto);
}


/*****************************************************************************
 * Function: readSettings  
 * Reads settings from Nextion 
 *****************************************************************************/
void readSettings() {
  celciusVar.getValue(&fahrenheit);
  airAssistVar.getValue(&airAssistAuto);
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
 *   temperature in Celcius or Fahrenheit (depending on global variable fahrenheit)
 * 
 * Global Variables Needed:
 *     fahrenheit  - true (1) if Fahrenheit, false (0) if Celcius
 *****************************************************************************/
float convertKtoCF(float kelvin) {
  if (fahrenheit) {   // 0=Celsius, 1=Fahrenheit
    return (kelvin - 273.15) * 1.8 + 32.0; // convert to Fahrenheit
  }else{
    return kelvin - 273.15; // convert to Celsius
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
  ////////////// Initialize variables ////////////
  // Read initial settings from Nextion variables
  readSettings();
  
  ////////////// Set Nextion global variables ////////////
  // turn on or off Peltier icon variable
  //  NOT WORKING @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  if (PERMIT_PELTIER) {
    sendCommand("vaPelt.val=1");   // set Nextion peltier variable to ON
    //String cmd = String("vaPelt.val=1");
    //sendCommand(cmd.c_str());
  } else {
    sendCommand("vaPelt.val=0");   // set Nextion peltier variable to OFF
  }
  // turn on or off Case Temp variable
  //  NOT WORKING @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  if (PERMIT_CASE_TEMP) {
    sendCommand("vaCaseTmp.val=1");   // set Nextion case temp variable to ON
  } else {
    sendCommand("vaCaseTmp.val=0");   // set Nextion case temp variable to OFF
  }

  // Register pop callback function for each button component
  lightsBtn.attachPop(lightsBtnPopCallback, &lightsBtn);
  assistBtn.attachPop(assistBtnPopCallback, &assistBtn);
  pointerBtn.attachPop(pointerBtnPopCallback, &pointerBtn);
  exhaustBtn.attachPop(exhaustBtnPopCallback, &exhaustBtn);
  airAssistVar.attachPop(airAssistVarPopCallback, &airAssistVar);
  
  ////////////////////////////// FLOW METER //////////////////////////////////
  // enable a call to the 'interrupt service handler' (ISR) on every rising edge at the interrupt pin
  attachInterrupt(FLOW_INTERRUPT, MeterISR, RISING);
  Meter.reset();   // reset meter variables to 0

  // Read temp and throw away first set of readings
  while (!readThermistors());
  
  // Read flow and throw away first set of readings
  Meter.tick(2000);

  homePage.show();   // Show home page
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
    if ((millis() - lastReadTemp) > UPDATE_TEMP_DELAY) {
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
                peltierPic.setPic(PELTIER_ON); // show Peltier ON picture
                peltierOn = true;
              }
            }
            if (Temp <= WATER_TEMP_LOWER_PELTIER) {
              if (peltierOn) {
                digitalWrite(PELTIER_PIN, LOW); // Turn off Peltier 
                peltierPic.setPic(PELTIER_OFF);// show Peltier OFF picture
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
          // waterGauge.setValue(gauge_val);
          waterGauge.setValue(gauge_val);

          // Display water temp value on display
          Temp = convertKtoCF(Temp);
          waterTemp.setFloatText(Temp, 1);
        }

        if (PERMIT_CASE_TEMP) {
          float Temp = convertToTemp(caseADC);

          (Temp > CASE_TEMP_UPPER_LIMIT) ? caseHighAlarm = true : caseHighAlarm = false;
          (Temp < CASE_TEMP_LOWER_LIMIT) ? caseLowAlarm = true : caseLowAlarm = false;
          
          Temp = convertKtoCF(Temp); // Display case temp value on display
          caseTempNum.setValue((int)Temp);
        }
      }
    lastReadTemp = millis();
    }
  }

  /////////////////////////// Read Water Flow //////////////////////////////
  if (PERMIT_FLOW) {
    // Read flow termistor(s) every UPDATE_TEMP_DELAY ms
    if ((millis() - lastReadFlow) > UPDATE_FLOW_DELAY) {
      // process the (possibly) counted ticks
      Meter.tick(UPDATE_FLOW_DELAY);

      // output some measurement result
      dbSerialPrintln("Currently " + String(Meter.getCurrentFlowrate()) + " l/min, " + String(Meter.getTotalVolume())+ " l total.");

      // Update nextion display
      float flowRate = Meter.getCurrentFlowrate();
      (flowRate > FLOW_RATE_UPPER_LIMIT) ? flowHighAlarm = true : flowHighAlarm = false;
      (flowRate < FLOW_RATE_LOWER_LIMIT) ? flowLowAlarm = true : flowLowAlarm = false;

      //display the flow rate number (integer)
      flowNum.setValue((int)flowRate);

      lastReadFlow = millis();
    }

    // Display the flow animation
    if ( (millis() - lastFlowAni) > FLOW_ANI_DELAY) {
      // Only animate if flowRate > 0
      if (Meter.getCurrentFlowrate() > 0.0) { 
        flowAni.setPic(currentFlowAni+ANI_START_PIC);
        currentFlowAni++;
        if (currentFlowAni == ANI_PICS) currentFlowAni = 0;
      }
      lastFlowAni = millis();
    }

  }

  /////////////////////// Display rest of items //////////////////////////////
  if ( (millis() - lastDisplayUpdate) > DISPLAY_UPDATE_DELAY) {
    // Update Power percentage from Power potentometer
    int powerVal = analogRead(POWER_PIN);  // read the power pot value
    dbSerialPrint("Power =" + String(powerVal));
    powerVal = map(powerVal, 0, 1023, 0, 100);  // convert analog value to %
    dbSerialPrintln("  Percentage=" + String(powerVal));
    psNum.setValue(powerVal);

    // Update Power Current
    // ...
    lastDisplayUpdate = millis();
  }

  // check if error condition and set fault indicator
  if (doorOpen)       current_fault = 1; //interlock opened
  if (keySwitchOpen)  current_fault = 2; //key not turned on
  if (flowHighAlarm)  current_fault = 3; //flow too high
  if (flowLowAlarm)   current_fault = 4; //flow too low
  if (waterHighAlarm) current_fault = 5; //water temp too high
  if (waterLowAlarm)  current_fault = 6; //water temp too low

  /////////////// Update status text only if status changed //////////////////
  if (current_fault != prevFault) {
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
    prevFault = current_fault;
  }
}

/* 
  dbSerialPrint("keySwitchOpen=");  // DEBUG 
  dbSerialPrintln(keySwitchOpen);   // DEBUG
          or
  dbSerialPrintln("keySwitchOpen=" + String(keySwitchOpen));   // DEBUG
*/

/*  I T E M S   T O   F I X   O R   A D D 
  Turn off visibility for objects not used
  set up code for adafruit soundboard FX 
*/

/*  I T E M S   F I X E D
  update variable names to standard defined
  Turn off Air Assist button if set to Auto (fixed in Nextion)
  Add code to watch for changes in Air Assist variable
*/
