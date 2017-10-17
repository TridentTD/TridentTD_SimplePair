/*
 Copyright (c) 2017 TridentTD (Ven.Phaisarn Techajaruwong).  All right reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "TridentTD_SimplePair.h"


TridentTD_SimplePair* staticSimplePair;

TridentTD_SimplePair::TridentTD_SimplePair(SimplePairMode_t sp_mode){
  staticSimplePair = this; 
  staticSimplePair->_sp_mode = sp_mode;
}

bool TridentTD_SimplePair::mode(SimplePairMode_t sp_mode) {
  if(sp_mode != SP_OFF && sp_mode != SP_STA && sp_mode != SP_AP  )
    return false;

  staticSimplePair->_sp_mode = sp_mode;
  if(sp_mode = SP_OFF) {
    staticSimplePair->end();
  }
  return true;
}

SimplePairMode_t TridentTD_SimplePair::getMode() {
    return staticSimplePair->_sp_mode;
}


void TridentTD_SimplePair::setWiFiStatusLed(bool turn_on){
  _wifi_status_led_turnon = turn_on;
}

void TridentTD_SimplePair::_turnon_WiFiStatusLed(){
  wifi_status_led_install(2,  PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
}

void TridentTD_SimplePair::_turnoff_WiFiStatusLed(){
  wifi_status_led_uninstall ();
}



//--------------------------------------------------------------

bool TridentTD_SimplePair::setSimplePairKey(uint8_t* sp_key)  { 
  if( sp_key == NULL ) { return false; }

  memset(staticSimplePair->_sp_key,0,16);
  memcpy(staticSimplePair->_sp_key,sp_key, 16);
  return true;
}

bool TridentTD_SimplePair::setSimplePairKey(char* sp_key)  { 
  return staticSimplePair->setSimplePairKey((uint8_t*)sp_key);
}

bool TridentTD_SimplePair::setSimplePairKey(String sp_key){
  if( sp_key.length() > 16 ){
    sp_key = sp_key.substring(0,16);
  } else {
    int space_count = 16 - sp_key.length();
    for(int i = 1; i <= space_count; i++){
      sp_key += " ";
    }
  }
  memset(staticSimplePair->_sp_key, 0, 16);
  memcpy(staticSimplePair->_sp_key, (uint8_t*)(sp_key.c_str()), 16);
  return true;
}


uint8_t*  TridentTD_SimplePair::getSimplePairKey(uint8_t *sp_key){
  sp_key = (uint8_t*) malloc(16);
  memcpy( sp_key, staticSimplePair->_sp_key, 16);
  return sp_key;
}

char*  TridentTD_SimplePair::getSimplePairKey(char *sp_key){
  sp_key = (char*) malloc(16);
  memcpy( (char*) sp_key, staticSimplePair->_sp_key, 16);
  sp_key[16]='\0';
  return sp_key;
}


String TridentTD_SimplePair::getSimplePairKey(){
  if( sizeof(staticSimplePair->_sp_key) != 16 ) { return ""; }
  String p16 = String((char*)staticSimplePair->_sp_key); p16.trim();
  return p16;
}
//------------------------------------------------------------------

bool TridentTD_SimplePair::setSimplePairData(uint8_t* data)  { 
  if( data == NULL ) { return false; }

  memset(staticSimplePair->_sp_data,0,16);
  memcpy(staticSimplePair->_sp_data,data, 16);
  return true;
}

bool TridentTD_SimplePair::setSimplePairData(char* data)  { 
  return staticSimplePair->setSimplePairData((uint8_t*)data);
}

bool TridentTD_SimplePair::setSimplePairData(String data)  { 
  if( data.length() > 16 ){
    data = data.substring(0,16);
  } else {
    int space_count = 16 - data.length();
    for(int i = 1; i <= space_count; i++){
      data += " ";
    }
  }
  
  memcpy(  staticSimplePair->_sp_data, (uint8_t*)(data.c_str()), 16);
  return true;
}



uint8_t*  TridentTD_SimplePair::getSimplePairData(uint8_t *data){
  data = (uint8_t*) malloc(16);
  memcpy( data, staticSimplePair->_sp_data, 16);
  return data;
}

char*  TridentTD_SimplePair::getSimplePairData(char *data){
  data = (char*) malloc(16);
  memcpy( (char*)data, staticSimplePair->_sp_data, 16);
  data[16]='\0';
  return data;
}

String TridentTD_SimplePair::getSimplePairData(){
  _sp_data[16] = '\0';
  String p16 = String((char*)staticSimplePair->_sp_data); p16.trim();
  return p16;
}

//----------------------------------------------

void TridentTD_SimplePair::annouce(){
  if(staticSimplePair->getMode() != SP_AP ) { return;}
  if(!staticSimplePair->isSleep() ) { return; }

  DEBUG_PRINTLN("[ Simple-Pair ] annouce()");
  if(_wifi_status_led_turnon){
    _turnon_WiFiStatusLed();
  }else{
    _turnoff_WiFiStatusLed();
  }

  staticSimplePair->begin();
}

void TridentTD_SimplePair::scanSP(){
  if(staticSimplePair->getMode() != SP_STA ) { return;}
  if(!staticSimplePair->isSleep() ) { return; }

  DEBUG_PRINTLN("[ Simple-Pair ] scan()");
  if(_wifi_status_led_turnon){
    _turnon_WiFiStatusLed();
  }else{
    _turnoff_WiFiStatusLed();
  }
  staticSimplePair->begin();
  while(staticSimplePair->status()!= ST_FINISH){ delay(1); }
  staticSimplePair->end();
}


bool TridentTD_SimplePair::isSleep(){
  return (staticSimplePair->status() == ST_SLEEP);
}


bool TridentTD_SimplePair::isSentData(){
  if(!staticSimplePair->getMode() != SP_AP ) { return false;}  
  if(!staticSimplePair->_sp_sta_negotiate) { return false;}
  if(!staticSimplePair->isSleep()) { return false; }
  if(staticSimplePair->_sp_sta_mac[0] == 0) { return false;}
  return true;
}

bool TridentTD_SimplePair::isReceivedData(){
  if(!staticSimplePair->getMode() != SP_STA ) { return false;}  
  if(!staticSimplePair->_sp_ap_found) { return false;}
  if(!staticSimplePair->isSleep()) { return false; }
  if(staticSimplePair->_sp_ap_mac[0] == 0) { return false;}
  return true;
}


void TridentTD_SimplePair::onSentData(SimplePairHandler fn){
  staticSimplePair->_sp_sentdata_cb = fn;
}

void TridentTD_SimplePair::onReceivedData(SimplePairHandler fn){
  staticSimplePair->_sp_receiveddata_cb = fn;
}
void TridentTD_SimplePair::onScanNotFound(SimplePairHandler fn){
  staticSimplePair->_sp_scannotfound_cb = fn;
}


bool TridentTD_SimplePair::begin(){

  end(); // end last previus begin
  
  staticSimplePair->_sp_status = ST_BEGIN;

  
  switch(staticSimplePair->_sp_mode) {
  case SP_AP :
    wifi_set_opmode(SOFTAP_MODE);

    staticSimplePair->_sp_sta_negotiate = false;
    wifi_get_macaddr(SOFTAP_IF, _sp_ap_mac);
    memset(_sp_sta_mac,0,6);
    
    if(simple_pair_init()!=0){
      DEBUG_PRINTLN("[ Simple-Pair ] error : simple-pair init");
      return false;
    }
    if(register_simple_pair_status_cb(
            [](u8 *sa, u8 status){

              memcpy(staticSimplePair->_sp_sta_mac, sa,  6);
              staticSimplePair->_sp_status = (SimplePairStatus_t) status;

              staticSimplePair->_simple_pair_ap_status_cb();
              
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
    staticSimplePair->_sp_scan_cnt = 0;
    wifi_set_opmode(STATION_MODE);

    staticSimplePair->_sp_ap_found = false;
    memset(_sp_ap_mac,0,6);        // รีเซ็ต เพื่อรอ scan simple-pair ap
    wifi_get_macaddr(STATION_IF, _sp_sta_mac);
    memset(_sp_data,0,16);  // รีเซ็ต เพีื่อรอรับจาก AP

    if(simple_pair_init()!=0) {
      DEBUG_PRINTLN("[ Simple-Pair ] error : simple-pair init");
      return false;
    }
          
//    if(register_simple_pair_status_cb(reinterpret_cast<simple_pair_status_cb_t>(&TridentTD_SimplePair::_simple_pair_sta_status_cb))!=0 ) {
    if(register_simple_pair_status_cb(
            [](u8 *sa, u8 status){

              memcpy(staticSimplePair->_sp_ap_mac, sa,  6);
              staticSimplePair->_sp_status = (SimplePairStatus_t) status;

              staticSimplePair->_simple_pair_sta_status_cb();
              
            })){
      DEBUG_PRINTLN("[ Simple-Pair ] error : regist simple-pair status callback");
      return false;
    }

    if(simple_pair_sta_enter_scan_mode()!=0) {
      DEBUG_PRINTLN("[ Simple-Pair ] error : STATION enter Scan-mode");
      return false;
    }    

    DEBUG_PRINTLN("[ Simple-Pair ] STA begin Scan mode");
    staticSimplePair->_simple_pair_scan();
    break;

  }
  return true;
}

void TridentTD_SimplePair::end(){
  simple_pair_deinit();
  staticSimplePair->_sp_status = ST_FINISH;
}

uint8_t TridentTD_SimplePair::status(){
  return staticSimplePair->_sp_status;
}

String TridentTD_SimplePair::getSourceMacAddress(){
  return mac2string(staticSimplePair->_sp_ap_mac);
}

uint8_t* TridentTD_SimplePair::getSourceMacAddress(uint8_t* smac){
  smac = (uint8_t*)malloc(6);
  memcpy(smac, staticSimplePair->_sp_ap_mac, 6);
  return  smac;
}

String TridentTD_SimplePair::getDestinationMacAddress(){
  return mac2string(staticSimplePair->_sp_sta_mac);
}

uint8_t* TridentTD_SimplePair::getDestinationMacAddress(uint8_t* dmac){
  dmac = (uint8_t*)malloc(6);
  memcpy(dmac, staticSimplePair->_sp_sta_mac, 6);
  return  dmac;
}

String TridentTD_SimplePair::getSSID(){
  if(staticSimplePair->getMode() != SP_STA) {return "";}
  if(!staticSimplePair->_sp_ap_found) { return ""; }

  return String((char*)staticSimplePair->_sp_ssid);
}

bool TridentTD_SimplePair::setMaxScan(uint8_t max_scan){
  if(max_scan>100) {return false;}

  staticSimplePair->_sp_max_scan = max_scan;
}

void TridentTD_SimplePair::_simple_pair_ap_status_cb(){
  switch (staticSimplePair->_sp_status) {
  case ST_FINISH:
    simple_pair_get_peer_ref(NULL, NULL, staticSimplePair->_sp_data);
    DEBUG_PRINTLN("[ Simple-Pair ] AP FINISH");
    /* TODO: Wait STA use the ex-key communicate with AP, for
    example use ESP-NOW */

    if(_sp_sentdata_cb != NULL ) {
      (*_sp_sentdata_cb)();
    }

    /* if test ok , deinit simple pair */
    simple_pair_deinit();
    _turnoff_WiFiStatusLed();
    break;
  case ST_AP_RECV_NEG:
    /* AP recv a STA's negotiate request */

    DEBUG_PRINTLN("[ Simple-Pair ] AP Recv Negotiate STA MAC " + mac2string(staticSimplePair->_sp_sta_mac));

    if(esp_now_is_peer_exist(staticSimplePair->_sp_sta_mac) > 0){
      esp_now_del_peer(staticSimplePair->_sp_sta_mac);
      DEBUG_PRINTLN("[ Simple-Pair ] Delete ESP-NOW's mac & SimplePair re begin");
    }

    DEBUG_PRINTLN("[ Simple-Pair ] AP Sending PrivateKey/Password16 to STA MAC");


    /* set peer must be called, because the simple pair need
       to know what peer mac is */
    simple_pair_set_peer_ref(staticSimplePair->_sp_sta_mac, staticSimplePair->_sp_key, staticSimplePair->_sp_data);

    staticSimplePair->_sp_sta_negotiate = true;
    
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
    if(esp_now_is_peer_exist(staticSimplePair->_sp_sta_mac) > 0){
      esp_now_del_peer(staticSimplePair->_sp_sta_mac);
      DEBUG_PRINTLN("[ Simple-Pair ] SP rebegin");
      //simple_pair_deinit();
      staticSimplePair->begin();
    }
    else {
      DEBUG_PRINTLN("[ Simple-Pair ] SP Reset & Annouce again");
      simple_pair_state_reset();
      simple_pair_ap_enter_announce_mode();
    }
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

  switch (staticSimplePair->_sp_status) {
  case ST_FINISH:
    simple_pair_get_peer_ref(NULL, NULL, staticSimplePair->_sp_data);

    DEBUG_PRINTLN("[ Simple-Pair ] STA FINISH");
    DEBUG_PRINTLN();
    DEBUG_PRINTLN("[ Simple-Pair ] Simple-Pair AP MAC : " + mac2string(staticSimplePair->_sp_ap_mac));
    DEBUG_PRINTLN("[ Simple-Pair ] Received String    : " + String((char*)staticSimplePair->_sp_data));
    DEBUG_PRINTLN("[ Simple-Pair ] Received Key       : " + key2array(staticSimplePair->_sp_data,16));


    /* TODO: Try to use the ex-key communicate with AP, for example use ESP-NOW */
    /* if test ok , deinit simple pair */
    simple_pair_deinit();
    //_turnoff_WiFiStatusLed();
    if(_sp_receiveddata_cb != NULL) {
      (*_sp_receiveddata_cb)();
    }
    break;
  case ST_AP_REFUSE_NEG:
    /* AP refuse , so try simple pair again or scan other ap*/
    DEBUG_PRINTLN("[ Simple-Pair ] Recv AP Refuse");
    simple_pair_state_reset();
    simple_pair_sta_enter_scan_mode();
    staticSimplePair->_simple_pair_scan();
    break;
  case ST_WAIT_TIMEOUT:
    /* In negotiate, timeout , so try simple pair again */
    DEBUG_PRINTLN("[ Simple-Pair ]  Negotiate Timeout");
    simple_pair_state_reset();
    simple_pair_sta_enter_scan_mode();
    staticSimplePair->_simple_pair_scan();
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

void TridentTD_SimplePair::_simple_pair_scan_done(struct bss_info *bss_link) {
  staticSimplePair->_sp_scan_cnt++;
  int i=0;
  for(struct bss_info* it = bss_link; it; it =  STAILQ_NEXT(it, next), i++) {
    if (bss_link->simple_pair) {
      DEBUG_PRINTLN("[ Simple-Pair ] STA found BSSID " + mac2string(bss_link->bssid));

      memcpy(staticSimplePair->_sp_ap_mac, bss_link->bssid, 6);
      memcpy(staticSimplePair->_sp_ssid  , bss_link->ssid, bss_link->ssid_len);
      staticSimplePair->_sp_ssid_len = bss_link->ssid_len;
      staticSimplePair->_sp_ap_found = true;
      
      simple_pair_set_peer_ref(staticSimplePair->_sp_ap_mac, staticSimplePair->_sp_key, NULL);
      
      if( esp_now_is_peer_exist(bss_link->bssid) > 0 ) {  // >0 = exist
        DEBUG_PRINTLN("[ Simple-Pair ] ESP-NOW already add this address.");
        if(staticSimplePair->_sp_scan_cnt++ < staticSimplePair->_sp_max_scan)  { 
          //DEBUG_PRINTLN("[ Simple-Pair ] rescan");
          simple_pair_state_reset();
          simple_pair_sta_enter_scan_mode();
          staticSimplePair->_simple_pair_scan();
        } else{
          DEBUG_PRINTLN("[ Simple-Pair ] Scan Limit Out");
          staticSimplePair->_sp_status = ST_FINISH;
          if(staticSimplePair->_sp_scannotfound_cb != NULL){
            (*(staticSimplePair->_sp_scannotfound_cb))();
          }
          simple_pair_deinit();
        }
        return;
      }else{
        if (simple_pair_sta_start_negotiate()!=0){
          DEBUG_PRINTLN("[ Simple-Pair ] STA start Negotiate Failed");
        } else {
          DEBUG_PRINTLN("[ Simple-Pair ] STA start Negotiate OK");
        }
      }

      break;
    }
  }

  if(! staticSimplePair->_sp_ap_found) {
    DEBUG_PRINTF("[ Simple-Pair ] %d. Scan done .. not found Simple-Pair AP\n", staticSimplePair->_sp_scan_cnt);
    if(staticSimplePair->_sp_scan_cnt < staticSimplePair->_sp_max_scan)  { 
      //DEBUG_PRINTLN("[ Simple-Pair ] rescan");
      simple_pair_state_reset();
      simple_pair_sta_enter_scan_mode();
      staticSimplePair->_simple_pair_scan();
    } else{
      DEBUG_PRINTLN("[ Simple-Pair ] Scan Limit Out");
      staticSimplePair->_sp_status = ST_FINISH;
      if(staticSimplePair->_sp_scannotfound_cb != NULL){
        (*(staticSimplePair->_sp_scannotfound_cb))();
      }
      simple_pair_deinit();
    }
  }
}

void TridentTD_SimplePair::_simple_pair_scan(){
  wifi_station_scan(NULL, [](void *arg, STATUS status){
                    struct bss_info *bssInfo = (struct bss_info *) arg;
                    if(status==OK)
                      staticSimplePair->_simple_pair_scan_done(bssInfo);
                    else {
                      DEBUG_PRINTLN("[ Simple-Pair ] ScanSP Failed");
                      staticSimplePair->_sp_status = ST_FINISH;
                      simple_pair_deinit();
                      if(staticSimplePair->_sp_scannotfound_cb != NULL){
                        (* (staticSimplePair->_sp_scannotfound_cb))();
                      }
                    }
                  });
}

String TridentTD_SimplePair::getVersion(){
  return (String)("[TridentTD Simple-Pair] Version ") + String(_version);
}

TridentTD_SimplePair  SimplePair;
