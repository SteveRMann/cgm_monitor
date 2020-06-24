#define sketchName "cgm.ino"
#define sketchVersion "3.00"    // Four characters
#define hostPrefix "CGM-"       // Six characters max

/*
   Port      = Location
   CGM....8a = office
   CGM....c1 = Protoype
   CGM....5b = Mater BR
   CGM....9c = Basement
   CGM....00 = Family RM
*/

/*****

  D:\River Documents\Arduino\Arduino Projects\CGM\cgm-wemos2\cgm-wemos2.ino
  04/23/2019 - Added second display, clone of cgm-Wemos2.ino
  04/30/2019 - This version is an attempt to get the local time to show in the second display as HH:MM.
  04/30/2019 - Flicker was fixed with 47uF caps on the display backpacks
  05/06/2019 - clone from Version 2c, but fixed the time format in Node Red
  05/24/2019 - Added #define sketchName
  Version 2
    6/27/2019 - Introduced to github.
                Added WiFi.enableInsecureWEP(); before WiFi.begin(ssid, password);
  Version 2.1
    02/12/2020 - Removed button handlers
    02/12/2020 - Lots of changes.
                 Added OTA and a unique hostname using the MAC address.
  Version 2.11
    04/23/2020 - Changed LED pin# to match the PCB.
  Version 3.00
    04/24/2020 - Reboot if the time is not updated in 2 minutes.

**PROBLEMS**
  I am using millis to test for stale data timeout, but the code also includes the ticker.h library.
  Which is more useful?

  IDE settings:
    NodeMCU 1.0 (ESP-12E Module), 4M, 1M SPIFFS
    Wemos D1 R1

  The CGM portion of this sketch runs equally on NodeMCU and Wemos D1 Mini.
  The button functions have not been tested on the Wemos and lightly tested on the NodeMCU.

  ---------------------------------------------------------------------------
  The CGM data comes from a Node-Red flow.  A 'poll state' node reads the sensor.yaml 'platform:rest' which gets my CGM data from
  https://my-g6.herokuapp.com/api/v1/entries/current.json.  My notes are here: 'Heroku APIs.txt'

  The instructions are here: https://community.home-assistant.io/t/dexcom-api/94730/5
  There are some details in the comments at the top of 'sensors.yaml'

  --------------------------------------------------------------------
  This is the API endpoint I am using in Home Assistant (sensors.yaml).
  https://my-g6.herokuapp.com/api/v1/entries/current.json

  It returns:
  [
     {
        "_id":"5e45b2b70ba41f5790624cc0",
        "sgv":173,
        "date":1581626007000,
        "dateString":"2020-02-13T20:33:27.000Z",
        "trend":4,
        "direction":"Flat",
        "device":"share2",
        "type":"sgv"
     }
  ]


*****/



// ****************************** Includes  ******************************
#include "ESP8266WiFi.h"
#include <PubSubClient.h>
#include <TimeLib.h>
//#include <Time.h>
#include <ArduinoOTA.h>
#include "D:\River Documents\Arduino\libraries\Kaywinnet.h"



#include <Ticker.h>
Ticker staleTicker;                   //Ticker object for the stale ticker flag.

#define DEBUG true                    //set to true for debug output, false for no debug ouput
#define Serial if(DEBUG)Serial

#include <Wire.h>                     // Wire.h is the library for communicating with I2C devices
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"



// ****************************** Globals  ******************************
#define NODENAME "cgm"
const char *connectName =  NODENAME "-officex";             //Must be unique
const char *statusTopic = NODENAME "/status";
const char *cmndTopic = NODENAME "/cmnd";
const char *bgTopic = NODENAME "/bg";
const char *trendTopic = NODENAME "/trend";
const char *dateTopic = NODENAME "/date";               // Sent as dateString from Node Red
const char *timeTopic = NODENAME "/time";               // Time from Node-Red, hh:mm every ten seconds.
const char *brightTopic = NODENAME "/bright";           // Allows the setting of the display brightness
//const char *alarmTopic = NODENAME "/alarm";
//const char *muteTopic = NODENAME "/mute";
//String lastTime = "";                                   //Last time displayed


