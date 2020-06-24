/********************************* showMac() ********************************/
void showMac() {
  // Shows the last two byte-pairs of the MAC address.

  // macBuffer contains the last three numbers of the mac address.
  // "4C:11:AE:0D:83:86" becomes "0D8386" in macBuffer.
  // Since the display is four digits, we display macBuffer[2] to macBuffer[5].



  timeDisplay.writeDigitRaw(0, letterM);
  timeDisplay.writeDigitRaw(1, letterA);
  timeDisplay.writeDigitRaw(2, letterC + segDP);
  timeDisplay.writeDigitRaw(3, 0x0);
  timeDisplay.writeDisplay();

  for (int i = 2; i < 6; i++) {
    bgDisplay.writeDigitAscii(i - 2, macBuffer[i]);
  }
  bgDisplay.writeDisplay();
  delay(1500);

}
