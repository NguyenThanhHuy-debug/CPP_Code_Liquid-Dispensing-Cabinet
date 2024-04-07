#include "00_ChuongTrinhChinh.h"

DanhSachMaLenh_t _MaLenhGRBL;
// #define debug
#define TocDoLOW 500.0f
#define TocDoMEDIUM 2000.0f
#define TocDoFAST 5000.0f

#define SoBuocLOW 1.0f
#define SoBuocMEDIUM 10.0f
#define SoBuocFAST 30.0f

#define GioiHanTrucX 530U
#define GioiHanTrucY 200U
#define GioiHanTrucZ 420U

bool DangTamDungPhanPhoi = false;
bool DangFastPump = false;
float TocDoTamThoi = TocDoLOW;
float SoBuocTamThoi = SoBuocLOW;

Flags _Flags;
#ifndef build_test_chay_khong_Wifi
WIFI _WiFi;                  // Kết nối WiFi cho board.
#endif                       //build_test_chay_khong_Wifi
Firmware _Firmware;          // Phiên bản firmware hiện tại của board.
ThongSoBoard _ThongSoBoard;  // Thông số cài đặt cho board lưu trong EEPROM.
RTC _RTC;                    // Mô-đun thời gian thực DS3231.
DWINHMI _DWIN;               // Các thao tác trên màn hình HMI DWIN

String _ID;                  // Số ID của ESP32, đây là số IMEI của board.
int _CODE;                   // Mã CODE gửi từ app xuống board để thực thi các tác vụ.
int _MODE;                   // Chế độ làm việc của máy.
//--------------------------------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------------------------------//

void vTaskDWINCode(void* pvParameters)
{
  while (1)
  {
    _DWIN.LangNgheDWIN();
    _DWIN.ChuyenTrangTheoBamChonCuaUserTrenHMI();
    _DWIN.XuLySetHome();
    _DWIN.XuLyChonChuongTrinhSeChay();
    _DWIN.XuLyThietLapChuongTrinhCheDoMulti();
    _DWIN.XuLyChonViTriPhanPhoi();
    _DWIN.XuLyConfigChuongTrinh();
    _DWIN.XuLySetupProgram();
    _DWIN.HienThiThongSoHienThiThoiGianThuc();
    _DWIN.XuLyBanPhim();   
    _DWIN.ThayDoiTenChuongTrinh();
    _DWIN.XuLyCalib();
    _DWIN.XuLyThoiGianThuc();
    _DWIN.XuLyXemConfig();
    _DWIN.ResetProgram();
    XuLyDuLieuNhanTuHMI();
    CapNhatDuLieuLenHMI();
    _DWIN.ResetCode();
    delay(1);
  }
}

