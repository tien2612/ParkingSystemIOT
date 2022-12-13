#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7,8);// 7 : CE and 8 : CSN
const byte Address[6][6] = {"PKIOT1", "PKIOT2", "PKIOT3" }; // Node 0(trungtam) : Node 1 : Node 2

String send_data;
String receive_data;

void setup() {
  Serial.begin(9600);
  if (!radio.begin()){
    Serial.println("Module isn't connected ...!!");
    while (1){}
  }
  radio.openWritingPipe(Address[0]); // Only write on a channel 
    // Open a channel to write on address"PKIOT0" 
  radio.openReadingPipe(1,Address[1]); // Open a channel to read on address"PKIOT1" (Node 1)
  radio.openReadingPipe(2,Address[2]); // Open a channel to read on address"PKIOT2" (Node 2)


  radio.setPALevel(RF24_PA_MIN); //Cài bộ khuyết địa công suất ở mức MIN
  radio.setChannel(80);
  radio.setDataRate(RF24_250KBPS);    

  if (!radio.available())
  {
    Serial.println("Chưa kết nối được với Node con...!!");
    Serial.println("CHỜ KẾT NỐI.......");
  }

  /////////////////

}

void loop() {

  radio.stopListening();
  /////

  /////
  radio.write(&send_data , sizeof(send_data));
  delay(10);

  radio.startListening();
  while (!radio.available());
  radio.read(&receive_data , sizeof(receive_data));
  //////

  //////
  delay(10);

}
