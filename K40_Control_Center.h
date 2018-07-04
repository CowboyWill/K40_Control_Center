/* Pin Assignments for Arduino board:
 * 
 * Nano Pins
 * D0-I- RX (Nextion)
 * D1-O- TX (Nextion)
 * D2-I- INT0-Water flow monitor(orange to D2, red to +5V, black to ground)
 * D3-I- Key switch (NC contact 1 to gnd, contact 2 to D7)
 * D4-O- Warning buzzer (Buzzer+ to D4, buzzer- to ground)
 * D5-O- Relay to disable laser (+5V to relay VCC, D5 to Relay IN,
 *          laser enable1 to relay common, laser enable2 to relay NO)
 * D6-Peltier On/Off
 * D7-I- Door Switch (Hook up switch between ground and D3)
 *         (Connect additional microswitches in series to protect other doors 
 *          like the laser tube door and controls door)
 * D8-O- Laser Pointer On/Off
 * D9-O- Internal Lights On/Off
 * D10-Little Buddy (CS)
 * D11-Little Buddy (DI)
 * D12-
 * D13-Little Buddy (SC)
 * D14/A0-Power Setting
 * D15/A1-Water Level
 * D16/A2-Air Assist  On/Off
 * D17/A3-Exhaust Fan On/Off
 * A4-Current Sensor (I2C)
 * A5-Current Sensor (I2C)
 * A6- Case temperature sensor = 10k Thermistor
 *      (AREF to 3.3v, 3.3v to 10k resistor to sensor and A2, sensor to gnd)
 * A7- Water temperature sensor = 10k Thermistor
 *          (3.3v to 10k resistor to sensor and A3, sensor to gnd)
*/

#if defined(ARDUINO_AVR_UNO)
  //Nano specific code
  #define WATER_FLOW_PIN  32  //D2
  #define KEY_SWITCH_PIN  1   //D3
  #define BUZZER_PIN      2   //D4
  #define INTERLOCK_PIN   9   //D5 
  #define PELTIER_PIN     10  //D6 
  #define DOOR_PIN        11  //D7
  #define POINTER_PIN     12  //D8
  #define LIGHTS_PIN      13  //D9
  #define BUDDY_CS_PIN    14  //D10 
  #define BUDDY_DI_PIN    15  //D11
  //D12 (16) not used because part of SPI
  #define BUDDY_SC_PIN    17  //D13
  #define POWER_PIN       23  //D14/A0
  #define LEVEL_PIN       24  //D15/A1
  #define ASSIST_PIN      25  //D16/A2
  #define EXHAUST_PIN     26  //D17/A3
  #define CURRENT_SDA_PIN 27  //A4
  #define CURRENT_SCL_PIN 28  //A5
  #define CASE_TEMP_PIN   19  //A6
  #define WATER_TEMP_PIN  22  //A7
#else
//Mega 2560 specific code
  #define DOOR_PIN        2  //D7
  #define KEY_SWITCH_PIN  3  //D3
  #define INTERLOCK_PIN   5  //D5 
  #define LIGHTS_PIN      9  //D9
  #define BUZZER_PIN      31 //D4
  #define WATER_TEMP_PIN  0  //A0

  #define EXHAUST_PIN     7  //D17/A3
  #define POINTER_PIN     8  //D8
  #define WATER_FLOW_PIN  32  //D2
  #define CASE_TEMP_PIN   1  //A1
  #define PELTIER_PIN     10  //D6 
  #define BUDDY_CS_PIN    14  //D10
  #define BUDDY_DI_PIN    15  //D11
  #define BUDDY_SC_PIN    17  //D13
  #define POWER_PIN       23  //D14/A0
  #define LEVEL_PIN       24  //D15/A1
  #define ASSIST_PIN      25  //D16/A2
  #define CURRENT_SDA_PIN 27  //A4
  #define CURRENT_SCL_PIN 28  //A5
#endif

char error_messages[9][28] = {
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

const bool BYPASS_INTERLOCK = false;  //true = ignore state of the interlocks

// Flow Rate
const bool BYPASS_FLOW = false; //set true to ignore the flow sensor
const uint32_t ANI_START_PIC = 1;    // Which image is the first for flow animation (Nextion pic #)
const uint32_t ANI_PICS = 4;         // How many images are in the flow animation
const unsigned long FLOW_ANI_DELAY = 200; // Animation delay in ms
const float FLOW_RATE_UPPER_LIMIT = 15.0; //(gal per minute)upper limit of flow rate
const float FLOW_RATE_LOWER_LIMIT = 2.0;  //(gal per minute)lower limit of flow rate

// Temperature
const bool BYPASS_WATER_TEMP = false;   //set true to ignore water temp sensor
const float WATER_TEMP_UPPER_LIMIT = 28.0;  // water temp in degrees upper limit (28 in C)
const float WATER_TEMP_LOWER_LIMIT = 22.0;  // water temp in degrees lower limit (22 in C)
const bool BYPASS_CASE_TEMP = true;  //enable the case temp sensor
const float CASE_TEMP_UPPER_LIMIT = 50.0;  // water temp in degrees upper limit (in C)
const float CASE_TEMP_LOWER_LIMIT = 10.0;  // water temp in degrees lower limit (in C)

// MISC settings
const bool OPEN = LOW;          // Door setting if opened
const bool ENGAGED = HIGH;      // Key setting if engaged
const float DISPLAY_UPDATE_DELAY = 1000.0;  // update every 1 second (1000ms)


