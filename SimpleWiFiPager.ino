/*  WiFi Web Server: Remote LEDs/Loads Control and Message sending via local WiFi network
 *  Possibly can be used from global Web if your router is configured properly
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
*/

#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include "button.h"   
#include "timer.h"    
#include "clock.h"
#include "show.h"  

#define LED_PIN 13
#define BTN_PIN 12
#define LCD_WIDTH 16
#define MSG_MAX_LEN 38 // max length of the message to fit into lcd scrolling screen (appr.40 for LCD 2*16 ??)

void show();                    // Shows the info on LCD, see below
String parseMessage(String &)   // Parses the message sent via html, see below
void blinkLCD(cnt)              // Flashes LCD screen, see below

const char* ssid     = "akuLpotS"; // your Wifi network name
const char* password = "dtxyjcnm"; // your Wifi network password
String msg = "";                   // here we hold the message received

Timer timer_1sec(1000);                     // is used to drive the clock
Timer timer_scroll(500);                    // is used to scroll and update the LCD screen
Clock myClock;                              // is used to measures time after the message is received
Button myButton(BTN_PIN);                   // push-button to confirm the receit of the message and turn-off the screen
LiquidCrystal_I2C lcd(0x27, LCD_WIDTH, 2);  // Check for proper I2C address (most common is 0x27)
WiFiServer server(80);

void setup(){
    pinMode(BTN_PIN, INPUT_PULLUP); // actually not nessesary since it is configured in Button class
    pinMode(LED_PIN, OUTPUT);       // set the LED pin mode
    
    Serial.begin(115200);

    lcd.init(); lcd.backlight(); lcd.setCursor(0, 0);
    show();

    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println(""); Serial.println("WiFi connected.");
    Serial.println("IP address: "); Serial.println(WiFi.localIP());
    // You can use the MAC-address displayed below to setup your router to allocate particular static and permanent IP-address for your ESP32 WiFi server
    Serial.println("MAC address: "); Serial.println(WiFi.macAddress());
    
    server.begin();
}

