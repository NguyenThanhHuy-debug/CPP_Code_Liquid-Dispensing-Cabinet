#ifndef _DWINHMI_h
#define _DWINHMI_h

#include "KingTubeAuto.h"
#include <Arduino.h>  
#include <DWIN.h>  

// #define debug_dwin

#define AddrPressXcong    "1010"
#define AddrHoldXcong     "1020"
#define AddrReleaseXcong  "1030"

#define AddrPressXtru     "1040"
#define AddrHoldXtru      "1050"
#define AddrReleaseXtru   "1060"


#define AddrPressYcong    "1070"
#define AddrHoldYcong     "1080"
#define AddrReleaseYcong  "1090"


#define AddrPressYtru     "2000"
#define AddrHoldYtru      "2010"
#define AddrReleaseYtru   "2020"


#define AddrPressZcong    "2030"
#define AddrHoldZcong     "2040"
#define AddrReleaseZcong  "2050"

#define AddrPressZtru     "2060"
#define AddrHoldZtru      "2070"
#define AddrReleaseZtru   "2080"

// #define OffsetIconOng   15
#define BanKinhIconOng  10U

#define KhoangCachIconX 25U
#define KhoangCachIconY 30U

#define ChieuDaiX KhoangCachIconX * 11U
#define ChieuDaiY KhoangCachIconY * 7U

#define MauDo 0xF800U
#define MauXanhLa 0x07E0U
#define MauTrang 0xFFFFU
#define MauDen 0x0000U

#define KhoangCachGiua2Khay 85U

#define BuachieuDaiXChonToaDo 650U
#define BuachieuDaiYChonToaDo 350U

#define ToaDoXIconHinhTronDauTienKhayChon 850U
#define ToaDoYIconHinhTronDauTienKhayChon 125U

#define ToaDoXIconHinhTronDauTienKhay1 35U
#define ToaDoYIconHinhTronDauTienKhay1 120U

#define ToaDoXIconHinhTronDauTienKhay2 405U
#define ToaDoYIconHinhTronDauTienKhay2 120U

#define ToaDoXIconHinhTronDauTienKhay3 775U
#define ToaDoYIconHinhTronDauTienKhay3 120U

#define ToaDoXIconHinhTronDauTienKhay4 1145U
#define ToaDoYIconHinhTronDauTienKhay4 120U

#define ToaDoXIconHinhTronDauTienKhay5 1515U
#define ToaDoYIconHinhTronDauTienKhay5 120U

#define ToaDoX0Khay1 30U
#define ToaDoY0Khay 105U
#define ToaDoY1Khay ToaDoY0Khay + ChieuDaiY /* + BanKinhIconOng * 4U */ + 10

#define ToaDoX0Khay(x) ToaDoX0Khay1 + x * (ChieuDaiX /* + BanKinhIconOng * 4U */ + KhoangCachGiua2Khay) + 45U
#define ToaDoX1Khay(x) ToaDoX0Khay(x) + ChieuDaiX /* + BanKinhIconOng * 4U */ + 5U

#define _VpAddressVung1KhayChon  0x3800UL
#define _VpAddressVung2KhayChon  0x3900UL
#define _VpAddressVung3KhayChon  0x3A00UL
#define _VpAddressVung4KhayChon  0x3B00UL

#define _VpAddressVung1KhayChonHienThiMau 0x3400UL
#define _VpAddressVung2KhayChonHienThiMau 0x3500UL
#define _VpAddressVung3KhayChonHienThiMau 0x3600UL
#define _VpAddressVung4KhayChonHienThiMau 0x3700UL

#define _VpAddressVung1Khay1HienThiMau 0x4000UL
#define _VpAddressVung2Khay1HienThiMau 0x4100UL
#define _VpAddressVung3Khay1HienThiMau 0x4200UL
#define _VpAddressVung4Khay1HienThiMau 0x4300UL

#define _VpAddressVung1Khay2HienThiMau 0x4800UL
#define _VpAddressVung2Khay2HienThiMau 0x4900UL
#define _VpAddressVung3Khay2HienThiMau 0x4A00UL
#define _VpAddressVung4Khay2HienThiMau 0x4B00UL

#define _VpAddressVung1Khay3HienThiMau 0x5000UL
#define _VpAddressVung2Khay3HienThiMau 0x5100UL
#define _VpAddressVung3Khay3HienThiMau 0x5200UL
#define _VpAddressVung4Khay3HienThiMau 0x5300UL

