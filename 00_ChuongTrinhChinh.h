#ifndef _ChuongTrinhChinh_h
#define _ChuongTrinhChinh_h

#include "01_Flags.h"
#include "04_Firmware.h"
#include "05_ThongSoBoard.h"
#include "06_RTC.h"
#include "10_DWINHMI.h"
#include "_GRBL.h"
#include "KingTubeAuto.h"

#define X2_SENSOR_PIN            39 //limit 2        //cảm biến hành trình cuối trục X
#define Y2_SENSOR_PIN            35 //limit 4     //cảm biến Y2 hiên tại đang bị hư nên không dùng
#define Z2_SENSOR_PIN            33

#define _ChanDocNguon 15
#define _ChanKichNguon 12

void ChayChuongTrinhChinh(void);
void ThucThiCacTacVuTheoFlag(void);
void KhoiTao(void);
void CapNhatDuLieuLenHMI(void);
void XuLyDuLieuNhanTuHMI(void);
void DWINCapNhatDSChuongTrinhBanDau();
void CapNhatDanhSachChuongTrinhLuuTrongEEPROM();

void CaiDatNguon(void);
void KiemTraNguon(void);

void Shutdown(void);

#endif