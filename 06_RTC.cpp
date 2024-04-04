#include "06_RTC.h"

// Thư viện WiFi của ESP32.
#include <WiFi.h>

// Thư viện cho phép lấy thời gian thực chuẩn từ NTP Server.
#include <NTPClient.h>
#include <WiFiUdp.h>

#include <Arduino_JSON.h>  // Thư viện xử lý dữ liệu kiểu JSON

// Nếu define NTPServer thì RTC sẽ được lấy từ nguồn server NTP.
//#define NTPServer

// fixed max size for server response
#define _INSIZE 512

//-----------------------------------------------------------------------------------
// Lấy thời gian thực từ NTP Server
WiFiUDP ntpUDP;

// You can specify the time server pool and the offset, (in seconds)
// additionally you can specify the update interval (in milliseconds).
// GMT +7 = 25200
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", 25200, 60000);
//-----------------------------------------------------------------------------------

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//------------------ BEGIN: Lấy RTC từ nguồn server NTP hay IoTVision  ------------------------------//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void RTC::KhoiTaoServerNTP(void) {
  timeClient.begin();
}
// Lấy thời gian thực từ NTP Server.
//-----------------------------------------------------------------------------------------------------
bool RTC::LayRTCTuServerNTP(void) {
  if (WiFi.status() == WL_CONNECTED) {

#if defined(debug) && defined(debug_RTC) 
    Serial.println("Đang lấy RTC từ server NTP...");
#endif

    timeClient.update();
    unsigned long epochTime = timeClient.getEpochTime();
    int currentHour = timeClient.getHours();
    int currentMinute = timeClient.getMinutes();
    int currentSecond = timeClient.getSeconds();
    int currentweekDay = timeClient.getDay();

    //Get a time structure
    struct tm *ptm = gmtime((time_t *)&epochTime);
    int monthDay = ptm->tm_mday;
    int currentMonth = ptm->tm_mon + 1;
    int currentYear = ptm->tm_year + 1900;

    // Chuyển RTC dạng JSON và kiểu dữ liệu
    // tự định nghĩa ThoiGian.
    //------------------------------------------------------
    dt.Thu = currentweekDay;  // CN = 0, Thứ 2 = 1,...
    dt.Ngay = monthDay;
    dt.Thang = currentMonth;
    dt.Nam = currentYear;
    dt.Gio = currentHour;
    dt.Phut = currentMinute;
    dt.Giay = currentSecond;
    //------------------------------------------------------

#if defined(debug) && defined(debug_RTC) 
    Serial.println("Thời gian trên NTP Server: " + String(dt.Thu) + " " + String(dt.Ngay) + "/" + String(dt.Thang) + "/" + String(dt.Nam) + " " + String(dt.Gio) + ":" + String(dt.Phut) + ":" + String(dt.Giay));
#endif

    return true;
  } else {
#if defined(debug) && defined(debug_RTC) 
    Serial.println("Không có WiFi để kết nối vào server NTP để lấy RTC");
#endif
  }

  return false;
}
//-----------------------------------------------------------------------------------------------------
// Lấy thời gian thực từ IoTVision's Server.
bool RTC::LayRTCTuServerIoTVision(void) {
  if (WiFi.status() == WL_CONNECTED) {

#if defined(debug) && defined(debug_RTC) 
    Serial.println("Đang lấy RTC từ server IoTVision...");
#endif

    String url = "https://App.IoTVision.vn/api/ThoiGian";
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    int httpCodeGet = http.GET();
    if (httpCodeGet == HTTP_CODE_OK) {  // needs a 200 to continue...
      int z = http.getSize();           // how big is the payload?
      if (z < _INSIZE) {                // payload must fit in the available space.
        String thoigianthuc = http.getString();

#if defined(debug) && defined(debug_RTC) 
        Serial.println("-------------------------------------------------------------");
        Serial.println("Thời thực lấy từ server IoTVision: ");
        Serial.println(thoigianthuc);
        Serial.println("-------------------------------------------------------------");
#endif

        if (thoigianthuc.length() > 0) {
          // Chuyển chuỗi thoigianthuc sang dạng JSON
          JSONVar RTC = JSON.parse(thoigianthuc);

          // Chuyển RTC dạng JSON và kiểu dữ liệu
          // tự định nghĩa ThoiGian.
          //------------------------------------------------------
          dt.Thu = (int)RTC["Thu"];  // CN = 0, Thứ 2 = 1,...
          dt.Ngay = (int)RTC["Ngay"];
          dt.Thang = (int)RTC["Thang"];
          dt.Nam = (int)RTC["Nam"];
          dt.Gio = (int)RTC["Gio"];
          dt.Phut = (int)RTC["Phut"];
          dt.Giay = (int)RTC["Giay"];

          // Lấy 2 số cuối của năm, vdu: 2020 thì chỉ lấy số 20.
          dt.Nam -= 2000;
          //------------------------------------------------------

          http.end();
          return true;
        }
      }
    }
    http.end();
  } else {
#if defined(debug) && defined(debug_RTC) 
    Serial.println("Không có WiFi để kết nối vào server IoTVision để lấy RTC");
#endif
  }

  return false;
}
//-----------------------------------------------------------------------------------------------------
bool RTC::LayRTCTuServer(void) {
#ifdef NTPServer
  // Lấy RTC từ nguồn server NTP.
  return LayRTCTuServerNTP();
#else
  // Lấy RTC từ nguồn server IoTVision

  return LayRTCTuServerIoTVision();
#endif
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//------------------ END: Lấy RTC từ nguồn server NTP hay IoTVision  --------------------------------//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
///////////////////////////// BEGIN: Lấy thời gian thực từ mô-đun RTC  ////////////////////////////////
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// Cập nhật RTC ngày/tháng/năm, giờ,phút,giây
void RTC::CapNhatRTCChoDS3231(uint8_t thuThuc, uint8_t ngayThuc, uint8_t thangThuc, uint8_t namThuc, uint8_t gioThuc, uint8_t phutThuc, uint8_t giayThuc)
{
  ds.setClockMode(false);  // set to 24h
  ds.setYear(namThuc);
  ds.setMonth(thangThuc);
  ds.setDate(ngayThuc);
  ds.setDoW(thuThuc);  // CN = 0, Thứ 2 = 1,...
  ds.setHour(gioThuc);
  ds.setMinute(phutThuc);
  ds.setSecond(giayThuc);
}
// Cài đặt thời gian cho DS3231.
void RTC::CaiDatRTCChoDS3231(void) {
  ds.setClockMode(false);  // set to 24h
  ds.setYear(dt.Nam);
  ds.setMonth(dt.Thang);
  ds.setDate(dt.Ngay);
  ds.setDoW(dt.Thu);  // CN = 0, Thứ 2 = 1,...
  ds.setHour(dt.Gio);
  ds.setMinute(dt.Phut);
  ds.setSecond(dt.Giay);
}
// Lấy thời gian thực từ server rồi cập nhật thời gian thực đó cho mô-đun RTC.
void RTC::LayRTCTuServerCaiDatChoDS3231(void) {
  if (LayRTCTuServer() == true)
    CaiDatRTCChoDS3231();  // Cài đặt giá trị RTC cho mô-đun RTC DS3231.
}
// Lấy thời gian thực từ mô-đun RTC.
void RTC::LayRTCTuDS3231(void) {
  bool century = false;
  bool h12Flag;
  bool pmFlag;
  dt.Thu = ds.getDoW();
  dt.Ngay = ds.getDate();
  dt.Thang = ds.getMonth(century);
  dt.Nam = ds.getYear();
  dt.Gio = ds.getHour(h12Flag, pmFlag);
  dt.Phut = ds.getMinute();
  dt.Giay = ds.getSecond();

  // Cộng thêm 2000 để ra đúng số năm, vdu: 20 + 2000 = 2020: năm 2020
  dt.Nam += 2000;
}
void RTC::HienThiRTCDaCaiDatChoDS3231(ThoiGian dt) {
  // Hiển thị RTC để người cài đặt biết mô-đun RTC DS3231
  // đã được setup real-time hay chưa.
  Serial.println("-------------------------------------------------------------");
  Serial.print("RTC onboard: Thứ ");
  Serial.print(dt.Thu);
  Serial.print(", ");
  Serial.print(dt.Ngay);
  Serial.print("/");
  Serial.print(dt.Thang);
  Serial.print("/");
  Serial.print(dt.Nam);
  Serial.print(" ");
  Serial.print(dt.Gio);
  Serial.print(":");
  Serial.print(dt.Phut);
  Serial.print(":");
  Serial.print(dt.Giay);
  Serial.println();
  Serial.println("-------------------------------------------------------------");
}
String RTC::ChuanHoaChuoiRTCDeGuiVeServer(void) {
  // Thêm dữ liệu thời gian RTC của DS1307 trên board gửi về server để có thể
  // đối chiếu kịp thời phát hiện sai lệch thời gian giữa board và server, từ
  // đó user có thể thực hiện update lại RTC cho DS1307 thông qua tính năng
  // cập nhật thông số board trên app.
  String Gio = String(dt.Gio);
  String Phut = String(dt.Phut);
  String Giay = String(dt.Giay);
  String Ngay = String(dt.Ngay);
  String Thang = String(dt.Thang);
  String Nam = String(dt.Nam);
  if (dt.Gio < 10) Gio = "0" + Gio;
  if (dt.Phut < 10) Phut = "0" + Phut;
  if (dt.Giay < 10) Giay = "0" + Giay;
  if (dt.Ngay < 10) Ngay = "0" + Ngay;
  if (dt.Thang < 10) Thang = "0" + Thang;

  String RTC = Gio + ":" + Phut + ":" + Giay + " " + Ngay + "/" + Thang + "/" + Nam;

#if defined(debug) && defined(debug_RTC) 
  Serial.print("Thời gian RTC trên DS3231 đã chuẩn hóa: ");
  Serial.println(RTC);
#endif

  return RTC;
}
String RTC::ChuanHoaChuoiRTC_Ngay(void) {
  String Ngay = String(dt.Ngay);
  String Thang = String(dt.Thang);
  String Nam = String(dt.Nam).substring(2);
  if (dt.Ngay < 10) Ngay = "0" + Ngay;
  if (dt.Thang < 10) Thang = "0" + Thang;

  String RTC = Ngay + "/" + Thang + "/" + Nam;
  return RTC;
}
String RTC::ChuanHoaChuoiRTC_Gio(void) {
  String Gio = String(dt.Gio);
  String Phut = String(dt.Phut);
  //String Giay = String(dt.Giay);
  if (dt.Gio < 10) Gio = "0" + Gio;
  if (dt.Phut < 10) Phut = "0" + Phut;
  //if (dt.Giay < 10) Giay = "0" + Giay;

  String RTC = Gio + ":" + Phut;// + ":" + Giay;
  return RTC;
}
void RTC::SoSanhSaiLechRTCDeCapNhatDS3231(void) {
  // RTC từ server sẽ được cập nhật tại biến _RTC khi gọi hàm LayRTCTuServer().
  if (LayRTCTuServer() == true) {
    // Lấy thời gian thực từ DS3231.
    //-----------------------------------------------------------------------
    ThoiGian t;
    bool century = false;
    bool h12Flag;
    bool pmFlag;
    t.Thu = ds.getDoW();
    t.Ngay = ds.getDate();
    t.Thang = ds.getMonth(century);
    t.Nam = ds.getYear();
    t.Gio = ds.getHour(h12Flag, pmFlag);
    t.Phut = ds.getMinute();
    t.Giay = ds.getSecond();

    // Cộng thêm 2000 để ra đúng số năm, vdu: 20 + 2000 = 2020: năm 2020
    t.Nam += 2000;
    //-----------------------------------------------------------------------

#if defined(debug) && defined(debug_RTC) 
    Serial.println("Thời gian trên internet: " + String(dt.Thu) + " " + String(dt.Ngay) + "/" + String(dt.Thang) + "/" + String(dt.Nam + 2000) + " " + String(dt.Gio) + ":" + String(dt.Phut) + ":" + String(dt.Giay));
    Serial.println("Thời gian trên RTC DS3231: " + String(t.Thu) + " " + String(t.Ngay) + "/" + String(t.Thang) + "/" + String(t.Nam) + " " + String(t.Gio) + ":" + String(t.Phut) + ":" + String(t.Giay));
#endif

    //-----------------------------------------------------------------------
    int tInternet = dt.Gio * 60 + dt.Phut;
    int tDS3231 = t.Gio * 60 + t.Phut;
    // Nếu sai lệch quá 3 phút thì cập nhật lại DS3231 theo giờ internet.
    // Hoặc là ngày || tháng || năm khác nhau thì cũng cập nhật lại DS3231
    if (abs(tInternet - tDS3231) > 3 || dt.Thu != t.Thu || dt.Ngay != t.Ngay || dt.Thang != t.Thang || (dt.Nam + 2000) != t.Nam) {
#if defined(debug) && defined(debug_RTC) 
      Serial.println("Có sai lệch RTC giữa internet và DS3231, cập nhật RTC cho DS3231...");
#endif
      CaiDatRTCChoDS3231();
    } else {
#if defined(debug) && defined(debug_RTC) 
      Serial.println("Không có sai lệch RTC giữa internet và DS3231.");
#endif
    }
    //-----------------------------------------------------------------------
  }
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
/////////////////////////////// END: Lấy thời gian thực từ mô-đun RTC  ////////////////////////////////
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
