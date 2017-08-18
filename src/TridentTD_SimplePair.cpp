#include "TridentTD_SimplePair.h"

TridentTD_SimplePair* staticThis;

TridentTD_SimplePair::TridentTD_SimplePair(SimplePairMode_t sp_mode){
  staticThis = this; 
  staticThis->_sp_mode = sp_mode;
}

bool TridentTD_SimplePair::mode(SimplePairMode_t sp_mode) {
  if(sp_mode != SP_OFF && sp_mode != SP_STA && sp_mode != SP_AP  )
    return false;

  staticThis->_sp_mode = sp_mode;
  if(sp_mode = SP_OFF) {
    staticThis->end();
  }
  return true;
}

SimplePairMode_t TridentTD_SimplePair::getMode() {
    return staticThis->_sp_mode;
}


bool TridentTD_SimplePair::setPublicKey(uint8_t *pb_key){
  if( pb_key == NULL ) { return false; }

  memset(staticThis->_sp_public_key, 0, 16);
  memcpy(staticThis->_sp_public_key, pb_key, 16);
  return true;

}

uint8_t* TridentTD_SimplePair::getPublicKey(uint8_t *pb_key){
  pb_key = (uint8_t*) malloc(16);
  memcpy( pb_key, staticThis->_sp_public_key, 16);
  return pb_key;
}


bool TridentTD_SimplePair::setSimplePairKey(String sp_key){
  if( sp_key.length() > 16 ){
    DEBUG_PRINTLN("[ Simple-Pair ] error : SimplePair Password is not 16 bytes");
    return false;
  } else {
    int space_count = 16 - sp_key.length();
    for(int i = 1; i <= space_count; i++){
      sp_key += " ";
    }
  }
  memset(staticThis->_sp_public_key, 0, 16);
  memcpy(staticThis->_sp_public_key, (uint8_t*)(sp_key.c_str()), 16);
  return true;
}


String TridentTD_SimplePair::getSimplePairKey(){
  if( sizeof(staticThis->_sp_public_key) != 16 ) { return ""; }
  String p16 = String((char*)staticThis->_sp_public_key); p16.trim();
  return p16;
}



bool TridentTD_SimplePair::setPrivateKey(uint8_t *pv_key) {
  if( pv_key == NULL ) { return false; }

  memset(staticThis->_sp_private_key,0,16);
  memcpy(staticThis->_sp_private_key,pv_key, 16);
  return true;
}

uint8_t* TridentTD_SimplePair::getPrivateKey(uint8_t *pv_key){
  pv_key = (uint8_t*) malloc(16);
  memcpy( pv_key, staticThis->_sp_private_key, 16);
  return pv_key;
}

bool TridentTD_SimplePair::setPassword16Byte(String password){
  if( password.length() > 16 ){
    DEBUG_PRINTLN("[ Simple-Pair ] error : Password is not 16 bytes");
    return false;
  } else {
    int space_count = 16 - password.length();
    for(int i = 1; i <= space_count; i++){
      password += " ";
    }
  }
  
  memcpy(  staticThis->_sp_private_key, (uint8_t*)(password.c_str()), 16);
  return true;
}

String TridentTD_SimplePair::getPassword16Byte(){
  if( sizeof(staticThis->_sp_private_key) != 16 ) { return ""; }
  String p16 = String((char*)staticThis->_sp_private_key); p16.trim();
  return p16;
}

bool TridentTD_SimplePair::setSimplePairData(String data)  { 
  return staticThis->setPassword16Byte(data);
}

String TridentTD_SimplePair::getSimplePairData() {
  return staticThis->getPassword16Byte();
}


void TridentTD_SimplePair::annouce(){
  if(staticThis->getMode() != SP_AP ) { return;}
  if(!staticThis->isSleep() ) { return; }

  DEBUG_PRINTLN("[ Simple-Pair ] annouce()");
  staticThis->begin();
}

void TridentTD_SimplePair::scanSP(){
  if(staticThis->getMode() != SP_STA ) { return;}
  if(!staticThis->isSleep() ) { return; }

  DEBUG_PRINTLN("[ Simple-Pair ] scan()");
  staticThis->begin();
  while(staticThis->status()!= ST_FINISH){ delay(1); }
  staticThis->end();
}


bool TridentTD_SimplePair::isSleep(){
  return (staticThis->status() == ST_SLEEP);
}


bool TridentTD_SimplePair::isSentData(){
  if(!staticThis->getMode() != SP_AP ) { return false;}  
  if(!staticThis->_sp_sta_negotiate) { return false;}
  if(!staticThis->isSleep()) { return false; }
  if(staticThis->_sp_sta_mac[0] == 0) { return false;}
  return true;
}

