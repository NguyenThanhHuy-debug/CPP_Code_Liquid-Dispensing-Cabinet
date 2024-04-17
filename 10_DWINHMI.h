#ifndef _DWINHMI_h
#define _DWINHMI_h

#include "KingTubeAuto.h"
#include <Arduino.h>  
#include <DWIN.h>  

#define RACK_X_UPPER_LIMIT 12
#define RACK_X_LOWER_LIMIT 1

#define RACK_Y_UPPER_LIMIT 8
#define RACK_Y_LOWER_LIMIT 1

#define DENSITY_LOWER_LIMIT 0

#define TUBE_ID_LOWER_LIMIT 0
#define SPEED_XY_UPPER_LIMIT DEFAULT_X_MAX_RATE

#define SPEED_PUMP_UPPER_LIMIT DEFAULT_A_MAX_RATE

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

#define ChieuDaiX 350
#define ChieuDaiY 180

#define MauDo 0xF800U
#define MauXanhLa 0x07E0U
#define MauTrang 0xFFFFU
#define MauDen 0x0000U

#define BuachieuDaiXChonToaDo 750U
#define BuachieuDaiYChonToaDo 320U

#define ToaDoXIconHinhTronDauTienKhayChon 760U
#define ToaDoYIconHinhTronDauTienKhayChon 45U

#define ToaDoXIconHinhTronDauTienKhay1 150U
#define ToaDoYIconHinhTronDauTienKhay1 140U

#define ToaDoXIconHinhTronDauTienKhay2 550U
#define ToaDoYIconHinhTronDauTienKhay2 ToaDoYIconHinhTronDauTienKhay1

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
#define _HMIBamNutChonProgram6TrangCaiDatCheDoChuongTrinh 0x205
#define _HMIBamNutChonProgram7TrangCaiDatCheDoChuongTrinh 0x206
#define _HMIBamNutChonProgram8TrangCaiDatCheDoChuongTrinh 0x207
#define _HMIBamNutChonProgram9TrangCaiDatCheDoChuongTrinh 0x208
#define _HMIBamNutChonProgram10TrangCaiDatCheDoChuongTrinh 0x209

#define _HMIBamNutXoaArea1 0x70
#define _HMIBamNutXoaArea2 0x71

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

#define _HMIBamNutThayDoiSoLuongOng 0x802 //TODO thay đổi keycode
#define _HMIBamNutOkThayDoiSoLuongOng 0x803 //TODO


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
#define _HMIBamNutEXITTrangThietLapCheDoChayMulti 0xA6
#define _HMIBamNutSecondTrangMULTIPROGRAMSETUP 0x201B
#define _HMIBamNutXoaTrangMULTIPROGRAMSETUP 0x201C

/* Define các nút bấm trang 136*/
#define _HMIBamNutEDITTrangThongSoChuongTrinh 0xA7
#define _HMIBamNutCOPYTrangThongSoChuongTrinh 0xA8
#define _HMIBamNutMODIFYTrangThongSoChuongTrinh 0xA9
#define _HMIBamNutEXITTrangThongSoChuongTrinh 0xAB

/* Define các nút bấm trang 137*/
#define _HMIBamNutTUBEPERROWTrangThietLapChuongTrinhDiChuyen 0xAC
#define _HMIBamNutTUBEPERCOLUMNTrangThietLapChuongTrinhDiChuyen 0xAD
#define _HMIBamNutSPEEDXYAXISrangThietLapChuongTrinhDiChuyen 0xAE
#define _HMIBamNutNEXTTrangThietLapChuongTrinhDiChuyen 0xAF

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
#define _HMIBamNutGetA1 0xD0
#define _HMIBamNutGetA2 0xD1
#define _HMIBamNutGetB1 0xD2
#define _HMIBamNutGetB2 0xD3
#define _HMIBamNutGetC1 0xD4
#define _HMIBamNutGetC2 0xD5
#define _HMIBamNutGetH1 0xD6
#define _HMIBamNutGetH2 0xD7

#define _HMIBamNutGET 0xED
#define _HMIBamNutBackTrangLayViTriXacNhan 0xEE