#define _VpAddressVung1Khay4HienThiMau 0x5800UL
#define _VpAddressVung2Khay4HienThiMau 0x5900UL
#define _VpAddressVung3Khay4HienThiMau 0x5A00UL
#define _VpAddressVung4Khay4HienThiMau 0x5B00UL

#define _VpAddressVung1Khay5HienThiMau 0x6000UL
#define _VpAddressVung2Khay5HienThiMau 0x6100UL
#define _VpAddressVung3Khay5HienThiMau 0x6200UL
#define _VpAddressVung4Khay5HienThiMau 0x6300UL

#define _HMIBamVaoVungChonToaDo 0x1A

#define _VpKhungHCN 0x9600UL
#define _VpKhungChonToaDo 0x9700UL

/* Define các nút bấm trang 130 */
#define _HMIBamNutEXITTrangManualControl 0x181
#define _HMIBamNutGoHomeTrangChaySetHome 0x81
#define _HMIBamNutExitTrangSetHome 0x82
/* Define các nút bấm trang 131 va 152 */
#define _HMIBamNutRunTrangCaiDatCheDoChuongTrinh 0x83
#define _HMIBamNutConfigTrangCaiDatCheDoChuongTrinh 0x84
#define _HMIBamNutSetting 0x80
#define _HMIBamNutModeTrangCaiDatCheDoChuongTrinh 0x85
#define _HMIBamNutProgram1TrangCaiDatCheDoChuongTrinh 0x86          // nút chọn chương trình cần thay thế 1->5
#define _HMIBamNutProgram2TrangCaiDatCheDoChuongTrinh 0x87
#define _HMIBamNutProgram3TrangCaiDatCheDoChuongTrinh 0x88
#define _HMIBamNutProgram4TrangCaiDatCheDoChuongTrinh 0x89
#define _HMIBamNutProgram5TrangCaiDatCheDoChuongTrinh 0x8A
#define _HMIBamNutChonProgram1TrangCaiDatCheDoChuongTrinh 0x200     // nút chọn chương trình cần chạy 1->5
#define _HMIBamNutChonProgram2TrangCaiDatCheDoChuongTrinh 0x201
#define _HMIBamNutChonProgram3TrangCaiDatCheDoChuongTrinh 0x202
#define _HMIBamNutChonProgram4TrangCaiDatCheDoChuongTrinh 0x203
#define _HMIBamNutChonProgram5TrangCaiDatCheDoChuongTrinh 0x204

/* Define các nút bấm trang 132*/
#define _HMIBamNutChonChuongTrinh1TrangChonChuongTrinh 0x8B         // nút chọn chương trình cần đổi 1->10
#define _HMIBamNutChonChuongTrinh2TrangChonChuongTrinh 0x8C
#define _HMIBamNutChonChuongTrinh3TrangChonChuongTrinh 0x8D
#define _HMIBamNutChonChuongTrinh4TrangChonChuongTrinh 0x8E
#define _HMIBamNutChonChuongTrinh5TrangChonChuongTrinh 0x8F
#define _HMIBamNutChonChuongTrinh6TrangChonChuongTrinh 0x90
#define _HMIBamNutChonChuongTrinh7TrangChonChuongTrinh 0x91
#define _HMIBamNutChonChuongTrinh8TrangChonChuongTrinh 0x92
#define _HMIBamNutChonChuongTrinh9TrangChonChuongTrinh 0x93
#define _HMIBamNutChonChuongTrinh10TrangChonChuongTrinh 0x94
#define _HMIBamNutOKTrangChonChuongTrinh 0x95
#define _HMIBamNutExitTrangChonChuongTrinh 0x96

/* Define các nút bấm trang 133 154 155 156 164*/
#define _HMIBamNutCHANGETrangPhanPhoiViTri 0x97
#define _HMIBamNutFULLTrangPhanPhoiViTri 0x98
#define _HMIBamNutCLEARTrangPhanPhoiViTri 0x99
#define _HMIBamNutADDTrangPhanPhoiViTri 0x9A
#define _HMIBamNutOKTrangPhanPhoiViTri 0x9B
#define _HMIBamNutEXITTrangPhanPhoiViTri 0x9C

