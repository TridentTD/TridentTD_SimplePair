/********************************************************
 * 
 * ตัวอย่างนี้ เป็นตัวอย่างสำหรับ ส่ง Key ผ่านทาง Simple-Pair เท่านั้น
 * เพื่อใช้สำหรับใช้ใน ESP-NOW ต่อไป
 * 
 *******************************************************/

#include <TridentTD_SimplePair.h>

// ค่า SIMPLE_PAIR_KEY ใช้สำหรับเข้ารหัส Simple-Pair ให้ตรงกัน
// Simple-Pair ที่จะจับคู่กัน ต้องตั้งค่านี้ให้ตรงกัน โดยมีความยาวไม่เกิน 16 ตัวอักษร
#define SIMPLE_PAIR_KEY   "HELLO_SIMPLEPAIR"

uint8_t *espnow_controller_mac;
uint8_t *espnow_slave_mac;
uint8_t *espnow_key;

/* เมื่อ STA ได้รับ ข้อมูล ทาง Simple-Pair เรียบร้อยแล้ว */
void On_SimplePair_ReceivedData(){
  /* ค่า ทั้ง 3 นี้ เอาไว้ใช้ในการ peer ใน ESP-NOW ต่อไป */
  espnow_controller_mac = SimplePair.getSourceMacAddress(espnow_controller_mac);
  espnow_slave_mac      = SimplePair.getDestinationMacAddress(espnow_slave_mac);
  espnow_key            = SimplePair.getEspNowKey(espnow_key);

  Serial.println("-----------------------------------------");
  Serial.println("Info For [ESP-NOW] config");
  Serial.print("Controller MAC :  "); Serial.println( mac2string(espnow_controller_mac));
  Serial.print("Slave MAC      :  "); Serial.println( mac2string(espnow_slave_mac));
  Serial.print("ESP-NOW Key    :  "); Serial.println( key2string(espnow_key) );

  // ทำการเชื่อมต่อทาง ESP-NOW ต่อไป ...
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  Serial.println("Simple-Pair STA Mode");
  
  SimplePair.mode(SP_STA);
  SimplePair.setSimplePairKey(SIMPLE_PAIR_KEY);
  SimplePair.onReceivedData(&On_SimplePair_ReceivedData);
  SimplePair.onScanNotFound([](){ Serial.println("Simple-Pair not found!"); });
  
  SimplePair.scanSP();  // STA ทำการแสกนหา Simple-Pair
}

void loop() {
}