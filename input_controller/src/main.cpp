#include <Arduino.h>

const char nr_of_v = 3;

const int read_v1_pin = 9; // nr 6;
const int read_v2_pin = 8; // nr 8;
const int read_v3_pin = 7; // nr 10;
const int read_v4_pin = 6; // nr 1;
const int read_v5_pin = 5; // nr 3;
const int read_v6_pin = 4; // nr 5;
const int read_v7_pin = 3; // nr 7;
const int read_v8_pin = 2; // nr 9;
const int read_v9_pin = 10; // nr 4;
const int read_v10_pin = 14; // nr 2;
const int timeout_send_msg = 5000;
const int min_time_send_msg = 50;
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
  pinMode(read_v3_pin, INPUT);
  pinMode(read_v4_pin, INPUT);
  pinMode(read_v5_pin, INPUT);
  pinMode(read_v6_pin, INPUT);
  pinMode(read_v7_pin, INPUT);
  pinMode(read_v8_pin, INPUT);
  pinMode(read_v9_pin, INPUT);
  pinMode(read_v10_pin, INPUT);

  time = millis();
}


void loop() {
  // Read all inputs
  v = (digitalRead(read_v1_pin) << 0) |
      (digitalRead(read_v2_pin) << 1) |
      (digitalRead(read_v3_pin) << 2) |
      (digitalRead(read_v4_pin) << 3) |
      (digitalRead(read_v5_pin) << 4) |
      (digitalRead(read_v6_pin) << 5) |
      (digitalRead(read_v7_pin) << 6) |
      (digitalRead(read_v8_pin) << 7) |
      (digitalRead(read_v9_pin) << 8) |
      (digitalRead(read_v10_pin) << 9) ;
  // if any change or timeout, send msg
  if ( (((millis()- time) > min_time_send_msg) && (v != v_old) ) || (millis() - time) >  timeout_send_msg)
  {
    // Create msg
    msg = "";
    msg +=  String(v);
    msg += "#";
    // Send msg to wifi;
    Serial1.print(msg);
    Serial.print(msg);
    //Serial1.flush();
    time = millis();
  }
  v_old = v;

  if (Serial1.available())
  {
    Serial.print(Serial.read());
  }
}