// setup_wifi vars
char macBuffer[24];       // Holds the last three digits of the MAC, in hex.
char hostNamePrefix[] = hostPrefix;
char hostName[12];        // Holds hostNamePrefix + the last three bytes of the MAC address.


//---------------- GPIO ------------------------------
const int ledPin = D5;
//const int alarmPin = 13;       // D7
//const int netLedPin = D4;      // D4 (GPIO2 Built-In LED)

//const int muteButton = 0;      // D3
//const int brightButton = 14;   // D5
//const int dimButton = 2;       // D4
//        I2C SDA                // D2
//        I2C SCL                // D1



//---------------- Global Vars -------------------------

#include "segmentMap.h"               // Get the segment definitions 

int lastDim = HIGH;
int lastBright = HIGH;
byte tempI;
const bool ledOFF = LOW;
const bool ledON = HIGH;
unsigned long ledMillis;              // Used to time the LED on period
unsigned long ledTime = 250;          // How long the LED should be on. Resets with each incoming MQTT message.

int timeTimer;                        // Used to reboot if no new time value is received in timeTime ms.
unsigned long timeTimeout = 120;      // How long to wait for a time message before rebooting, in seconds.

int bgTimestamp = 0;                  // Used to detect if bg reading is not received in ten minutes
const int staleTime = 600;            // Number of seconds in ten minutes.
String sensorDate;
String lastSensorDate;                // The last sendor date.  If this is unchanged after ten minutes, then the data is stale.
bool staleFlag = false;               // Flag is set if no new sensor data is received in ten minutes
String alarm = "off";

byte brightness = 5;                  // Display brightness, initially dim






//----------------------------------- Initialize stuff ----------------------------------------
//
//************* Initialize the Alphanumeric displays *************
// Make two instances of the Adafruit_AlphaNum4 object
// There's no arguments or pins because the first display uses the fixed I2C pins and address 0x70
// The I2C address used in timeDisplay will be set with timeDisplay.begin
Adafruit_AlphaNum4 bgDisplay = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 timeDisplay = Adafruit_AlphaNum4();


// ************* Initialize the espClient ***********************
// Initializes the espClient. The espClient name must be unique
WiFiClient espCGM_officez;
PubSubClient client(espCGM_officez);








