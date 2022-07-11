# ESP 8266-201 Development board setup

This will explain how to install the ESP board to the arduino ide and set up and use the ESP 8266 201 development board.

## Installation

### Installing Arduino

You should download the [Arduino IDE](https://docs.arduino.cc/).

### Installing ESP 8266 board to the Arduino IDE

1. Open preferences from File > Preferences or by pressing Ctrl + ,
2. Paste the below link in 'Additional boards manager URLs'
        ```
        https://arduino.esp8266.com/stable/package_esp8266com_index.json
        ```
3. Press 'OK' and close the Arduino Software and open the software.
4. Open Board Manager from Tools > Board > Board Manager or by pressing Ctrl + Shift + B
5. Search for esp8266. And download the latest version (v2.7.4)
6. In Tools > Board you should see esp8266. That means the board is installed correctly.

### Installing DHT Sensor Library

1. Open Manage libraries from Tools > Manage libraries  or by pressing  Ctrl + Shift + I
2. Search dht sensor library and download the library. (By Adafruit v1.4.4)

## Uploading code

1. Connect the board.
5. Power On by pressing the button
2. Select the board from Tools > Board > esp8266 and select Generic ESP8266 Module
3. Select com port from Tools > Port (The port can be also found in Device Manager as well)
4. Switch on 7, 8 switches. And press Upload button or by pressing  Ctrl + U
5. Power Off by pressing the button
4. Switch off 7, 8 switches. And press Upload button or by pressing  Ctrl + U

## Run the Main.ino code to check boards outputs (Make sure to switch on all switches except 7,8)

![DHT 11 Pinout](https://lastminuteengineers.b-cdn.net/wp-content/uploads/arduino/DHT11-DHT22-AM2302-Temperature-Humidity-Sensor-Pinout.png)

The DHT sensor should be connected to the female pin header with the net like thing looking the ESP board

![ESP 8266 201 development board](https://github.com/MindiyaCreations/esp-201-thingsboard/blob/main/Main/Schematic.png)

## Pin numbers

- RED_PIN 15
- GREEN_PIN 13
- BLUE_PIN 12
- WHITE_PIN 14
- RELAY_PIN 16
- BUZZER_PIN 5
- DHT_PIN 4
- S2_PIN 0
- S3_PIN 2
