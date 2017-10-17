#include "TridentTD_common.h"

String mac2string(u8 *mac_addr){
  char macStr[18] = { 0 };
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", 
      mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  return String(macStr);
}

String key2array(u8 *key, u8 len){
  String keyArrString="{";
  for(int i=0; i< len; i++){
    char buf[4];
    sprintf( buf, "0x%02X", key[i] );
    keyArrString += ((i)? ", ":"")+String(buf);
  }
  keyArrString += "}";

  return keyArrString;
}

String key2string(u8 *key){
  return key2array(key, 16);
}
