#pragma region Ghi chú
#ifdef GhiChu
//=========================================================================================//
// - Phiên bản kết nối cấu hình WiFi cho board được hoàn thành vào ngày 21 / 10 / 2023.
// - Sau khi kết nối được access point(AP) do board phát ra thì vào trình duyệt web
// để vào trang web cấu hình WiFi cho board với url = "192.168.9.1".
// - Việc kiểm tra WiFi còn hay mất sẽ được thực hiện trong Task RTOS.
// - Chỉ cần gọi _WiFi.DocWiFiDaLuuTrongEEPROM() và _WiFi.KetNoiWiFi(10) ở chương
// trình chính phần khởi tạo.
//=========================================================================================//  
#endif
#pragma endregion Ghi chú


#include "02_WIFI.h"      //
#include <EEPROM.h>       // Thư viện để lưu dữ liệu thông tin WiFi vào bộ nhớ ROM.
#include <WiFi.h>         // Thư viện WiFi của ESP32.
#include <WebServer.h>    //
#include <WiFiAP.h>       //
#include "WEBConfig.h"    // Giao diện HTML trang cấu hình WiFi cho board.
#include "ArduinoJson.h"  // Xử lý kiểu dữ liệu JSON

#ifndef build_test_chay_khong_Wifi

//Phần bắt đầu của WiFi ConFig SSID, để phân biệt các board khác dự án.
//----------------------------------------------------------------------
#define _TenDuAn "LABone@"
#define EEPROM_SSID 415
#define EEPROM_PASS 447

WebServer server(80);
IPAddress ip_ap(192, 168, 9, 1);
IPAddress gateway_ap(192, 168, 9, 1);
IPAddress subnet_ap(255, 255, 255, 0);
//----------------------------------------------------------------------
//===============================================================================
// Những biến là static nên cần phải được khai báo lại tại
// đầu file .cpp theo cú pháp sau thì mới truy xuất được đến biến static.
String WIFI::TenWiFi = "";
String WIFI::MatKhauWiFi = "";
bool WIFI::DaBatAP = false;
char WIFI::AP_SSID[25];
String WIFI::AP_PASS = "LABone@2023";
//===============================================================================
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
String WIFI::LaySoMAC(void) {
#pragma region LaySoMAC
  //===============================================================================
  //---------------------------------------------------------------
  // Lấy số MAC của ESP32, đây là số IMEI của board.
  //---------------------------------------------------------------
  String MAC;
  MAC = WiFi.macAddress();
  MAC.replace(":", "");

#if defined(debug) && defined(DEBUG_WIFI) 

  Serial.println("");
  Serial.print("MAC: ");
  Serial.println(MAC);
#endif

  // Phát WiFi với số MAC của thiết bị để user biết WiFi là của
  // thiết bị nào đang phát ra để mà cấu hình WiFi cho đúng với thiết bị đó.
  String str = _TenDuAn + MAC;
  str.toCharArray(WIFI::AP_SSID, 24);

#if defined(debug) && defined(DEBUG_WIFI) 

  Serial.println(WIFI::AP_SSID);
#endif
  //---------------------------------------------------------------

  return MAC;
  //===============================================================================
#pragma endregion LaySoMAC
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void WIFI::DocWiFiDaLuuTrongEEPROM(void) {
#pragma region DocWiFiDaLuuTrongEEPROM
  TenWiFi = "";
  MatKhauWiFi = "";
  //===============================================================================
  if (EEPROM.read(EEPROM_SSID) != 0) {
    for (int i = EEPROM_SSID; i < EEPROM_PASS; ++i) {
      TenWiFi += char(EEPROM.read(i));
    }

    for (int i = EEPROM_PASS; i < 512; ++i) {
      MatKhauWiFi += char(EEPROM.read(i));
    }

    TenWiFi = TenWiFi.c_str();
    MatKhauWiFi = MatKhauWiFi.c_str();

#if defined(debug) && defined(DEBUG_WIFI) 
    Serial.print("Tên WiFi: ");
    Serial.println(TenWiFi);
    Serial.print("Mật khẩu: ");
    Serial.println(MatKhauWiFi);
#endif
  }
  //===============================================================================
#pragma endregion DocWiFiDaLuuTrongEEPROM
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void WIFI::KetNoiWiFi(int ThoiGianChoKetNoi) {
#pragma region KetNoiWiFi
  //===============================================================================
  // Xóa các cấu hình WiFi cũ đã lưu trước đó trong ESP.
  //ESP.eraseConfig();

  // Chuyển sang MODE STA (Stations) để dừng phát Access Point.
  WiFi.mode(WIFI_STA);
  delay(250);

  // Bắt đầu kết nối WiFi.
  WiFi.begin(TenWiFi, MatKhauWiFi);
  Serial.print("Đang kết nối WiFi... ");

  // Kiểm tra kết nối WiFi
  //---------------------------------------------------------------
  uint16_t count = 0;
  while (count < ThoiGianChoKetNoi) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Kết nối WiFi thành công!");
      break;
    }
    delay(1000);
    count++;
    Serial.print(count);
    Serial.print(" > ");
  }
  if (count >= ThoiGianChoKetNoi) {
    Serial.println("");
    Serial.println("Đã hết thời gian, kết nối WiFi thất bại!");
  }
  //---------------------------------------------------------------
  //===============================================================================

  //-----------------------------------------------------------------------------------
  // create a task that will be executed in the Task1code() function,
  // with priority 1 and executed on core 1
  //#ifdef xTaskKiemTraKetNoiWiFi
  xTaskCreatePinnedToCore(
    TaskKiemTraKetNoiWiFi,    /* Task function. */
    "Task KiemTraKetNoiWiFi", /* name of task. */
    4096,                     /* Stack size of task */
    NULL,                     /* parameter of the task */
    1,                        /* priority of the task */
    &(this->xTaskHandle_Wifi),                     /* Task handle to keep track of created task */
    1);                       /* pin task to core 1 */
  delay(500);
  //#endif
  //-----------------------------------------------------------------------------------
#pragma endregion KetNoiWiFi
}