void KhoiTao(void){
  #pragma region KhoiTao
  //MMMMMMMMM7MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM//
  _DWIN.restartHMI();
  
  CaiDatNguon();
  // delay(1000);
  //Phải khởi tạo GRBL trước để begin Serial, do GRBL đã viết lại Serial
  _grbl.KhoiTaoGRBL();
  //---------------------------------------------------------------

  #pragma region Các khởi tạo cơ bản của máy
    #ifndef build_test_chay_khong_Wifi
    _ID = _WiFi.LaySoMAC();
    Serial.print("ID: ");
    Serial.println(_ID);
    #endif

    _Firmware.SETPhienBanFirmware("KingTubeAuto_2024.01.10_v01");
    Serial.print("Phiên bản firmware: ");
    Serial.println(_Firmware.PhienBan);

    _ThongSoBoard.ID = _ID;
    _ThongSoBoard.KhoiTao();
    CapNhatDanhSachChuongTrinhLuuTrongEEPROM();

    #ifdef CoKetNoiServer
#pragma region Khởi tạo WIFI
#ifndef BAT_TAT_WIFI
  //======================================================================
  //------ Begin: Khởi tạo để có thể cấu hình kết nối WiFi tự động -----//
  //======================================================================
  // Nếu muốn xóa thông tin WIFI đã lưu trong EEPROM thì mở dòng code này.
  // _WiFi.XoaWIFIDaLuuTrongEEPROM();
  _WiFi.CapNhatWiFiVaoEEPROM("IoTVision_2.4GHz", "iotvision@2022");
  // _WiFi.CapNhatWiFiVaoEEPROM("", "");
  // delay(500);
  // Đoạn code này phải được gọi ở cuối cùng ở hàm setup().
  _WiFi.DocWiFiDaLuuTrongEEPROM();

  // Dành 10s để kết nối WiFI
  // Lưu ý: Phải có thời gian chờ cho việc kết nối WIFI nếu không sẽ
  // gây ra tình trạng board bị reset và không thể phát access point (AP).
  _WiFi.KetNoiWiFi(10);

  // delay(1000);

  //xTaskCreate(TaskWIFI, "Task WIFI", 2048, NULL, 1, NULL);
  //======================================================================
  //------ End: Khởi tạo để có thể cấu hình kết nối WiFi tự động -------//
  //======================================================================
#endif
#pragma endregion Khởi tạo WIFI
#endif
#pragma endregion Các khởi tạo cơ bản của máy

  // Khởi tạo HMI và chạy giao diện khởi động.
  _DWIN.KhoiTaoDWIN();
  DWINCapNhatDSChuongTrinhBanDau();
  xTaskCreate(vTaskDWINCode, "Task DWIN", 4096, NULL, 2, NULL);

  CapNhatDuLieuLenHMI();

/////////////////////////////////////////////////////////////////////////
#ifdef RESET_EEPROM
    _ThongSoBoard.XoaEEPROM(0,_ChieuDaiI2CEEPROM);
#endif
}

void CapNhatDuLieuLenHMI(void) 
{
  _DWIN.DuLieuBoardGuiLenHMI.ViTriHienTaiX = _grbl.ViTriHienTaiCuaSTEP(TrucX);
  _DWIN.DuLieuBoardGuiLenHMI.ViTriHienTaiY = _grbl.ViTriHienTaiCuaSTEP(TrucY);
  _DWIN.DuLieuBoardGuiLenHMI.ViTriHienTaiZ = _grbl.ViTriHienTaiCuaSTEP(TrucZ);
  _DWIN.DuLieuBoardGuiLenHMI.DSChuongTrinh = _grbl._DSChuongTrinh;
  _DWIN.DuLieuBoardGuiLenHMI.RTC_ChuoiNgay = _RTC.ChuanHoaChuoiRTC_Ngay();
  _DWIN.DuLieuBoardGuiLenHMI.RTC_ChuoiGio  = _RTC.ChuanHoaChuoiRTC_Gio();
  _DWIN.DuLieuBoardGuiLenHMI.OngDaPhanPhoi = _grbl.OngDaPhanPhoi;
  _DWIN.DuLieuBoardGuiLenHMI.KhuVucDangChay = _grbl.KhuVucDangChay;
  _DWIN.DuLieuBoardGuiLenHMI.ToaDoXDaChay = _grbl.ToaDoXDaChay;
  _DWIN.DuLieuBoardGuiLenHMI.ToaDoYDaChay = _grbl.ToaDoYDaChay;
  _DWIN.DuLieuBoardGuiLenHMI.DaChayXongCacKhuVuc = _grbl.DaChayXongCacKhuVuc;

}

