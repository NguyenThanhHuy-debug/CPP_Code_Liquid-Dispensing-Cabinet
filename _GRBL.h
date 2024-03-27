#pragma once
#ifndef _GRBL_H _
#define _GRBL_H_

#include "KingTubeAuto.h"
#include <limits>
#include <algorithm>

// #define debug_grbl

#define DoUuTienGRBL      3
#define DoUuTienPhanPhoi  2

#define SoLuongKhuVucToiDa 5

#define TheTichBomDuocKhiQuay1VongMacDinh 187.125748503 // ul/duongkinhong

//* API Step bơm
//* BatDauCalibBom();
//* TamDungCalibBom();
//* LuuThongSoCalibBom();

//* API Phan phoi

/*
*   Các biến giá trị:
*    Tạo ra 1 vector một program
*   - Struct Program:
*   + Volume
*   + Tubing ID
*   + Pump speed
*   + Antidrop
*   + Rack
*   + Postion A,B,c
*   + Fill head hight H
*   + Speed run
  */

double KhoangCachGiua2Diem(const toadoINT_t& Diem1, const toadoINT_t& Diem2);
vector<uint8_t> ThuatToanTimDuongDiNganNhat(const vector<toadoINT_t>& DanhSachToaDo);

class _GRBL {
public:
    void ResetChuongTrinh();
    bool VeViTriHome();
    void SapXepThuTuOng(vector<toadoINT_t>& DANHSACHTOADO);
    void BatDauPhanPhoi();
    float ViTriHienTaiCuaSTEP(uint8_t STEP);
    // void Calib();

    void DiChuyenX(bool Chieu, float Buoc, float TocDo);
    void DiChuyenY(bool Chieu, float Buoc, float TocDo);
    void DiChuyenZ(bool Chieu, float Buoc, float TocDo);

    void KhoiTaoGRBL(void);

    void PhanPhoi(void);
    void TamDungPhanPhoi(void);
    void TiepTucPhanPhoi(void);
    void DungPhanPhoi(void);

    void BatDauCalib(ChuongTrinh_t& ChuongTrinh);
    void DungCalib(ChuongTrinh_t& ChuongTrinh);
    void LuuCalib(ChuongTrinh_t& ChuongTrinh);
    vector<ChuongTrinh_t> _DSChuongTrinh;

    void TamDungChay(void);

    void DiChuyenToiViTri(float X, float Y, float Z, uint16_t TocDo);

    void Calib(ChuongTrinh_t * ChuongTrinh);

    void TestFill(ChuongTrinh_t ChuongTrinh);

    int8_t KhuVucDangChay = -1;
    int8_t OngDaPhanPhoi = -1;
    uint8_t RackXHienTai;
    uint8_t RackYHienTai;
    uint8_t ToaDoXDaChay;
    uint8_t ToaDoYDaChay;
    bool DaChayXongCacKhuVuc = false;

private:
    bool CoBaoDungPhanPhoi = false;

    toado_t _KhoangCachGiuaHaiOng;

    static void StaticGRBLTask(void* _this);
    void GRBLTask(void);

    static void StaticPhanPhoiTask(void* _this);
    void PhanPhoiTask(void);
    TaskHandle_t _PhanPhoiTaskHandle;

    void TinhKhoangCachGiuaHaiOng(ChuongTrinh_t ChuongTrinh, uint8_t KhuVuc);

};

extern _GRBL _grbl;

#endif // _GRBL_H_
