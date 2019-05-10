/* 
* Battery Tester
* 
* 2 Modes are available at booting.
*   - Capacity Test Mode
*   - Internal Resistance Test Mode
*  
* Uses Arduino Pro Mini 
* Uses Philips PCD8544 (Nokia 5110) Display (84*48 dots)
* Uses 2 Ohm power resister (10W) as shunt. Load can be added in series via screw terminal.
* Uses logic level N-channel MOSFET (IRLR3103) controls shunt.
* 
* Credits:
*   - Adam Welch              https://www.youtube.com/watch?v=qtws6VSIoYk
*   - Paul (learnelectronics) https://www.youtube.com/watch?v=W9bMmcnHi28&t=33s
* 
* Required Library 
*   - LCD5110_Graph.h         http://www.rinkydinkelectronics.com/library.php?id=47
*/

#include "LCD5110_Graph.h"
// Setup Nokia 5110 LCD
// pin 13   - Serial clock out (SCLK/CLK)
// pin 11   - Serial data out (DIN/MOSI/DATA)
// pin 10   - Data/Command select (D/C)
// pin RST  - LCD reset (RST)
// pin 12   - LCD chip select (CS/CE/SCE)
LCD5110 myGLCD(13, 11, 10, 7, 12);
extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];

#define gatePin 9
#define highPin A2
#define lowPin A3
 
boolean finished = false;
int printStart = 0;
int interval = 5000;    //Interval (ms) between measurements
 
float mAh = 0.0;
float shuntRes = 2.08;   // In Ohms - Shunt resistor resistance
float voltRef = 5.02;   // Reference voltage (probe your 5V pin) 
float current = 0.0;
float battVolt = 0.0;
float shuntVolt = 0.0;
float battLow = 2.6;
 
unsigned long previousMillis = 0;
unsigned long millisPassed = 0;
 
void setup() {
 
  Serial.begin(9600);
  Serial.println("Battery Tester v1.0");
  Serial.println("battVolt  current  mAh");
 
  pinMode(gatePin, OUTPUT);
  digitalWrite(gatePin, LOW);
 
  myGLCD.InitLCD(); //initialize LCD with contrast of 50
  myGLCD.setContrast(30);
  // Set default font size. 
  // TinyFont       4x6, 
  // SmallFont      6x8, 
  // MediumNumbers  12x16, 
  // BigNumbers     14x24
  myGLCD.setFont(SmallFont);
  myGLCD.clrScr();
  
  myGLCD.print("Battery", CENTER, 4);
  myGLCD.print("Capacity", CENTER, 16);
  myGLCD.print("Tester", CENTER, 28);
  myGLCD.invertText(true);
  myGLCD.print(" CHECKING... ", CENTER, 40);
  myGLCD.invertText(false);
  myGLCD.update(); 
  delay(3000);
  myGLCD.clrScr();

}
 
void loop() {
 
  battVolt = analogRead(highPin) * voltRef / 1024.0;
  shuntVolt = analogRead(lowPin) * voltRef / 1024.0;
   
  if(battVolt >= battLow && finished == false)
  {
      digitalWrite(gatePin, HIGH);
      millisPassed = millis() - previousMillis;
      current = (battVolt - shuntVolt) / shuntRes;
      mAh = mAh + (current * 1000.0) * (millisPassed / 3600000.0);
      previousMillis = millis();
 
      myGLCD.clrScr();
      myGLCD.print("Discharge", CENTER, 0);
      myGLCD.print("Voltage:", 0, 10);
      myGLCD.printNumF(battVolt, 2, 50, 10);
      myGLCD.print("v", 77, 10);
      myGLCD.print("Current:", 0, 20);
      myGLCD.printNumF(current, 2, 50, 20);
      myGLCD.print("a", 77, 20);
      myGLCD.printNumI(mAh, 30, 30);
      myGLCD.print("mAh", 65, 30);
      myGLCD.invertText(true);
      myGLCD.print(" Running ", CENTER, 40);
      myGLCD.invertText(false);
      myGLCD.update(); 

      Serial.print(battVolt);
      Serial.print("\t");
      Serial.print(current);
      Serial.print("\t");
      Serial.println(mAh);
       
      delay(interval);
  }
  if(battVolt < battLow)
  {
      digitalWrite(gatePin, LOW);
      
      finished = true;
       
      if(mAh < 10) {
        printStart = 40;
      }
      else if(mAh < 100) {
        printStart = 30;
      }
      else if(mAh <1000) {
        printStart = 24;
      }
      else if(mAh <10000) {
        printStart = 14;
      }
      else {
        printStart = 0;
      }
       
      myGLCD.clrScr();
      myGLCD.print("Discharge", CENTER, 0);
      myGLCD.print("Voltage:", 0, 10);
      myGLCD.printNumF(battVolt, 2, 50, 10);
      myGLCD.print("v", 77, 10);
      myGLCD.setFont(MediumNumbers);
      myGLCD.printNumI(mAh, printStart, 21);
      myGLCD.setFont(SmallFont);
      myGLCD.print("mAh", 65, 30);
      myGLCD.invertText(true);
      myGLCD.print(" Completed ", CENTER, 40);
      myGLCD.invertText(false);
      myGLCD.update(); 
       
      delay(interval * 2);
  }
}    
