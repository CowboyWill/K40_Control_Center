Title: K40_Control_Center

# K40_Control_Center

K40 Laser Cutter Control System using Arduino and Nextion Display.  

# __NOTE: this is not functional yet, still a work in progress.  I'll update this screen when it is functional.__

__List of Hardware Used:__
  * A custom made circuit board, with these components attached to the board:
    * Arduino Uno (Mega for testing Nextion display)
    * 1-channel relay board -- https://????
    * 4-channel relay board -- https://www.amazon.com/Huayao-Channel-Optocoupler-Arduino-Raspberry/dp/B07DN8DTRW/, 
    http://www.microcenter.com/product/476352/4_Channel_Relay_Module
    * GY-INA219 High Precision I2C Digital Current Sensor Module -- https://www.banggood.com/GY-INA219-High-Precision-I2C-Digital-Current-Sensor-Module-p-1200612.html 
    * Matek Systems 90dB 5V Loud Buzzer -- https://www.amazon.com/gp/product/B01N78FH2B 
    * Adafruit Audio FX Mini Sound Board #2342 -- https://www.adafruit.com/product/2342 
    * Adafruit Mono 2.5W Class D Audio Amplifier - PAM8302 #2130 -- https://www.adafruit.com/product/2130 

These items connect to the circuit board:
  * Water Flow Sensor with Temp -- Koolance (SEN-FM18T10) Coolant Flow Meter with Temperature Sensor, 10K Ohm -- https://koolance.com/coolant-flow-meter-stainless-steel-with-temperature-sensor-sen-fm18t10
  * Thermister Temp Sensor, standard 10k ohm 
  * Nextion Display 5"

__List of Libraries Used:__
  * Nextion.h - Official Nextion display libraries (with custom modifications, see Nextion_Custom_Mods.md)  -- https://github.com/itead/ITEADLIB_Arduino_Nextion 
  * FlowMeter.h - Flow Meter library by sekdiy (note I changed the library.properties and the keywords.txt per the 2 issues) -- https://github.com/sekdiy/FlowMeter

__This is being run as an Agile project, using trello.com to manage it__
