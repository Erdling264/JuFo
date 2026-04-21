#include <Arduino.h>


#define RXD2 16
#define TXD2 17



void setup() {
  // put your setup code here, to run once:
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(115200);
  Serial.println("RS485 Monitor started");
  //Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial Txd is on pin: "+String(TX));
  Serial.println("Serial Rxd is on pin: "+String(RX));
  Serial.println("Serial2 Txd is on pin: "+String(TXD2));
  Serial.println("Serial2 Rxd is on pin: "+String(RXD2));

  // Serial2.print("1230030902=?112\r");

}

void loop() {
  Serial2.print("9600030902=?106\r");
//  Serial2.print("9600030902=?121\r");
//  Serial2.print("1230030902=?112\r");
  delay(100);
  // put your main code here, to run repeatedly:
  while (Serial2.available()) {
    Serial.print(char(Serial2.read()));
  }
}