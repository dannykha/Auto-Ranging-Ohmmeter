/*************************************
EE 425 
Ohmmeter Project Code
Professor Arnie Berger and Professor Joe Decuir
Code by Danny Kha and Max Kelton
***************/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
#include <Adafruit_ADS1X15.h>
Adafruit_ADS1115 ads;

volatile double resistorRes; // unknown resistor
volatile double currCurrent; // current current that the current circuit is set to

const double openCircuit = 1300000;
const int noBat = -1;
const int displayRate = 300; // Setting display rate
volatile double rangeLow;
volatile double rangeHigh;
volatile int lastSample; // value of the last sample
volatile int lastDisplay; // value of the last time dispalyed
volatile int currRange;

int res750 = 3; // current 1 (0.01 A) range 1
int res7p5k = 4; // current 2 (0.001 A) range 2
int res75k = 5; // current 3 (0.0001 A) range 3
int res750k = 6; // current 4 (0.00001 A) range 4
int res7p5M = 7; // current 5 (0.000001 A) range 5
int PGA0 = 12; // for setting gain of PGA
int PGA1 = 13; // for setting gain of PGA

// setup method
void setup(void)
{
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  ads.begin(0x48);

  if (!ads.begin()) {
    lcd.setCursor(5, 0);
    lcd.print("NO ADC");
  }

  // Setting out the pins
  pinMode(res750, OUTPUT);
  pinMode(res7p5k, OUTPUT);
  pinMode(res75k, OUTPUT);
  pinMode(res750k, OUTPUT);
  pinMode(res7p5M, OUTPUT);
  pinMode(PGA0, OUTPUT);
  pinMode(PGA1, OUTPUT);

  // Gain setters
  digitalWrite(PGA0, LOW);
  digitalWrite(PGA1, LOW);
  ads.setGain(GAIN_TWO);
  
  currRange = 5; // initially set to the lowest current setting
  currentSwitch(currRange);
  lastSample = 0;
  lastDisplay = 0;
}

// main loop
void loop(void)
{
  int currTime = millis();
    lastSample = currTime;
    measureAndCalc();
    if (resistorRes < rangeLow) {  // if the resistor is found to be lower then go down
      if (currRange > 1)
      {
        currRange--;
        currentSwitch(currRange);
      } 
    }
    else if (resistorRes > rangeHigh) { // if the resistor is found to be higher then go up
      if (currRange < 5) {
        currRange++;
        currentSwitch(currRange);
      }
    }
  if ((currTime - lastDisplay) >= displayRate) // display timing
  {
    lastDisplay = currTime;
    displayMeasurement();
  }
}

// display the measurements
void displayMeasurement() {
  lcd.clear();
  lcd.setCursor(0,0);
  if (resistorRes > openCircuit) 
  {
    lcd.setCursor(7,0);
    lcd.print("OL");
  }
  else
   {
    lcd.print("   Ohms:   ");
    lcd.setCursor(3,1);
     if (resistorRes >= 500000) {
      resistorRes *= 1.013;
    }
    lcd.print(resistorRes,7);
  }
}

// measuring the voltage and then calculating resistor result value
void measureAndCalc()
{
  int16_t readADC;
  double voltageReading;
  readADC = ads.readADC_SingleEnded(0);
  voltageReading = ads.computeVolts(readADC); // find the voltage!!
  resistorRes = voltageReading / currCurrent; // calculate the resistance
}

// switching the current
void currentSwitch(int rangeVal)
{
  switch(rangeVal) {
    case 1:
      current1();
      break;
    case 2:
      current2();
      break;
    case 3:
      current3();
      break;
    case 4:
      current4();
      break;
    case 5:
      current5();
      break;
  }
}

// current 1, range 1, at 750 ohm, 0.01 A
void current1() 
{
  digitalWrite(res750, HIGH);
  digitalWrite(res7p5k, LOW);
  digitalWrite(res75k, LOW);
  digitalWrite(res750k, LOW);
  digitalWrite(res7p5M, LOW);
  rangeLow = 1;
  rangeHigh = 130;
  currCurrent = 0.01;
}

// current 2, range 2, at 7.5k ohm, 0.001 A
void current2() 
{
  digitalWrite(res750, LOW);
  digitalWrite(res7p5k, HIGH);
  digitalWrite(res75k, LOW);
  digitalWrite(res750k, LOW);
  digitalWrite(res7p5M, LOW);
  rangeLow = 130;
  rangeHigh = 1300;
  currCurrent = 0.001;
}

// current 3, range 3, at 75k ohm, 0.0001 A
void current3() 
{
  digitalWrite(res750, LOW);
  digitalWrite(res7p5k, LOW);
  digitalWrite(res75k, HIGH);
  digitalWrite(res750k, LOW);
  digitalWrite(res7p5M, LOW);
  rangeLow = 1300;
  rangeHigh = 13000;
  currCurrent = 0.0001;
}

// current 4, range 4, at 750k ohm, 0.00001 A
void current4() 
{
  digitalWrite(res750, LOW);
  digitalWrite(res7p5k, LOW);
  digitalWrite(res75k, LOW);
  digitalWrite(res750k, HIGH);
  digitalWrite(res7p5M, LOW);
  rangeLow = 13000;
  rangeHigh = 130000;
  currCurrent = 0.00001;
}

// current 5, range 5, at 7.5M ohm, 0.000001 A
void current5() 
{
  digitalWrite(res750, LOW);
  digitalWrite(res7p5k, LOW);
  digitalWrite(res75k, LOW);
  digitalWrite(res750k, LOW);
  digitalWrite(res7p5M, HIGH);
  rangeLow = 130000;
  rangeHigh = 1300000;
  currCurrent = 0.000001;
}