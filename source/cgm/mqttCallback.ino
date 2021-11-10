// **********************************  mqtt callback *************************************
// This function is executed when some device publishes a message to a topic that this ESP8266 is subscribed to.
//
void callback(String topic, byte * message, unsigned int length) {
  int digit1 = 0;
  int digit3;

  digitalWrite(ledPin, ledON);          //Turn on LED and start timing it.
  ledMillis = millis();

  Serial.println();
  Serial.print(F("Message arrived on topic: "));
  Serial.println(topic);


  // Convert the character array to a string
  String messageString;
  for (uint16_t i = 0; i < length; i++) {
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
    if ( messageString == "REBOOT") {
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

    // Clear staleFlag here???

    //Start the time timeout. If no time message arrives in crashTimeout seconds, then reboot the ESP.
    //This is different from the staleTimer that tracks only if no new BG data is received.
    crashTimer = now();

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

    // Reset the stale data timer
    staleFlag = false;                    // Clear the stale flag, if it's on.
    staleTicker.detach();                 // Restart the stale timer
    staleTicker.attach(600, staleTick);   // Call the staleTick function after ten minutes.

  }         //if topic==bgtopic



  if (topic == trendTopic) {
    Serial.print(F("trendTopic:messageString= "));
    Serial.println(messageString);
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


    ///if (staleFlag) {                      //Override the last displayed state
    ///bgDisplay.writeDigitAscii(3, '*');  //?? Needed here? Already in loop
    ///bgDisplay.writeDisplay();
    ///}       //if staleflag

  }         //if topic==trendtopic
}           //callback
