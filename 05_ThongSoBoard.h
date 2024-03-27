#pragma once
#ifndef _ThongSoBoard_h
#define _ThongSoBoard_h

#include "_GRBL.h"
#include <Arduino.h>       // Để khai báo kiểu String
#include <Arduino_JSON.h>  // Thư viện xử lý dữ liệu kiểu JSON
// #define debug
// // Từ vị trí 0-409 là vùng EEPROM chứa thông số board.
#define _ChieuDaiMaxThongSoBoard 409
#define _ChieuDaiI2CEEPROM 1700
#define _SoChuongTrinh 10  // Số chương trình cài đặt để user chọn khi chạy máy
#define _ChieuDaiMaxTenChuongTrinh 20
#define _KyTu '\r'

class ThongSoBoard {
public:
  String ID = "MAC123456789";  // Số ID của ESP32, đây là số IMEI của board.

  //------------------------------------------------------------------------------------------------//
  JSONVar ThongSo;
  //  Thông số board sẽ được load từ database trên server và lưu vào bộ nhớ EFROM của
  //  ESP32. Việc cài đặt này sẽ được thực hiện thông qua App. Thông số board bao gồm:
  //  String  ID;
  //  vector<ChuongTrinh_t> DSChuongTrinh;

  //------------------------------------------------------------------------------------------------//

private:
  void LayThongSoBoardTuROM(void);
  void ThietLapThongSoBoardMacDinh(void);
  
public:
  void KhoiTao(void);
  void XoaEEPROM(int start, int end);
  String TachChuoi(String data, char separator, int index);

  void CaiDatThongSoBoardVaoROM(JSONVar ThongSo);
  bool GuiThongSoBoardVeServer(void);
  String LayThongSoBoardTuServer(void);
  bool LayThongSoBoardTuServerRoiCaiDatVaoROM(void);
  String ChuyenChuongTrinhThanhJsonString(ChuongTrinh_t ChuongTrinh);
  ChuongTrinh_t ChuyenDoiJSONVarSangChuongTrinh(JSONVar data,uint8_t idxchuongtrinh);
  JSONVar ChuyenDoiChuongTrinhSangJsonVar(vector<ChuongTrinh_t> DsChuongTrinhCanChuyenDoi);

  String DienVaoChoTrong(String Chuoi, size_t DoDaiChuoi = _ChieuDaiMaxTenChuongTrinh, char KyTuCanDien = _KyTu);
};

#endif