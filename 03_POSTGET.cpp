#include "03_POSTGET.h"

#ifdef CODE_POST_GET_CU
// #include "03_POSTGET.h"
#include <WiFi.h>  // Thư viện WiFi của ESP32.

#define URL_GETLenhGuiXuongBoard "https://App.IoTVision.vn/api/LABone_KingIncu_DuLieuGuiXuongBoard?CheDo=1&key="
#define URL_POSTDuLieuTuBoardLenServer "https://App.IoTVision.vn/api/LABone_KingIncu_DuLieu"
#define URL_CapNhatCODE "https://App.IoTVision.vn/api/LABone_KingIncu_DuLieuGuiXuongBoard"

// Thiết lập thời gian timeout.
#define timeout 1000

//=================================================================================
// Ví dụ, DuLieu = "1089;210;13.3;20:06:13 18/08/2023"
// 1089 => K1 = 1, MODE = 0 (MAN), RSSI WiFi = 89%
// Tốc độ lắc = 210 (RPM)
// Thời gian lắc = 13.3 (giờ)
// HH:MM:SS DD/MM/YYYY = 20:06:13 18/08/2023
//=================================================================================
bool POSTGET::POSTDuLieuBoard(String ID, String DuLieu) {
  if (ID.isEmpty()) {
    Serial.printf("\n\n\t ID TRONG \n\n");
  }
  if (DuLieu.isEmpty()) {
    Serial.printf("\n\n\t DuLieu TRONG \n\n");
  }
  try {
    if (WiFi.status() == WL_CONNECTED) {
      //this->http.setTimeout(timeout);  // Thiết lập thời gian timeout.
      this->http.begin(URL_POSTDuLieuTuBoardLenServer);
      this->http.addHeader("Content-Type", "application/json");
      String data = "{\"ID\":\"" + ID + "\",\"S\":\"" + DuLieu + "\"}";
      this->http.POST(data);  //Send the request 
      this->http.end();

      return 1;
    } else
      return 0;
  } catch (String error) {
    return 0;
  }
}

String POSTGET::GETLenhGuiXuongBoard(String ID) {
  try {
    if (WiFi.status() == WL_CONNECTED) {
      String url = URL_GETLenhGuiXuongBoard + ID;
      //this->http.setTimeout(timeout);  // Thiết lập thời gian timeout.
      this->http.begin(url);
      this->http.addHeader("Content-Type", "application/json");
#if defined(debug) && defined(POST_GET)
      unsigned long ms = millis();
#endif
      int httpCodeGet = this->http.GET();
      if (httpCodeGet == HTTP_CODE_OK) {  // needs a 200 to continue...
        String data = this->http.getString();
        data.remove(0, 1);                  // Loại ký tự '[' tại đầu chuỗi.
        data.remove(data.length() - 1, 1);  // Loại ký tự ']' tại cuối chuỗi.
        this->http.end();

#if defined(debug) && defined(POST_GET)
        Serial.print(millis() - ms);
        Serial.print("(ms) for GET ");
        Serial.println(data);
#endif

        return data;
      } else {
#if defined(debug) && defined(POST_GET)
        Serial.println(this->http.errorToString(httpCodeGet).c_str());
#endif
        this->http.end();
        return "";
      }
    } else {
#if defined(debug) && defined(POST_GET)
      Serial.println("Mất kết nối WIFI!");
#endif
      return "";
    }
  } catch (String error) {
    return "";
  }
}

//=================================================================================
// Ví dụ, DuLieu = "1011"
// K1 = 1, MODE = 0 (MAN), CODE = 11
//=================================================================================
void POSTGET::CapNhatCODETrongDatabaseTrenServer(String ID, String DuLieu) {
  if (WiFi.status() == WL_CONNECTED) {
    //this->http.setTimeout(timeout);  // Thiết lập thời gian timeout.
    this->http.addHeader("Content-Type", "application/json");
    this->http.begin(URL_CapNhatCODE);
    this->http.addHeader("Content-Type", "application/json");

    String data = "{\"ID\":\"" + ID + "\",\"S\":\"" + DuLieu + "\"}";
    this->http.POST(data);  //Send the request
    this->http.end();
  }
}

#else

#include <WiFi.h>  // Thư viện WiFi của ESP32.

#define HOST "App.IoTVision.vn"
#define GET_PATH "/api/LABone_KingIncu_DuLieuGuiXuongBoard?CheDo=1&key="
#define POST_PATH "/api/LABone_KingIncu_DuLieu"
#define UPCODE_POST_PATH "/api/LABone_KingIncu_DuLieuGuiXuongBoard"

#define HTTP_TIMEOUT 3000

POSTGET::POSTGET(void) {
    client = &WF_Cli;
}

void POSTGET::SETLuongMang(http_stream_t stream) {
    this->stream = stream;
    if (this->stream == HTTP_OVER_ETH) client = &ETH_Cli;
    else client = &WF_Cli;
}

