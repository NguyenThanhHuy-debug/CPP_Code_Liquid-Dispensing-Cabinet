#include "10_DWINHMI.h"
//----------------------------------------------------------------------------------------------------------------------
const byte     RxPin = 16;      // Chân Rx2 trên ESP32
const byte     TxPin = 17;      // Chân Tx2 trên ESP32
HardwareSerial _DWINSerial(2);  // Màn hình HMI DWIN giao tiếp với MCU qua UART2
#define DGUS_SERIAL Serial2     // Cổng UART giao tiếp giữa MCU với HMI
#define DGUS_BAUD 115200        // Baundrate giao tiếp UART giữa MCU và HMI
//----------------------------------------------------------------------------------------------------------------------


//=============================================================================================================================
// Biến codeHMI là static nên cần phải được khai báo lại tại
// đầu file .cpp theo cú pháp sau thì mới truy xuất được đến biến static
int DWINHMI::codeHMI = -1;  // Reset mã code trả về của các nút do user bấm trên HMI
int DWINHMI::lastcodeHMI = -1;  // Reset mã code cũ trả về của các nút do user bấm trên HMI
// uint8_t XYZ;
static uint32_t _time500ms = millis();
static uint32_t _time1s = millis();
static uint32_t _time5s = millis(); 

bool DWINHMI::ManualControl = false;
// static String TimeRunStart_c = "00:00";
// static int TimeNow = 0;
uint8_t holdAdd = 0; // 0 1 2
DanhSachMaLenh_t MaLenhManualControl = RESETMALENH;
uint32_t HoldingTime;
String twoDigits(int number);
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//=============================================================================================================================
DWINHMI::DWINHMI() : DWIN(DGUS_SERIAL, RxPin, TxPin, DGUS_BAUD) {}
//=============================================================================================================================
void DWINHMI::KhoiTaoDWIN(void) 
{
  DWIN::echoEnabled(false);

  DWIN::hmiCallBack(onHMIEvent);

  xTaskCreate(StaticTaskFunction_RunTime, "Task Name", 4096, (void *)this, 1, &xTaskHandle_RunTime);
  vTaskSuspend(xTaskHandle_RunTime);
  memset(DSIndexChuongTrinhDuocChon, -1, sizeof(DSIndexChuongTrinhDuocChon));
  IndexChuongTrinhDangXuLy = 0;
  }
//=============================================================================================================================
void DWINHMI::LangNgheDWIN(void) 
{
  // Listen HMI Events
  // Hàm này được gọi mỗi lần có nút nhấn được bấm
  DWIN::listen();
}
//=============================================================================================================================
void DWINHMI::onHMIEvent(String address, int lastByte, String message, String response) 
{
#pragma region onHMIEvent
  //Nếu cần kiểm tra địa chỉ, chuỗi được gửi từ màn hình về esp32 thì define debug

#ifdef DEBUG_DWIN
  Serial.println("OnEvent : [ A : " + address + " | D : " + String(lastByte, HEX) + " | M : " + message + " | R : " + response + " ]");
  // Serial.print("lastByte: ");
  // Serial.println(lastByte);
#endif
  if (response.indexOf("5a a5 82") == -1)  // Nếu chuỗi nhận về là việc cập nhật nút bấm từ HMI thì mới cập nhật codeHMI
  {
    String data = response.substring(18);
    data.replace(" ", "");
    const String hexDigits = "0123456789ABCDEF";
    data.toUpperCase();
    codeHMI = 0;
    for (int i = 0; i < data.length(); i++) {
      codeHMI <<= 4;
      codeHMI |= hexDigits.indexOf(data[i]);
    }
    //delay(1);
    ESP_LOGI("CODE", "CODEHMI :%d", codeHMI);
    ESP_LOGW("CODE", "Lastcode :%d", lastcodeHMI);
  }
  else
  {
    return;
  }

    if (!ManualControl) return;
    //extract the first 4 characters of the address
    address = address.substring(0, 4);

    static String prevAddr = address;

    #ifdef DEBUG_DWIN
    Serial.println("address: " + address);
    Serial.println("prevAddr: " + prevAddr);
    #endif

    //////////////// Xử lý khi nhấn nhả ////////////////////////
    if (((address == AddrReleaseXcong) && (prevAddr == AddrPressXcong)) || 
        ((address == AddrPressXcong) && (prevAddr == AddrPressXcong)))
    {
      MaLenhManualControl = BAMNUTDICHUYENXCONG;

    }
    else if (((address == AddrReleaseXtru) && (prevAddr == AddrPressXtru)) || 
              ((address == AddrPressXtru) && (prevAddr == AddrPressXtru)))
    {
      MaLenhManualControl = BAMNUTDICHUYENXTRU;

    }

    else if (((address == AddrReleaseYcong) && (prevAddr == AddrPressYcong)) || 
              ((address == AddrPressYcong) && (prevAddr == AddrPressYcong)))
    {
      MaLenhManualControl = BAMNUTDICHUYENYCONG;

    }
    else if (((address == AddrReleaseYtru) && (prevAddr == AddrPressYtru) || 
              ((address == AddrPressYtru) && (prevAddr == AddrPressYtru))))
    {
      MaLenhManualControl = BAMNUTDICHUYENYTRU;

    }

    else if (((address == AddrReleaseZcong) && (prevAddr == AddrPressZcong) || 
              (address == AddrPressZcong) && (prevAddr == AddrPressZcong)))
    {
      MaLenhManualControl = BAMNUTDICHUYENZCONG;

    }
    else if (((address == AddrReleaseZtru) && (prevAddr == AddrPressZtru) || 
              (address == AddrPressZtru) && (prevAddr == AddrPressZtru)))
    {
      MaLenhManualControl = BAMNUTDICHUYENZTRU;

    }
    
    ////////////// Xử lý khi nhấn giữ ////////////////////////

    else if (address == AddrHoldXcong)
    {
      MaLenhManualControl = HOLDXCONG;

    }
    else if (address == AddrHoldXtru)
    {
      MaLenhManualControl = HOLDXTRU;

    }
    else if (address == AddrHoldYcong)
    {
      MaLenhManualControl = HOLDYCONG;

    }
    else if (address == AddrHoldYtru)
    {
      MaLenhManualControl = HOLDYTRU;

    }
    else if (address == AddrHoldZcong)
    {
      MaLenhManualControl = HOLDZCONG;

    }
    else if (address == AddrHoldZtru)
    {
      MaLenhManualControl = HOLDZTRU;

    }

    else if ((address == AddrReleaseXcong) ||
            (address == AddrReleaseXtru) ||
            (address == AddrReleaseYcong) ||
            (address == AddrReleaseYtru) ||
            (address == AddrReleaseZcong) ||
            (address == AddrReleaseZtru))
    {
      MaLenhManualControl = DUNGDICHUYEN;

    }
    else 
    {
      MaLenhManualControl = RESETMALENH; 
    }
    prevAddr = address;

#pragma endregion onHMIEvent
}
//=============================================================================================================================
/*Code thực hiện việc chuyển trang*/
void DWINHMI::ChuyenTrangTheoBamChonCuaUserTrenHMI(void)
{
#pragma region ChuyenTrangTheoBamChonCuaUserTrenHMI
    switch (codeHMI)
    {
      case -1:
      break;
    case _HMIBamNutWiFiTrangCaiDat:
      setPage(_TrangWiFi);
      codeHMI = -1;
      break;
    case _HMIBamNutINFORTrangCaiDat:
    {
      DWIN::setPage(_TrangThongTin);
      codeHMI = -1;
      break;
    }
    case _HMIBamNutExitTrangSetHome:
    {
      DWIN::setPage(_TrangCaiDat);
      codeHMI = -1;
      break;
    }
    case _HMIBamNutSetting:
    {
      DWIN::setPage(_TrangCaiDat);
      codeHMI = -1;
      break;
    }
    case _HMIBamNutBackTrangCaiDat:
    {
      if (DangOViTriHome() == true)
        {
          DWIN::setPage(_TrangCaiDatCheDoChuongTrinh_HOME);
        }
        else
        {
          DWIN::setPage(_TrangCaiDatCheDoChuongTrinh_notHOME);
        }
      break;
    }
    case _HMIBamNutCalibTrangCaiDat:
    {
      CheDoVaoTrangChonChuongTrinh = _VaoTrangTuNutSelectTrangCalib;
      DWIN::setPage(_TrangCalib);
      break;
    }
    case _HMIBamNutExitTrangCalib:
    {
      DWIN::setPage(_TrangCaiDat);
      codeHMI = -1;
      break;
    }
    
    /* =============================== END: CODE TRANG 130 BẮT ĐẦU CHẠY SET HOME ===============================*/
    /* =============================== BEGIN: CODE TRANG 131 CÀI ĐẶT CHẾ ĐỘ CHƯƠNG TRÌNH ===============================*/
    case _HMIBamNutRunTrangCaiDatCheDoChuongTrinh:
    {
      DaChayXongCacKhuVuc_oneTimeMulti = false;
      bool CoTonTaiChuongTrinhDuocChon = false;
      for (uint8_t i = 0; i < sizeof(DSIndexChuongTrinhDuocChon); i++)
      {
        if (DSIndexChuongTrinhDuocChon[i] != -1)
        {
          CoTonTaiChuongTrinhDuocChon = true;
          break;
        }
      }

      if (CoTonTaiChuongTrinhDuocChon == false)
      {
        codeHMI = -1;
        return;
      }

      for (uint8_t i = 0; i < sizeof(DSIndexChuongTrinhDuocChon); i++)
      {
        if (DSIndexChuongTrinhDuocChon[i] == -1)
        {
          continue;
        }
        IdxKhuVucDangXuLy = i;
        ResetFullMau(DSIndexChuongTrinhDuocChon[i], MauDo, i);
        // delay(20);
      }
      flushDWIN();
      if (DangOViTriHome() == true)
      {
        DWINHMI::setPage(_TrangHienThiPhanPhoiThoiGianThuc_OFF_HOME);
      }
      else
      {
        DWINHMI::setPage(_TrangHienThiPhanPhoiThoiGianThuc_OFF_notHOME);
      }
      break;
    }
    case _HMIBamNutSetupProgramTrangCaiDat:
    {
      /* Dòng code này giúp ta xác định việc chọn chương trình để làm gì ?? */
      CheDoVaoTrangChonChuongTrinh = _VaoTrangTuNutSetupProgramTrangCaiDatCheDoChuongTrinh;
      GuiTextLenHMI(_VPTextHienThiChuongTrinhTrangChonChuongTrinh, "");
      DWIN::setPage(_TrangChonChuongTrinh);
      break;
    }
    /* =============================== END: CODE TRANG 131 CÀI ĐẶT CHẾ ĐỘ CHƯƠNG TRÌNH ===============================*/
    /* =============================== BEGIN: CODE TRANG 132 TRANG CHỌN CHƯƠNG TRÌNH ===============================*/
    case _HMIBamNutOKTrangChonChuongTrinh:
    {
      /* Khi và chỉ khi người dùng xác định được chương trình sẽ chọn thì mới chuyển trang */
      if (lastcodeHMI >= _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh && lastcodeHMI <= _HMIBamNutChonChuongTrinh10TrangChonChuongTrinh)
      {
        HienThiIconChuongTrinhDuocChon(_HMIBamNutChonChuongTrinh1TrangChonChuongTrinh);

        /* Tùy thuộc vào tính năng chọn chương trình để làm gì mà ta có việc chuyển trang tương ứng */
        switch (CheDoVaoTrangChonChuongTrinh)
        {
          case _VaoTrangTuCacNutProgramTrangCaiDatCheDoChuongTrinh:
            DWIN::setPage(_TrangPhanPhoiViTri);
            break;
          case _VaoTrangTuNutChangeTrangPhanPhoiViTri:
            DWIN::setPage(_TrangThietLapCheDoChayMulti);
            break;
          case _VaoTrangTuNutCopyTrangThongSoChuongTrinh:
            setPage(_TrangCaiDat);
            break;
          case _VaoTrangTuNutSetupProgramTrangCaiDatCheDoChuongTrinh:
            if (lastcodeHMI >= _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh && lastcodeHMI < _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh + SoLuongChuongTrinhCoDinh)
            {
              CheDoVaoTrangBanPhim = _VaoTrangBanPhimMatKhau;
              GuiTextLenHMI(_VpTextMatKhau, "");
              GuiTextLenHMI(_VpTextCanhBaoChuoiDangNhap, "");
              setPage(_TrangBanPhimMauKhau);
            }
            else
            {
              DWIN::setPage(_TrangThongSoChuongTrinh);
            }
            break;
          case _VaoTrangTuNutSelectTrangCalib:
            if (lastcodeHMI >= _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh && lastcodeHMI < _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh + SoLuongChuongTrinhCoDinh)
            {
              CheDoVaoTrangBanPhim = _VaoTrangBanPhimMatKhau;
              GuiTextLenHMI(_VpTextMatKhau, "");
              GuiTextLenHMI(_VpTextCanhBaoChuoiDangNhap, "");
              setPage(_TrangBanPhimMauKhau);
            }
            else
            {
            DWIN::setPage(_TrangCalib);
            }
            break;
          case _VaoTrangTuNutSelectTrangXemConfig:
            DWIN::setPage(_TrangXemThongSoConfig);
            break;
        }
      }
      break;
    }
    case _HMIBamNutExitTrangChonChuongTrinh:
    {
      /* Tùy thuộc vào tính năng chọn chương trình để làm gì mà ta có việc chuyển trang tương ứng */
      switch (CheDoVaoTrangChonChuongTrinh)
      { 
        case _VaoTrangTuNutSetupProgramTrangCaiDatCheDoChuongTrinh:
          DWIN::setPage(_TrangCaiDat);
          break;
        case _VaoTrangTuCacNutProgramTrangCaiDatCheDoChuongTrinh:
          if (DangOViTriHome() == true)
          {
            DWIN::setPage(_TrangCaiDatCheDoChuongTrinh_HOME);
          }
          else
          {
            DWIN::setPage(_TrangCaiDatCheDoChuongTrinh_notHOME);
          }
          break;
        case _VaoTrangTuNutCopyTrangThongSoChuongTrinh:
          DWIN::setPage(_TrangThongSoChuongTrinh);
          break;
        case _VaoTrangTuNutChangeTrangPhanPhoiViTri:
          DWIN::setPage(_TrangPhanPhoiViTri);
          break;
        case _VaoTrangTuNutSelectTrangCalib:
          DWIN::setPage(_TrangCalib);
          break;
        case _VaoTrangTuNutSelectTrangXemConfig:
          DWIN::setPage(_TrangXemThongSoConfig);
          break;
      }
      codeHMI = -1;
      break;
    }
    /* =============================== END: CODE TRANG 132 TRANG CHỌN CHƯƠNG TRÌNH ===============================*/
    /* =============================== BEGIN: CODE TRANG 133 TRANG PHÂN PHỐI VỊ TRÍ ===============================*/
    case _HMIBamNutCHANGETrangPhanPhoiViTri:
    {
        /* Dòng code này giúp ta xác định việc chọn chương trình để làm gì ?? */
      CheDoVaoTrangChonChuongTrinh = _VaoTrangTuNutChangeTrangPhanPhoiViTri;
      GuiTextLenHMI(_VPTextHienThiChuongTrinhTrangChonChuongTrinh, "");
      DWIN::setPage(_TrangChonChuongTrinh);
      break;
    }
    case _HMIBamNutOKTrangPhanPhoiViTri:
    {
      if (DangOViTriHome() == true)
      {
        DWIN::setPage(_TrangCaiDatCheDoChuongTrinh_HOME);
      }
      else
      {
        DWIN::setPage(_TrangCaiDatCheDoChuongTrinh_notHOME);
      }
      
      break;
    }
    case _HMIBamNutEXITTrangPhanPhoiViTri:
    {
      if (DangOViTriHome() == true)
      {
        DWIN::setPage(_TrangCaiDatCheDoChuongTrinh_HOME);
      }
      else
      {
        DWIN::setPage(_TrangCaiDatCheDoChuongTrinh_notHOME);
      }

      XoaHinhTron(_VpAddressVung1KhayChon);
      XoaHinhTron(_VpAddressVung2KhayChon);
      XoaHinhTron(_VpAddressVung3KhayChon);
      XoaHinhTron(_VpAddressVung4KhayChon);
      XoaMau(_VpAddressVung1KhayChonHienThiMau);
      XoaMau(_VpAddressVung2KhayChonHienThiMau);
      XoaMau(_VpAddressVung3KhayChonHienThiMau);
      XoaMau(_VpAddressVung4KhayChonHienThiMau);
      break;
    }
    /* =============================== END: CODE TRANG 133 TRANG PHÂN PHỐI VỊ TRÍ ===============================*/
    /* =============================== BEGIN: CODE TRANG 134 TRANG PHÂN PHỐI THỜI GIAN THỰC ===============================*/
    case _HMIBamNutEXITTrangHienThiPhanPhoiThoiGianThuc:
    {
      if (sys.state == State::Homing)
      {
        codeHMI = -1;
        return;
      }
      DaChayXongCacKhuVuc_oneTimeMulti = false;
      /* Chỉ thực hiện Exit khi đã máy đã tắt */
      if (BamNutRunON == false)
      {
        if (DangOViTriHome() == true)
        {
          DWINHMI::setPage(_TrangCaiDatCheDoChuongTrinh_HOME);
        }
        else
        {
          DWINHMI::setPage(_TrangCaiDatCheDoChuongTrinh_notHOME);
        }
        XoaToanBoHinhTron();
        

      }
      break;
    }
    /* =============================== END: CODE TRANG 134 TRANG PHÂN PHỐI THỜI GIAN THỰC ===============================*/
    /* =============================== BEGIN: CODE TRANG 135 TRANG THIẾT LẬP CHẾ ĐỘ CHẠY MULTI ===============================*/
    case _HMIBamNutMODIFY1TrangThietLapCheDoChayMulti:
    case _HMIBamNutMODIFY2TrangThietLapCheDoChayMulti:
    case _HMIBamNutMODIFY3TrangThietLapCheDoChayMulti:
    case _HMIBamNutMODIFY4TrangThietLapCheDoChayMulti:
    case _HMIBamNutMODIFY5TrangThietLapCheDoChayMulti:
    {
      // DWIN::setPage(_TrangPhanPhoiViTri);
      CheDoVaoTrangChonChuongTrinh = _VaoTrangTuCacNutProgramTrangCaiDatCheDoChuongTrinh;
      GuiTextLenHMI(_VPTextHienThiChuongTrinhTrangChonChuongTrinh, "");
      DWIN::setPage(_TrangChonChuongTrinh);
      break;
    }
    case _HMIBamNutEXITTrangThietLapCheDoChayMulti:
    {
        DWIN::setPage(_TrangCaiDatCheDoChuongTrinh_notHOME);
      break;
    }
    /* =============================== END: CODE TRANG 135 TRANG THIẾT LẬP CHẾ ĐỘ CHẠY MULTI ===============================*/
    /* =============================== BEGIN: CODE TRANG 136 TRANG THÔNG SỐ CHƯƠNG TRÌNH ===============================*/
    case _HMIBamNutCOPYTrangThongSoChuongTrinh:
    {
      /* Dòng code này giúp ta xác định việc chọn chương trình để làm gì ?? */
      CheDoVaoTrangChonChuongTrinh = _VaoTrangTuNutCopyTrangThongSoChuongTrinh;
      GuiTextLenHMI(_VPTextHienThiChuongTrinhTrangChonChuongTrinh, "");
      DWIN::setPage(_TrangChonChuongTrinh);
      break;
    }
    case _HMIBamNutMODIFYTrangThongSoChuongTrinh:
    {
      DWIN::setPage(_TrangThietLapChuongTrinhDiChuyen);
      break;
    }
    case _HMIBamNutEXITTrangThongSoChuongTrinh:
    {
      // CapNhatHienThiThongTinTenChuongTrinhKhiVaoTrangChonChuongTrinh();
      GuiTextLenHMI(_VPTextHienThiChuongTrinhTrangChonChuongTrinh, "");
      DWIN::setPage(_TrangChonChuongTrinh);
      codeHMI = -1;
      break;
    }
    /* =============================== END: CODE TRANG 136 TRANG THÔNG SỐ CHƯƠNG TRÌNH ===============================*/
    /* =============================== BEGIN: CODE TRANG 137 TRANG THIẾT LẬP CHƯƠNG TRÌNH DI CHUYỂN ===============================*/
    case _HMIBamNutTUBEPERROWTrangThietLapChuongTrinhDiChuyen:
    {
      CheDoVaoTrangBanPhim = _VaoTrangThietLapSoOngTrenMotHang;
      DWIN::setPage(_TrangThietLapSoOngTrenMotHang);
      break;
    }
    case _HMIBamNutTUBEPERCOLUMNTrangThietLapChuongTrinhDiChuyen:
    {
      CheDoVaoTrangBanPhim = _VaoTrangThietLapSoOngTrenMotCot;
      DWIN::setPage(_TrangThietLapSoOngTrenMotCot);
      break;
    }
    case _HMIBamNutSPEEDXYAXISrangThietLapChuongTrinhDiChuyen:
    {
      CheDoVaoTrangBanPhim = _VaoTrangThietLapTocDoTrucXY;
      DWIN::setPage(_TrangThietLapTocDoTrucXY);
      break;
    }
    case _HMIBamNutNEXTTrangThietLapChuongTrinhDiChuyen:
    {
      //DWIN::setPage(_TrangLayViTriChonDiem);
      DWIN::setPage(_TrangThietLapTheTichPhanPhoi);
      break;
    }
    case _HMIBamNutNextTrangThietLapTheTichPhanPhoi:
    {
      DWIN::setPage(_TrangLayViTriChonDiem);
      break;
    }
    case _HMIBamNutBackTrangThietLapTheTichPhanPhoi:
    {
      DWIN::setPage(_TrangThietLapChuongTrinhDiChuyen);

      break;
    }
    case _HMIBamNutGETATrangLayViTriChonDiem:
    {
      DWIN::setPage(_TrangLayViTriXacNhanA);
      CheDoVaoTrangGETABCH = _VaoTrangGETA;
      break;
    }
    case _HMIBamNutGETBTrangLayViTriChonDiem:
    {
      DWIN::setPage(_TrangLayViTriXacNhanB);
      CheDoVaoTrangGETABCH = _VaoTrangGETB;
      break;
    }
    case _HMIBamNutGETCTrangLayViTriChonDiem:
    {
      DWIN::setPage(_TrangLayViTriXacNhanC);
      CheDoVaoTrangGETABCH = _VaoTrangGETC;
      break;
    }
    case _HMIBamNutGETHTrangLayViTriChonDiem:
    {
      DWIN::setPage(_TrangLayViTriXacNhanH);
      CheDoVaoTrangGETABCH = _VaoTrangGETH;
      break;
    }
    case _HMIBamNutNEXTTrangLayViTriChonDiem:
    {
      DWIN::setPage(_TrangCaiDat);
      HienThiNoiDungChuongTrinh(IndexChuongTrinhDangXuLy, _VPTextHienThiChucNangProgramTrangCaiDatCheDoChuongTrinh);
      break;
    }
    case _HMIBamNutBACKTrangLayViTriChonDiem:
    {
      DWIN::setPage(_TrangThietLapTheTichPhanPhoi);
      break;
    }
    /* =============================== END: CODE TRANG 141 TRANG LẤY VỊ TRÍ CHỌN ĐIỂM ===============================*/
    /* =============================== BEGIN: CODE TRANG 142 TRANG LẤY VỊ TRÍ CHỌN ĐIỂM ===============================*/
    case _HMIBamNutBackTrangLayViTriXacNhan:
    {
      DWIN::setPage(_TrangLayViTriChonDiem);
      break;
    }
    /* =============================== END: CODE TRANG 142 TRANG LẤY VỊ TRÍ CHỌN ĐIỂM ===============================*/
    /* =============================== BEGIN: CODE TRANG 146 TRANG CÀI ĐẶT ===============================*/
    /* =============================== END: CODE TRANG 146 TRANG CÀI ĐẶT ===============================*/
    /* =============================== BEGIN: CODE TRANG 147 TRANG CALIB ===============================*/
    case (_HMIBamNutSelectTrangCalib):
    {
      CheDoVaoTrangChonChuongTrinh = _VaoTrangTuNutSelectTrangCalib;
      GuiTextLenHMI(_VPTextHienThiChuongTrinhTrangChonChuongTrinh, "");
      DWIN::setPage(_TrangChonChuongTrinh);
      break;
    }
    case (_HMIBamNutSelectTrangXemConifg):
    {
      CheDoVaoTrangChonChuongTrinh = _VaoTrangTuNutSelectTrangXemConfig;
      GuiTextLenHMI(_VPTextHienThiChuongTrinhTrangChonChuongTrinh, "");
      DWIN::setPage(_TrangChonChuongTrinh);
      break;
    }
    case _HMIBamTextActualWeight:
    {
      CheDoVaoTrangBanPhim = _VaoTrangBanPhimActualWeight;
      DWIN::setPage(_TrangBanPhimActualWeight);
      break;
    }
    case _HMIBamTextDensity:
    {
      CheDoVaoTrangBanPhim = _VaoTrangBanPhimDensity;
      DWIN::setPage(_TrangBanPhimDensity);
      break;
    }
    case(_HMIBamNutVolumeTrangThietLapTheTichPhanPhoi):
    {
      CheDoVaoTrangBanPhim = _VaoTrangBanPhimVolume;
      DWIN::setPage(_TrangBanPhimVolume);
      break;
    }
    case(_HMIBamNutTubingIDTrangThietLapTheTichPhanPhoi):
    {
      CheDoVaoTrangBanPhim = _VaoTrangBanPhimTubeID;
      DWIN::setPage(_TrangBanPhimTubeID);
      break;
    }
    case (_HMIBamNutBackTrangInfo):
    {
      DWIN::setPage(_TrangCaiDat);
      codeHMI = -1;
      break;
    }
    case(_HMIBamNutPumpSpeedTrangThietLapTheTichPhanPhoi):
    {
      CheDoVaoTrangBanPhim = _VaoTrangBanPhimPumpSpeed;
      DWIN::setPage(_TrangBanPhimPumpSpeed);
      break;
    }
    case(_HMIBamNutAntiDropTrangThietLapTheTichPhanPhoi):
    {
      CheDoVaoTrangBanPhim = _VaoTrangBanPhimAntiDrop;
      DWIN::setPage(_TrangBanPhimAntiDrop);
      break;
    }
    case(_HMIBamNutConfigTrangCaiDatCheDoChuongTrinh):
    {
      DWIN::setPage(_TrangXemThongSoConfig);
      break;
    }
    case (_HMIBamNutExitTrangXemConfig):
    {
      setPage(_TrangCaiDat);
      break;
    }
    case (_HMIBamNutModifyProgramTrangXemConfig):
    {
      if(IndexChuongTrinhDangXuLy != -1)
        DWIN::setPage(_TrangThongSoChuongTrinh);
      break;
    }
    case(_HMIBamNutManualControlTrangXemConfig):
    {
      DWIN::setPage(_TrangManualControl);
      ManualControl = true;
      break;
    }
    case(_HMIBamNutSecondTrangMULTIPROGRAMSETUP):
    {
      CheDoVaoTrangBanPhim = _VaoTrangBanPhimSecondLoop;
      DWIN::setPage(_TrangBanPhimSecondLoop);
      break;
    }

    case _HMIBamNutEDITTrangThongSoChuongTrinh:
    {
      CheDoVaoTrangBanPhimMoRong = _VaoTrangBanPhimMoRongTuNutEDITTrangThongSoChuongTrinh;
      DWIN::setPage(_TrangBanPhimMoRong);
      break;
    }
    case (_HMIBamNutTimeTrangCaiDat):
    {
      DWIN::setPage(_TrangThoiGian);
      CapNhatHienThiDuLieuThoiGianTuDS3231();
      break;
    }
    case (_HMIBamNutSetHourTrangDatenTime):
      CheDoCaiDatThoiGian = _CaiDatGio;
      GuiTextLenHMI(_VPTextKieuThoiGianTrangBanPhimDaynTime,"Hour");
      goto hienthi;
    case (_HMIBamNutSetMinuteTrangDatenTime):
      CheDoCaiDatThoiGian = _CaiDatPhut;
      GuiTextLenHMI(_VPTextKieuThoiGianTrangBanPhimDaynTime,"Minute");
      goto hienthi;
    case (_HMIBamNutSetDayTrangDatenTime):
      CheDoCaiDatThoiGian = _CaiDatNgay;
      GuiTextLenHMI(_VPTextKieuThoiGianTrangBanPhimDaynTime,"Day");
      goto hienthi;
    case (_HMIBamNutSetMonthTrangDatenTime):
      CheDoCaiDatThoiGian = _CaiDatThang;
      GuiTextLenHMI(_VPTextKieuThoiGianTrangBanPhimDaynTime,"Month");
      goto hienthi;
    case (_HMIBamNutSetYearTrangDatenTime):
    CheDoCaiDatThoiGian = _CaiDatNam;
      GuiTextLenHMI(_VPTextKieuThoiGianTrangBanPhimDaynTime,"Year");
      goto hienthi;
      hienthi:
      CheDoVaoTrangBanPhim = _VaoTrangBanPhimCaiDatThoiGian;
      GuiTextLenHMI(_VpTextCanhBaoChuoiDangNhap, "");
      DWIN::setPage(_TrangBanPhimCaiDatThoiGian);
      // codeHMI = -1;
      break;
    case (_HMIBamNutEnterTrangDatenTime):
    {
      Serial.println("_HMIBamNutEnterTrangDatenTime");
      DuLieuHMIGuiXuongBoard.CapNhatThoiGianRTC = true;
      String Ngay_str = DuLieuHMIGuiXuongBoard.RTC_Ngay +  "/" + DuLieuHMIGuiXuongBoard.RTC_Thang + "/" + DuLieuHMIGuiXuongBoard.RTC_Nam.substring(2);
      String Gio_str = DuLieuHMIGuiXuongBoard.RTC_Gio + ":" + DuLieuHMIGuiXuongBoard.RTC_Phut;
      Serial.println(Ngay_str);
      Serial.println(Gio_str);
      GuiTextLenHMI(_VPTextNgay,Ngay_str);
      GuiTextLenHMI(_VPTextGio,Gio_str);
    }
    case (_HMIBamNutReturnTrangDatenTime):
      DWIN::setPage(_TrangCaiDat);
      codeHMI = -1;
      break;
    case _HMIBamNutEXITTrangManualControl:
      DWIN::setPage(_TrangXemThongSoConfig);
      break;
    case _HMIBamNutGetA1:
    case _HMIBamNutGetA2:
    case _HMIBamNutGetB1:
    case _HMIBamNutGetB2:
    case _HMIBamNutGetC1:
    case _HMIBamNutGetC2:
      if (digitalRead(X_LIMIT_PIN) == LOW 
      ||  digitalRead(X2_SENSOR_PIN) == LOW
      ||  digitalRead(Y_LIMIT_PIN) == LOW
      || digitalRead(Y2_SENSOR_PIN) == LOW)
      {
        GuiTextLenHMI(_VPTextXacNhanGETTrangLayViTri, "Check Limit XY");
        codeHMI = -1;
      }
      else
      {
        setPage(_TrangLayViTriChonDiem);
      }
      break;
    case _HMIBamNutGetH1:
    case _HMIBamNutGetH2:
      if (digitalRead(Z_LIMIT_PIN) == LOW
      || digitalRead(Z2_SENSOR_PIN) == LOW)
      {
        GuiTextLenHMI(_VPTextXacNhanGETTrangLayViTri, "Check Limit Z");
        codeHMI = -1;
      }
      else
      {
        setPage(_TrangLayViTriChonDiem);
      }
      break;
  }
#pragma endregion ChuyenTrangTheoBamChonCuaUserTrenHMI
}
//=============================================================================================================================

