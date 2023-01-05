
#include <ESP8266WiFi.h>
#include "FirebaseESP8266.h"
#include "ArduinoJson.h"

#define WIFI_SSID "Nguyen Minh Tien"
#define WIFI_PASSWORD "0949291518"

#define FIREBASE_HOST "apptest-1880b-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "0fHtyV9tFd8BXYJMhlAP3MhIz2doCnUYJqSnZPH0"

FirebaseData firebaseData;

String path = "/";

FirebaseJson json;


#include <SoftwareSerial.h>

SoftwareSerial ESP8266_softSerial(D7, D6); // RX:D7, TX:D6

String data;
String UID;
int status ;
int slot;
unsigned long currentTimer = 0;
unsigned long timer = 0;
bool flag_received_data[4] = {false};
bool flag_getData = false;

// int UID[4];
void intData(void){
  Firebase.setInt(firebaseData,path + "/Slot/0/slot",0);
  Firebase.setInt(firebaseData,path + "/Slot/0/status",3);
  Firebase.setString(firebaseData,path + "/Slot/0/UID","113 184 187 28");

  Firebase.setInt(firebaseData,path + "/Slot/1/slot",1);
  Firebase.setInt(firebaseData,path + "/Slot/1/status",3);
  Firebase.setString(firebaseData,path + "/Slot/1/UID","129 88 223 28");

  Firebase.setInt(firebaseData,path + "/Slot/2/slot",2);
  Firebase.setInt(firebaseData,path + "/Slot/2/status",3);
  Firebase.setString(firebaseData,path + "/Slot/2/UID","129 30 26 28");
  
  Firebase.setInt(firebaseData,path + "/Slot/3/slot",3);
  Firebase.setInt(firebaseData,path + "/Slot/3/status",3);
  Firebase.setString(firebaseData,path + "/Slot/3/UID","113 166 45 28");
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  ESP8266_softSerial.begin(9600);
  // Serial.begin(115200);
  
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
  // intData();  
}


void comparser_command_data(String cmd){
  
  cmd.replace("!", "");
  cmd.replace("#", "");

  const char UID_Reg[] = "UID";
  const char SLOT_Reg[] = "SLOT";
  const char RESERVED_Reg[] = "RESERVED";
  // const char UID_Reg[] = "UID";
  // const char UID_Reg[] = "UID";
  // const char UID_Reg[] = "UID";
  // const char UID_Reg[] = "UID";

  String data_split[10]; 
  String data = "";
  int j = 0;
  for (int i = 0; i < cmd.length(); i++) {
    if (cmd[i] == ':') {
      data_split[j] = data;
      data = "";
      j++;
    } else data += cmd[i];
  }

  data_split[j] = data;
  Serial.println(data_split[0]);
  
  if (data_split[0] == UID_Reg){ // Format :!UID:String#
      // store new UID to EEPROM
      // Serial.println(data_split[1]);
      // String data_temp = "";   
      // int k = 0;
      // for (int i = 0; i < data_split[1].length(); i++) {
      //   if (data_split[1][i] == ' ') {
          
      //     UID[k] = data_temp.toInt();
      //     data_temp = "";
      //     k++;
      //   } else data_temp += data_split[1][i];
      // }
     //Transmit_Data_To_ESP();
       
  } else if (data_split[0] == SLOT_Reg) {// Format : !SLOT:slot:status#        
      int slot = data_split[1].toInt();
      // Serial.println(slot);
      int status = data_split[2].toInt();
      // Serial.println(status);
      PushSlotStatus(slot,status); 
  
  } else if (data_split[0] == RESERVED_Reg) {// Format : !RESERVED:slot:uid#
      int slot = data_split[1].toInt();
      // Serial.println(slot);        
  }
    else if (data_split[0] == "OK1") flag_received_data[0] = true;
    else if (data_split[0] == "OK2") flag_received_data[1] = true;
    else if (data_split[0] == "OK3") flag_received_data[2] = true;
    else if (data_split[0] == "OK4") flag_received_data[3] = true;
}

void PushSlotStatus(int slot,int status){
  switch (slot){
    case 0:
      Firebase.setInt(firebaseData,path + "/Slot/0/status",status);
      break;
    case 1:
      Firebase.setInt(firebaseData,path + "/Slot/1/status",status);
      break;
    case 2:
      Firebase.setInt(firebaseData,path + "/Slot/2/status",status);
      break;
    case 3:
      Firebase.setInt(firebaseData,path + "/Slot/3/status",status);
      break;
  }
} 