void WIFI::NgatKetNoiWiFi() {
  // ngắt kết nối wifi 
  WiFi.disconnect();
#if defined(debug) && defined(DEBUG_WIFI)
  Serial.println("ngắt kết nối wifi");
#endif
  //* esp_wifi_disconnect();
  //* esp_wifi_stop();
  //* esp_wifi_deinit();

  // hủy task kiểm tra wifi 
  if (this->xTaskHandle_Wifi != NULL) {
    vTaskDelete(this->xTaskHandle_Wifi);
  }
}


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void WIFI::KiemTraKetNoiWiFi(void) {
#pragma region KiemTraKetNoiWiFi
  //===============================================================================
#if defined(debug) && defined(DEBUG_WIFI) 

  Serial.println("Kiểm tra kết nối WiFi...");
  Serial.print("WiFi.softAPIP:");
  Serial.println(WiFi.softAPIP().toString());
  Serial.print("ip_ap:");
  Serial.println(ip_ap.toString());
  Serial.print("Độ mạnh của WiFi:");
  Serial.println(WIFI::TinhDoManhCuaWiFi());
#endif

  if (WiFi.status() != WL_CONNECTED) {
    // Nếu ESP mất kết nối WiFi thì sẽ tự động phát Access Point (AP)
    // để điện thoại có thể quét được WiFi phát ra của board, nhờ đó user
    // có thể kết nối web cấu hình WiFi của board để tiến hành cài đặt thông số WiFi mới (nếu muốn).
    ThietLapAP();
  }
  else {
    // Trường hợp có kết nối WiFi ok, thì kiểm tra xem trước đó
    // cờ bật Access Point có bật không (true), nếu trước đó cờ có bật
    // thì sẽ chuyển mode WiFi của ESP32 sang mode STA, lúc này ESP32
    // sẽ kết nối WiFi với mạng WiFi đang có trong nhà.
    if (WIFI::DaBatAP == true) {
      // Chuyển sang MODE STA (Stations) để dừng phát Access Point
      WiFi.mode(WIFI_STA);
      Serial.println("Kết nối WiFi OK. Tắt chế độ phát Access Point.");

      // Tắt cờ trạng thái đang thiết lập AP.
      WIFI::DaBatAP = false;
    }
  }

  if (WIFI::DaBatAP == true) {
    // Nếu như mà kết nối WiFi bị mất thì board sẽ phát ra WiFi của board,
    // tức là phát access point (AP), lúc này gọi hàm handleClient() để server
    // lắng nghe các thao tác từ trang web dùng để config WiFi.
    // CHÚ Ý: Luôn luôn phải gọi hàm này thì mới mở được
    // trang web WEBConfig mỗi khi board phát AP.
    server.handleClient();
  }
  //===============================================================================
#pragma endregion KiemTraKetNoiWiFi
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void WIFI::TaskKiemTraKetNoiWiFi(void* pvParameter) {
#pragma region TaskKiemTraKetNoiWiFi
#if defined(debug) && defined(DEBUG_WIFI) 

  Serial.print("TaskKiemTraKetNoiWiFi đang chạy ở core ");
  Serial.println(xPortGetCoreID());
#endif

  for (;;) {
    WIFI::KiemTraKetNoiWiFi();
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
#pragma endregion TaskKiemTraKetNoiWiFi
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//===============================================================================//
//   Return the quality (Received Signal Strength Indicator)                     //
//   of the WiFi network.                                                        //
//   Returns a number between 0 and 100 if WiFi is connected.                    //
//   Returns -1 if WiFi is disconnected.                                         //
//===============================================================================//
int WIFI::TinhDoManhCuaWiFi(void) {
#pragma region TinhDoManhCuaWiFi
  //===============================================================================
  if (WiFi.status() != WL_CONNECTED)
    return -1;

  int dBm = WiFi.RSSI();

#if defined(debug) && defined(DEBUG_WIFI) 

  Serial.print("WiFi's RSSI: ");
  Serial.println(dBm);
#endif

  if (dBm <= -100)
    return 0;
  if (dBm >= -50)
    return 100;
  return 2 * (dBm + 100);
  //===============================================================================
#pragma endregion TinhDoManhCuaWiFi
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void WIFI::ThietLapAP(void) {
#pragma region ThietLapAP
  //===============================================================================
  if (WIFI::DaBatAP == false) {
    // Bật cờ này lên để k phải gọi lại tác vụ thiết lập AP này nhiều lần.
    WIFI::DaBatAP = true;

    // Chuyển sang mode phát Access Point của ESP32.
    // Lưu ý phải là mode WIFI_AP_STA để lúc này ESP32 vừa
    // có thể phát Access Point vừa là Station kết nối WiFi
    // WiFi.mode(WIFI_AP);
    WiFi.mode(WIFI_AP_STA);
    delay(250);

    WiFi.softAP(WIFI::AP_SSID, WIFI::AP_PASS);
    delay(250);

    WiFi.softAPConfig(ip_ap, gateway_ap, subnet_ap);
    ServerON();

#if defined(debug) && defined(DEBUG_WIFI) 
    Serial.println("Đang thiết lập Access Point...");
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("SoftAP IP: ");
    Serial.println(WiFi.softAPIP());
    Serial.print("WiFi do ESP32 phát ra: ");
    Serial.print(WIFI::AP_SSID);
    Serial.print("|Mật khẩu: ");
    Serial.println(WIFI::AP_PASS);
#endif
  }
  else {
#if defined(debug) && defined(DEBUG_WIFI) 

    Serial.println("Access Point đã được thiết lập!");
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("SoftAP IP: ");
    Serial.println(WiFi.softAPIP());
    Serial.print("WiFi do ESP32 phát ra: ");
    Serial.print(WIFI::AP_SSID);
    Serial.print("|Mật khẩu: ");
    Serial.println(WIFI::AP_PASS);
#endif
  }
  //===============================================================================
#pragma endregion ThietLapAP
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void WIFI::QuetTimWiFi(void) {
#pragma region QuetTimWiFi
  //===============================================================================
  WiFi.disconnect();
  int n = WiFi.scanNetworks();

  //---------------------------------------------------------------
  uint16_t count = 0;
  while (count < 10) {
    delay(100);
    count++;
    Serial.print(count);
    Serial.print(" > ");
  }
  //---------------------------------------------------------------

#if defined(debug) && defined(DEBUG_WIFI) 

  if (n <= 0)
    Serial.println("Không tìm thấy WiFi nào.");
  else {
    Serial.print("Đã quét thấy ");
    Serial.print(n);
    Serial.println(" mạng WiFi");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
#endif

  String str = "";
  for (int i = 0; i < n; ++i) {
    // Print SSID and RSSI for each network found
    String tenWiFi = WiFi.SSID(i);
    String ap_class = "";
    String icon = WIFI::rssiToIcon(WiFi.RSSI(i));
    str += "<div class='ape'><div class='" + icon + "'><div>" + tenWiFi + "</div></div></div>";
  }
  delay(100);
  server.send(200, "text/html", str);
  //===============================================================================
#pragma endregion QuetTimWiFi
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
String WIFI::rssiToIcon(int rssi) {
  if (rssi >= -60) {
    return "w0";
  }
  else if (rssi >= -67) {
    return "w1";
  }
  else if (rssi >= -75) {
    return "w2";
  }
  else {
    return "w3";
  }
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void WIFI::LuuWiFiVaoEEPROM(void) {
#pragma region LuuWiFiVaoEEPROM
  //===============================================================================
  // Lấy tên WiFi và mật khẩu từ phương thức GET ở hàm LuuWiFiVaoEEPROM()
  // trong file WEBConfig.h
  WIFI::TenWiFi = server.arg("TenWiFi");
  WIFI::MatKhauWiFi = server.arg("MatKhauWiFi");

  // Trước tiên cần xóa trắng thông tin WiFi đã lưu trước đó.
  for (int i = EEPROM_SSID; i < 512; ++i) {
    EEPROM.write(i, 0);
  }

  Serial.println("Lưu tên WiFi vào EEPROM...");
  for (int i = 0; i < TenWiFi.length(); ++i) {
    EEPROM.write(EEPROM_SSID + i, TenWiFi[i]);
    delay(10);
    Serial.print("Đã lưu: ");
    Serial.println(TenWiFi[i]);
  }

  Serial.println("Lưu mật khẩu WiFi vào EEPROM...");
  for (int i = 0; i < MatKhauWiFi.length(); ++i) {
    EEPROM.write(EEPROM_PASS + i, MatKhauWiFi[i]);
    delay(10);
    Serial.print("Đã lưu: ");
    Serial.println(MatKhauWiFi[i]);
  }

  // The write command does not write directly to EEPROM. It just prepares an array
  // of bytes which must then be saved in the memory area using the commit() command.
  EEPROM.commit();
  delay(500);

#if defined(debug) && defined(DEBUG_WIFI) 

  Serial.println("Đã lưu WiFi vào EEPROM!");
  Serial.print("Tên WiFi: ");
  Serial.println(TenWiFi);
  Serial.print("Mật khẩu: ");
  Serial.println(MatKhauWiFi);
#endif

  String str;
  str = "Lưu thành công WiFi: ";
  str += TenWiFi;
  str += "|Mật khẩu: ";
  str += MatKhauWiFi;
  server.send(200, "text/html", str);

  // Kết nối WiFi
  WiFi.begin(TenWiFi, MatKhauWiFi);
  //===============================================================================
#pragma endregion LuuWiFiVaoEEPROM
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void WIFI::CapNhatWiFiVaoEEPROM(String TenWiFi, String MatKhauWiFi) {
#pragma region LuuWiFiVaoEEPROM
  //===============================================================================  
  WIFI::TenWiFi = TenWiFi;
  WIFI::MatKhauWiFi = MatKhauWiFi;

  // Trước tiên cần xóa trắng thông tin WiFi đã lưu trước đó.
  for (int i = EEPROM_SSID; i < 512; ++i) {
    EEPROM.write(i, 0);
  }

  Serial.println("Lưu tên WiFi vào EEPROM...");
  for (int i = 0; i < TenWiFi.length(); ++i) {
    EEPROM.write(EEPROM_SSID + i, TenWiFi[i]);
    delay(10);
    Serial.print("Đã lưu: ");
    Serial.println(TenWiFi[i]);
  }

  Serial.println("Lưu mật khẩu WiFi vào EEPROM...");
  for (int i = 0; i < MatKhauWiFi.length(); ++i) {
    EEPROM.write(EEPROM_PASS + i, MatKhauWiFi[i]);
    delay(10);
    Serial.print("Đã lưu: ");
    Serial.println(MatKhauWiFi[i]);
  }

  // The write command does not write directly to EEPROM. It just prepares an array
  // of bytes which must then be saved in the memory area using the commit() command.
  EEPROM.commit();
  delay(500);

#if defined(debug) && defined(DEBUG_WIFI) 

  Serial.println("Đã lưu WiFi vào EEPROM!");
  Serial.print("Tên WiFi: ");
  Serial.println(TenWiFi);
  Serial.print("Mật khẩu: ");
  Serial.println(MatKhauWiFi);
#endif

  // Kết nối WiFi
  WiFi.begin(TenWiFi, MatKhauWiFi);
  //===============================================================================
#pragma endregion LuuWiFiVaoEEPROM
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void WIFI::TrangChu(void) {
  String s = FPSTR(MainpageHTML);
  server.send(200, "text/html", s);
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void WIFI::StyleCSS(void) {
  String s = FPSTR(style_css);
  server.send(200, "text/css", s);
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void WIFI::CodeJS(void) {
  String s = FPSTR(code_js);
  server.send(200, "text/js", s);
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void WIFI::ServerON(void) {
#pragma region ServerON
  server.on("/", WIFI::TrangChu);                          // Mở trang web để cấu hình WiFi cho board
  server.on("/style.css", WIFI::StyleCSS);                 // Gửi style và css qua file style.css
  server.on("/code.js", WIFI::CodeJS);                     // Gửi code js qua file code.js
  server.on("/LuuWiFiVaoEEPROM", WIFI::LuuWiFiVaoEEPROM);  // Gọi hàm khi user bấm nút LuuWiFiVaoEEPROM
  server.on("/QuetTimWiFi", WIFI::QuetTimWiFi);            // Gọi hàm khi user bấm nút QuetTimWiFi
  server.begin();                                          // Khởi động server
#pragma endregion ServerON
}


#endif //build_test_chay_khong_Wifi