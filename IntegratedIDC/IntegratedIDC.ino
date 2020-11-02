#include <Wire.h> // I2C library, required for MLX90614
#include <SparkFunMLX90614.h> // SparkFunMLX90614 Arduino library
#define Rx 17
#define Tx 16
#include <Servo.h>

Servo left;
Servo right;

int QTIpins[] = {47, 51, 52}; // left, center, right
int QTIdurations[3];
int QTIvalues[3];

int r = 45;
int b = 44;
int g = 46;

int threshold = 100;

// Tracks the number of hashes encountered so far.
int encountered = 0;

int bottleLocation = -1;

IRTherm therm;

// Threshold temperature for "cold" -- probably need to update before IDC.
float thresholdTemperature = 66.0;

// ALL BOT and NON-DINO variables.
int teamResults[4];
int minIndex = 0; 
char finalRoutine;
long startTime = millis();


void setup() {
  // Configure everything.
  configureSensing();
  configureCommunication();
  setUpQTI();
  lightSetup();
}

void loop() {
  // Stop after all 5 hash marks encountered.
  while(encountered < 5) {
    // Find the next hash mark.
    lineFollow();
    // Record that a hash mark has been found.
    encountered++;
    // Check object temperature.
    if(getTemperature() < thresholdTemperature){
      // Record bottle location.
      bottleLocation = encountered;
      // Update the LCD if object found.
      updateLCD();
      // Send location of bottle.
      sendCharacter((char) ('e'+encountered));
      teamResults[1] = encountered;
    }
    // Turn all LEDs (onboard and breadboard) off.
    resetLED();
    if(encountered == 5) {
      Serial3.write(12);
      Serial3.write(13);
      if(bottleLocation > -1) sendCharacter((char) ('e'+bottleLocation));
      Serial3.write("Waiting...");
      while(!receiveCharacter()) {}
      compute();
      Serial3.write(12);
      Serial3.write(13);
      if(minIndex == 0) {Serial3.write("1 is lowest.");}
      if(minIndex == 1) {Serial3.write("2 is lowest.");}
      if(minIndex == 2) {Serial3.write("3 is lowest.");}
      if(minIndex == 3) {Serial3.write("4 is lowest.");}
      while(!receiveFinalRoutine()) {}
      Serial3.write(12);
      Serial3.write(13);
      if(finalRoutine == 'x') {Serial3.write(("ESCAPE;"+String(bottleLocation)+";"+String(minIndex+1)).c_str());}
      if(finalRoutine == 'y') {Serial3.write(("DINO5;"+String(bottleLocation)+";"+String(minIndex+1)).c_str()); lightShow();}
      if(finalRoutine == 'z') {Serial3.write(("LS;"+String(bottleLocation)+";"+String(minIndex+1)).c_str()); lightShow();}
    }
  }
}

// Uses the integrated temperature sensor to determine bottle temp.
float getTemperature() {
  therm.read();
  return therm.object();
}

// Configures the temp sensor.
void configureSensing() {
  Serial3.begin(9600);
  therm.begin();
  therm.setUnit(TEMP_F);
  pinMode(2,OUTPUT);
  digitalWrite(2,LOW);
}

// Update the LCD after each temp measurement.
void updateLCD() {
  String topLine = (bottleLocation > 0) ? "COLD @ " + String(bottleLocation) : "NOT FOUND";
  Serial3.write(12);
  Serial3.write(13);
  Serial3.println(topLine);
}

// Send a character with the Xbee.
void sendCharacter(char c) {
  char outgoing = c;
  Serial2.write(outgoing);
  digitalWrite(45, LOW);
}

// Turns off onboard and breadboard LEDs.
void resetLED(){
  digitalWrite(2, LOW); 
  digitalWrite(45, HIGH);
  digitalWrite(46, HIGH);
}

// Configures LED and Xbee. 
void configureCommunication(){
  pinMode(45, OUTPUT);
  pinMode(46, OUTPUT);
  pinMode(5, OUTPUT);
  Serial2.begin(9600);
  Serial3.begin(9600);
}

// Configures servos.
void setUpQTI(){
  left.attach(11);
  right.attach(12);
}
  
// Method for checking QTI sensors.
void checkSensors() {
  for(int i = 0; i < sizeof(QTIpins)/2; i++) { 
    long duration = 0;                      
    pinMode(QTIpins[i], OUTPUT);                 
    digitalWrite(QTIpins[i], HIGH);           
    delay(1);                               
    pinMode(QTIpins[i], INPUT);              
    digitalWrite(QTIpins[i], LOW);             
    while(digitalRead(QTIpins[i])){
      // Wait for RC circuit to decay.            
      duration++;                          
    }
    // Convert read value to Boolean.
    QTIdurations[i] = duration;               
    QTIvalues[i] = (duration > threshold);   
  }
}    

