/***************************************************************************** 
 * File: K40_Control_Center.h
 *   Header file for K40_Control_Center.ino
 * 
 *   Defines: Pinouts of Arduino
 * 
 *   These are pinouts for both Arduino
 * 
 *  WATER_FLOW_PIN - Water flow monitor(?????? to INT0, black to ground)
 *  KEY_PIN        - Key switch (NC contact 1 to gnd, contact 2 to PIN)
 *  BUZZER_PIN     - Warning buzzer (Buzzer+ to PIN, buzzer- to ground)
 *  INTERLOCK_PIN  - Relay to disable laser (+5V to relay VCC, PIN to Relay IN,
 *                   laser enable1 to relay common, laser enable2 to relay NO)
 *  PELTIER_PIN    - Used to turn Peltier cooler on/Off
 *  DOOR_PIN       - Door Switch (Hook up switch between ground and PIN)
 *                   Connect additional microswitches in series to protect 
 *                   other doors like the laser tube door and controls door
 *  POINTER_PIN    - Laser pointer connected to this pin
 *  LIGHTS_PIN     - Cabinet lights connected to this pin
 *  POWER_PIN      - Power potentiometer level
 *  LEVEL_PIN      - Water level detector connected to this pin
 *  ASSIST_PIN     - Air Assist pump connected to this pin
 *  EXHAUST_PIN    - Exhaust fan connected to this pin
 *  CURRENT_SDA_PIN- Current Sensor (I2C)
 *  CURRENT_SCL_PIN- Current Sensor (I2C)
 *  CASE_TEMP_PIN  - Case temperature sensor = 10k Thermistor
 *                   5v to 10k resistor to sensor and PIN, sensor to gnd
 *  WATER_TEMP_PIN - Water temperature sensor = 10k Thermistor
 *                   5v to 10k resistor to sensor and PIN, sensor to gnd
*****************************************************************************/
#if defined(ARDUINO_AVR_UNO)
  //Nano specific code
  #define WATER_FLOW_PIN  32  //D2
  #define KEY_PIN         1   //D3
  #define BUZZER_PIN      2   //D4
  #define INTERLOCK_PIN   9   //D5 
  #define PELTIER_PIN     10  //D6 
  #define DOOR_PIN        11  //D7
  #define POINTER_PIN     12  //D8
  #define LIGHTS_PIN      13  //D9
  #define POWER_PIN       23  //D14/A0
  #define LEVEL_PIN       24  //D15/A1
  #define ASSIST_PIN      25  //D16/A2
  #define EXHAUST_PIN     26  //D17/A3
  #define CURRENT_SDA_PIN 27  //A4
  #define CURRENT_SCL_PIN 28  //A5
  #define CASE_TEMP_PIN   19  //A6
  #define WATER_TEMP_PIN  22  //A7
