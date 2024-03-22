#include "05_ThongSoBoard.h"
#include <EEPROM.h>      // Thư viện để lưu dữ liệu thông số board vào bộ nhớ ROM
#include <WiFi.h>        // Thư viện WiFi của ESP32.
#include <HTTPClient.h>  // Cho phép khai báo kiểu HTTPClient
#include <extEEPROM.h>
#define URL_POSTGETThongSoBoard "https://App.IoTVision.vn/api/LABone_KingTubeAuto_ThongSoCaiDatBoard"

extEEPROM _i2c_eeprom(kbits_512, 1, 128, 0x50);

void ThongSoBoard::KhoiTao(void) 
{

#pragma region KhoiTao
  // EEPROM.begin(_ChieuDaiMaxThongSoBoard);
  _i2c_eeprom.begin(extEEPROM::twiClock100kHz);
  LayThongSoBoardTuROM();

#ifdef debug
  Serial.print("Chiều dài thông số:");
  Serial.println(this->ThongSo.keys().length());
  Serial.println("Thông số đã cài đặt cho board:");
  Serial.println(this->ThongSo);
#endif

  if (this->ThongSo.keys().length() <= 0) {
    ThietLapThongSoBoardMacDinh();
    CaiDatThongSoBoardVaoROM(this->ThongSo);
#ifdef debug
    Serial.println("Board trắng chưa có thông số.");
#endif
  }
#pragma endregion KhoiTao
}
    
