#ifndef _Firmware_h
#define _Firmware_h

#include "KingTubeAuto.h"  // Có define debug để bật/tắt các debug ra Serial.
#include <Arduino.h>     // Để khai báo kiểu String
#include <HTTPClient.h>  // Cho phép khai báo kiểu HTTPClient

class Firmware {
private:
  HTTPClient http;

public:
  String PhienBan;

public:
  void SETPhienBanFirmware(String version);
  bool CapNhatFirmwareChoBoard(void);
  void GuiPhienBanFirmwareCuaBoardLenServer(String ID);
};

#endif
