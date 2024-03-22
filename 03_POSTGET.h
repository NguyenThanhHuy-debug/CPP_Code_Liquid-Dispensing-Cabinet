#ifndef _POSTGET_h
#define _POSTGET_h

#define CODE_POST_GET_CU

#ifdef CODE_POST_GET_CU

#include "KingTubeAuto.h"     // Có define debug để bật/tắt các debug ra Serial.
#include <Arduino.h>      // Để khai báo kiểu String
#include <HTTPClient.h>   // Cho phép khai báo kiểu HTTPClient

class POSTGET {
private:
  HTTPClient http;

public:
  bool POSTDuLieuBoard(String ID, String DuLieu);
  String GETLenhGuiXuongBoard(String ID);
  void CapNhatCODETrongDatabaseTrenServer(String ID, String DuLieu);
};

#else // CODE_POST_GET_CU

#include "KingTubeAuto.h"  // Có define debug để bật/tắt các debug ra Serial.
#include "WiFiClient.h"
#include "EthernetClient.h"

// #define DEBUG_POST_RESP 1

typedef enum{
    HTTP_OVER_WIFI = 0,
    HTTP_OVER_ETH = 1,
} http_stream_t;


class POSTGET {
    public:
        POSTGET(void);

        void SETLuongMang(http_stream_t stream);
        void Huy(void);

        bool POSTDuLieuBoard(String ID, String DuLieu);
        String GETLenhGuiXuongBoard(String ID);
        void CapNhatCODETrongDatabaseTrenServer(String ID, String DuLieu);

    private:
        WiFiClient WF_Cli;
        EthernetClient ETH_Cli;

        Client *client;
        
        http_stream_t stream;
};
#endif //CODE_POST_GET_CU

#endif // _POSTGET_h
