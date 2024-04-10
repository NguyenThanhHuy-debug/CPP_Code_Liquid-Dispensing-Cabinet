#ifndef _Flags_h
#define _Flags_h

//----------------------------------------------------//
//-------- Begin: Các define cho cờ thời gian --------//
//----------------------------------------------------//
// Lưu ý: muốn sử dụng cờ nào thì mở define cho cờ đó.
// #define _Flag_100ms
// #define _Flag_250ms
// #define _Flag_500ms
//#define _Flag_750ms
#define _Flag_1s
//#define _Flag_2s
// #define _Flag_3s
// #define _Flag_4s
//#define _Flag_5s
//#define _Flag_6s
//#define _Flag_7s
//#define _Flag_8s
//#define _Flag_9s
//#define _Flag_10s
//#define _Flag_15s
//#define _Flag_30s
//#define _Flag_35s
//#define _Flag_1m
//#define _Flag_5m
//#define _Flag_10m
// #define _Flag_1h
//----------------------------------------------------//
//-------- End: Các define cho cờ thời gian ----------//
//----------------------------------------------------//


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//============= Begin: KHAI BÁO CÁC FLAGS THỜI GIAN ĐỂ THỰC HIỆN CÁC TÁC VỤ SONG SONG ===============//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
typedef struct {
#ifdef _Flag_100ms
  bool t100ms = 0;
#endif
#ifdef _Flag_250ms
  bool t250ms = 0;
#endif
#ifdef _Flag_500ms
  bool t500ms = 0;
#endif
#ifdef _Flag_750ms
  bool t750ms = 0;
#endif
#ifdef _Flag_1s
  bool t1s = 0;
#endif
#ifdef _Flag_2s
  bool t2s = 0;
#endif
#ifdef _Flag_3s
  bool t3s = 0;
#endif
#ifdef _Flag_4s
  bool t4s = 0;
#endif
#ifdef _Flag_5s
  bool t5s = 0;
#endif
#ifdef _Flag_6s
  bool t6s = 0;
#endif
#ifdef _Flag_7s
  bool t7s = 0;
#endif
#ifdef _Flag_8s
  bool t8s = 0;
#endif
#ifdef _Flag_9s
  bool t9s = 0;
#endif
#ifdef _Flag_10s
  bool t10s = 0;
#endif
#ifdef _Flag_15s
  bool t15s = 0;
#endif
#ifdef _Flag_30s
  bool t30s = 0;
#endif
#ifdef _Flag_35s
  bool t35s = 0;
#endif
#ifdef _Flag_1m
  bool t1m = 0;
#endif
#ifdef _Flag_5m
  bool t5m = 0;
#endif
#ifdef _Flag_10m
  bool t10m = 0;
#endif
#ifdef _Flag_1h
  bool t1h = 0;
#endif
} flagType;
//extern flagType _Flag;
//------------ Begin: Biến mốc thời gian bắt đầu phất cờ --------------//
typedef struct {
#ifdef _Flag_100ms
  unsigned long t100ms = 0;
#endif
#ifdef _Flag_250ms
  unsigned long t250ms = 0;
#endif
#ifdef _Flag_500ms
  unsigned long t500ms = 0;
#endif
#ifdef _Flag_750ms
  unsigned long t750ms = 0;
#endif
#ifdef _Flag_1s
  unsigned long t1s = 0;
#endif
#ifdef _Flag_2s
  unsigned long t2s = 0;
#endif
#ifdef _Flag_3s
  unsigned long t3s = 0;
#endif
#ifdef _Flag_4s
  unsigned long t4s = 0;
#endif
#ifdef _Flag_5s
  unsigned long t5s = 0;
#endif
#ifdef _Flag_6s
  unsigned long t6s = 0;
#endif
#ifdef _Flag_7s
  unsigned long t7s = 0;
#endif
#ifdef _Flag_8s
  unsigned long t8s = 0;
#endif
#ifdef _Flag_9s
  unsigned long t9s = 0;
#endif
#ifdef _Flag_10s
  unsigned long t10s = 0;
#endif
#ifdef _Flag_15s
  unsigned long t15s = 0;
#endif
#ifdef _Flag_30s
  unsigned long t30s = 0;
#endif
#ifdef _Flag_35s
  unsigned long t35s = 0;
#endif
#ifdef _Flag_1m
  unsigned long t1m = 0;
#endif
#ifdef _Flag_5m
  unsigned long t5m = 0;
#endif
#ifdef _Flag_10m
  unsigned long t10m = 0;
#endif
#ifdef _Flag_1h
  unsigned long t1h = 0;
#endif
} startType;
//extern startType _StartTimer;
//------------ End: Biến mốc thời gian bắt đầu phất cờ ----------------//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//============= Begin: KHAI BÁO CÁC FLAGS THỜI GIAN ĐỂ THỰC HIỆN CÁC TÁC VỤ SONG SONG ===============//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

class Flags {
private:
  startType StartTimer;

public:
  flagType Flag;

public:
  void TurnONFlags(void);
  void TurnOFFFlags(void);
};


#endif