bool TridentTD_SimplePair::isReceivedData(){
  if(!staticThis->getMode() != SP_STA ) { return false;}  
  if(!staticThis->_sp_ap_found) { return false;}
  if(!staticThis->isSleep()) { return false; }
  if(staticThis->_sp_ap_mac[0] == 0) { return false;}
  return true;
}


void TridentTD_SimplePair::onSentData(SimplePairHandler fn){
  _sp_sentdata_cb = fn;
}

void TridentTD_SimplePair::onReceivedData(SimplePairHandler fn){
  _sp_receiveddata_cb = fn;
}
void TridentTD_SimplePair::onScanNotFound(SimplePairHandler fn){
  _sp_scannotfound_cb = fn;
}


bool TridentTD_SimplePair::begin(){

  end(); // end last previus begin
  
  staticThis->_sp_status = ST_BEGIN;

  
  switch(staticThis->_sp_mode) {
  case SP_AP :
    wifi_set_opmode(SOFTAP_MODE);

    staticThis->_sp_sta_negotiate = false;
    wifi_get_macaddr(SOFTAP_IF, _sp_ap_mac);
    memset(_sp_sta_mac,0,6);
    
    if(simple_pair_init()!=0){
      DEBUG_PRINTLN("[ Simple-Pair ] error : simple-pair init");
      return false;
    }
    if(register_simple_pair_status_cb(
            [](u8 *sa, u8 status){

              memcpy(staticThis->_sp_sta_mac, sa,  6);
              staticThis->_sp_status = (SimplePairStatus_t) status;

              staticThis->_simple_pair_ap_status_cb();
              
            })){

      DEBUG_PRINTLN("[ Simple-Pair ] error : regist simple-pair status callback");
      return false;
    }
    if(simple_pair_ap_enter_announce_mode()!=0){
      DEBUG_PRINTLN("[ Simple-Pair ] error : AP enter Announce mode");
      return false;
    }
    
    DEBUG_PRINTLN("[ Simple-Pair ] AP begin Announce mode");
    break;

  case SP_STA:
    staticThis->_sp_scan_cnt = 0;
    wifi_set_opmode(STATION_MODE);

    staticThis->_sp_ap_found = false;
    memset(_sp_ap_mac,0,6);        // รีเซ็ต เพื่อรอ scan simple-pair ap
    wifi_get_macaddr(STATION_IF, _sp_sta_mac);
    memset(_sp_private_key,0,16);  // รีเซ็ต เพีื่อรอรับจาก AP

    if(simple_pair_init()!=0) {
      DEBUG_PRINTLN("[ Simple-Pair ] error : simple-pair init");
      return false;
    }
          
//    if(register_simple_pair_status_cb(reinterpret_cast<simple_pair_status_cb_t>(&TridentTD_SimplePair::_simple_pair_sta_status_cb))!=0 ) {
    if(register_simple_pair_status_cb(
            [](u8 *sa, u8 status){

              memcpy(staticThis->_sp_ap_mac, sa,  6);
              staticThis->_sp_status = (SimplePairStatus_t) status;

              staticThis->_simple_pair_sta_status_cb();
              
            })){
      DEBUG_PRINTLN("[ Simple-Pair ] error : regist simple-pair status callback");
      return false;
    }

    if(simple_pair_sta_enter_scan_mode()!=0) {
      DEBUG_PRINTLN("[ Simple-Pair ] error : STATION enter Scan-mode");
      return false;
    }    

    DEBUG_PRINTLN("[ Simple-Pair ] STA begin Scan mode");
    staticThis->_simple_pair_scan();
    break;

  }
  return true;
}

void TridentTD_SimplePair::end(){
  simple_pair_deinit();
  staticThis->_sp_status = ST_FINISH;
}

uint8_t TridentTD_SimplePair::status(){
  return staticThis->_sp_status;
}

String TridentTD_SimplePair::getSourceMacAddress(){
  return mac2string(staticThis->_sp_ap_mac);
}

uint8_t* TridentTD_SimplePair::getSourceMacAddress(uint8_t* smac){
  smac = (uint8_t*)malloc(6);
  memcpy(smac, staticThis->_sp_ap_mac, 6);
  return  smac;
}

String TridentTD_SimplePair::getDestinationMacAddress(){
  return mac2string(staticThis->_sp_sta_mac);
}

uint8_t* TridentTD_SimplePair::getDestinationMacAddress(uint8_t* dmac){
  dmac = (uint8_t*)malloc(6);
  memcpy(dmac, staticThis->_sp_sta_mac, 6);
  return  dmac;
}

