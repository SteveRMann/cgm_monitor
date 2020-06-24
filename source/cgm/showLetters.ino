// *********************************** showNumbers()*********************************
void showLetters() {
  int dlay = 1000;           // Time between numbers

  timeDisplay.writeDigitRaw(0, letterA);
  timeDisplay.writeDigitRaw(1, letterB);
  timeDisplay.writeDigitRaw(2, letterC);
  timeDisplay.writeDigitRaw(3, letterD);
  timeDisplay.writeDisplay();

  bgDisplay.writeDigitRaw(0, letterE);
  bgDisplay.writeDigitRaw(1, letterF);
  bgDisplay.writeDigitRaw(2, letterG);
  bgDisplay.writeDigitRaw(3, letterH);
  bgDisplay.writeDisplay();
  delay(dlay);

  timeDisplay.writeDigitRaw(0, letterI);
  timeDisplay.writeDigitRaw(1, letterJ);
  timeDisplay.writeDigitRaw(2, letterK);
  timeDisplay.writeDigitRaw(3, letterL);
  timeDisplay.writeDisplay();

  bgDisplay.writeDigitRaw(0, letterM);
  bgDisplay.writeDigitRaw(1, letterN);
  bgDisplay.writeDigitRaw(2, letterO);
  bgDisplay.writeDigitRaw(3, letterP);
  bgDisplay.writeDisplay();
  delay(dlay);

  timeDisplay.writeDigitRaw(0, letterQ);
  timeDisplay.writeDigitRaw(1, letterR);
  timeDisplay.writeDigitRaw(2, letterS);
  timeDisplay.writeDigitRaw(3, letterT);
  timeDisplay.writeDisplay();

  bgDisplay.writeDigitRaw(0, letterU);
  bgDisplay.writeDigitRaw(1, letterV);
  bgDisplay.writeDigitRaw(2, letterW);
  bgDisplay.writeDigitRaw(3, letterX);
  bgDisplay.writeDisplay();
  delay(dlay);

  timeDisplay.writeDigitRaw(0, letterY);
  timeDisplay.writeDigitRaw(1, letterZ);
  timeDisplay.writeDigitRaw(2, 0x0);              // To-do, add punctuation
  timeDisplay.writeDigitRaw(3, 0x0);
  timeDisplay.writeDisplay();

  bgDisplay.writeDigitRaw(0, 0x0);
  bgDisplay.writeDigitRaw(1, 0x0);
  bgDisplay.writeDigitRaw(2, 0x0);
  bgDisplay.writeDigitRaw(3, 0x0);
  bgDisplay.writeDisplay();
 
  delay(1500);
}
