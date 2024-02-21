#include <Servo.h>
#include <Wire.h>
#include <RtcDS3231.h>

#define sensorPower 8
#define sensorPin A0
#define pump 9

#define redPin 2 
#define greenPin 3
#define bluePin 4

RtcDS3231<TwoWire> Rtc(Wire);
Servo myServo;

int wet = 370;
int dry = 600;

int pos;
float temp;

void setup() {
  // put your setup code here, to run once:
  pinMode(sensorPower, OUTPUT);
  pinMode(pump, OUTPUT);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // Initially keep the sensor OFF
  digitalWrite(sensorPower, LOW);

  myServo.attach(6);
  myServo.write(90);
  
  Wire.begin();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  updateSoil();
  updatePump();
  updateServo();
}

//Början av funktioner kopplade till vattenpump och jordfuktssensor

int readSensor() { // Denna funktion sätter igång jordfuktssensor och läser av värdet. Sedan returnerar den värdet med variabeln 'val' efter den har avaktiverats.
  digitalWrite(sensorPower, HIGH);  // Slå på jordfuktssensorn
  delay(500);              // Väntar en halv sekund
  int val = analogRead(sensorPin); // Läser av analoga värdet från jordfuktssensorn och kallar den för 'val'
  digitalWrite(sensorPower, LOW);   // Släcker jordfuktssensorn
  return val;             // Returnerar det analoga fuktighetsvärdet
}

void updateSoil(){ // Denna funktion använder variabeln 'val' för att visa värdet i seriell monitorn och if-villkor för att få en förståelse över jordfuktigheten. 
  int val = readSensor();
  Serial.println();
  Serial.print("Digital Output: ");
  Serial.println(val);

  /*
   * if-villkoren nedan använder värdet 'val' och gränserna 'wet' och 'dry' för att ändra färgen på RGB lysdioden.
   * Om 'val' är högre än 'dry' ska lysdioden visa färgen röd och statuset visas i seriell monitorn.
   * Om 'val' är lägre än 'wet' ska lysdioden visa färgen gul och statuset visas.
   * Annars ska lysdioden visa färgen grön.
   * Hela funktionen updateSoil() ska upprepas varje halv sekund.
   */
  
  if (val > dry) {
   Serial.println("Status: Soil is too dry - activating pump");
   setColor(255, 0, 0);
  } 
  else if (val < wet) {
    Serial.println("Status: Soil moisture is too wet - activating servo");
    setColor(255, 255, 0);
  }
  else {
    Serial.println("Status: Soil moisture is perfect");
    setColor(0, 255, 0);
  }
  delay(500);
  Serial.println();
}

void updatePump(){
  int val = readSensor();
  if (val > dry ){
    digitalWrite(pump, HIGH);
  }
  else {
    digitalWrite(pump, LOW);
  }
}
//Slutet av dessa funktioner


//början av funktioner för Servot och RTC-modulen

/* 
 * readRTC()funktionen hämtar temperaturen från RTC-modulen och retunerar den i celsius med variabelnamnet 'temp'.
 * Funktionen visas även i seriell monitorn.
 * int användes för att det ska vara möjligt att använda funktionen i en annan
 */

int readRTC(){
  RtcTemperature rtcTemp = Rtc.GetTemperature();
  temp = rtcTemp.AsFloatDegC(); // tilldelar
  Serial.print("temperature C: ");
  Serial.println(temp); //Visar temperatur i seriell-monitor
  return temp;
}

/*
 * updateServo()funktionen använder sig av updateRTC()för att tilldela temperaturen till variabelnamnet 'temp'.
 * Om 'temp' är högre än 24.0 i detta fall ska servots arm flyttas till position 0.
 * Om 'temp' är lägre än 24.0 kommer servot att retunera till den ursprungliga positionen.
 * Om servot är igång eller inte skrivs ner i seriell monitorn.
 * Funktionen upprepas varje sekund.
 */

void updateServo(){
  temp = readRTC();
  myServo.write(pos);
  
  if (temp < 24.0){
    pos = 90 ;
    Serial.print("Enclosure temperature is good - Servo OFF");
  }
  else {
    pos = 0;
    Serial.print("Enclosure is too hot - Servo ON");
  }
    delay(1000);
}
//Slutet av dessa funktioner

/*
 * setColor() funktionen används för färgen som ska visas på RGB lysdioden.
 * Detta är en input-funktion eftersom det behövs olika värden för att få funktionen att funka. 
 * I detta fall är värdena till för RGB kod mellan 0-255.
 */

int setColor(int R, int G, int B){
  analogWrite(redPin, R);
  analogWrite(greenPin, G);
  analogWrite(bluePin, B);
  
}