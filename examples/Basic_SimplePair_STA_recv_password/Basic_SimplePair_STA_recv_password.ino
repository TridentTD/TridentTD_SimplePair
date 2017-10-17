/********************************************************
 * 
 * ตัวอย่างนี้ เป็นตัวอย่างสำหรับ รับ WiFi Password ผ่านทาง Simple-Pair 
 * เพื่อนำไปใช้ เชื่อมต่อกับ Router ที่รู้จักอยู่ก่อนแล้ว
 * 
 *******************************************************/

#include <ESP8266WiFi.h>
#include <TridentTD_SimplePair.h>

// ค่า SIMPLE_PAIR_KEY ใช้สำหรับเข้ารหัส Simple-Pair ให้ตรงกัน
// Simple-Pair ที่จะจับคู่กัน ต้องตั้งค่านี้ให้ตรงกัน โดยมีความยาวไม่เกิน 16 ตัวอักษร
#define SIMPLE_PAIR_KEY   "HELLO_SIMPLEPAIR"

String SSID = "---YOUR ROUTER SSID---";  // ใส่ชื่อ SSID ที่ต้องการเชื่อมต่อ
String PASSWORD;                         // PASSWORD ไม่ต้องกำหนด จะทำการรับจาก SimplePair แทน

void setup() {
  Serial.begin(115200);
  Serial.println();
  
  //ตัดการเชื่อมต่อ WiFi เดิมๆที่เคยจำไว้ออก
  WiFi.persistent(true);
  WiFi.disconnect(true);

  Serial.println("Simple-Pair STA Mode");
  
  SimplePair.mode(SP_STA);                               // กำหนด SimplePair เป็นโหมด STATION
  SimplePair.setSimplePairKey(SIMPLE_PAIR_KEY);          // กำหนด กุญแจ SimplePair Key
  SimplePair.onReceivedData([](){                        // เมื่อมีการรับค่าทาง SimplePair เข้ามา
      PASSWORD    = SimplePair.getSimplePairData();      // รับข้อมูลที่ส่งมาทาง SimplePair
      Serial.println("Recv. PASSWORD : " + PASSWORD);
    });

  Serial.println("Simple-Pair scaning...");
  SimplePair.scanSP();  // โหมด STATION ทำการเริ่มแสกนหา Simple-Pair ที่ annouce ออกมา

  if( PASSWORD != "") {
    //เชื่อมต่อ WiFi
    Serial.println("WiFi connecting...");
    WiFi.begin(SSID.c_str(), PASSWORD.c_str());
    while (WiFi.status() != WL_CONNECTED) {
      delay(300); Serial.print(".");
    }
    Serial.println("\nWiFi connected to ssid " + SSID);
    Serial.print("IP Address is "); Serial.println(WiFi.localIP());
  }
}

void loop() {
}