void XuLyDuLieuNhanTuHMI(void) 
{
  if(_DWIN.DuLieuHMIGuiXuongBoard.CapNhatThoiGianRTC == true)
  { 
    uint8_t ngay  =   _DWIN.DuLieuHMIGuiXuongBoard.RTC_Ngay.toInt();
    uint8_t thang =   _DWIN.DuLieuHMIGuiXuongBoard.RTC_Thang.toInt();
    String nam_str =  _DWIN.DuLieuHMIGuiXuongBoard.RTC_Nam;
    uint8_t nam   =   nam_str.substring(nam_str.length()-2).toInt(); // Lấy 2 giá trị cuối
    uint8_t gio   =   _DWIN.DuLieuHMIGuiXuongBoard.RTC_Gio.toInt();
    uint8_t phut  =   _DWIN.DuLieuHMIGuiXuongBoard.RTC_Phut.toInt();
    
    _RTC.CapNhatRTCChoDS3231(0,ngay,thang,nam,gio,phut,0);
    _DWIN.DuLieuBoardGuiLenHMI.RTC_ChuoiNgay = _RTC.ChuanHoaChuoiRTC_Ngay();
    _DWIN.DuLieuBoardGuiLenHMI.RTC_ChuoiGio  = _RTC.ChuanHoaChuoiRTC_Gio();
    _DWIN.DuLieuHMIGuiXuongBoard.CapNhatThoiGianRTC = false;
  }
  _MaLenhGRBL = _DWIN.ReturnMaLenhGRBL();

  switch (_MaLenhGRBL)
  {
    case GOHOMEKHONGCHUYENTRANG:
      if (DangTamDungPhanPhoi == true)
      {
        Serial.println("Set Home khi dang phan phoi");
        _grbl.CoBaoPhanPhoiLaiOng = true;
        _grbl.TamDungPhanPhoi();
        WebUI::inputBuffer.write(0x18);
        delay(100);
        WebUI::inputBuffer.push("$H\n");
        delay(100);
        break;
      }
      _grbl.VeToaDoGoc();
      break;
    case GOHOME:
      #ifdef DEBUG_CHUONGTRINHCHINH
      Serial.println("GoHome");
      #endif
      _DWIN.DuLieuBoardGuiLenHMI.TrangThaiXacNhanSetHome = _grbl.VeViTriHome();
      // _DWIN.DuLieuBoardGuiLenHMI.TrangThaiXacNhanSetHome = true;
      break;
    case RUN:
      _grbl._DSChuongTrinh =_DWIN._ChuongTrinhEEPROM;
      if (_grbl.DaChayXongCacKhuVuc == true)
      {
        _grbl.OngDaPhanPhoi = -1;
        _DWIN.DuLieuBoardGuiLenHMI.OngDaPhanPhoi = -1;
      }
      if (DangTamDungPhanPhoi == true) 
      {
        #ifdef DEBUG_CHUONGTRINHCHINH
        Serial.println("RUN TiepTucPhanPhoi");
        #endif
        _grbl.TiepTucPhanPhoi();
        DangTamDungPhanPhoi = false;
      }
      else 
      {
        #ifdef DEBUG_CHUONGTRINHCHINH
        Serial.println("RUN PhanPhoi");
        #endif
        memcpy(_grbl.KhuVucChay, _DWIN.DSIndexChuongTrinhDuocChon, SoLuongKhuVucToiDa);
        _grbl.PhanPhoi();
      }
      break;
    case RESUME:
      #ifdef DEBUG_CHUONGTRINHCHINH
      Serial.println("Tạm dừng");
      #endif
      _grbl.TamDungPhanPhoi();
      DangTamDungPhanPhoi = true;
      break;
    case RESETPROGRAM:
      if (DangTamDungPhanPhoi == true)
      {
        #ifdef DEBUG_CHUONGTRINHCHINH
        Serial.println("RESET DungPhanPhoi");
        #endif
        DangTamDungPhanPhoi = false;
        _grbl.DungPhanPhoi();
      }
      _grbl.OngDaPhanPhoi = -1;
      _DWIN.DuLieuBoardGuiLenHMI.OngDaPhanPhoi = -1;
      _grbl.VeToaDoGoc();
      break;
    case EXITVETRANGCAIDATCHEDOCHUONGTRINH:
      Serial.println("EXITVETRANGCAIDATCHEDOCHUONGTRINH");
      _grbl.OngDaPhanPhoi = -1;
      _DWIN.DuLieuBoardGuiLenHMI.OngDaPhanPhoi = -1;
      if (_grbl.CoBaoPhanPhoiLaiOng == true)
      {
        Serial.println("EXITVETRANGCAIDATCHEDOCHUONGTRINH CoBaoPhanPhoiLaiOng");
        DangTamDungPhanPhoi = false;
        _grbl.DungPhanPhoi();
        break;
      }
      if (DangTamDungPhanPhoi == true)
      {
        Serial.println("DungPhanPhoi");
        DangTamDungPhanPhoi = false;
        _grbl.DungPhanPhoi();
      }
      // _grbl.VeViTriHome();
      _grbl.VeToaDoGoc();
      break;
    case CAPNHATTHONGSO:            // khi nhấn enter lúc nhập các thông số trong SETUP PROGRAM
      Serial.println("CapNhatThongSo");
      _grbl._DSChuongTrinh = _DWIN._ChuongTrinhEEPROM;
      /* Lưu vào Eeprom */
      _ThongSoBoard.CaiDatThongSoBoardVaoROM(_ThongSoBoard.ChuyenDoiChuongTrinhSangJsonVar(_grbl._DSChuongTrinh));
      break;
    case BAMNUTDICHUYENXTRU:
      // if (_DWIN.DuLieuBoardGuiLenHMI.V iTriHienTaiX - SoBuocTamThoi < DEFAULT_HOMING_PULLOFF)
      //   break;
      _grbl.DiChuyenX(Am,SoBuocTamThoi,TocDoTamThoi);
      #ifdef DEBUG_CHUONGTRINHCHINH
      Serial.printf("X-, %g buoc, %g tocdo\n",SoBuocTamThoi,TocDoFAST);
      #endif
      break;
    case BAMNUTDICHUYENYCONG:
      // if (_DWIN.DuLieuBoardGuiLenHMI.ViTriHienTaiY + SoBuocTamThoi > -DEFAULT_HOMING_PULLOFF)
      //   break;
      _grbl.DiChuyenY(Duong,SoBuocTamThoi,TocDoTamThoi);
      #ifdef DEBUG_CHUONGTRINHCHINH
      Serial.printf("Y+, %g buoc, %g tocdo\n",SoBuocTamThoi,TocDoFAST);
      #endif
      break;
    case BAMNUTDICHUYENXCONG:
      // if (_DWIN.DuLieuBoardGuiLenHMI.ViTriHienTaiX + SoBuocTamThoi > GioiHanTrucX)
      //   break;
      _grbl.DiChuyenX(Duong, SoBuocTamThoi, TocDoTamThoi);
      #ifdef DEBUG_CHUONGTRINHCHINH
      Serial.printf("X+, %g buoc, %g tocdo\n",SoBuocTamThoi,TocDoFAST);
      #endif

      break;
    case BAMNUTDICHUYENYTRU:
      // if (_DWIN.DuLieuBoardGuiLenHMI.ViTriHienTaiY - SoBuocTamThoi < -GioiHanTrucY)
      //   break;
      _grbl.DiChuyenY(Am, SoBuocTamThoi, TocDoTamThoi);
      #ifdef DEBUG_CHUONGTRINHCHINH
      Serial.printf("Y-, %g buoc, %g tocdo\n",SoBuocTamThoi,TocDoFAST);
      #endif
      break;
    case BAMNUTDICHUYENZCONG:
      // if (_DWIN.DuLieuBoardGuiLenHMI.ViTriHienTaiZ + SoBuocTamThoi > DEFAULT_HOMING_PULLOFF)
      //   break;
      _grbl.DiChuyenZ(Duong, SoBuocTamThoi, TocDoTamThoi);
      #ifdef DEBUG_CHUONGTRINHCHINH
      Serial.printf("Y-, %g buoc, %g tocdo\n",SoBuocTamThoi,TocDoFAST);
      #endif
      break;
    case BAMNUTDICHUYENZTRU:
      // if (_DWIN.DuLieuBoardGuiLenHMI.ViTriHienTaiZ - SoBuocTamThoi < -GioiHanTrucZ)
      //   break;
      _grbl.DiChuyenZ(Am,SoBuocTamThoi,TocDoTamThoi);
      #ifdef DEBUG_CHUONGTRINHCHINH
      Serial.printf("Z-, %g buoc, %g tocdo\n",SoBuocTamThoi,TocDoFAST);
      #endif
      break;

    case HOLDXCONG:
      _grbl.DiChuyenX(Duong, 1000.0f, TocDoTamThoi);
      #ifdef DEBUG_CHUONGTRINHCHINH
      Serial.printf("Hold X+, %g buoc, %g tocdo\n",SoBuocTamThoi,TocDoTamThoi);
      #endif
      break;

    case HOLDXTRU:
      _grbl.DiChuyenX(Am, 1000.0f, TocDoTamThoi);
      #ifdef DEBUG_CHUONGTRINHCHINH
      Serial.printf("Hold X-, %g buoc, %g tocdo\n",SoBuocTamThoi,TocDoTamThoi);
      #endif
      break;

    case HOLDYCONG:
      _grbl.DiChuyenY(Duong, 1000.0f, TocDoTamThoi);
      #ifdef DEBUG_CHUONGTRINHCHINH
      Serial.printf("Hold Y+, %g buoc, %g tocdo\n",SoBuocTamThoi,TocDoTamThoi);
      #endif
      break;

    case HOLDYTRU:
      _grbl.DiChuyenY(Am, 1000.0f, TocDoTamThoi);
      #ifdef DEBUG_CHUONGTRINHCHINH
      Serial.printf("Hold Y-, %g buoc, %g tocdo\n",SoBuocTamThoi,TocDoTamThoi);
      #endif
      break;

    case HOLDZCONG:
      _grbl.DiChuyenZ(Duong, 1000.0f, TocDoTamThoi);
      #ifdef DEBUG_CHUONGTRINHCHINH
      Serial.printf("Hold Z+, %g buoc, %g tocdo\n",SoBuocTamThoi,TocDoTamThoi);
      #endif
      break;
    
    case HOLDZTRU:
      _grbl.DiChuyenZ(Am, 1000.0f, TocDoTamThoi);
      #ifdef DEBUG_CHUONGTRINHCHINH
      Serial.printf("Hold Z-, %g buoc, %g tocdo\n",SoBuocTamThoi,TocDoTamThoi);
      #endif
      break;

    case DUNGDICHUYEN:
      _grbl.TamDungChay();
      #ifdef DEBUG_CHUONGTRINHCHINH
      Serial.println("Dừng di chuyển");
      #endif
      break;

    case CAPNHATTOADO:
      _grbl._DSChuongTrinh =_DWIN._ChuongTrinhEEPROM;
      break;
    case XACNHAN_A_B_C_H:                               // khi nhấn GET để lưu giá trị A, B, C, H
      _grbl._DSChuongTrinh =_DWIN._ChuongTrinhEEPROM;
      _ThongSoBoard.CaiDatThongSoBoardVaoROM(_ThongSoBoard.ChuyenDoiChuongTrinhSangJsonVar(_grbl._DSChuongTrinh));
      break;
    case CALIB:                                         // khi nhấn CALIB để thay đổi số vòng bơm
    {
      _grbl.Calib(&_DWIN._ChuongTrinhEEPROM[_DWIN.IndexChuongTrinhDangXuLy]);
      _grbl._DSChuongTrinh = _DWIN._ChuongTrinhEEPROM;
      _ThongSoBoard.CaiDatThongSoBoardVaoROM(_ThongSoBoard.ChuyenDoiChuongTrinhSangJsonVar(_grbl._DSChuongTrinh));
      _DWIN.DuLieuBoardGuiLenHMI.TrangThaiXacNhanSaveCalib = true;
#ifdef DEBUG_CHUONGTRINHCHINH
      Serial.printf("actualweight: %g, density: %g, duongkinhong: %g, vitriA: %g\n", _DWIN._ChuongTrinhEEPROM[_DWIN.IndexChuongTrinhDangXuLy].actualweight, _DWIN._ChuongTrinhEEPROM[_DWIN.IndexChuongTrinhDangXuLy].density, _DWIN._ChuongTrinhEEPROM[_DWIN.IndexChuongTrinhDangXuLy].duongkinhong, _grbl.ViTriHienTaiCuaSTEP(MayBom));      
      Serial.printf("TheTichBomDuocKhiQuay1Vong: %g\n", _DWIN._ChuongTrinhEEPROM[_DWIN.IndexChuongTrinhDangXuLy].TheTichBomDuocKhiQuay1Vong);
#endif
      break;
    }
    case TESTCALIB:
    case TESTFILL:
    {
      if (_DWIN.countTest == 2){
        sys_position[A_AXIS] = 0;
#ifdef DEBUG_CHUONGTRINHCHINH
        Serial.println("Zero");
#endif
      }
      _grbl.TestFill(_DWIN._ChuongTrinhEEPROM[_DWIN.IndexChuongTrinhDangXuLy]);

      break;
    }
    case DIRECTION:
    {
      ChuongTrinh_t* ChuongTrinhDangXuLy = &_DWIN._ChuongTrinhEEPROM[_DWIN.IndexChuongTrinhDangXuLy];
      // ChuongTrinhDangXuLy->SoVongDeBom1ml *= -1.0f;
      ChuongTrinhDangXuLy->TheTichBomDuocKhiQuay1Vong *= -1.0f;
      _grbl._DSChuongTrinh = _DWIN._ChuongTrinhEEPROM;
      // _ThongSoBoard.CaiDatThongSoBoardVaoROM(_ThongSoBoard.ChuyenDoiChuongTrinhSangJsonVar(_grbl._DSChuongTrinh));
#ifdef DEBUG_CHUONGTRINHCHINH
      Serial.println("DIRECTION");
      // Serial.printf("SoVongDeBom1ml: %g\n", ChuongTrinhDangXuLy->SoVongDeBom1ml);
      Serial.printf("TheTichBomDuocKhiQuay1Vong: %g\n", ChuongTrinhDangXuLy->TheTichBomDuocKhiQuay1Vong);
#endif
    }
      break;
    case FASTPUMP:
      if (DangFastPump == false){
        ChuongTrinh_t ChuongTrinhDangXuLy = _DWIN._ChuongTrinhEEPROM[_DWIN.IndexChuongTrinhDangXuLy];
        if (ChuongTrinhDangXuLy.TheTichBomDuocKhiQuay1Vong > 0)
          WebUI::inputBuffer.push("G91A1000\r");
        else
          WebUI::inputBuffer.push("G91A-1000\r");
        DangFastPump = true;
      }
      else{
        _grbl.TamDungChay();
        DangFastPump = false;
      }
      break;
    case GOA1:
    case GOA2:
    {
      ChuongTrinh_t ChuongTrinhDangConfig = _DWIN._ChuongTrinhEEPROM[_DWIN.IndexChuongTrinhDangXuLy];
      uint8_t KhuVucDangChon = _MaLenhGRBL % GOA1;
      Serial.println(_MaLenhGRBL);
      Serial.println(KhuVucDangChon);
      Serial.println("X: " + String(ChuongTrinhDangConfig.ViTriA[KhuVucDangChon].X) + " Y: " + String(ChuongTrinhDangConfig.ViTriA[KhuVucDangChon].Y) + " H: " + String(ChuongTrinhDangConfig.ViTriH[KhuVucDangChon]) + " tocdo: " + String(ChuongTrinhDangConfig.tocdoXY));
      _grbl.DiChuyenToiViTri(ChuongTrinhDangConfig.ViTriA[KhuVucDangChon].X, ChuongTrinhDangConfig.ViTriA[KhuVucDangChon].Y, ChuongTrinhDangConfig.ViTriH[KhuVucDangChon], ChuongTrinhDangConfig.tocdoXY);
      break;
    }
    case GOB1:
    case GOB2:
    {
      ChuongTrinh_t ChuongTrinhDangConfig = _DWIN._ChuongTrinhEEPROM[_DWIN.IndexChuongTrinhDangXuLy];
      uint8_t KhuVucDangChon = _MaLenhGRBL % GOB1;
      _grbl.DiChuyenToiViTri(ChuongTrinhDangConfig.ViTriB[KhuVucDangChon].X, ChuongTrinhDangConfig.ViTriB[KhuVucDangChon].Y, ChuongTrinhDangConfig.ViTriH[KhuVucDangChon], ChuongTrinhDangConfig.tocdoXY);
      break;
    }
    case GOC1:
    case GOC2:
    {
      ChuongTrinh_t ChuongTrinhDangConfig = _DWIN._ChuongTrinhEEPROM[_DWIN.IndexChuongTrinhDangXuLy];
      uint8_t KhuVucDangChon = _MaLenhGRBL % GOC1;
      _grbl.DiChuyenToiViTri(ChuongTrinhDangConfig.ViTriC[KhuVucDangChon].X, ChuongTrinhDangConfig.ViTriC[KhuVucDangChon].Y, ChuongTrinhDangConfig.ViTriH[KhuVucDangChon], ChuongTrinhDangConfig.tocdoXY);
      break;
    }

    case LOWSPEED:
    {
      #ifdef DEBUG_CHUONGTRINHCHINH
      Serial.println("LOW SPEED");
      #endif
      TocDoTamThoi = TocDoLOW;
      SoBuocTamThoi = SoBuocLOW;
      break;
    }
    case MEDIUMSPEED:
    {
      #ifdef DEBUG_CHUONGTRINHCHINH
      Serial.println("MEDIUM SPEED");
      #endif
      TocDoTamThoi = TocDoMEDIUM;
      SoBuocTamThoi = SoBuocMEDIUM;
      break;
    }
    case FASTSPEED:
    {
      #ifdef DEBUG_CHUONGTRINHCHINH
      Serial.println("FAST SPEED");
      #endif
      TocDoTamThoi = TocDoFAST;
      SoBuocTamThoi = SoBuocFAST;
      break;
    }
    case UNLOCK:
     WebUI::inputBuffer.push("$X\n");
     delay(100);
     break;

    case RESET:
      if (sys.state == State::Alarm)
      {
        _grbl.VeViTriHome();
        break;
      }
      WebUI::inputBuffer.write(0x18);
      delay(100);
      WebUI::inputBuffer.push("$X\n");
      delay(100);
      break;

    case SHUTDOWN:
      Shutdown();
      break;
  }
}