void loop() {
  Serial.println("Avai") ;   
  /* ESP8266 Received msg from Arduino */
  while(ESP8266_softSerial.available()) {
    Serial.println("Available") ;   
    data = ESP8266_softSerial.readStringUntil('\n');
    /* Remove end line */
    data.remove(data.length() - 1, 1);
    Serial.println("Du lieu nhan tu Arduino");  
    Serial.println(data);
    comparser_command_data(data);
    data = "";
  }
  
  if (flag_getData == false){
    Serial.println("Get data from firebase");  
    if (Firebase.getString(firebaseData,path + "/Slot/0/UID" )){
      Serial.println("Trong");  
      
      UID = firebaseData.stringData();
      Firebase.getInt(firebaseData,path + "/Slot/0/slot" );
      slot = firebaseData.intData();
      // data += "!RESERVED:";
      data += "!RESERVED:" + String(slot) + ":" + UID + "#";
      // data += "#";
      ESP8266_softSerial.println(data);
      Serial.println("Gui du lieu toi Arduino");  
      Serial.println(data);
      flag_getData = true;
      data = "";
      UID = "";
      slot = -1;
    }
      Serial.println("Ngoai");  

    // delay(100);
    if (Firebase.getString(firebaseData,path + "/Slot/1/UID" )){
      UID = firebaseData.stringData();
      Firebase.getInt(firebaseData,path + "/Slot/1/slot" );
      slot = firebaseData.intData();
      // data += "!RESERVED:";
      data += "!RESERVED:" + String(slot) + ":" + UID +"#";
      // data += "#";
      ESP8266_softSerial.println(data);
      Serial.println("Gui du lieu toi Arduino");  
      Serial.println(data);
      flag_getData = true;
      data = "";
      UID = "";
      slot = -1;
      // delay(100);
    }

    if (Firebase.getString(firebaseData,path + "/Slot/2/UID" )){
      UID = firebaseData.stringData();
      Firebase.getInt(firebaseData,path + "/Slot/2/slot" );
      slot = firebaseData.intData();
      // data += "!RESERVED:";
      data += "!RESERVED:" + String(slot) + ":" + UID +"#";
      // data += "#";
      ESP8266_softSerial.println(data);
      Serial.println("Gui du lieu toi Arduino");  
      Serial.println(data);
      flag_getData = true;
      data = "";
      UID = "";
      slot = -1;
      // delay(100);
    }

    if (Firebase.getString(firebaseData,path + "/Slot/3/UID" )){
      UID = firebaseData.stringData();
      Firebase.getInt(firebaseData,path + "/Slot/3/slot" );
      slot = firebaseData.intData();
      // data += "!RESERVED:";
      data += "!RESERVED:" + String(slot) + ":" + UID +"#";
      // data += "#";
      ESP8266_softSerial.println(data);
      Serial.println("Gui du lieu toi Arduino");  
      Serial.println(data);
      flag_getData = true;
      data = "";
      UID = "";
      slot = -1;
      // delay(100);
    }
    
  }else if (flag_getData == true){
      Serial.println("Chuan bi delete");      
    if ( flag_received_data[0]){
      Serial.println("Delete IUD SLOT 1");
      flag_received_data[0] = false;
      Firebase.deleteNode(firebaseData,path + "/Slot/0/UID");
      flag_getData = false;
    }
    
    if ( flag_received_data[1]){
      Serial.println("Delete IUD SLOT 2");
      flag_received_data[1] = false;
      Firebase.deleteNode(firebaseData,path + "/Slot/1/UID");
      flag_getData = false;

    }
    
    if ( flag_received_data[2]){
      Serial.println("Delete IUD SLOT 3");
      flag_received_data[2] = false;
      Firebase.deleteNode(firebaseData,path + "/Slot/2/UID");
      flag_getData = false;

    }
    
    if ( flag_received_data[3]){
      Serial.println("Delete IUD SLOT 4");
      flag_received_data[3] = false;
      Firebase.deleteNode(firebaseData,path + "/Slot/3/UID");
      flag_getData = false;
    }
  }
  
  delay(100);
  // if (millis() - timer > 20000){
  // // if (flag){
  //   timer = millis();
  //   ESP8266_softSerial.println(data);
  //   Serial.println("Gui du lieu toi Arduino");  
  //   Serial.println(data);
  //   data = "";
  //   flag = false;
  // }
  // delay(2000);
}
