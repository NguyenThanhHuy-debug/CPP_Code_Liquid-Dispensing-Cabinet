#pragma once
#ifndef _GRBL_H _
#define _GRBL_H_

#include "KingTubeAuto.h"
#include <limits>

// #define debug_grbl

#define DoUuTienGRBL      3
#define DoUuTienPhanPhoi  2

#define TheTichBomDuocKhiQuay1VongMacDinh 187.125748503 // ul/duongkinhong

double KhoangCachGiua2Diem(const toadoINT_t& Diem1, const toadoINT_t& Diem2);
vector<uint8_t> ThuatToanTimDuongDiNganNhat(const vector<toadoINT_t>& DanhSachToaDo);

class _GRBL {
public:
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

    vector<ChuongTrinh_t> _DSChuongTrinh;

    void TamDungChay(void);

    void DiChuyenToiViTri(float X, float Y, float Z, uint16_t TocDo);

    void Calib(ChuongTrinh_t * ChuongTrinh);

    void TestFill(ChuongTrinh_t ChuongTrinh);

    void VeToaDoGoc(void);

    int8_t KhuVucDangChay = -1;
    int8_t OngDaPhanPhoi = -1;
    uint8_t ToaDoXDaChay;
    uint8_t ToaDoYDaChay;
    bool DaChayXongCacKhuVuc = false;
    int8_t KhuVucChay[SoLuongKhuVucToiDa];

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