/* Define các nút bấm trang 134*/
#define _HMIBamNutRUNTrangHienThiPhanPhoiThoiGianThuc 0x9D
#define _HMIBamNutRESETPROGRAMTrangHienThiPhanPhoiThoiGianThuc 0x9E
#define _HMIBamNutEXITTrangHienThiPhanPhoiThoiGianThuc 0x9F

/* Define các nút bấm trang 135*/
#define _HMIBamNutMODIFY1TrangThietLapCheDoChayMulti 0xA0
#define _HMIBamNutMODIFY2TrangThietLapCheDoChayMulti 0xA1
#define _HMIBamNutMODIFY3TrangThietLapCheDoChayMulti 0xA2
#define _HMIBamNutMODIFY4TrangThietLapCheDoChayMulti 0xA3
#define _HMIBamNutMODIFY5TrangThietLapCheDoChayMulti 0xA4
#define _HMIBamNutRUNTrangThietLapCheDoChayMulti 0xA5
#define _HMIBamNutEXITTrangThietLapCheDoChayMulti 0xA6
#define _HMIBamNutSecondTrangMULTIPROGRAMSETUP 0x201B
#define _HMIBamNutXoaTrangMULTIPROGRAMSETUP 0x201C

/* Define các nút bấm trang 136*/
#define _HMIBamNutEDITTrangThongSoChuongTrinh 0xA7
#define _HMIBamNutCOPYTrangThongSoChuongTrinh 0xA8
#define _HMIBamNutMODIFYTrangThongSoChuongTrinh 0xA9
#define _HMIBamNutOKTrangThongSoChuongTrinh 0xAA
#define _HMIBamNutEXITTrangThongSoChuongTrinh 0xAB

/* Define các nút bấm trang 137*/
#define _HMIBamNutTUBEPERROWTrangThietLapChuongTrinhDiChuyen 0xAC
#define _HMIBamNutTUBEPERCOLUMNTrangThietLapChuongTrinhDiChuyen 0xAD
#define _HMIBamNutSPEEDXYAXISrangThietLapChuongTrinhDiChuyen 0xAE
#define _HMIBamNutNEXTTrangThietLapChuongTrinhDiChuyen 0xAF
#define _HMIBamNutBACKTrangThietLapChuongTrinhDiChuyen 0xB0

/* Define bàn phím trang 138 139 140 158 159 160 161 162 163 166*/
#define _HMIBamNutBanPhim0 0xB1
#define _HMIBamNutBanPhim1 0xB2
#define _HMIBamNutBanPhim2 0xB3
#define _HMIBamNutBanPhim3 0xB4
#define _HMIBamNutBanPhim4 0xB5
#define _HMIBamNutBanPhim5 0xB6
#define _HMIBamNutBanPhim6 0xB7
#define _HMIBamNutBanPhim7 0xB8
#define _HMIBamNutBanPhim8 0xB9
#define _HMIBamNutBanPhim9 0xBA
#define _HMIBamNutBanPhimdelete 0xBB
#define _HMIBamNutReturnTrangBanPhim 0xBC
#define _HMIBamNutEnterTrangBanPhim 0xBD
#define _HMIBamNutSettingTrangBanPhim 0xBE
#define _HMIBamNutBanPhimCham 0xBF

/* Define các nút bấm trang 141*/
#define _HMIBamNutGETATrangLayViTriChonDiem 0xDB
#define _HMIBamNutGETBTrangLayViTriChonDiem 0xDC
#define _HMIBamNutGETHTrangLayViTriChonDiem 0xDD
#define _HMIBamNutGETCTrangLayViTriChonDiem 0xDE
#define _HMIBamNutNEXTTrangLayViTriChonDiem 0xDF
#define _HMIBamNutBACKTrangLayViTriChonDiem 0xE0

/* Define các nút bấm trang 142 143 144 145*/
#define _HMIBamNutXTru 0xE1
#define _HMIBamNutYCong 0xE2
#define _HMIBamNutXCong 0xE3
#define _HMIBamNutYTru 0xE4
#define _HMIBamNutZCong 0xE5
#define _HMIBamNutZTru 0xE6
#define _HMIBamNutLOW 0xEA
#define _HMIBamNutMEDIUM 0xEB
#define _HMIBamNutFAST 0xEC
#define _HMIBamNutGET 0xED
#define _HMIBamNutBackTrangLayViTriXacNhan 0xEE

