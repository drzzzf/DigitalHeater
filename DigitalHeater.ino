#include <max6675.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>


int thermoDO = 4;
int thermoCS = 5;
int thermoCLK = 6;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
int vccPin = 9;
int gndPin = 8;
int heaterPin = 7;

int tempUpPin = 2;
int tempDownPin = 3;

LiquidCrystal_I2C lcd(0x27,20,4); // set the LCD address to 0x27 for a 16 chars and 2 line display

uint8_t degree[8]  = {140,146,146,140,128,128,128,128};

int targetTemp = 40;

int targetTempOld = 40;
int tempOld;

void setup()
{
  Serial.begin(9600);
  int val = EEPROM.get(0, val);
  if(val<0){
    EEPROM.put(0,targetTemp);
  }else{
    targetTemp=val;
  }
  Serial.println(val);

  lcd.init(); // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();
  lcd.createChar(0, degree);
  lcd.setCursor(0, 0);
  lcd.print("Now     Target");
    // go to line #1
  lcd.setCursor(0,1);
  lcd.print(strAddPrefixSpace(round1(thermocouple.readCelsius()),3));
  
  lcd.setCursor(3,1);
  lcd.write((byte)0);
  lcd.print("C ");

  lcd.setCursor(8,1);
  lcd.print(targetTemp);
  
  lcd.setCursor(11,1);
  lcd.write((byte)0);
  lcd.print('C');

  // use Arduino pins 
  pinMode(vccPin, OUTPUT); digitalWrite(vccPin, HIGH);
  pinMode(gndPin, OUTPUT); digitalWrite(gndPin, LOW);
  pinMode(heaterPin, OUTPUT); digitalWrite(heaterPin, LOW);

  pinMode(12, OUTPUT); digitalWrite(12, LOW);
  pinMode(10, OUTPUT); digitalWrite(10, HIGH);
  pinMode(11, OUTPUT); digitalWrite(11, LOW);

  
  pinMode(tempUpPin,INPUT_PULLUP);
  pinMode(tempDownPin,INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(tempUpPin), tempUp, FALLING);
  attachInterrupt(digitalPinToInterrupt(tempDownPin), tempDown, FALLING);

}
void loop()
{
  float fTemp=thermocouple.readCelsius();
  int tempNew=round1(thermocouple.readCelsius());
  if(tempNew!=tempOld){
    lcd.setCursor(0,1);
    lcd.print(strAddPrefixSpace(tempNew,3));
    tempOld=tempNew;
  }
  if(targetTemp!=targetTempOld){ 
    lcd.setCursor(8,1);
    lcd.print(strAddPrefixSpace(targetTemp,3));
    targetTempOld=targetTemp;
  }
  if(tempNew<targetTemp){
    digitalWrite(heaterPin, HIGH);
  }else{
    digitalWrite(heaterPin, LOW);
  }
  delay(200);
}

void tempUp()
{
  Serial.println("temp up");
  if(targetTemp>=250) {
    return;
  }
  targetTemp = targetTemp + 5;
  EEPROM.put(0,targetTemp);
}

void tempDown()
{
  Serial.println("temp down");
  if(targetTemp<=0) {
    return;
  }
  targetTemp = targetTemp - 5;
  EEPROM.put(0,targetTemp);
}

int round1(float d){
  int dst=(int)(d - 0.5);
  return dst;
}

String strAddPrefixSpace(int iData,int iLength){
  String strDst= String(iData);
  int iStrLength=strDst.length();
  while(iStrLength<iLength){
    strDst=" "+strDst;
    iStrLength++;
  }
  Serial.println(strDst);

  return strDst;
}
