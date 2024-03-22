#include "10_DWINHMI.h"
//----------------------------------------------------------------------------------------------------------------------
const byte     RxPin = 16;      // Chân Rx2 trên ESP32
const byte     TxPin = 17;      // Chân Tx2 trên ESP32
HardwareSerial _DWINSerial(2);  // Màn hình HMI DWIN giao tiếp với MCU qua UART2
#define DGUS_SERIAL Serial2     // Cổng UART giao tiếp giữa MCU với HMI
#define DGUS_BAUD 115200        // Baundrate giao tiếp UART giữa MCU và HMI
//----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------
#define _ON 1   // ON thiết bị
#define _OFF 0  // OFF thiết bị
//-----------------------------------------------------------------

//=============================================================================================================================
// Biến codeHMI là static nên cần phải được khai báo lại tại
// đầu file .cpp theo cú pháp sau thì mới truy xuất được đến biến static
int DWINHMI::codeHMI = -1;  // Reset mã code trả về của các nút do user bấm trên HMI
int DWINHMI::lastcodeHMI = -1;  // Reset mã code cũ trả về của các nút do user bấm trên HMI
static int _indexdsmulti = 0; // Biến này được dùng để đếm index danh sách chương trình được chọn ở chế độ multi
// uint8_t XYZ;
static uint32_t _time500ms = millis();
static uint32_t _time1s = millis();
static uint32_t _time5s = millis(); 
// static String TimeRunStart_c = "00:00";
// static int TimeNow = 0;
uint8_t holdAdd = 0; // 0 1 2
volatile bool ManualControl = false;
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

  xTaskCreate(xTaskFunction_RunTime, "Task Name", 4096, (void *)this, 1, &xTaskHandle_RunTime);
  vTaskSuspend(xTaskHandle_RunTime);


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
  if (address.substring(0, 4) == "4f4b")
  {
    return;
  }