/* Define nút bấm trang 146 */
#define _HMIBamNutCalibTrangCaiDat 0xF0
#define _HMIBamNutBackTrangCaiDat 0xF1
#define _HMIBamNutWiFiTrangCaiDat 0xF2
#define _HMIBamNutINFORTrangCaiDat 0xF3
#define _HMIBamNutSetupProgramTrangCaiDat 0xF4
#define _HMIBamNutTimeTrangCaiDat 0xF5
/* Define nút bấm trang 147 */
#define _HMIBamNutSelectTrangCalib 0xF7
#define _HMIBamNutTestTrangCalib 0xF8
#define _HMIBamNutSaveTrangCalib 0xF9
#define _HMIBamNutExitTrangCalib 0xFA
#define _HMIBamTextActualWeight 0x1000
#define _HMIBamTextDensity 0x1001
#define _HMIBamNutResetTrangCalib 0x1002

/* Define nút bấm trang 149 */
#define _HMIBamNutCapsLockTrangBanPhim 0xFF80
#define _HMIBamNutDeleteTrangBanPhim 0xFF7F
#define _HMIBamNutChuyenTrangSoTrangBanPhim 0xFF81

/* Define nút bấm trang 150 */
#define _HMIBamNutSetHourTrangDatenTime 0x101
#define _HMIBamNutSetMinuteTrangDatenTime 0x102
#define _HMIBamNutSetDayTrangDatenTime 0x103
#define _HMIBamNutSetMonthTrangDatenTime 0x104
#define _HMIBamNutSetYearTrangDatenTime 0x105
#define _HMIBamNutReturnTrangDatenTime 0x106  
#define _HMIBamNutEnterTrangDatenTime 0x107
/* Define nút bấm trang 157 */
#define _HMIBamNutVolumeTrangThietLapTheTichPhanPhoi 0xFB
#define _HMIBamNutTubingIDTrangThietLapTheTichPhanPhoi 0xFC
#define _HMIBamNutPumpSpeedTrangThietLapTheTichPhanPhoi 0xFD
#define _HMIBamNutAntiDropTrangThietLapTheTichPhanPhoi 0xFE
#define _HMIBamNutNextTrangThietLapTheTichPhanPhoi 0xEF
#define _HMIBamNutBackTrangThietLapTheTichPhanPhoi 0x100

/* Define nút bấm trang 165 */
#define _HMIBamNutSelectTrangXemConifg 0x1010
#define _HMIBamNutResetTrangXemconfig 0x1011
#define _HMIBamNutTestFillTrangXemConfig 0x1012
#define _HMIBamNutDirectionTrangXemConfig 0x1013
#define _HMIBamNutFastPumpTrangXemConfig 0x1014
#define _HMIBamNutGoATrangXemConfig 0x1015
#define _HMIBamNutGoBTrangXemConfig 0x1016
#define _HMIBamNutGoCTrangXemConfig 0x1017
#define _HMIBamNutModifyProgramTrangXemConfig 0x1018
#define _HMIBamNutManualControlTrangXemConfig 0x1019
#define _HMIBamNutExitTrangXemConfig 0x101A

/* Define nút bấm trang 168 */
#define _HMIBamNutEnterTrangBanPhimMoRong 0x101B
#define _HMIBamNutReturnTrangBanPhimMoRong 0x101C

#define _HMIBamNutBackTrangInfo 0x1020

/* Define nút bấm trang 174*/
#define _HMIBamNutUNLOCKTrangManualControl 0x7000
#define _HMIBamNutRESETTrangManualControl 0x7001

