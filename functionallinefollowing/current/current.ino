#include <Servo.h>

Servo left;
Servo right;

int QTIpins[] = {47, 51, 52}; // left, center, right
int QTIdurations[3];
int QTIvalues[3];

int threshold = 100;
int encountered = 0;

void setup() {
  left.attach(11);
  right.attach(12);
  Serial.begin(9600);
  Serial3.begin(9600);
}

void loop() {
  Serial3.println();
  Serial3.println();
  Serial3.println("Hashes: "+String(encountered)+"       ");
  Serial3.println("                ");
  checkSensors();

  // For threshold determination.
  for (int i = 0; i < 3; i++) {
    Serial.print(QTIvalues[i]);
    Serial.print(" ");
  }
  Serial.println();
  if( encountered < 5){
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
    // Still need to implement counting.
    else if (QTIvalues[0] and QTIvalues[1] and QTIvalues[2]) {
      left.detach();
      right.detach();
      delay(500);
      encountered++;
      encountered = encountered % 6;
      left.attach(11);
      right.attach(12);
      left.write(100);
      right.write(85);
      delay(200);
    }
    //
    else {
      left.write(100);
      right.write(85);
    }
  }
  else{ 
    left.detach();
    right.detach();
  }
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
