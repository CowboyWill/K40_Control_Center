/*****************************************************************************
 * @File     K40 Control Center.ino
 * @Title    Chinese Co2 Laser Control Center
 * @Author   Will Travis 2018
 * @Date     06-25-2018
 * @Version  0.12
 *
 * See instruction file for hookup information.
 * 
 * room temperature between 5-40C (41-104F)
 * Water temperature between 
 *    10-40C (50-104F) Tube manual 
 *    0-35C (32-95F) 
 * 
 * @Description
 * Arduino Nano - 14 Digital I/O pins, 6 Analog input pins:
 *    18-20C - 25max (64.4-68F 77max)
 *    22-25C / 71.6-77F  ideal dont go above 28F/ 82.4F
 *    16-18C 24C max / 60.8-64.4F 75.2F max
 *    never above 25C / 77F
 *    20-30C 25C / 68-86F 77F  multiple people
 *    
 * 
 *****************************************************************************/

/**************************** INCLUDE FILES *********************************/
#include "Nextion.h"
#include "test.h"
/**************************** END INCLUDES  *********************************/

/************************ GLOBAL VARIABLES **********************************/
int prev_fault = 1;        // Used to display faults only when changing

//average the flow sensor values
byte flowAni = 0;           // used to animate flow graphics
float flowRate;

// Temperature
uint32_t celcius;

unsigned long prevMillis = millis();
/************************ END GLOBAL VARIABLES ******************************/

/********************************************************************
 * Nextion components for home page
 *******************************************************************/
NexPage selfcheck_page    = NexPage(0, 0, "selfcheck");
NexVariable celcius_va      = NexVariable(0, 2, "vacf");

NexPage home              = NexPage(1, 0, "home");
NexDSButton lights_btn      = NexDSButton(1, 9, "lights_btn");
NexScrolltext status_txt    = NexScrolltext(1, 4, "status_txt");
NexText water_temp          = NexText(1, 7, "water_temp");

NexTouch *nex_listen_list[] = 
{
    &lights_btn,
    NULL
};


/******************************* FUNCTIONS **********************************/
void display_text(int fault) {
  if(fault == 0){  // 0 = OK, do not flash, but scroll instead
    status_txt.Set_scroll_dir(1);  // scroll right to left 
    status_txt.Set_scroll_distance(10); // scroll speed
  } else {
    status_txt.Set_scroll_dir(2);  // scroll up to down
    status_txt.Set_scroll_distance(40);  // this causes it to flash (height of box)
  }
  status_txt.setText(error_messages[fault]);
}

void selfCheck() { // Check all systems at startup
  dbSerialPrintln("START Self Check"); 

  delay(2000);  // wait 1 second before moving to home page
  home.show();
  delay(10);
  dbSerialPrintln("END Self Check"); 
}


/*************  Dual state button pop callback functions **************/
//Turn the LED lights on or off based on the value of the lights button. 
void lights_btnPopCallback(void *ptr)
{
  uint32_t dual_state;
  // Get the state value of dual state button component.
  lights_btn.getValue(&dual_state);
  digitalWrite(LIGHTS_PIN, dual_state);  // Turn on lights
}


// Read settings
void readSettings() {
  celcius_va.getValue(&celcius); 
}

// Read temperature from sensor
float readTemperature(byte pin) {
  int tempReading = analogRead(pin);
  float tempVolts = tempReading * 3.3 / 1024.0;
  float tempC = (tempVolts - 0.5) * 100.0;
  if (celcius) {  // 0=Celsius, 1=Fahrenheit
    return tempC;
  } else {
    return (tempC * 1.8 + 32);
  }
}



void setup(void)
{    
  pinMode(KEY_SWITCH_PIN, INPUT_PULLUP);
  pinMode(DOOR_PIN, INPUT_PULLUP);
  pinMode(LIGHTS_PIN, OUTPUT);
  digitalWrite(LIGHTS_PIN, LOW);

  nexInit();

  delay(10);  // allow nextion to initialize
  selfCheck();

  /* Register the pop event callback function of the dual state button component. */
  lights_btn.attachPop(lights_btnPopCallback, &lights_btn);
  
  dbSerialPrintln("setup done"); 
}

void loop(void)
{   
  /*
   * When a pop or push event occured every time,
   * the corresponding component[right page id and component id] in touch event list will be asked.
   */
  nexLoop(nex_listen_list);

  /* Set to no faults detected, 0=ok
      0=ok, 2=interlocks, 3=key off, 
      4=flow rate upper limit, 5=flow rate lower limit
      6=water temp upper limit, 7=water temp lower limit, 
      8=case temp upper limit, 9=case temp lower limit
  */
  int current_fault = 0; 

  //skip if bypass interlock is true
  if (!BYPASS_INTERLOCK) {
    // Check if interlock switches were opened
    if (digitalRead(DOOR_PIN) == LOW) {
        current_fault = 1; //interlocks opened
    }
    if (digitalRead(KEY_SWITCH_PIN) == LOW) {
        current_fault = 2; //key not turned on
    }
  }

  // skip temp read if true
  if (!BYPASS_WATER_TEMP) {
    float waterTemp = readTemperature(WATER_TEMP_PIN);
    water_temp.setFloatText(waterTemp,1);
    if (waterTemp > WATER_TEMP_UPPER_LIMIT) {
      current_fault = 5; //temp above upper limit
    } else if (waterTemp < WATER_TEMP_LOWER_LIMIT) {
      current_fault = 6; //temp below lower limit
    }
  }

  //enable or disable the interlock output on any error
  if (current_fault != 0) {
    digitalWrite(INTERLOCK_PIN, HIGH);  // Turn off interlock relay
  } else {
    digitalWrite(INTERLOCK_PIN, LOW);  // Turn on interlock relay
  }
  
  // Sound buzzer for critial errors
  if (current_fault > 2) {
    digitalWrite(BUZZER_PIN, HIGH); // turn alarm on
  } else {
    digitalWrite(BUZZER_PIN, LOW);  // turn off alarm
  }

  // Update the status text if status # changed
  if (current_fault != prev_fault) {
    display_text(current_fault);
    prev_fault = current_fault;
  }

}
