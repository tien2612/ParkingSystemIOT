#include "user_RFID.h"

user_data_ID user_ID[N0_USER_ID];

int current_index_user_ID = 0;

int check_id(int* user_id, int* data){
  for (int i = 0; i <sizeof(user_id)/sizeof(user_id[0]);i++){
    if (user_id[i] != data[i]){
      return 0;
    }
  }
  return 1;
}

// int check_id_all_user_ID(int* data){
//   for (int i = 0; i <= current_index_user_ID; i++){
//     if (check_id(&user[i]), &)
//   }
// }

void update_user_ID(int slot, String UID){
  int index = 0;
  String data = "";
  for (int i = 0; i < sizeof(UID)/sizeof(UID[0]); i++) {
    if (i == ' ') {
      user_ID[slot].ID[index++] = data.toInt();  
      data = "";
    } else data += UID[i];
  }
}