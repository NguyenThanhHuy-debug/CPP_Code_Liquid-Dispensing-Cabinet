#pragma once
#ifndef _GRBL_H _
#define _GRBL_H_

#include "src/Grbl.h"
#include <vector>
#include <limits>
using namespace std;

// #define debug_grbl

#define TrucX             X_AXIS
#define TrucY             Y_AXIS
#define TrucZ             Z_AXIS
#define MayBom            A_AXIS

#define Duong             true
#define Am                false

#define TocDoCao          DEFAULT_X_MAX_RATE           
#define TocDoTrungBinh    DEFAULT_X_MAX_RATE / 2
#define TocDoThap         DEFAULT_X_MAX_RATE / 4

#define SoBuoc            10 //* 10mm

#define DoUuTienGRBL      3
#define DoUuTienPhanPhoi  2

#define Serial            Uart0

#define DuocChon          true
#define KhongDuocChon     false

#define SoLuongChuongTrinhChayToiDa 5

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

typedef struct {
    uint8_t X;
    uint8_t Y;
}toadoINT_t;

typedef struct {
    double X;
    double Y;
}toado_t;

typedef struct {
    bool TrangThaiChuongTrinh = KhongDuocChon;
    String tenchuongtrinh;

    float thetich = 5.0; //ml
    float duongkinhong = 3.2; //mm
    uint16_t tocdobom = TocDoTrungBinh; //vòng/phút
    uint16_t tocdoXY = TocDoTrungBinh; //vòng/phút

    float chongnhogiot = 90; //độ
    uint8_t rackX = 12;
    uint8_t rackY = 8;

    toado_t vitriA = { 21.0, -21.0 };
    toado_t vitriB = { 274.0, -21.0 };
    toado_t vitriC = { 274.0, -181.0 }; 

    float docaoH = -380.0;
    int8_t ToaDoHienTai = -1;
    vector<toadoINT_t> DanhSachToaDo;

    double actualweight = 0;
    double density = 1;
    // float SoVongDeBom1ml = 0.521875;
    float TheTichBomDuocKhiQuay1Vong = 187.125748503; // ul/duongkinhong
    int8_t ThuTuChay = -1;
}chuongtrinh_t;

typedef enum {
    RESETMALENH,
    RUN,
    RESUME,
    GOHOMEKHONGCHUYENTRANG,
    GOHOME,
    CAPNHATCHUONGTRINH,
    RESETPROGRAM,
    EXITVETRANGCAIDATCHEDOCHUONGTRINH,
    CAPNHATTOADO,
    CONFIGCHUONGTRINH,
    CAPNHATTHONGSO,
    BAMNUTDICHUYENXCONG,
    BAMNUTDICHUYENXTRU,
    BAMNUTDICHUYENYCONG,
    BAMNUTDICHUYENYTRU,
    BAMNUTDICHUYENZCONG,
    BAMNUTDICHUYENZTRU,
    XACNHAN_A_B_C_H,
    TESTCALIB,
    CALIB,
    TESTFILL,
    DIRECTION,
    FASTPUMP,
    GOA,
    GOB,
    GOC,
    LOWSPEED,
    MEDIUMSPEED,
    FASTSPEED,
    UNLOCK,
    HOLDXTRU,
    HOLDXCONG,
    HOLDYTRU,
    HOLDYCONG,
    HOLDZTRU,
    HOLDZCONG,
    DUNGDICHUYEN,
    RESET,
}DanhSachMaLenh_t;
double KhoangCachGiua2Diem(const toadoINT_t& Diem1, const toadoINT_t& Diem2);
vector<uint8_t> ThuatToanTimDuongDiNganNhat(const vector<toadoINT_t>& DanhSachToaDo);

class _GRBL {
public:
    void ResetChuongTrinh();
    bool VeViTriHome();
    void SapXepThuTuOng(vector<toadoINT_t>& DANHSACHTOADO);
    void ChonTungChuongTrinh();
    void ChonChuongTrinh();
    void BatDauPhanPhoi();
    void LayChuongTrinhTuEEPROM();
    float ViTriHienTaiCuaSTEP(uint8_t STEP);
    // void Calib();

    void DiChuyenX(bool Chieu, float Buoc, float TocDo);
    void DiChuyenY(bool Chieu, float Buoc, float TocDo);
    void DiChuyenZ(bool Chieu, float Buoc, float TocDo);

    void KhoiTaoGRBL(void);

    void LayViTriA(chuongtrinh_t& ChuongTrinh);
    void LayViTriB(chuongtrinh_t& ChuongTrinh);
    void LayViTriC(chuongtrinh_t& ChuongTrinh);
    void LayDoCaoH(chuongtrinh_t& ChuongTrinh);

    void CaiDatTocDoTrucXY(chuongtrinh_t& ChuongTrinh, uint16_t TocDo);

    void CaiDatSoOngTrenMotHang(chuongtrinh_t& ChuongTrinh, uint8_t SoOngTrenMotHang);

    void CaiDatSoOngTrenMotCot(chuongtrinh_t& ChuongTrinh, uint8_t SoOngTrenMotCot);

    void CaiDatTheTichOng(chuongtrinh_t& ChuongTrinh, float TheTichOng);

    void CaiDatTocDoBom(chuongtrinh_t& ChuongTrinh, uint16_t TocDoBom);

    void CaiDatGocChongNhoGiot(chuongtrinh_t& ChuongTrinh, float GocChongNhoGiot);

    void PhanPhoi(void);
    void TamDungPhanPhoi(void);
    void TiepTucPhanPhoi(void);
    void DungPhanPhoi(void);

    void BatDauCalib(chuongtrinh_t& ChuongTrinh);
    void DungCalib(chuongtrinh_t& ChuongTrinh);
    void LuuCalib(chuongtrinh_t& ChuongTrinh);
    vector<chuongtrinh_t> _DSChuongTrinh;

    void TamDungChay(void);

    void DiChuyenToiViTri(float X, float Y, float Z, uint16_t TocDo);

    void Calib(chuongtrinh_t * ChuongTrinh);

    void TestFill(chuongtrinh_t ChuongTrinh);

private:
    bool CoBaoDungPhanPhoi = false;

    toado_t _KhoangCachGiuaHaiOng;

    static void StaticGRBLTask(void* _this);
    void GRBLTask(void);

    static void StaticPhanPhoiTask(void* _this);
    void PhanPhoiTask(void);
    TaskHandle_t _PhanPhoiTaskHandle;

    void TinhKhoangCachGiuaHaiOng(chuongtrinh_t& ChuongTrinh);

};

extern _GRBL _grbl;

#endif // _GRBL_H_