#define _TrangBatDauChaySetHome 130
#define _TrangCaiDatCheDoChuongTrinh 131
#define _TrangChonChuongTrinh 132
#define _TrangPhanPhoiViTri 133
#define _TrangHienThiPhanPhoiThoiGianThuc 134
#define _TrangThietLapCheDoChayMulti 135
#define _TrangThongSoChuongTrinh 136
#define _TrangThietLapChuongTrinhDiChuyen 137
#define _TrangThietLapSoOngTrenMotHang 138
#define _TrangThietLapSoOngTrenMotCot 139
#define _TrangThietLapTocDoTrucXY 140
#define _TrangLayViTriChonDiem 141
#define _TrangLayViTriXacNhanH 142
#define _TrangLayViTriXacNhanA 143
#define _TrangLayViTriXacNhanB 144
#define _TrangLayViTriXacNhanC 173
#define _TrangManualControl 174
#define _TrangCaiDat 146
#define _TrangCalib 147
#define _TrangWiFi 148
#define _TrangBanPhimMoRong 172
#define _TrangThoiGian 171
#define _TrangThongTin 151
#define _TrangCaiDatCheDoChuongTrinhMULTI 170
#define _TrangHienThiPhanPhoiThoiGianThucON 153
#define _TrangPhanPhoiViTriFULL 154
#define _TrangPhanPhoiViTriCLEAR 155
#define _TrangPhanPhoiViTriADD 156
#define _TrangThietLapTheTichPhanPhoi 157
#define _TrangBanPhimVolume 158
#define _TrangBanPhimTubeID 159
#define _TrangBanPhimPumpSpeed 160
#define _TrangBanPhimAntiDrop 161
#define _TrangBanPhimDensity 162
#define _TrangBanPhimActualWeight 163
#define _TrangPhanPhoiViTriSub 164
#define _TrangXemThongSoConfig 169
#define _TrangBanPhimSecondLoop 166
#define _TrangBanPhimCaiDatThoiGian 167
#define _TrangBanPhimMoRongSo 168



#define _VPChonProgram1TrangCaiDatCheDoChuongTrinh 0x3300
#define _VPChonProgram2TrangCaiDatCheDoChuongTrinh 0x3301
#define _VPChonProgram3TrangCaiDatCheDoChuongTrinh 0x3302
#define _VPChonProgram4TrangCaiDatCheDoChuongTrinh 0x3303
#define _VPChonProgram5TrangCaiDatCheDoChuongTrinh 0x3304

/* Trang 131 */
#define _VPTextHienThiTenProgramTrangCaiDatCheDoChuongTrinh 0x9500
#define _VPTextHienThiChucNangProgramTrangCaiDatCheDoChuongTrinh 0x5400
#define _VPTextProgram1TrangChonCheDo 0xA000
#define _VPTextProgram2TrangChonCheDo 0xA100
#define _VPTextProgram3TrangChonCheDo 0xA200
#define _VPTextProgram4TrangChonCheDo 0xA300
#define _VPTextProgram5TrangChonCheDo 0xA400

/* Trang 132*/
#define _VPTextProgram1TrangChonChuongTrinh 0x8000
#define _VPTextProgram2TrangChonChuongTrinh 0x8100
#define _VPTextProgram3TrangChonChuongTrinh 0x8200
#define _VPTextProgram4TrangChonChuongTrinh 0x8300
#define _VPTextProgram5TrangChonChuongTrinh 0x8400
#define _VPTextProgram6TrangChonChuongTrinh 0x8500
#define _VPTextProgram7TrangChonChuongTrinh 0x8600
#define _VPTextProgram8TrangChonChuongTrinh 0x8700
#define _VPTextProgram9TrangChonChuongTrinh 0x8800
#define _VPTextProgram10TrangChonChuongTrinh 0x8900
#define _VPTextHienThiChuongTrinhTrangChonChuongTrinh 0x9000

/* Trang 133 154 155 156 164 */
#define _VPTextHienThiTenProgramTrangPhanPhoiViTri 0xC600

/* Trang 134 */
#define _VPTextTimeRunTrangHienThiPhanPhoiThoiGianThuc 0xC100
#define _VPTextFillNumberTrangHienThiPhanPhoiThoiGianThuc 0xC200
#define _VPTextXTrangHienThiPhanPhoiThoiGianThuc 0xC300
#define _VPTextYTrangHienThiPhanPhoiThoiGianThuc 0xC400
#define _VPTextZTrangHienThiPhanPhoiThoiGianThuc 0xC500

/* Trang 135 */
#define _VPTextprogram1TrangThietLapChayCheDoMULTI 0xB000
#define _VPTextprogram2TrangThietLapChayCheDoMULTI 0xB100
#define _VPTextprogram3TrangThietLapChayCheDoMULTI 0xB200
#define _VPTextprogram4TrangThietLapChayCheDoMULTI 0xB300
#define _VPTextprogram5TrangThietLapChayCheDoMULTI 0xB400
#define _VPTextSecondTrangMULTIPROGRAMSETUP 0xB500

