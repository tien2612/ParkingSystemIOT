#include "user_RFID.h"
#include "myColor.h"

/* User RFID */
user_data_ID user_ID[N0_USER_ID];
user_data_ID user_ID_Slot[N0_USER_ID];


int check_id(int *user_id, int *data) {
  for (int i = 0; i < sizeof(user_id)/sizeof(user_id[0]); i++) {
    if (user_id[i] != data[i]) return 0;
  }
  
  return 1;
}