void DWINHMI::XuLySetHome()
{
  static bool flagOneTime = false;
  if(flagOneTime == false)
  {
    while(DWIN::getPage() == _TrangBatDauChaySetHome && flagOneTime == false)
    {
      flagOneTime = true;

      CapNhatHienThiDuLieuThoiGianTuDS3231();
      GuiTextLenHMI(_VPTextNgay,DuLieuBoardGuiLenHMI.RTC_ChuoiNgay);
      GuiTextLenHMI(_VPTextGio,DuLieuBoardGuiLenHMI.RTC_ChuoiGio);  
      GuiTextLenHMI(_VPTextGetLocationStep,"1.0");
      GuiTextLenHMI(_VPTextGetLocationFeed,"500");
      GuiTextLenHMI(_VPTextSPEEDXYZTrangLayViTri,"LOW");

      for (ChuongTrinh_t& ChuongTrinh : _ChuongTrinhEEPROM)
      {
        // ChuongTrinh.DsToaDo[IdxKhuVucDangXuLy].clear();
        for (uint8_t i = 0; i < ChuongTrinh.rackX; i++)
        {
          for (uint8_t j = 0; j < ChuongTrinh.rackY; j++)
          {
            for (uint8_t KhuVuc = 0; KhuVuc < SoLuongKhuVucToiDa; KhuVuc++)
            {
              ChuongTrinh.DsToaDo[KhuVuc].push_back({i, j});
            }
          }
        }
      }
      CapNhatHienThiThongTinTenChuongTrinhKhiVaoTrangChonChuongTrinh();
      codeHMI = _HMIBamNutChonProgram1TrangCaiDatCheDoChuongTrinh;
      HienThiIconChuongTrinhDuocChon(_HMIBamNutChonProgram1TrangCaiDatCheDoChuongTrinh);
    }
  }
  /* Chuyển trang khi go home OK */
  if(DuLieuBoardGuiLenHMI.TrangThaiXacNhanSetHome == true)
  {
      DWIN::setPage(_TrangCaiDatCheDoChuongTrinh_HOME);
      DuLieuBoardGuiLenHMI.TrangThaiXacNhanSetHome = false;
  }

}
//=============================================================================================================================
/* Đoạn code này được sử dụng để xử lý chọn chương trình ở chế độ simple hoặc multi */
void DWINHMI::XuLyChonChuongTrinhSeChay(void)
{
  if(codeHMI ==  _HMIBamNutEXITTrangHienThiPhanPhoiThoiGianThuc || codeHMI == _HMIBamNutBackTrangCaiDat)
  {
    GuiTextLenHMI(_VPTextHienThiTenProgramTrangCaiDatCheDoChuongTrinh, "");
    // GuiTextLenHMI(_VPTextHienThiChucNangProgramTrangCaiDatCheDoChuongTrinh, "");
  }
  /* Khi bấm vào các icon để chuyển sang tick xanh để chọn program */
  else if (codeHMI >= _HMIBamNutChonProgram1TrangCaiDatCheDoChuongTrinh && codeHMI <= _HMIBamNutChonProgram10TrangCaiDatCheDoChuongTrinh)
  {
    HienThiIconChuongTrinhDuocChon(_HMIBamNutChonProgram1TrangCaiDatCheDoChuongTrinh);
  }
  
  /* Kiểm tra nếu như chế độ vào trang chọn chương trình là khi ta thay đổi chương trình hiển thị tại trang cài đặt chế độ chương trình */
  if (CheDoVaoTrangChonChuongTrinh == _VaoTrangTuCacNutProgramTrangCaiDatCheDoChuongTrinh)
  {
    /* Hiển thị dữ liệu khi bấm chọn chương trình */
    if (codeHMI >= _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh && codeHMI <= _HMIBamNutChonChuongTrinh10TrangChonChuongTrinh && lastcodeHMI != codeHMI)
    {
      CapNhatHienThiThongTinChuongTrinhTrangChonChuongTrinh();
    }
  
    /* Khi kết thúc việc chọn bấm OK */
    else if (codeHMI == _HMIBamNutOKTrangChonChuongTrinh)
    {

      if (!((lastcodeHMI >= _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh) && (lastcodeHMI <= _HMIBamNutChonChuongTrinh10TrangChonChuongTrinh)))
      {
        Serial.println("Khong phai nut chon chuong trinh");
        codeHMI = -1;
        return;
      }

      DSIndexChuongTrinhDuocChon[IdxKhuVucDangXuLy] = lastcodeHMI % _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh;

      GuiTextLenHMI(_VPTextHienThiTenProgramTrangCaiDatCheDoChuongTrinh, _ChuongTrinhEEPROM[DSIndexChuongTrinhDuocChon[IdxKhuVucDangXuLy]].tenchuongtrinh);
      HienThiNoiDungChuongTrinh(DSIndexChuongTrinhDuocChon[IdxKhuVucDangXuLy], _VPTextHienThiChuongTrinhTrangChonChuongTrinh);
      HienThiThongTinPhanPhoi();

      for (uint8_t KhuVuc = 0; KhuVuc < sizeof(DSIndexChuongTrinhDuocChon); KhuVuc++)
      {
        if (DSIndexChuongTrinhDuocChon[KhuVuc] == -1)
        {
          continue;
        }
        GuiTextLenHMI(_VPTextTenProgramArea1 + 0x100 * KhuVuc, _ChuongTrinhEEPROM[DSIndexChuongTrinhDuocChon[KhuVuc]].tenchuongtrinh);
      }
    }
  }


}
//=============================================================================================================================
void DWINHMI::XuLyConfigChuongTrinh(void)
{
  /* Hiển thị các dữ liệu chương trình khi vào trang chọn chương trình */
  if (codeHMI == _HMIBamNutSetupProgramTrangCaiDat)
  {
      // CapNhatHienThiThongTinTenChuongTrinhKhiVaoTrangChonChuongTrinh();
      codeHMI = -1;
  }
  if (CheDoVaoTrangChonChuongTrinh == _VaoTrangTuNutSetupProgramTrangCaiDatCheDoChuongTrinh)
  {
    /* Hiển thị dữ liệu khi bấm chọn chương trình */
    if (codeHMI >= _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh 
    && codeHMI <= _HMIBamNutChonChuongTrinh10TrangChonChuongTrinh && lastcodeHMI != codeHMI)
    {
      // IndexChuongTrinhDangXuLy = codeHMI % _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh;
      CapNhatHienThiThongTinChuongTrinhTrangChonChuongTrinh();
    }
    else if (codeHMI == _HMIBamNutOKTrangChonChuongTrinh)
    {
    if (!((lastcodeHMI >= _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh) && (lastcodeHMI <= _HMIBamNutChonChuongTrinh10TrangChonChuongTrinh)))
      {
        Serial.println("Khong phai nut chon chuong trinh");
        codeHMI = -1;
        return;
      }
      IndexChuongTrinhDangXuLy = lastcodeHMI % _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh;
      CapNhatHienThiThongTinChiTietChuongTrinh();
      HienThiThongTinABCH();
    }
  }
}
//=============================================================================================================================
/* Đoạn code này dùng để thiết lập chương trình ở chế độ multi bao gồm hiển thị
và bấm vào để thiết lập các vị trí tọa độ cần phân phối theo từng chương trình */
void DWINHMI::XuLyThietLapChuongTrinhCheDoMulti(void)
{
  /* Đoạn code này dùng để hiển thị các thông số khi cài đặt số ống sẽ phân phối khi vào bằng nút RUN hoặc khi cập nhật bằng nút OK */
  if (codeHMI >= _HMIBamNutMODIFY1TrangThietLapCheDoChayMulti && codeHMI <= _HMIBamNutMODIFY5TrangThietLapCheDoChayMulti)
  {
    // NutModidyDaBam = codeHMI % _HMIBamNutMODIFY1TrangThietLapCheDoChayMulti;
    IdxKhuVucDangXuLy = codeHMI % _HMIBamNutMODIFY1TrangThietLapCheDoChayMulti;
    GuiTextLenHMI(_VPTextHienThiTenProgramTrangPhanPhoiViTri, _ChuongTrinhEEPROM[IdxKhuVucDangXuLy].tenchuongtrinh);
  }
  else if((codeHMI == _HMIBamNutXoaTrangMULTIPROGRAMSETUP) && (oneTimeMulti == false))
  {
    oneTimeMulti = true;
    GuiTextLenHMI(_VPTextSecondTrangMULTIPROGRAMSETUP,"X");
    codeHMI = -1;
  }

  else if ((codeHMI == _HMIBamNutXoaTrangMULTIPROGRAMSETUP) && (oneTimeMulti == true))
  {
    oneTimeMulti = false;
    timeLoopDelay = 0;
    timeLoopDelay_c = "00:00:00";
    GuiTextLenHMI(_VPTextSecondTrangMULTIPROGRAMSETUP, "0");
    codeHMI = -1;
  }

  else if(codeHMI == _HMIBamNutSecondTrangMULTIPROGRAMSETUP)
  {
    oneTimeMulti = false;
    codeHMI = -1;
  }

  else if (codeHMI >= _HMIBamNutXoaArea1 && codeHMI <= _HMIBamNutXoaArea2)
  {
    uint8_t KhuVucCanXoa = codeHMI % _HMIBamNutXoaArea1;
    if (DSIndexChuongTrinhDuocChon[KhuVucCanXoa] == -1)
    {
      codeHMI = -1;
      return;
    }
    else
    {
      DSIndexChuongTrinhDuocChon[KhuVucCanXoa] = -1;
      GuiTextLenHMI(_VPTextTenProgramArea1 + 0x100 * KhuVucCanXoa, "");
      GuiTextLenHMI(_VPTextprogram1TrangThietLapChayCheDoMULTI + 0x100 * KhuVucCanXoa, "");
    }
  }
}
//=============================================================================================================================
/* Đoạn code này dùng để xử lý các tọa độ mà các ống cần phân phối */
void DWINHMI::XuLyChonViTriPhanPhoi(void)
{
  static ChuongTrinh_t ChuongTrinhTamThoi;
  static uint16_t KhayChonTamThoi[12][8];
  static uint8_t SoLuongOngThayDoi_Add_prev = 0;
  /* Khi bấm Run hoặc modify để vào trang phân phối thì cập nhật chương trình đang xử lý */
  if(codeHMI == _HMIBamNutOKTrangChonChuongTrinh && CheDoVaoTrangChonChuongTrinh == _VaoTrangTuCacNutProgramTrangCaiDatCheDoChuongTrinh)
  {
    // IndexChuongTrinhDangXuLy = IdxKhuVucDangXuLy;             
    IndexChuongTrinhDangXuLy = lastcodeHMI % _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh;                
    ChuongTrinhTamThoi = _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy];      // Lưu lại danh sách tọa độ ban đầu để nếu người dùng không lưu thì sẽ lấy lại danh sách tọa độ ban đầu
    for (uint8_t i = 0; i < 12; i++)                                        // backup khay chọn và chương trình gốc
    {
      for (uint8_t j = 0; j < 8; j++)
      {
        KhayChonTamThoi[i][j] = khaychon[i][j];
      }
    }

    //* Đảm bảo tất cả các icon được clear
    vitrivatoado.clear();
    VeToanBoHinhTron(_VpAddressVung1KhayChon, _ChuongTrinhEEPROM[DSIndexChuongTrinhDuocChon[IdxKhuVucDangXuLy]]);
    VeToanBoHinhTron(_VpAddressVung2KhayChon, _ChuongTrinhEEPROM[DSIndexChuongTrinhDuocChon[IdxKhuVucDangXuLy]]);
    VeToanBoHinhTron(_VpAddressVung3KhayChon, _ChuongTrinhEEPROM[DSIndexChuongTrinhDuocChon[IdxKhuVucDangXuLy]]);
    VeToanBoHinhTron(_VpAddressVung4KhayChon, _ChuongTrinhEEPROM[DSIndexChuongTrinhDuocChon[IdxKhuVucDangXuLy]]);
    delay(20);
    ResetFullMauChonToaDo(MauDo);
    SoLuongOngThayDoi = 0;
    SoLuongOngThayDoi_Add_prev = _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DsToaDo[IdxKhuVucDangXuLy].size();
    GuiTextLenHMI(_VpTextThayDoiSoLuongOng, "0");
  }

  else if(codeHMI == _HMIBamNutFULLTrangPhanPhoiViTri)
  {
    DWIN::setPage(_TrangPhanPhoiViTriFULL);
    _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DsToaDo[IdxKhuVucDangXuLy].clear();

    for (uint8_t i = 0; i < _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackX; i++)
    {
      for (uint8_t j = 0; j < _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackY; j++)
      {
        _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DsToaDo[IdxKhuVucDangXuLy].push_back({i, j});
      }
    }
    SoLuongOngThayDoi_Add_prev = _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DsToaDo[IdxKhuVucDangXuLy].size();
    ResetFullMauChonToaDo(MauDo);
    DWIN::setPage(_TrangPhanPhoiViTri);
  }
  /* Clear toàn bộ tọa độ */
  else if (codeHMI == _HMIBamNutCLEARTrangPhanPhoiViTri) 
  {
    DWIN::setPage(_TrangPhanPhoiViTriCLEAR);
    ResetFullMauChonToaDo(MauTrang);
    // _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DanhSachToaDo.clear();
    _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DsToaDo[IdxKhuVucDangXuLy].clear();
    DWIN::setPage(_TrangPhanPhoiViTri);
  }
  /* Khi bấm nút ADD */
  else if(codeHMI == _HMIBamNutADDTrangPhanPhoiViTri)
  {
    if(holdAdd == 0)
    {
      DWINHMI::setPage(_TrangPhanPhoiViTriADD);
      holdAdd = 1;
    }
    else if(holdAdd == 1)
    {
      holdAdd = 2;
      DWINHMI::setPage(_TrangPhanPhoiViTriSub);
    }
    else if(holdAdd == 2)
    {
      holdAdd = 0;
      DWINHMI::setPage(_TrangPhanPhoiViTri);
    }
  }


  else if(codeHMI == _HMIBamVaoVungChonToaDo)
  {
    // chuongtrinhxuly = _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy];
    /* Gui lenh len HMI de kiem tra tra ve */
    uint8_t data[] = {0x5A, 0xA5, 0x04, 0x83, 0x00, 0x16, 0x03};
    DGUS_SERIAL.write(data,sizeof(data));
    unsigned long startTime = millis();
    String resp;
    String X,Y;
    uint8_t count;
    while((millis() - startTime < 100))
    {
      if(DGUS_SERIAL.available() > 0){
        int c = DGUS_SERIAL.read();
        resp.concat(" " + String(c, HEX));
        
        if(count == 9 || count == 10)
          X += String(c, HEX);
        if(count == 11 || count == 12)
          Y += String(c, HEX);       
        count++;
      }
    }
    uint32_t vitriX = strtol(X.c_str(),NULL,16);
    uint32_t vitriY = strtol(Y.c_str(),NULL,16);
    //Serial.printf("Count %d, du lieu nhan duoc %s | X: %d | Y: %d \r\n",count,resp.c_str(),vitriX,vitriY);
    uint8_t e_data[] = {0x5A,0x5A,0x05,0x82,0x00,0x16,0x00,0x00};
    DGUS_SERIAL.write(e_data,sizeof(e_data));
  
    if(holdAdd != 0)
    {
      Serial.printf("ViTriVaToaDo size: %d\n", vitrivatoado.size());
      for(int i = 0; i < vitrivatoado.size(); i++)
      {
        // if((long) (vitriX - vitrivatoado[i].vitriX) < (BuachieuDaiXChonToaDo / _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackX - 10) && 
        //   (long) (vitriY- vitrivatoado[i].vitriY) < (BuachieuDaiYChonToaDo / _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackY - 10))
        // if value of vitriX is between (vitrivatoado[i].vitriX - KichThuocHcnDaVeTheoX) and (vitrivatoado[i].vitriX + KichThuocHcnDaVeTheoX)
        // and value of vitriY is between (vitrivatoado[i].vitriY - KichThuocHcnDaVeTheoY) and (vitrivatoado[i].vitriY + KichThuocHcnDaVeTheoY)
        if((long) (vitriX - vitrivatoado[i].vitriX) < (BuachieuDaiXChonToaDo / _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackX - 10) && 
          (long) (vitriY- vitrivatoado[i].vitriY) < (BuachieuDaiYChonToaDo / _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackY - 10))
        {
          for (uint8_t m = 0; m < _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DsToaDo[IdxKhuVucDangXuLy].size(); m++) 
          {
            if (_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DsToaDo[IdxKhuVucDangXuLy][m].X == vitrivatoado[i].toadoX && _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DsToaDo[IdxKhuVucDangXuLy][m].Y == vitrivatoado[i].toadoY)
            {
              if(holdAdd == 1)
                return;
              if(holdAdd == 2)
              {
                HienThiMau(-1, vitrivatoado[i].toadoX, vitrivatoado[i].toadoY, _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy],MauTrang);
                if(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DsToaDo[IdxKhuVucDangXuLy].size() != 0)
                {
                  _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DsToaDo[IdxKhuVucDangXuLy].erase(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DsToaDo[IdxKhuVucDangXuLy].begin() + m);
                  Serial.printf("Xoa toa do chuong trinh: %s tai x: %d, y: %d", _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tenchuongtrinh.c_str(), vitrivatoado[i].toadoX, vitrivatoado[i].toadoY);
                  return;
                }
              }
            }
          }
          if(holdAdd == 1)
          {
            Serial.printf("X: %d | Y: %d\r\n",vitrivatoado[i].toadoX,vitrivatoado[i].toadoY);
            _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DsToaDo[IdxKhuVucDangXuLy].push_back({vitrivatoado[i].toadoX,vitrivatoado[i].toadoY});
            Serial.printf("Them toa do chuong trinh: %s tai x: %d, y: %d", _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tenchuongtrinh.c_str(), vitrivatoado[i].toadoX, vitrivatoado[i].toadoY);
            HienThiMau(-1, vitrivatoado[i].toadoX, vitrivatoado[i].toadoY, _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy],MauDo);
          }
        }
      }
    }

  }

  else if (codeHMI == _HMIBamNutEXITTrangPhanPhoiViTri)
  {

    // Khồi phục lại chương trình ban đầu
    _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy] = ChuongTrinhTamThoi;

    for (uint16_t i = 0; i < 12; i++)       // khôi phục lại DanhSach chọn của khaychon
    {
      for (uint16_t j = 0; j < 8; j++)
      {
        khaychon[i][j] = KhayChonTamThoi[i][j];
      }
    }
  }
  else if(codeHMI == _HMIBamNutOKTrangPhanPhoiViTri ||
    codeHMI  == _HMIBamNutEXITTrangPhanPhoiViTri ||
    codeHMI  == _HMIBamNutFULLTrangPhanPhoiViTri ||
    codeHMI  == _HMIBamNutCLEARTrangPhanPhoiViTri)
  {
    if (codeHMI == _HMIBamNutOKTrangPhanPhoiViTri)
    {
      HienThiThongTinPhanPhoi();
    }
    holdAdd = 0;
  }
  else if (codeHMI == _HMIBamNutThayDoiSoLuongOng)
  {
    setPage(_TrangBanPhimThayDoiSoLuongOng);
  }


}
//=============================================================================================================================