#else
/*Mega 2560 specific code
                                      +-----+
         +----[PWR]-------------------| USB |--+
         |                            +-----+  |
         |           GND/RST2  [ ] [ ]         |
         |         MOSI2/SCK2  [ ] [ ]  SCL[ ] |   D0
         |            5V/MISO2 [ ] [ ]  SDA[ ] |   D1
         |                             AREF[ ] |
         |                              GND[ ] |
         | [ ]N/C                        13[ ]~|   B7
         | [ ]IOREF                      12[ ]~|   B6
         | [ ]RST                        11[ ]~|   B5
         | [ ]3V3      +----------+      10[ ]~|   B4
         | [ ]5v       | ARDUINO  |       9[ ]~|   H6
         | [ ]GND      |   MEGA   |       8[ ]~|   H5
         | [ ]GND      +----------+            |
         | [ ]Vin                         7[ ]~|   H4
         |                                6[ ]~|   H3
         | [ ]A0                          5[ ]~|   E3
         | [ ]A1                          4[ ]~|   G5
         | [ ]A2                     INT5/3[ ]~|   E5
         | [ ]A3                     INT4/2[ ]~|   E4
         | [ ]A4                       TX>1[ ]~|   E1
         | [ ]A5                       RX<0[ ]~|   E0
         | [ ]A6                               |   
         | [ ]A7                     TX3/14[ ] |   J1
         |                           RX3/15[ ] |   J0
         | [ ]A8                     TX2/16[ ] |   H1         
         | [ ]A9                     RX2/17[ ] |   H0
         | [ ]A10               TX1/INT3/18[ ] |   D3         
         | [ ]A11               RX1/INT2/19[ ] |   D2
         | [ ]A12           I2C-SDA/INT1/20[ ] |   D1         
         | [ ]A13           I2C-SCL/INT0/21[ ] |   D0
         | [ ]A14                              |            
         | [ ]A15                              |   Ports:
         |                RST SCK MISO         |    22=A0  23=A1   
         |         ICSP   [ ] [ ] [ ]          |    24=A2  25=A3   
         |                [ ] [ ] [ ]          |    26=A4  27=A5   
         |                GND MOSI 5V          |    28=A6  29=A7   
         | G                                   |    30=C7  31=C6   
         | N 5 5 4 4 4 4 4 3 3 3 3 3 2 2 2 2 5 |    32=C5  33=C4   
         | D 2 0 8 6 4 2 0 8 6 4 2 0 8 6 4 2 V |    34=C3  35=C2   
         |         ~ ~                         |    36=C1  37=C0   
         | @ # # # # # # # # # # # # # # # # @ |    38=D7  39=G2    
         | @ # # # # # # # # # # # # # # # # @ |    40=G1  41=G0   
         |           ~                         |    42=L7  43=L6   
         | G 5 5 4 4 4 4 4 3 3 3 3 3 2 2 2 2 5 |    44=L5  45=L4   
         | N 3 1 9 7 5 3 1 9 7 5 3 1 9 7 5 3 V |    46=L3  47=L2   
         | D                                   |    48=L1  49=L0    SPI:
         |                                     |    50=B3  51=B2     50=MISO 51=MOSI
         |     2560                ____________/    52=B1  53=B0     52=SCK  53=SS 
          \_______________________/         
         http://busyducks.com/ascii-art-arduinos   */

  #define WATER_FLOW_PIN    2  //INT
  #define KEY_PIN           3
  #define DOOR_PIN          4
  #define INTERLOCK_PIN     5
  #define PELTIER_PIN       6
  #define BUZZER_PIN        7
  #define POINTER_PIN       8
  #define LIGHTS_PIN        9
  #define AUDIO_FX_TX_PIN  10
  #define AUDIO_FX_RX_PIN  11
  #define AUDIO_FX_RST_PIN 12
  #define POWER_PIN        A0
  #define ASSIST_PIN       A2
  #define EXHAUST_PIN      A3
  #define WATER_TEMP_PIN   A6
  #define CASE_TEMP_PIN    A7

  #define LEVEL_PIN        A1
  #define CURRENT_SDA_PIN  A4
  #define CURRENT_SCL_PIN  A5
#endif

//////////////////////////////////////////////////////////////////////////////
// Constant: MESSAGES
//   contains an array messages to display on bottom of Nextion
const char MESSAGES[9][28] = {
  "READY TO CUT",
  "CASE COVER OPEN",
  "KEY TURNED OFF, NOT READY",
  "FLOW RATE TOO HIGH",
  "FLOW RATE TOO LOW",
  "WATER TEMP TOO HIGH",
  "WATER TEMP TOO LOW",
  "CASE TEMP TOO HIGH",
  "CASE TEMP TOO LOW"
};

//////////////////////////////////////////////////////////////////////////////
/***************************************************************************** 
 * Constants: Locks
 *    PERMIT_INTERLOCK - true = read state of interlocks, false = ignore state
 *    PERMIT_INTERLOCK - true = read state of key switch, false = ignore state
*****************************************************************************/
const bool PERMIT_INTERLOCK = true;  
const bool PERMIT_KEY = true;  