/* Trang 136 */
#define _VPTextTenChuongTrinhTrangThietLapChuongTrinh 0x6600
#define _VPTextThongSoTrangThietLapChuongTrinh 0xC000

/* Trang 137 138*/
#define _VPTextTubePerRow 0x6700
/* Trang 137 139 */
#define _VPTextTubePerColumn 0x6800
/* Trang 137 140 */
#define _VPTextSpeedXYAxis 0x6900

/* Trang 141 */
#define _VPTextInformationLocation 0xD000

/* Trang 142 143 144 145*/
#define _VPTextGetLocationX 0xE000
#define _VPTextGetLocationY 0xE100
#define _VPTextGetLocationZ 0xE200
#define _VPTextGetLocationStep 0xE300
#define _VPTextGetLocationFeed 0xE400
#define _VPTextSPEEDXYZTrangLayViTri 0xF400
#define _VPTextXacNhanGETTrangLayViTri 0xF500

/* Trang 147 */
#define _VPTextChuongTrinhTrangCalib 0xE500
#define _VPTextDensity 0xEA00
#define _VPTextActualWeight 0xEB00
#define _VPTextFillNumber 0xEC00
#define _VPTextXacNhanSaveCalib 0xED00


/* Trang 157 */
#define _VPTextVolumeTrangCaiDatTheTichPhanPhoi 0xE600
#define _VPTextTubingIDTrangCaiDatTheTichPhanPhoi 0xE700
#define _VPTextPumpSpeedTrangCaiDatTheTichPhanPhoi 0xE800
#define _VPTextAntiDropTrangCaiDatTheTichPhanPhoi 0xE900


#define _VPTextXYZTrangXemConfig 0xEE00
#define _VPTextTenChuongTrinhTrangXemConfig 0xEF00
#define _VPTextTotalFillTubeTrangXemConfig 0xF000
#define _VpTextPumpControlTrangXemConfig 0xF100
#define _VPTextMinutesTrangCaiDatThoiGianLoop 0xF200

#define _VPTextHourTrangDaynTime 0xF210
#define _VPTextMinuteTrangDaynTime 0xF230
#define _VPTextDayTrangDaynTime 0xF250
#define _VPTextMonthTrangDaynTime 0xF270
#define _VPTextYearTrangDaynTime 0xF290

#define _VPTextDataTrangBanPhim 0xF500
#define _VPTextTimeTrangBanPhimDaynTime 0xF300
#define _VPTextKieuThoiGianTrangBanPhimDaynTime 0xF400

#define _VPTextNgay 0xF600
#define _VPTextGio 0xF700

#define _VPIconDIRECTION 0xEA00

#define _VPTextDelayTimeRunTrangHienThiPhanPhoiThoiGianThuc 0xC700

#define _VPTextSecondTrangCaiDatThoiGianLoop 0xC900
#define _VPTextHoursTrangCaiDatThoiGianLoop 0xC800

#define _VPTextTenChuongTrinhDangChay 0xCA00

#define _HMIBamNutHourTrangSecondLoop 0x7010
#define _HMIBamNutMinutesTrangSecondLoop 0x7020
#define _HMIBamNutSecondsTrangSecondLoop 0x7030

#define _HMIBamNutGoHomeTrangDieuKhien 0x801
typedef enum {
  _CaiDatGioLoop,
  _CaiDatPhutLoop,
  _CaiDatGiayLoop
}CheDoCaiDatTimeLoop_t;


typedef struct {
  uint32_t vitriX; // Tren HMI
  uint32_t vitriY; // Tren HMI
  uint8_t toadoX;
  uint8_t toadoY;
}ViTrivaToaDo_t;

typedef enum {
  _VaoTrangGETA,
  _VaoTrangGETB,
  _VaoTrangGETC,
  _VaoTrangGETH
}CheDoVaoTrangGETABCH_t;

typedef enum {
  _VaoTrangTuCacNutProgramTrangCaiDatCheDoChuongTrinh,
  _VaoTrangTuNutSetupProgramTrangCaiDatCheDoChuongTrinh,
  _VaoTrangTuNutCopyTrangThongSoChuongTrinh,
  _VaoTrangTuNutOK_EXITTrangThongSoChuongTrinh,
  _VaoTrangTuNutChangeTrangPhanPhoiViTri,
  _VaoTrangTuNutSelectTrangCalib,
  _VaoTrangTuNutSelectTrangXemConfig,
}CheDoVaoTrangChonChuongTrinh_t;