//=============================================================================================================================
void DWINHMI::XuLyCalib()
{
    /* Reset lại hiển thị */
  if(codeHMI == _HMIBamNutCalibTrangCaiDat)
  {
    if (IndexChuongTrinhDangXuLy < SoLuongChuongTrinhCoDinh)
    {
      IndexChuongTrinhDangXuLy = SoLuongChuongTrinhCoDinh;
    }
    ChuongTrinh_t ChuongTrinhDangCalib = _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy];
    countTest = 1;
    HienThiNoiDungChuongTrinh(IndexChuongTrinhDangXuLy, _VPTextHienThiChuongTrinhTrangChonChuongTrinh);
    GuiTextLenHMI(_VPTextChuongTrinhTrangCalib, ChuongTrinhDangCalib.tenchuongtrinh);
    GuiTextLenHMI(_VPTextFillNumber, "0");
    GuiTextLenHMI(_VPTextDensity, "1.0");
    GuiTextLenHMI(_VPTextActualWeight, "0.0");
    GuiTextLenHMI(_VPTextXacNhanSaveCalib,"");

    codeHMI = -1;
  }

  if(CheDoVaoTrangChonChuongTrinh == _VaoTrangTuNutSelectTrangCalib)
  {

    if(codeHMI == _HMIBamNutSelectTrangCalib)
    {
      // CapNhatHienThiThongTinTenChuongTrinhKhiVaoTrangChonChuongTrinh();
    }

    /* Hiển thị dữ liệu khi bấm chọn chương trình */
    else if (codeHMI >= _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh && codeHMI <= _HMIBamNutChonChuongTrinh10TrangChonChuongTrinh && lastcodeHMI != codeHMI)
    {
      CapNhatHienThiThongTinChuongTrinhTrangChonChuongTrinh();
    }

    /* Khi kết thúc việc chọn bấm OK */
    else if (codeHMI == _HMIBamNutOKTrangChonChuongTrinh)
    {      
      // nếu đổi chương trình khác thì reset lại các thông tin calib trước đó và hiện thông tin chương trình mới
      if (!((lastcodeHMI >= _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh) && (lastcodeHMI <= _HMIBamNutChonChuongTrinh10TrangChonChuongTrinh)))
      {
        Serial.println("Khong phai nut chon chuong trinh");
        codeHMI = -1;
        return;
      }
      IndexChuongTrinhDangXuLy = lastcodeHMI % _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh;
      countTest = 1;
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].actualweight = 0.0;

      String name = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tenchuongtrinh);
      GuiTextLenHMI(_VPTextChuongTrinhTrangCalib, name);
      Serial.println("Chuong trinh: " + name);
      String density = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].density);
      GuiTextLenHMI(_VPTextDensity, density);
      String actualweight = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].actualweight);
      GuiTextLenHMI(_VPTextActualWeight, actualweight);     

      GuiTextLenHMI(_VPTextFillNumber, "0");

    } 


    /* Xử lý save calib */
    static uint32_t _timeHienThiTrong2s;
    if(codeHMI == _HMIBamNutSaveTrangCalib && IndexChuongTrinhDangXuLy != -1)
    {
      if ((countTest == 1) || (_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].actualweight == 0))
      {
        GuiTextLenHMI(_VPTextXacNhanSaveCalib, "Please Filling first");
        codeHMI = -1;
      }
      else
      {
      GuiTextLenHMI(_VPTextXacNhanSaveCalib,"Waiting");
      _timeHienThiTrong2s = millis();
      }
    }
  
    if(millis() - _timeHienThiTrong2s > 2000)
    {
      if(DuLieuBoardGuiLenHMI.TrangThaiXacNhanSaveCalib == true)
      {
        DuLieuBoardGuiLenHMI.TrangThaiXacNhanSaveCalib = false;
        GuiTextLenHMI(_VPTextXacNhanSaveCalib,"OK");
        countTest = 1;
        GuiTextLenHMI(_VPTextFillNumber, "0");
        GuiTextLenHMI(_VPTextActualWeight, "0");

      }
    }
    if((codeHMI == _HMIBamNutTestTrangCalib) && (IndexChuongTrinhDangXuLy != -1))
    {
      if (countTest == 1)
      {
        GuiTextLenHMI(_VPTextXacNhanSaveCalib, "");
      }
      String data = (String) (countTest*_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].thetich*_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].density);
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].actualweight = data.toFloat();
      GuiTextLenHMI(_VPTextActualWeight,data);
      GuiTextLenHMI(_VPTextFillNumber,(String)countTest);
      countTest++;
    }
    else if(codeHMI == _HMIBamNutResetTrangCalib)
    {
      countTest = 1;
      GuiTextLenHMI(_VPTextFillNumber,"0");
      GuiTextLenHMI(_VPTextActualWeight,"0");
      codeHMI = -1;
    }  
  }
}
//=============================================================================================================================
void DWINHMI::XuLyBanPhim(void)
{
  static String ChuoiDangNhap = "";
  static String Gio,Phut,Giay;
  /* Cập nhật hiển thị khi chuyển trang*/
  if(codeHMI == _HMIBamNutMODIFYTrangThongSoChuongTrinh || codeHMI == _HMIBamNutBackTrangThietLapTheTichPhanPhoi)
  {
    CapNhatHienThiThongTinChuongTrinhTrangThietLapChuongTrinhDiChuyen();
  }
  /* Cập nhật hiển thị khi chuyển trang*/
  else if(codeHMI == _HMIBamNutNEXTTrangThietLapChuongTrinhDiChuyen || codeHMI == _HMIBamNutBACKTrangLayViTriChonDiem)
  {
    CapNhatHienThiThongTinChuongTrinhTrangThietLapTheTichPhanPhoi();
  }

  switch(codeHMI)
  {
    case -1:
      break;
    case _HMIBamNutHourTrangSecondLoop:
      codeHMI = -1;
      ChuoiDangNhap = "";
      CheDoCaiDatTimeLoop = _CaiDatGioLoop;
      GuiTextLenHMI(_VPTextHoursTrangCaiDatThoiGianLoop, ChuoiDangNhap);
      break;
    case _HMIBamNutMinutesTrangSecondLoop:
      codeHMI = -1;
      ChuoiDangNhap = "";
      CheDoCaiDatTimeLoop = _CaiDatPhutLoop;
      GuiTextLenHMI(_VPTextMinutesTrangCaiDatThoiGianLoop, ChuoiDangNhap);
      break;
    case _HMIBamNutSecondsTrangSecondLoop:
      codeHMI = -1;
      ChuoiDangNhap = "";
      CheDoCaiDatTimeLoop = _CaiDatGiayLoop;
      GuiTextLenHMI(_VPTextSecondTrangCaiDatThoiGianLoop, ChuoiDangNhap);
      break;
    case _HMIBamNutThayDoiSoLuongOng:
      codeHMI = -1;
      ChuoiDangNhap = "";
      CheDoVaoTrangBanPhim = _VaoTrangBanPhimThayDoiSoLuongOng;
      GuiTextLenHMI(_VpTextThayDoiSoLuongOng, ChuoiDangNhap);
      GuiTextLenHMI(_VpTextCanhBaoChuoiDangNhap, "");
      break;

    case _HMIBamNutSetHourTrangDatenTime:
    case _HMIBamNutSetMinuteTrangDatenTime:
    case _HMIBamNutSetDayTrangDatenTime:
    case _HMIBamNutSetMonthTrangDatenTime:
    case _HMIBamNutSetYearTrangDatenTime:
      ChuoiDangNhap = "";
      GuiTextLenHMI(_VPTextTimeTrangBanPhimDaynTime,"");
      break;
  }

  /* Reset lại biến ChuoiDangNhap*/
  if((codeHMI >= _HMIBamNutTUBEPERROWTrangThietLapChuongTrinhDiChuyen && codeHMI <= _HMIBamNutSPEEDXYAXISrangThietLapChuongTrinhDiChuyen)
  || (codeHMI >= _HMIBamNutVolumeTrangThietLapTheTichPhanPhoi && codeHMI <= _HMIBamNutAntiDropTrangThietLapTheTichPhanPhoi)
  || (codeHMI == _HMIBamTextActualWeight || codeHMI == _HMIBamTextDensity))
  {
    switch(codeHMI)
    {
      case _HMIBamNutTUBEPERROWTrangThietLapChuongTrinhDiChuyen:
        GuiTextLenHMI(_VPTextTubePerRow, "");
        break;
      case _HMIBamNutTUBEPERCOLUMNTrangThietLapChuongTrinhDiChuyen:
        GuiTextLenHMI(_VPTextTubePerColumn, "");
        break;
      case _HMIBamNutSPEEDXYAXISrangThietLapChuongTrinhDiChuyen:
        GuiTextLenHMI(_VPTextSpeedXYAxis, "");
        break;
      case _HMIBamNutPumpSpeedTrangThietLapTheTichPhanPhoi:
        GuiTextLenHMI(_VPTextPumpSpeedTrangCaiDatTheTichPhanPhoi, "");
        break;
      case _HMIBamNutVolumeTrangThietLapTheTichPhanPhoi:
        GuiTextLenHMI(_VPTextVolumeTrangCaiDatTheTichPhanPhoi, "");
        break;
      case _HMIBamNutAntiDropTrangThietLapTheTichPhanPhoi:
        GuiTextLenHMI(_VPTextAntiDropTrangCaiDatTheTichPhanPhoi, "");
        break;
      case _HMIBamNutTubingIDTrangThietLapTheTichPhanPhoi:
        GuiTextLenHMI(_VPTextTubingIDTrangCaiDatTheTichPhanPhoi, "");
        break;
      case _HMIBamTextActualWeight:
        GuiTextLenHMI(_VPTextActualWeight, "");
        break;
      case _HMIBamTextDensity:
        GuiTextLenHMI(_VPTextDensity, "");
        break;
    }
    GuiTextLenHMI(_VpTextCanhBaoChuoiDangNhap, "");
    ChuoiDangNhap = "";
  }
  else if (codeHMI == _HMIBamNutOKTrangChonChuongTrinh && (CheDoVaoTrangChonChuongTrinh == _VaoTrangTuNutSetupProgramTrangCaiDatCheDoChuongTrinh || CheDoVaoTrangChonChuongTrinh == _VaoTrangTuNutSelectTrangCalib))
  {
    ChuoiDangNhap = "";
  }
  /* Khi bấm nút trên bàn phím */
  else if( (codeHMI >= _HMIBamNutBanPhim0 && codeHMI <= _HMIBamNutBanPhim9) 
  || (codeHMI == _HMIBamNutBanPhimdelete && ChuoiDangNhap.length() > 0)
  || codeHMI == _HMIBamNutBanPhimCham)
  {

    if(codeHMI >= _HMIBamNutBanPhim0 && codeHMI <= _HMIBamNutBanPhim9) 
    {
      ChuoiDangNhap += (String) (codeHMI % _HMIBamNutBanPhim0);
    }
    
    if(codeHMI == _HMIBamNutBanPhimCham)
    {
      ChuoiDangNhap += '.';
      codeHMI = -1;
    }
      
    if(codeHMI == _HMIBamNutBanPhimdelete && ChuoiDangNhap.length() > 0)
      ChuoiDangNhap.remove(ChuoiDangNhap.length()-1);

    //byte IDPage = DWIN::getPage();
    switch (CheDoVaoTrangBanPhim)
    { 
      case _VaoTrangThietLapSoOngTrenMotHang:
        GuiTextLenHMI(_VPTextTubePerRow,ChuoiDangNhap);
        break;
      case _VaoTrangThietLapSoOngTrenMotCot:
        GuiTextLenHMI(_VPTextTubePerColumn,ChuoiDangNhap);
        break;
      case _VaoTrangThietLapTocDoTrucXY:
        GuiTextLenHMI(_VPTextSpeedXYAxis,ChuoiDangNhap);
        break;
      case _VaoTrangBanPhimVolume:
        GuiTextLenHMI(_VPTextVolumeTrangCaiDatTheTichPhanPhoi,ChuoiDangNhap);
        break;    
      case _VaoTrangBanPhimTubeID:
        GuiTextLenHMI(_VPTextTubingIDTrangCaiDatTheTichPhanPhoi,ChuoiDangNhap);
        break;
      case _VaoTrangBanPhimPumpSpeed:
        GuiTextLenHMI(_VPTextPumpSpeedTrangCaiDatTheTichPhanPhoi,ChuoiDangNhap);
        break;
      case _VaoTrangBanPhimAntiDrop:
        GuiTextLenHMI(_VPTextAntiDropTrangCaiDatTheTichPhanPhoi,ChuoiDangNhap); 
        break;
      case _VaoTrangBanPhimDensity:
        GuiTextLenHMI(_VPTextDensity,ChuoiDangNhap);
        break;
      case _VaoTrangBanPhimActualWeight:
        GuiTextLenHMI(_VPTextActualWeight,ChuoiDangNhap);
        break;
      case _VaoTrangBanPhimSecondLoop:
        switch(CheDoCaiDatTimeLoop)
        {
          case _CaiDatGioLoop:
            Gio = ChuoiDangNhap;
            GuiTextLenHMI(_VPTextHoursTrangCaiDatThoiGianLoop,ChuoiDangNhap);
            break;
          case _CaiDatPhutLoop:
            Phut = ChuoiDangNhap;
            GuiTextLenHMI(_VPTextMinutesTrangCaiDatThoiGianLoop,ChuoiDangNhap);
            break;
          case _CaiDatGiayLoop:
            Giay = ChuoiDangNhap;
            GuiTextLenHMI(_VPTextSecondTrangCaiDatThoiGianLoop,ChuoiDangNhap);
            break;
        }
        break;
      case _VaoTrangBanPhimCaiDatThoiGian:
        GuiTextLenHMI(_VPTextTimeTrangBanPhimDaynTime,ChuoiDangNhap);
        break;
      case _VaoTrangBanPhimMatKhau:
        GuiTextLenHMI(_VpTextMatKhau, ChuoiDangNhap);
        break;
      case _VaoTrangBanPhimThayDoiSoLuongOng:
        GuiTextLenHMI(_VpTextThayDoiSoLuongOng, ChuoiDangNhap);
        break;
    } 
  }
  else if(codeHMI == _HMIBamNutEnterTrangBanPhim)
  {
    if (ChuoiDangNhap.length() == 0)
    {
      codeHMI = -1;
      GuiTextLenHMI(_VpTextCanhBaoChuoiDangNhap, "Please enter the value");
      return;
    }
    //byte IDPage = DWIN::getPage();
    switch (CheDoVaoTrangBanPhim)
    {
      case _VaoTrangThietLapSoOngTrenMotCot:
        if ((ChuoiDangNhap.toInt() > RACK_Y_UPPER_LIMIT) || (ChuoiDangNhap.toInt() < RACK_Y_LOWER_LIMIT))
        {
          GuiTextLenHMI(_VpTextCanhBaoChuoiDangNhap, "ERROR: Quantity's range from " + String(RACK_Y_LOWER_LIMIT) + " to " + String(RACK_Y_UPPER_LIMIT));
          codeHMI = -1;
          return;
        }
        CanCapNhatThongSo = true;
        _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackY = ChuoiDangNhap.toInt();
        CapNhatLaiDanhSachToaDo(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy]); // reset lại danh sách tọa độ do số lượng ống thay đổi
        break;
      case _VaoTrangThietLapSoOngTrenMotHang:
        if ((ChuoiDangNhap.toInt() > RACK_X_UPPER_LIMIT) || (ChuoiDangNhap.toInt() < RACK_X_LOWER_LIMIT))
        {
          GuiTextLenHMI(_VpTextCanhBaoChuoiDangNhap, "ERROR: Quantity's range from " + String(RACK_X_LOWER_LIMIT) + " to " + String(RACK_X_UPPER_LIMIT));
          codeHMI = -1;
          return;
        }
        CanCapNhatThongSo = true;
        _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackX = ChuoiDangNhap.toInt();
        CapNhatLaiDanhSachToaDo(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy]);
        break;
      case _VaoTrangThietLapTocDoTrucXY:
        if (ChuoiDangNhap.toInt() > SPEED_XY_UPPER_LIMIT || ChuoiDangNhap.toInt() == SPEED_XY_LOWER_LIMIT)
        {
          GuiTextLenHMI(_VpTextCanhBaoChuoiDangNhap, "ERROR: XY Speed's range from " + String(SPEED_XY_LOWER_LIMIT) + " to " + String(SPEED_XY_UPPER_LIMIT));
          codeHMI = -1;
          return;
        }
        CanCapNhatThongSo = true;
        _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tocdoXY = ChuoiDangNhap.toInt();
        break;
      case _VaoTrangBanPhimVolume:
        if (ChuoiDangNhap.toFloat() == 0)
        {
          GuiTextLenHMI(_VpTextCanhBaoChuoiDangNhap, "ERROR: Volume must be greater than " + String(VOLUME_LOWER_LIMIT));
          codeHMI = -1;
          return;
        }
        CanCapNhatThongSo = true;
        _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].thetich = ChuoiDangNhap.toFloat();
        Serial.println(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].thetich);
        break;
      case _VaoTrangBanPhimTubeID:
        if (ChuoiDangNhap.toFloat() == TUBE_ID_LOWER_LIMIT)
        {
          GuiTextLenHMI(_VpTextCanhBaoChuoiDangNhap, "ERROR: Tube ID must be greater than " + String(TUBE_ID_LOWER_LIMIT));
          codeHMI = -1;
          return;
        }
        CanCapNhatThongSo = true;
        _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].duongkinhong = ChuoiDangNhap.toFloat();
        break;
      case _VaoTrangBanPhimPumpSpeed:
        if (ChuoiDangNhap.toInt() > SPEED_PUMP_UPPER_LIMIT)
        {
          GuiTextLenHMI(_VpTextCanhBaoChuoiDangNhap, "ERROR: Max Pump Speed " + String(SPEED_PUMP_UPPER_LIMIT));
          codeHMI = -1;
          return;
        }
        CanCapNhatThongSo = true;
        _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tocdobom = ChuoiDangNhap.toInt();
        break;
      case _VaoTrangBanPhimAntiDrop:
        CanCapNhatThongSo = true;
        _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].chongnhogiot = ChuoiDangNhap.toFloat();
        break;
      case _VaoTrangBanPhimDensity:
        if (ChuoiDangNhap.toFloat() == DENSITY_LOWER_LIMIT)
        {
          GuiTextLenHMI(_VpTextCanhBaoChuoiDangNhap, "ERROR: Density must be greater than " + String(DENSITY_LOWER_LIMIT));
          codeHMI = -1;
          return;
        }
        _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].density = ChuoiDangNhap.toFloat();
        break;
      case _VaoTrangBanPhimActualWeight:
        _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].actualweight = ChuoiDangNhap.toFloat();
        break;
      case _VaoTrangBanPhimSecondLoop:
          timeLoopDelay = (uint32_t) (Gio.toInt() * 60 * 60 * 1000 + Phut.toInt() * 60 * 1000 + Giay.toInt() * 1000);
          Serial.printf("Time loop delay %d\r\n",timeLoopDelay);
          GuiTextLenHMI(_VPTextSecondTrangMULTIPROGRAMSETUP,(String)(timeLoopDelay/1000));
        break;
      case _VaoTrangBanPhimCaiDatThoiGian:
        switch (CheDoCaiDatThoiGian)
        {
        case _CaiDatGio:
          if (ChuoiDangNhap.toInt() > 23)
          {
            GuiTextLenHMI(_VpTextCanhBaoChuoiDangNhap, "ERROR: Hours must be less than 24");
            codeHMI = -1;
            return;
          }
          DuLieuHMIGuiXuongBoard.RTC_Gio = ChuoiDangNhap;
          GuiTextLenHMI(_VPTextHourTrangDaynTime,ChuoiDangNhap); 
          break;
        case _CaiDatPhut:
          if (ChuoiDangNhap.toInt() > 59)
          {
            GuiTextLenHMI(_VpTextCanhBaoChuoiDangNhap, "ERROR: Minutes must be less than 60");
            codeHMI = -1;
            return;
          }
          DuLieuHMIGuiXuongBoard.RTC_Phut = ChuoiDangNhap;
          GuiTextLenHMI(_VPTextMinuteTrangDaynTime,ChuoiDangNhap);
          break;
        case _CaiDatNgay:
          if ((ChuoiDangNhap.toInt() > 31))
          {
            GuiTextLenHMI(_VpTextCanhBaoChuoiDangNhap, "ERROR: Days must be less than 32");
            codeHMI = -1;
            return;
          }
          DuLieuHMIGuiXuongBoard.RTC_Ngay = ChuoiDangNhap;
          GuiTextLenHMI(_VPTextDayTrangDaynTime,ChuoiDangNhap);
          break;
        case _CaiDatThang:
          if (ChuoiDangNhap.toInt() > 12)
          {
            GuiTextLenHMI(_VpTextCanhBaoChuoiDangNhap, "ERROR: Months must be less than 13");
            codeHMI = -1;
            return;
          }
          DuLieuHMIGuiXuongBoard.RTC_Thang = ChuoiDangNhap;
          GuiTextLenHMI(_VPTextMonthTrangDaynTime,ChuoiDangNhap);
          break;
        case _CaiDatNam:
          if ((ChuoiDangNhap.toInt() < 2000) && (ChuoiDangNhap.toInt() > 2099))
          {
            GuiTextLenHMI(_VpTextCanhBaoChuoiDangNhap, "ERROR: Years must be from 2000 to 2099");
            codeHMI = -1;
            return;
          }
          DuLieuHMIGuiXuongBoard.RTC_Nam = ChuoiDangNhap;
          GuiTextLenHMI(_VPTextYearTrangDaynTime,ChuoiDangNhap);
          break;
        }
        break;
        case _VaoTrangBanPhimThayDoiSoLuongOng:
          codeHMI = -1;
          uint8_t SoLuongOngThayDoiToiDa = _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackX * _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackY;
          if (ChuoiDangNhap.toInt() > SoLuongOngThayDoiToiDa)
          {
            GuiTextLenHMI(_VpTextCanhBaoChuoiDangNhap, "ERROR: Tubes must be <= " + String(SoLuongOngThayDoiToiDa));
            return;
          }
          SoLuongOngThayDoi = ChuoiDangNhap.toInt();
          auto & DsToaDoCanKiemTra = _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DsToaDo[IdxKhuVucDangXuLy];
          DsToaDoCanKiemTra.clear();
          for (uint8_t SoLuong = 1; SoLuong <= SoLuongOngThayDoi; SoLuong++)
          {
            uint8_t X = (SoLuong - 1) / _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackY;
            uint8_t Y = (SoLuong - 1) % _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackY;
            DsToaDoCanKiemTra.push_back({X, Y});
          }    
          ResetFullMauChonToaDo(MauDo);
          break;
    }
    switch (CheDoVaoTrangBanPhim)
    {
      case _VaoTrangThietLapSoOngTrenMotCot:
      case _VaoTrangThietLapSoOngTrenMotHang:
      case _VaoTrangThietLapTocDoTrucXY:
      {
        DWIN::setPage(_TrangThietLapChuongTrinhDiChuyen);
        break;
      }
      case _VaoTrangBanPhimVolume:
      case _VaoTrangBanPhimTubeID:
      case _VaoTrangBanPhimPumpSpeed:
      case _VaoTrangBanPhimAntiDrop:
      {
        DWIN::setPage(_TrangThietLapTheTichPhanPhoi);
        break;
      }
      case _VaoTrangBanPhimActualWeight:
      case _VaoTrangBanPhimDensity:
      {
        DWIN::setPage(_TrangCalib);
        break;
      }
      case _VaoTrangBanPhimSecondLoop:
      {
        if (DangOViTriHome() == true)
        {
          setPage(_TrangCaiDatCheDoChuongTrinh_HOME);
        }
        else
        {
          setPage(_TrangCaiDatCheDoChuongTrinh_notHOME);
        }
        break;
      }
      case _VaoTrangBanPhimCaiDatThoiGian:
      {
         DWIN::setPage(_TrangThoiGian);
        break;
      }
      case _VaoTrangBanPhimMatKhau:
        codeHMI = -1;
        if (ChuoiDangNhap == MatKhau)
        {
          CheDoVaoTrangBanPhim = _RESETBanPhim;
          switch (CheDoVaoTrangChonChuongTrinh)
          {
            case _VaoTrangTuNutSetupProgramTrangCaiDatCheDoChuongTrinh:
              setPage(_TrangThongSoChuongTrinh);
              break;

            case _VaoTrangTuNutSelectTrangCalib:
              setPage(_TrangCalib);
              break;
          }
        }
        else
        {
          GuiTextLenHMI(_VpTextCanhBaoChuoiDangNhap, "Wrong password");
        }
        break;
      case _VaoTrangBanPhimThayDoiSoLuongOng:
      {
        switch (holdAdd)
        {
        case 0:
          setPage(_TrangPhanPhoiViTri);
          break;
        
        case 1:
          setPage(_TrangPhanPhoiViTriADD);
          break;
        case 2:
          setPage(_TrangPhanPhoiViTriSub);
          break;
        }
      }
    }  
  }

  else if(codeHMI == _HMIBamNutReturnTrangBanPhim)
  {
    //byte IDPage = DWIN::getPage();

    switch (CheDoVaoTrangBanPhim)
    {
      case _VaoTrangThietLapSoOngTrenMotCot:
        GuiTextLenHMI(_VPTextTubePerColumn,(String)_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackY);
        break;
      case _VaoTrangThietLapSoOngTrenMotHang:
        GuiTextLenHMI(_VPTextTubePerRow,(String)_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackX);
        break;
      case _VaoTrangThietLapTocDoTrucXY:
        GuiTextLenHMI(_VPTextSpeedXYAxis,(String)_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tocdoXY);
        break;
      case _VaoTrangBanPhimVolume:
        GuiTextLenHMI(_VPTextVolumeTrangCaiDatTheTichPhanPhoi,(String)_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].thetich);
        break;
      case _VaoTrangBanPhimTubeID:
        GuiTextLenHMI(_VPTextTubingIDTrangCaiDatTheTichPhanPhoi,(String)_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].duongkinhong);
        break;
      case _VaoTrangBanPhimPumpSpeed:
        GuiTextLenHMI(_VPTextPumpSpeedTrangCaiDatTheTichPhanPhoi,(String)_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tocdobom);
        break;
      case _VaoTrangBanPhimAntiDrop:
        GuiTextLenHMI(_VPTextAntiDropTrangCaiDatTheTichPhanPhoi,(String)_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].chongnhogiot);
        break;
      case _VaoTrangBanPhimDensity:
        GuiTextLenHMI(_VPTextDensity,(String)_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].density);
        break;
      case _VaoTrangBanPhimActualWeight:
        GuiTextLenHMI(_VPTextActualWeight,(String)_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].actualweight);
        break;
      case _VaoTrangBanPhimSecondLoop:
      {
        GuiTextLenHMI(_VPTextSecondTrangMULTIPROGRAMSETUP,(String)(timeLoopDelay));
        break;
      }
      case _VaoTrangBanPhimThayDoiSoLuongOng:
        GuiTextLenHMI(_VpTextThayDoiSoLuongOng, String(SoLuongOngThayDoi));
        break;
    }

    switch (CheDoVaoTrangBanPhim)
    {
      case _VaoTrangThietLapSoOngTrenMotCot:
      case _VaoTrangThietLapSoOngTrenMotHang:
      case _VaoTrangThietLapTocDoTrucXY:
      {
        DWIN::setPage(_TrangThietLapChuongTrinhDiChuyen);
        break;
      }
      case _VaoTrangBanPhimVolume:
      case _VaoTrangBanPhimTubeID:
      case _VaoTrangBanPhimPumpSpeed:
      case _VaoTrangBanPhimAntiDrop:
      {
        DWIN::setPage(_TrangThietLapTheTichPhanPhoi);
        break;
      }
      case _VaoTrangBanPhimActualWeight:
      case _VaoTrangBanPhimDensity:
      {
        DWIN::setPage(_TrangCalib);
        break;
      }
      case _VaoTrangBanPhimSecondLoop:
      {
        if (DangOViTriHome() == true)
        {
          setPage(_TrangCaiDatCheDoChuongTrinh_HOME);
        }
        else
        {
          setPage(_TrangCaiDatCheDoChuongTrinh_notHOME);
        }
        break;
      }
      case _VaoTrangBanPhimCaiDatThoiGian:
      {
        DWIN::setPage(_TrangThoiGian);
        break;
      }
      case _VaoTrangBanPhimMatKhau:
        GuiTextLenHMI(_VpTextCanhBaoChuoiDangNhap, "");
        GuiTextLenHMI(_VPTextHienThiChuongTrinhTrangChonChuongTrinh, "");
        setPage(_TrangChonChuongTrinh);
        break;
      case _VaoTrangBanPhimThayDoiSoLuongOng:
      {
        switch (holdAdd)
        {
        case 0:
          setPage(_TrangPhanPhoiViTri);
          break;
        
        case 1:
          setPage(_TrangPhanPhoiViTriADD);
          break;
        case 2:
          setPage(_TrangPhanPhoiViTriSub);
          break;
        }
      }
    }  
  }
  else if(codeHMI == _HMIBamNutEnterTrangBanPhim || codeHMI == _HMIBamNutReturnTrangBanPhim)
  {
    GuiTextLenHMI(_VPTextTimeTrangBanPhimDaynTime,"");
    ChuoiDangNhap="";
  }

}
//=============================================================================================================================
void DWINHMI::XuLySetupProgram(void)
{
  if(CheDoVaoTrangChonChuongTrinh == _VaoTrangTuNutCopyTrangThongSoChuongTrinh)
  {
    static uint8_t lastprogram;
    if (codeHMI == _HMIBamNutCOPYTrangThongSoChuongTrinh)
    {
      // CapNhatHienThiThongTinChuongTrinhTrangChonChuongTrinh();
      lastprogram = IndexChuongTrinhDangXuLy;
    }
    else if (codeHMI >= _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh 
    && codeHMI <= _HMIBamNutChonChuongTrinh10TrangChonChuongTrinh && lastcodeHMI != codeHMI)
    {
      uint8_t IndexChuongTrinhVuaChon = codeHMI % _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh;
      ChuongTrinh_t chuongtrinhxuly = _ChuongTrinhEEPROM[IndexChuongTrinhVuaChon];
      String rackX = String(chuongtrinhxuly.rackX);
      String rackY = String(chuongtrinhxuly.rackY);
      String Volume = String(chuongtrinhxuly.thetich);
      String PumpSpeed = String(chuongtrinhxuly.tocdobom);
      String DKOng = String(chuongtrinhxuly.duongkinhong);
      String SpeedXY = String(chuongtrinhxuly.tocdoXY);
      String data = "Quantity: " + rackX + "x" + rackY + "\r\n" + "Volume: " + Volume + " ml\r\n" +
        "Pump speed : " + PumpSpeed + " rpm\r\n" + "Tube ID: " + DKOng + " mm\r\n" + "Speed XY: " + SpeedXY + " mm/min";
      GuiTextLenHMI(_VPTextHienThiChuongTrinhTrangChonChuongTrinh, data);
    }
    else if(codeHMI == _HMIBamNutOKTrangChonChuongTrinh)
    {
      if (!((lastcodeHMI >= _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh) && (lastcodeHMI <= _HMIBamNutChonChuongTrinh10TrangChonChuongTrinh)))
      {
        Serial.println("Khong phai nut chon chuong trinh");
        codeHMI = -1;
        return;
      }
      IndexChuongTrinhDangXuLy = lastcodeHMI % _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh;
      _ChuongTrinhEEPROM[lastprogram] = _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy];
      CapNhatHienThiThongTinChiTietChuongTrinh();
      CapNhatHienThiThongTinTenChuongTrinhKhiVaoTrangChonChuongTrinh();
    }
      
    else if(codeHMI == _HMIBamNutExitTrangChonChuongTrinh)
    {
      IndexChuongTrinhDangXuLy = lastprogram;
      CapNhatHienThiThongTinChiTietChuongTrinh();
    }
    /* Reset lai hien thi */
  }
  if(codeHMI == _HMIBamNutNEXTTrangThietLapChuongTrinhDiChuyen)
  {
    HienThiThongTinABCH();
  }
  else if(codeHMI == _HMIBamNutLOW)
  {
    GuiTextLenHMI(_VPTextGetLocationStep,"1.0");
    GuiTextLenHMI(_VPTextGetLocationFeed,"500");
    GuiTextLenHMI(_VPTextSPEEDXYZTrangLayViTri,"LOW");
  }
  else if(codeHMI == _HMIBamNutMEDIUM)
  {
    GuiTextLenHMI(_VPTextGetLocationStep,"10.0");
    GuiTextLenHMI(_VPTextGetLocationFeed,"2000");
    GuiTextLenHMI(_VPTextSPEEDXYZTrangLayViTri,"MEDIUM");
  }
  else if(codeHMI == _HMIBamNutFAST)
  {
    GuiTextLenHMI(_VPTextGetLocationStep,"30.0");
    GuiTextLenHMI(_VPTextGetLocationFeed,"5000");
    GuiTextLenHMI(_VPTextSPEEDXYZTrangLayViTri,"FAST");
  }

  if( codeHMI == _HMIBamNutGETATrangLayViTriChonDiem ||
    codeHMI == _HMIBamNutGETBTrangLayViTriChonDiem ||
    codeHMI == _HMIBamNutGETCTrangLayViTriChonDiem ||
    codeHMI == _HMIBamNutGETHTrangLayViTriChonDiem)
  {
    GuiTextLenHMI(_VPTextXacNhanGETTrangLayViTri, "");
    ManualControl = true;
  }

  switch(codeHMI)
  {
    case -1:
      break;
    case _HMIBamNutGetA1:
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriA[0].X = DuLieuBoardGuiLenHMI.ViTriHienTaiX;
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriA[0].Y = DuLieuBoardGuiLenHMI.ViTriHienTaiY;
      CanCapNhatThongSo = true;
      HienThiThongTinABCH();
      break;
    case _HMIBamNutGetA2:
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriA[1].X = DuLieuBoardGuiLenHMI.ViTriHienTaiX;
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriA[1].Y = DuLieuBoardGuiLenHMI.ViTriHienTaiY;
      CanCapNhatThongSo = true;
      HienThiThongTinABCH();
      break;
    case _HMIBamNutGetB1:
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriB[0].X = DuLieuBoardGuiLenHMI.ViTriHienTaiX;
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriB[0].Y = DuLieuBoardGuiLenHMI.ViTriHienTaiY;
      CanCapNhatThongSo = true;
      HienThiThongTinABCH();
      break;
    case _HMIBamNutGetB2:
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriB[1].X = DuLieuBoardGuiLenHMI.ViTriHienTaiX;
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriB[1].Y = DuLieuBoardGuiLenHMI.ViTriHienTaiY;
      CanCapNhatThongSo = true;
      HienThiThongTinABCH();
      break;
    case _HMIBamNutGetC1:
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriC[0].X = DuLieuBoardGuiLenHMI.ViTriHienTaiX;
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriC[0].Y = DuLieuBoardGuiLenHMI.ViTriHienTaiY;
      CanCapNhatThongSo = true;
      HienThiThongTinABCH();
      break;
    case _HMIBamNutGetC2:
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriC[1].X = DuLieuBoardGuiLenHMI.ViTriHienTaiX;
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriC[1].Y = DuLieuBoardGuiLenHMI.ViTriHienTaiY;
      CanCapNhatThongSo = true;
      HienThiThongTinABCH();
      break;
    case _HMIBamNutGetH1:
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriH[0] = DuLieuBoardGuiLenHMI.ViTriHienTaiZ;
      CanCapNhatThongSo = true;
      HienThiThongTinABCH();
      break;
    case _HMIBamNutGetH2:
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriH[1] = DuLieuBoardGuiLenHMI.ViTriHienTaiZ;
      CanCapNhatThongSo = true;
      HienThiThongTinABCH();
      break;
  }

}
//=============================================================================================================================
void DWINHMI::HienThiThongSoHienThiThoiGianThuc(void)
{

  if(codeHMI == _HMIBamNutRunTrangCaiDatCheDoChuongTrinh)
  { 
    if(!oneTimeMulti)
    {
      uint32_t seconds,minutes,hours;
      seconds = timeLoopDelay / 1000; // Tính số giây
      minutes = seconds / 60; // Tính số phút
      hours = minutes / 60; // Tính số giờ
      timeLoopDelay_c = twoDigits(hours % 24) + ":" + twoDigits(minutes % 60) + ":" + twoDigits(seconds % 60);
      GuiTextLenHMI(_VPTextDelayTimeRunTrangHienThiPhanPhoiThoiGianThuc, timeLoopDelay_c);
    }
    if( (oneTimeMulti == true) || (codeHMI == _HMIBamNutOKTrangPhanPhoiViTri))
    {
      GuiTextLenHMI(_VPTextDelayTimeRunTrangHienThiPhanPhoiThoiGianThuc, "x");
    }
      
    GuiTextLenHMI(_VPTextTenChuongTrinhDangChay,"1");
    GuiTextLenHMI(_VPTextFillNumberTrangHienThiPhanPhoiThoiGianThuc, "0");
    GuiTextLenHMI(_VPTextTimeRunTrangHienThiPhanPhoiThoiGianThuc, "00:00:00");
  }

  // /* Cập nhật lại các giá trị để sử dụng */
  if(codeHMI == _HMIBamNutRUNTrangHienThiPhanPhoiThoiGianThuc)
  {
    if (sys.state == State::Homing)
    {
      codeHMI = -1;
      return;
    }
    if (BamNutRunON == true)
    {
      DWIN::setPage(_TrangHienThiPhanPhoiThoiGianThuc_OFF_notHOME);
      vTaskSuspend(xTaskHandle_RunTime);  
    }
    else
    {
      DWIN::setPage(_TrangHienThiPhanPhoiThoiGianThucON);
      vTaskResume(xTaskHandle_RunTime);
    }
  }

 
  if (codeHMI == _HMIBamNutEXITTrangHienThiPhanPhoiThoiGianThuc && BamNutRunON != true)
  {
    TimeNow = 0;
    delay_flag = false;
  }
}


