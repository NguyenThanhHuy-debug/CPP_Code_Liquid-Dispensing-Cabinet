#ifndef _WiFi_h
#define _WiFi_h

#include "KingTubeAuto.h"
#include <Arduino.h>    // Để khai báo kiểu String

#ifndef build_test_chay_khong_Wifi

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//============================ Begin: classWiFi =====================================================//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
#pragma region classWiFi
class WIFI {
private:
  static String TenWiFi;
  static String MatKhauWiFi;
  static bool DaBatAP;
  static char AP_SSID[25];
  static String AP_PASS;
  TaskHandle_t xTaskHandle_Wifi;

public:
  
  String LaySoMAC(void);
  void DocWiFiDaLuuTrongEEPROM(void);
  void KetNoiWiFi(int ThoiGianChoKetNoi);
  void NgatKetNoiWiFi();
  static int TinhDoManhCuaWiFi(void);
  void CapNhatWiFiVaoEEPROM(String TenWiFi, String MatKhauWiFi);

private:
  static void KiemTraKetNoiWiFi(void);
  static void ThietLapAP(void);
  static void TrangChu(void);
  static void StyleCSS(void);
  static void CodeJS(void);
  static void QuetTimWiFi(void);
  static void LuuWiFiVaoEEPROM(void);
  static void ServerON(void);
  static void TaskKiemTraKetNoiWiFi(void *pvParameter);
  static String rssiToIcon(int rssi);
};
#pragma endregion classWiFi
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//============================ End: classWiFi =======================================================//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

#endif //build_test_chay_khong_Wifi

#endif 