String TridentTD_SimplePair::getSSID(){
  if(staticThis->getMode() != SP_STA) {return "";}
  if(!staticThis->_sp_ap_found) { return ""; }

  return String((char*)staticThis->_sp_ssid);
}

bool TridentTD_SimplePair::setMaxScan(uint8_t max_scan){
  if(max_scan>100) {return false;}

  staticThis->_sp_max_scan = max_scan;
}

void TridentTD_SimplePair::_simple_pair_ap_status_cb(){
  switch (staticThis->_sp_status) {
  case ST_FINISH:
    simple_pair_get_peer_ref(NULL, NULL, staticThis->_sp_private_key);
    DEBUG_PRINTLN("[ Simple-Pair ] AP FINISH");
    /* TODO: Wait STA use the ex-key communicate with AP, for
    example use ESP-NOW */

    if(_sp_sentdata_cb) {
      _sp_sentdata_cb();
    }

    /* if test ok , deinit simple pair */
    simple_pair_deinit();
    break;
  case ST_AP_RECV_NEG:
    /* AP recv a STA's negotiate request */
    DEBUG_PRINTLN("[ Simple-Pair ] AP Recv Negotiate STA MAC " + mac2string(staticThis->_sp_sta_mac));
    DEBUG_PRINTLN("[ Simple-Pair ] AP Sending PrivateKey/Password16 to STA MAC");
    /* set peer must be called, because the simple pair need
       to know what peer mac is */
    simple_pair_set_peer_ref(staticThis->_sp_sta_mac, staticThis->_sp_public_key, staticThis->_sp_private_key);

    staticThis->_sp_sta_negotiate = true;
    
    /* TODO:In this phase, the AP can interaction with Smart Phone,
       if the Phone agree, call start_neg or refuse */
    simple_pair_ap_start_negotiate();
    
    //simple_pair_ap_refuse_negotiate();
    /* TODO:if refuse, maybe call simple_pair_deinit() to
       ending the simple pair */
    break;
  case ST_WAIT_TIMEOUT:
    /* In negotiate, timeout , so re-enter in to announce mode*/
    DEBUG_PRINTLN("[ Simple-Pair ] Neg Timeout");
    simple_pair_state_reset();
    simple_pair_ap_enter_announce_mode();
    break;
  case ST_SEND_ERROR:
    DEBUG_PRINTLN("[ Simple-Pair ] Send Error");
    /* maybe the simple_pair_set_peer_ref() haven't called,
       it send to a wrong mac address */
    break;
  case ST_KEY_INSTALL_ERR:
    DEBUG_PRINTLN("[ Simple-Pair ] Key Install Error");
    /* 1. maybe something argument error.
       2. maybe the key number is full in system*/
    /* TODO: Check other modules which use lots of keys
       Example: ESPNOW and STA/AP use lots of keys
    */
    break;
  case ST_KEY_OVERLAP_ERR:
    DEBUG_PRINTLN("[ Simple-Pair ] Key Overlap Error");
    /* 1. maybe something argument error.
       2. maybe the MAC Address is already use in ESP-NOW or other module
          the same MAC Address has multi key*/
    /* TODO: Check if the same MAC Address used already,
       Example: del MAC item of ESPNOW or other module */
    break;
  case ST_OP_ERROR:
    DEBUG_PRINTLN("[ Simple-Pair ] Operation Order Error");
    /* 1. maybe the function call order has something wrong */
    /* TODO: Adjust your function call order */
    break;
  default:
    DEBUG_PRINTLN("[ Simple-Pair ] Unknown Error");
    break;
  }
}