typedef enum {
  _VaoTrangBanPhimActualWeight,
  _VaoTrangBanPhimDensity,
  _VaoTrangBanPhimVolume,
  _VaoTrangBanPhimTubeID,
  _VaoTrangBanPhimPumpSpeed,
  _VaoTrangBanPhimAntiDrop,
  _VaoTrangThietLapSoOngTrenMotHang,
  _VaoTrangThietLapSoOngTrenMotCot,
  _VaoTrangThietLapTocDoTrucXY,
  _VaoTrangBanPhimSecondLoop,
  _VaoTrangBanPhimCaiDatThoiGian,
}CheDoVaoTrangBanPhim_t;
typedef enum {
  _VaoTrangBanPhimMoRongTuNutEDITTrangThongSoChuongTrinh,
}CheDoVaoTrangBanPhimMoRong_t;

typedef enum {
  _CheDoHoatDongSimple,
  _CheDoHoatDongMulti
}CheDoHoatDong_t;

typedef enum {
  _CaiDatGio,
  _CaiDatPhut,
  _CaiDatNgay,
  _CaiDatThang,
  _CaiDatNam
}CheDoCaiDatThoiGian_t;

typedef struct {
  vector<chuongtrinh_t> DSChuongTrinh;
  double ViTriHienTaiX;
  double ViTriHienTaiY;
  double ViTriHienTaiZ;
  bool TrangThaiXacNhanSaveCalib;
  bool TrangThaiXacNhanSetHome;
  String RTC_Ngay = "";
  String RTC_Thang = "";
  String RTC_Nam = "";
  String RTC_Gio = "";
  String RTC_Phut = "";
  String RTC_ChuoiNgay = "";
  String RTC_ChuoiGio = "";
  bool TrangThaiChayXongMulti = false;
  bool CapNhatThoiGianRTC;
} DuLieuHMI;// Struct cấu trúc dữ liệu dùng để giao tiếp giữa board và HMI.


class DWINHMI : public DWIN {
public:

  DWINHMI();                                                    // Init class DWINHMI.
  void KhoiTaoDWIN();                                       // Hàm khởi tạo cho màn hình DWIN.
  void LangNgheDWIN();                                      // Hàm lắng nghe các thao tác của user trên màn hình HMI.
  DuLieuHMI DuLieuHMIGuiXuongBoard;                             
  DuLieuHMI DuLieuBoardGuiLenHMI;
  vector<chuongtrinh_t> _ChuongTrinhEEPROM;                 // Biến này chỉ thay đổi khi người dùng muốn thay đổỉ để lưu chương trình
  void ChuyenTrangTheoBamChonCuaUserTrenHMI();              // Hàm này dùng chuyển đến trang khi user bấm chuyển trang trên HMI.
  void XuLySetHome();
  void XuLyChonChuongTrinhSeChay();                               // Hiển thị trên màn hình HMI các dữ liệu từ board gửi lên.
  void XuLyChonViTriPhanPhoi();
  void XuLyConfigChuongTrinh();
  void XuLyBanPhim();
  void XuLyXemConfig();
  void XuLyThietLapChuongTrinhCheDoMulti();
  void HienThiThongSoHienThiThoiGianThuc();
  void ResetProgram();
  void XuLySetupProgram();
  void XuLyChonMode();
  void XuLyThoiGianThuc();
  void XuLyCalib();
  void ResetCode();
  void ThayDoiTenChuongTrinh();

