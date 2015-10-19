#include <IRremote.h>
#include <SoftwareSerial.h>
int b1 = 2; // pin button 1
int b2 = 3; // pin button 2
int b3 = 4; // pin button 3
int l1 = 9; // pin light 1
int l2 = 10; // pin light 2
int l3 = 11; // pin light 3
int irPin = 7; // pin IR Receiver
int lastStateB1 = LOW;
int lastStateB2 = LOW;
int lastStateB3 = LOW;
int state1 = LOW; // state light 1
int state2 = LOW; // state light 2
int state3 = LOW; // state light 3
int tx=6; // BT tx
int rx=5; // BT rx
char bufferBT[32]; // bluetooth buffer read
int i = 0; // bluetooth buffer read index
boolean reading = false; // bluetooth buffer reading state

#define REMOTE_BUTTON_1 3001999165
#define REMOTE_BUTTON_2 211564353
#define REMOTE_BUTTON_3 46690913
#define REMOTE_BUTTON_ON 2985278109
#define REMOTE_BUTTON_OFF 1811778305

IRrecv irrecv(irPin);
decode_results results;
SoftwareSerial BTserial(rx, tx);

void setup(){

  pinMode(l1, OUTPUT);
  pinMode(l2, OUTPUT);
//  pinMode(l3, OUTPUT);

  pinMode(b1, INPUT);
  pinMode(b2, INPUT);

  Serial.begin(9600);
  Serial.println("Arduino is ready");

  // Start IR Receiver
  irrecv.enableIRIn();
  // HC-05 default serial speed for commincation mode is 9600
  BTserial.begin(9600); 
}

void loop(){

    // BLUETOOTH 
    if (BTserial.available()) {
      char ch = BTserial.read();
      if (ch == '&') {
        reading = true;
      }
      if (reading) {
        bufferBT[i] = ch;
        if (bufferBT[i] == ';') {
          bufferBT[i + 1] = '\0';
          processCmd(bufferBT);
          i = 0;
          reading = false;
        } else {
          i++;
        }
        if (i > 32) {
          //for (int j = 0; j < 32; j++) { bufferBT[j] = '\0';};
          memset(&bufferBT[0], 0, sizeof(bufferBT));
        }
      }
    }

  // BUTTONS
  int b1state = digitalRead(b1);
  int b2state = digitalRead(b2);
  if (b1state != lastStateB1 && b1state == LOW) {
    state1 = state1 == LOW ? HIGH : LOW;
  }
  if (b2state != lastStateB2 && b2state == LOW) {
    state2 = state2 == LOW ? HIGH : LOW;
  }
  lastStateB1 = b1state;
  lastStateB2 = b2state;

  // IR
  if (irrecv.decode(&results)) {
    switch (results.value) {
      case REMOTE_BUTTON_1: state1 = state1 == LOW ? HIGH : LOW; Serial.println("on1"); break;
      case REMOTE_BUTTON_2: state2 = state2 == LOW ? HIGH : LOW; Serial.println("on2"); break;
//      case REMOTE_BUTTON_3: state3 = state3 == LOW ? HIGH : LOW; break;
      case REMOTE_BUTTON_ON: state1 = HIGH; state2 = HIGH; Serial.println("onAll");  break;
      case REMOTE_BUTTON_OFF: state1 = LOW; state2 = LOW; Serial.println("ofAll"); break;
    }
    Serial.println(results.value);
    irrecv.resume();
  }

  // LIGHTS STATES
  digitalWrite(l1, state1);
  digitalWrite(l2, state2);
} 

void processCmd(char cmd[]) {
  Serial.write(cmd);
  Serial.println("\n");
  if (strcmp(cmd, "&light1;") == 0) {
    state1 = state1 == LOW ? HIGH : LOW;
  }
  if (strcmp(cmd, "&light2;") == 0) {
    state2 = state2 == LOW ? HIGH : LOW;
  }
  //for (int j = 0; j < 32; j++) { bufferBT[j] = '\0';};
  memset(&cmd[0], 0, sizeof(cmd));
}