void DWINCapNhatDSChuongTrinhBanDau()
{
  _DWIN.DuLieuBoardGuiLenHMI.DSChuongTrinh = _grbl._DSChuongTrinh;
}

void CapNhatDanhSachChuongTrinhLuuTrongEEPROM()
{
  vector<ChuongTrinh_t> DSChuongTrinhLuuTrongEeprom;
  ChuongTrinh_t ChuongTrinh1, ChuongTrinh2, ChuongTrinh3, ChuongTrinh4, ChuongTrinh5, ChuongTrinh6, ChuongTrinh7, ChuongTrinh8, ChuongTrinh9, ChuongTrinh10;
  ChuongTrinh1 = _ThongSoBoard.ChuyenDoiJSONVarSangChuongTrinh(_ThongSoBoard.ThongSo["ChuongTrinh1"],0);
  ChuongTrinh2 = _ThongSoBoard.ChuyenDoiJSONVarSangChuongTrinh(_ThongSoBoard.ThongSo["ChuongTrinh2"],0);
  ChuongTrinh3 = _ThongSoBoard.ChuyenDoiJSONVarSangChuongTrinh(_ThongSoBoard.ThongSo["ChuongTrinh3"],0);
  ChuongTrinh4 = _ThongSoBoard.ChuyenDoiJSONVarSangChuongTrinh(_ThongSoBoard.ThongSo["ChuongTrinh4"],0);
  ChuongTrinh5 = _ThongSoBoard.ChuyenDoiJSONVarSangChuongTrinh(_ThongSoBoard.ThongSo["ChuongTrinh5"],0);
  ChuongTrinh6 = _ThongSoBoard.ChuyenDoiJSONVarSangChuongTrinh(_ThongSoBoard.ThongSo["ChuongTrinh6"],0);
  ChuongTrinh7 = _ThongSoBoard.ChuyenDoiJSONVarSangChuongTrinh(_ThongSoBoard.ThongSo["ChuongTrinh7"],0);
  ChuongTrinh8 = _ThongSoBoard.ChuyenDoiJSONVarSangChuongTrinh(_ThongSoBoard.ThongSo["ChuongTrinh8"],0);
  ChuongTrinh9 = _ThongSoBoard.ChuyenDoiJSONVarSangChuongTrinh(_ThongSoBoard.ThongSo["ChuongTrinh9"],0);
  ChuongTrinh10 = _ThongSoBoard.ChuyenDoiJSONVarSangChuongTrinh(_ThongSoBoard.ThongSo["ChuongTrinh10"],0);

  DSChuongTrinhLuuTrongEeprom.push_back(ChuongTrinh1);
  DSChuongTrinhLuuTrongEeprom.push_back(ChuongTrinh2);
  DSChuongTrinhLuuTrongEeprom.push_back(ChuongTrinh3);
  DSChuongTrinhLuuTrongEeprom.push_back(ChuongTrinh4);
  DSChuongTrinhLuuTrongEeprom.push_back(ChuongTrinh5);
  DSChuongTrinhLuuTrongEeprom.push_back(ChuongTrinh6);
  DSChuongTrinhLuuTrongEeprom.push_back(ChuongTrinh7);
  DSChuongTrinhLuuTrongEeprom.push_back(ChuongTrinh8);
  DSChuongTrinhLuuTrongEeprom.push_back(ChuongTrinh9);
  DSChuongTrinhLuuTrongEeprom.push_back(ChuongTrinh10);

  for (uint8_t i = 0; i < DSChuongTrinhLuuTrongEeprom.size(); i++)
  {
    if ((DSChuongTrinhLuuTrongEeprom[i].TheTichBomDuocKhiQuay1Vong == 0) || (DSChuongTrinhLuuTrongEeprom[i].TheTichBomDuocKhiQuay1Vong == NAN))
    {
      DSChuongTrinhLuuTrongEeprom[i].TheTichBomDuocKhiQuay1Vong = HeSoCalibMacDinh;
    }
  }

  _grbl._DSChuongTrinh = DSChuongTrinhLuuTrongEeprom;
}


