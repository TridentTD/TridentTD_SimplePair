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

// กุญแจที่ใช้สำหรับจับคู่ peer กันใน ESP-NOW 
// โดยจะทำการส่งไปทาง Simple-Pair ให้อีกฝั่งนึง
byte ESP_NOW_KEY[16] = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x54, 0x65, 0x73, 
                        0x74, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};


uint8_t *espnow_controller_mac;
uint8_t *espnow_slave_mac;
uint8_t *espnow_key;

/* เมื่อ AP ได้ส่ง ข้อมูล ไปทาง Simple-Pair เรียบร้อยแล้ว */
void On_SimplaPair_SentData(){
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

  Serial.println("Simple-Pair AP Mode");

  SimplePair.mode(SP_AP);
  SimplePair.setSimplePairKey(SIMPLE_PAIR_KEY);
  SimplePair.setEspNowKey(ESP_NOW_KEY); // key สำหรับ จะใช้ใน ESP-NOW เพื่อการจับคู่ โดยจะถูกส่งไปให้ทาง Simple-Pair 
  SimplePair.onSentData(&On_SimplaPair_SentData);
}

void loop() {
  SimplePair.annouce();  // AP ปล่อยสัญญาณ Simple-Pair
}
