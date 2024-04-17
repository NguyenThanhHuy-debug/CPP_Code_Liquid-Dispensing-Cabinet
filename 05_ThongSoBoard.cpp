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
  _i2c_eeprom.begin(extEEPROM::twiClock100kHz);
  LayThongSoBoardTuROM();

#ifdef DEBUG_THONGSOBOARD
  Serial.print("Chiều dài thông số:");
  Serial.println(this->ThongSo.keys().length());
  Serial.println("Thông số đã cài đặt cho board:");
  Serial.println(this->ThongSo);
#endif

  if (this->ThongSo.keys().length() <= 0) {
    ThietLapThongSoBoardMacDinh();
    CaiDatThongSoBoardVaoROM(this->ThongSo);
#ifdef DEBUG_THONGSOBOARD
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

#ifdef DEBUG_THONGSOBOARD
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
#ifdef DEBUG_THONGSOBOARD
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

  ChuongTrinh_t ChuongTrinhGoc;
  for (uint8_t i = 0; i < SoLuongChuongTrinhToiDa; i++){
    // ChuongTrinhGoc.tenchuongtrinh = "Program " + String(i + 1);
    ChuongTrinhGoc.tenchuongtrinh = "Program " + String(i + 1);
    this->ThongSo["ChuongTrinh" + String((i + 1))] = ChuyenChuongTrinhThanhJsonString(ChuongTrinhGoc);
  }

#ifdef DEBUG_THONGSOBOARD
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

#ifdef DEBUG_THONGSOBOARD
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
  + ChuongTrinh5 + ";" + ChuongTrinh6 + ";" + ChuongTrinh7 + ";" + ChuongTrinh8 + ";" + ChuongTrinh9 + ";" + ChuongTrinh10 + ";";

  // Chuyển dữ liệu thông số board dạng json sang string
  String strThongSo = JSON.stringify(jsThongSo);

#ifdef DEBUG_THONGSOBOARD
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

#ifdef DEBUG_THONGSOBOARD
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

// tên CT & thể tích & đường kính & tốc độ bơm & tốc độ XY & antidrop & rackx & racky & A1.X % A1.Y & B1.X % B1.Y & C1.X % C1.Y & H1 & A2.X % A2.Y & B2.X % B2.Y & C2.X % C2.Y & H2 & TheTichBomDuocKhiQuay1Vong

String ThongSoBoard::ChuyenChuongTrinhThanhJsonString(ChuongTrinh_t ChuongTrinh){
  String strChuongTrinh = "";
  char *ThongSoChuongTrinh;
  asprintf(&ThongSoChuongTrinh, "%s&%07.3f&%07.3f&%05d&%05d&%07.3f&%02d&%02d&%07.3f%%%07.3f&%07.3f%%%07.3f&%07.3f%%%07.3f&%07.3f&%07.3f%%%07.3f&%07.3f%%%07.3f&%07.3f%%%07.3f&%07.3f&%07.3f&", 
          DienVaoChoTrong(ChuongTrinh.tenchuongtrinh).c_str(), ChuongTrinh.thetich, ChuongTrinh.duongkinhong, ChuongTrinh.tocdobom, 
          ChuongTrinh.tocdoXY, ChuongTrinh.chongnhogiot, ChuongTrinh.rackX, ChuongTrinh.rackY, ChuongTrinh.ViTriA[0].X, ChuongTrinh.ViTriA[0].Y, ChuongTrinh.ViTriB[0].X, ChuongTrinh.ViTriB[0].Y, ChuongTrinh.ViTriC[0].X, ChuongTrinh.ViTriC[0].Y, ChuongTrinh.ViTriH[0],
          ChuongTrinh.ViTriA[1].X, ChuongTrinh.ViTriA[1].Y, ChuongTrinh.ViTriB[1].X, ChuongTrinh.ViTriB[1].Y, ChuongTrinh.ViTriC[1].X, ChuongTrinh.ViTriC[1].Y, ChuongTrinh.ViTriH[1],
          ChuongTrinh.TheTichBomDuocKhiQuay1Vong);
  strChuongTrinh = String(ThongSoChuongTrinh);
  free(ThongSoChuongTrinh);
  return strChuongTrinh;
}

ChuongTrinh_t ThongSoBoard::ChuyenDoiJSONVarSangChuongTrinh(JSONVar data,uint8_t idxchuongtrinh)
{
  String strdata = (const char *)data;
  #ifdef DEBUG_THONGSOBOARD
  Serial.print("Data: ");
  Serial.println(strdata);
  #endif
  String str = TachChuoi(strdata,',',idxchuongtrinh);
  ChuongTrinh_t chuongtrinh;
  chuongtrinh.tenchuongtrinh = TachChuoi(str,'&',0);
  chuongtrinh.tenchuongtrinh = TachChuoi(chuongtrinh.tenchuongtrinh,'\r',0);

  chuongtrinh.thetich = TachChuoi(str,'&',1).toFloat();
  chuongtrinh.duongkinhong =  TachChuoi(str,'&',2).toFloat();
  chuongtrinh.tocdobom = TachChuoi(str,'&',3).toInt();
  chuongtrinh.tocdoXY = TachChuoi(str,'&',4).toInt();
  chuongtrinh.chongnhogiot = TachChuoi(str,'&',5).toInt();
  chuongtrinh.rackX = TachChuoi(str,'&',6).toInt();
  chuongtrinh.rackY = TachChuoi(str,'&',7).toInt();

  String strViTri = TachChuoi(str,'&',8);
  chuongtrinh.ViTriA[0].X = TachChuoi(strViTri,'%',0).toFloat();
  chuongtrinh.ViTriA[0].Y = TachChuoi(strViTri,'%',1).toFloat();


  strViTri = TachChuoi(str,'&',9);
  chuongtrinh.ViTriB[0].X =  TachChuoi(strViTri,'%',0).toFloat();
  chuongtrinh.ViTriB[0].Y = TachChuoi(strViTri,'%',1).toFloat();


  strViTri = TachChuoi(str,'&',10);
  chuongtrinh.ViTriC[0].X =  TachChuoi(strViTri,'%',0).toFloat();
  chuongtrinh.ViTriC[0].Y = TachChuoi(strViTri,'%',1).toFloat();

  chuongtrinh.ViTriH[0] = TachChuoi(str,'&',11).toFloat();

  strViTri = TachChuoi(str,'&',12);
  chuongtrinh.ViTriA[1].X = TachChuoi(strViTri,'%',0).toFloat();
  chuongtrinh.ViTriA[1].Y = TachChuoi(strViTri,'%',1).toFloat();

  strViTri = TachChuoi(str,'&',13);
  chuongtrinh.ViTriB[1].X =  TachChuoi(strViTri,'%',0).toFloat();
  chuongtrinh.ViTriB[1].Y = TachChuoi(strViTri,'%',1).toFloat();

  strViTri = TachChuoi(str,'&',14);
  chuongtrinh.ViTriC[1].X =  TachChuoi(strViTri,'%',0).toFloat();
  chuongtrinh.ViTriC[1].Y = TachChuoi(strViTri,'%',1).toFloat();

  chuongtrinh.ViTriH[1] = TachChuoi(str,'&',15).toFloat();

  chuongtrinh.TheTichBomDuocKhiQuay1Vong = TachChuoi(str,'&',16).toFloat();

#ifdef DEBUG_THONGSOBOARD
  Serial.println(chuongtrinh.tenchuongtrinh);
  Serial.println(chuongtrinh.thetich);
  Serial.println(chuongtrinh.duongkinhong);
  Serial.println(chuongtrinh.tocdobom);
  Serial.println(chuongtrinh.tocdoXY);
  Serial.println(chuongtrinh.chongnhogiot);
  Serial.println(chuongtrinh.rackX);
  Serial.println(chuongtrinh.rackY);
  Serial.println(chuongtrinh.ViTriA[0].X);
  Serial.println(chuongtrinh.ViTriA[0].Y);
  Serial.println(chuongtrinh.ViTriB[0].X);
  Serial.println(chuongtrinh.ViTriB[0].Y);
  Serial.println(chuongtrinh.ViTriC[0].X);
  Serial.println(chuongtrinh.ViTriC[0].Y);
  Serial.println(chuongtrinh.ViTriH[0]);
  Serial.println(chuongtrinh.ViTriA[1].X);
  Serial.println(chuongtrinh.ViTriA[1].Y);
  Serial.println(chuongtrinh.ViTriB[1].X);
  Serial.println(chuongtrinh.ViTriB[1].Y);
  Serial.println(chuongtrinh.ViTriC[1].X);
  Serial.println(chuongtrinh.ViTriC[1].Y);
  Serial.println(chuongtrinh.ViTriH[1]);
  Serial.println(chuongtrinh.TheTichBomDuocKhiQuay1Vong);
#endif
  return chuongtrinh;
}

JSONVar ThongSoBoard::ChuyenDoiChuongTrinhSangJsonVar(vector<ChuongTrinh_t> DsChuongTrinhCanChuyenDoi){
  JSONVar DsChuongTrinhJsonVar;
  DsChuongTrinhJsonVar["ID"] = ID;
  ChuongTrinh_t ChuongTrinh;
  for (uint8_t i = 0; i < SoLuongChuongTrinhToiDa; i++){
    ChuongTrinh = DsChuongTrinhCanChuyenDoi[i];
    DsChuongTrinhJsonVar["ChuongTrinh" + String(i + 1)] = ChuyenChuongTrinhThanhJsonString(ChuongTrinh);
  }
  return DsChuongTrinhJsonVar;
}

String ThongSoBoard::DienVaoChoTrong(String Chuoi, size_t DoDaiChuoi, char KyTuCanDien){
  String strChuongTrinh = Chuoi;
  for (size_t i = Chuoi.length(); i < DoDaiChuoi; i++){
    strChuongTrinh += KyTuCanDien;
  }
  return strChuongTrinh;
}