void CaiDatNguon(void)
{
  pinMode(2, OUTPUT); // chân buzzer
  pinMode(_ChanDocNguon, INPUT);
  pinMode(_ChanKichNguon, OUTPUT);
  digitalWrite(_ChanKichNguon, HIGH);

  digitalWrite(2, HIGH);
  delay(100);
  digitalWrite(2, LOW);
  // while (digitalRead(_ChanDocNguon))
  // {
  //   delay(1);
  // }
}

void KiemTraNguon(void)
{
    static uint32_t ThoiDiemHienTai = millis();
    static uint32_t ThoiDiemTruoc = millis();
    static bool DaTatNguon = false;
    if (DaTatNguon == true)
    {
      return;
    }
    if (!digitalRead(_ChanDocNguon)) // khi không nhấn
    {
      ThoiDiemHienTai = millis();
    }
    else                              // khi nhấn
    {
      ThoiDiemTruoc = millis();
      if (ThoiDiemHienTai - ThoiDiemTruoc >= 500)
      {
        digitalWrite(2, HIGH);
        delay(100);
        digitalWrite(2, LOW);
        DaTatNguon = true;     
        digitalWrite(_ChanKichNguon, LOW);
      }
    }
}

void ChayChuongTrinhChinh()
{
  _Flags.TurnONFlags();

  ThucThiCacTacVuTheoFlag();

  _Flags.TurnOFFFlags();
}
void ThucThiCacTacVuTheoFlag()
{
  if(_Flags.Flag.t1s)
  {
    _RTC.LayRTCTuDS3231();
    KiemTraNguon();
  }
}

void Shutdown(void)
{
  _grbl.VeToaDoGoc();
  delay(100);
  while (sys.state != State::Idle)
  {
    delay(1);
  }
  
  digitalWrite(_ChanKichNguon, LOW);
}