// **********************************  mqtt callback *************************************
// This function is executed when some device publishes a message to a topic that this ESP8266 is subscribed to.
//
void callback(String topic, byte * message, unsigned int length) {
  int digit1;
  int digit3;
  int intHour;
  char buf [2];
  digitalWrite(ledPin, ledON);          //Turn on LED and start timing it.
  ledMillis = millis();

  Serial.println();
  Serial.print(F("Message arrived on topic: "));
  Serial.println(topic);


  // Convert the character array to a string
  String messageString;
  for (int i = 0; i < length; i++) {
    messageString += (char)message[i];
  }
  messageString.trim();
  messageString.toUpperCase();          //Make the string upper-case


  Serial.print("messageString: ");
  Serial.print(messageString);
  Serial.println();
  //Serial.print(F("Length= "));
  //Serial.print(length);
  //Serial.println();




  if (topic == cmndTopic) {
    if( messageString=="REBOOT"){
      ESP.restart();
    }
  }


  if (topic == timeTopic) {
    //Time from Node Red is always 5 characters.
    //For example: "12:34" or "01:05"
    //Add the decimal point to the second digit since the display doesn't have a colon.
    switch (message[1])
    {
      case 48:
        digit1 = num0 + segDP;
        break;
      case 49:
        digit1 = num1 + segDP;
        break;
      case 50:
        digit1 = num2 + segDP;
        break;
      case 51:
        digit1 = num3 + segDP;
        break;
      case 52:
        digit1 = num4 + segDP;
        break;
      case 53:
        digit1 = num5 + segDP;
        break;
      case 54:
        digit1 = num6 + segDP;
        break;
      case 55:
        digit1 = num7 + segDP;
        break;
      case 56:
        digit1 = num8 + segDP;
        break;
      case 57:
        digit1 = num9 + segDP;
        break;
    }

    //Send the digits to timeDisplay
    timeDisplay.writeDigitAscii(0, message[0]);
    timeDisplay.writeDigitRaw(1, digit1);
    timeDisplay.writeDigitAscii(2, message[3]);
    timeDisplay.writeDigitAscii(3, message[4]);
    timeDisplay.writeDisplay();

    //Start the time timeout. If no time message arrives in timeTimeout seconds, then reboot the ESP.
    //This is different from the staleTimer that tracks only if no new BG data is received.
    timeTimer = now();

  }





  if (topic == brightTopic) {
    brightness = messageString.toInt();
    timeDisplay.setBrightness(brightness);
    bgDisplay.setBrightness(brightness);
  }


  if (topic == dateTopic) {
    sensorDate = messageString;
    Serial.print (F("Date= "));
    Serial.println(sensorDate);

    if (sensorDate != lastSensorDate) {     // New date?
      Serial.print(F("DEBUG "));
      Serial.print(F("lastSensorDate= "));
      Serial.println(lastSensorDate);
      Serial.print(F("SensorDate= "));
      Serial.println(sensorDate);

      lastSensorDate = sensorDate;          // Yes, save the new sensor date and
      staleFlag = false;                    // Clear the stale flag, if it's on.
      staleTicker.detach();                 // Restart the stale timer
      staleTicker.attach(600, staleTick);   // Call the staleTick function after ten minutes.
    }
  }


  if (topic == bgTopic) {
    //What we really want to do is display the msg on the HT16K33 I2C display.
    //Format the array, right-justified, leading spaces, but leaving the rightmost display character empty.
    //for trend up/down arrows.

    //Send the data to bgDisplay, digits 0,1 and 2.
    if (length < 2) {
      bgDisplay.writeDigitAscii(0, '?');
      bgDisplay.writeDigitAscii(1, '?');
      bgDisplay.writeDigitAscii(2, '?');
      bgDisplay.writeDisplay();
    }
    else if (length == 2) {
      bgDisplay.writeDigitAscii(0, ' ');
      bgDisplay.writeDigitAscii(1, message[0]);
      bgDisplay.writeDigitAscii(2, message[1]);
      bgDisplay.writeDisplay();
    }
    else if (length == 3) {
      bgDisplay.writeDigitAscii(0, message[0]);
      bgDisplay.writeDigitAscii(1, message[1]);
      bgDisplay.writeDigitAscii(2, message[2]);
      bgDisplay.writeDisplay();
    }
    else if (length > 3) {
      bgDisplay.writeDigitAscii(0, '?');
      bgDisplay.writeDigitAscii(1, '?');
      bgDisplay.writeDigitAscii(2, '?');
      bgDisplay.writeDisplay();
    }
    if (staleFlag) {                            //?? Needed here?  This is in loop.
      bgDisplay.writeDigitAscii(3, '*');
      bgDisplay.writeDisplay();
    }       //if staleflag

    bgTimestamp = now();                  // Used to flag if more than staleTime without a bg read has elapsed.
    // now() is giving you UNIX time in seconds.

  }         //if topic==bgtopic



  if (topic == trendTopic) {
    switch (messageString.toInt())
    {
      case 1:
        digit3 = segA + segJ + segM;                //Tall up arrow
        break;
      case 2:
        digit3 = segA + segJ;
        break;
      case 3:
        digit3 = segK;
        break;
      case 4:
        digit3 = segG2;
        break;
      case 5:
        digit3 = segN;
        break;
      case 6:
        digit3 = segM + segD;
        break;
      case 7:
        digit3 = segM + segD + segJ;                  //Tall down arrow
        break;
      default:
        //Should never get here
        digit3 = segA + segB + segG2 + segM;      //Question Mark
        Serial.print(F("Trend? "));
        Serial.print(messageString);

        Serial.print(F("digit3= "));
        Serial.println(digit3);
    }       //switch
    bgDisplay.writeDigitRaw(3, digit3);
    bgDisplay.writeDisplay();


    if (staleFlag) {                      //Override the last displayed state
      bgDisplay.writeDigitAscii(3, '*');  //?? Needed here? Already in loop
      bgDisplay.writeDisplay();
    }       //if staleflag
  }         //if topic==trendtopic
}           //callback






