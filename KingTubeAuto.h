#ifndef _KingTubeAuto_h
#define _KingTubeAuto_h
#include "src/Grbl.h"
#include <vector>
using namespace std;

// #define DEBUG_THONGSOBOARD
// #define DEBUG_DWIN
#define DEBUG_GRBL
#define DEBUG_CHUONGTRINHCHINH

// #define RESET_EEPROM

// #define ABCH_LAB
#define ABCH_NHAMAY

// #define TEST_SENSORS


#define Serial                              Uart0

#define SoLuongChuongTrinhToiDa             10
#define SoLuongKhuVucToiDa                  2
#define SoLuongChuongTrinhCoDinh            5
#define _ChieuDaiMaxTenChuongTrinh          15

#define TrucX                               X_AXIS
#define TrucY                               Y_AXIS
#define TrucZ                               Z_AXIS
#define MayBom                              A_AXIS

#define Duong                               true
#define Am                                  false

#define TocDoCao                            DEFAULT_X_MAX_RATE           
#define TocDoTrungBinh                      DEFAULT_X_MAX_RATE / 2
#define TocDoThap                           DEFAULT_X_MAX_RATE / 4

#define ToaDoGoc                            DEFAULT_HOMING_PULLOFF, -DEFAULT_HOMING_PULLOFF, -DEFAULT_HOMING_PULLOFF

#define MatKhau                             "123456"

#define HeSoCalibMacDinh 403.041 // ul/duongkinhong

#define X2_SENSOR_PIN            39 //limit 2        //cảm biến hành trình cuối trục X
#define Y2_SENSOR_PIN            35 //limit 4     //cảm biến Y2 hiên tại đang bị hư nên không dùng
#define Z2_SENSOR_PIN            33

typedef struct {
    uint8_t X;
    uint8_t Y;
}toadoINT_t;

typedef struct {
    float X;
    float Y;
}toado_t;

typedef struct {
    String tenchuongtrinh;

    float thetich = 10.0; //ml
    float duongkinhong = 1.6; //mm
    uint16_t tocdobom = 500; //vòng/phút
    uint16_t tocdoXY = TocDoCao; //vòng/phút

    float chongnhogiot = 45; //độ
    uint8_t rackX = 11;
    uint8_t rackY = 8;

#ifdef ABCH_LAB
    toado_t ViTriA[SoLuongKhuVucToiDa] = { { 20.0, -25.0 },     { 283.0, -25.0 } };
    toado_t ViTriB[SoLuongKhuVucToiDa] = { { 250.0, -25.0 },    {513.0, -25.0 } };
    toado_t ViTriC[SoLuongKhuVucToiDa] = { { 250.0, -187.0 },   {513.0, -187.0 } };
    float ViTriH[SoLuongKhuVucToiDa] = {-400.0,                -400.0};
#endif

#ifdef ABCH_NHAMAY
    toado_t ViTriA[SoLuongKhuVucToiDa] = { { 32.0, -17.0 },     { 297.0, -17.0 } };
    toado_t ViTriB[SoLuongKhuVucToiDa] = { { 263.0, -17.0 },    {527.0, -17.0 } };
    toado_t ViTriC[SoLuongKhuVucToiDa] = { { 263.0, -180.0 },   {527.0, -180.0 } };
    float ViTriH[SoLuongKhuVucToiDa] = {-105.0,                -105.0};
#endif

    vector<toadoINT_t> DsToaDo[SoLuongKhuVucToiDa];

    float actualweight = 1.0;
    float density = 1.0;
    // float SoVongDeBom1ml = 0.521875;
    float TheTichBomDuocKhiQuay1Vong = HeSoCalibMacDinh; // ul/duongkinhong
}ChuongTrinh_t;

typedef enum {
    RESETMALENH,
    RUN,
    PAUSE,
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
    GOA1,
    GOA2,
    GOB1,
    GOB2,
    GOC1,
    GOC2,
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
    SHUTDOWN,
    RESET,
}DanhSachMaLenh_t;


#endif