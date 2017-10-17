/********************************************************
 * 
 * ตัวอย่างนี้ เป็นตัวอย่างสำหรับ รับ WiFi Password ผ่านทาง Simple-Pair 
 * โดยลูกข่าย WiFi ไม่จำเป็นต้อง มีชื่อ AP และ Password แต่อย่างใด
 * 
 *******************************************************/

#include <ESP8266WiFi.h>
#include <TridentTD_SimplePair.h>

// ค่า SIMPLE_PAIR_KEY ใช้สำหรับเข้ารหัส Simple-Pair ให้ตรงกัน
// Simple-Pair ที่จะจับคู่กัน ต้องตั้งค่านี้ให้ตรงกัน โดยมีความยาวไม่เกิน 16 ตัวอักษร
#define SIMPLE_PAIR_KEY   "HELLO_SIMPLEPAIR"

String AP_SSID;
String AP_PASSWORD;

/* เมื่อ STA ได้รับข้อมูล ทาง Simple-Pair เรียบร้อยแล้ว */
void On_SimplePair_ReceivedData(){
  // รับค่า AP_SSID และ AP_PASSSWORD ผ่านทาง Simple-Pair
  AP_SSID        = SimplePair.getSSID();
  AP_PASSWORD    = SimplePair.getSimplePairData(); 

  uint8_t *AP_BSSID;
  AP_BSSID = SimplePair.getBSSID(AP_BSSID);

  Serial.println("Recv. SSID     : " + AP_SSID);  
  Serial.println("Recv. PASSWORD : " + AP_PASSWORD);
  Serial.println("Recv. BSSID    : " + mac2string(AP_BSSID));
  Serial.println();
}

/* เมื่อ STA ไม่สามารถพบ Simple-Pair ใดๆ */
void On_SimplePair_ScanNotFound(){
  Serial.println("Simple-Pair not found!");
  Serial.println("Change to manual WiFI connection.");

  /* กำหนด AP_SSID และ AP_PASSSWORD ที่รู้จักเอง */
  AP_SSID     = "---- KNOWN SSID -------";
  AP_PASSWORD = "-----KNOWN PASSWORD----";
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  Serial.println("Simple-Pair STA Mode");
  
  SimplePair.mode(SP_STA);                               // กำหนด SimplePair เป็นโหมด STATION
  SimplePair.setSimplePairKey(SIMPLE_PAIR_KEY);          // กำหนด กุญแจ SimplePair Key
  SimplePair.onReceivedData(On_SimplePair_ReceivedData); // กำหนด function เมื่อมีการรับค่าทาง SimplePair
  SimplePair.onScanNotFound(On_SimplePair_ScanNotFound); // กำหนด function เมื่อ scan ไม่พบ SimplePair

  //SimplePair.setWiFiStatusLed(false);                  //หากจะปิด LED แสดงสถานะ WiFi
  
  SimplePair.scanSP();  // โหมด STATION ทำการแสกนหา Simple-Pair ที่ annouce ออกมา

  if( AP_SSID != "") {
    //เชื่อมต่อ WiFi
    WiFi.begin(AP_SSID.c_str(), AP_PASSWORD.c_str());
    while (WiFi.waitForConnectResult() != WL_CONNECTED);
    Serial.println("WiFi connected to ssid " + AP_SSID);
    Serial.print("IP Address is "); Serial.println(WiFi.localIP());
  }
}

void loop() {

}