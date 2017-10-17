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


#ifndef __TRIDENTTD_SIMPLE_PAIR_H__
#define __TRIDENTTD_SIMPLE_PAIR_H__

//#define  TRIDENTTD_DEBUG_MODE


#include "Arduino.h"
#include "TridentTD_common.h"

extern "C" {
  #include "user_interface.h"
  #include "simple_pair.h"
  #include "espnow.h"
}


#ifdef TRIDENTTD_DEBUG_MODE
    #define DEBUG_PRINTER Serial
    #define DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
    #define DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
    #define DEBUG_PRINTF(fmt, args...) { DEBUG_PRINTER.printf(fmt,## args); }
#else
    #define DEBUG_PRINT(...) { }
    #define DEBUG_PRINTLN(...) { }
    #define DEBUG_PRINTF(fmt, args...) { }
#endif

typedef enum SimplePairMode 
{
  SP_OFF = 0, SP_STA = 1, SP_AP = 2
} SimplePairMode_t;

typedef enum 
{
  ST_BEGIN  = -1,
  ST_FINISH = 0, // AP or STATION finished negotiation
  ST_SLEEP  = 0,
  ST_AP_RECV_NEG, // AP received a request of negotiation from station
  ST_AP_REFUSE_NEG, // station received the refusal to negotiate from AP
  /* definitions below are error codes */
  ST_WAIT_TIMEOUT, // Error: time out
  ST_SEND_ERROR, // Error: error occur when sending data
  ST_KEY_INSTALL_ERR, // Error: error occur during key installation
  ST_KEY_OVERLAP_ERR, // Error: one MAC address uses multiple keys
  ST_OP_ERROR, // Error: operational error
  ST_UNKNOWN_ERROR, // Error: unknown error

  ST_MAX,
} SimplePairStatus_t;


  
class TridentTD_SimplePair {
public:
  typedef void (*SimplePairHandler)(void);

  TridentTD_SimplePair(SimplePairMode_t sp_mode=SP_STA);

  bool              mode(SimplePairMode_t sp_mode);
  SimplePairMode_t  getMode();
  void              setWiFiStatusLed(bool turn_on);

  void              annouce();
  void              scanSP();
  bool              setMaxScan(uint8_t max_scan=10);

  void              onSentData(SimplePairHandler fn);
  void              onReceivedData(SimplePairHandler fn);
  void              onScanNotFound(SimplePairHandler fn);

  //---------- SimplePair Key function -------------
  bool              setSimplePairKey(String sp_key);
  bool              setSimplePairKey(uint8_t* sp_key);
  bool              setSimplePairKey(char* sp_key);

  String            getSimplePairKey();
  uint8_t*          getSimplePairKey(uint8_t *sp_key);
  char*             getSimplePairKey(char *sp_key);

  //---------- SimplePair Data function ------------
  bool              setSimplePairData(String data);
  bool              setSimplePairData(uint8_t* data);  
  bool              setSimplePairData(char* data);

  String            getSimplePairData();
  uint8_t*          getSimplePairData(uint8_t *data);
  char*             getSimplePairData(char *data);

  //---------- ESPNOW KEY (alias to SimplePair Data) function ------------
  bool              setEspNowKey(String esp_key)    {return setSimplePairData(esp_key);}
  bool              setEspNowKey(uint8_t *esp_key)  {return setSimplePairData(esp_key);}
  bool              setEspNowKey(char *esp_key)     {return setSimplePairData(esp_key);}
  String            getEspNowKey()                  {return getSimplePairData(); }
  uint8_t*          getEspNowKey(uint8_t *esp_key)  {return getSimplePairData(esp_key);}
  char*             getEspNowKey(char *esp_key)     {return getSimplePairData(esp_key);}

  //---------- SimplePair SP_AC Mac
  String            getSourceMacAddress();
  uint8_t*          getSourceMacAddress(uint8_t* mac);

  //---------- SimplePair SP_STA Mac
  String            getDestinationMacAddress();
  uint8_t*          getDestinationMacAddress(uint8_t* mac);

  //---------- When SP_AP mode config WiFi AccessPoint, another SP_STA can receive BSSID and SSID too
  String            getBSSID()                      { return this->getSourceMacAddress();}
  uint8_t*          getBSSID(uint8_t* mac)          { return this->getSourceMacAddress(mac); }

  String            getSSID();
  

  String            getVersion();

private:
  SimplePairMode_t    _sp_mode;
  SimplePairStatus_t  _sp_status=ST_SLEEP;

  void              _simple_pair_ap_status_cb();
  void              _simple_pair_sta_status_cb();
  void              _simple_pair_scan_done(struct bss_info *bss_link); //_simple_pair_scan_done(void *arg, STATUS status);
  void              _simple_pair_scan();

  void              _turnon_WiFiStatusLed();
  void              _turnoff_WiFiStatusLed();

  SimplePairHandler _sp_sentdata_cb;
  SimplePairHandler _sp_receiveddata_cb;
  SimplePairHandler _sp_scannotfound_cb;

  //uint32_t          _sp_timer;
  bool              _sp_ap_found = true;
  bool              _sp_sta_negotiate = true;
  uint8_t           _sp_ap_mac[6];
  uint8_t           _sp_sta_mac[6];
  uint8_t           _sp_scan_cnt=0;
  uint8_t           _sp_max_scan=10;
  
  uint8_t           _sp_ssid[32];
  uint8_t           _sp_ssid_len;
  
  uint8_t           _sp_key[16]  = {0x4E, 0x61, 0x6D, 0x6F, 0x42, 0x75, 0x64, 0x44, 0x68, 0x61, 0x53, 0x61, 0x6E, 0x67, 0x4D, 0x69};
  uint8_t           _sp_data[16] = {0};

  bool              _wifi_status_led_turnon=true;

  bool              begin();
  void              end();
  uint8_t           status();
  bool              isSleep();
  bool              isReceivedData();
  bool              isSentData();
  
  float             _version = 1.1;
};
extern TridentTD_SimplePair SimplePair;

#endif //__TRIDENTTD_SIMPLE_PAIR_H__
