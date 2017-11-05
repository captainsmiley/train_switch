#include <Arduino.h>

const char nr_of_v = 2;

const int read_v1_pin = 10;
const int read_v2_pin = 16;
// variable to save v states
uint32_t v = 0;
uint32_t v_old = 0;

String msg = "";

unsigned long time;

void setup() {
  // Setup serial Serial for debug and Serial1 for wifi com
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial.println("Start of input controller program");

  // Setup all inputs
  pinMode(read_v1_pin, INPUT);
  pinMode(read_v2_pin, INPUT);

  time = millis();
}


void loop() {
  // Read all inputs
  v = (digitalRead(read_v1_pin) << 0) |
      (digitalRead(read_v2_pin) << 1) ;
  // if any change, send msg
  if (v != v_old || (millis() - time) > 5000 )
  {
    // Create msg
    msg = "";
    msg +=  String(v);
    msg += "#";
    // Send msg to wifi;
    Serial1.print(msg);
    //Serial1.flush();
    time = millis();
  }
  v_old = v;
}
