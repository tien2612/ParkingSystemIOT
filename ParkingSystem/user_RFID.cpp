#include "user_RFID.h"
#include "myColor.h"

int current_user = 0;

int check_id(int *user_id, int *data) {
  for (int i = 0; i < sizeof(user_id)/sizeof(user_id[0]); i++) {
    if (user_id[i] != data[i]) return 0;
  }
  
  return 1;
}

<<<<<<< Updated upstream
void send_new_data_to_gateWay(int slot, int status, int *uid) {
  /* Check which type data send to gateway, UID or slot status
  * if data send is UID */
  if ( (status != GREEN_COLOR || status != RED_COLOR || status != YELLOW_COLOR)) {
    // use RF
    String data_send = "!UID:";

    for (int i = 0; i < sizeof(uid)/sizeof(uid[0]); i++) {
      data_send += String(uid[i]);
      if ( i != sizeof(uid)/sizeof(uid[0]) ) 
        data_send += " ";
    }
    data_send += "#";
    // send here
    // ....
  } else { // data send is slot status format !SLOT:slot-value:status-value#
    String data_send = "!SLOT:" + String(slot) + ":" + String(status) + "#";
    // send here
    // ..
  }

}
=======
void send_new_uid_to_gateWay(int *uid) {
  // use RF
  String data_send = "";

  for (int i = 0; i < sizeof(uid)/sizeof(uid[0]); i++) {
    data_send += String(uid[i]);
    if ( i != sizeof(uid)/sizeof(uid[0]) ) 
      data_send += " ";
  }
}



>>>>>>> Stashed changes
