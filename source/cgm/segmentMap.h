// ************************ Display Segments *********************************
// See the segment map at
// D:\River Documents\Arduino\Projects\CGM\14-segment display.png
// D:\River Documents\Arduino\Projects\CGM\LED Display Letters and Numerals.jpg
//

int segA = 1;              // Display Segment binary values
int segB = 2;
int segC = 4;
int segD = 8;
int segE = 16;
int segF = 32;
int segG1 = 64;
int segG2 = 128;
int segH = 256;
int segJ = 512;
int segK = 1024;
int segL = 2048;
int segM = 4096;
int segN = 8192;
int segDP = 16384;

//Numbers
int num0 = segA + segB + segC + segD + segE + segF + segK + segL;
int num1 = segB + segC;
int num2 = segA + segB + segG1 + segG2 + segE + segD;
int num3 = segA + segB + segC + segD + segG2;
int num4 = segF + segB + segC + segG1 + segG2;
int num5 = segA + segF + segG1 + segN + segD;
int num6 = segA + segF + segE + segD + segC + segG1 + segG2;
int num7 = segA + segB + segC;
int num8 = segA + segB + segC + segD + segE + segF + segG1 + segG2;
int num9 = segA + segB + segC + segD + segF + segG1 + segG2;
int numbers[]={num0,num1,num2,num3,num4,num5,num6,num7,num8,num9};


//Letters
int letterA = segA + segB + segC + segE + segF + segG1 + segG2;
int letterB = segA + segB + segC + segD + segJ + segM + segG2;
int letterC = segA + segF + segE + segD;
int letterD = segA + segB + segC + segD + segJ + segM;
int letterE = segA + segF + segE + segD + segG1;
int letterF = segA + segF + segE + segG1;
int letterG = segA + segF + segE + segD + segC + segG2;
int letterH = segF + segE + segB + segC + segG1 + segG2;
int letterI = segA + segD + segJ + segM;
int letterJ = segB + segC + segD + segE;
int letterK = segF + segE + segG1 + segK + segN;
int letterL = segF + segE + segD;
int letterM = segE + segF + segA + segB + segC + segH + segK;
int letterN = segE + segF + segH + segN + segB + segC;
int letterO = segA + segB + segC + segD + segE + segF;
int letterP = segA + segB + segG1 + segG2 + segE + segF;
int letterQ = segA + segB + segC + segD + segE + segF + segN;
int letterR = segA + segB + segG1 + segG2 + segE + segF + segN;
int letterS = segA + segF + segG1 + segG2 + segC + segD;
int letterT = segA + segJ + segM;
int letterU = segB + segC + segD + segE + segF;
int letterV = segF + segE + segL + segK;
int letterW = segF + segE + segB + segC + segL + segN;
int letterX = segH + segK + segL + segN;
int letterY = segH + segK + segM;
int letterZ = segA + segD + segK + segL;