//=============================================================================================================================
void DWINHMI::ResetProgram(void)
{
    if (codeHMI == _HMIBamNutRESETPROGRAMTrangHienThiPhanPhoiThoiGianThuc && BamNutRunON == false)
    {
      if (sys.state == State::Homing)
      {
        codeHMI = -1;
        return;
      }
      DaChayXongCacKhuVuc_oneTimeMulti = false;
      for (uint8_t i = 0; i < sizeof(DSIndexChuongTrinhDuocChon); i++)
      {
        if (DSIndexChuongTrinhDuocChon[i] == -1)
        {
          continue;
        }
        IdxKhuVucDangXuLy = i;
        ResetFullMau(DSIndexChuongTrinhDuocChon[i], MauDo, i);
        // delay(20);
      }
      flushDWIN();
      GuiTextLenHMI(_VPTextTimeRunTrangHienThiPhanPhoiThoiGianThuc, "00:00:00");
      GuiTextLenHMI(_VPTextDelayTimeRunTrangHienThiPhanPhoiThoiGianThuc, "00:00:00");
      GuiTextLenHMI(_VPTextFillNumberTrangHienThiPhanPhoiThoiGianThuc,"0");
      TimeNow = 0;
    }

    else if (codeHMI == _HMIBamNutRUNTrangHienThiPhanPhoiThoiGianThuc && BamNutRunON == false && DaChayXongCacKhuVuc_oneTimeMulti == true)
    {
      DaChayXongCacKhuVuc_oneTimeMulti = false;
      for (uint8_t i = 0; i < sizeof(DSIndexChuongTrinhDuocChon); i++)
      {
        if (DSIndexChuongTrinhDuocChon[i] == -1)
        {
          continue;
        }
        IdxKhuVucDangXuLy = i;
        ResetFullMau(DSIndexChuongTrinhDuocChon[i], MauDo, i);
        // delay(20);
      }
      flushDWIN();
      GuiTextLenHMI(_VPTextTimeRunTrangHienThiPhanPhoiThoiGianThuc, "00:00:00");
      GuiTextLenHMI(_VPTextDelayTimeRunTrangHienThiPhanPhoiThoiGianThuc, "00:00:00");
      GuiTextLenHMI(_VPTextFillNumberTrangHienThiPhanPhoiThoiGianThuc,"0");
      TimeNow = 0;
    }
}
//=============================================================================================================================
DanhSachMaLenh_t DWINHMI::ReturnMaLenhGRBL()
{
  static bool Holding = false;

  if (codeHMI != lastcodeHMI)
  {
    switch (codeHMI)
    {
      case -1:
        break;
      //case _HMIBamNutExitTrangXemConfig:
      case _HMIBamNutGoHomeTrangDieuKhien:
      {
        codeHMI = -1;
        return GOHOMEKHONGCHUYENTRANG;
      }
      case _HMIBamNutGoHomeTrangChaySetHome:
      {
        codeHMI = -1;
        return GOHOME;
      }
      case _HMIBamNutRUNTrangHienThiPhanPhoiThoiGianThuc:
      {
        if (BamNutRunON == false)
        {
          BamNutRunON = true;
#ifdef DEBUG_DWIN
          Serial.print("DSIndexChuongTrinhDuocChon: ");
          for (uint8_t i : DSIndexChuongTrinhDuocChon)
          {
            Serial.printf("%d, ", i);
          }
          Serial.println();
#endif
          return RUN;
        }
        else
        {
          BamNutRunON = false;
          return PAUSE;
        }     
      }
      case _HMIBamNutRESETPROGRAMTrangHienThiPhanPhoiThoiGianThuc:
      {
        if (BamNutRunON == false) 
        {
          GuiTextLenHMI(_VPTextFillNumberTrangHienThiPhanPhoiThoiGianThuc, "0");
          return RESETPROGRAM;
        }
        break;
      }
      case _HMIBamNutEXITTrangHienThiPhanPhoiThoiGianThuc:
      {
        if (BamNutRunON == true)
         break;
        else
        {
         return EXITVETRANGCAIDATCHEDOCHUONGTRINH;
        }
      }

      case _HMIBamNutOKTrangPhanPhoiViTri:
      case _HMIBamNutRunTrangCaiDatCheDoChuongTrinh:
        return CAPNHATTOADO;

      case _HMIBamNutNEXTTrangLayViTriChonDiem:
        codeHMI = -1;
        if (CanCapNhatThongSo == true)
        {
          for (uint8_t KhuVuc = 0; KhuVuc < SoLuongKhuVucToiDa; KhuVuc++)
          {
            if (DSIndexChuongTrinhDuocChon[KhuVuc] == IndexChuongTrinhDangXuLy)
            {
              IdxKhuVucDangXuLy = KhuVuc;
              HienThiThongTinPhanPhoi();
            }
          }
          CanCapNhatThongSo = false;
          return CAPNHATTHONGSO;
        }
        else
        {
          return RESETMALENH;
        }

      case _HMIBamNutEnterTrangBanPhimMoRong:
        return CAPNHATTHONGSO;

      case _HMIBamNutOKTrangChonChuongTrinh:
        if(CheDoVaoTrangChonChuongTrinh == _VaoTrangTuNutCopyTrangThongSoChuongTrinh)
        {
          return CAPNHATTHONGSO;
        }
        break;
      case _HMIBamNutCOPYTrangThongSoChuongTrinh:
        codeHMI = -1;
        break;
      case _HMIBamNutXTru:
        return BAMNUTDICHUYENXTRU;
      case _HMIBamNutYCong:
        return BAMNUTDICHUYENYCONG;
      case _HMIBamNutXCong:
        codeHMI = -1;
        return BAMNUTDICHUYENXCONG;
      case _HMIBamNutYTru:
        return BAMNUTDICHUYENYTRU;
      case _HMIBamNutZCong:
        return BAMNUTDICHUYENZCONG;
      case _HMIBamNutZTru:
        return BAMNUTDICHUYENZTRU;
      case _HMIBamNutGET:
        codeHMI = 0;
        return XACNHAN_A_B_C_H;
      case _HMIBamNutTestTrangCalib:
        codeHMI = -1;
        return TESTCALIB;
      case _HMIBamNutSaveTrangCalib:
        codeHMI = -1;
        return CALIB;
      case _HMIBamNutTestFillTrangXemConfig:
        codeHMI = -1;
        return TESTFILL;
      case _HMIBamNutDirectionTrangXemConfig:
        codeHMI = -1;
        return DIRECTION;
      case _HMIBamNutFastPumpTrangXemConfig:
        codeHMI = -1;
        return FASTPUMP;
      case _HMIBamNutGoA1TrangXemConfig:
        codeHMI = -1;
        return GOA1;
      case _HMIBamNutGoB1TrangXemConfig:
        codeHMI = -1;
        return GOB1;
      case _HMIBamNutGoC1TrangXemConfig:
        codeHMI = -1;
        return GOC1;
      case _HMIBamNutGoA2TrangXemConfig:
        codeHMI = -1;
        return GOA2;
      case _HMIBamNutGoB2TrangXemConfig:
        codeHMI = -1;
        return GOB2; 
      case _HMIBamNutGoC2TrangXemConfig:
        codeHMI = -1;
        return GOC2;
      case _HMIBamNutLOW:
        codeHMI = -1;
        return LOWSPEED;
      case _HMIBamNutMEDIUM:
        codeHMI = -1;
        return MEDIUMSPEED;
      case _HMIBamNutFAST:
        codeHMI = -1;
        return FASTSPEED;

      case _HMIBamNutShutdownTrangCaiDat:
        codeHMI = -1;
        return SHUTDOWN;
    }
  }
  if ((ManualControl == true))
  {
    DanhSachMaLenh_t MaLenhNhanDuoc = RESETMALENH;
    switch (MaLenhManualControl)
    {
      case BAMNUTDICHUYENXCONG:
      case BAMNUTDICHUYENXTRU:
      case BAMNUTDICHUYENYCONG:      
      case BAMNUTDICHUYENYTRU:
      case BAMNUTDICHUYENZCONG:
      case BAMNUTDICHUYENZTRU:
        MaLenhNhanDuoc = MaLenhManualControl;
        break;
    }
    if (MaLenhNhanDuoc != RESETMALENH)
    {
      MaLenhManualControl = RESETMALENH;
      return MaLenhNhanDuoc;
    }
    switch (MaLenhManualControl)
    {
      case HOLDXCONG:
      case HOLDXTRU:
      case HOLDYCONG:
      case HOLDYTRU:
      case HOLDZCONG:
      case HOLDZTRU:
        if (!Holding)
        {
          // Serial.println("Start Holding");
          // ESP_LOGI("Holding", "Start Holding");
          MaLenhNhanDuoc = MaLenhManualControl;
          Holding = true;
          HoldingTime = millis();
        }
        break;
    }

    if ((millis() - HoldingTime >= 450UL) && (Holding))
    {
      HoldingTime = millis();
      if ((MaLenhManualControl >= HOLDXTRU) && (MaLenhManualControl <= HOLDZCONG))
      {
        // ESP_LOGI("Holding", "On Holding");
        // Serial.println("On Holding");
        MaLenhManualControl = RESETMALENH;
      }
      else if ((MaLenhManualControl == RESETMALENH) || (MaLenhManualControl == DUNGDICHUYEN))
      {
        // ESP_LOGI("Holding", "Stop Holding");
        // Serial.println("Stop Holding");
        Holding = false;
        MaLenhNhanDuoc = DUNGDICHUYEN;
      }
    }
    return MaLenhNhanDuoc;
  }

return RESETMALENH;
}
//=============================================================================================================================
void DWINHMI::ResetCode() 
{
  if (codeHMI == -1){}
  else if(codeHMI == _HMIBamNutBackTrangCaiDat)
    codeHMI = -1;
  else if(codeHMI == _HMIBamNutConfigTrangCaiDatCheDoChuongTrinh)
    codeHMI = -1;
  else if(codeHMI == _HMIBamNutEXITTrangManualControl)
    codeHMI = -1;
  else if(codeHMI == _HMIBamNutManualControlTrangXemConfig)
    codeHMI = -1;
  else if(codeHMI == _HMIBamNutEnterTrangBanPhimMoRong)
    codeHMI = -1;
  else if(codeHMI == _HMIBamNutTimeTrangCaiDat)
    codeHMI = -1;
  else if(codeHMI == _HMIBamNutGoHomeTrangChaySetHome)
   codeHMI = -1;
  else if (codeHMI >= _HMIBamNutMODIFY1TrangThietLapCheDoChayMulti && codeHMI <= _HMIBamNutMODIFY5TrangThietLapCheDoChayMulti)
    codeHMI = -1;
  else if (codeHMI >= _HMIBamNutProgram1TrangCaiDatCheDoChuongTrinh && codeHMI <= _HMIBamNutProgram5TrangCaiDatCheDoChuongTrinh) 
    codeHMI = -1;
  else if ((codeHMI >= _HMIBamNutChonProgram1TrangCaiDatCheDoChuongTrinh && codeHMI <= _HMIBamNutChonProgram10TrangCaiDatCheDoChuongTrinh))
    codeHMI = -1;
  else if (codeHMI == _HMIBamNutOKTrangChonChuongTrinh)
    codeHMI = -1;
  else if (codeHMI == _HMIBamNutEXITTrangPhanPhoiViTri)
    codeHMI = -1;
  else if(codeHMI >= _HMIBamNutGETATrangLayViTriChonDiem && codeHMI <= _HMIBamNutGETCTrangLayViTriChonDiem)
    codeHMI = -1;
  else if(codeHMI == _HMIBamNutRunTrangCaiDatCheDoChuongTrinh)
    codeHMI = -1;
  else if(codeHMI == _HMIBamNutEXITTrangHienThiPhanPhoiThoiGianThuc)
    codeHMI = -1;
  else if(codeHMI == _HMIBamNutRESETPROGRAMTrangHienThiPhanPhoiThoiGianThuc)
    codeHMI = -1;
  else if(codeHMI == _HMIBamNutRUNTrangHienThiPhanPhoiThoiGianThuc)
    codeHMI = -1;
  else if(codeHMI >= _HMIBamNutTUBEPERROWTrangThietLapChuongTrinhDiChuyen && codeHMI <= _HMIBamNutSPEEDXYAXISrangThietLapChuongTrinhDiChuyen )
    codeHMI = -1;
  else if(codeHMI == _HMIBamNutMODIFYTrangThongSoChuongTrinh)
    codeHMI = -1;
  else if(codeHMI == _HMIBamNutNEXTTrangThietLapChuongTrinhDiChuyen)
    codeHMI = -1;
  else if(codeHMI == _HMIBamNutBACKTrangLayViTriChonDiem)
    codeHMI = -1;
  else if (codeHMI == _HMIBamNutNextTrangThietLapTheTichPhanPhoi)
    codeHMI = -1;
  else if (codeHMI == _HMIBamTextActualWeight)
    codeHMI = -1;
  else if(codeHMI == _HMIBamTextDensity)
    codeHMI = -1;
  else if(codeHMI >= _HMIBamNutBanPhim0 && codeHMI <= _HMIBamNutBanPhim9)
    codeHMI = -1;
  else if(codeHMI == _HMIBamNutBanPhimdelete)
    codeHMI = -1;
  else if(codeHMI == _HMIBamNutModeTrangCaiDatCheDoChuongTrinh)
    codeHMI = -1;    
  else if(codeHMI == _HMIBamNutRunTrangCaiDatCheDoChuongTrinh)
    codeHMI = -1;
  else if(codeHMI == _HMIBamNutEXITTrangThietLapCheDoChayMulti)
    codeHMI = -1;
  else if(codeHMI == _HMIBamNutOKTrangPhanPhoiViTri)
    codeHMI = -1;
  else if(codeHMI >= _HMIBamNutXTru && codeHMI <= _HMIBamNutZTru)
    codeHMI = -1;
  else if(codeHMI >= _HMIBamNutLOW && codeHMI <= _HMIBamNutBackTrangLayViTriXacNhan)
    codeHMI = -1;
  else if(codeHMI >= _HMIBamNutXTru && codeHMI <= _HMIBamNutZTru)
    codeHMI = -1;
  else if (codeHMI == _HMIBamNutADDTrangPhanPhoiViTri)
    codeHMI = -1;    
  else if (codeHMI == _HMIBamNutFULLTrangPhanPhoiViTri)
    codeHMI = -1;
  else if (codeHMI == _HMIBamNutCLEARTrangPhanPhoiViTri)
    codeHMI = -1;
  else if(codeHMI == _HMIBamNutSelectTrangCalib)
    codeHMI = -1;
  else if(codeHMI >= _HMIBamNutVolumeTrangThietLapTheTichPhanPhoi && codeHMI <= _HMIBamNutBackTrangThietLapTheTichPhanPhoi)
    codeHMI = -1;
  else if(codeHMI == _HMIBamNutEnterTrangBanPhim || codeHMI == _HMIBamNutReturnTrangBanPhim)
    codeHMI = -1;
  else if(codeHMI >= _HMIBamNutSetHourTrangDatenTime && codeHMI <= _HMIBamNutSetYearTrangDatenTime)
    codeHMI = -1;
  else if(codeHMI == _HMIBamVaoVungChonToaDo)
     codeHMI = -1;
  else if (codeHMI == _HMIBamNutEDITTrangThongSoChuongTrinh)
    codeHMI = -1;
  else if (codeHMI == _HMIBamNutGetA1 || codeHMI == _HMIBamNutGetA2 
        || codeHMI == _HMIBamNutGetB1 || codeHMI == _HMIBamNutGetB2
        || codeHMI == _HMIBamNutGetC1 || codeHMI == _HMIBamNutGetC2
        || codeHMI == _HMIBamNutGetH1 || codeHMI == _HMIBamNutGetH2 )
    codeHMI = -1;
  lastcodeHMI = codeHMI;
  
  if (millis() - _time500ms >= 500)
  {
    _time500ms = millis();
  }

}
//=============================================================================================================================
void DWINHMI::XuLyXemConfig()
{
  static uint32_t fillnumber = 0; 
  static bool controlABC = false;
  if(codeHMI == _HMIBamNutConfigTrangCaiDatCheDoChuongTrinh)
  {
    GuiTextLenHMI(_VPTextGetLocationX, String(DuLieuBoardGuiLenHMI.ViTriHienTaiX));
    GuiTextLenHMI(_VPTextGetLocationY, String(-DuLieuBoardGuiLenHMI.ViTriHienTaiY));
    GuiTextLenHMI(_VPTextGetLocationZ, String(-DuLieuBoardGuiLenHMI.ViTriHienTaiZ));
    GuiTextLenHMI(_VPTextTenChuongTrinhTrangXemConfig, _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tenchuongtrinh);
    GuiTextLenHMI(_VPTextTotalFillTubeTrangXemConfig,"0");
    GuiTextLenHMI(_VpTextPumpControlTrangXemConfig, String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].thetich) + "ml");
  }
  else if(codeHMI == _HMIBamNutEXITTrangManualControl || codeHMI == _HMIBamNutBackTrangLayViTriXacNhan)
  {
    ManualControl = false;
    if (sys.state == State::Alarm)
    {
      //TODO
      // WebUI::inputBuffer.write(0x18);
      // delay(100);
      // WebUI::inputBuffer.push("$X\n");
      // delay(100);
      mc_reset();
      sys.state = State::Idle;
    }
  }
  else if(
    codeHMI == _HMIBamNutManualControlTrangXemConfig ||
    codeHMI == _HMIBamNutGETATrangLayViTriChonDiem ||
    codeHMI == _HMIBamNutGETBTrangLayViTriChonDiem ||
    codeHMI == _HMIBamNutGETCTrangLayViTriChonDiem ||
    codeHMI == _HMIBamNutGETHTrangLayViTriChonDiem ||
    codeHMI == _HMIBamNutConfigTrangCaiDatCheDoChuongTrinh)
  {
    controlABC = true;
  } 
  if(controlABC == true)
  {
    if (millis() - _time500ms >= 500)
    {
      GuiTextLenHMI(_VPTextGetLocationX,(String)DuLieuBoardGuiLenHMI.ViTriHienTaiX);
      GuiTextLenHMI(_VPTextGetLocationY,(String)(-DuLieuBoardGuiLenHMI.ViTriHienTaiY));
      GuiTextLenHMI(_VPTextGetLocationZ,(String)(-DuLieuBoardGuiLenHMI.ViTriHienTaiZ));
    } 
  }
  if(codeHMI == _HMIBamNutExitTrangXemConfig ||
    codeHMI == _HMIBamNutBackTrangLayViTriXacNhan ||
    codeHMI == _HMIBamNutNEXTTrangLayViTriChonDiem ||
    codeHMI == _HMIBamNutBACKTrangLayViTriChonDiem)
  {
    controlABC = false;
  }

  else if(codeHMI == _HMIBamNutTestFillTrangXemConfig)
  {
    Serial.println("Test fill");
    fillnumber++;
    GuiTextLenHMI(_VPTextTotalFillTubeTrangXemConfig,(String)fillnumber);
  }
  else if(codeHMI == _HMIBamNutDirectionTrangXemConfig)
  {
    static bool RevertDirection = false;
    RevertDirection = !RevertDirection;
    if (RevertDirection == true)
      setVP(_VPIconDIRECTION, 1);
      // GuiIconLenHMI(_VPIconDIRECTION, 0);
    else
    {
      setVP(_VPIconDIRECTION, 0);
      // GuiIconLenHMI(_VPIconDIRECTION, 1);
    }
    // codeHMI = -1;
  }
  else if(codeHMI == _HMIBamNutModifyProgramTrangXemConfig)
  {
    CapNhatHienThiThongTinChiTietChuongTrinh();
    codeHMI = -1;
  }
  if(CheDoVaoTrangChonChuongTrinh == _VaoTrangTuNutSelectTrangXemConfig)
  {

    if(codeHMI == _HMIBamNutSelectTrangXemConifg)
    {
      // CapNhatHienThiThongTinTenChuongTrinhKhiVaoTrangChonChuongTrinh();
      codeHMI = -1;
    }
    /* Hiển thị dữ liệu khi bấm chọn chương trình */
    else if (codeHMI >= _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh && codeHMI <= _HMIBamNutChonChuongTrinh10TrangChonChuongTrinh && lastcodeHMI != codeHMI)
    {
      CapNhatHienThiThongTinChuongTrinhTrangChonChuongTrinh();
    }
    else if(codeHMI == _HMIBamNutOKTrangChonChuongTrinh)
    {
      if (!(lastcodeHMI >= _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh && lastcodeHMI <= _HMIBamNutChonChuongTrinh10TrangChonChuongTrinh))
      {
        codeHMI = -1;
        return;
      }
      IndexChuongTrinhDangXuLy = lastcodeHMI % _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh;
      GuiTextLenHMI(_VPTextTenChuongTrinhTrangXemConfig,_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tenchuongtrinh);
      //GuiTextLenHMI(_VPTextTotalFillTubeTrangXemConfig,(String)_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DanhSachToaDo.size());
      GuiTextLenHMI(_VPTextTotalFillTubeTrangXemConfig,"0");
      GuiTextLenHMI(_VpTextPumpControlTrangXemConfig,(String)_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].thetich);
    }
  }
  if(codeHMI == _HMIBamNutResetTrangXemconfig)
  {
    // _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DsToaDo[IdxKhuVucDangXuLy].clear();
    fillnumber = 0;
    GuiTextLenHMI(_VPTextTotalFillTubeTrangXemConfig,(String)fillnumber);
    codeHMI = -1;
  }
}
//=============================================================================================================================
void DWINHMI::GuiTextLenHMI(uint16_t VPAddress, String text) 
{
  uint8_t len = text.length() + 5;
  DGUS_SERIAL.write(0x5A);
  DGUS_SERIAL.write(0xA5);
  DGUS_SERIAL.write(len);
  DGUS_SERIAL.write(0x82);
  DGUS_SERIAL.write(VPAddress >> 8);
  DGUS_SERIAL.write(VPAddress);
  DGUS_SERIAL.print(text);
  DGUS_SERIAL.write(0xff);
  DGUS_SERIAL.write(0xff);
  // delay(5);
  // DGUS_SERIAL.println("OK");
}
//=============================================================================================================================
void DWINHMI::GuiIconLenHMI(uint16_t VPAddress, uint8_t data) 
{
  unsigned char s_data[8] = { 0x5a, 0xa5, 0x05, 0x82, VPAddress >> 8, VPAddress, 0x00, 0x00 };
  s_data[6] = highByte(data);
  s_data[7] = lowByte(data);
  DGUS_SERIAL.write(s_data, 8);
  // delay(5);
  // DGUS_SERIAL.println("OK");
}
//=============================================================================================================================
void DWINHMI::Debug(vector<ChuongTrinh_t> data)
{
  ESP_LOGI("DULIEUHMI","Kich thuoc chuong trinh: %d",data.size()); 
  for(int i = 0; i < data.size(); i++)
  {
    ESP_LOGI("DULIEUHMI","Ten chuong trinh: %s",data[i].tenchuongtrinh.c_str());
    ESP_LOGI("DULIEUHMI","So ong phan phoi: %d",data[i].DsToaDo[IdxKhuVucDangXuLy].size());  
  }
}
//=============================================================================================================================
void DWINHMI::CapNhatHienThiThongTinChiTietChuongTrinh()
{
    ChuongTrinh_t chuongtrinhxuly = _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy];
    String program = String(chuongtrinhxuly.tenchuongtrinh);
    GuiTextLenHMI(_VPTextTenChuongTrinhTrangThietLapChuongTrinh, program);
    String Volume = String(chuongtrinhxuly.thetich);
    String DKOng = String(chuongtrinhxuly.duongkinhong);
    String PumpSpeed = String(chuongtrinhxuly.tocdobom);
    String Antidrop = String(chuongtrinhxuly.chongnhogiot);
    String rackX = String(chuongtrinhxuly.rackX);
    String rackY = String(chuongtrinhxuly.rackY);
    String SpeedXY = String(chuongtrinhxuly.tocdoXY);

    String PositionAx1 = String(chuongtrinhxuly.ViTriA[0].X);
    String PositionAy1 = String(-chuongtrinhxuly.ViTriA[0].Y);
    String PositionBx1 = String(chuongtrinhxuly.ViTriB[0].X);
    String PositionBy1 = String(-chuongtrinhxuly.ViTriB[0].Y);
    String PositionCx1 = String(chuongtrinhxuly.ViTriC[0].X);
    String PositionCy1 = String(-chuongtrinhxuly.ViTriC[0].Y);
    String heightH1 = String(-chuongtrinhxuly.ViTriH[0]);

    String PositionAx2 = String(chuongtrinhxuly.ViTriA[1].X);
    String PositionAy2 = String(-chuongtrinhxuly.ViTriA[1].Y);
    String PositionBx2 = String(chuongtrinhxuly.ViTriB[1].X);
    String PositionBy2 = String(-chuongtrinhxuly.ViTriB[1].Y);
    String PositionCx2 = String(chuongtrinhxuly.ViTriC[1].X);
    String PositionCy2 = String(-chuongtrinhxuly.ViTriC[1].Y);
    String heightH2 = String(-chuongtrinhxuly.ViTriH[1]);

    String data = "Volume: " + Volume + "ml\r\n" + 
                  "TubingID: " + DKOng + " mm\r\n" +
                  "PumpSpeed: " + PumpSpeed + " rpm\r\n" + 
                  "Antidrop: " + Antidrop + " deg\r\n" +
                  "Rack " + rackX + "x" + rackY + " tube\r\n" //+
      // "Position A1: " + "X=" + PositionAx1 + " Y=" + PositionAy1 + "\r\n" +
      // "Position B1: " + "X=" + PositionBx1 + " Y=" + PositionBy1 + "\r\n" +
      // "Position C1: " + "X=" + PositionCx1 + " Y=" + PositionCy1 + "\r\n" +
      // "Postion A2: " + "X=" + PositionAx2 + " Y=" + PositionAy2 + "\r\n" +
      // "Position B2: " + "X=" + PositionBx2 + " Y=" + PositionBy2 + "\r\n" +
      // "Position C2: " + "X=" + PositionCx2 + " Y=" + PositionCy2 + "\r\n" +
      // "Height H1:" + heightH1 + ", Height H2:" + heightH2 + "\r\n"
      // "Speed run: " + SpeedXY + " mm/min"
      ;
    GuiTextLenHMI(_VPTextThongSoTrangThietLapChuongTrinh, data);
}
//=============================================================================================================================
void DWINHMI::CapNhatHienThiThongTinChuongTrinhTrangChonChuongTrinh()
{
    int8_t IdxChuongTrinhVuaChon = codeHMI % _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh;
    String name = String(_ChuongTrinhEEPROM[IdxChuongTrinhVuaChon].tenchuongtrinh);
    String rackX = String(_ChuongTrinhEEPROM[IdxChuongTrinhVuaChon].rackX);
    String rackY = String(_ChuongTrinhEEPROM[IdxChuongTrinhVuaChon].rackY);
    String Volume = String(_ChuongTrinhEEPROM[IdxChuongTrinhVuaChon].thetich);
    String PumpSpeed = String(_ChuongTrinhEEPROM[IdxChuongTrinhVuaChon].tocdobom);
    String DKOng = String(_ChuongTrinhEEPROM[IdxChuongTrinhVuaChon].duongkinhong);
    String SpeedXY = String(_ChuongTrinhEEPROM[IdxChuongTrinhVuaChon].tocdoXY);
    String data = "Name: " + name + "\r\n" + "Quantity: " + rackX + "x" + rackY + "\r\n" + "Volume: " + Volume + " ml\r\n" +
      "Pump speed : " + PumpSpeed + " rpm\r\n" + "Tube ID: " + DKOng + " mm\r\n" + "SpeedXY: " + SpeedXY + " mm/min";
    GuiTextLenHMI(_VPTextHienThiChuongTrinhTrangChonChuongTrinh, data);

}
void DWINHMI::CapNhatHienThiThongTinTenChuongTrinhKhiVaoTrangChonChuongTrinh()
{
  for (uint8_t i = 0; i < SoLuongChuongTrinhToiDa; i++) 
  {
    String TenChuongTrinh = _ChuongTrinhEEPROM[i % _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh].tenchuongtrinh;

    GuiTextLenHMI(_VPTextProgram1TrangChonChuongTrinh + (i % _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh) * 0x100, TenChuongTrinh);
  }

  // GuiTextLenHMI(_VPTextHienThiChuongTrinhTrangChonChuongTrinh, "");
}
void DWINHMI::CapNhatHienThiThongTinChuongTrinhTrangThietLapTheTichPhanPhoi()
{
  
  GuiTextLenHMI(_VPTextVolumeTrangCaiDatTheTichPhanPhoi,(String)_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].thetich);
  GuiTextLenHMI(_VPTextTubingIDTrangCaiDatTheTichPhanPhoi,(String)_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].duongkinhong);
  GuiTextLenHMI(_VPTextPumpSpeedTrangCaiDatTheTichPhanPhoi,(String)_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tocdobom);
  GuiTextLenHMI(_VPTextAntiDropTrangCaiDatTheTichPhanPhoi,(String)_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].chongnhogiot);  
}
void DWINHMI::CapNhatHienThiThongTinChuongTrinhTrangThietLapChuongTrinhDiChuyen()
{
  GuiTextLenHMI(_VPTextTubePerRow,(String)_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackX);
  GuiTextLenHMI(_VPTextTubePerColumn,(String)_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackY);
  GuiTextLenHMI(_VPTextSpeedXYAxis,(String)_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tocdoXY);
}