#ifdef debug_dwin
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

    if (!ManualControl) return;
    //extract the first 4 characters of the address
    address = address.substring(0, 4);

    static String prevAddr = address;

    #ifdef debug_dwin
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
      DWIN::setPage(_TrangCaiDatCheDoChuongTrinh);
      break;
    }
    case _HMIBamNutCalibTrangCaiDat:
    {
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
      if (Mode == _CheDoHoatDongSimple)  // Tùy vào chế độ sẽ vào từng trang khác nhau
        DWIN::setPage(_TrangPhanPhoiViTri);
      else
        DWIN::setPage(_TrangThietLapCheDoChayMulti);  
      break;
    }
    case _HMIBamNutModeTrangCaiDatCheDoChuongTrinh:
    {
      if (Mode == _CheDoHoatDongSimple)  // Chỉnh chế độ mode
      {
        DWIN::setPage(_TrangCaiDatCheDoChuongTrinhMULTI);
      }
      else 
      {
        DWIN::setPage(_TrangCaiDatCheDoChuongTrinh);
      }
      break;
    }
    case _HMIBamNutSetupProgramTrangCaiDat:
    {
      /* Dòng code này giúp ta xác định việc chọn chương trình để làm gì ?? */
      CheDoVaoTrangChonChuongTrinh = _VaoTrangTuNutSetupProgramTrangCaiDatCheDoChuongTrinh;
      DWIN::setPage(_TrangChonChuongTrinh);
      break;
    }
    case _HMIBamNutProgram1TrangCaiDatCheDoChuongTrinh:
    case _HMIBamNutProgram2TrangCaiDatCheDoChuongTrinh:
    case _HMIBamNutProgram3TrangCaiDatCheDoChuongTrinh:
    case _HMIBamNutProgram4TrangCaiDatCheDoChuongTrinh:
    case _HMIBamNutProgram5TrangCaiDatCheDoChuongTrinh:
    {
      /* Dòng code này giúp ta xác định việc chọn chương trình để làm gì ?? */
      CheDoVaoTrangChonChuongTrinh = _VaoTrangTuCacNutProgramTrangCaiDatCheDoChuongTrinh;
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
        /* Tùy thuộc vào tính năng chọn chương trình để làm gì mà ta có việc chuyển trang tương ứng */
        switch (CheDoVaoTrangChonChuongTrinh)
        {
          case _VaoTrangTuCacNutProgramTrangCaiDatCheDoChuongTrinh:
          {
            if (Mode == _CheDoHoatDongSimple)
              DWIN::setPage(_TrangCaiDatCheDoChuongTrinh);
            else
              DWIN::setPage(_TrangCaiDatCheDoChuongTrinhMULTI);
            break;
          }
          case _VaoTrangTuNutChangeTrangPhanPhoiViTri:
            DWIN::setPage(_TrangThietLapCheDoChayMulti);
            break;
          case _VaoTrangTuNutCopyTrangThongSoChuongTrinh:
          case _VaoTrangTuNutSetupProgramTrangCaiDatCheDoChuongTrinh:
          case _VaoTrangTuNutOK_EXITTrangThongSoChuongTrinh:
            DWIN::setPage(_TrangThongSoChuongTrinh);
            break;
          case _VaoTrangTuNutSelectTrangCalib:
            DWIN::setPage(_TrangCalib);
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
          if (Mode == _CheDoHoatDongSimple)
            DWIN::setPage(_TrangCaiDatCheDoChuongTrinh);
          else
            DWIN::setPage(_TrangCaiDatCheDoChuongTrinhMULTI);
          break;
        case _VaoTrangTuNutCopyTrangThongSoChuongTrinh:
          DWIN::setPage(_TrangThongSoChuongTrinh);
          break;
        case _VaoTrangTuNutChangeTrangPhanPhoiViTri:
          DWIN::setPage(_TrangPhanPhoiViTri);
          break;
        case _VaoTrangTuNutOK_EXITTrangThongSoChuongTrinh:
          DWIN::setPage(_TrangCaiDat);
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
      if (Mode == _CheDoHoatDongMulti)
        /* Dòng code này giúp ta xác định việc chọn chương trình để làm gì ?? */
        CheDoVaoTrangChonChuongTrinh = _VaoTrangTuNutChangeTrangPhanPhoiViTri;
      DWIN::setPage(_TrangChonChuongTrinh);
      break;
    }
    case _HMIBamNutOKTrangPhanPhoiViTri:
    {
      /* Chỉ thực hiện chuyển trang từ nút OK khi và chỉ khi người dùng cập nhật tọa độ */
      // if (_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DanhSachToaDo.size() != 0)
      // {
        if (Mode == _CheDoHoatDongSimple)
          DWIN::setPage(_TrangHienThiPhanPhoiThoiGianThuc);
        else
          DWIN::setPage(_TrangThietLapCheDoChayMulti);
      // }
      break;
    }
    case _HMIBamNutEXITTrangPhanPhoiViTri:
    {
      if (Mode == _CheDoHoatDongSimple)
        DWIN::setPage(_TrangCaiDatCheDoChuongTrinh);
      else
        DWIN::setPage(_TrangThietLapCheDoChayMulti);
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
      /* Chỉ thực hiện Exit khi đã máy đã tắt */
      if (BamNutRunON == false)
      {
        if (Mode == _CheDoHoatDongSimple)
          DWIN::setPage(_TrangCaiDatCheDoChuongTrinh);
        else
          DWIN::setPage(_TrangCaiDatCheDoChuongTrinhMULTI);

        VeKhungHCN(1, MauDen);

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
      DWIN::setPage(_TrangPhanPhoiViTri);
      break;
    }
    case _HMIBamNutEXITTrangThietLapCheDoChayMulti:
    {
        DWIN::setPage(_TrangCaiDatCheDoChuongTrinhMULTI);
      break;
    }
    case _HMIBamNutRUNTrangThietLapCheDoChayMulti:
    {
      DWIN::setPage(_TrangHienThiPhanPhoiThoiGianThuc);

      VeKhungHCN(DSIndexChuongTrinhDuocChon.size(), MauDen);

      for (uint8_t i = 0; i < DSIndexChuongTrinhDuocChon.size(); i++)
      {
        Serial.printf("Ve hinh tron %d \r\n",i);
        ResetFullMau(DSIndexChuongTrinhDuocChon[i], MauDo, i);
      }

      break;
    }
    /* =============================== END: CODE TRANG 135 TRANG THIẾT LẬP CHẾ ĐỘ CHẠY MULTI ===============================*/
    /* =============================== BEGIN: CODE TRANG 136 TRANG THÔNG SỐ CHƯƠNG TRÌNH ===============================*/
    case _HMIBamNutCOPYTrangThongSoChuongTrinh:
    {
      /* Dòng code này giúp ta xác định việc chọn chương trình để làm gì ?? */
      CheDoVaoTrangChonChuongTrinh = _VaoTrangTuNutCopyTrangThongSoChuongTrinh;
      DWIN::setPage(_TrangChonChuongTrinh);
      break;
    }
    case _HMIBamNutMODIFYTrangThongSoChuongTrinh:
    {
      DWIN::setPage(_TrangThietLapChuongTrinhDiChuyen);
      break;
    }
    case _HMIBamNutOKTrangThongSoChuongTrinh:
    case _HMIBamNutEXITTrangThongSoChuongTrinh:
    {

      /* Dòng code này giúp ta xác định việc chọn chương trình để làm gì ?? */
      CheDoVaoTrangChonChuongTrinh = _VaoTrangTuNutOK_EXITTrangThongSoChuongTrinh;
      DWIN::setPage(_TrangChonChuongTrinh);
      CapNhatHienThiThongTinTenChuongTrinhKhiVaoTrangChonChuongTrinh();
      GuiTextLenHMI(_VPTextHienThiChuongTrinhTrangChonChuongTrinh,"");
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
    case _HMIBamNutBACKTrangThietLapChuongTrinhDiChuyen:
    {
      DWIN::setPage(_TrangThongSoChuongTrinh);
      codeHMI = -1;
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
      DWIN::setPage(_TrangThongSoChuongTrinh);
      break;
    }
    case _HMIBamNutBACKTrangLayViTriChonDiem:
    {
      DWIN::setPage(_TrangThietLapTheTichPhanPhoi);
      //DWIN::setPage(_TrangThietLapChuongTrinhDiChuyen);
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
      DWIN::setPage(_TrangChonChuongTrinh);
      break;
    }
    case (_HMIBamNutSelectTrangXemConifg):
    {
      CheDoVaoTrangChonChuongTrinh = _VaoTrangTuNutSelectTrangXemConfig;
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
      if(Mode == _CheDoHoatDongMulti)
        DWIN::setPage(_TrangCaiDatCheDoChuongTrinhMULTI);
      else 
        DWIN::setPage(_TrangCaiDatCheDoChuongTrinh);
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
      codeHMI = -1;
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
      DWIN::setPage(_TrangBanPhimCaiDatThoiGian);
      codeHMI = -1;
      break;
    case (_HMIBamNutEnterTrangDatenTime):
    {
      Serial.println("_HMIBamNutEnterTrangDatenTime");
      DuLieuHMIGuiXuongBoard.CapNhatThoiGianRTC = true;
      String Ngay_str = DuLieuHMIGuiXuongBoard.RTC_Ngay +  "/" + DuLieuHMIGuiXuongBoard.RTC_Thang + "/" + DuLieuHMIGuiXuongBoard.RTC_Nam;
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
      /* Cập nhật tất cả chương trình có trong EEPROM*/
      _ChuongTrinhEEPROM = DuLieuBoardGuiLenHMI.DSChuongTrinh;

      CapNhatHienThiDuLieuThoiGianTuDS3231();
      GuiTextLenHMI(_VPTextNgay,DuLieuBoardGuiLenHMI.RTC_ChuoiNgay);
      GuiTextLenHMI(_VPTextGio,DuLieuBoardGuiLenHMI.RTC_ChuoiGio);  
      GuiTextLenHMI(_VPTextGetLocationStep,"1.0");
      GuiTextLenHMI(_VPTextGetLocationFeed,"500");
      GuiTextLenHMI(_VPTextSPEEDXYZTrangLayViTri,"LOW");
      VeKhungHCN(_VpKhungChonToaDo, 795, 100, 1495, 450, MauDen);  

      for (chuongtrinh_t& ChuongTrinh : _ChuongTrinhEEPROM)
      {
        ChuongTrinh.DanhSachToaDo.clear();
        for (uint8_t i = 0; i < ChuongTrinh.rackX; i++)
        {
          for (uint8_t j = 0; j < ChuongTrinh.rackY; j++)
          {
            ChuongTrinh.DanhSachToaDo.push_back({i, j});
          }
        }
      }
    }
  }
  /* Chuyển trang khi go home OK */
  if(DuLieuBoardGuiLenHMI.TrangThaiXacNhanSetHome == true)
  {
      DWIN::setPage(_TrangCaiDatCheDoChuongTrinh);
      DuLieuBoardGuiLenHMI.TrangThaiXacNhanSetHome = false;
  }

  /* Đoạn code này được sử dụng để hiển thị các chương trình hiển thị lần đầu tiên */
  if (codeHMI == _HMIBamNutGoHomeTrangChaySetHome)
  {
    CapNhatHienThiTenCacChuongTrinhTrangCaiDatCheDoChuongTrinh();
  } 
  if(codeHMI == _HMIBamNutBackTrangCaiDat)
  {
    CapNhatHienThiTenCacChuongTrinhTrangCaiDatCheDoChuongTrinh();
  }
}
//=============================================================================================================================
/* Đoạn code này được sử dụng để xử lý chọn chương trình ở chế độ simple hoặc multi */
void DWINHMI::XuLyChonChuongTrinhSeChay(void)
{
  if(codeHMI ==  _HMIBamNutEXITTrangHienThiPhanPhoiThoiGianThuc || codeHMI == _HMIBamNutBackTrangCaiDat)
  {
    // HienThiNoiDungChuongTrinh();

    if ((Mode == _CheDoHoatDongSimple) && (DSIndexChuongTrinhDuocChon.size() > 0))
    {
      HienThiNoiDungChuongTrinh();
    }
    else
    {
    GuiTextLenHMI(_VPTextHienThiTenProgramTrangCaiDatCheDoChuongTrinh, "");
    GuiTextLenHMI(_VPTextHienThiChucNangProgramTrangCaiDatCheDoChuongTrinh, "");
    }
  }
  /* Khi bấm vào các icon để chuyển sang tick xanh để chọn program */
  if (codeHMI >= _HMIBamNutChonProgram1TrangCaiDatCheDoChuongTrinh && codeHMI <= _HMIBamNutChonProgram5TrangCaiDatCheDoChuongTrinh)
  {

    /* Khi nhấn nút sẽ trả về VP của icon tại nút đó */
    IndexChuongTrinhDangXuLy = DSIndexChuongTrinhHienThi[codeHMI % _HMIBamNutChonProgram1TrangCaiDatCheDoChuongTrinh];
    int vp = _VPChonProgram1TrangCaiDatCheDoChuongTrinh + (codeHMI % _HMIBamNutChonProgram1TrangCaiDatCheDoChuongTrinh);

    /* Cập nhật lại trạng thái chương trình */
    _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].TrangThaiChuongTrinh = !_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].TrangThaiChuongTrinh;

    GuiIconLenHMI(vp, _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].TrangThaiChuongTrinh);
    
    if (Mode == _CheDoHoatDongSimple) /* Nếu ở chế độ simple */
    {
      /* Cập nhật lại trạng thái của các chương trình khác */
      for (int i = 0; i < 5; i++)
      {
        if (DSIndexChuongTrinhHienThi[i] != IndexChuongTrinhDangXuLy) 
        {
          _ChuongTrinhEEPROM[DSIndexChuongTrinhHienThi[i]].TrangThaiChuongTrinh = KhongDuocChon;
          GuiIconLenHMI(_VPChonProgram1TrangCaiDatCheDoChuongTrinh + i, 0);
        }
      }
    }
    
    /* Hiển thị text nội dung */
    if (_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].TrangThaiChuongTrinh == KhongDuocChon)
    {
      GuiTextLenHMI(_VPTextHienThiTenProgramTrangCaiDatCheDoChuongTrinh, "");
      GuiTextLenHMI(_VPTextHienThiChucNangProgramTrangCaiDatCheDoChuongTrinh, "");
    }
    else
    {
      GuiTextLenHMI(_VPTextHienThiTenProgramTrangCaiDatCheDoChuongTrinh, _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tenchuongtrinh);
      //String program = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tenchuongtrinh);
      String rackX = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackX);
      String rackY = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackY);
      String Volume = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].thetich);
      String PumpSpeed = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tocdobom);
      String DKOng = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].duongkinhong);
      String SpeedXY = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tocdoXY);
      String data = "Quatity: " + rackX + "x" + rackY + "\r\n" + "Volume: " + Volume + " ml\r\n" +
        "Pump speed : " + PumpSpeed + " rpm\r\n" + "Tube ID: " + DKOng + " mm\r\n" + "Speed XY: " + SpeedXY + " mm/min";
      GuiTextLenHMI(_VPTextHienThiChucNangProgramTrangCaiDatCheDoChuongTrinh, data);
    }
  }
  
  /* Kiểm tra nếu như chế độ vào trang chọn chương trình là khi ta thay đổi chương trình hiển thị tại trang cài đặt chế độ chương trình */
  if (CheDoVaoTrangChonChuongTrinh == _VaoTrangTuCacNutProgramTrangCaiDatCheDoChuongTrinh)
  {
    /* Hiển thị các dữ liệu chương trình khi vào trang chọn chương trình */
    if (codeHMI >= _HMIBamNutProgram1TrangCaiDatCheDoChuongTrinh && codeHMI <= _HMIBamNutProgram5TrangCaiDatCheDoChuongTrinh)
    {
      IndexChuongTrinhDangXuLy = codeHMI % _HMIBamNutProgram1TrangCaiDatCheDoChuongTrinh;
      CapNhatHienThiThongTinTenChuongTrinhKhiVaoTrangChonChuongTrinh();
    }

    /* Hiển thị dữ liệu khi bấm chọn chương trình */
    if (codeHMI >= _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh && codeHMI <= _HMIBamNutChonChuongTrinh10TrangChonChuongTrinh && lastcodeHMI != codeHMI)
    {
      // IndexChuongTrinhDangXuLy = codeHMI % _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh;
      CapNhatHienThiThongTinChuongTrinhTrangChonChuongTrinh();
    }
  
    /* Khi kết thúc việc chọn bấm OK */
    if (codeHMI == _HMIBamNutOKTrangChonChuongTrinh)
    {

      if (!((lastcodeHMI >= _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh) && (lastcodeHMI <= _HMIBamNutChonChuongTrinh10TrangChonChuongTrinh)))
      {
        Serial.println("Khong phai nut chon chuong trinh");
        return;
      }

      uint8_t IdxChuongTrinhVuaChon = lastcodeHMI % _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh;
      for (size_t i = 0; i < sizeof(DSIndexChuongTrinhHienThi); i++)
      {
        if (DSIndexChuongTrinhHienThi[i] == IdxChuongTrinhVuaChon)
        {
          return;
        }
      }

      /* Sẽ cho trạng thái chương trình được chọn thành trạng thái chương trình trước đó */
      _ChuongTrinhEEPROM[lastcodeHMI % _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh].TrangThaiChuongTrinh = _ChuongTrinhEEPROM[DSIndexChuongTrinhHienThi[IndexChuongTrinhDangXuLy]].TrangThaiChuongTrinh; 
      
      /* Lúc này chương trình dù có chọn hay không trước đó thì đều sẽ chuyển thành không chọn */
      _ChuongTrinhEEPROM[DSIndexChuongTrinhHienThi[IndexChuongTrinhDangXuLy]].TrangThaiChuongTrinh = 0;

      /* Cập nhật lại danh sách index của các chương trình hiển thị và ở chế độ multi */
      DSIndexChuongTrinhHienThi[IndexChuongTrinhDangXuLy] = lastcodeHMI % _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh;
      Serial.println(IndexChuongTrinhDangXuLy);
      /* Cập nhật để hiển thị các chương trình trang cài đặt chế độ chương trình */
      for (uint8_t i = 0; i < 5; i++)
      {
        GuiTextLenHMI(_VPTextProgram1TrangChonCheDo + i * 0x100, _ChuongTrinhEEPROM[DSIndexChuongTrinhHienThi[i]].tenchuongtrinh);
        Serial.println(DSIndexChuongTrinhHienThi[i]);
      }
      /* Cập nhật hiển thị dữ liệu khi chuyển trang */
      vector<chuongtrinh_t> chuongtrinhhienthi = _ChuongTrinhEEPROM;
      uint8_t indexchuongtrinhhienthi = DSIndexChuongTrinhHienThi[IndexChuongTrinhDangXuLy];
      
      if (chuongtrinhhienthi[indexchuongtrinhhienthi].TrangThaiChuongTrinh == 1)
      {
        GuiTextLenHMI(_VPTextHienThiTenProgramTrangCaiDatCheDoChuongTrinh, chuongtrinhhienthi[indexchuongtrinhhienthi].tenchuongtrinh);
        String rackX = String(chuongtrinhhienthi[indexchuongtrinhhienthi].rackX);
        String rackY = String(chuongtrinhhienthi[indexchuongtrinhhienthi].rackY);
        String Volume = String(chuongtrinhhienthi[indexchuongtrinhhienthi].thetich);
        String PumpSpeed = String(chuongtrinhhienthi[indexchuongtrinhhienthi].tocdobom);
        String DKOng = String(chuongtrinhhienthi[indexchuongtrinhhienthi].duongkinhong);
        String SpeedXY = String(chuongtrinhhienthi[indexchuongtrinhhienthi].tocdoXY);
        String data = "Quatity: " + rackX + "x" + rackY + "\r\n" + "Volume: " + Volume + " ml\r\n" +
          "Pump speed : " + PumpSpeed + " rpm\r\n" + "Tube ID: " + DKOng + " mm\r\n" + "Speed XY: " + SpeedXY + " mm/min";
        GuiTextLenHMI(_VPTextHienThiChucNangProgramTrangCaiDatCheDoChuongTrinh, data);
      }
      else
      {
        String data = "";
        GuiTextLenHMI(_VPTextHienThiTenProgramTrangCaiDatCheDoChuongTrinh, data);
        GuiTextLenHMI(_VPTextHienThiChucNangProgramTrangCaiDatCheDoChuongTrinh, data);
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
      CapNhatHienThiThongTinTenChuongTrinhKhiVaoTrangChonChuongTrinh();
      codeHMI = -1;
  }
  if (CheDoVaoTrangChonChuongTrinh == _VaoTrangTuNutSetupProgramTrangCaiDatCheDoChuongTrinh 
  || CheDoVaoTrangChonChuongTrinh == _VaoTrangTuNutOK_EXITTrangThongSoChuongTrinh)
  {
    /* Hiển thị dữ liệu khi bấm chọn chương trình */
    if (codeHMI >= _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh 
    && codeHMI <= _HMIBamNutChonChuongTrinh10TrangChonChuongTrinh && lastcodeHMI != codeHMI)
    {
      IndexChuongTrinhDangXuLy = codeHMI % _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh;

      chuongtrinh_t chuongtrinhxuly = _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy];
      String program = String(chuongtrinhxuly.tenchuongtrinh);
      String rackX = String(chuongtrinhxuly.rackX);
      String rackY = String(chuongtrinhxuly.rackY);
      String Volume = String(chuongtrinhxuly.thetich);
      String PumpSpeed = String(chuongtrinhxuly.tocdobom);
      String DKOng = String(chuongtrinhxuly.duongkinhong);
      String SpeedXY = String(chuongtrinhxuly.tocdoXY);
      String data = program + "\r\n" + "Quatity: " + rackX + "x" + rackY + "\r\n" + "Volume: " + Volume + " ml\r\n" +
        "Pump speed : " + PumpSpeed + " rpm\r\n" + "Tube ID: " + DKOng + " mm\r\n" + "Speed XY: " + SpeedXY + " mm/min";
      GuiTextLenHMI(_VPTextHienThiChuongTrinhTrangChonChuongTrinh, data);
    }
    if (codeHMI == _HMIBamNutOKTrangChonChuongTrinh)
    {
      CapNhatHienThiThongTinChiTietChuongTrinh();
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
    IndxchuongTrinhDangModify = DSIndexChuongTrinhHienThi[codeHMI % _HMIBamNutMODIFY1TrangThietLapCheDoChayMulti];
    GuiTextLenHMI(_VPTextHienThiTenProgramTrangPhanPhoiViTri, _ChuongTrinhEEPROM[IndxchuongTrinhDangModify].tenchuongtrinh);
  }
  if((codeHMI == _HMIBamNutXoaTrangMULTIPROGRAMSETUP) && (oneTimeMulti == false))
  {
    oneTimeMulti = true;
    GuiTextLenHMI(_VPTextSecondTrangMULTIPROGRAMSETUP,"X");
    codeHMI = -1;
  }

  if ((codeHMI == _HMIBamNutXoaTrangMULTIPROGRAMSETUP) && (oneTimeMulti == true))
  {
    oneTimeMulti = false;
    timeLoopDelay = 0;
    timeLoopDelay_c = "00:00:00";
    GuiTextLenHMI(_VPTextSecondTrangMULTIPROGRAMSETUP, "0");
    codeHMI = -1;
  }

  if(codeHMI == _HMIBamNutSecondTrangMULTIPROGRAMSETUP)
  {
    oneTimeMulti = false;
    codeHMI = -1;
  }
}
//=============================================================================================================================
/* Đoạn code này dùng để xử lý các tọa độ mà các ống cần phân phối */
void DWINHMI::XuLyChonViTriPhanPhoi(void)
{
  static chuongtrinh_t ChuongTrinhTamThoi;
  static uint16_t KhayChonTamThoi[12][8];
  if(codeHMI == _HMIBamNutRunTrangCaiDatCheDoChuongTrinh)
  {

    /* Luôn đảm bảo giá trị phân phối bằng full */
    if(Mode == _CheDoHoatDongSimple)
    {
      /* Lúc này cập nhật IndexChuongTrinhDangXuLy */

      for(uint8_t i = 0; i < 10; i++)
      {
        chuongtrinh_t chuongtrinh = _ChuongTrinhEEPROM[i];
        if(chuongtrinh.TrangThaiChuongTrinh == DuocChon)
        {
          IndexChuongTrinhDangXuLy = i;
          _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].ThuTuChay = 0;
          ChuongTrinhTamThoi = _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy];
          break;
        }     
      }

      GuiTextLenHMI(_VPTextHienThiTenProgramTrangPhanPhoiViTri,_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tenchuongtrinh);

   
      String name = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tenchuongtrinh);
      String rackX = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackX);
      String rackY = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackY);
      String Volume = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].thetich);
      String PumpSpeed = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tocdobom);
      String DKOng = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].duongkinhong);
      String SpeedXY = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tocdoXY);
      String data = name + "\r\n" + "Quatity: " + rackX + "x" + rackY + "\r\n" + "Volume: " + Volume + " ml\r\n" +
      "Pump speed : " + PumpSpeed + " rpm\r\n" + "Tube ID: " + DKOng + " mm\r\n" + "Speed XY: " + SpeedXY + " mm/min";
      GuiTextLenHMI(_VPTextHienThiChuongTrinhTrangChonChuongTrinh, data);

      /* Full toa do */
      vitrivatoado.clear();
      VeToanBoHinhTron(_VpAddressVung1KhayChon, _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy]);
      VeToanBoHinhTron(_VpAddressVung2KhayChon, _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy]);
      VeToanBoHinhTron(_VpAddressVung3KhayChon, _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy]);
      VeToanBoHinhTron(_VpAddressVung4KhayChon, _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy]);
      ResetFullMauChonToaDo(MauDo);
    }
    
    if(Mode == _CheDoHoatDongMulti)
    {
      DSIndexChuongTrinhDuocChon.clear();
      for(int i = 0 ; i < 5 ; i++ )
      {
        chuongtrinh_t& chuongtrinhxuly = _ChuongTrinhEEPROM[DSIndexChuongTrinhHienThi[i]];
        
        /* Kiểm tra xem trong các chương trình hiển thị, chương trình nào được chọn thì thêm vào danh sách index chương trình được chọn */
        // chuongtrinhxuly = _ChuongTrinhEEPROM[DSIndexChuongTrinhHienThi[i]];

        if(chuongtrinhxuly.TrangThaiChuongTrinh == DuocChon)
        {
          DSIndexChuongTrinhDuocChon.push_back(DSIndexChuongTrinhHienThi[i]);
          chuongtrinhxuly.ThuTuChay = i;
        }

        /* Hiển thị */
        String namestr, rackstr, volstr, fillstr;
        namestr = String(chuongtrinhxuly.tenchuongtrinh) + "\r\n";
        rackstr = "Rack: " + String(chuongtrinhxuly.rackX) + "x" + String(chuongtrinhxuly.rackY) + "\t\t";
        volstr = "Vol: " + String(chuongtrinhxuly.thetich) + "ml\r\n";
        fillstr = "Fill: " + String(chuongtrinhxuly.DanhSachToaDo.size()) + "/" + String(chuongtrinhxuly.rackX * chuongtrinhxuly.rackY);
        String data;
        data = namestr + rackstr + volstr + fillstr;
        GuiTextLenHMI(_VPTextprogram1TrangThietLapChayCheDoMULTI + i * 0x100, data);
      }

      for(int i = 0; i < DSIndexChuongTrinhDuocChon.size(); i++)
        Serial.printf("Index dc chon %d\r\n",DSIndexChuongTrinhDuocChon[i]);     
    }
  }
  /* Khi bấm Run hoặc modify để vào trang phân phối thì cập nhật chương trình đang xử lý */
  if(codeHMI >= _HMIBamNutMODIFY1TrangThietLapCheDoChayMulti && codeHMI <= _HMIBamNutMODIFY5TrangThietLapCheDoChayMulti)
  {
    if (Mode == _CheDoHoatDongMulti)
    {
      IndexChuongTrinhDangXuLy = IndxchuongTrinhDangModify;                             
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
      VeToanBoHinhTron(_VpAddressVung1KhayChon, _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy]);
      VeToanBoHinhTron(_VpAddressVung2KhayChon, _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy]);
      VeToanBoHinhTron(_VpAddressVung3KhayChon, _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy]);
      VeToanBoHinhTron(_VpAddressVung4KhayChon, _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy]);
      ResetFullMauChonToaDo(MauDo);
    }
  }

  if(codeHMI == _HMIBamNutFULLTrangPhanPhoiViTri)
  {
    DWIN::setPage(_TrangPhanPhoiViTriFULL);
    // _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DanhSachToaDo.clear();
    _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DanhSachToaDo.clear();
    for (int i = 0; i < _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackX * _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackY; i++)
    {
      toadoINT_t toadoht;
      toadoht.X = i % _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackX;
      toadoht.Y = i / _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackX;
      // _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DanhSachToaDo.push_back(toadoht);
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DanhSachToaDo.push_back(toadoht);
    }
    ResetFullMauChonToaDo(MauDo);
    DWIN::setPage(_TrangPhanPhoiViTri);
  }
  /* Clear toàn bộ tọa độ */
  if (codeHMI == _HMIBamNutCLEARTrangPhanPhoiViTri) 
  {
    DWIN::setPage(_TrangPhanPhoiViTriCLEAR);
    ResetFullMauChonToaDo(MauTrang);
    // _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DanhSachToaDo.clear();
    _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DanhSachToaDo.clear();
    DWIN::setPage(_TrangPhanPhoiViTri);
  }
  /* Khi bấm nút ADD */
  if(codeHMI == _HMIBamNutADDTrangPhanPhoiViTri)
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


  if(codeHMI == _HMIBamVaoVungChonToaDo)
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
        if (vitriX >= vitrivatoado[i].vitriX - KichThuocHcnDaVeTheoX && vitriX <= vitrivatoado[i].vitriX + KichThuocHcnDaVeTheoX &&
            vitriY >= vitrivatoado[i].vitriY - KichThuocHcnDaVeTheoY && vitriY <= vitrivatoado[i].vitriY + KichThuocHcnDaVeTheoY)
        {
          for (uint8_t m = 0; m < _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DanhSachToaDo.size(); m++) 
          {
            if (_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DanhSachToaDo[m].X == vitrivatoado[i].toadoX && _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DanhSachToaDo[m].Y == vitrivatoado[i].toadoY)
            {
              if(holdAdd == 1)
                return;
              if(holdAdd == 2)
              {
                HienThiMau(-1, vitrivatoado[i].toadoX, vitrivatoado[i].toadoY, _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy],MauTrang);
                if(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DanhSachToaDo.size() != 0)
                {
                  _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DanhSachToaDo.erase(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DanhSachToaDo.begin() + m);
                  Serial.printf("Xoa toa do chuong trinh: %s tai x: %d, y: %d", _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tenchuongtrinh.c_str(), vitrivatoado[i].toadoX, vitrivatoado[i].toadoY);
                  return;
                }
              }
            }
          }
          if(holdAdd == 1)
          {
            Serial.printf("X: %d | Y: %d\r\n",vitrivatoado[i].toadoX,vitrivatoado[i].toadoY);
            _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DanhSachToaDo.push_back({vitrivatoado[i].toadoX,vitrivatoado[i].toadoY});
            Serial.printf("Them toa do chuong trinh: %s tai x: %d, y: %d", _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tenchuongtrinh.c_str(), vitrivatoado[i].toadoX, vitrivatoado[i].toadoY);
            HienThiMau(-1, vitrivatoado[i].toadoX, vitrivatoado[i].toadoY, _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy],MauDo);
          }
        }
      }
    }

  }

  if (codeHMI == _HMIBamNutEXITTrangPhanPhoiViTri)
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
  if(codeHMI == _HMIBamNutOKTrangPhanPhoiViTri ||
    codeHMI  == _HMIBamNutEXITTrangPhanPhoiViTri ||
    codeHMI  == _HMIBamNutFULLTrangPhanPhoiViTri ||
    codeHMI  == _HMIBamNutCLEARTrangPhanPhoiViTri)
  {
    holdAdd = 0;
  }
  if (codeHMI == _HMIBamNutOKTrangPhanPhoiViTri)
  {
    if(Mode == _CheDoHoatDongMulti)
    {
      for(int i = 0 ; i < 5 ; i++ )
      {
        /* Hiển thị */
        chuongtrinh_t chuongtrinhhienthi;
        chuongtrinhhienthi = _ChuongTrinhEEPROM[DSIndexChuongTrinhHienThi[i]];
        String namestr, rackstr, volstr, fillstr;
        namestr = String(chuongtrinhhienthi.tenchuongtrinh) + "\r\n";
        rackstr = "Rack: " + String(chuongtrinhhienthi.rackX) + "x" + String(chuongtrinhhienthi.rackY) + "\t\t";
        volstr = "Vol: " + String(chuongtrinhhienthi.thetich) + "ml\r\n";
        fillstr = "Fill: " + String(chuongtrinhhienthi.DanhSachToaDo.size()) + "/" + String(chuongtrinhhienthi.rackX * chuongtrinhhienthi.rackY);
        String data;
        data = namestr + rackstr + volstr + fillstr;
        GuiTextLenHMI(_VPTextprogram1TrangThietLapChayCheDoMULTI + i * 0x100, data);
      }     
    }
    if(Mode == _CheDoHoatDongSimple)
    {
      Serial.printf("DS toa do chuong trinh %d\r\n",_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DanhSachToaDo.size());
      DSIndexChuongTrinhDuocChon.clear();
      DSIndexChuongTrinhDuocChon.push_back(IndexChuongTrinhDangXuLy);
      VeKhungHCN(DSIndexChuongTrinhDuocChon.size(), MauDen);

      for (uint8_t i = 0; i < DSIndexChuongTrinhDuocChon.size(); i++)
      {
        ResetFullMau(DSIndexChuongTrinhDuocChon[i], MauDo, i);
      }
    }

  }
}
//=============================================================================================================================
void DWINHMI::XuLyChonMode()
{
  /* Khi bấm vào nút Mode ( multi hoặc simple ) */
  if (codeHMI == _HMIBamNutModeTrangCaiDatCheDoChuongTrinh)
  {
    /* Reset lại các icon */
    for (int i = _VPChonProgram1TrangCaiDatCheDoChuongTrinh; i < _VPChonProgram5TrangCaiDatCheDoChuongTrinh + 1; i++) 
      GuiIconLenHMI(i, 0);

    /* Reset lại hiển thị thông số cài đặt chương trình */
    GuiTextLenHMI(_VPTextHienThiTenProgramTrangCaiDatCheDoChuongTrinh, "");
    GuiTextLenHMI(_VPTextHienThiChucNangProgramTrangCaiDatCheDoChuongTrinh, "");

    /* Reset trạng thái của chương trình : điều này thể hiện các chương trình sẽ không được chạy */
    for (int i = 0; i < 5; i++) 
    {
      _ChuongTrinhEEPROM[i].TrangThaiChuongTrinh = 0;
      _ChuongTrinhEEPROM[i].ThuTuChay = -1;
    }

    /* Thay đổi Mode */
    if (Mode == _CheDoHoatDongSimple) 
      Mode = _CheDoHoatDongMulti;
    else if (Mode == _CheDoHoatDongMulti) 
      Mode = _CheDoHoatDongSimple;
  }
}
//=============================================================================================================================
void DWINHMI::XuLyCalib()
{
  if(CheDoVaoTrangChonChuongTrinh == _VaoTrangTuNutSelectTrangCalib)
  {

    if(codeHMI == _HMIBamNutSelectTrangCalib)
    {
      CapNhatHienThiThongTinTenChuongTrinhKhiVaoTrangChonChuongTrinh();
    }

    /* Hiển thị dữ liệu khi bấm chọn chương trình */
    if (codeHMI >= _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh && codeHMI <= _HMIBamNutChonChuongTrinh10TrangChonChuongTrinh && lastcodeHMI != codeHMI)
    {
      IndexChuongTrinhDangXuLy = codeHMI % _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh;
      CapNhatHienThiThongTinChuongTrinhTrangChonChuongTrinh();
    }

    /* Khi kết thúc việc chọn bấm OK */
    if (codeHMI == _HMIBamNutOKTrangChonChuongTrinh)
    {      
      // nếu đổi chương trình khác thì reset lại các thông tin calib trước đó và hiện thông tin chương trình mới

      countTest = 1;
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].actualweight = 0.0;

      String name = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tenchuongtrinh);
      GuiTextLenHMI(_VPTextChuongTrinhTrangCalib, name);
      String density = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].density);
      GuiTextLenHMI(_VPTextDensity, density);
      String actualweight = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].actualweight);
      GuiTextLenHMI(_VPTextActualWeight, actualweight);     

      GuiTextLenHMI(_VPTextFillNumber, "0");

      // HienThiNoiDungChuongTrinh();
    } 
    /* Reset lại hiển thị */
    if(codeHMI == _HMIBamNutCalibTrangCaiDat)
    {
      countTest = 1;
      String data = "";
      GuiTextLenHMI(_VPTextHienThiChucNangProgramTrangCaiDatCheDoChuongTrinh, data);
      GuiTextLenHMI(_VPTextHienThiChuongTrinhTrangChonChuongTrinh, data);
      GuiTextLenHMI(_VPTextChuongTrinhTrangCalib, data);
      GuiTextLenHMI(_VPTextFillNumber, data);
      GuiTextLenHMI(_VPTextDensity, data);
      GuiTextLenHMI(_VPTextActualWeight, data);
      GuiTextLenHMI(_VPTextXacNhanSaveCalib,data);

      // HienThiNoiDungChuongTrinh();
      IndexChuongTrinhDangXuLy = -1;
      codeHMI = -1;
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
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].actualweight = data.toDouble();
      GuiTextLenHMI(_VPTextActualWeight,data);
      GuiTextLenHMI(_VPTextFillNumber,(String)countTest);
      countTest++;
    }
    if(codeHMI == _HMIBamNutResetTrangCalib)
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
  if(codeHMI == _HMIBamNutNEXTTrangThietLapChuongTrinhDiChuyen || codeHMI == _HMIBamNutBACKTrangLayViTriChonDiem)
  {
    CapNhatHienThiThongTinChuongTrinhTrangThietLapTheTichPhanPhoi();
  }

  switch(codeHMI)
  {
    case _HMIBamNutHourTrangSecondLoop:
      codeHMI = -1;
      ChuoiDangNhap = "";
      CheDoCaiDatTimeLoop = _CaiDatGioLoop;
      break;
    case _HMIBamNutMinutesTrangSecondLoop:
      codeHMI = -1;
      ChuoiDangNhap = "";
      CheDoCaiDatTimeLoop = _CaiDatPhutLoop;
      break;
    case _HMIBamNutSecondsTrangSecondLoop:
      codeHMI = -1;
      ChuoiDangNhap = "";
      CheDoCaiDatTimeLoop = _CaiDatGiayLoop;
      break;
  }

  /* Reset lại biến ChuoiDangNhap*/
  if((codeHMI >= _HMIBamNutTUBEPERROWTrangThietLapChuongTrinhDiChuyen && codeHMI <= _HMIBamNutSPEEDXYAXISrangThietLapChuongTrinhDiChuyen)
  || (codeHMI >= _HMIBamNutVolumeTrangThietLapTheTichPhanPhoi && codeHMI <= _HMIBamNutAntiDropTrangThietLapTheTichPhanPhoi)
  || (codeHMI == _HMIBamTextActualWeight || codeHMI == _HMIBamTextDensity))
  {
    ChuoiDangNhap = "";
  }
  /* Khi bấm nút trên bàn phím */
  if( (codeHMI >= _HMIBamNutBanPhim0 && codeHMI <= _HMIBamNutBanPhim9) 
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
    } 
  }

  if(codeHMI == _HMIBamNutEnterTrangBanPhim)
  {
    //byte IDPage = DWIN::getPage();
    
    switch (CheDoVaoTrangBanPhim)
    {
      case _VaoTrangThietLapSoOngTrenMotCot:
        _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackY = ChuoiDangNhap.toInt();
        CapNhatLaiDanhSachToaDo(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy]); // reset lại danh sách tọa độ do số lượng ống thay đổi
        break;
      case _VaoTrangThietLapSoOngTrenMotHang:
        _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].rackX = ChuoiDangNhap.toInt();
        CapNhatLaiDanhSachToaDo(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy]);
        break;
      case _VaoTrangThietLapTocDoTrucXY:
        _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tocdoXY = ChuoiDangNhap.toInt();
        break;
      case _VaoTrangBanPhimVolume:
        _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].thetich = ChuoiDangNhap.toDouble();
        break;
      case _VaoTrangBanPhimTubeID:
        _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].duongkinhong = ChuoiDangNhap.toDouble();
        break;
      case _VaoTrangBanPhimPumpSpeed:
        _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tocdobom = ChuoiDangNhap.toInt();
        break;
      case _VaoTrangBanPhimAntiDrop:
        _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].chongnhogiot = ChuoiDangNhap.toDouble();
        break;
      case _VaoTrangBanPhimDensity:
        _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].density = ChuoiDangNhap.toDouble();
        break;
      case _VaoTrangBanPhimActualWeight:
        _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].actualweight = ChuoiDangNhap.toDouble();
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
          DuLieuHMIGuiXuongBoard.RTC_Gio = ChuoiDangNhap;
          GuiTextLenHMI(_VPTextHourTrangDaynTime,ChuoiDangNhap); 
          break;
        case _CaiDatPhut:
          DuLieuHMIGuiXuongBoard.RTC_Phut = ChuoiDangNhap;
          GuiTextLenHMI(_VPTextMinuteTrangDaynTime,ChuoiDangNhap);
          break;
        case _CaiDatNgay:
          DuLieuHMIGuiXuongBoard.RTC_Ngay = ChuoiDangNhap;
          GuiTextLenHMI(_VPTextDayTrangDaynTime,ChuoiDangNhap);
          break;
        case _CaiDatThang:
          DuLieuHMIGuiXuongBoard.RTC_Thang = ChuoiDangNhap;
          GuiTextLenHMI(_VPTextMonthTrangDaynTime,ChuoiDangNhap);
          break;
        case _CaiDatNam:
          DuLieuHMIGuiXuongBoard.RTC_Nam = ChuoiDangNhap;
          GuiTextLenHMI(_VPTextYearTrangDaynTime,ChuoiDangNhap);
          break;
        }
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
        DWIN::setPage(_TrangThietLapCheDoChayMulti);
        break;
      }
      case _VaoTrangBanPhimCaiDatThoiGian:
      {
         DWIN::setPage(_TrangThoiGian);
        break;
      }
    }  
  }

  if(codeHMI == _HMIBamNutReturnTrangBanPhim)
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
        GuiTextLenHMI(_VPTextSecondTrangMULTIPROGRAMSETUP,(String)timeLoopDelay);
        break;
      }
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
        DWIN::setPage(_TrangThietLapCheDoChayMulti);
        break;
      }
      case _VaoTrangBanPhimCaiDatThoiGian:
      {
        DWIN::setPage(_TrangThoiGian);
        break;
      }
    }  
  }
  if(codeHMI == _HMIBamNutEnterTrangBanPhim || codeHMI == _HMIBamNutReturnTrangBanPhim)
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
    if (codeHMI >= _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh 
    && codeHMI <= _HMIBamNutChonChuongTrinh10TrangChonChuongTrinh && lastcodeHMI != codeHMI)
    {
      IndexChuongTrinhDangXuLy = codeHMI % _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh;

      chuongtrinh_t chuongtrinhxuly = _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy];
      String program = String(chuongtrinhxuly.tenchuongtrinh);
      String rackX = String(chuongtrinhxuly.rackX);
      String rackY = String(chuongtrinhxuly.rackY);
      String Volume = String(chuongtrinhxuly.thetich);
      String PumpSpeed = String(chuongtrinhxuly.tocdobom);
      String DKOng = String(chuongtrinhxuly.duongkinhong);
      String SpeedXY = String(chuongtrinhxuly.tocdoXY);
      String data = program + "\r\n" + "Quatity: " + rackX + "x" + rackY + "\r\n" + "Volume: " + Volume + " ml\r\n" +
        "Pump speed : " + PumpSpeed + " rpm\r\n" + "Tube ID: " + DKOng + " mm\r\n" + "Speed XY: " + SpeedXY + " mm/min";
      GuiTextLenHMI(_VPTextHienThiChuongTrinhTrangChonChuongTrinh, data);
    }
    if(codeHMI == _HMIBamNutOKTrangChonChuongTrinh)
    {
      _ChuongTrinhEEPROM[lastprogram] = _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy];
      CapNhatHienThiThongTinChiTietChuongTrinh();
    }
      
    if(codeHMI == _HMIBamNutExitTrangChonChuongTrinh)
    {
      IndexChuongTrinhDangXuLy = lastprogram;
      CapNhatHienThiThongTinChiTietChuongTrinh();
    }
    /* Reset lai hien thi */
  }
  if(codeHMI == _HMIBamNutNEXTTrangLayViTriChonDiem)
  {
    CapNhatHienThiThongTinChiTietChuongTrinh();
    codeHMI = 0;
  }
  if(codeHMI == _HMIBamNutNEXTTrangThietLapChuongTrinhDiChuyen)
  {
    String HZ = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].docaoH);
    String AX = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].vitriA.X);
    String AY = String(-_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].vitriA.Y);
    String BX = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].vitriB.X);
    String BY = String(-_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].vitriB.Y);
    String CX = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].vitriC.X);
    String CY = String(-_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].vitriC.Y);
    String temp = "H: Z=" + HZ + "\r\n" 
    + "A: X=" + AX + "; Y=" + AY + "\r\n" 
    + "B: X=" + BX + "; Y=" + BY + "\r\n" 
    + "C: X=" + CX + "; Y=" + CY + "\r\n" ;
    GuiTextLenHMI(_VPTextInformationLocation,temp);
  }
  // if(codeHMI >= _HMIBamNutGETATrangLayViTriChonDiem && codeHMI <= _HMIBamNutGETCTrangLayViTriChonDiem)
  // {
  //   GuiTextLenHMI(_VPTextGetLocationStep,"1.0");
  //   GuiTextLenHMI(_VPTextGetLocationFeed,"500");
  //   GuiTextLenHMI(_VPTextSPEEDXYZTrangLayViTri,"LOW");
  // }
  if(codeHMI == _HMIBamNutLOW)
  {
    GuiTextLenHMI(_VPTextGetLocationStep,"1.0");
    GuiTextLenHMI(_VPTextGetLocationFeed,"500");
    GuiTextLenHMI(_VPTextSPEEDXYZTrangLayViTri,"LOW");
  }
  if(codeHMI == _HMIBamNutMEDIUM)
  {
    GuiTextLenHMI(_VPTextGetLocationStep,"10.0");
    GuiTextLenHMI(_VPTextGetLocationFeed,"2000");
    GuiTextLenHMI(_VPTextSPEEDXYZTrangLayViTri,"MEDIUM");
  }
  if(codeHMI == _HMIBamNutFAST)
  {
    GuiTextLenHMI(_VPTextGetLocationStep,"30.0");
    GuiTextLenHMI(_VPTextGetLocationFeed,"5000");
    GuiTextLenHMI(_VPTextSPEEDXYZTrangLayViTri,"FAST");
  }
  static bool OKlable = false;
  if(codeHMI == _HMIBamNutGET)
  {
    GuiTextLenHMI(_VPTextXacNhanGETTrangLayViTri,"OK");
    OKlable = true;
  }

  /* Reset lại hiển thị nếu đã bấm OK */
  if(OKlable == true)
  {
    if(codeHMI >= _HMIBamNutXTru && codeHMI <= _HMIBamNutBackTrangLayViTriXacNhan && codeHMI != _HMIBamNutGET)
    {
      GuiTextLenHMI(_VPTextXacNhanGETTrangLayViTri,"");
      OKlable = false;
    }
  }

  if( codeHMI == _HMIBamNutGETATrangLayViTriChonDiem ||
    codeHMI == _HMIBamNutGETBTrangLayViTriChonDiem ||
    codeHMI == _HMIBamNutGETCTrangLayViTriChonDiem )
  {
    ManualControl = true;
  }
  if(codeHMI == _HMIBamNutGET)
  {
    double viTriX = DuLieuBoardGuiLenHMI.ViTriHienTaiX;
    double viTriY = DuLieuBoardGuiLenHMI.ViTriHienTaiY;
    double viTriZ = DuLieuBoardGuiLenHMI.ViTriHienTaiZ;
    switch (CheDoVaoTrangGETABCH)
    {
    case _VaoTrangGETH:
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].docaoH = viTriZ;
      break;
    case _VaoTrangGETA:
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].vitriA.X = viTriX;
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].vitriA.Y = viTriY;
      break;
    case _VaoTrangGETB:
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].vitriB.X = viTriX;
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].vitriB.Y = viTriY;      
      break;
    case _VaoTrangGETC:
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].vitriC.X = viTriX;
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].vitriC.Y = viTriY;
      break;
    default:
      break;
    }
  }
  if(codeHMI == _HMIBamNutBackTrangLayViTriXacNhan)
  {
    String HZ = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].docaoH);
    String AX = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].vitriA.X);
    String AY = String(-_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].vitriA.Y);
    String BX = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].vitriB.X);
    String BY = String(-_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].vitriB.Y);
    String CX = String(_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].vitriC.X);
    String CY = String(-_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].vitriC.Y);
    String temp = "H: Z=" + HZ + "\r\n" 
    + "A: X=" + AX + ";Y=" + AY + "\r\n" 
    + "B: X=" + BX + ";Y=" + BY + "\r\n" 
    + "C: X=" + CX + ";Y=" + CY + "\r\n" ;
    GuiTextLenHMI(_VPTextInformationLocation,temp);
  }
}
//=============================================================================================================================
void DWINHMI::HienThiThongSoHienThiThoiGianThuc(void)
{

  if(codeHMI == _HMIBamNutRUNTrangThietLapCheDoChayMulti || (codeHMI == _HMIBamNutOKTrangPhanPhoiViTri && Mode == _CheDoHoatDongSimple))
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
    if (BamNutRunON == true)
    {
      DWIN::setPage(_TrangHienThiPhanPhoiThoiGianThuc);
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
    _indexdsmulti = 0;
    delay_flag = false;
  }
}


