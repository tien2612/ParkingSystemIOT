
#include <ESP8266WiFi.h>
#include "FirebaseESP8266.h"
#include "ArduinoJson.h"

#define WIFI_SSID "Nguyen Minh Tien"
#define WIFI_PASSWORD "0949291518"

#define FIREBASE_HOST "fir-demo-6ee7d-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "EszGkrqXHWUpg1XyafHpmcJ7hlbhyhSVsS6iK1TZ"

FirebaseData firebaseData;

String path = "/";

FirebaseJson json;


#include <SoftwareSerial.h>

SoftwareSerial ESP8266_softSerial(D7, D6);

String data;

int UID[4];

void setup() {
  // put your setup code here, to run once:
  // Serial.begin(9600);
  
  // ESP8266_softSerial.begin(9600);

  Serial.begin(115200);
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Firebase.begin(FIREBASE_HOST,FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  if(!Firebase.beginStream(firebaseData,path)){
    Serial.println("REASON: "+ firebaseData.errorReason());
    Serial.println();
  }

  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void loop() {
    Firebase.setInt(firebaseData,path + "/data",random(0,100));
    Firebase.setInt(firebaseData,path + "/data",random(0,100));
      
  // put your main code here, to run repeatedly:
//   data = "02,02";
//   ESP8266_softSerial.println(data);
//   Serial.println("Gui du lieu toi Arduino");  
//   Serial.println(data);
//   data = "";
  
// /* ESP8266 Received msg from Arduino */
//   while(ESP8266_softSerial.available()) {
//     data = ESP8266_softSerial.readStringUntil('\n');
//     /* Remove end line */
//     data.remove(data.length() - 1, 1);
//     Serial.println("Du lieu nhan tu Arduino");  
//     Serial.println(data);    
//     data = "";
//   }

//   delay(1000);
  
  

}