void TridentTD_SimplePair::_simple_pair_sta_status_cb(){

  switch (staticThis->_sp_status) {
  case ST_FINISH:
    simple_pair_get_peer_ref(NULL, NULL, staticThis->_sp_private_key);

    DEBUG_PRINTLN("[ Simple-Pair ] STA FINISH");
    DEBUG_PRINTLN();
    DEBUG_PRINTLN("[ Simple-Pair ] Simple-Pair AP MAC      : " + mac2string(staticThis->_sp_ap_mac));
    DEBUG_PRINTLN("[ Simple-Pair ] Recieved Password16Byte : " + String((char*)staticThis->_sp_private_key));
    DEBUG_PRINTLN("[ Simple-Pair ] Recieved Private Key    : " + key2array(staticThis->_sp_private_key,16));


    /* TODO: Try to use the ex-key communicate with AP, for example use ESP-NOW */
    /* if test ok , deinit simple pair */
    simple_pair_deinit();
    
    if(staticThis->_sp_receiveddata_cb) {
      staticThis->_sp_receiveddata_cb();
    }
    break;
  case ST_AP_REFUSE_NEG:
    /* AP refuse , so try simple pair again or scan other ap*/
    DEBUG_PRINTLN("[ Simple-Pair ] Recv AP Refuse");
    simple_pair_state_reset();
    simple_pair_sta_enter_scan_mode();
    staticThis->_simple_pair_scan();
    break;
  case ST_WAIT_TIMEOUT:
    /* In negotiate, timeout , so try simple pair again */
    DEBUG_PRINTLN("[ Simple-Pair ]  Negotiate Timeout");
    simple_pair_state_reset();
    simple_pair_sta_enter_scan_mode();
    staticThis->_simple_pair_scan();
    break;
  case ST_SEND_ERROR:
    DEBUG_PRINTLN("[ Simple-Pair ]Send Error");
    /* maybe the simple_pair_set_peer_ref() haven't called, it send to a wrong mac address */
    break;
  case ST_KEY_INSTALL_ERR:
    DEBUG_PRINTLN("[ Simple-Pair ] Key Install Error");
    /* 1. maybe something argument error.
       2. maybe the key number is full in system */
    /* TODO: Check other modules which use lots of keys
       Example: ESPNOW and STA/AP use lots of keys */
    break;
  case ST_KEY_OVERLAP_ERR:
    DEBUG_PRINTLN("[ Simple-Pair ] Key Overlap Error");
    /* 1. maybe something argument error.
       2. maybe the MAC Address is already use in ESP-NOW or other module
       the same MAC Address has multi key*/
    /* TODO: Check if the same MAC Address used already, module */
    break;
  case ST_OP_ERROR:
    DEBUG_PRINTLN("[ Simple-Pair ] Operation Order Error");
    /* 1. maybe the function call order has something wrong */
    /* TODO: Adjust your function call order */
    break;
  default:
    DEBUG_PRINTLN("[ Simple-Pair ] Unknown Error");
    break;
  }

}

void TridentTD_SimplePair::_simple_pair_scan_done(bss_info *bss_link) {
  staticThis->_sp_scan_cnt++;
  int i=0;
  for(bss_info* it = bss_link; it; it = it->next, i++) {
    if (bss_link->simple_pair) {
      DEBUG_PRINTLN("[ Simple-Pair ] STA found BSSID " + mac2string(bss_link->bssid));

      memcpy(staticThis->_sp_ap_mac, bss_link->bssid, 6);
      memcpy(staticThis->_sp_ssid  , bss_link->ssid, bss_link->ssid_len);
      staticThis->_sp_ssid_len = bss_link->ssid_len;
      staticThis->_sp_ap_found = true;
      
      simple_pair_set_peer_ref(staticThis->_sp_ap_mac, staticThis->_sp_public_key, NULL);
      
      if (simple_pair_sta_start_negotiate()!=0){
        DEBUG_PRINTLN("[ Simple-Pair ] STA start Negotiate Failed");
      } else {
        DEBUG_PRINTLN("[ Simple-Pair ] STA start Negotiate OK");
      }

      break;
    }
  }

  if(! staticThis->_sp_ap_found) {
    DEBUG_PRINTF("[ Simple-Pair ] %d. Scan done .. not found Simple-Pair AP\n", staticThis->_sp_scan_cnt);
    if(staticThis->_sp_scan_cnt < staticThis->_sp_max_scan)  { 
      //DEBUG_PRINTLN("[ Simple-Pair ] rescan");
      simple_pair_state_reset();
      simple_pair_sta_enter_scan_mode();
      staticThis->_simple_pair_scan();
    } else{
      DEBUG_PRINTLN("[ Simple-Pair ] Scan Limit Out");
      staticThis->_sp_status = ST_FINISH;
      if(_sp_scannotfound_cb){
        _sp_scannotfound_cb();
      }
      simple_pair_deinit();
    }
  }
}

void TridentTD_SimplePair::_simple_pair_scan(){
  wifi_station_scan(NULL, [](void *arg, STATUS status){
                    bss_info *bssInfo = (bss_info *) arg;
                    if(status==OK)
                      staticThis->_simple_pair_scan_done(bssInfo);
                    else {
                      DEBUG_PRINTLN("[ Simple-Pair ] ScanSP Failed");
                      staticThis->_sp_status = ST_FINISH;
                      simple_pair_deinit();
                      if(staticThis->_sp_scannotfound_cb){
                        staticThis->_sp_scannotfound_cb();
                      }
                    }
                  });
}

String TridentTD_SimplePair::getVersion(){
  return (String)("[TridentTD Simple-Pair] Version ") + String(_version);
}



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

TridentTD_SimplePair  SimplePair;

