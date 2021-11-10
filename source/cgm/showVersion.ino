/********************************* showVersion() ********************************/
void showVersion() {
  // Shows the firmware version #.

  const char version[] = {VERSION};
  int versionChars = sizeof(version) / sizeof(version[0]);
  versionChars = versionChars - 1;                          // Remove one for the line end.
  
  Serial.println(F("__________________"));
  Serial.print(F("version[] = '"));
  Serial.print(version);
  Serial.println(F("'"));
  Serial.print(F("versionChars = "));
  Serial.println(versionChars);
  Serial.println(F("__________________"));




  timeDisplay.writeDigitRaw(0, letterV);
  timeDisplay.writeDigitRaw(1, letterE);
  timeDisplay.writeDigitRaw(2, letterR + segDP);
  timeDisplay.writeDigitRaw(3, 0x0);
  timeDisplay.writeDisplay();


  for (int i = 0; i < 4; i++) {
    if (version[i] == '.') {
      bgDisplay.writeDigitRaw(i, segDP);
    } else {
      bgDisplay.writeDigitAscii(i, version[i]);
    }
  }
  bgDisplay.writeDisplay();
  delay(1500);

}
