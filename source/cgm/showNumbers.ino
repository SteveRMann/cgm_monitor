// *********************************** showNumbers()*********************************
void showNumbers() {
  int dlay = 1000;           // Time between numbers


  timeDisplay.writeDigitRaw(0, num0);
  timeDisplay.writeDigitRaw(1, num1);
  timeDisplay.writeDigitRaw(2, num2);
  timeDisplay.writeDigitRaw(3, num3);
  timeDisplay.writeDisplay();
  bgDisplay.writeDigitRaw(0, num4);
  bgDisplay.writeDigitRaw(1, num5);
  bgDisplay.writeDigitRaw(2, num6);
  bgDisplay.writeDigitRaw(3, num7);
  bgDisplay.writeDisplay();
  delay(dlay);
  timeDisplay.writeDigitRaw(0, num8);
  timeDisplay.writeDigitRaw(1, num9);
  timeDisplay.writeDigitRaw(2, 0x0);
  timeDisplay.writeDigitRaw(3, 0x0);
  timeDisplay.writeDisplay();
  bgDisplay.writeDigitRaw(0, 0x0);
  bgDisplay.writeDigitRaw(1, 0x0);
  bgDisplay.writeDigitRaw(2, 0x0);
  bgDisplay.writeDigitRaw(3, 0x0);
  bgDisplay.writeDisplay();
  delay(1000);
}
