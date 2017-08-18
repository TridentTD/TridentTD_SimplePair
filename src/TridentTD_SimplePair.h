#ifndef __TRIDENTTD_SIMPLE_PAIR_H__
#define __TRIDENTTD_SIMPLE_PAIR_H__

//#define  TRIDENTTD_DEBUG_MODE


#include "Arduino.h"
#include <functional>

extern "C" {
  #include "user_interface.h"
  #include "simple_pair.h"
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

typedef enum {
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
  typedef std::function<void(void)> SimplePairHandler;
  // typedef std::function<void(unsigned int, unsigned int)> OnAPSentDataHandler;
  // typedef std::function<void(unsigned int, unsigned int)> OnStaReceivedDataHandler;

  TridentTD_SimplePair(SimplePairMode_t sp_mode=SP_STA);

  bool              mode(SimplePairMode_t sp_mode);
  SimplePairMode_t  getMode();

  void              annouce();
  void              scanSP();
  bool              begin();
  void              end();
  uint8_t           status();
  bool              isSleep();
  bool              isReceivedData();
  bool              isSentData();

  void              onSentData(SimplePairHandler fn);
  void              onReceivedData(SimplePairHandler fn);
  void              onScanNotFound(SimplePairHandler fn);

  void              tuneon_WiFiStatusLed();
  void              turnoff_WiFiStatusLed();


  bool      setMaxScan(uint8_t max_scan=10);
  
  bool      setPublicKey(uint8_t *pb_key);
  uint8_t*  getPublicKey(uint8_t *pb_key);

  bool      setPrivateKey(uint8_t *pv_key);
  uint8_t*  getPrivateKey(uint8_t *pv_key);

  bool      setPassword16Byte(String password);
  String    getPassword16Byte();

  bool      setSimplePairKey(String sp_key);
  String    getSimplePairKey();

  bool      setSimplePairData(String data);
  String    getSimplePairData();

  bool      setEspNowKey(uint8_t *esp_key)  {return setPrivateKey(esp_key);}
  uint8_t*  getEspNowKey(uint8_t *esp_key)  {return getPrivateKey(esp_key);}
  
  String    getSourceMacAddress();
  uint8_t*  getSourceMacAddress(uint8_t* mac);

  String    getDestinationMacAddress();
  uint8_t*  getDestinationMacAddress(uint8_t* mac);

  String    getBSSID()                      { return this->getSourceMacAddress();}
  uint8_t*  getBSSID(uint8_t* mac)          { return this->getSourceMacAddress(mac); }

  String    getSSID();
  
  String    getVersion();

private:
  SimplePairMode_t    _sp_mode;
  SimplePairStatus_t  _sp_status=ST_SLEEP;

  void    _simple_pair_ap_status_cb();
  void    _simple_pair_sta_status_cb();
  void    _simple_pair_scan_done(bss_info *bss_link); //_simple_pair_scan_done(void *arg, STATUS status);
  void    _simple_pair_scan();

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
  
  uint8_t           _sp_public_key[16]  = {0x4E, 0x61, 0x6D, 0x6F, 0x42, 0x75, 0x64, 0x44, 0x68, 0x61, 0x53, 0x61, 0x6E, 0x67, 0x4D, 0x69};
  uint8_t           _sp_private_key[16] = {0};
  
  float             _version = 1.0;
};


extern TridentTD_SimplePair SimplePair;

String mac2string(u8 *mac_addr);
String key2array(u8 *key, u8 len);
String key2string(u8 *key);

#endif //__TRIDENTTD_SIMPLE_PAIR_H__
