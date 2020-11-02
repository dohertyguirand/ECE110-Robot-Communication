#define Rx 17
#define Tx 16

/*
 * David Coffman
 * Nov 13, 2019
 * 
 * This code is for team communication. Make sure to read the comments :) 
 */

// ALL BOT variables.
int teamResults[4];
int minIndex = 0; 
// End ALL BOT variables.

// DINO variables.
int dinoResult;
// End DINO variables.

// NON-DINO variables.
char finalRoutine;
// End NON-DINO variables.

void setup() {
  // Need to make sure Xbee and LCD are configured or code won't work.
  // Feel free to configure however you want.
  Serial3.begin(9600);
}

// See comments inside the method -> don't copy/paste the whole thing.
void loop() {
  /*
   * Only put this on DINO. Put it AFTER your sensing/line following code.
   */
   // Set dinoResult to your group's result.
  while(!receiveCharacter()){}
  char outgoing = dinoCompute();
  for(int i = 0; i < 10; i++) {
    sendCharacter(outgoing);
  }
  // End of dino-only code.

  /*
  * Only put this on NON-DINO. Put it AFTER your sensing/line following AND character sending code.
  * 
  * After this section of the code runs, your bot will (a) know which final routine, 
  * which will be stored in the char finalRoutine variable, and (b) have displayed 
  * the bot # of the team with the minimum score.
  */
  while(!receiveCharacter()) {}
  compute();
  if(minIndex == 0) {Serial3.write("1 is lowest.");}
  if(minIndex == 1) {Serial3.write("2 is lowest.");}
  if(minIndex == 2) {Serial3.write("3 is lowest.");}
  if(minIndex == 3) {Serial3.write("4 is lowest.");}
  while(!receiveFinalRoutine()) {}
  // End of non-dino code.  
}

// Needed for NON-DINO bots.
boolean receiveFinalRoutine() {
  if(Serial2.available()) {
    char incoming = Serial2.read();
    if(incoming >= 'X' && incoming <= 'Z') {
      finalRoutine = incoming;
      return true;
    }
    return false;
  }
  return false;
}

// Configure communication however you want.
// Needed for ALL bots.
void configureCommunication(){
  pinMode(45, OUTPUT);
  pinMode(46, OUTPUT);
  pinMode(5, OUTPUT);
  Serial2.begin(9600);
  Serial.begin(9600);
}

// Use your own sendCharacter implementation if you want -- just needs to send a character.
// Needed for ALL bots.
void sendCharacter(char c) {
  char outgoing = c;
  Serial2.write(outgoing);
  digitalWrite(45, LOW);
  delay(50);
}

// receiveCharacter needed for ALL BOTS.
boolean receiveCharacter() {
  // Team 1: a-e
  // Team 2: f-j
  // Team 3: k-o
  // Team 4: p-t
  // Team 5: not needed since team 5 computes.
    
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

// dinoCompute() needed only for DINO.
char dinoCompute() {
  for(int i = 0; i < 4; i++) {
    if(teamResults[i] < teamResults[minIndex]) {
      minIndex = i;
    }
  }
  if(teamResults[minIndex] == dinoResult) {
    return 'X';
  }
  else if(dinoResult == 5){
    return 'Y';
  }
  else {
    return 'Z';
  }
}

// compute() needed only for NON-DINO.
void compute() {
  for(int i = 0; i < 4; i++) {
    if(teamResults[i] < teamResults[minIndex]) {
      minIndex = i;
    }
  }
}
