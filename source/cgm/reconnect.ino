//************************** reconnect to MQTT broker ********************************
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print(F("Attempting MQTT connection..."));

    // Attempt to connect
    //if (client.connect(connectName))
    if (client.connect(hostName))
    {
      Serial.println(F("connected"));

      // Subscribe or resubscribe to a topic
      client.subscribe(cmndTopic);
      Serial.print(F("Subscribing to "));
      Serial.println(cmndTopic);

      client.subscribe(bgTopic);
      Serial.print(F("Subscribing to "));
      Serial.println(bgTopic);

      client.subscribe(trendTopic);
      Serial.print(F("Subscribing to "));
      Serial.println(trendTopic);

      client.subscribe(dateTopic);
      Serial.print(F("Subscribing to "));
      Serial.println(dateTopic);

      client.subscribe(timeTopic);
      Serial.print(F("Subscribing to "));
      Serial.println(timeTopic);

      client.subscribe(brightTopic);
      Serial.print(F("Subscribing to "));
      Serial.println(brightTopic);

      //client.subscribe(alarmTopic);
      //Serial.print(F("Subscribing to "));
      //Serial.println(alarmTopic);

      Serial.println(F("====================="));

    } else {
      //digitalWrite(ledPin, ledOFF);
      Serial.print(F("failed, rc="));
      Serial.print(String(client.state()));
      Serial.println(F(" try again in 5 seconds"));
      delay(5000);

      /* Possible values for client.state()
         #define MQTT_CONNECTION_TIMEOUT     -4
         #define MQTT_CONNECTION_LOST        -3
         #define MQTT_CONNECT_FAILED         -2
         #define MQTT_DISCONNECTED           -1
         #define MQTT_CONNECTED               0
         #define MQTT_CONNECT_BAD_PROTOCOL    1
         #define MQTT_CONNECT_BAD_CLIENT_ID   2
         #define MQTT_CONNECT_UNAVAILABLE     3
         #define MQTT_CONNECT_BAD_CREDENTIALS 4
         #define MQTT_CONNECT_UNAUTHORIZED    5
      */
    }
  }
}