//=============================================================================================================================
void DWINHMI::ResetProgram(void)
{
    if (codeHMI == _HMIBamNutRESETPROGRAMTrangHienThiPhanPhoiThoiGianThuc && BamNutRunON == false)
    {
      for (uint8_t i = 0; i < DSIndexChuongTrinhDuocChon.size(); i++)
      {
        ResetFullMau(DSIndexChuongTrinhDuocChon[i], MauDo, i);
      }
      GuiTextLenHMI(_VPTextTimeRunTrangHienThiPhanPhoiThoiGianThuc, "00:00:00");
      GuiTextLenHMI(_VPTextDelayTimeRunTrangHienThiPhanPhoiThoiGianThuc, "00:00:00");
      GuiTextLenHMI(_VPTextFillNumberTrangHienThiPhanPhoiThoiGianThuc,"0");
      TimeNow = 0;
      _indexdsmulti = 0;
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
#ifdef debug_dwin
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
          return RESUME;
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
         return EXITVETRANGCAIDATCHEDOCHUONGTRINH;
      }
      // case _HMIBamNutRunTrangCaiDatCheDoChuongTrinh:
      //   return CAPNHATCHUONGTRINH;

      case _HMIBamNutOKTrangPhanPhoiViTri:
      case _HMIBamNutRUNTrangThietLapCheDoChayMulti:
        return CAPNHATTOADO;

      // case _HMIBamNutSetupProgramTrangCaiDat:
      //   return CONFIGCHUONGTRINH;
      case _HMIBamNutEnterTrangBanPhim:
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
      case _HMIBamNutGoATrangXemConfig:
        codeHMI = -1;
        return GOA;
      case _HMIBamNutGoBTrangXemConfig:
        codeHMI = -1;
        return GOB;
      case _HMIBamNutGoCTrangXemConfig:
        codeHMI = -1;
        return GOC;
      case _HMIBamNutLOW:
        codeHMI = -1;
        return LOWSPEED;
      case _HMIBamNutMEDIUM:
        codeHMI = -1;
        return MEDIUMSPEED;
      case _HMIBamNutFAST:
        codeHMI = -1;
        return FASTSPEED;
      case _HMIBamNutUNLOCKTrangManualControl:
        codeHMI = -1;
        return UNLOCK;
      case _HMIBamNutRESETTrangManualControl:
        codeHMI = -1;
        return RESET;
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
          ESP_LOGI("Holding", "Start Holding");
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
        ESP_LOGI("Holding", "On Holding");
        // Serial.println("On Holding");
        MaLenhManualControl = RESETMALENH;
      }
      else if ((MaLenhManualControl == RESETMALENH) || (MaLenhManualControl == DUNGDICHUYEN))
      {
        ESP_LOGI("Holding", "Stop Holding");
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
  if(codeHMI == _HMIBamNutBackTrangCaiDat)
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
  else if ((codeHMI >= _HMIBamNutChonProgram1TrangCaiDatCheDoChuongTrinh && codeHMI <= _HMIBamNutChonProgram5TrangCaiDatCheDoChuongTrinh))
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
  else if(codeHMI == _HMIBamTextDensity)
    codeHMI = -1;
  else if(codeHMI >= _HMIBamNutBanPhim0 && codeHMI <= _HMIBamNutBanPhim9)
    codeHMI = -1;
  else if(codeHMI == _HMIBamNutBanPhimdelete)
    codeHMI = -1;
  else if(codeHMI == _HMIBamNutModeTrangCaiDatCheDoChuongTrinh)
    codeHMI = -1;    
  else if(codeHMI == _HMIBamNutRUNTrangThietLapCheDoChayMulti)
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
  lastcodeHMI = codeHMI;
  
  if (millis() - _time500ms >= 500)
  {
    _time500ms = millis();
  }
  /* Với trường hợp codeHMI >= _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh && codeHMI <= _HMIBamNutChonChuongTrinh10TrangChonChuongTrinh
  để lastcode được tồn tại nên không reset lascode*/
}
//=============================================================================================================================
void DWINHMI::XuLyXemConfig()
{
  static uint32_t fillnumber = 0; 
  static bool controlABC = false;
  if(codeHMI == _HMIBamNutConfigTrangCaiDatCheDoChuongTrinh)
  {
    IndexChuongTrinhDangXuLy = -1;
    GuiTextLenHMI(_VPTextGetLocationX,"");
    GuiTextLenHMI(_VPTextGetLocationY,"");
    GuiTextLenHMI(_VPTextGetLocationZ,"");
    GuiTextLenHMI(_VPTextTenChuongTrinhTrangXemConfig,"");
    GuiTextLenHMI(_VPTextTotalFillTubeTrangXemConfig,"");
    GuiTextLenHMI(_VpTextPumpControlTrangXemConfig,"");
  }
  if(codeHMI == _HMIBamNutEXITTrangManualControl || codeHMI == _HMIBamNutBackTrangLayViTriXacNhan)
  {
    ManualControl = false;
  }
  if( codeHMI == _HMIBamNutGoATrangXemConfig || 
    codeHMI == _HMIBamNutGoBTrangXemConfig ||
    codeHMI == _HMIBamNutGoCTrangXemConfig || 
    codeHMI == _HMIBamNutManualControlTrangXemConfig ||
    codeHMI == _HMIBamNutGETATrangLayViTriChonDiem ||
    codeHMI == _HMIBamNutGETBTrangLayViTriChonDiem ||
    codeHMI == _HMIBamNutGETCTrangLayViTriChonDiem )
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
    codeHMI == _HMIBamNutBackTrangLayViTriXacNhan)
  {
    controlABC = false;
  }

  if(codeHMI == _HMIBamNutTestFillTrangXemConfig)
  {
    Serial.println("Test fill");
    fillnumber++;
    GuiTextLenHMI(_VPTextTotalFillTubeTrangXemConfig,(String)fillnumber);
  }
  if(codeHMI == _HMIBamNutDirectionTrangXemConfig)
  {
    static bool direction = false;
    direction = !direction;
    if (direction == true)
      GuiIconLenHMI(_VPIconDIRECTION, 0);
    else
      GuiIconLenHMI(_VPIconDIRECTION, 1);
    // codeHMI = -1;
  }
  if(codeHMI == _HMIBamNutModifyProgramTrangXemConfig)
  {
    CapNhatHienThiThongTinChiTietChuongTrinh();
    codeHMI = -1;
  }
  if(CheDoVaoTrangChonChuongTrinh == _VaoTrangTuNutSelectTrangXemConfig)
  {

    if(codeHMI == _HMIBamNutSelectTrangXemConifg)
    {
      CapNhatHienThiThongTinTenChuongTrinhKhiVaoTrangChonChuongTrinh();
      codeHMI = -1;
    }
    /* Hiển thị dữ liệu khi bấm chọn chương trình */
    if (codeHMI >= _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh && codeHMI <= _HMIBamNutChonChuongTrinh10TrangChonChuongTrinh && lastcodeHMI != codeHMI)
    {
      IndexChuongTrinhDangXuLy = codeHMI % _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh;
      CapNhatHienThiThongTinChuongTrinhTrangChonChuongTrinh();
    }
    if(codeHMI == _HMIBamNutOKTrangChonChuongTrinh)
    {
      GuiTextLenHMI(_VPTextTenChuongTrinhTrangXemConfig,_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tenchuongtrinh);
      //GuiTextLenHMI(_VPTextTotalFillTubeTrangXemConfig,(String)_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DanhSachToaDo.size());
      GuiTextLenHMI(_VPTextTotalFillTubeTrangXemConfig,"0");
      GuiTextLenHMI(_VpTextPumpControlTrangXemConfig,(String)_ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].thetich);
    }
  }
  if(codeHMI == _HMIBamNutResetTrangXemconfig)
  {
    _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].DanhSachToaDo.clear();
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
void DWINHMI::Debug(vector<chuongtrinh_t> data)
{
  ESP_LOGI("DULIEUHMI","Kich thuoc chuong trinh: %d",data.size()); 
  for(int i = 0; i < data.size(); i++)
  {
    ESP_LOGI("DULIEUHMI","Ten chuong trinh: %s",data[i].tenchuongtrinh.c_str());
    ESP_LOGI("DULIEUHMI","Trang thai chuong trinh: %d",data[i].TrangThaiChuongTrinh);  
    ESP_LOGI("DULIEUHMI","So ong phan phoi: %d",data[i].DanhSachToaDo.size());  
  }
}
//=============================================================================================================================
void DWINHMI::CapNhatHienThiThongTinChiTietChuongTrinh()
{
    chuongtrinh_t chuongtrinhxuly = _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy];
    String program = String(chuongtrinhxuly.tenchuongtrinh);
    GuiTextLenHMI(_VPTextTenChuongTrinhTrangThietLapChuongTrinh, program);
    String Volume = String(chuongtrinhxuly.thetich);
    String DKOng = String(chuongtrinhxuly.duongkinhong);
    String PumpSpeed = String(chuongtrinhxuly.tocdobom);
    String Antidrop = String(chuongtrinhxuly.chongnhogiot);
    String rackX = String(chuongtrinhxuly.rackX);
    String rackY = String(chuongtrinhxuly.rackY);
    String PositionAx = String(chuongtrinhxuly.vitriA.X);
    String PositionAy = String(-chuongtrinhxuly.vitriA.Y);
    String PositionBx = String(chuongtrinhxuly.vitriB.X);
    String PositionBy = String(-chuongtrinhxuly.vitriB.Y);
    String PositionCx = String(chuongtrinhxuly.vitriC.X);
    String PositionCy = String(-chuongtrinhxuly.vitriC.Y);
    String heightH = String(-chuongtrinhxuly.docaoH);
    String SpeedXY = String(chuongtrinhxuly.tocdoXY);
    String data = "Volume: " + Volume + "ml, " + "Tubing ID: " + DKOng + " mm\r\n" +
      "Pump speed: " + PumpSpeed + " rpm, " + "Antidrop: " + Antidrop + " degree\r\n" +
      "Rack " + rackX + "x" + rackY + " tube\r\n" +
      "Position A: " + "X=" + PositionAx + " Y=" + PositionAy + "\r\n" +
      "Position B: " + "X=" + PositionBx + " Y=" + PositionBy + "\r\n" +
      "Position C: " + "X=" + PositionCx + " Y=" + PositionCy + "\r\n" +
      "Fill head height H:" + heightH + ", " + "Speed run: " + SpeedXY + " mm/min";
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
    String data = name + "\r\n" + "Quatity: " + rackX + "x" + rackY + "\r\n" + "Volume: " + Volume + " ml\r\n" +
      "Pump speed : " + PumpSpeed + " rpm\r\n" + "Tube ID: " + DKOng + " mm\r\n" + "Speed XY: " + SpeedXY + " mm/min";
    GuiTextLenHMI(_VPTextHienThiChuongTrinhTrangChonChuongTrinh, data);

}
void DWINHMI::CapNhatHienThiThongTinTenChuongTrinhKhiVaoTrangChonChuongTrinh()
{
  for (uint8_t i = 0; i < 10; i++) 
  {
    GuiTextLenHMI(_VPTextProgram1TrangChonChuongTrinh + (i % _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh) * 0x100,
    _ChuongTrinhEEPROM[i % _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh].tenchuongtrinh);
  }
  GuiTextLenHMI(_VPTextHienThiChuongTrinhTrangChonChuongTrinh, "");
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
      stringdangxuly ="";

    if(codeHMI == _HMIBamNutChuyenTrangSoTrangBanPhim)
    {
      if(DWIN::getPage() == _TrangBanPhimMoRong)
        DWIN::setPage(_TrangBanPhimMoRongSo);
      else
        DWIN::setPage(_TrangBanPhimMoRong);
      codeHMI = -1;
    }

    if( (codeHMI & 0xFF00) == 0xFF00 && codeHMI != -1)
    {
      if(codeHMI == 0xFF7F)
        stringdangxuly.remove(stringdangxuly.length()-1);
      else
        stringdangxuly += (char)codeHMI;
      GuiTextLenHMI(_VPTextDataTrangBanPhim,stringdangxuly);
      codeHMI = -1;
    }
    if( codeHMI == _HMIBamNutEnterTrangBanPhimMoRong)
    {
      DWIN::setPage(_TrangThongSoChuongTrinh);
      _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy].tenchuongtrinh = stringdangxuly;
      GuiTextLenHMI(_VPTextTenChuongTrinhTrangThietLapChuongTrinh,stringdangxuly);
      //codeHMI = -1;
    }
    if(codeHMI == _HMIBamNutReturnTrangBanPhimMoRong)
    {
      DWIN::setPage(_TrangThongSoChuongTrinh);
      codeHMI = -1;
    }
  }
}

