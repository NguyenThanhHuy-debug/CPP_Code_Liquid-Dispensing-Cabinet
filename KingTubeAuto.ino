#include "00_ChuongTrinhChinh.h"

void setup() {
  KhoiTao();
}

void loop(){
#ifdef TEST_SENSORS
  if (digitalRead(X_LIMIT_PIN) == LOW)
    Serial.println("X_LIMIT_PIN");
  if (digitalRead(X2_LIMIT_PIN) == LOW)
    Serial.println("X2_LIMIT_PIN");
  if (digitalRead(Y_LIMIT_PIN) == LOW)
    Serial.println("Y_LIMIT_PIN");
  if (digitalRead(Y2_LIMIT_PIN) == LOW)
    Serial.println("Y2_LIMIT_PIN");
  if (digitalRead(Z_LIMIT_PIN) == LOW)
    Serial.println("Z_LIMIT_PIN");
  // if (digitalRead(Z2_LIMIT_PIN) == LOW)
  //   Serial.println("Z2_LIMIT_PIN");
#endif
  ChayChuongTrinhChinh();
  delay(1);
}