void loop(){

 // On button press: clear message, turn-off LCD backlight and reset the clock
 if (myButton.click())                                       { msg = ""; lcd.noBacklight(); myClock.reset(); }
 // Increment clock only if message is not empty
 if (msg.length() && timer_1sec.ready())                     { myClock.tick(); show(); }
 // scroll the screen if message does not fit into screen
 if (timer_scroll.ready() && msg.length() > LCD_WIDTH - 2)   { lcd.scrollDisplayLeft(); }  
 
 WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");          // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK) and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK"); client.println("Content-type:text/html"); client.println();

            // the content of the HTTP response follows the header:
            // Give some feedback on the web-page about status of the load and the message
            if (digitalRead(LED_PIN)) client.print("LOAD IS ON!");
            else                      client.print("LOAD IS OFF!");
            // Buttons to turn ON/OFF the loads. You can add additional buttons and buttons status messages HERE
            client.print("<form action = '' method = 'get'> <input type = 'hidden' name = 'L' value = '1' /> <input type = 'submit' value = 'Turn ON' /> </form>");
            client.print("<form action = '' method = 'get'> <input type = 'hidden' name = 'L' value = '0' /> <input type = 'submit' value = 'Turn OFF' /> </form>");
            // Message submit field and submit and check buttons
            client.print("<form action = '' method = 'get'> <input type = 'form' name = 'M'/> <input type = 'submit' value = 'SEND' /> </form>");
            client.print("<form action = '' method = 'get'> <input type = 'hidden' name = 'C' value = '0'/> <input type = 'submit' value = 'CHECK MESSAGE STATUS' /> </form>");
            if (!msg.length()) client.print("THE MESSAGE HAS BEEN READ!");
            else {
              client.print("THE MESSAGE HAS NOT BEEN READ for: "); client.print(myClock.getHours()); client.print(" h : ");
              client.print(myClock.getMinutes()); client.print(" m : "); client.print(myClock.getSeconds()); client.print(" s");
            }
            
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see the client requests:
        if (currentLine.endsWith("GET /?L=1")) {                          
          digitalWrite(LED_PIN, HIGH);                                    // turns the LED on
          Serial.println(); Serial.println("-------------LED is ON!");    // for debugging and control purpose only
          Serial.println(); Serial.flush(); 
        }
        else if (currentLine.endsWith("GET /?L=0")) {                               
          digitalWrite(LED_PIN, LOW);                                     // turns the LED off
          Serial.println(); Serial.println("-------------LED is OFF!");   // for debugging and control purpose only
          Serial.println(); Serial.flush();                               
        }
        // Process the message
        else if (currentLine.endsWith("GET /?M=")){
          String message;
          while (client.available()) {                                    // if there's bytes to read from the client,
            char cc = client.read();
            message += cc;
            if (message.endsWith(" HTTP"))  { message = message.substring(0, message.length()-5); break; };
            if (message.endsWith("\n"))     { break; }
          }
          //Serial.println(); Serial.println("------------MESSAGE IS: "); // for debugging and control purpose only
          msg = parseMessage(message);
          // cut the message if it is too long
          if (msg.length() > MSG_MAX_LEN) msg = msg.substring(0, MSG_MAX_LEN);
          // flash the LCD screen to draw attention to incoming message  
          blinkLCD(5); 
          // reset the timer to 0:0:0 and position of the message to beginning of the line on LCD screen        
          myClock.reset();     
          currentLine.clear(); 
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected."); Serial.flush();
    if (msg.length()){ // for debugging and control purpose only
      Serial.println(); Serial.println("----------------------------------MESSAGE IS: "); Serial.println(msg); Serial.println(); Serial.println();
    }
  }
}
//======================================
void show(){ 
  lcd.setCursor(0, 0); lcd.print("                ");
  byte h = myClock.getHours(), m = myClock.getMinutes(), s = myClock.getSeconds();
  lcd.setCursor(2, 0); //lcd.print("Time:  ");
  if (h < 10) lcd.print(0);
  lcd.print(h); lcd.print(":");
  if (m < 10) lcd.print(0);
  lcd.print(m);lcd.print(":");
  if (s < 10) lcd.print(0);
  lcd.print(s);
  lcd.setCursor(0, 1); lcd.print("                ");
  lcd.setCursor(2, 1); lcd.print(msg);
  // used to show measured temperature
  //lcd.setCursor(0, 1); lcd.print("Temp:  "); lcd.print(temperature); lcd.write(223); lcd.print("C ");
}
//=======================================
String parseMessage(String &inputLine){ // Parses the message sent via html
  //Serial.println(); Serial.print("INPUT MESSAGE: "); Serial.println(inputLine); Serial.println(); Serial.println();
  String message = "";
  for (size_t i = 0; i < inputLine.length(); ++i){
    
    if (inputLine[i] == '+')  {message += ' '; continue;}
    if (inputLine[i] == '\n') {/*message += ' '*/; continue;}
    if (inputLine[i] == '%' && i < inputLine.length() - 2) {
      String symbolToDecode = inputLine.substring(i, i + 3); // e.g. symbol ',' is "%3A"
      //Serial.println(); Serial.print("++++++symbolToDecode: "); Serial.println(symbolToDecode); Serial.println();
      if      (symbolToDecode == "%2C") message += ','; 
      else if (symbolToDecode == "%21") message += '!';
      else if (symbolToDecode == "%3A") message += ':';
      else if (symbolToDecode == "%3F") message += '?';
      else                              message += " - "; // other undecoded symbols
      i += 2;
      continue;
    }
    message += inputLine[i];
  }
  //Serial.println(); Serial.print("OUTPUT MESSAGE: "); Serial.println(message); Serial.println(); Serial.println();
  return message;
}
//==============================================
void blinkLCD(size_t cnt){ // blinks LCD-backlight cnt-times
  for (size_t i = 0; i < cnt; ++i){
    lcd.noBacklight();
    delay(100);
    lcd.backlight();
    delay(100);
  }
  lcd.init(); lcd.setCursor(2, 0);
}
