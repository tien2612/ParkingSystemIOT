#include "user_RFID.h"
#include "myColor.h"

int current_user = 0;

int check_id(int *user_id, int *data) {
  for (int i = 0; i < sizeof(user_id)/sizeof(user_id[0]); i++) {
    if (user_id[i] != data[i]) return 0;
  }
  
  return 1;
}
