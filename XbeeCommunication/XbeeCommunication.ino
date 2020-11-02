#define Rx 17
#define Tx 16

void setup() {
  // Use onboard LED instead of LEDs in breadboard.
  pinMode(45, OUTPUT);
  pinMode(46, OUTPUT);
  // use pin 4 to detect whether push button pressed.
  pinMode(4, INPUT);
  Serial2.begin(9600);
  Serial.begin(9600);
  delay(500);
}

void loop() {
  // Check for incoming characters.
  if(Serial2.available()){
    char incoming = Serial2.read();
    // Filter incoming characters - use only '5's.
    if( incoming == '5'){
      // Turn the LED red for incoming 5.
      digitalWrite(45, LOW);
      Serial.println(incoming);
      // Wait 500ms so it's clear that a message has been received.
      delay(500);
    }
  }
  // Send a 5 if push button pressed.
  if(digitalRead(4)){
    char outgoing = '5';
    Serial2.write(outgoing);
    // Turn LED green for outgoing message.
    digitalWrite(46,LOW);
    Serial.println(outgoing);
    // Wait 500ms to avoid sending a duplicate message.
    delay(500);
  }
// Turn LED off.
digitalWrite(45, HIGH);
digitalWrite(46, HIGH);
}