// Lấy bộ thông số cài đặt của board từ trong EFROM. Thông số này
// được lấy ra để sử dụng trong quá trình thực thi của board.
void ThongSoBoard::LayThongSoBoardTuROM(void) 
{
#pragma region LayThongSoBoardTuROM

  String strThongSo;
  for (int i = 0; i < _ChieuDaiI2CEEPROM; ++i) 
  {
    // uint8_t e = EEPROM.read(i);
    uint8_t e = _i2c_eeprom.read(i);
    // Đọc đến khi gặp ký tự khoảng trắng thì dừng không cần
    // phải đọc tiếp vì sau đó không phải là dữ liệu thông số board
    if (e == 0)
      break;
    else
      strThongSo += char(e);
  }

#ifdef debug
  Serial.println("Chuỗi string trong EEPROM chứa thông số board: ");
  Serial.println(strThongSo);
#endif

  // Chuẩn hóa string chứa thông số board thành dạng json.
  JSONVar json = JSON.parse(strThongSo);
  if (json.keys().length() > 0) 
  {
    this->ThongSo["ID"] = this->ID;

    // Các thông số board sẽ lưu trong chuỗi ThongSo, cách nhau bằng các dấu ";"
    String para = (const char *)json["ThongSo"];
    
    this->ThongSo["ChuongTrinh1"] = TachChuoi(para, ';', 0);
    this->ThongSo["ChuongTrinh2"] = TachChuoi(para, ';', 1);
    this->ThongSo["ChuongTrinh3"] = TachChuoi(para, ';', 2);
    this->ThongSo["ChuongTrinh4"] = TachChuoi(para, ';', 3);
    this->ThongSo["ChuongTrinh5"] = TachChuoi(para, ';', 4);
    this->ThongSo["ChuongTrinh6"] = TachChuoi(para, ';', 5);
    this->ThongSo["ChuongTrinh7"] = TachChuoi(para, ';', 6);
    this->ThongSo["ChuongTrinh8"] = TachChuoi(para, ';', 7);
    this->ThongSo["ChuongTrinh9"] = TachChuoi(para, ';', 8);
    this->ThongSo["ChuongTrinh10"] = TachChuoi(para, ';',9);

  }
#ifdef debug
  Serial.println("Thông số board đọc từ EEPROM");
  Serial.println(this->ThongSo);
#endif

#pragma endregion LayThongSoBoardTuROM
}
/*
Thông số chương trình:"Program 1&5.0&3&100&1&90&12&8&21.0;-21.0&21.0;-181.0&274.0;-181.0&-380.0&-1&{0,0}"
- Tên chương trình: Program 1
- Thể tích: 5.0 ml;
- Đường kính ống: 3mm;
- Tốc độ bơm: 100 vòng/phút
- Tốc độ XY = 1 (TocDoTrungBinh)
- Chống nhỏ giọt = 90 Độ
- RackX = 12
- RackY = 8
- Vị trí A = {21.0,-21.0}
- Vị trí B = { 21.0, -181.0}
- Vị trí C = { 274.0, -181.0}
- Độ cao H = -380.0
- Danh sách tọa độ đã thiết lập trước đó: {0,0};{0,1};{0,2};... (thay đổi)

- Số vòng để bơm đủ 1ml
- Khối lượng riêng
*/
void ThongSoBoard::ThietLapThongSoBoardMacDinh(void) {
  this->ThongSo["ID"] = this->ID;

  chuongtrinh_t ChuongTrinhGoc;
  for (uint8_t i = 0; i < _SoChuongTrinh; i++){
    // ChuongTrinhGoc.tenchuongtrinh = "Program " + String(i + 1);
    ChuongTrinhGoc.tenchuongtrinh = "Program " + String(i + 1);
    this->ThongSo["ChuongTrinh" + String((i + 1))] = ChuyenChuongTrinhThanhJsonString(ChuongTrinhGoc);
  }
  // this->ThongSo["ChuongTrinh1"] =   "Program 1           &05.0&03.0&1000&1000&90&12&8&21.0%-21.0&21.0%-181.0&274.0%-181.0&-380.0&1&"; //73
  // this->ThongSo["ChuongTrinh2"] =   "Program 2           &05.0&03&1000&1000&90&12&8&21.0%-21.0&21.0%-181.0&274.0%-181.0&-380.0&1&";
  // this->ThongSo["ChuongTrinh3"] =   "Program 3           &05.0&03&1000&1000&90&12&8&21.0%-21.0&21.0%-181.0&274.0%-181.0&-380.0&1&";
  // this->ThongSo["ChuongTrinh4"] =   "Program 4           &05.0&03&1000&1000&90&12&8&21.0%-21.0&21.0%-181.0&274.0%-181.0&-380.0&1&";
  // this->ThongSo["ChuongTrinh5"] =   "Program 5           &05.0&03&1000&1000&90&12&8&21.0%-21.0&21.0%-181.0&274.0%-181.0&-380.0&1&";
  // this->ThongSo["ChuongTrinh6"] =   "Program 6           &05.0&03&1000&1000&90&12&8&21.0%-21.0&21.0%-181.0&274.0%-181.0&-380.0&1&";
  // this->ThongSo["ChuongTrinh7"] =   "Program 7           &05.0&03&1000&1000&90&12&8&21.0%-21.0&21.0%-181.0&274.0%-181.0&-380.0&1&";
  // this->ThongSo["ChuongTrinh8"] =   "Program 8           &05.0&03&1000&1000&90&12&8&21.0%-21.0&21.0%-181.0&274.0%-181.0&-380.0&1&";
  // this->ThongSo["ChuongTrinh9"] =   "Program 9           &05.0&03&1000&1000&90&12&8&21.0%-21.0&21.0%-181.0&274.0%-181.0&-380.0&1&";
  // this->ThongSo["ChuongTrinh10"] =  "Program 10          &05.0&03&1000&1000&90&12&8&21.0%-21.0&21.0%-181.0&274.0%-181.0&-380.0&1&";

#ifdef debug
  Serial.println("Thông số board mặc định: ");
  Serial.println(this->ThongSo);
#endif
}

