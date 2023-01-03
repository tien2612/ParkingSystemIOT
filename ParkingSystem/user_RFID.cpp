#include "user_RFID.h"
#include "myColor.h"

int current_user = 0;

int check_id(int *user_id, int *data) {
  for (int i = 0; i < sizeof(user_id)/sizeof(user_id[0]); i++) {
    if (user_id[i] != data[i]) return 0;
  }
  
  return 1;
}

void send_new_data_to_gateWay(int slot, int status, String UID) {
  /* Check which type data send to gateway, UID or slot status
  * if data send is UID */
  if ( (status != GREEN_COLOR || status != RED_COLOR || status != YELLOW_COLOR)) {
    // use RF
    String data_send = "!UID:";

    data_send += UID + "#";
    // send here
    // ....
  } else { // data send is slot status format !SLOT:slot-value:status-value#
    String data_send = "!SLOT:" + String(slot) + ":" + String(status) + "#";
    // send here
    // ..
  }
}