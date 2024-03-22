#include "01_Flags.h"
#include <Arduino.h> // Gọi thư viện Arduino.h để có thể sử dụng hàm millis()

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//========================== Begin: FLAGS ĐIỀU KHIỂN ĐA NHIỆM =======================================//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// Bật các cờ lấy mốc thời gian thực hiện các tác vụ.
// Luôn luôn gọi ở đầu vòng loop().
//===============================================================
void Flags::TurnONFlags(void) {
    #ifdef _Flag_100ms
        if ((millis() - StartTimer.t100ms) >= 100) {
            Flag.t100ms = true;
            StartTimer.t100ms = millis();
        }
    #endif
    #ifdef _Flag_250ms
        if ((millis() - StartTimer.t250ms) >= 250) {
            Flag.t250ms = true;
            StartTimer.t250ms = millis();
        }
    #endif
    #ifdef _Flag_500ms
        if ((millis() - StartTimer.t500ms) >= 500) {
            Flag.t500ms = true;
            StartTimer.t500ms = millis();
        }
    #endif
    #ifdef _Flag_750ms
        if ((millis() - StartTimer.t750ms) >= 750) {
            Flag.t750ms = true;
            StartTimer.t750ms = millis();
        }
    #endif
    #ifdef _Flag_1s
        if ((millis() - StartTimer.t1s) >= 1000) {
            Flag.t1s = true;
            StartTimer.t1s = millis();
        }
    #endif
    #ifdef _Flag_2s
        if ((millis() - StartTimer.t2s) >= 2000) {
            Flag.t2s = true;
            StartTimer.t2s = millis();
        }
    #endif
    #ifdef _Flag_3s
        if ((millis() - StartTimer.t3s) >= 3000) {
            Flag.t3s = true;
            StartTimer.t3s = millis();
        }
    #endif
    #ifdef _Flag_4s
        if ((millis() - StartTimer.t4s) >= 4000) {
            Flag.t4s = true;
            StartTimer.t4s = millis();
        }
    #endif
    #ifdef _Flag_5s
        if ((millis() - StartTimer.t5s) >= 5000) {
            Flag.t5s = true;
            StartTimer.t5s = millis();
        }
    #endif
    #ifdef _Flag_6s
        if ((millis() - StartTimer.t6s) >= 6000) {
            Flag.t6s = true;
            StartTimer.t6s = millis();
        }
    #endif
    #ifdef _Flag_7s
        if ((millis() - StartTimer.t7s) >= 7000) {
            Flag.t7s = true;
            StartTimer.t7s = millis();
        }
    #endif
    #ifdef _Flag_8s
        if ((millis() - StartTimer.t8s) >= 8000) {
            Flag.t8s = true;
            StartTimer.t8s = millis();
        }
    #endif
    #ifdef _Flag_9s
        if ((millis() - StartTimer.t9s) >= 9000) {
            Flag.t9s = true;
            StartTimer.t9s = millis();
        }
    #endif
    #ifdef _Flag_10s
        if ((millis() - StartTimer.t10s) >= 10000) {
            Flag.t10s = true;
            StartTimer.t10s = millis();
        }
    #endif
    #ifdef _Flag_15s
        if ((millis() - StartTimer.t15s) >= 15000) {
            Flag.t15s = true;
            StartTimer.t15s = millis();
        }    
    #endif
    #ifdef _Flag_30s
        if ((millis() - StartTimer.t30s) >= 30000) {
            Flag.t30s = true;
            StartTimer.t30s = millis();
        }
    #endif
    #ifdef _Flag_35s
        if ((millis() - StartTimer.t35s) >= 35000) {
            Flag.t35s = true;
            StartTimer.t35s = millis();
        }
    #endif
    #ifdef _Flag_1m
        if ((millis() - StartTimer.t1m) >= 60000) {
            Flag.t1m = true;
            StartTimer.t1m = millis();
        }
    #endif
    #ifdef _Flag_5m
        if ((millis() - StartTimer.t5m) >= 300000) {
            Flag.t5m = true;
            StartTimer.t5m = millis();
        }
    #endif
    #ifdef _Flag_10m
        if ((millis() - StartTimer.t10m) >= 600000) {
            Flag.t10m = true;
            StartTimer.t10m = millis();
        }
    #endif
    #ifdef _Flag_1h
        if ((millis() - StartTimer.t1h) >= 3600000) {
            Flag.t1h = true;
            StartTimer.t1h = millis();
        }
    #endif
}
//===============================================================
// Tắt các cờ lấy mốc thời gian thực hiện các tác vụ.
// Luôn luôn gọi ở cuối vòng loop().
//===============================================================
void Flags::TurnOFFFlags(void) {
    #ifdef _Flag_100ms
        Flag.t100ms  =   false;
    #endif
    #ifdef _Flag_250ms
        Flag.t250ms  =   false;
    #endif
    #ifdef _Flag_500ms
        Flag.t500ms  =   false;
    #endif
    #ifdef _Flag_750ms
        Flag.t750ms  =   false;
    #endif
    #ifdef _Flag_1s
        Flag.t1s  =   false;
    #endif
    #ifdef _Flag_2s
        Flag.t2s  =   false;
    #endif
    #ifdef _Flag_3s
        Flag.t3s  =   false;
    #endif
    #ifdef _Flag_4s
        Flag.t4s  =   false;
    #endif
    #ifdef _Flag_5s
        Flag.t5s  =   false;
    #endif
    #ifdef _Flag_6s
        Flag.t6s  =   false;
    #endif
    #ifdef _Flag_7s
        Flag.t7s  =   false;
    #endif
    #ifdef _Flag_8s
        Flag.t8s  =   false;
    #endif
    #ifdef _Flag_9s
        Flag.t9s  =   false;
    #endif
    #ifdef _Flag_10s
        Flag.t10s  =   false;
    #endif
    #ifdef _Flag_15s
        Flag.t15s  =   false;
    #endif
    #ifdef _Flag_30s
        Flag.t30s  =   false;
    #endif
    #ifdef _Flag_35s
        Flag.t35s  =   false;
    #endif
    #ifdef _Flag_1m
        Flag.t1m  =   false;
    #endif
    #ifdef _Flag_5m
        Flag.t5m  =   false;
    #endif
    #ifdef _Flag_10m
        Flag.t10m  =   false;
    #endif
    #ifdef _Flag_1h
        Flag.t1h  =   false;
    #endif
}
//===============================================================
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//========================== End: FLAGS ĐIỀU KHIỂN ĐA NHIỆM =========================================//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
