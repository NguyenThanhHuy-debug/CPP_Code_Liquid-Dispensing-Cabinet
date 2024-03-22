#include "04_Firmware.h"

#include <WiFi.h>        // Thư viện WiFi của ESP32.
#include <ESP32httpUpdate.h>  // Thư viện phục vụ OTA update firmware từ xa
#include <Arduino_JSON.h>       // Thư viện xử lý dữ liệu kiểu JSON

// Lưu ý: file .bin dùng OTA phải thuộc url http:// (không được là https://)
#define URL_CapNhatFirmware "http://App.IoTVision.vn/Assets/UpdateFirmware/KingIncu.bin"
#define URL_GuiPhienBanFirmwareCuaBoardLenServer "https://App.IoTVision.vn/api/LABone_KingIncu_PhienBan"

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//=================================== Begin: FIRMWARE ===============================================//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void Firmware::SETPhienBanFirmware(String version) {
  PhienBan = version;
}
// Kết nối với file firmware (có đuôi .bin) từ URL trên server để tiến hành cập nhật firmware mới
// cho board. Sau khi update thành công thì board sẽ tự động reset để chạy lại từ đầu với firmware mới.
bool Firmware::CapNhatFirmwareChoBoard(void) {
  if (WiFi.status() == WL_CONNECTED) {
    //ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

    t_httpUpdate_return ret = ESPhttpUpdate.update(URL_CapNhatFirmware);
    //Serial.print("ret"); Serial.println(ret);

    if (ret == 0) {
      return false;
    } else
      return true;
  } else
    return false;
}
void Firmware::GuiPhienBanFirmwareCuaBoardLenServer(String ID) {
  if (WiFi.status() == WL_CONNECTED) {
    JSONVar FW;
    FW["ID"] = ID;
    FW["PhienBan"] = PhienBan;
    http.setTimeout(60000);  // Thiết lập thời gian timeout.
    http.begin(URL_GuiPhienBanFirmwareCuaBoardLenServer);
    http.addHeader("Content-Type", "application/json");
    http.POST(JSON.stringify(FW));  //Send the request
    http.end();
  }
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//=================================== End: FIRMWARE =================================================//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