/***************************************************************************** 
 * Constants: Flow Rate Variables
 *     PERMIT_FLOW    - set to false to ignore the flow sensor
 *     FLOW_RATE_UPPER_LIMIT - (gal per minute)upper limit of flow rate
 *     FLOW_RATE_LOWER_LIMIT - (gal per minute)lower limit of flow rate
 *     UPDATE_FLOW_DELAY  - set the measurement update period in ms
 *     FLOW_INTERRUPT     - interrupt pin for flow meter
 *     ANI_START_PIC  - Which image is the first for flow animation (Nextion pic #)
 *     ANI_PICS       - How many images are in the flow animation
 *     FLOW_ANI_DELAY - Animation delay in ms
*****************************************************************************/
const bool PERMIT_FLOW = true; 
const float FLOW_RATE_UPPER_LIMIT = 15.0;
const float FLOW_RATE_LOWER_LIMIT = 2.0;
const unsigned long UPDATE_FLOW_DELAY = 500; // 1000ms = 1sec
const byte FLOW_INTERRUPT = digitalPinToInterrupt(WATER_FLOW_PIN);

const byte ANI_PICS = 4;
const byte ANI_START_PIC = 21;
const unsigned long FLOW_ANI_DELAY = 200;

/***************************************************************************** 
 * Constants: Temperature Variables
 *     UPDATE_TEMP_DELAY      - update temp on display every ms
 *     PERMIT_WATER_TEMP      - set to false to ignore water temp sensor
 *     WATER_TEMP_UPPER_LIMIT - water temp in degrees upper limit (in K)
 *     WATER_TEMP_LOWER_LIMIT - water temp in degrees lower limit (in K)
 *     PERMIT_CASE_TEMP       - set to false to ignore case temp sensor
 *     CASE_TEMP_UPPER_LIMIT  - case temp in degrees upper limit (in K)
 *     CASE_TEMP_LOWER_LIMIT  - case temp in degrees lower limit (in K)
 *     
 *     Limits: All temp limits in Kelvin
*****************************************************************************/
const int UPDATE_TEMP_DELAY = 100;

const bool PERMIT_WATER_TEMP = true;
const float WATER_TEMP_LOWER_LIMIT = 303.15; // (293.15K / 20C)
const float WATER_TEMP_UPPER_LIMIT = 318.15; // (303.15K / 30C)

const bool PERMIT_CASE_TEMP = true;
const float CASE_TEMP_LOWER_LIMIT = 10.0;    // (5C / 41F)
const float CASE_TEMP_UPPER_LIMIT = 50.0;    // (40C / 104F)

/***************************************************************************** 
 * Constants: Thermistor Variables
 *    INVBETA     - invert Beta Coefficient for Thermister
 *    INVT0       - room temp in Kelvin (Celcuis + 273.15)
 *    NUM_SAMPLES - number of termistor samples to average
*****************************************************************************/
const float INVBETA = 1.00 / 3950.0;
const float INVT0 = 1.00 / (25+273.15);
const int NUM_SAMPLES = 5;

/***************************************************************************** 
 * Constants: Peliter
 *    PERMIT_PELTIER           - set to false to ignore peltier
 *    WATER_TEMP_UPPER_PELTIER - Upper temp to turn on Peltier
 *    WATER_TEMP_LOWER_PELTIER - Lower temp to turn off Peltier
 *    PELTIER_OFF              - picture number of peltier turned off
 *    PELTIER_ON               - picture number of peltier turned on
*****************************************************************************/
const bool PERMIT_PELTIER = true;
const float WATER_TEMP_UPPER_PELTIER = 310.15; // (300.15K / 30C)
const float WATER_TEMP_LOWER_PELTIER = 308.15; // (296.15K / 30C)
const byte PELTIER_OFF = 19;
const byte PELTIER_ON = 20;

/*****************************************************************************
 * Constants: MISC settings Variables
 *    OPEN                 - Door setting if opened
 *    LOCKED               - Key locked (disable laser)
 *    DISPLAY_UPDATE_DELAY - update Nextion display every ms
*****************************************************************************/
const bool OPEN = LOW;
const bool LOCKED = LOW;
const unsigned long DISPLAY_UPDATE_DELAY = 200;


/*  Temp limits from different sources

    18-20C - 25max (64.4-68F 77max)
    22-25C / 71.6-77F  ideal dont go above 28F/ 82.4F
    16-18C 24C max / 60.8-64.4F 75.2F max
    never above 25C / 77F
    20-30C 25C / 68-86F 77F  multiple people
*/