// *********************  Callback Function to display the "Stale Data" symbol *********************
void staleTick()
{
  staleFlag = true;
  Serial.println(F("Staleflag set"));
}






// **************************************** SETUP *************************************
void setup() {
  Serial.begin(115200);
  Serial.println(F("\n"));
  delay(10);
  Serial.println(F(sketchName));
  Serial.print(F("Version "));
  Serial.println(F(sketchVersion));
  Serial.println();



  //pinMode(netLedPin, OUTPUT);             // netLedPin is an indicator that a network connection is made.
  //digitalWrite(netLedPin, ledOFF);

  pinMode(ledPin, OUTPUT);                // LedPin is an indicator that an MQTT message was received.
  digitalWrite(ledPin, ledOFF);

  //pinMode(alarmPin, OUTPUT);              // AlarmPin is connected to the audible alarm.
  //digitalWrite(alarmPin, LOW);



  setup_wifi();
  start_OTA();                            // Coming

  client.setServer(mqtt_server, 1883);    //Server details for pubsub.
  client.setCallback(callback);


  Serial.println("...............");
  Serial.print(F("Node name= "));
  Serial.println(NODENAME);
  Serial.print(F("Unique MQTT Connect name = "));
  Serial.println(connectName);
  Serial.print(F("ststusTopic= "));
  Serial.println((String(statusTopic)));
  Serial.print(F("connectName= "));
  Serial.println((String(connectName)));
  Serial.print(F("bgTopic= "));
  Serial.println((String(bgTopic)));
  Serial.print(F("trendTopic= "));
  Serial.println((String(trendTopic)));
  Serial.print(F("dateTopic= "));
  Serial.println((String(dateTopic)));
  Serial.print(F("timeTopic= "));
  Serial.println((String(timeTopic)));
  Serial.println(F("..............."));



  bgDisplay.begin(0x70);                             // bgDisplay is on I2C address 70
  timeDisplay.begin(0x71);                           // timeDisplay is on I2C address 71
  bgDisplay.setBrightness(brightness);
  timeDisplay.setBrightness(brightness);


  digitalWrite(ledPin, ledON);
  //  lightAllSegments();       // Light up all segments
  //  showNumbers();            // Show the digits, 0-9
  //  showLetters();            // Show all letters.
  showVersion();                // Show the software Version number
  showMac();                    // Show the last two byte-pairs of the MAC address.
  delay(1000);
  timeDisplay.clear();
  timeDisplay.writeDisplay();
  bgDisplay.clear();
  bgDisplay.writeDisplay();
  digitalWrite(ledPin, ledOFF);

  timeTimer = now();         // Reboot if the time is not updated in timeTimeout seconds.
}



/* ***************************************** LOOP ***************************************** */
void loop() {
  ArduinoOTA.handle();

  //Re-connect the pubsub client to the mqtt broker
  if (!client.connected()) {
    reconnect();
  }
  if (!client.loop()) {
    //client.connect(connectName);
    client.connect(hostName);
  }

  if (millis() - ledMillis > ledTime) {     //If the led has been on for a while, turn it off.
    digitalWrite(ledPin, ledOFF);
  }


  /*
    Serial.print (now() - timeTimer);
    Serial.print(",  ");
    Serial.println(timeTimeout);
  */

  if (now() - timeTimer > timeTimeout) {    //If the time has not been updated, assume the worst.
    ESP.restart();
  }

  if (now() - bgTimestamp > staleTime) {    //If the BG reading is stale..
    bgDisplay.writeDigitAscii(3, '*');
    bgDisplay.writeDisplay();
  }


  //Now, we just wait for a callback



  // Reset the ESP8266 WDT timer.
  yield();
}
