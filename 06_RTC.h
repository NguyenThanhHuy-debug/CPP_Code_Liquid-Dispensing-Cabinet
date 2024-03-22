#ifndef _RTC_h
#define _RTC_h

#include "KingTubeAuto.h"
// #include "Config.h"  // Có define debug để bật/tắt các debug ra Serial.
#include <DS3231.h>      // Thư viện lấy thời gian thực (RTC)
#include <HTTPClient.h>  // Cho phép khai báo kiểu HTTPClient

typedef struct {
  int Thu;
  int Ngay;
  int Thang;
  int Nam;
  int Gio;
  int Phut;
  int Giay;
} ThoiGian;

class RTC {
private:
  HTTPClient http;
  DS3231 ds;

public:
  ThoiGian dt;

public:
  void KhoiTaoServerNTP(void);

private:
  // Lấy RTC từ nguồn server NTP hay IoTVision
  //-------------------------------------------------
  bool LayRTCTuServerNTP(void);
  bool LayRTCTuServerIoTVision(void);
  bool LayRTCTuServer(void);
  //-------------------------------------------------

public:
  void CapNhatRTCChoDS3231(uint8_t thuThuc, uint8_t ngayThuc, uint8_t thangThuc, uint8_t namThuc, uint8_t gioThuc, uint8_t phutThuc, uint8_t giayThuc);
  void CaiDatRTCChoDS3231(void);
  void LayRTCTuServerCaiDatChoDS3231(void);
  void LayRTCTuDS3231(void);
  void HienThiRTCDaCaiDatChoDS3231(ThoiGian dt);
  String ChuanHoaChuoiRTCDeGuiVeServer(void);
  String ChuanHoaChuoiRTC_Ngay(void);
  String ChuanHoaChuoiRTC_Gio(void);
  void SoSanhSaiLechRTCDeCapNhatDS3231(void);
};

#endif
