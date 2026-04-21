#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define RXD2 16
#define TXD2 17

const char* ssid = "Jugend-forscht";
const char* password = "jufo2019";

// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";
const char* mqtt_server = "192.168.205.205";
const char *mqtt_username = "vacuum";
const char *mqtt_password = "plasma";
const char *topic = "vacuum/p_sense/pressure";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void callback(char *topic, byte *payload, unsigned int length) {
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
 for (int i = 0; i < length; i++) {
     Serial.print((char) payload[i]);
 }
 Serial.println();
 Serial.println("-----------------------");
}

void setup() {

  //Wlan Anmeldung

  Serial.begin(9600);
  WiFi.begin(ssid,password);

  while (WiFi.status() !=WL_CONNECTED) {
    delay(1000);
    Serial.println("Verbinden");
  }
  Serial.println("Verbunden");

  //MQTT Anmeldung

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
        Serial.println("Public emqx mqtt broker connected");
    } else {
        Serial.print("failed with state ");
        Serial.print(client.state());
        delay(2000);
      }
  }
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  
  Serial.println("RS485 Monitor started");
  //Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial Txd is on pin: "+String(TX));
  Serial.println("Serial Rxd is on pin: "+String(RX));
  Serial.println("Serial2 Txd is on pin: "+String(TXD2));
  Serial.println("Serial2 Rxd is on pin: "+String(RXD2));

  // Serial2.print("1230030902=?112\r");

}

float analoginput = 0;

void loop() {
  client.loop();
  
  for(int i=1;i<500;i++) {
    analoginput = 0.95*analoginput + 0.05*analogRead(35);
    delay(5);
  }
  analoginput = (((analogRead(35)*3.3)/4095)*3);
  Serial.println(analoginput);
  float p = pow(10, 1.5*analoginput-12);
  char ptxt[10];
  sprintf(ptxt,"%.3e",p);
  client.publish(topic, ptxt);
  delay(500);
  Serial.println(ptxt);

  Serial2.print("9600030902=?106\r");
//  Serial2.print("9600030902=?121\r");
//  Serial2.print("1230030902=?112\r");
  delay(100);
  // put your main code here, to run repeatedly:
  while (Serial2.available()) {
    Serial.print(char(Serial2.read()));
  }
}