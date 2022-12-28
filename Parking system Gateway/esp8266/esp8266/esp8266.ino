#include <SoftwareSerial.h>

SoftwareSerial ESP8266_softSerial(D7, D6);

String data;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  ESP8266_softSerial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  data = "02,02";
  ESP8266_softSerial.println(data);
  Serial.println("Gui du lieu toi Arduino");  
  Serial.println(data);
  data = "";
  
/* ESP8266 Received msg from Arduino */
  while(ESP8266_softSerial.available()) {
    data = ESP8266_softSerial.readStringUntil('\n');
    /* Remove end line */
    data.remove(data.length() - 1, 1);
    Serial.println("Du lieu nhan tu Arduino");  
    
    Serial.println(data);    
    data = "";
  }

  delay(1000);

}
