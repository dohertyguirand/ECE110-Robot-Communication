#include <Wire.h> // I2C library, required for MLX90614
#include <SparkFunMLX90614.h> // SparkFunMLX90614 Arduino library
#define Rx 17
#define Tx 16


IRTherm therm;
float thresholdTemperature = 66.0;

void setup() 
{
  Serial.begin(9600);
  configureSensing();
  configureCommunication();
}

void loop() {
  receiveCharacter();
  updateLCD();
  if(getTemperature() < thresholdTemperature){
    digitalWrite(2,HIGH);
    sendCharacter('O');
  }
  delay(500);
  digitalWrite(2,LOW);
  resetLED();
}

float getTemperature() {
  therm.read();
  return therm.object();
}

void configureSensing() {
  Serial3.begin(9600);
  therm.begin();
  therm.setUnit(TEMP_F);
  pinMode(2,OUTPUT);
  digitalWrite(2,LOW);
}

void updateLCD() {
  float temperature = getTemperature();
  String topLine = (String(temperature)+"F");
  String bottomLine = (temperature) < thresholdTemperature ? "COLD" : "NOT COLD";
  Serial3.write(12);
  Serial3.println(topLine);
  Serial3.write(13);
  Serial3.println(bottomLine);
}

void sendCharacter(char c) {
  char outgoing = c;
    Serial2.write(outgoing);
    digitalWrite(45, LOW);
    delay(500);
}

char receiveCharacter() {
  if(Serial2.available()){
    char incoming = Serial2.read();
    if(incoming == '5'){
      digitalWrite(46, LOW);
      delay(500);
      return incoming;
    }
  }
  return '0';
}

void resetLED(){ 
  digitalWrite(45, HIGH);
  digitalWrite(46, HIGH);
}

void configureCommunication(){
  pinMode(45, OUTPUT);
  pinMode(46, OUTPUT);
  pinMode(5, OUTPUT);
  Serial2.begin(9600);
  Serial.begin(9600);
}


