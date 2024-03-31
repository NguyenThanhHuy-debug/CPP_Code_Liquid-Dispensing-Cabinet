#ifndef _KingTubeAuto_h
#define _KingTubeAuto_h
#include "src/Grbl.h"
#include <vector>
using namespace std;

#define Serial                              Uart0

#define SoLuongChuongTrinhToiDa             10
#define SoLuongKhuVucToiDa                  2
#define SoLuongChuongTrinhCoDinh            5

#define TrucX                               X_AXIS
#define TrucY                               Y_AXIS
#define TrucZ                               Z_AXIS
#define MayBom                              A_AXIS

#define Duong                               true
#define Am                                  false

#define TocDoCao                            DEFAULT_X_MAX_RATE           
#define TocDoTrungBinh                      DEFAULT_X_MAX_RATE / 2
#define TocDoThap                           DEFAULT_X_MAX_RATE / 4

#define FIXED                               true
#define ADJ                                 false

#define ToaDoGoc                            DEFAULT_HOMING_PULLOFF, -DEFAULT_HOMING_PULLOFF, -DEFAULT_HOMING_PULLOFF

#define MatKhau                             "000000"

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

    float thetich = 5.0; //ml
    float duongkinhong = 3.2; //mm
    uint16_t tocdobom = TocDoTrungBinh; //vòng/phút
    uint16_t tocdoXY = TocDoTrungBinh; //vòng/phút

    float chongnhogiot = 90; //độ
    uint8_t rackX = 12;
    uint8_t rackY = 8;

    toado_t ViTriA[SoLuongKhuVucToiDa] = { { 21.0, -21.0 },     { 300.0, -21.0 } };
    toado_t ViTriB[SoLuongKhuVucToiDa] = { { 274.0, -21.0 },    {500.0, -21.0 } };
    toado_t ViTriC[SoLuongKhuVucToiDa] = { { 274.0, -181.0 },   { 500.0, -181.0 } };
    float ViTriH[SoLuongKhuVucToiDa] = {-380.0,                -380.0};
    
    vector<toadoINT_t> DsToaDo[SoLuongKhuVucToiDa];

    float actualweight = 1.0;
    float density = 1.0;
    // float SoVongDeBom1ml = 0.521875;
    float TheTichBomDuocKhiQuay1Vong = 187.125748503; // ul/duongkinhong
    bool type = ADJ;
}ChuongTrinh_t;

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
    SHUTDOWN,
    RESET,
}DanhSachMaLenh_t;


#endif

  

// Text tên AREA1: 0x9100 (hiện tên chương trình của AREA ở trang đầu tiên)
//               AREA2: 0x9200

// Text tên Chương Trình đang xử lý: 0x9500

// Nút Chọn CT1: 0x200 (xem thông tin ở trang đầu tiên)
//                  CT2: 0x201
//                  CT3: 0x202
//                  CT4: 0x203
//                  CT5: 0x204
//                  CT6: 0x205
//                  CT7: 0x206
//                  CT8: 0x207
//                  CT9: 0x208
//                  CT10: 0x209

// VP Icon     CT1: 0x3300 (icon của chương trình được chọn để hiển thị ở trang đầu tiên
//                  CT2: 0x3301
//                  CT3: 0x3302
//                  CT4: 0x3303
//                  CT5: 0x3304
//                  CT6: 0x3305
//                  CT7: 0x3306
//                  CT8: 0x3307
//                  CT9: 0x3308
//                  CT10: 0x3309

// VP tên CT1->CT10: 0x8000->0x8900 (dùng chung trang đầu tiên và trang chọn chương trình)

// Trang đầu tiên ở vị trí HOME: 131, khác vị trí HOME: 170

// Trang RUN chưa chạy HOME: 134 , khác HOME: 175

// Nút AREA1: nút MODIFY1 cũ, nút AREA2: nút MODIFY2 cũ

// Tọa độ icon hiển thị: 130, 160
//                     chọn:      740, 50

// Chiều dài hiển thị X: 130 -> 415   = 285
//                      Y: 160 -> 330 = 170
// Chiều dài chọn X:    740 -> 1500  = 760
                        // 50 -> 350 = 300

                        