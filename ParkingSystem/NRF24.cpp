#include "NRF24.h"

RF24 radio(7, 8); // CE, CSN

const byte address[6][6] = {"PKIOT1", "PKIOT2"}; //0, 1

int booking_slot = -1;
int *UID = {0};
unsigned long int startWaitingCustomer = 0;

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