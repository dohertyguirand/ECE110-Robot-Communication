#include <Servo.h>

Servo left;
Servo right;

int QTIpins[] = {47, 51, 52}; // left, center, right
int QTIdurations[3];
int QTIvalues[3];

int threshold = 100;

void setup() {
  left.attach(11);
  right.attach(12);
  Serial.begin(9600);
}

void loop() {
  // if left black, turn left
  // if right black, turn right

  checkSensors();
  for (int i = 0; i < 3; i++) {
    Serial.print(QTIvalues[i]);
    Serial.print(" ");
  }
  Serial.println();

  if (QTIvalues[0] and not QTIvalues[2]) { // if left
    left.write(80); // left backwards
    right.write(80); // right forwards
  }
  else if (QTIvalues[2] and not QTIvalues[0]) {
    left.write(100); // left forwards
    right.write(100); // right backwards
  }
  else if (QTIvalues[0] and QTIvalues[1] and QTIvalues[2]) {
    left.detach();
    right.detach();
    delay(1000);
    left.attach(11);
    right.attach(12);
    while (QTIvalues[0] and QTIvalues[1] and QTIvalues[2]) {
      left.write(100);
      right.write(85);
      checkSensors();
    }
  }
  else {
    left.write(100);
    right.write(85);
  }
}

/* 
 * INPUT:  Reads the QTIvalues from the QTI sensors stored in the array QTIpins[]
 * OUTPUT: A 0 or 1 in array QTIvalues[] depending on if the sensor reads a dark or light objects
 * In order to calibrate what is dark and what is light, use the threshold variable above
 * 
 * From: https://github.com/erisawesome/QTI-Sensor-Demo/blob/master/QTI-Sensor-Demo.ino
 */
void checkSensors() {
  for(int i = 0; i < sizeof(QTIpins)/2; i++) { 
    long duration = 0;                      //
    pinMode(QTIpins[i], OUTPUT);               // code that actually    
    digitalWrite(QTIpins[i], HIGH);            // reads from the sensor 
    delay(1);                               // 
    pinMode(QTIpins[i], INPUT);                // low QTIvalues from the sensor
    digitalWrite(QTIpins[i], LOW);             // are dark objects, high
    while(digitalRead(QTIpins[i])){            // QTIvalues are light objects
      duration++;                           //
    }                                       
    
    QTIdurations[i] = duration;                // store QTIvalues in arrays
    QTIvalues[i] = (duration > threshold);     // 
  }
}
