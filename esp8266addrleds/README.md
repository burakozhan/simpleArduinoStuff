This folder holds an implementation for a WS2812 Strip that is system powered, and Relay separable. It board is based on the ESP8266 controller. Software is based on the ESP8266 Captive Portal example provided with the arduino library.

Switch 1 - Reset
Switch 2 - GPIO 0 (pull-up) - UART Mode
Switch 3 - GPIO 2 (pull-up) - Flash Mode
Gnd - GPIO 15 - SD card boot

Relay K1 - GPIO 12 - Data out to ws2812b
Relay K2 - GPIO 5
Relay K3 - GPIO 4 - Power switch to LED strip

Input 1 - GPIO 16
Input 2 - GPIO 14
Input 3 - GPIO 13