void DWINHMI::CapNhatHienThiTenCacChuongTrinhTrangCaiDatCheDoChuongTrinh()
{
    for (int i = 0; i < 5; i++)
    {
      //_ChuongTrinhEEPROM.push_back(DuLieuBoardGuiLenHMI.DSChuongTrinh[i]);
      GuiTextLenHMI(_VPTextProgram1TrangChonCheDo + i * 0x100, _ChuongTrinhEEPROM[DSIndexChuongTrinhHienThi[i]].tenchuongtrinh);
    }
}

  void DWINHMI::HienThiMau(uint32_t VpAddress, uint16_t X, uint16_t Y, uint16_t Mau, chuongtrinh_t ChuongTrinh){
  uint8_t data[240]; /* 24 * 10 */
  uint16_t KhayDuocChon[12][8];
  uint16_t toadoXIcondautien;
  uint16_t toadoYIcondautien;
  uint16_t chieudaiX = ChieuDaiX;
  uint16_t chieudaiY = ChieuDaiY;
  #ifdef debug_dwin
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
    toadoXIcondautien = ToaDoX0Khay(0) + 20;
    toadoYIcondautien = ToaDoY0Khay + 20;
    khay1[X][Y] = Mau;
    for (uint8_t i = 0; i < 12; i++){
      for (uint8_t j = 0; j < 8; j++){
        KhayDuocChon[i][j] = khay1[i][j];
      }
    }
  }
  else if(VpAddress >= _VpAddressVung1Khay2HienThiMau && VpAddress <= _VpAddressVung4Khay2HienThiMau)
  {
    toadoXIcondautien = ToaDoX0Khay(1) + 20;
    toadoYIcondautien = ToaDoY0Khay + 20;
    khay2[X][Y] = Mau;
    for (uint8_t i = 0; i < 12; i++){
      for (uint8_t j = 0; j < 8; j++){
        KhayDuocChon[i][j] = khay2[i][j];
      }
    }
  }
  else if(VpAddress >= _VpAddressVung1Khay3HienThiMau && VpAddress <= _VpAddressVung4Khay3HienThiMau)
  {
    toadoXIcondautien = ToaDoX0Khay(2) + 20;
    toadoYIcondautien = ToaDoY0Khay + 20;
    khay3[X][Y] = Mau;
    for (uint8_t i = 0; i < 12; i++){
      for (uint8_t j = 0; j < 8; j++){
        KhayDuocChon[i][j] = khay3[i][j];
      }
    }
  }
  else if(VpAddress >= _VpAddressVung1Khay4HienThiMau && VpAddress <= _VpAddressVung4Khay4HienThiMau)
  {
    toadoXIcondautien = ToaDoX0Khay(3) + 20;
    toadoYIcondautien = ToaDoY0Khay + 20;
    khay4[X][Y] = Mau;
    for (uint8_t i = 0; i < 12; i++){
      for (uint8_t j = 0; j < 8; j++){
        KhayDuocChon[i][j] = khay4[i][j];
      }
    }
  }
  else if(VpAddress >=  _VpAddressVung1Khay5HienThiMau && VpAddress <= _VpAddressVung4Khay5HienThiMau)
  {
    toadoXIcondautien = ToaDoX0Khay(4) + 20;
    toadoYIcondautien = ToaDoY0Khay + 20;
    khay5[X][Y] = Mau;
    for (uint8_t i = 0; i < 12; i++){
      for (uint8_t j = 0; j < 8; j++){
        KhayDuocChon[i][j] = khay5[i][j];
      }
    }
  }

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
  
  uint16_t KhoangCachGiua2DiemTheoX = chieudaiX / (ChuongTrinh.rackX - 0);
  uint16_t KhoangCachGiua2DiemTheoY = chieudaiY / (ChuongTrinh.rackY - 0);

  #pragma endregion KhayDuocChon
  for (uint8_t i = 0; i < 240; i+=10)
  {
    
    uint8_t ToaDoXMaTran = ((i / 10) % 12);
    uint8_t ToaDoYMaTran = ((i / 10) / 12) + OffsetToaDo;

    uint16_t ToaDoXHMI = 0;
    uint16_t ToaDoYHMI = 0;

    if (ChuongTrinh.rackX == 1U)
    {
      ToaDoXHMI = toadoXIcondautien + chieudaiX / 2;
      KhoangCachGiua2DiemTheoX = chieudaiX / 2;
    }
    else
    {
      ToaDoXHMI = toadoXIcondautien + KhoangCachGiua2DiemTheoX * ToaDoXMaTran;
    }
    
    if (ChuongTrinh.rackY == 1){
      ToaDoYHMI = toadoYIcondautien + chieudaiY / 2 - 30;
      KhoangCachGiua2DiemTheoY = chieudaiY / 2;
    }
    else{
      ToaDoYHMI = toadoYIcondautien + KhoangCachGiua2DiemTheoY * ToaDoYMaTran;
    }
    
    bool include = false;
    for (uint8_t k = 0; k < ChuongTrinh.DanhSachToaDo.size(); k++){
      if (ChuongTrinh.DanhSachToaDo[k].X == ToaDoXMaTran && ChuongTrinh.DanhSachToaDo[k].Y == ToaDoYMaTran){
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

      uint16_t ToaDoXHMITrenTrai = ToaDoXHMI - (KhoangCachGiua2DiemTheoX / 4);
      uint16_t ToaDoXHMIDuoiPhai = ToaDoXHMI + (KhoangCachGiua2DiemTheoX / 4);

      uint16_t ToaDoYHMITrenTrai = ToaDoYHMI - (KhoangCachGiua2DiemTheoY / 4);
      uint16_t ToaDoYHMIDuoiPhai = ToaDoYHMI + (KhoangCachGiua2DiemTheoY / 4);

      data[i    ] = ToaDoXHMITrenTrai >> 8;
      data[i + 1] = ToaDoXHMITrenTrai & 0xFF;

      data[i + 2] = ToaDoYHMITrenTrai >> 8;
      data[i + 3] = ToaDoYHMITrenTrai & 0xFF;

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

  #ifdef debug_dwin
  for (uint8_t i = 0; i < 156; i++){
    // Serial.printf("%02X ", buffer[i]);
  }
  // Serial.println();
  #endif
}
void DWINHMI::ResetFullMauChonToaDo(uint16_t Mau) {
  chuongtrinh_t ChuongTrinhDuocChon = _ChuongTrinhEEPROM[IndexChuongTrinhDangXuLy];
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
  chuongtrinh_t ChuongTrinhDuocChon = _ChuongTrinhEEPROM[IdxChuongTrinhDuocChon];

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
    case 2:
      for (uint8_t i = 0; i < 12; i++){
        for (uint8_t j = 0; j < 8; j++){
          khay3[i][j] = Mau;
        }
      }
      HienThiMau(_VpAddressVung1Khay3HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
      HienThiMau(_VpAddressVung2Khay3HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
      HienThiMau(_VpAddressVung3Khay3HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
      HienThiMau(_VpAddressVung4Khay3HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
      break;
    case 3:
      for (uint8_t i = 0; i < 12; i++){
        for (uint8_t j = 0; j < 8; j++){
          khay4[i][j] = Mau;
        }
      }
      HienThiMau(_VpAddressVung1Khay4HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
      HienThiMau(_VpAddressVung2Khay4HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
      HienThiMau(_VpAddressVung3Khay4HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
      HienThiMau(_VpAddressVung4Khay4HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
      break;
    case 4:
      for (uint8_t i = 0; i < 12; i++){
        for (uint8_t j = 0; j < 8; j++){
          khay5[i][j] = Mau;
        }
      }
      HienThiMau(_VpAddressVung1Khay5HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
      HienThiMau(_VpAddressVung2Khay5HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
      HienThiMau(_VpAddressVung3Khay5HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
      HienThiMau(_VpAddressVung4Khay5HienThiMau, 0, 0, Mau, ChuongTrinhDuocChon);
      break;
    default:
      break;
  }

}

void DWINHMI::VeToanBoHinhTron(uint32_t VpAddress, chuongtrinh_t ChuongTrinh)
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

  uint16_t KhoangCachGiua2DiemTheoX = chieudaiX / (ChuongTrinh.rackX - 0);
  uint16_t KhoangCachGiua2DiemTheoY = chieudaiY / (ChuongTrinh.rackY - 0);
  // Serial.println(dodaiXcuaicon);
  // Serial.println(dodaiYcuaicon);
  uint8_t data[240]; /* 24 * 10 */
  memset(data, 0, 240);
  Serial.printf("Size vitrivatoado trước khi push_back: %d\n",vitrivatoado.size());
  Serial.printf("ChuongTrinh: %s, rackX: %d, rackY: %d, size DSTD: %d\n",ChuongTrinh.tenchuongtrinh,ChuongTrinh.rackX,ChuongTrinh.rackY,ChuongTrinh.DanhSachToaDo.size());
  for (uint16_t i = 0; i < 240; i+=10)
  {

    uint8_t ToaDoXMaTran = ((i / 10) % 12);
    uint8_t ToaDoYMaTran = ((i / 10) / 12) + OffsetToaDo;

    uint16_t ToaDoXHMI = 0;
    uint16_t ToaDoYHMI = 0;

    if (ChuongTrinh.rackX == 1)
    {
      ToaDoXHMI = toadoXIcondautien + chieudaiX / 2;
      KhoangCachGiua2DiemTheoX = chieudaiX / 2;
    }
    else
    {
      ToaDoXHMI = toadoXIcondautien + KhoangCachGiua2DiemTheoX * ToaDoXMaTran;
    }

    if (ChuongTrinh.rackY == 1)
    {
      ToaDoYHMI = toadoYIcondautien + chieudaiY / 2 - 30;
      KhoangCachGiua2DiemTheoY = chieudaiY / 2;
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
      #ifdef debug_dwin
      // Serial.println("^^^^^^^^^^^^^^^^^^");
      #endif

      uint16_t ToaDoXHMITrenTrai = ToaDoXHMI - (KhoangCachGiua2DiemTheoX / 4);
      uint16_t ToaDoXHMIDuoiPhai = ToaDoXHMI + (KhoangCachGiua2DiemTheoX / 4);

      uint16_t ToaDoYHMITrenTrai = ToaDoYHMI - (KhoangCachGiua2DiemTheoY / 4);
      uint16_t ToaDoYHMIDuoiPhai = ToaDoYHMI + (KhoangCachGiua2DiemTheoY / 4);

      data[i    ] = ToaDoXHMITrenTrai >> 8;
      data[i + 1] = ToaDoXHMITrenTrai & 0xFF;

      data[i + 2] = ToaDoYHMITrenTrai >> 8;
      data[i + 3] = ToaDoYHMITrenTrai & 0xFF;

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

  this->KichThuocHcnDaVeTheoX = KhoangCachGiua2DiemTheoX / 4;
  this->KichThuocHcnDaVeTheoY = KhoangCachGiua2DiemTheoY / 4;

  #ifdef debug_dwin
  // for (uint8_t i = 0; i < 252; i++){
  //   // Serial.printf("%02X ", buffer[i]);
  // }
  // Serial.println();
  #endif
}

void DWINHMI::XuLyThoiGianThuc()
{

  if(codeHMI == _HMIBamNutTimeTrangCaiDat)
  {
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


void DWINHMI::xTaskFunction_RunTime(void *param)
{
  DWINHMI *dwinhmiclass = static_cast<DWINHMI*>(param);
  uint32_t time1s = millis();
  int32_t timeloopdelay;
  uint32_t seconds,minutes,hours;

  while(1)
  {
    if (millis() - time1s >= 1000)
    {
      if(dwinhmiclass->delay_flag == true)
      {
        timeloopdelay-=1000;
        seconds = timeloopdelay / 1000; // Tính số giây
        minutes = seconds / 60; // Tính số phút
        hours = minutes / 60; // Tính số giờ

        // Tạo chuỗi thời gian
        dwinhmiclass->timeLoopDelay_c = twoDigits(hours % 24) + ":" + twoDigits(minutes % 60) + ":" + twoDigits(seconds % 60);
        dwinhmiclass->GuiTextLenHMI(_VPTextDelayTimeRunTrangHienThiPhanPhoiThoiGianThuc, String(dwinhmiclass->timeLoopDelay_c));
      }
      else if (dwinhmiclass->delay_flag == false)
      {
        dwinhmiclass->TimeNow += 1000;  
        // Chuyển đổi thời gian thành giờ, phút, giây
        seconds = dwinhmiclass->TimeNow / 1000; // Tính số giây
        minutes = seconds / 60; // Tính số phút
        hours = minutes / 60; // Tính số giờ

        // Tạo chuỗi thời gian
        dwinhmiclass->TimeRunStart_c = twoDigits(hours % 24) + ":" + twoDigits(minutes % 60) + ":" + twoDigits(seconds % 60);
        dwinhmiclass->GuiTextLenHMI(_VPTextTimeRunTrangHienThiPhanPhoiThoiGianThuc, String(dwinhmiclass->TimeRunStart_c));
      }
      time1s = millis();
    }

    chuongtrinh_t& chuongtrinhdangxuly = _grbl._DSChuongTrinh[dwinhmiclass->DSIndexChuongTrinhDuocChon[_indexdsmulti]];
    
    static int8_t toadotruoc = -1;

    if (chuongtrinhdangxuly.ToaDoHienTai != -1 && dwinhmiclass->delay_flag == false)
    {
      if(toadotruoc != chuongtrinhdangxuly.ToaDoHienTai)
      {
        dwinhmiclass->GuiTextLenHMI(_VPTextFillNumberTrangHienThiPhanPhoiThoiGianThuc, String(chuongtrinhdangxuly.ToaDoHienTai + 1));
        uint16_t x = chuongtrinhdangxuly.DanhSachToaDo[chuongtrinhdangxuly.ToaDoHienTai].X;
        uint16_t y = chuongtrinhdangxuly.DanhSachToaDo[chuongtrinhdangxuly.ToaDoHienTai].Y;
#ifdef debug_dwin
        Serial.printf("ChuongTrinh: %s, X: %d, Y: %d, ToaDoTruoc: %d, ToaDoHienTai: %d\n", chuongtrinhdangxuly.tenchuongtrinh.c_str(), x, y, toadotruoc, chuongtrinhdangxuly.ToaDoHienTai);
#endif
        toadotruoc = chuongtrinhdangxuly.ToaDoHienTai;
        dwinhmiclass->HienThiMau(_indexdsmulti, x, y, chuongtrinhdangxuly,MauXanhLa);
      }
    }
    if(dwinhmiclass->Mode == _CheDoHoatDongMulti)
    {
      /* Khi mà tọa độ hiện tại bằng với size của vector tọa độ -1 */
      //if (chuongtrinhdangxuly.ToaDoHienTai == dwinhmiclass->DuLieuBoardGuiLenHMI.DSChuongTrinh[dwinhmiclass->DSIndexChuongTrinhDuocChon[_indexdsmulti]].DanhSachToaDo.size() - 1) 
      if ((chuongtrinhdangxuly.ToaDoHienTai == _grbl._DSChuongTrinh[dwinhmiclass->DSIndexChuongTrinhDuocChon[_indexdsmulti]].DanhSachToaDo.size() - 1) || (_grbl._DSChuongTrinh[dwinhmiclass->DSIndexChuongTrinhDuocChon[_indexdsmulti]].DanhSachToaDo.size() == 0))
      {
#ifdef debug_dwin
        Serial.printf("Phân phối xong ChuongTrinh: %s, ToaDoHienTai: %d\n", chuongtrinhdangxuly.tenchuongtrinh.c_str(), chuongtrinhdangxuly.ToaDoHienTai);
#endif
        chuongtrinhdangxuly.ToaDoHienTai = -1; 
        toadotruoc = -1;
        _indexdsmulti++;
        dwinhmiclass->GuiTextLenHMI(_VPTextTenChuongTrinhDangChay, (String)(_indexdsmulti + 1));
        if(_indexdsmulti == dwinhmiclass->DSIndexChuongTrinhDuocChon.size())
        {
          dwinhmiclass->GuiTextLenHMI(_VPTextTenChuongTrinhDangChay, (String)(_indexdsmulti));
          if(dwinhmiclass->oneTimeMulti == false) //trường hợp chạy MULTI nhiều lần
          {
            dwinhmiclass->delay_flag = true;
            _indexdsmulti = 0;
            timeloopdelay = dwinhmiclass->timeLoopDelay;
          }
          else                                    //trường hợp chạy MULTI 1 lần
          {
            _indexdsmulti = 0;
            for (uint8_t i = 0; i < dwinhmiclass->DSIndexChuongTrinhDuocChon.size(); i++)
            {
              dwinhmiclass->ResetFullMau(dwinhmiclass->DSIndexChuongTrinhDuocChon[i], MauDo, i);
            }
            dwinhmiclass->setPage(_TrangHienThiPhanPhoiThoiGianThuc);
            dwinhmiclass->BamNutRunON = false;
            vTaskSuspend(NULL);
          }
        }
      }
      if (dwinhmiclass->delay_flag)
      {
        if (timeloopdelay <= 0)
        {

          seconds = (dwinhmiclass->timeLoopDelay) / 1000; // Tính số giây
          minutes = seconds / 60; // Tính số phút
          hours = minutes / 60; // Tính số giờ
          // Tạo chuỗi thời gian
          dwinhmiclass->timeLoopDelay_c = twoDigits(hours % 24) + ":" + twoDigits(minutes % 60) + ":" + twoDigits(seconds % 60);
          dwinhmiclass->GuiTextLenHMI(_VPTextDelayTimeRunTrangHienThiPhanPhoiThoiGianThuc, String(dwinhmiclass->timeLoopDelay_c));

          dwinhmiclass->delay_flag = false;
          
          dwinhmiclass->GuiTextLenHMI(_VPTextTenChuongTrinhDangChay, "1");

          for (uint8_t i = 0; i < dwinhmiclass->DSIndexChuongTrinhDuocChon.size(); i++)
          {
            dwinhmiclass->ResetFullMau(dwinhmiclass->DSIndexChuongTrinhDuocChon[i], MauDo, i);
          }

          codeHMI = _HMIBamNutRUNTrangHienThiPhanPhoiThoiGianThuc;
          dwinhmiclass->BamNutRunON = false;
        }
      }
    }
    else
    {
      /* Khi mà tọa độ hiện tại bằng với size của vector tọa độ -1 */
      //if (chuongtrinhdangxuly.ToaDoHienTai == dwinhmiclass->DuLieuBoardGuiLenHMI.DSChuongTrinh[dwinhmiclass->DSIndexChuongTrinhDuocChon[_indexdsmulti]].DanhSachToaDo.size() - 1) 
      if (chuongtrinhdangxuly.ToaDoHienTai == _grbl._DSChuongTrinh[dwinhmiclass->DSIndexChuongTrinhDuocChon[_indexdsmulti]].DanhSachToaDo.size() - 1) 
      {
        // Serial.println("chuongtrinhdangxuly.ToaDoHienTai: " + String(chuongtrinhdangxuly.ToaDoHienTai));
        chuongtrinhdangxuly.ToaDoHienTai = -1;
        dwinhmiclass->setPage(_TrangHienThiPhanPhoiThoiGianThuc);
        dwinhmiclass->BamNutRunON = false;
        vTaskSuspend(NULL);

      }
    }
    delay(1);
  }
}

void DWINHMI::HienThiMau(int8_t SoKhay, uint16_t X, uint16_t Y, chuongtrinh_t ChuongTrinh,uint16_t Mau){
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
      HienThiMau(_VpAddressVung1Khay1HienThiMau, X, Y, Mau, ChuongTrinh);
      HienThiMau(_VpAddressVung2Khay1HienThiMau, X, Y, Mau, ChuongTrinh);
      HienThiMau(_VpAddressVung3Khay1HienThiMau, X, Y, Mau, ChuongTrinh);
      HienThiMau(_VpAddressVung4Khay1HienThiMau, X, Y, Mau, ChuongTrinh);
      break;
    case 1:
      HienThiMau(_VpAddressVung1Khay2HienThiMau, X, Y, Mau, ChuongTrinh);
      HienThiMau(_VpAddressVung2Khay2HienThiMau, X, Y, Mau, ChuongTrinh);
      HienThiMau(_VpAddressVung3Khay2HienThiMau, X, Y, Mau, ChuongTrinh);
      HienThiMau(_VpAddressVung4Khay2HienThiMau, X, Y, Mau, ChuongTrinh);
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

void DWINHMI::VeKhungHCN(uint8_t SoLuongKhay, uint16_t Mau){
  Serial.printf("SoLuongKhay: %d\n",SoLuongKhay);
  if (SoLuongKhay == 0 || SoLuongKhay > 5)
    return;

  uint8_t DataLength = 10 * SoLuongKhay + 9;
  uint8_t BufferSize = DataLength + 3;

  uint8_t buffer[BufferSize] = {0x5A, 0xA5, DataLength, 0x82, _VpKhungHCN >> 8U, _VpKhungHCN & 0xFF, 0x00, 0x03, 0x00, SoLuongKhay};

  uint8_t* ptr = buffer + 10;
  for (uint8_t i = 0; i < SoLuongKhay; i++){
    *ptr++ = ToaDoX0Khay(i) >> 8U;
    *ptr++ = ToaDoX0Khay(i) & 0xFF; //* upper left X0
    *ptr++ = ToaDoY0Khay >> 8U;     //* upper left Y0
    *ptr++ = ToaDoY0Khay & 0xFF;

    *ptr++ = ToaDoX1Khay(i) >> 8U;
    *ptr++ = ToaDoX1Khay(i) & 0xFF; //* lower right X0
    *ptr++ = ToaDoY1Khay >> 8U;     //* lower right Y0
    *ptr++ = ToaDoY1Khay & 0xFF;

    *ptr++ = Mau >> 8U;
    *ptr++ = Mau & 0xFF;
  }

  *ptr++ = 0xFF;
  *ptr = 0x00;

#ifdef debug_dwin
  for (uint8_t i = 0; i < BufferSize; i++){
    Serial.printf("%02X ", buffer[i]);
  }
  Serial.println();
#endif

  DGUS_SERIAL.write(buffer, BufferSize);
  DGUS_SERIAL.println("OK");

}

void DWINHMI::VeKhungHCN(uint32_t VpAddress, uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Mau){
  uint8_t buffer[] = {0x5A, 0xA5, 0x13, 0x82, VpAddress >> 8, VpAddress & 0xFF, 0x00, 0x03, 0x00, 0x01, X0 >> 8, X0 & 0xFF, Y0 >> 8, Y0 & 0xFF, X1 >> 8, X1 & 0xFF, Y1 >> 8, Y1 & 0xFF, Mau >> 8, Mau & 0xFF, 0xFF, 0x00};
  DGUS_SERIAL.write(buffer, sizeof(buffer));
  DGUS_SERIAL.println("OK");
}

void DWINHMI::CapNhatLaiDanhSachToaDo(chuongtrinh_t & ChuongTrinh)
{
  ChuongTrinh.DanhSachToaDo.clear();
  for (uint8_t x = 0; x < ChuongTrinh.rackX; x++)
  {
    for (uint8_t y = 0; y < ChuongTrinh.rackY; y++)
    {
      ChuongTrinh.DanhSachToaDo.push_back({x, y});
    }
  }
}

void DWINHMI::HienThiNoiDungChuongTrinh(void)
{

  uint8_t IdxChuongTrinhDuocChonDauTien = DSIndexChuongTrinhDuocChon[0];

  Serial.println("IdxChuongTrinhDuocChonDauTien: " + String(IdxChuongTrinhDuocChonDauTien));
  Serial.println("DSIndexChuongTrinhDuocChon[IdxChuongTrinhDuocChonDauTien]: " + String(DSIndexChuongTrinhHienThi[IdxChuongTrinhDuocChonDauTien]));
  String name = String(_ChuongTrinhEEPROM[DSIndexChuongTrinhHienThi[IdxChuongTrinhDuocChonDauTien]].tenchuongtrinh);
  String rackX = String(_ChuongTrinhEEPROM[DSIndexChuongTrinhHienThi[IdxChuongTrinhDuocChonDauTien]].rackX);
  String rackY = String(_ChuongTrinhEEPROM[DSIndexChuongTrinhHienThi[IdxChuongTrinhDuocChonDauTien]].rackY);
  String Volume = String(_ChuongTrinhEEPROM[DSIndexChuongTrinhHienThi[IdxChuongTrinhDuocChonDauTien]].thetich);
  String PumpSpeed = String(_ChuongTrinhEEPROM[DSIndexChuongTrinhHienThi[IdxChuongTrinhDuocChonDauTien]].tocdobom);
  String DKOng = String(_ChuongTrinhEEPROM[DSIndexChuongTrinhHienThi[IdxChuongTrinhDuocChonDauTien]].duongkinhong);
  String SpeedXY = String(_ChuongTrinhEEPROM[DSIndexChuongTrinhHienThi[IdxChuongTrinhDuocChonDauTien]].tocdoXY);
  String data = "Quatity: " + rackX + "x" + rackY + "\r\n" + "Volume: " + Volume + " ml\r\n" +
    "Pump speed : " + PumpSpeed + " rpm\r\n" + "Tube ID: " + DKOng + " mm\r\n" + "Speed XY: " + SpeedXY + " mm/min";
  GuiTextLenHMI(_VPTextHienThiTenProgramTrangCaiDatCheDoChuongTrinh, name);
  GuiTextLenHMI(_VPTextHienThiChuongTrinhTrangChonChuongTrinh, data);
}