/* Define nút bấm trang 146 */
#define _HMIBamNutCalibTrangCaiDat 0xF0
#define _HMIBamNutBackTrangCaiDat 0xF1
#define _HMIBamNutWiFiTrangCaiDat 0xF2
#define _HMIBamNutINFORTrangCaiDat 0xF3
#define _HMIBamNutSetupProgramTrangCaiDat 0xF4
#define _HMIBamNutTimeTrangCaiDat 0xF5
#define _HMIBamNutShutdownTrangCaiDat 0xF6
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
#define _HMIBamNutGoA1TrangXemConfig 0x1015
#define _HMIBamNutGoB1TrangXemConfig 0x1016
#define _HMIBamNutGoC1TrangXemConfig 0x1017
#define _HMIBamNutGoA2TrangXemConfig 0x101D
#define _HMIBamNutGoB2TrangXemConfig 0x101E
#define _HMIBamNutGoC2TrangXemConfig 0x101F

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
#define _TrangCaiDatCheDoChuongTrinh_HOME 131
#define _TrangChonChuongTrinh 132
#define _TrangPhanPhoiViTri 133
#define _TrangHienThiPhanPhoiThoiGianThuc_OFF_HOME 134
#define _TrangHienThiPhanPhoiThoiGianThuc_OFF_notHOME 175
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
#define _TrangManualControl 178 //174
#define _TrangCaiDat 146
#define _TrangCalib 147
#define _TrangWiFi 148
#define _TrangBanPhimMoRong 172
#define _TrangThoiGian 171
#define _TrangThongTin 151
#define _TrangCaiDatCheDoChuongTrinh_notHOME 170
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
#define _TrangXemThongSoConfig 165
#define _TrangBanPhimSecondLoop 166
#define _TrangBanPhimCaiDatThoiGian 167
#define _TrangBanPhimMoRongSo 168
#define _TrangBanPhimMauKhau 176
#define _TrangBanPhimThayDoiSoLuongOng 177 //TODO



#define _VPChonProgram1TrangCaiDatCheDoChuongTrinh 0x3300
#define _VPChonProgram2TrangCaiDatCheDoChuongTrinh 0x3301
#define _VPChonProgram3TrangCaiDatCheDoChuongTrinh 0x3302
#define _VPChonProgram4TrangCaiDatCheDoChuongTrinh 0x3303
#define _VPChonProgram5TrangCaiDatCheDoChuongTrinh 0x3304
#define _VPChonProgram6TrangCaiDatCheDoChuongTrinh 0x3305
#define _VPChonProgram7TrangCaiDatCheDoChuongTrinh 0x3306
#define _VPChonProgram8TrangCaiDatCheDoChuongTrinh 0x3307
#define _VPChonProgram9TrangCaiDatCheDoChuongTrinh 0x3308
#define _VPChonProgram10TrangCaiDatCheDoChuongTrinh 0x3309

/* Trang 131 */
#define _VPTextTenProgramArea1 0x9100
#define _VPTextTenProgramArea2 0x9200
#define _VPTextHienThiTenProgramTrangCaiDatCheDoChuongTrinh 0x9500   
#define _VPTextHienThiChucNangProgramTrangCaiDatCheDoChuongTrinh 0x5400 

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

#define _VpTextMatKhau        0x3100
#define _VpTextCanhBaoChuoiDangNhap 0x3200

#define _HMIBamNutHourTrangSecondLoop 0x7010
#define _HMIBamNutMinutesTrangSecondLoop 0x7020
#define _HMIBamNutSecondsTrangSecondLoop 0x7030

#define _HMIBamNutGoHomeTrangDieuKhien 0x801

// Trang 176
#define _VpTextThayDoiSoLuongOng 0x3C00UL//TODO

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
  _VaoTrangTuNutChangeTrangPhanPhoiViTri,
  _VaoTrangTuNutSelectTrangCalib,
  _VaoTrangTuNutSelectTrangXemConfig,
}CheDoVaoTrangChonChuongTrinh_t;

typedef enum {
  _RESETBanPhim,
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
  _VaoTrangBanPhimMatKhau,
  _VaoTrangBanPhimThayDoiSoLuongOng,
}CheDoVaoTrangBanPhim_t;
typedef enum {
  _VaoTrangBanPhimMoRongTuNutEDITTrangThongSoChuongTrinh,
}CheDoVaoTrangBanPhimMoRong_t;

typedef enum {
  _CaiDatGio,
  _CaiDatPhut,
  _CaiDatNgay,
  _CaiDatThang,
  _CaiDatNam
}CheDoCaiDatThoiGian_t;

typedef struct {
  vector<ChuongTrinh_t> DSChuongTrinh;
  float ViTriHienTaiX;
  float ViTriHienTaiY;
  float ViTriHienTaiZ;
  bool TrangThaiXacNhanSaveCalib;
  bool TrangThaiXacNhanSetHome = false; //TTODO
  String RTC_Ngay = "";
  String RTC_Thang = "";
  String RTC_Nam = "";
  String RTC_Gio = "";
  String RTC_Phut = "";
  String RTC_ChuoiNgay = "";
  String RTC_ChuoiGio = "";
  bool CapNhatThoiGianRTC;
  int8_t KhuVucDangChay = -1;
  int8_t OngDaPhanPhoi = -1;
  uint8_t ToaDoXDaChay;
  uint8_t ToaDoYDaChay;
  bool DaChayXongCacKhuVuc = false;
  bool DaChayXongMotOng = false;
} DuLieuHMI;// Struct cấu trúc dữ liệu dùng để giao tiếp giữa board và HMI.


class DWINHMI : public DWIN {
public:

