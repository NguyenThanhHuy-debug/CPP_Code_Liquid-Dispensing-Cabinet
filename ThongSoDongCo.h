#pragma once
// clang-format off

/*
    3axis_v4.h
    Part of Grbl_ESP32

    Pin assignments for the ESP32 Development Controller, v4.1 and later.
    https://github.com/bdring/Grbl_ESP32_Development_Controller
    https://www.tindie.com/products/33366583/grbl_esp32-cnc-development-board-v35/

    2018    - Bart Dring
    2020    - Mitch Bradley

    Grbl_ESP32 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Grbl is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Grbl_ESP32.  If not, see <http://www.gnu.org/licenses/>.
*/

//* Tên máy
#define MACHINE_NAME            "KingAutoTube-M"

//* Số lượng trục được sử dụng
#define N_AXIS 4

//* Thời gian giữa mỗi xung
#define DEFAULT_STEP_PULSE_MICROSECONDS 10

//* Sau khi thực hiện xong một lệnh Gcode thì kích chân EN để tắt các động cơ một khoảng thời gian
//* Đặt 255 để không tắt, luôn giữ vị trí do phải giữ vị trí của trục Z, tránh bị tụt
#define DEFAULT_STEPPER_IDLE_LOCK_TIME 255 // stay on

//* Khai báo các chân EN, STEP, DIR, LIMIT của các trục
#define STEPPERS_DISABLE_PIN    GPIO_NUM_25

#define X_STEP_PIN              GPIO_NUM_26
#define X_DIRECTION_PIN         GPIO_NUM_27

#define Y_STEP_PIN              GPIO_NUM_13
#define Y_DIRECTION_PIN         GPIO_NUM_5

#define Z_STEP_PIN              GPIO_NUM_18
#define Z_DIRECTION_PIN         GPIO_NUM_14

#define A_STEP_PIN              GPIO_NUM_23
#define A_DIRECTION_PIN         GPIO_NUM_19

#define X_LIMIT_PIN             36 //* limit 1        //cảm biến hành trình đầu trục X
// #define X2_LIMIT_PIN            39 //limit 2        //cảm biến hành trình cuối trục X
#define Y_LIMIT_PIN             34 //* limit 3        //cảm biến hành trình đầu trục Y
// #define Y2_LIMIT_PIN            35 //limit 4     //cảm biến Y2 hiên tại đang bị hư nên không dùng
#define Z_LIMIT_PIN             32 //* limit 5        //cảm biến hành trình đầu trục Z    
// #define Z2_LIMIT_PIN            33               //khi mở máy thì cảm biến Z2 sẽ bị kích hoạt, nên không dùng


//* Khai báo số xung trên 1mm để dùng lệnh Gcode
//* VD X200 => trục X di chuyển 200mm
//* Có thể thay đổi để phù hợp với đơn vị đo
//* Công thức: (Xung trên vòng) * (vi bước) / (khoảng cách trên vòng)
#define DEFAULT_X_STEPS_PER_MM  160 //* $100 //200 bước/vòng, vi bước 32, puly 20 răng, bước răng 2mm => 200 * 32 / (20 * 2) = 160
#define DEFAULT_Y_STEPS_PER_MM  160 //* $101 
#define DEFAULT_Z_STEPS_PER_MM  160 //* $102
#define DEFAULT_A_STEPS_PER_MM  6400 //* $103 1 vòng / phút

//* Khai báo tốc độ di chuyển tối đa của các trục
#define DEFAULT_X_MAX_RATE      10000.0 //* mm/min $110
#define DEFAULT_Y_MAX_RATE      10000.0 //* mm/min $111
#define DEFAULT_Z_MAX_RATE      10000.0 //* mm/min $112
#define DEFAULT_A_MAX_RATE      500.0 //* vòng/phút $113 

//* Khai báo gia tốc của các trục
#define DEFAULT_X_ACCELERATION  300 //* mm/sec^2. 50 mm/sec^2 = 180000 mm/min^2 $120
#define DEFAULT_Y_ACCELERATION  300 //* mm/sec^2. 50 mm/sec^2 = 180000 mm/min^2 $121
#define DEFAULT_Z_ACCELERATION  300 //* mm/sec^2. 50 mm/sec^2 = 180000 mm/min^2 $122
#define DEFAULT_A_ACCELERATION  50 //* mm/sec^2. 50 mm/sec^2 = 180000 mm/min^2 $123

//* Khoảng cách di chuyển tối đa của các trục
//* Phụ thuộc vào kích thước máy
//* Chính là cảm biến hành trình mềm
//!Phải là giá trị dương
#define DEFAULT_X_MAX_TRAVEL    1000 //* mm $130 
#define DEFAULT_Y_MAX_TRAVEL    1000 //* mm $131
#define DEFAULT_Z_MAX_TRAVEL    2000 //* mm $132
// #define DEFAULT_A_MAX_TRAVEL    1000 //* mm $133

//* Cho phép dùng cảm biến hành trình
#define DEFAULT_HARD_LIMIT_ENABLE 1  // $21 false

//* Cho phép dùng lệnh Set Home
#define DEFAULT_HOMING_ENABLE 1

//* Đơn vị theo mm
#define DEFAULT_REPORT_INCHES 0 

//* Hướng di chuyển của các trục khi Set Home
/////////////Ngược với hướng di chuyển bình thường để tới cảm biến hành trình đầu
#define DEFAULT_HOMING_DIR_MASK  (bit(X_AXIS))

//* Không cho phép di chuyển lúc mới mở máy
//* Chỉ di chuyển khi đã Set Home
#define HOMING_INIT_LOCK 

//? Cảm biến hành trình không bị dội thì nên dùng
//? File config.h đề xuất
// #define HARD_LIMIT_FORCE_STATE_CHECK 

//* Kiểm tra cảm biến hành trình tại thời điểm khởi động
#define CHECK_LIMITS_AT_INIT

// *Tốc độ di chuyển khi Set Home
//* Đặt tốc độ chậm để tránh va đập mạnh
#define DEFAULT_HOMING_SEEK_RATE 5000.0 

//* Tốc độ di chuyển sau khi gặp được cảm biến hành trình
//* Sau khi gặp được cảm biến hành trình thì di chuyển về phía ngược lại để không kích hoạt cảm biến nữa
#define DEFAULT_HOMMING_FEED_RATE 1000.0

//* Khoảng cách di chuyển ngược lại sau khi xác định được vị trí Home
#define DEFAULT_HOMING_PULLOFF 5.0

//* Thứ tự di chuyển của các trục khi Set Home
//* Đầu tiên di chuyển trục Z, sau đó di chuyển trục X và Y
#define DEFAULT_HOMING_CYCLE_0 (bit(Z_AXIS))
#define DEFAULT_HOMING_CYCLE_1  (bit(X_AXIS) | bit(Y_AXIS))

//* Đổi hướng di chuyển bình thường của trục XYZ để phù hợp với HDSD của máy
#define DEFAULT_DIRECTION_INVERT_MASK 0
