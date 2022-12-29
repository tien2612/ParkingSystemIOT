#include "NRF24.h"

RF24 radio(7, 8); // CE, CSN

const byte address[6][6] = {"PKIOT1", "PKIOT2"}; //0, 1

int booking_slot = -1;
int *UID = {0};

int* check_booking_receive() {
  int arr[2];
  if (booking_slot >= 0 && booking_slot < N0_NODE_CAR) {
    arr[0] = booking_slot;
    arr[1] = UID;
    return arr;
  } 
  
  return NULL;
}

void confirm_data_receive(String msg) {
  // send msg confirm to gateway
}

int *parse_command(String cmd) {
  int *data_buffer_return = new int[10];
  
  cmd.replace("!", "");
  cmd.replace("#", "");

  const char UID_Reg[] = "UID";
  const char SLOT_Reg[] = "SLOT";
  const char RESERVED_Reg[] = "RESERVED";

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

  if (data_split[0] == UID_Reg) {
    // UID
  } else if (data_split[0] == SLOT_Reg) {
    // slot + status
  } else if (data_split[0] == RESERVED_Reg) {
    // slot + UID
    data_buffer_return[0] = data_split[1].toInt();
    int k = 0;
    String data_temp = "";
    
    for (int i = 0; i < data_split[2].length(); i++) {
     
      if (data_split[2][i] == ' ') {
        
        data_buffer_return[k] = data_temp.toInt();
        data_temp = "";
        k++;
      } else data_temp += data_split[2][i];
    }

    data_buffer_return[k] = data_temp.toInt();
  }
  
  return data_buffer_return;
}