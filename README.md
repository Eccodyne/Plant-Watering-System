## 📌 Welcome to the "Plant Watering System" Project!
![Plant Watering System](https://github.com/Eccodyne/Plant-Watering-System/blob/main/Images/IMG_2521.JPG)

# Plant Watering System
This is a fully automated plant watering system based on an ESP32 board and Arduino code. It provides for two separate water pumps which can be easiliy configured through a self explainatory menu system using the rotary encoder and the secondary button.

### 💡What is this repository for?
This repository contains the code and build instructions for the "Plant Watering System" Project with the required 3D printing files to be found on Makerworld.com. 

Features:
- Two separately controllable water pumps
- Individual watering schedules for each weekday (Monday to Sunday)
- Customizable watering duration for each pump
- Saves all settings to the EEPROM of the ESP32 board so nothing gets lost when device is disconnected from power
- Uses DS3231 RTC module to keep time/date to trigger watering at the right ime
- Easy navigation and configuraton via rotary encoder with push button (to enter menus and make settings) and secondary push button (to return from menus)
- Automatic screen dimming (idle timer can be configured)

This project is not complicated and does not require a high level of technical know-how. You just need time, patience and to follow the instructions as set out below.

Link to 3D printing files on Makerworld: [Click](https://makerworld.com/de/models/2927499-garmin-g1000-controller-ms-flight-simulator-2024#profileId-3276793)

---
### 🛒 What components do I need to make this project?
- 3D printed housing parts
- Keyestudio ESP3232-WROOM-32 Development Board 
- 2,4 inch EC11 LCD display with rotary encoder and secondary push button (320x240 pixels)
- DS3231 real time clock module with backup battery (board has three holes for screw fixing)
- 2x mini water pumps (3v)
- Silicon hose 3mm inner diameter, 5mm outer diameter. Purchase whatever length you require, I bought a 5m long hose and cut it to indivual lenghts as per my needs.  
- 2x 3v relay power switches
- Sufficient number of jumper wires
- 15x M2.5 screws to attach the ESP32 baord, the relays and the DS3231 RTC mdoule to the housing
- 10x M3 screws to attach the LCD display, the display cover and to fixate the back of the housing
- Hot glue to fix the water pumps to the housing
- USB-C cable to connect the ESP32 board to your PC
- 4x self-adhesive rubber feet to be attached to the bottom of the housing

***
### 💾 What software do I need to make this project?  
- Arduino IDE (free of cost)

***
### 🔎 How do I get started?
- Download *.3mf file for the Plant Watering System Project on Makerworld: [Link](https://makerworld.com/de/models/2927499-garmin-g1000-controller-ms-flight-simulator-2024#profileId-3276793)
- Print all parts with your 3D printer
- Attach the ESP32 board to the bottom of the housing using M2.5 screws
- Attach the DS3231 RTC module to the bottom of the housing using M2.5 screws
- If there are any mini plugs/connectors attached to the cables of your water pumps, cut them off to do the next step
- Solder jumnper wires to the GND cable (blue) and V cable (red) of the water pumps. Important: the V cable coming from the water pump must have a male connector which needs to be attached to a relay later on. This does not work with a female connector.
- Attach the water pumps to the buttom of the housing using hot glue
- Attach the relays to the top part of the housing using M2.5 screws
- Solder the pin bridge to the board of the LCD display
- Attach the LCD display to the display cover using M2.5 screws
- Attach the printed control knob to the rotary encoder of the LCD display
- Connect all pins of the LCD display to the ESP32 board using jumper wires. Take note of each pin number you are using on the ESP32 board, you will need to adjust the Arduino code accordingly later on 
- Pins A and B of the LCD's rotary encoder can be attached to analogue inputs of the ESP32 board, everything else needs to go to digital inputs.
- Make sure that GND and V (Voltage) pins are connected correctly. Do not use 5V pins on the ESP32 board, only 3V pins.
- Wiring the relays: Connect VCC to a 3V pin on the ESP32 board, connect GND to a GND pin on the ESP32 board, connect the IN pin to a digital input on the ESP32 board (take note of the input used). Connect NC to the V cable of a water pump, connect COM to a 3V pin on the ESP32 board
- Start Arduino IDE and load the code/sketch provided for in this repository
- Go to the // PIN CONFIGURATION // section of the sketch
- Change the pin numbers in accordance with the pins that you are using on your ESP32 board
- Save the sketch and upload it to your ESP32 board (for the Keyestudio ESP32, select "ESP32 Dev Module" as device in Arduino IDE)
- Attach the silicon hoses to the "in" and "out" ports of the water pumps. The "out" ports have a smaller inner diameter than the "in" ports. You can use mini zip ties if you want to attach the hoses to the "in" and "out" ports more securely.
- Attach the silicon hoses coming from the water outlets of the Plant Watering System to the printed spikes. Push the spikes into the soil of the plants you want to water.
- Fill a container with water and place silicon hoses connected to the "in" ports of the Plant Watering System in the container. Use a weight to keep the hoses from moving in the water container

***
### ⚙️ Configuring your Plant Watering System
Configuration is very easy as all menu items are self explainatory. The rotary knob is used for scrolling through the menus and adjusting settings - settings are applied when pressing down the rotary knob as it also has a button function. The secondary button below the rotary encoder is used to jump back from a menu page to the previous menu page.

When running the system for the first time, make sure to set time and date. Given that the DS3231 RTC module has a battery backup, no time/date settings will be lost should the device be disconnected from power. The same is true for all other settings of the device which are saved to the ESP32's EEPROM.

The most important point to make here is that you can create individual watering schedules for each pump. There are seven schedules for each weekday from Monday to Sunday. Activate or deactivate any schedule as per your own watering requirements. You can also manually run the pumps (single or dual) if need be. The watering time for each pump has to be set in seconds - it is best to run a test to see how much water your pumps deliver per second - adjust the overall time accordingly. 

***
### 📷 Images of the build and its parts:

![Water pump](https://github.com/Eccodyne/Plant-Watering-System/blob/main/Images/PIC5.JPG)

![Relay](https://github.com/Eccodyne/Plant-Watering-System/blob/main/Images/PIC6.JPG)

![Display](https://github.com/Eccodyne/Plant-Watering-System/blob/main/Images/PIC4.JPG)

![PIC1](https://github.com/Eccodyne/Plant-Watering-System/blob/main/Images/PIC1.JPG)

![PIC2](https://github.com/Eccodyne/Plant-Watering-System/blob/main/Images/PIC2.JPG)

![PIC3](https://github.com/Eccodyne/Plant-Watering-System/blob/main/Images/PIC3.JPG)

![PIC4](https://github.com/Eccodyne/Plant-Watering-System/blob/main/Images/Housing_Back.JPG)



