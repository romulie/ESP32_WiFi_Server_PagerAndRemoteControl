 WiFi Web Server: Remote LEDs/Loads Control and Message sending via local WiFi network
 Possibly can be used from global Web if your router is configured properly
 
 A simple web server that lets you control LEDs (or turn on/off loads) via the web and WiFi and send messages (up to 38 chars for LCD 16*2, English only)
 
 You should change the name of your wifi network and provide the password (see below)
 This sketch will print the IP address and MAC address of your WiFi Shield (once connected) to the Serial monitor.
 TODO: for dynamic IP assignment show IP address on LCD after connecting to the WiFI to be able to connect to the device without serial monitor on PC
 From there, you can open that address in a web browser to turn on and off the LED (or loads) on pin(s) defined below
 and send messages which are then displayed on the LCD screen.
 Using push-button you can reset the received message and turn-off the backlight of the LCD screen
 You can see the status of the LEDs/loads and the status of the message (read or not and for how long) on the web-page as well.

 If the IP address of your shield is yourAddress:
 http://yourAddress/L=1 turns the LED/load on
 http://yourAddress/L=0 turns it off

 This example is written for a network using WPA2 encryption. For insecure
 WEP or WPA, change the Wifi.begin() call and use Wifi.setMinSecurity() accordingly.

 Circuit:
 ESP32WROOM;
 LED(load) attached to pin 13;
 push-button attached to pin 12 (should have pull-up);
 I2C LCD 16*2 attached to pins 21(SDA) and 22(SCL) (I2C of ESP32), VCC and GND

created for arduino 25 Nov 2012 by Tom Igoe and modified Jun 2023 by romulie
ported for sparkfun esp32  31.01.2017 by Jan Hendrik Berlin
