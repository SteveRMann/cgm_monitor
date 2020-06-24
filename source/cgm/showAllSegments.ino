void lightAllSegments() {
  // Light up all digits, one at a time.  Also has the effect of clearing the display buffers.

    // timeDisplay
  timeDisplay.writeDigitRaw(3, 0x0);
  timeDisplay.writeDigitRaw(0, 0xFFFF);
  timeDisplay.writeDisplay();
  delay(200);
  timeDisplay.writeDigitRaw(0, 0x0);
  timeDisplay.writeDigitRaw(1, 0xFFFF);
  timeDisplay.writeDisplay();
  delay(200);
  timeDisplay.writeDigitRaw(1, 0x0);
  timeDisplay.writeDigitRaw(2, 0xFFFF);
  timeDisplay.writeDisplay();
  delay(200);
  timeDisplay.writeDigitRaw(2, 0x0);
  timeDisplay.writeDigitRaw(3, 0xFFFF);
  timeDisplay.writeDisplay();
  delay(500);
  timeDisplay.clear();
  timeDisplay.writeDisplay();
  delay(100);
  
  // bgDisplay
  bgDisplay.writeDigitRaw(3, 0x0);
  bgDisplay.writeDigitRaw(0, 0xFFFF);
  bgDisplay.writeDisplay();
  delay(200);
  bgDisplay.writeDigitRaw(0, 0x0);
  bgDisplay.writeDigitRaw(1, 0xFFFF);
  bgDisplay.writeDisplay();
  delay(200);
  bgDisplay.writeDigitRaw(1, 0x0);
  bgDisplay.writeDigitRaw(2, 0xFFFF);
  bgDisplay.writeDisplay();
  delay(200);
  bgDisplay.writeDigitRaw(2, 0x0);
  bgDisplay.writeDigitRaw(3, 0xFFFF);
  bgDisplay.writeDisplay();
  delay(500);
  bgDisplay.clear();
  bgDisplay.writeDisplay();
  
  delay(1000);


}