void POSTGET::Huy(void) {
    client->stop();
}
//=================================================================================
// Ví dụ, DuLieu = "1089;210;13.3;20:06:13 18/08/2023"
// 1089 => K1 = 1, MODE = 0 (MAN), RSSI WiFi = 89%
// Tốc độ lắc = 210 (RPM)
// Thời gian lắc = 13.3 (giờ)
// HH:MM:SS DD/MM/YYYY = 20:06:13 18/08/2023
//=================================================================================
bool POSTGET::POSTDuLieuBoard(String ID, String DuLieu) {
#if defined(debug) && defined(DEBUG_POST_GET) 
    if (ID.isEmpty()) {
        Serial.printf("\n\n\t ID TRONG \n\n");
    }
    if (DuLieu.isEmpty()) {
        Serial.printf("\n\n\t DuLieu TRONG \n\n");
    }
#endif
    if (WiFi.status() == WL_CONNECTED) {
        uint32_t time_start;
        String data = "{\"ID\": \"" + ID + "\", \"S\": \"" + DuLieu + "\"}";

        String post_req = "POST " + String(POST_PATH) + " HTTP/1.1\r\n"
            "Host: " + String(HOST) + "\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: " + String(data.length()) + "\r\n"
            "Connection: close\r\n"
            "\r\n" + data + "\r\n";
        if (client->connect(HOST, 80)) {
#if defined(debug) && defined(DEBUG_POST_GET) 
            Serial.println("Connected to server, starting to post data!");
#endif
            client->print(post_req);
        }
        else {
#if defined(debug) && defined(DEBUG_POST_GET) 
            Serial.println("connection failed");
#endif
            goto disconnect;
        }
#if defined(debug) && defined(DEBUG_POST_GET) 
        time_start = millis();
        while (client->connected() && ((millis() - time_start) <= HTTP_TIMEOUT)) {
            if (client->available()) {
                char c = client->read();
                Serial.print(c);
            }
        }
#endif

        return 1;
    disconnect:
        client->stop();
#if defined(debug) && defined(DEBUG_POST_GET) 
        Serial.println("Post data successful, Disconnected to server!");
#endif
        return 0;
    }
    return 0;
}

String POSTGET::GETLenhGuiXuongBoard(String ID) {
    if (WiFi.status() == WL_CONNECTED) {
        uint32_t time_start;
        int startIndex, endIndex;
        String post_req = "GET " + static_cast<String>(GET_PATH + ID) + " HTTP/1.1\r\n"
            "Host: " + static_cast<String>(HOST) + "\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n"
            "\r\n";
        String response_content;

        if (client->connect(HOST, 80)) {
#if defined(debug) && defined(DEBUG_POST_GET) 
            Serial.println("Connected to server, starting to get data!");
#endif
            client->print(post_req);
        }
        else {
#if defined(debug) && defined(DEBUG_POST_GET) 
            Serial.println("connection failed");
#endif
            goto disconnect;
        }

        time_start = millis();
        while (client->connected() && ((millis() - time_start) <= HTTP_TIMEOUT)) {
            if (client->readStringUntil('\n') == "\r") break;
        }
        time_start = millis();
        while (client->connected() && ((millis() - time_start) <= HTTP_TIMEOUT)) {
            response_content += client->readStringUntil('\n');
        }

        startIndex = response_content.indexOf('[');//[{ nội dung   }]
        endIndex = response_content.indexOf(']');


        if (startIndex >= 0 && endIndex >= 0) {
            response_content = response_content.substring(startIndex + 1, endIndex);

        }
        else {
#if defined(debug) && defined(DEBUG_POST_GET) 
            Serial.println("String error");
#endif
        }
    disconnect:
        client->stop();
#if defined(debug) && defined(DEBUG_POST_GET) 
        Serial.print("dữ liệu nhận từ serve: ");
        Serial.println(response_content);
        Serial.println("Get data successful, Disconnected to server!");
#endif
        return response_content;
    }
    else {
#if defined(debug) && defined(DEBUG_POST_GET)
        Serial.println("Mất kết nối WIFI!");
#endif
        return "";
    }
    return "";
}

//=================================================================================
// Ví dụ, DuLieu = "1011"
// K1 = 1, MODE = 0 (MAN), CODE = 11
//=================================================================================
void POSTGET::CapNhatCODETrongDatabaseTrenServer(String ID, String DuLieu) {
    if (WiFi.status() == WL_CONNECTED) {
        uint32_t time_start;
        String data = "{\"ID\":\"" + ID + "\",\"S\":\"" + DuLieu + "\"}";
        String post_req = "POST " + String(UPCODE_POST_PATH) + " HTTP/1.1\r\n"
            "Host: " + String(HOST) + "\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: " + String(data.length()) + "\r\n"
            "Connection: close\r\n"
            "\r\n" + data + "\r\n";

        if (client->connect(HOST, 80)) {
#if defined(debug) && defined(DEBUG_POST_GET) 
            Serial.println("Connected to server, starting to post data!");
#endif
            client->print(post_req);
        }
        else {
#if defined(debug) && defined(DEBUG_POST_GET) 
            Serial.println("connection failed");
#endif
            goto disconnect;
        }
#if defined(debug) && defined(DEBUG_POST_GET) 
        time_start = millis();
        while (client->connected() && ((millis() - time_start) <= HTTP_TIMEOUT)) {
            if (client->available()) {
                char c = client->read();
                Serial.print(c);
            }
        }
#endif

    disconnect:
        client->stop();
#if defined(debug) && defined(DEBUG_POST_GET) 
        Serial.println("Post data successful, Disconnected to server!");
#endif
    }
}
#endif //CODE_POST_GET_CU