void DWINHMI::ThayDoiTenChuongTrinh()
{
  if(CheDoVaoTrangBanPhimMoRong == _VaoTrangBanPhimMoRongTuNutEDITTrangThongSoChuongTrinh)
  {
    static String stringdangxuly ="";

    /* Reset lại giá trị hiển thị*/
    if(codeHMI == _HMIBamNutEDITTrangThongSoChuongTrinh)
    {
      stringdangxuly ="";
      GuiTextLenHMI(_VPTextDataTrangBanPhim,stringdangxuly);
      GuiTextLenHMI(_VpTextCanhBaoChuoiDangNhap, "");
    }

    else if(codeHMI == _HMIBamNutChuyenTrangSoTrangBanPhim)
    {
      if(DWIN::getPage() == _TrangBanPhimMoRong)
        DWIN::setPage(_TrangBanPhimMoRongSo);
      else
        DWIN::setPage(_TrangBanPhimMoRong);
      codeHMI = -1;
    }
    static bool CapsLock = false;
    if( (codeHMI & 0xFF00) == 0xFF00 && codeHMI != -1)
    {
      if (codeHMI == 0xFF80)
      {
        CapsLock = !CapsLock;
        codeHMI = -1;
        return;
      }
      if(codeHMI == 0xFF7F)
        stringdangxuly.remove(stringdangxuly.length()-1);
      else
      {
        if (codeHMI >= 0xFF41 && codeHMI <= 0xFF5A)
        {
          if (CapsLock == true)
          {
            stringdangxuly += (char)(codeHMI);
          }
          else
          {
            stringdangxuly += (char)(codeHMI + 32);
          }
        }
        else
          stringdangxuly += (char)codeHMI;
      }
      GuiTextLenHMI(_VPTextDataTrangBanPhim,stringdangxuly);
      codeHMI = -1;
    }
    if( codeHMI == _HMIBamNutEnterTrangBanPhimMoRong)
    {
      if ((stringdangxuly.length() > _ChieuDaiMaxTenChuongTrinh) || (stringdangxuly.length() == 0))
      {
        GuiTextLenHMI(_VpTextCanhBaoChuoiDangNhap, "ERROR: Valid length 1 to " + String(_ChieuDaiMaxTenChuongTrinh) + " characters");
        codeHMI = -1;
        return;
      }
      DWIN::setPage(_TrangThongSoChuongTrinh);
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tenchuongtrinh = stringdangxuly;
      GuiTextLenHMI(_VPTextTenChuongTrinhTrangThietLapChuongTrinh,stringdangxuly);
      CapNhatHienThiThongTinTenChuongTrinhKhiVaoTrangChonChuongTrinh();
      for (uint8_t KhuVuc = 0; KhuVuc < SoLuongKhuVucToiDa; KhuVuc++)
      {
        if (DSIndexChuongTrinhDuocChon[KhuVuc] == IndexChuongTrinhDangXuLy)
        {
          GuiTextLenHMI(_VPTextTenProgramArea1 + 0x100 * KhuVuc, stringdangxuly);
        }
      }

    }
    else if(codeHMI == _HMIBamNutReturnTrangBanPhimMoRong)
    {
      DWIN::setPage(_TrangThongSoChuongTrinh);
      codeHMI = -1;
    }
  }
}

