#ifndef _ChuongTrinhChinh_h
#define _ChuongTrinhChinh_h

#include "01_Flags.h"
#include "02_WIFI.h"
#include "03_POSTGET.h"
#include "04_Firmware.h"
#include "05_ThongSoBoard.h"
#include "06_RTC.h"
#include "10_DWINHMI.h"
#include "_GRBL.h"
#include "KingTubeAuto.h"

#define _ChanDocNguon 15
#define _ChanKichNguon 12

void ChayChuongTrinhChinh(void);
void ThucThiCacTacVuTheoFlag(void);
void KhoiTao(void);
void CapNhatDuLieuLenHMI(void);
void XuLyDuLieuNhanTuHMI(void);
void DWINCapNhatDSChuongTrinhBanDau();
void CapNhatDanhSachChuongTrinhLuuTrongEEPROM();
JSONVar ChuyenDoiChuongTrinhSangJsonVar(vector<chuongtrinh_t> DsChuongTrinhCanChuyenDoi);
chuongtrinh_t ChuyenDoiJSONVarSangChuongTrinh(JSONVar data,uint8_t idxchuongtrinh);

void CaiDatNguon(void);
void KiemTraNguon(void);

#endif