// Finds the next hash.
void lineFollow(){
  left.attach(11);
  right.attach(12);
  left.write(100);
  right.write(85);
  delay(200);
  
  while(true) {
    checkSensors();
    // Turn handling: right turn.
    if (QTIvalues[0] and not QTIvalues[2]) { // if left
      left.write(80); // left backwards
      right.write(80); // right forwards
    }
    // Turn handling: left turn.
    else if (QTIvalues[2] and not QTIvalues[0]) {
      left.write(100); // left forwards
      right.write(100); // right backwards
    }
    // Stop handling: reached a hashmark. 
    else if (QTIvalues[0] and QTIvalues[1] and QTIvalues[2]) {
      left.detach();
      right.detach();
      delay(500);
      // Break out of line following.
      return;
    }
    // Nothing to do -- keep moving forward.
    else {
      left.write(100);
      right.write(85);
    } 
  }
}

// Needed for NON-DINO bots.
boolean receiveFinalRoutine() {
//  if(bottleLocation > -1) sendCharacter((char) ('e'+bottleLocation));
//  delay(50);
  if(Serial2.available()) {
    char incoming = Serial2.read();
    if(incoming >= 'x' && incoming <= 'z') {
      finalRoutine = incoming;
      return true;
    }
    return false;
  }
  return false;
}

// receiveCharacter needed only for ALL BOTS.
boolean receiveCharacter() {
  // Team 1: a-e
  // Team 2: f-j
  // Team 3: k-o
  // Team 4: p-t
  // Team 5: not needed since team 5 computes.

//  if(bottleLocation > -1) sendCharacter((char) ('e'+bottleLocation));
  if(millis()-startTime > 60000) return true;
//  delay(50);
  
  if(Serial2.available()){
    char incoming = Serial2.read();

    if(incoming >= 'a' && incoming <= 'e') {
      teamResults[0] = incoming-96;
    }
    if(incoming >= 'f' && incoming <='j') {
      teamResults[1] = incoming-101;
    }
    if(incoming >= 'k' && incoming <='o') {
      teamResults[2] = incoming-106;
    }
    if(incoming >= 'p' && incoming <='t') {
      teamResults[3] = incoming-111;
    }
    for(int i = 0; i < 4; i++) {
      // If some data not yet received, return false.
      if(teamResults[i] == 0) {return false;}
    }
    // If we have all data, return true;
    return true;
  }
  return false;
}

// compute() needed only for NON-DINO.
void compute() {
  for(int i = 0; i < 4; i++) {
    if(teamResults[i] < teamResults[minIndex] && teamResults[i] != 0) {
      minIndex = i;
    }
  }
}

// FINAL ROUTINE: LIGHT SHOW

void lightSetup() {
  pinMode(r,OUTPUT);
  pinMode(g,OUTPUT);
  pinMode(b,OUTPUT);
}

void bu(int timer){
  digitalWrite(r,HIGH);
  digitalWrite(g,HIGH);
  digitalWrite(b,LOW);
  delay(2*timer);
}

void rd(int timer){
  digitalWrite(r,LOW);
  digitalWrite(g,HIGH);
  digitalWrite(b,HIGH);
  delay(2*timer);
}

void gn(int timer){
  digitalWrite(r,HIGH);
  digitalWrite(g,LOW);
  digitalWrite(b,HIGH);
  delay(2*timer);
}

void wt(int timer){
  digitalWrite(r,LOW);
  digitalWrite(g,LOW);
  digitalWrite(b,LOW);
  delay(2*timer);
}

void off(int timer){
  digitalWrite(r,HIGH);
  digitalWrite(g,HIGH);
  digitalWrite(b,HIGH);
  delay(2*timer);
}

void actOne(){
  //about 5 secounds
  //Only team 1
  off(1500);
  //Teams 1,2; 2 wait 1500 before
  wt(250);
  off(250);
  wt(250);
  off(500);
  //teams 1,2,3 3 wait 1250 before
  wt(166);
  off(166);
  wt(166);
  off(166);
  wt(166);
  off(500);
  //teams 1,2,3,4, 4 wait 1330 before
  wt(125);
  off(125);
  wt(125);
  off(125);
  wt(125);
  off(125);
  wt(125);
  off(125);
}

void actTwo(){
  //about 5.5 secounds
  bu(1000);
  rd(500);
  gn(500);
  bu(2000);
  rd(500);
  gn(500); 
  bu(500);
}

void actThree(){
  //about 7 seconds
  rd(500);
  gn(500);
  rd(500);
  gn(500);
  wt(500);
  bu(500);
  wt(500);
  bu(500);
  rd(250);
  gn(250);
  rd(250);
  gn(250);
  wt(250);
  bu(250);
  wt(250);
  bu(250);
  rd(125);
  gn(125);
  rd(125);
  gn(125);
  wt(125);
  bu(125);
  wt(125);
  bu(125);
  off(125);
}

void lightShow() {
  actOne();
  actTwo();
  actThree();
}