void DWINHMI ::HienThiMau(uint32_t VpAddress, uint16_t X, uint16_t Y, uint16_t Mau, ChuongTrinh_t ChuongTrinh){
  uint8_t data[240]; /* 24 * 10 */
  uint16_t KhayDuocChon[12][8];
  uint16_t toadoXIcondautien;
  uint16_t toadoYIcondautien;
  uint16_t chieudaiX = ChieuDaiX;
  uint16_t chieudaiY = ChieuDaiY;
  #ifdef DEBUG_DWIN
  // Serial.println("vvvvvvvvvvvvvvvvvvvvvvv");
  #endif
  #pragma region KhayDuocChon
  if(VpAddress >= _VpAddressVung1KhayChonHienThiMau && VpAddress <= _VpAddressVung4KhayChonHienThiMau)
  {
    chieudaiX = BuachieuDaiXChonToaDo; // Bùa
    chieudaiY = BuachieuDaiYChonToaDo; // Bùa

    toadoXIcondautien = ToaDoXIconHinhTronDauTienKhayChon;
    toadoYIcondautien = ToaDoYIconHinhTronDauTienKhayChon;

    khaychon[X][Y] = Mau;
    for (uint8_t i = 0; i < 12; i++){
      for (uint8_t j = 0; j < 8; j++){
        KhayDuocChon[i][j] = khaychon[i][j];
      }
    }
  }
  else if(VpAddress >= _VpAddressVung1Khay1HienThiMau && VpAddress <= _VpAddressVung4Khay1HienThiMau)
  {
    toadoXIcondautien = ToaDoXIconHinhTronDauTienKhay1;
    toadoYIcondautien = ToaDoYIconHinhTronDauTienKhay1;

    khay1[X][Y] = Mau;
    for (uint8_t i = 0; i < 12; i++){
      for (uint8_t j = 0; j < 8; j++){
        KhayDuocChon[i][j] = khay1[i][j];
      }
    }
  }
  else if(VpAddress >= _VpAddressVung1Khay2HienThiMau && VpAddress <= _VpAddressVung4Khay2HienThiMau)
  {
    toadoXIcondautien = ToaDoXIconHinhTronDauTienKhay2;
    toadoYIcondautien = ToaDoYIconHinhTronDauTienKhay2;
    khay2[X][Y] = Mau;
    for (uint8_t i = 0; i < 12; i++){
      for (uint8_t j = 0; j < 8; j++){
        KhayDuocChon[i][j] = khay2[i][j];
      }
    }
  }
  // else if(VpAddress >= _VpAddressVung1Khay3HienThiMau && VpAddress <= _VpAddressVung4Khay3HienThiMau)
  // {
  //   toadoXIcondautien = ToaDoX0Khay(2) + 20;
  //   toadoYIcondautien = ToaDoY0Khay + 20;
  //   khay3[X][Y] = Mau;
  //   for (uint8_t i = 0; i < 12; i++){
  //     for (uint8_t j = 0; j < 8; j++){
  //       KhayDuocChon[i][j] = khay3[i][j];
  //     }
  //   }
  // }
  // else if(VpAddress >= _VpAddressVung1Khay4HienThiMau && VpAddress <= _VpAddressVung4Khay4HienThiMau)
  // {
  //   toadoXIcondautien = ToaDoX0Khay(3) + 20;
  //   toadoYIcondautien = ToaDoY0Khay + 20;
  //   khay4[X][Y] = Mau;
  //   for (uint8_t i = 0; i < 12; i++){
  //     for (uint8_t j = 0; j < 8; j++){
  //       KhayDuocChon[i][j] = khay4[i][j];
  //     }
  //   }
  // }

  uint8_t OffsetToaDo;

  switch(VpAddress){
    case _VpAddressVung1KhayChonHienThiMau:
    case _VpAddressVung1Khay1HienThiMau:
    case _VpAddressVung1Khay2HienThiMau:
    case _VpAddressVung1Khay3HienThiMau:
    case _VpAddressVung1Khay4HienThiMau:
    case _VpAddressVung1Khay5HienThiMau:
      OffsetToaDo = 0;
      break;
    case _VpAddressVung2KhayChonHienThiMau:
    case _VpAddressVung2Khay1HienThiMau:
    case _VpAddressVung2Khay2HienThiMau:
    case _VpAddressVung2Khay3HienThiMau:
    case _VpAddressVung2Khay4HienThiMau:
    case _VpAddressVung2Khay5HienThiMau:
      OffsetToaDo = 2;
      break;
    case _VpAddressVung3KhayChonHienThiMau:
    case _VpAddressVung3Khay1HienThiMau:
    case _VpAddressVung3Khay2HienThiMau:
    case _VpAddressVung3Khay3HienThiMau:
    case _VpAddressVung3Khay4HienThiMau:
    case _VpAddressVung3Khay5HienThiMau:
      OffsetToaDo = 4;
      break;
    case _VpAddressVung4KhayChonHienThiMau:
    case _VpAddressVung4Khay1HienThiMau:
    case _VpAddressVung4Khay2HienThiMau:
    case _VpAddressVung4Khay3HienThiMau:
    case _VpAddressVung4Khay4HienThiMau:
    case _VpAddressVung4Khay5HienThiMau:
      OffsetToaDo = 6;
      break;
  }

  memset(data, 0, sizeof(data));

  uint16_t KhoangCachGiua2DiemTheoX = chieudaiX / (ChuongTrinh.rackX);

  uint16_t KhoangCachGiua2DiemTheoY = chieudaiY / (ChuongTrinh.rackY);

  #pragma endregion KhayDuocChon
  for (uint8_t i = 0; i < 240; i+=10)
  {
    
    uint8_t ToaDoXMaTran = ((i / 10) % 12);
    uint8_t ToaDoYMaTran = ((i / 10) / 12) + OffsetToaDo;

    uint16_t ToaDoXHMI = 0;
    uint16_t ToaDoYHMI = 0;

   if (ChuongTrinh.rackX == 1)
    {
      ToaDoXHMI = toadoXIcondautien + chieudaiX / 2.5;
      KhoangCachGiua2DiemTheoX = (chieudaiX / (12 - ChuongTrinh.rackX)) * 2;
    }
    else
    {
      ToaDoXHMI = toadoXIcondautien + KhoangCachGiua2DiemTheoX * ToaDoXMaTran;
    }

    if (ChuongTrinh.rackY == 1)
    {
      ToaDoYHMI = toadoYIcondautien + chieudaiY / 2.5;
      KhoangCachGiua2DiemTheoY = (chieudaiY / (12 - ChuongTrinh.rackY)) * 2;
    }
    else
    {
      ToaDoYHMI = toadoYIcondautien + KhoangCachGiua2DiemTheoY * ToaDoYMaTran;
    }
    
    bool include = false;
    for (uint8_t k = 0; k < ChuongTrinh.DsToaDo[IdxKhuVucDangXuLy].size(); k++)
    {
      if (ChuongTrinh.DsToaDo[IdxKhuVucDangXuLy][k].X == ToaDoXMaTran && ChuongTrinh.DsToaDo[IdxKhuVucDangXuLy][k].Y == ToaDoYMaTran){
        include = true;
        break;
      }
    }

    if (!include){
      if (ToaDoXMaTran < ChuongTrinh.rackX && ToaDoYMaTran < ChuongTrinh.rackY){
        KhayDuocChon[ToaDoXMaTran][ToaDoYMaTran] = MauTrang;
        include = true;
      }
    }

    if (include){

      uint16_t ToaDoXHMIDuoiPhai = ToaDoXHMI + KhoangCachGiua2DiemTheoX - 10;

      uint16_t ToaDoYHMIDuoiPhai = ToaDoYHMI + KhoangCachGiua2DiemTheoY - 10;

      data[i    ] = ToaDoXHMI >> 8;
      data[i + 1] = ToaDoXHMI & 0xFF;

      data[i + 2] = ToaDoYHMI >> 8;
      data[i + 3] = ToaDoYHMI & 0xFF;

      data[i + 4] = ToaDoXHMIDuoiPhai >> 8;
      data[i + 5] = ToaDoXHMIDuoiPhai & 0xFF;

      data[i + 6] = ToaDoYHMIDuoiPhai >> 8;
      data[i + 7] = ToaDoYHMIDuoiPhai & 0xFF;

      data[i + 8] = KhayDuocChon[ToaDoXMaTran][ToaDoYMaTran] >> 8;
      data[i + 9] = KhayDuocChon[ToaDoXMaTran][ToaDoYMaTran] & 0xFF;
    }
  }
  uint8_t buffer[252] = {0x5A, 0xA5, 249, 0x82, VpAddress >> 8, VpAddress,0x00,0x04,0x00,0x18};
  memcpy(buffer + 10, data, 240);
  uint8_t endBuffer[] = {0xFF, 0x00};
  memcpy(buffer + 250, endBuffer, 2);
  DGUS_SERIAL.write(buffer, 252);
  #ifdef DEBUG_DWIN
  // for (uint8_t i = 0; i < 156; i++){
  //   Serial.printf("%02X ", buffer[i]);
  // }
  // Serial.println();
  #endif
}
void DWINHMI::ResetFullMauChonToaDo(uint16_t Mau) 
{
  ChuongTrinh_t ChuongTrinhDuocChon = _ChuongTrinhEEPROM[DSIndexChuongTrinhDuocChon[IdxKhuVucDangXuLy]];
  for (uint8_t i = 0; i < 12; i++){
    for (uint8_t j = 0; j < 8; j++){
      khaychon[i][j] = Mau;
    }
  }
  HienThiMau(_VpAddressVung1KhayChonHienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
  HienThiMau(_VpAddressVung2KhayChonHienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
  HienThiMau(_VpAddressVung3KhayChonHienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
  HienThiMau(_VpAddressVung4KhayChonHienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
}
void DWINHMI::ResetFullMau(uint8_t IdxChuongTrinhDuocChon, uint16_t Mau, uint8_t SoKhay){
  ChuongTrinh_t ChuongTrinhDuocChon = _ChuongTrinhEEPROM[IdxChuongTrinhDuocChon];
  switch(SoKhay){
    case 0:
      for (uint8_t i = 0; i < 12; i++){
        for (uint8_t j = 0; j < 8; j++){
          khay1[i][j] = Mau;
        }
      }
      HienThiMau(_VpAddressVung1Khay1HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
      HienThiMau(_VpAddressVung2Khay1HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
      HienThiMau(_VpAddressVung3Khay1HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
      HienThiMau(_VpAddressVung4Khay1HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
      break;
    case 1:
      for (uint8_t i = 0; i < 12; i++){
        for (uint8_t j = 0; j < 8; j++){
          khay2[i][j] = Mau;
        }
      }
      HienThiMau(_VpAddressVung1Khay2HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
      HienThiMau(_VpAddressVung2Khay2HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
      HienThiMau(_VpAddressVung3Khay2HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
      HienThiMau(_VpAddressVung4Khay2HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
      break;
    // case 2:
    //   for (uint8_t i = 0; i < 12; i++){
    //     for (uint8_t j = 0; j < 8; j++){
    //       khay3[i][j] = Mau;
    //     }
    //   }
    //   HienThiMau(_VpAddressVung1Khay3HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
    //   HienThiMau(_VpAddressVung2Khay3HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
    //   HienThiMau(_VpAddressVung3Khay3HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
    //   HienThiMau(_VpAddressVung4Khay3HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
    //   break;
    // case 3:
    //   for (uint8_t i = 0; i < 12; i++){
    //     for (uint8_t j = 0; j < 8; j++){
    //       khay4[i][j] = Mau;
    //     }
    //   }
    //   HienThiMau(_VpAddressVung1Khay4HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
    //   HienThiMau(_VpAddressVung2Khay4HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
    //   HienThiMau(_VpAddressVung3Khay4HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
    //   HienThiMau(_VpAddressVung4Khay4HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
    //   break;
    // default:
    //   break;
  }

}

void DWINHMI::VeToanBoHinhTron(uint32_t VpAddress, ChuongTrinh_t ChuongTrinh)
{
  uint16_t toadoXIcondautien;
  uint16_t toadoYIcondautien;

  uint16_t chieudaiX;
  uint16_t chieudaiY;

  if(VpAddress >= _VpAddressVung1KhayChon && VpAddress <= _VpAddressVung4KhayChon)
  {
    chieudaiX = BuachieuDaiXChonToaDo; // Bùa
    chieudaiY = BuachieuDaiYChonToaDo; // Bùa
    toadoXIcondautien = ToaDoXIconHinhTronDauTienKhayChon;
    toadoYIcondautien = ToaDoYIconHinhTronDauTienKhayChon;

  }

  uint8_t OffsetToaDo;

  switch(VpAddress){
    
    case _VpAddressVung1KhayChon:
      OffsetToaDo = 0;
      break;
    case _VpAddressVung2KhayChon:
      OffsetToaDo = 2;
      break;
    case _VpAddressVung3KhayChon:
      OffsetToaDo = 4;
      break;
    case _VpAddressVung4KhayChon:
      OffsetToaDo = 6;
      break;
  }

  uint16_t KhoangCachGiua2DiemTheoX = chieudaiX / (ChuongTrinh.rackX);

  uint16_t KhoangCachGiua2DiemTheoY = chieudaiY / (ChuongTrinh.rackY);

  uint8_t data[240]; /* 24 * 10 */
  memset(data, 0, 240);
  Serial.printf("Size vitrivatoado trước khi push_back: %d\n",vitrivatoado.size());
  Serial.printf("ChuongTrinh: %s, rackX: %d, rackY: %d, size DSTD: %d\n",ChuongTrinh.tenchuongtrinh,ChuongTrinh.rackX,ChuongTrinh.rackY,ChuongTrinh.DsToaDo[IdxKhuVucDangXuLy].size());
  for (uint16_t i = 0; i < 240; i+=10)
  {

    uint8_t ToaDoXMaTran = ((i / 10) % 12);
    uint8_t ToaDoYMaTran = ((i / 10) / 12) + OffsetToaDo;

    uint16_t ToaDoXHMI = 0;
    uint16_t ToaDoYHMI = 0;

    if (ChuongTrinh.rackX == 1)
    {
      ToaDoXHMI = toadoXIcondautien + chieudaiX / 2.5;
      KhoangCachGiua2DiemTheoX = (chieudaiX / (12 - ChuongTrinh.rackX)) * 2;
    }
    else
    {
      ToaDoXHMI = toadoXIcondautien + KhoangCachGiua2DiemTheoX * ToaDoXMaTran;
    }

    if (ChuongTrinh.rackY == 1)
    {
      ToaDoYHMI = toadoYIcondautien + chieudaiY / 2.5;
      KhoangCachGiua2DiemTheoY = (chieudaiY / (12 - ChuongTrinh.rackY)) * 2;
    }
    else
    {
      ToaDoYHMI = toadoYIcondautien + KhoangCachGiua2DiemTheoY * ToaDoYMaTran;
    }

    bool include = false;
    // Serial.printf("ChuongTrinh: %s, Size: %d\n",ChuongTrinh.tenchuongtrinh,ChuongTrinh.DanhSachToaDo.size());
    if ((ToaDoXMaTran < ChuongTrinh.rackX) && (ToaDoYMaTran < ChuongTrinh.rackY))
    {
      include = true;
      vitrivatoado.push_back({ToaDoXHMI, ToaDoYHMI, ToaDoXMaTran, ToaDoYMaTran});
    }
    if(include)
    {
      #ifdef DEBUG_DWIN
      // Serial.println("^^^^^^^^^^^^^^^^^^");
      #endif

      uint16_t ToaDoXHMIDuoiPhai = ToaDoXHMI + KhoangCachGiua2DiemTheoX - 10;

      uint16_t ToaDoYHMIDuoiPhai = ToaDoYHMI + KhoangCachGiua2DiemTheoY - 10;

      data[i    ] = ToaDoXHMI >> 8;
      data[i + 1] = ToaDoXHMI & 0xFF;

      data[i + 2] = ToaDoYHMI >> 8;
      data[i + 3] = ToaDoYHMI & 0xFF;

      data[i + 4] = ToaDoXHMIDuoiPhai >> 8;
      data[i + 5] = ToaDoXHMIDuoiPhai & 0xFF;

      data[i + 6] = ToaDoYHMIDuoiPhai >> 8;
      data[i + 7] = ToaDoYHMIDuoiPhai & 0xFF;

      data[i + 8] = MauDo >> 8;
      data[i + 9] = MauDo & 0xFF;
    }
  }

  uint8_t buffer[252] = {0x5A, 0xA5, 249, 0x82, VpAddress >> 8, VpAddress,0x00,0x03,0x00,0x18};
  memcpy(buffer + 10, data, 240);
  uint8_t endBuffer[] = {0xFF, 0x00};
  memcpy(buffer + 250, endBuffer, 2);
  DGUS_SERIAL.write(buffer, 252);

  #ifdef DEBUG_DWIN
  // for (uint8_t i = 0; i < 252; i++){
  //   // Serial.printf("%02X ", buffer[i]);
  // }
  // Serial.println();
  #endif
}

void DWINHMI::XuLyThoiGianThuc()
{

  if(codeHMI == _HMIBamNutTimeTrangCaiDat)
  { // 01/02/2002
    // 0123456789

    //12:30
    //01234
    DuLieuHMIGuiXuongBoard.RTC_Ngay = DuLieuBoardGuiLenHMI.RTC_ChuoiNgay.substring(0,2);
    DuLieuHMIGuiXuongBoard.RTC_Thang = DuLieuBoardGuiLenHMI.RTC_ChuoiNgay.substring(3,5);
    DuLieuHMIGuiXuongBoard.RTC_Nam = DuLieuBoardGuiLenHMI.RTC_ChuoiNgay.substring(6);

    DuLieuHMIGuiXuongBoard.RTC_Gio = DuLieuBoardGuiLenHMI.RTC_ChuoiGio.substring(0,2);
    DuLieuHMIGuiXuongBoard.RTC_Phut = DuLieuBoardGuiLenHMI.RTC_ChuoiGio.substring(3);

  }
  
  static uint32_t time50s = millis();
  if(millis()-time50s > 50000)
  {
    GuiTextLenHMI(_VPTextNgay,DuLieuBoardGuiLenHMI.RTC_ChuoiNgay);
    GuiTextLenHMI(_VPTextGio,DuLieuBoardGuiLenHMI.RTC_ChuoiGio);
    time50s = millis();
  }
}
void DWINHMI::CapNhatHienThiDuLieuThoiGianTuDS3231()
{
  if(DuLieuBoardGuiLenHMI.RTC_ChuoiGio.length() == 5 && DuLieuBoardGuiLenHMI.RTC_ChuoiNgay.length() == 10)
  {
    DuLieuHMIGuiXuongBoard.RTC_Gio = DuLieuBoardGuiLenHMI.RTC_ChuoiGio.substring(0,2);
    DuLieuHMIGuiXuongBoard.RTC_Phut = DuLieuBoardGuiLenHMI.RTC_ChuoiGio.substring(3,5);
    DuLieuHMIGuiXuongBoard.RTC_Ngay = DuLieuBoardGuiLenHMI.RTC_ChuoiNgay.substring(0,2);
    DuLieuHMIGuiXuongBoard.RTC_Thang = DuLieuBoardGuiLenHMI.RTC_ChuoiNgay.substring(3,5);
    DuLieuHMIGuiXuongBoard.RTC_Nam = DuLieuBoardGuiLenHMI.RTC_ChuoiNgay.substring(6,10);

    GuiTextLenHMI(_VPTextHourTrangDaynTime,DuLieuHMIGuiXuongBoard.RTC_Gio);
    GuiTextLenHMI(_VPTextMinuteTrangDaynTime,DuLieuHMIGuiXuongBoard.RTC_Phut);
    GuiTextLenHMI(_VPTextDayTrangDaynTime, DuLieuHMIGuiXuongBoard.RTC_Ngay);
    GuiTextLenHMI(_VPTextMonthTrangDaynTime,DuLieuHMIGuiXuongBoard.RTC_Thang);
    GuiTextLenHMI(_VPTextYearTrangDaynTime,DuLieuHMIGuiXuongBoard.RTC_Nam);
  }
}
//=============================================================================================================================
String twoDigits(int number) {
  if (number < 10) {
    return "0" + String(number);
  }
  return String(number);
}
void DWINHMI::ChuyenDoiThanhChuoiThoiGian()
{
  // Chuyển đổi thời gian thành giờ, phút, giây
  uint32_t seconds = TimeNow / 1000; // Tính số giây
  uint32_t minutes = seconds / 60; // Tính số phút
  uint32_t hours = minutes / 60; // Tính số giờ

  // Tạo chuỗi thời gian
  TimeRunStart_c = twoDigits(hours % 24) + ":" + twoDigits(minutes % 60) + ":" + twoDigits(seconds % 60);

  seconds = timeLoopDelay / 1000; // Tính số giây
  minutes = seconds / 60; // Tính số phút
  hours = minutes / 60; // Tính số giờ

  // Tạo chuỗi thời gian
  timeLoopDelay_c = String(hours % 24) + ":" + twoDigits(minutes % 60) + ":" + twoDigits(seconds % 60);
}
//=============================================================================================================================


void DWINHMI::StaticTaskFunction_RunTime(void* _this)
{
  ((DWINHMI*)_this)->TaskFunction_RunTime();
}

void DWINHMI::TaskFunction_RunTime(void)
{
  uint32_t time1s = millis();
  int32_t timeloopdelay;
  uint32_t seconds,minutes,hours;
  while (1)
  {

    if (millis() - time1s >= 1000)
    {
      if(delay_flag == true)
      {
        timeloopdelay-=1000;
        seconds = timeloopdelay / 1000; // Tính số giây
        minutes = seconds / 60; // Tính số phút
        hours = minutes / 60; // Tính số giờ

        // Tạo chuỗi thời gian
        timeLoopDelay_c = twoDigits(hours % 24) + ":" + twoDigits(minutes % 60) + ":" + twoDigits(seconds % 60);
        GuiTextLenHMI(_VPTextDelayTimeRunTrangHienThiPhanPhoiThoiGianThuc, String(timeLoopDelay_c));
      }
      else if (delay_flag == false)
      {
        TimeNow += 1000;  
        // Chuyển đổi thời gian thành giờ, phút, giây
        seconds = TimeNow / 1000; // Tính số giây
        minutes = seconds / 60; // Tính số phút
        hours = minutes / 60; // Tính số giờ

        // Tạo chuỗi thời gian
        TimeRunStart_c = twoDigits(hours % 24) + ":" + twoDigits(minutes % 60) + ":" + twoDigits(seconds % 60);
        GuiTextLenHMI(_VPTextTimeRunTrangHienThiPhanPhoiThoiGianThuc, String(TimeRunStart_c));
        
      }
      GuiTextLenHMI(_VPTextGetLocationX,(String)DuLieuBoardGuiLenHMI.ViTriHienTaiX);
      GuiTextLenHMI(_VPTextGetLocationY,(String)(-DuLieuBoardGuiLenHMI.ViTriHienTaiY));
      GuiTextLenHMI(_VPTextGetLocationZ,(String)(-DuLieuBoardGuiLenHMI.ViTriHienTaiZ));
      time1s = millis();
      flushDWIN();
    }

    if (DuLieuBoardGuiLenHMI.DaChayXongMotOng == true && delay_flag == false)
    {
      DuLieuBoardGuiLenHMI.DaChayXongMotOng = false;
      // Serial.printf("KhuVuc: %d, X: %d, Y: %d\n",DuLieuBoardGuiLenHMI.KhuVucDangChay,DuLieuBoardGuiLenHMI.ToaDoXDaChay,DuLieuBoardGuiLenHMI.ToaDoYDaChay);
      IdxKhuVucDangXuLy = DuLieuBoardGuiLenHMI.KhuVucDangChay;
      GuiTextLenHMI(_VPTextFillNumberTrangHienThiPhanPhoiThoiGianThuc, String(DuLieuBoardGuiLenHMI.OngDaPhanPhoi + 1));
      HienThiMau(DuLieuBoardGuiLenHMI.KhuVucDangChay, DuLieuBoardGuiLenHMI.ToaDoXDaChay, DuLieuBoardGuiLenHMI.ToaDoYDaChay, _ChuongTrinhEEPROM[DSIndexChuongTrinhDuocChon[DuLieuBoardGuiLenHMI.KhuVucDangChay]], MauXanhLa);
      flushDWIN();
    }

      if (DuLieuBoardGuiLenHMI.DaChayXongCacKhuVuc == true && delay_flag == false)
      {
        DuLieuBoardGuiLenHMI.DaChayXongCacKhuVuc = false;
        if (oneTimeMulti == false)
        {
          delay_flag = true;
          timeloopdelay = timeLoopDelay;
        }
        else
        {
          DaChayXongCacKhuVuc_oneTimeMulti = true;
          setPage(_TrangHienThiPhanPhoiThoiGianThuc_OFF_HOME);
          BamNutRunON = false;
          vTaskSuspend(xTaskHandle_RunTime);
        }
      }

    if (delay_flag)
    {
      if (timeloopdelay <= 0)
      {
        seconds = (timeLoopDelay) / 1000; // Tính số giây
        minutes = seconds / 60; // Tính số phút
        hours = minutes / 60; // Tính số giờ
        // Tạo chuỗi thời gian
        timeLoopDelay_c = twoDigits(hours % 24) + ":" + twoDigits(minutes % 60) + ":" + twoDigits(seconds % 60);
        GuiTextLenHMI(_VPTextDelayTimeRunTrangHienThiPhanPhoiThoiGianThuc, String(timeLoopDelay_c));

        delay_flag = false;
        
        GuiTextLenHMI(_VPTextTenChuongTrinhDangChay, "1");

        for (uint8_t i = 0; i < sizeof(DSIndexChuongTrinhDuocChon); i++)
        {
          if (DSIndexChuongTrinhDuocChon[i] == -1)
          {
            continue;
          }
          IdxKhuVucDangXuLy = i;
          ResetFullMau(DSIndexChuongTrinhDuocChon[i], MauDo, i);
        }
        flushDWIN();
        codeHMI = _HMIBamNutRUNTrangHienThiPhanPhoiThoiGianThuc;
        BamNutRunON = false;
      }
    }
    delay(1);
  }
}

void DWINHMI::HienThiMau(int8_t SoKhay, uint16_t X, uint16_t Y, ChuongTrinh_t ChuongTrinh,uint16_t Mau){
  switch(SoKhay){
    case -1:
      if (Y < 2)
        HienThiMau(_VpAddressVung1KhayChonHienThiMau, X, Y, Mau, ChuongTrinh);
      else if (Y < 4)
        HienThiMau(_VpAddressVung2KhayChonHienThiMau, X, Y, Mau, ChuongTrinh);
      else if (Y < 6) 
        HienThiMau(_VpAddressVung3KhayChonHienThiMau, X, Y, Mau, ChuongTrinh);
      else if (Y < 8) 
        HienThiMau(_VpAddressVung4KhayChonHienThiMau, X, Y, Mau, ChuongTrinh);
      break;    

    case 0:
      if (Y < 2)
      {
        HienThiMau(_VpAddressVung1Khay1HienThiMau, X, Y, Mau, ChuongTrinh);
      }
      else if (Y < 4)
      {
        HienThiMau(_VpAddressVung2Khay1HienThiMau, X, Y, Mau, ChuongTrinh);
      }
      else if (Y < 6)
      {
        HienThiMau(_VpAddressVung3Khay1HienThiMau, X, Y, Mau, ChuongTrinh);
      }
      else if (Y < 8)
      {
        HienThiMau(_VpAddressVung4Khay1HienThiMau, X, Y, Mau, ChuongTrinh);
      }
      break;
    case 1:
      if (Y < 2)
      {
        HienThiMau(_VpAddressVung1Khay2HienThiMau, X, Y, Mau, ChuongTrinh);
      }
      else if (Y < 4)
      {
        HienThiMau(_VpAddressVung2Khay2HienThiMau, X, Y, Mau, ChuongTrinh);
      }
      else if (Y < 6)
      {
        HienThiMau(_VpAddressVung3Khay2HienThiMau, X, Y, Mau, ChuongTrinh);
      }
      else if (Y < 8)
      {
        HienThiMau(_VpAddressVung4Khay2HienThiMau, X, Y, Mau, ChuongTrinh);
      }
      break;
    case 2:
      HienThiMau(_VpAddressVung1Khay3HienThiMau, X, Y, Mau, ChuongTrinh);
      HienThiMau(_VpAddressVung2Khay3HienThiMau, X, Y, Mau, ChuongTrinh);
      HienThiMau(_VpAddressVung3Khay3HienThiMau, X, Y, Mau, ChuongTrinh);
      HienThiMau(_VpAddressVung4Khay3HienThiMau, X, Y, Mau, ChuongTrinh);
      break;
    case 3:
      HienThiMau(_VpAddressVung1Khay4HienThiMau, X, Y, Mau, ChuongTrinh);
      HienThiMau(_VpAddressVung2Khay4HienThiMau, X, Y, Mau, ChuongTrinh);
      HienThiMau(_VpAddressVung3Khay4HienThiMau, X, Y, Mau, ChuongTrinh);
      HienThiMau(_VpAddressVung4Khay4HienThiMau, X, Y, Mau, ChuongTrinh);
      break;
    case 4:
      HienThiMau(_VpAddressVung1Khay5HienThiMau, X, Y, Mau, ChuongTrinh);
      HienThiMau(_VpAddressVung2Khay5HienThiMau, X, Y, Mau, ChuongTrinh);
      HienThiMau(_VpAddressVung3Khay5HienThiMau, X, Y, Mau, ChuongTrinh);
      HienThiMau(_VpAddressVung4Khay5HienThiMau, X, Y, Mau, ChuongTrinh);
      break;
  }
}

void DWINHMI::XoaToanBoHinhTron(void){

  for (uint8_t i = 0; i < 4; i++){
    XoaHinhTron(_VpAddressVung1KhayChon + 0x100UL * i);
    XoaMau(_VpAddressVung1KhayChonHienThiMau + 0x100UL * i);
    XoaMau(_VpAddressVung1Khay1HienThiMau + 0x100UL * i);
    XoaMau(_VpAddressVung1Khay2HienThiMau + 0x100UL * i);
    XoaMau(_VpAddressVung1Khay3HienThiMau + 0x100UL * i);
    XoaMau(_VpAddressVung1Khay4HienThiMau + 0x100UL * i);
    XoaMau(_VpAddressVung1Khay5HienThiMau + 0x100UL * i);

  }
}
void DWINHMI::XoaHinhTron(uint32_t VpAddress){
  uint8_t bufferTron[] = {0x5A, 0xA5, 0x11, 0x82, VpAddress >> 8, VpAddress & 0xFF, 0x00, 0x05, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xF8, 0x00, 0xFF, 0x00};
  DGUS_SERIAL.write(bufferTron, sizeof(bufferTron));
  //DGUS_SERIAL.println("OK");
  //uint8_t ebuf[] = {0x5A,0xA5,0x05,0x82, VpAddress >> 8, VpAddress,0x00,0xFF};
  //DGUS_SERIAL.write(ebuf,8);
  // delay(20);
}

void DWINHMI::XoaMau(uint32_t VpAddress){
  uint8_t bufferMau[] = {0x5A, 0xA5, 0x0F, 0x82, VpAddress >> 8, VpAddress & 0xFF, 0x00, 0x08, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, MauTrang >> 8, MauTrang & 0xFF, 0xFF, 0x00};
  DGUS_SERIAL.write(bufferMau, sizeof(bufferMau));
  // //DGUS_SERIAL.println("OK");
  // uint8_t ebuf[] = {0x5A,0xA5,0x05,0x82, VpAddress >> 8, VpAddress,0x00,0xFF};
  // DGUS_SERIAL.write(ebuf,8);
}

void DWINHMI::CapNhatLaiDanhSachToaDo(ChuongTrinh_t & ChuongTrinh)
{
  for (uint8_t KhuVuc = 0; KhuVuc < SoLuongKhuVucToiDa; KhuVuc++)
  {
    ChuongTrinh.DsToaDo[KhuVuc].clear();
    for (uint8_t x = 0; x < ChuongTrinh.rackX; x++)
    {
      for (uint8_t y = 0; y < ChuongTrinh.rackY; y++)
      {
        ChuongTrinh.DsToaDo[KhuVuc].push_back({x, y});
      }
    }
  }

}

void DWINHMI::HienThiNoiDungChuongTrinh(uint8_t IdxChuongTrinh, uint32_t VpAddress)
{
  String Name = _ChuongTrinhEEPROM[IdxChuongTrinh].tenchuongtrinh;
  String rackX = String(_ChuongTrinhEEPROM[IdxChuongTrinh].rackX);
  String rackY = String(_ChuongTrinhEEPROM[IdxChuongTrinh].rackY);
  String Volume = String(_ChuongTrinhEEPROM[IdxChuongTrinh].thetich);
  String PumpSpeed = String(_ChuongTrinhEEPROM[IdxChuongTrinh].tocdobom);
  String DKOng = String(_ChuongTrinhEEPROM[IdxChuongTrinh].duongkinhong);
  String SpeedXY = String(_ChuongTrinhEEPROM[IdxChuongTrinh].tocdoXY);
  String Antidrop = String(_ChuongTrinhEEPROM[IdxChuongTrinh].chongnhogiot);
  String data = Name + "\r\n" 
                + "Quantity: " + rackX + "x" + rackY + "\r\n" 
                + "Volume: " + Volume + " ml\r\n" 
                + "PumpSpeed:" + PumpSpeed + "rpm\r\n" 
                + "Tube ID: " + DKOng + " mm\r\n" 
                + "SpeedXY:" + SpeedXY + "mm/min\r\n" 
                + "Antidrop: " + Antidrop + "deg";
  GuiTextLenHMI(VpAddress, data);
}

bool DWINHMI::DangOViTriHome(void)
{
  if (DuLieuBoardGuiLenHMI.ViTriHienTaiX == DEFAULT_HOMING_PULLOFF &&
      DuLieuBoardGuiLenHMI.ViTriHienTaiY == -DEFAULT_HOMING_PULLOFF &&
      DuLieuBoardGuiLenHMI.ViTriHienTaiZ == -DEFAULT_HOMING_PULLOFF)
  {
    return true;
  }
  return false;
}

void DWINHMI::HienThiThongTinPhanPhoi(void)
{
  Serial.println("KhuVucDangXuLy: " + String(IdxKhuVucDangXuLy));
  ChuongTrinh_t chuongtrinhhienthi = _ChuongTrinhEEPROM[DSIndexChuongTrinhDuocChon[IdxKhuVucDangXuLy]];
  String rackstr, volstr, fillstr, AntiDropStr;
  rackstr = "Rack: " + String(chuongtrinhhienthi.rackX) + "x" + String(chuongtrinhhienthi.rackY) + "\r\n";
  volstr = "Vol: " + String(chuongtrinhhienthi.thetich) + "ml\r\n";
  fillstr = "Fill: " + String(chuongtrinhhienthi.DsToaDo[IdxKhuVucDangXuLy].size()) + "/" + String(chuongtrinhhienthi.rackX * chuongtrinhhienthi.rackY) + "\r\n";
  AntiDropStr = "Antidrop: " + String(chuongtrinhhienthi.chongnhogiot) + "deg";
  String data;
  data = rackstr + volstr + fillstr + AntiDropStr;
  GuiTextLenHMI(_VPTextprogram1TrangThietLapChayCheDoMULTI + IdxKhuVucDangXuLy * 0x100, data);
}

void DWINHMI::HienThiIconChuongTrinhDuocChon(uint32_t VpAddressNutNhanDauTien)
{
  uint32_t VpAddressIcon;
  int Code;
  if (VpAddressNutNhanDauTien == _HMIBamNutChonProgram1TrangCaiDatCheDoChuongTrinh)
  {
    if (!(codeHMI >= _HMIBamNutChonProgram1TrangCaiDatCheDoChuongTrinh && codeHMI <= _HMIBamNutChonProgram10TrangCaiDatCheDoChuongTrinh))
    {
      codeHMI = -1;
      return;
    }
    Code = codeHMI;
  }
  else
  {
    if (!(lastcodeHMI >= _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh && lastcodeHMI <= _HMIBamNutChonChuongTrinh10TrangChonChuongTrinh))
    {
      codeHMI = -1;
      return;
    }
    Code = lastcodeHMI;
  }

  VpAddressIcon = _VPChonProgram1TrangCaiDatCheDoChuongTrinh + (Code % VpAddressNutNhanDauTien);
  IndexChuongTrinhDangXuLy = Code % VpAddressNutNhanDauTien;

  for (uint8_t icon = 0; icon < SoLuongChuongTrinhToiDa; icon++)
  {
    if (icon == IndexChuongTrinhDangXuLy)
    {
      GuiIconLenHMI(VpAddressIcon, 1);
      HienThiNoiDungChuongTrinh(icon, _VPTextHienThiChucNangProgramTrangCaiDatCheDoChuongTrinh);
    }
    else
    {
      GuiIconLenHMI(_VPChonProgram1TrangCaiDatCheDoChuongTrinh + icon, 0);
    }
  }
}

void DWINHMI::HienThiThongTinABCH(void)
{
  int HZ1 = _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriH[0];
  int AX1 = _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriA[0].X;
  int AY1 = -_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriA[0].Y;
  int BX1 = _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriB[0].X;
  int BY1 = -_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriB[0].Y;
  int CX1 = _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriC[0].X;
  int CY1 = -_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriC[0].Y;

  int HZ2 = _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriH[1];
  int AX2 = _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriA[1].X;
  int AY2 = -_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriA[1].Y;
  int BX2 = _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriB[1].X;
  int BY2 = -_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriB[1].Y;
  int CX2 = _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriC[1].X;
  int CY2 = -_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ViTriC[1].Y;

  char * temp;
  asprintf(&temp, "H1:%d H2:%d\r\n"
            "A1:(%d,%d) A2:(%d,%d)\r\n" 
            "B1:(%d,%d) B2:(%d,%d)\r\n" 
            "C1:(%d,%d) C2:(%d,%d)\r\n", 
            HZ1, HZ2, AX1, AY1, AX2, AY2, BX1, BY1, BX2, BY2, CX1, CY1, CX2, CY2);
  GuiTextLenHMI(_VPTextInformationLocation,String(temp));
  free(temp);
}

void DWINHMI::flushDWIN(void)
{
  uint32_t startTime = millis();
  while (millis() - startTime < 100)
  {
    delay(1);
    int c = -1;
    while (DGUS_SERIAL.available() > 0)
    {
      c = DGUS_SERIAL.read();
      // if (c == 0x5A)
      // {
      //   return;
      // }
    }
    if (c != -1)
    {
      return;
    }

  }
}