  DanhSachMaLenh_t ReturnMaLenhGRBL();
  CheDoHoatDong_t Mode = _CheDoHoatDongSimple;
  bool BamNutRunON = false;
  bool delay_flag = false;
  int32_t timeLoopDelay = 0;
  bool TamDungChayDelay = false;
  int8_t IndexChuongTrinhDangXuLy;                           // Đây là biến tạm để biết chương trình nào đang xử lý
  uint8_t countTest = 1;

private:
  uint16_t KichThuocHcnDaVeTheoX;  // kích thước X / 2
  uint16_t KichThuocHcnDaVeTheoY;  // kích thước Y / 2
  vector<ViTrivaToaDo_t> vitrivatoado;
  uint16_t khaychon[12][8];
  uint16_t khay1[12][8];
  uint16_t khay2[12][8];
  uint16_t khay3[12][8];
  uint16_t khay4[12][8];
  uint16_t khay5[12][8];
  String TimeRunStart_c = "00:00";
  String timeLoopDelay_c = "00:00"; 
  int TimeNow = 0;
  uint32_t DentaT = 0;
  // uint8_t NutModidyDaBam;                                         // Đây là biến dùng để biết nút modify nào được bấm để hiển thị full chương trình hợp lý           
  uint8_t IndxchuongTrinhDangModify;                              // Đây là biến dùng để sử dụng ở chế độ multi để biêt chương trình nào đang được thay đổi
  vector <uint8_t> DSIndexChuongTrinhDuocChon;                    // Đây là biến dùng để xử lý chương trình được chọn ở chê độ MULTI
  uint8_t DSIndexChuongTrinhHienThi[5]={0,1,2,3,4};               // Đây là biến dùng để xử lý chương trình hiển thị được chọn ở chê độ MULTI
  CheDoVaoTrangChonChuongTrinh_t CheDoVaoTrangChonChuongTrinh;
  CheDoVaoTrangBanPhim_t CheDoVaoTrangBanPhim;
  CheDoVaoTrangBanPhimMoRong_t CheDoVaoTrangBanPhimMoRong;
  CheDoCaiDatThoiGian_t CheDoCaiDatThoiGian;
  CheDoCaiDatTimeLoop_t CheDoCaiDatTimeLoop;
  CheDoVaoTrangGETABCH_t CheDoVaoTrangGETABCH;
  bool oneTimeMulti = false; // Xác định chế độ multi chỉ chạy một lần
  bool resetTimeRunStart = true;
  static int lastcodeHMI;                  // Mã code cũ mỗi khi user bấm các nút trên màn hình HMI.
  static int codeHMI;                      // Mã code mỗi khi user bấm các nút trên màn hình HMI.
  static void onHMIEvent(String address, int lastByte, String message, String response);
  void GuiTextLenHMI(uint16_t VPAddress, String text);              // Hiển thị nội dung text trên HMI.
  void GuiIconLenHMI(uint16_t VPAddress, uint8_t data);
  void Debug(vector<chuongtrinh_t> data);
  void CapNhatHienThiThongTinChiTietChuongTrinh();
  void CapNhatHienThiThongTinChuongTrinhTrangChonChuongTrinh();
  void CapNhatHienThiThongTinTenChuongTrinhKhiVaoTrangChonChuongTrinh();
  void CapNhatHienThiThongTinChuongTrinhTrangThietLapTheTichPhanPhoi();
  void CapNhatHienThiThongTinChuongTrinhTrangThietLapChuongTrinhDiChuyen();
  void CapNhatHienThiTenCacChuongTrinhTrangCaiDatCheDoChuongTrinh();
  void CapNhatHienThiDuLieuThoiGianTuDS3231();
  void HienThiMau(uint32_t VpAddress, uint16_t X, uint16_t Y, uint16_t Mau, chuongtrinh_t ChuongTrinh);
  void HienThiMau(int8_t SoKhay, uint16_t X, uint16_t Y, chuongtrinh_t ChuongTrinh,uint16_t Mau);
  void ResetFullMauChonToaDo(uint16_t Mau);
  void ResetFullMau(uint8_t IdxChuongTrinhDuocChon, uint16_t Mau, uint8_t SoKhay);
  void VeToanBoHinhTron(uint32_t VpAddress, chuongtrinh_t ChuongTrinh);
  void VeToanBoHinhTron(uint8_t IdxChuongTrinhDuocChon, uint8_t SoKhay);
  void VeKhungHCN(uint8_t SoLuongKhay, uint16_t Mau);
  void VeKhungHCN(uint32_t VpAddress, uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Mau);
  void XoaToanBoHinhTron(void);
  void XoaHinhTron(uint32_t VpAddress);
  void XoaMau(uint32_t VpAddress);
  TaskHandle_t xTaskHandle_RunTime;
  static void xTaskFunction_RunTime(void *);
  void ChuyenDoiThanhChuoiThoiGian();

  void CapNhatLaiDanhSachToaDo(chuongtrinh_t & ChuongTrinh);

  void HienThiNoiDungChuongTrinh(void);
};

#endif