// Tách chuỗi
//-----------------------------------------------------------------------------------------------------
String ThongSoBoard::TachChuoi(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
//-----------------------------------------------------------------------------------------------------

// Thông số board sẽ được load từ database trên server và lưu vào bộ nhớ EFROM của ESP32.
// Việc cài đặt này sẽ được thực hiện thông qua App.
//-----------------------------------------------------------------------------------------------------
void ThongSoBoard::CaiDatThongSoBoardVaoROM(JSONVar ThongSo) {
#pragma region CaiDatThongSoBoardVaoROM

#ifdef debug
  Serial.println("Thông số board sẽ cài vào EPPROM: ");
  Serial.println(ThongSo);
#endif

  String ChuongTrinh1 = (const char *)ThongSo["ChuongTrinh1"];
  String ChuongTrinh2 = (const char *)ThongSo["ChuongTrinh2"];
  String ChuongTrinh3 = (const char *)ThongSo["ChuongTrinh3"];
  String ChuongTrinh4 = (const char *)ThongSo["ChuongTrinh4"];
  String ChuongTrinh5 = (const char *)ThongSo["ChuongTrinh5"];
  String ChuongTrinh6 = (const char *)ThongSo["ChuongTrinh6"];
  String ChuongTrinh7 = (const char *)ThongSo["ChuongTrinh7"];
  String ChuongTrinh8 = (const char *)ThongSo["ChuongTrinh8"];
  String ChuongTrinh9 = (const char *)ThongSo["ChuongTrinh9"];
  String ChuongTrinh10 = (const char *)ThongSo["ChuongTrinh10"];

  JSONVar jsThongSo;
  jsThongSo["ID"] = (const char *)ThongSo["ID"];
  jsThongSo["ThongSo"] = ChuongTrinh1 + ";"  + ChuongTrinh2 + ";"  + ChuongTrinh3 + ";" + ChuongTrinh4 + ';'
  + ChuongTrinh5 + ";" + ChuongTrinh6 + ";" + ChuongTrinh7 + ";" + ChuongTrinh8 + ";" + ChuongTrinh9 + ";" + ChuongTrinh10;

  // Chuyển dữ liệu thông số board dạng json sang string
  String strThongSo = JSON.stringify(jsThongSo);

#ifdef debug
  Serial.println("Chuỗi string thông số board sẽ lưu vào ROM: ");
  Serial.println(strThongSo);
  Serial.print("Độ dài: ");
  Serial.println(strThongSo.length());

  Serial.println("-------------------------------------------------------------");
  Serial.println("Đang cài đặt thông số board vào ROM, chờ...");
#endif

  for (int i = 0; i < strThongSo.length(); ++i) {
    // EEPROM.write(i, strThongSo[i]);
    _i2c_eeprom.update(i, strThongSo[i]);
  }
  // delay(5000);

#ifdef debug
  Serial.println("Đã lưu thông số board vào ROM thành công!");
  Serial.println("-------------------------------------------------------------");
#endif

  // The write command does not write directly to EEPROM. It just prepares an array
  // of bytes which must then be saved in the memory area using the commit() command.
  // EEPROM.commit();
  // delay(500);
#pragma endregion CaiDatThongSoBoardVaoROM
}
//-----------------------------------------------------------------------------------------------------

// Xóa thông tin lưu trong bộ nhớ EFROM của ESP32 từ start đến end.
void ThongSoBoard::XoaEEPROM(int start, int end) {
  for (int i = start; i < end; ++i) {
    // EEPROM.write(i, 0);
    _i2c_eeprom.write(i, 0);
    // delay(10);
  }

  // The write command does not write directly to EEPROM. It just prepares an array
  // of bytes which must then be saved in the memory area using the commit() command.
  // EEPROM.commit();
  // delay(500);
}

String ThongSoBoard::ChuyenChuongTrinhThanhJsonString(chuongtrinh_t ChuongTrinh){
  String strChuongTrinh = "";
  char *ThongSoChuongTrinh;
  asprintf(&ThongSoChuongTrinh, "%s&%07.3f&%07.3f&%05d&%05d&%07.3f&%02d&%02d&%07.3f%%%07.3f&%07.3f%%%07.3f&%07.3f%%%07.3f&%07.3f&%07.3f&", 
          DienVaoChoTrong(ChuongTrinh.tenchuongtrinh).c_str(), ChuongTrinh.thetich, ChuongTrinh.duongkinhong, ChuongTrinh.tocdobom, 
          ChuongTrinh.tocdoXY, ChuongTrinh.chongnhogiot, ChuongTrinh.rackX, ChuongTrinh.rackY, ChuongTrinh.vitriA.X, 
          ChuongTrinh.vitriA.Y, ChuongTrinh.vitriB.X, ChuongTrinh.vitriB.Y, ChuongTrinh.vitriC.X, ChuongTrinh.vitriC.Y, ChuongTrinh.docaoH, 
          /* ChuongTrinh.SoVongDeBom1ml */ ChuongTrinh.TheTichBomDuocKhiQuay1Vong);
  strChuongTrinh = String(ThongSoChuongTrinh);
  free(ThongSoChuongTrinh);
  return strChuongTrinh;
}

String ThongSoBoard::DienVaoChoTrong(String Chuoi, size_t DoDaiChuoi, char KyTuCanDien){
  String strChuongTrinh = Chuoi;
  for (size_t i = Chuoi.length(); i < DoDaiChuoi; i++){
    strChuongTrinh += KyTuCanDien;
  }
  return strChuongTrinh;
}


