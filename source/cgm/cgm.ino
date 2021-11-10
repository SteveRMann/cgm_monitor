#define SKETCH "cgm.ino"
#define VERSION "3.20"           // Four characters
#define hostPrefix "CGM-"        // Six characters max

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
  Version 3.10
    06/24/2020 - Added topic: cgm/cmnd, message: REBOOT to reboot all nodes.


**PROBLEMS**
  I am using millis to test for stale data timeout, but the code also includes the ticker.h library.
  Which is more useful?

  IDE settings:
    NodeMCU 1.0 (ESP-12E Module), 4M, 1M SPIFFS
    Wemos D1 R1 Mini

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
#include "Kaywinnet.h"



#include <Ticker.h>
Ticker staleTicker;                   //Ticker object for the stale ticker flag.

#define DEBUG true                    //set to true for debug output, false for no debug ouput
#define Serial if(DEBUG)Serial

#include <Wire.h>                     // Wire.h is the library for communicating with I2C devices
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"



// ****************************** Globals  ******************************
#define NODENAME "cgm"
const char *connectName =  NODENAME "-workshop";             //Must be unique
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
//const int alarmPin = 13;       // D7 It's on the PCB, but not used.
//const int netLedPin = D4;      // D4 (GPIO2 Built-In LED)



//---------------- Global Vars -------------------------

#include "segmentMap.h"               // Get the segment definitions 

int lastDim = HIGH;
int lastBright = HIGH;
byte tempI;
const bool ledOFF = LOW;
const bool ledON = HIGH;
unsigned long ledMillis;              // Used to time the LED on period
unsigned long ledTime = 250;          // How long the LED should be on. Resets with each incoming MQTT message.

int crashTimer;                       // Used to reboot if no new time value is received in timeTime ms.
unsigned long crashTimeout = 120;     // How long to wait for new data before rebooting, in seconds.

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
WiFiClient espCGM_kitchen;
PubSubClient client(espCGM_kitchen);

/*
   /MAC Addresses:
   D010E9  Kitchen
   5B668A  Office
   ADA75B  MBR
   0C6E00  FamilyRoom
   69789C  Workshop
*/


// *********************  Callback Function to display the "Stale Data" symbol *********************
void staleTick()
{
  staleFlag = true;
  Serial.println(F("Staleflag set"));
}



// **************************************** SETUP *************************************
void setup() {
  beginSerial();

  //pinMode(netLedPin, OUTPUT);             // netLedPin is an indicator that a network connection is made.
  //digitalWrite(netLedPin, ledOFF);

  pinMode(ledPin, OUTPUT);                  // LedPin is an indicator that an MQTT message was received.
  digitalWrite(ledPin, ledOFF);

  //pinMode(alarmPin, OUTPUT);              // AlarmPin is connected to the audible alarm.
  //digitalWrite(alarmPin, LOW);

  setup_wifi();
  start_OTA();                            

  client.setServer(MQTT_SERVER, 1883);    //Server details for pubsub.
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

  crashTimer = now();          // Reboot if the data is not updated in crashTimeout seconds.
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

  if (now() - crashTimer > crashTimeout) {    //If the time has not been updated, assume the worst.
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