  DWINHMI();                                                    // Init class DWINHMI.
  void KhoiTaoDWIN();                                       // Hàm khởi tạo cho màn hình DWIN.
  void LangNgheDWIN();                                      // Hàm lắng nghe các thao tác của user trên màn hình HMI.
  DuLieuHMI DuLieuHMIGuiXuongBoard;                             
  DuLieuHMI DuLieuBoardGuiLenHMI;
  vector<ChuongTrinh_t> _ChuongTrinhEEPROM;                 // Biến này chỉ thay đổi khi người dùng muốn thay đổỉ để lưu chương trình
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
  void XuLyThoiGianThuc();
  void XuLyCalib();
  void ResetCode();
  void ThayDoiTenChuongTrinh();

  DanhSachMaLenh_t ReturnMaLenhGRBL();
  bool BamNutRunON = false;
  bool delay_flag = false;
  int32_t timeLoopDelay = 0;
  int8_t IndexChuongTrinhDangXuLy;                           // Đây là biến tạm để biết chương trình nào đang xử lý
  uint8_t countTest = 1;
  int8_t DSIndexChuongTrinhDuocChon[SoLuongKhuVucToiDa];      
  static bool ManualControl;
      

private:
  uint8_t SoLuongOngThayDoi = 0;
  bool DaChayXongCacKhuVuc_oneTimeMulti = false;
  vector<ViTrivaToaDo_t> vitrivatoado;
  uint16_t khaychon[12][8];
  uint16_t khay1[12][8];
  uint16_t khay2[12][8];
  uint16_t khay3[12][8];
  uint16_t khay4[12][8];
  String TimeRunStart_c = "00:00";
  String timeLoopDelay_c = "00:00"; 
  int TimeNow = 0;
  uint8_t IdxKhuVucDangXuLy;                             
  bool CanCapNhatThongSo = false;
  CheDoVaoTrangChonChuongTrinh_t CheDoVaoTrangChonChuongTrinh;
  CheDoVaoTrangBanPhim_t CheDoVaoTrangBanPhim;
  CheDoVaoTrangBanPhimMoRong_t CheDoVaoTrangBanPhimMoRong;
  CheDoCaiDatThoiGian_t CheDoCaiDatThoiGian;
  CheDoCaiDatTimeLoop_t CheDoCaiDatTimeLoop;
  CheDoVaoTrangGETABCH_t CheDoVaoTrangGETABCH;
  bool oneTimeMulti = true; // Xác định chế độ multi chỉ chạy một lần
  bool resetTimeRunStart = true;
  static int lastcodeHMI;                  // Mã code cũ mỗi khi user bấm các nút trên màn hình HMI.
  static int codeHMI;                      // Mã code mỗi khi user bấm các nút trên màn hình HMI.
  static void onHMIEvent(String address, int lastByte, String message, String response);
  void GuiTextLenHMI(uint16_t VPAddress, String text);              // Hiển thị nội dung text trên HMI.
  void GuiIconLenHMI(uint16_t VPAddress, uint8_t data);
  void Debug(vector<ChuongTrinh_t> data);
  void CapNhatHienThiThongTinChiTietChuongTrinh();
  void CapNhatHienThiThongTinChuongTrinhTrangChonChuongTrinh();
  void CapNhatHienThiThongTinTenChuongTrinhKhiVaoTrangChonChuongTrinh();
  void CapNhatHienThiThongTinChuongTrinhTrangThietLapTheTichPhanPhoi();
  void CapNhatHienThiThongTinChuongTrinhTrangThietLapChuongTrinhDiChuyen();
  void CapNhatHienThiDuLieuThoiGianTuDS3231();
  void HienThiMau(uint32_t VpAddress, uint16_t X, uint16_t Y, uint16_t Mau, ChuongTrinh_t ChuongTrinh);
  void HienThiMau(int8_t SoKhay, uint16_t X, uint16_t Y, ChuongTrinh_t ChuongTrinh,uint16_t Mau);
  void ResetFullMauChonToaDo(uint16_t Mau);
  void ResetFullMau(uint8_t IdxChuongTrinhDuocChon, uint16_t Mau, uint8_t SoKhay);
  void VeToanBoHinhTron(uint32_t VpAddress, ChuongTrinh_t ChuongTrinh);
  void VeToanBoHinhTron(uint8_t IdxChuongTrinhDuocChon, uint8_t SoKhay);
  void XoaToanBoHinhTron(void);
  void XoaHinhTron(uint32_t VpAddress);
  void XoaMau(uint32_t VpAddress);
  TaskHandle_t xTaskHandle_RunTime;
  static void StaticTaskFunction_RunTime(void* _this);
  void TaskFunction_RunTime(void);
  void ChuyenDoiThanhChuoiThoiGian();

  void CapNhatLaiDanhSachToaDo(ChuongTrinh_t & ChuongTrinh);

  void HienThiNoiDungChuongTrinh(uint8_t IdxChuongTrinh, uint32_t VpAddress);
  bool DangOViTriHome(void);
  void HienThiThongTinPhanPhoi(void);

  void HienThiIconChuongTrinhDuocChon(uint32_t VpAddressNutNhanDauTien);

  void HienThiThongTinABCH(void);

};

#endif