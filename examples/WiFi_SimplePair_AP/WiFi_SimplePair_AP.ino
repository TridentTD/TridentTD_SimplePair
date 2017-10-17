/********************************************************
 * 
 * ตัวอย่างนี้ เป็นตัวอย่างสำหรับ ส่ง WiFi Password ผ่านทาง Simple-Pair 
 * โดยลูกข่าย WiFi ไม่จำเป็นต้อง จำค่า Password แต่อย่างใด
 * 
 *******************************************************/

#include <ESP8266WiFi.h>
#include <TridentTD_SimplePair.h>

// ค่า SIMPLE_PAIR_KEY ใช้สำหรับเข้ารหัส Simple-Pair ให้ตรงกัน
// Simple-Pair ที่จะจับคู่กัน ต้องตั้งค่านี้ให้ตรงกัน โดยมีความยาวไม่เกิน 16 ตัวอักษร
#define SIMPLE_PAIR_KEY   "HELLO_SIMPLEPAIR"

String AP_SSID          = "TridentTD_AP";
String AP_PASSWORD      = "9876543210123456";  // พาสเวิร์ดของ AP นี้จะถูกส่งไปให้ทาง Simple-Pair 
                                               // โดยมีความยาว 16 ตัวอักษรสูงสุด

void setup() {
  Serial.begin(115200);
  Serial.println();

  WiFi.softAPdisconnect(true);
  WiFi.softAP(AP_SSID.c_str(), AP_PASSWORD.c_str());  // ทำให้ ESP8266 เป็นแม่ข่าย WiFi softAP

  Serial.println("Simple-Pair AP Mode");
  SimplePair.mode(SP_AP);                         // กำหนด SimplePair เป็นโหมด AccessPoint เพื่อเป็นฝั่งส่งข้อมูล
  SimplePair.setSimplePairKey(SIMPLE_PAIR_KEY);   // กำหนด กุญแจ SimplePair Key
  SimplePair.setSimplePairData(AP_PASSWORD);      // ข้อมูล คือ พาสเวิร์ดของ AP นี้จะถูกส่งไปให้ทาง Simple-Pair 

  //SimplePair.setWiFiStatusLed(false);   //หากจะปิด LED แสดงสถานะ WiFi

}

void loop() {
  SimplePair.annouce();  // โหมด SP_AP ปล่อยสัญญาณ Simple-Pair เพื่อกระจายข้อมูล AP_PASSWORD ออกไป
}
