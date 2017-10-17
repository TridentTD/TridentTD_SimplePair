#ifndef __TRIDENTTD_COMMON_H__
#define __TRIDENTTD_COMMON_H__

#include "Arduino.h"

String mac2string(u8 *mac_addr);
String key2array(u8 *key, u8 len);
String key2string(u8 *key);


#endif //__TRIDENTTD_COMMON_H__