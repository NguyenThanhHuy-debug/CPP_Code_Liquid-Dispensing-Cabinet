#include "_GRBL.h"
// #define debug_grbl
bool _GRBL::VeViTriHome()
{
  if (sys.state == State::Homing)
     return false; //* tránh set home liên tục
  if (sys.state == State::Alarm)
  {
    WebUI::inputBuffer.push("$X\r"); //* unlock khi đang gặp lỗi
    delay(100);
  }
  WebUI::inputBuffer.push("$H\r"); //* set home
  sys_position[MayBom] = 0; //* reset vị trí máy bơm
  digitalWrite(2, HIGH);
  delay(500);
  digitalWrite(2, LOW);
  while (sys.state == State::Homing){
      delay(100);
  }
  return true;
}
void _GRBL::LayChuongTrinhTuEEPROM()
{
  //* Cập nhật danh sách chương trình
}
//* Hàm này sẽ khởi tạo các tọa độ sẽ đi qua tùy vào người dùng bấm chọn và sẽ lưu tọa độ vào 1 vector
//* Sau đó sẽ sử dụng vector đó và được sắp xếp dựa vào thuật toán tìm đường ngắn nhất
double KhoangCachGiua2Diem(const toadoINT_t& Diem1, const toadoINT_t& Diem2) {
  double dx = Diem1.X - Diem2.X;
  double dy = Diem1.Y - Diem2.Y;
  return sqrt(dx * dx + dy * dy);
}
// vector<uint8_t> ThuatToanTimDuongDiNganNhat(const vector<toadoINT_t>& DanhSachToaDo) {
//   int SoLuongToaDo = DanhSachToaDo.size();
//   vector<bool> visited(SoLuongToaDo, false);
//   vector<uint8_t> route;
//   route.push_back(0);  // Bắt đầu từ điểm (0,0)
//   visited[0] = true;

//   for (int i = 1; i < SoLuongToaDo; i++) {
//     double minDistance = numeric_limits<double>::max();
//     int nearestNeighbor = -1;

//     for (int j = 0; j < SoLuongToaDo; j++) {
//       if (!visited[j]) {
//         double distance = KhoangCachGiua2Diem(DanhSachToaDo[route.back()], DanhSachToaDo[j]);
//         if (distance < minDistance) {
//           minDistance = distance;
//           nearestNeighbor = j;
//         }
//       }
//     }

//     if (nearestNeighbor != -1) {
//       route.push_back(nearestNeighbor);
//       visited[nearestNeighbor] = true;
//     }
//   }

//   return route;
// }

vector<uint8_t> ThuatToanTimDuongDiNganNhat(const vector<toadoINT_t>& DanhSachToaDo) {
  int SoLuongToaDo = DanhSachToaDo.size();
  vector<bool> visited(SoLuongToaDo, false);
  vector<uint8_t> route;

  // Tìm điểm gần nhất với điểm (0, 0)
  double minDistance = numeric_limits<double>::max();
  int nearestNeighbor = -1;
  for (int i = 0; i < SoLuongToaDo; i++) {
    double distance = KhoangCachGiua2Diem({0, 0}, DanhSachToaDo[i]);
    if (distance < minDistance) {
      minDistance = distance;
      nearestNeighbor = i;
    }
  }

  // Nếu tìm được điểm gần nhất, bắt đầu từ đó
  if (nearestNeighbor != -1) {
    route.push_back(nearestNeighbor);
    visited[nearestNeighbor] = true;

    for (int i = 1; i < SoLuongToaDo; i++) {
      minDistance = numeric_limits<double>::max();
      nearestNeighbor = -1;

      for (int j = 0; j < SoLuongToaDo; j++) {
        if (!visited[j]) {
          double distance = KhoangCachGiua2Diem(DanhSachToaDo[route.back()], DanhSachToaDo[j]);
          if (distance < minDistance) {
            minDistance = distance;
            nearestNeighbor = j;
          }
        }
      }

      if (nearestNeighbor != -1) {
        route.push_back(nearestNeighbor);
        visited[nearestNeighbor] = true;
      }
    }
  }

  return route;
}


void _GRBL::SapXepThuTuOng(vector<toadoINT_t>& DANHSACHTOADO)
{

  vector<toadoINT_t> DanhSachToaDoTamThoi = DANHSACHTOADO;

  //* Tìm đường ngắn nhất 
  vector<uint8_t> ViTriUuTien = ThuatToanTimDuongDiNganNhat(DANHSACHTOADO);

  DANHSACHTOADO.clear();

  //* Cho vào danh sách tọa độ 
  for (uint8_t i = 0; i < ViTriUuTien.size();i++)
  {
    DANHSACHTOADO.push_back(DanhSachToaDoTamThoi[ViTriUuTien[i]]);
  }

}

void _GRBL::BatDauPhanPhoi()
{

  // Chờ đến khi về Home xong
  // while (sys.state == State::Homing) delay(1);
  // TODO: Hiển thị các giá trị của danh sách tọa độ lên màn hình dwin
  // TODO: Hiển thị thời gian bắt đầu 0:00

  //* Xử lý vụ multi với simple
  char* ViTri;
  char* SoVongDeBomDungTheTich;
  char* ChongNhoGiot;

  float SoVongCanBom;
  float SoVongDeChongNhoGiot;

#ifdef debug_grbl
  for (uint8_t i = 0; i < _DSChuongTrinh.size(); i++){
    Serial.printf("Chuongtrinh: %s, ToaDoHienTai: %d\n", _DSChuongTrinh[i].tenchuongtrinh.c_str(), _DSChuongTrinh[i].ToaDoHienTai);
    if (_DSChuongTrinh[i].TrangThaiChuongTrinh == DuocChon){
      Serial.printf("ChuongTrinh %s - Size %d\n", _DSChuongTrinh[i].tenchuongtrinh.c_str(), _DSChuongTrinh[i].DanhSachToaDo.size());
    }
  }

  for (uint8_t i = 0; i < _DSChuongTrinh.size(); i++)
  {
    if (_DSChuongTrinh[i].TrangThaiChuongTrinh == DuocChon)
    {
      for (uint8_t j = 0; j < _DSChuongTrinh[i].DanhSachToaDo.size(); j++)
      {
        Serial.printf("ChuongTrinh: %s, Toa do X: %d, Y: %d\n", _DSChuongTrinh[i].tenchuongtrinh.c_str(), _DSChuongTrinh[i].DanhSachToaDo[j].X, _DSChuongTrinh[i].DanhSachToaDo[j].Y);
        _DSChuongTrinh[i].ToaDoHienTai = -1;
      }
    }
  }

#endif

  // Sắp xếp thứ tự chạy của các chương trình được chọn dựa vào TrangThaiChuongTrinh và ThuTuChuongTrinhCanChay
  int8_t ThuTuChuongTrinhCanChay[SoLuongChuongTrinhChayToiDa];
  memset(ThuTuChuongTrinhCanChay, -1, sizeof(ThuTuChuongTrinhCanChay));

  for (uint8_t i = 0; i < _DSChuongTrinh.size(); i++)
  {
    Serial.printf("ChuongTrinh: %s, TrangThai: %d, ThuTu: %d, Size: %d\n", _DSChuongTrinh[i].tenchuongtrinh.c_str(), _DSChuongTrinh[i].TrangThaiChuongTrinh, _DSChuongTrinh[i].ThuTuChay,  _DSChuongTrinh[i].DanhSachToaDo.size());

    if (_DSChuongTrinh[i].TrangThaiChuongTrinh == DuocChon)
    {
      ThuTuChuongTrinhCanChay[_DSChuongTrinh[i].ThuTuChay] = i;
    }
  }

  for (auto i : ThuTuChuongTrinhCanChay)
  {
    Serial.printf("Thu tu chay: %d\n", i);
  }

  // Chạy các chương trình theo thứ tự đã sắp xếp
  for (uint8_t i = 0; i < sizeof(ThuTuChuongTrinhCanChay); i++)
  {
    int8_t IdxChuongTrinh = ThuTuChuongTrinhCanChay[i];
    if (IdxChuongTrinh == -1) continue;
    //* Nếu chương trình không được chọn hoặc không có ống cần phân phối thì bỏ qua
    if ((_DSChuongTrinh[IdxChuongTrinh].TrangThaiChuongTrinh == KhongDuocChon) || (_DSChuongTrinh[IdxChuongTrinh].DanhSachToaDo.size() == 0)) continue;
    #ifdef debug_grbl
    for (uint8_t ToaDo = 0; ToaDo < _DSChuongTrinh[IdxChuongTrinh].DanhSachToaDo.size();ToaDo++)
    {
      Serial.printf("Toa do %d: %d %d\n", ToaDo, _DSChuongTrinh[IdxChuongTrinh].DanhSachToaDo[ToaDo].X, _DSChuongTrinh[IdxChuongTrinh].DanhSachToaDo[ToaDo].Y);
    }
    #endif

    TinhKhoangCachGiuaHaiOng(_DSChuongTrinh[IdxChuongTrinh]);
    //* Sắp xếp lại thứ tự di chuyển của các ống cần phân phối
    SapXepThuTuOng(_DSChuongTrinh[IdxChuongTrinh].DanhSachToaDo);

    // SoVongDeChongNhoGiot = (signbit(_DSChuongTrinh[i].SoVongDeBom1ml) ? 1 : -1) * (_DSChuongTrinh[i].chongnhogiot / 360.0); //* đảm bảo chiều chống nhỏ giọt luôn ngược với chiều bơm
    SoVongDeChongNhoGiot = (signbit(_DSChuongTrinh[IdxChuongTrinh].TheTichBomDuocKhiQuay1Vong) ? 1 : -1) * (_DSChuongTrinh[IdxChuongTrinh].chongnhogiot / 360.0); //* đảm bảo chiều chống nhỏ giọt luôn ngược với chiều bơm

    // SoVongCanBom = _DSChuongTrinh[i].SoVongDeBom1ml * _DSChuongTrinh[i].thetich * 1000 * _DSChuongTrinh[i].duongkinhong;
    SoVongCanBom = (_DSChuongTrinh[IdxChuongTrinh].thetich * 1000 / (_DSChuongTrinh[IdxChuongTrinh].TheTichBomDuocKhiQuay1Vong * _DSChuongTrinh[IdxChuongTrinh].duongkinhong)) ;

    for (uint8_t ToaDoHienTai = 0; ToaDoHienTai < _DSChuongTrinh[IdxChuongTrinh].DanhSachToaDo.size(); ToaDoHienTai++)
    {
      //* Di chuyển đến vị trí X Y Z
      asprintf(&ViTri, "G1G90X%gY%gZ%gF%d\r", _DSChuongTrinh[IdxChuongTrinh].vitriA.X + _DSChuongTrinh[IdxChuongTrinh].DanhSachToaDo[ToaDoHienTai].X * _KhoangCachGiuaHaiOng.X,
        _DSChuongTrinh[IdxChuongTrinh].vitriA.Y + _DSChuongTrinh[IdxChuongTrinh].DanhSachToaDo[ToaDoHienTai].Y * _KhoangCachGiuaHaiOng.Y,
        _DSChuongTrinh[IdxChuongTrinh].docaoH,
        _DSChuongTrinh[IdxChuongTrinh].tocdoXY); //thêm F để đặt tôc độ trục XY
      WebUI::inputBuffer.push(ViTri);
      #ifdef debug_grbl
      Serial.printf("X: %g Y: %g Z: %g\n", _DSChuongTrinh[IdxChuongTrinh].vitriA.X + _DSChuongTrinh[IdxChuongTrinh].DanhSachToaDo[ToaDoHienTai].X * _KhoangCachGiuaHaiOng.X,
        _DSChuongTrinh[IdxChuongTrinh].vitriA.Y + _DSChuongTrinh[IdxChuongTrinh].DanhSachToaDo[ToaDoHienTai].Y * _KhoangCachGiuaHaiOng.Y,
        _DSChuongTrinh[IdxChuongTrinh].docaoH);
      #endif
      free(ViTri);

      //* Xoay ống bơm 
      asprintf(&SoVongDeBomDungTheTich, "G1G91A%gF%d\r", SoVongCanBom,
        _DSChuongTrinh[IdxChuongTrinh].tocdobom); //thêm F để đặt tôc độ trục A
      WebUI::inputBuffer.push(SoVongDeBomDungTheTich);
      free(SoVongDeBomDungTheTich);

      //* phải delay để grbl xử lý xong lệnh
      delay(100);
      //* Chờ đến khi máy bơm quay được đúng bằng số vòng cần để bơm đúng thể tích */
      //* Hoặc khi máy bị mất điện => ALARM
      while ((sys.state == State::Cycle) || (sys.state == State::Hold)) {
        if (CoBaoDungPhanPhoi == true) {
          return;
        }
        delay(1);
      }

      //Kiểm tra xem góc chống nhỏ giọt có khác 0
      if (_DSChuongTrinh[IdxChuongTrinh].chongnhogiot == 0) continue;

      //* Chống nhỏ giọt
      //* Hiện tại đã bơm xong, cần quay ngược lại để chống nhỏ giọt
      if (sys.state != State::Alarm) {
        //* 1 bước của bơm hiện tại là 6400 xung => 1 vòng
        //* đơn vị của chống nhỏ giọt là độ => 1 / 360 (vòng)
        //*dùng A- do đây là quay ngược lại và trước đó đã reset lại vị trí máy bơm
        // float SoVongDeChongNhoGiot = -(_DSChuongTrinh[i].chongnhogiot / 360.0);
        asprintf(&ChongNhoGiot, "G1G91A%gF%d\r", SoVongDeChongNhoGiot, _DSChuongTrinh[IdxChuongTrinh].tocdobom);
        WebUI::inputBuffer.push(ChongNhoGiot);
        free(ChongNhoGiot);
        //* phải delay để grbl xử lý xong lệnh
        delay(100);
        //* Chờ đến khi máy bơm chống nhỏ giọt xong
        while ((sys.state == State::Cycle) || (sys.state == State::Hold)) {
          if (CoBaoDungPhanPhoi == true) {
            return;
          }
          delay(1);
        }

        if (sys.state != State::Alarm) {
          _DSChuongTrinh[IdxChuongTrinh].ToaDoHienTai = ToaDoHienTai;
        }
        else {
          break;
        }
      }
      else {
        break;
      }
    }
  }
  VeViTriHome();
}
//* Trả về vị trí hiện tại của STEP
float _GRBL::ViTriHienTaiCuaSTEP(uint8_t STEP) {
  return sys_position[STEP] / axis_settings[STEP]->steps_per_mm->get();
}
//* Nhấn nút RESET PROGRAME ở RUNTIME USER INTERFACE để chạy lại chương trình
void _GRBL::ResetChuongTrinh()
{
  // TODO: Hiển thị các giá trị reset lại từ đầu
  BatDauPhanPhoi();
}

void _GRBL::TamDungPhanPhoi(void) {
  WebUI::inputBuffer.push("!\r");
  // vTaskSuspend(_PhanPhoiTaskHandle);
}

void _GRBL::TiepTucPhanPhoi(void) {
  WebUI::inputBuffer.push("~\r");
  // vTaskResume(_PhanPhoiTaskHandle);
}

void _GRBL::DungPhanPhoi(void) {
  CoBaoDungPhanPhoi = true;
  delay(1); /* NOTE delay(100)*/
  WebUI::inputBuffer.write(0x18);
  CoBaoDungPhanPhoi = false;
  delay(600); /* NOTE delay(100)*/
  WebUI::inputBuffer.push("$X\n");
  delay(100);
}

void _GRBL::BatDauCalib(chuongtrinh_t& ChuongTrinh) {
  sys_position[MayBom] = 0;

  char* ViTri;
  asprintf(&ViTri, "A1000F%d\r", ChuongTrinh.tocdobom);
  WebUI::inputBuffer.push("A1000\r");
  free(ViTri);

  //TODO: Bắt đầu đếm thời gian
}

void _GRBL::DungCalib(chuongtrinh_t& ChuongTrinh) {
  TamDungChay();
}

// void _GRBL::LuuCalib(chuongtrinh_t& ChuongTrinh) {
//   float SoVongDaQuayCuaMayBom = ViTriHienTaiCuaSTEP(MayBom);
//   float TheTichThuc = _KhoiLuongThuc / _KhoiLuongRieng;
//   _SoVongDeBomDungTheTich = ChuongTrinh.thetich * SoVongDaQuayCuaMayBom / TheTichThuc;
// }


//* Các hàm điều khiển X+, X-, Y+, Y-, Z+, Z-
//* Chieu là hướng di chuyển, Duong hoặc Am
//* Buoc là số mm di chuyển mỗi lần nhấn
void _GRBL::DiChuyenX(bool Chieu, float Buoc, float TocDo) {
  char* ViTri;

  //* nếu chiều âm - thì thêm dấu "-" để xoay ngược chiều
  //* Thêm G91 để dùng vị trí tương đối
  if (Chieu == Duong) {
    asprintf(&ViTri, "G1G91X%3.3fF%3.3f\r", Buoc, TocDo); //* thêm F để đặt tôc độ trục XY
  }
  else {
    asprintf(&ViTri, "G1G91X-%3.3fF%3.3f\r", Buoc, TocDo); //* thêm F để đặt tôc độ trục XY
  }

  WebUI::inputBuffer.push(ViTri);
  free(ViTri);
}

void _GRBL::DiChuyenY(bool Chieu, float Buoc, float TocDo) {
  char* ViTri;

  if (Chieu == Duong) {
    asprintf(&ViTri, "G1G91Y%3.3fF%3.3f\r", Buoc, TocDo); //* thêm F để đặt tôc độ trục XY
  }
  else {
    asprintf(&ViTri, "G1G91Y-%3.3fF%3.3f\r", Buoc, TocDo); //* thêm F để đặt tôc độ trục XY
  }

  WebUI::inputBuffer.push(ViTri);
  free(ViTri);
}

void _GRBL::DiChuyenZ(bool Chieu, float Buoc, float TocDo) {
  char* ViTri;

  if (Chieu == Duong) {
    asprintf(&ViTri, "G1G91Z%3.3fF%3.3f\r", Buoc, TocDo);
  }
  else {
    asprintf(&ViTri, "G1G91Z-%3.3fF%3.3f\r", Buoc, TocDo);
  }

  WebUI::inputBuffer.push(ViTri);
  free(ViTri);
}

//* Task chạy GRBL liên tục
//* Cần dùng một hàm static gọi đến Task cần dùng để truyền vào xTaskCreate
void _GRBL::StaticGRBLTask(void* _this) {
  ((_GRBL*)_this)->GRBLTask();
}

void _GRBL::GRBLTask(void) {
  while (1) {
    run_once();
    vTaskDelay(1);
  }
}

//* Task phân phối, gọi hàm BatDauPhanPhoi, bắt đầu chạy khi gọi hàm PhanPhoi()
void _GRBL::StaticPhanPhoiTask(void* _this) {
  ((_GRBL*)_this)->PhanPhoiTask();
}

void _GRBL::PhanPhoiTask(void) {
  while (1) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    BatDauPhanPhoi();
  }
}

//* Hàm khởi tạo được gọi trong setup()
//* Khởi tạo task GRBL và Phân phối
//* Các hàm cài đặt chỉ dùng để test chức năng
void _GRBL::KhoiTaoGRBL(void) {
  grbl_init();

  xTaskCreate(StaticGRBLTask,
    "GRBLTask",
    4096,
    this,
    DoUuTienGRBL,
    NULL);

  xTaskCreate(StaticPhanPhoiTask,
    "PhanPhoiTask",
    4096,
    this,
    DoUuTienPhanPhoi,
    &_PhanPhoiTaskHandle);
}

//* Các hàm lấy vị trí của A, B, C, H
//* Tương ứng Get A, Get B, Get C, Get H trên HMI
//* Do không truyền được _ChuongTrinhHienTaiGRBL vào hàm ở file .ino nên tạo thêm các hàm dùng chương trình mặc định là _ChuongTrinhHienTaiGRBL
void _GRBL::LayViTriA(chuongtrinh_t& ChuongTrinh) {
  ChuongTrinh.vitriA.X = ViTriHienTaiCuaSTEP(TrucX);
  ChuongTrinh.vitriA.Y = ViTriHienTaiCuaSTEP(TrucY);
}

void _GRBL::LayViTriB(chuongtrinh_t& ChuongTrinh) {
  ChuongTrinh.vitriB.X = ViTriHienTaiCuaSTEP(TrucX);
  ChuongTrinh.vitriB.Y = ViTriHienTaiCuaSTEP(TrucY);
}

void _GRBL::LayViTriC(chuongtrinh_t& ChuongTrinh) {
  ChuongTrinh.vitriC.X = ViTriHienTaiCuaSTEP(TrucX);
  ChuongTrinh.vitriC.Y = ViTriHienTaiCuaSTEP(TrucY);
}

void _GRBL::LayDoCaoH(chuongtrinh_t& ChuongTrinh) {
  ChuongTrinh.docaoH = ViTriHienTaiCuaSTEP(TrucZ);
}

//* Hàm tính khoảng cách giữa 2 ống theo X, Y dựa vào vị trí A, B, C của chương trình
void _GRBL::TinhKhoangCachGiuaHaiOng(chuongtrinh_t& ChuongTrinh) {

  if (ChuongTrinh.rackX == 1)
  {
    _KhoangCachGiuaHaiOng.X = 0;
  }
  else
  {
  _KhoangCachGiuaHaiOng.X = (ChuongTrinh.vitriC.X - ChuongTrinh.vitriA.X) / (float)(ChuongTrinh.rackX - 1);
  }

  if (ChuongTrinh.rackY == 1)
  {
    _KhoangCachGiuaHaiOng.Y = 0;
  }
  else
  {
  _KhoangCachGiuaHaiOng.Y = (ChuongTrinh.vitriC.Y - ChuongTrinh.vitriA.Y) / (float)(ChuongTrinh.rackY - 1);
  }

  #ifdef debug_grbl
  Serial.printf("Khoang cach giua hai ong: %g %g\n", _KhoangCachGiuaHaiOng.X, _KhoangCachGiuaHaiOng.Y);
  #endif
}

//* Các hàm cài đặt các thông số của chương trình
//* Tốc độ trục XY, RackX, RackY, Tốc độ bơm, Góc chống nhỏ giọt

//* Tốc độ XY
void _GRBL::CaiDatTocDoTrucXY(chuongtrinh_t& ChuongTrinh, uint16_t TocDo) {
  ChuongTrinh.tocdoXY = TocDo;
}

//* Rack X
void _GRBL::CaiDatSoOngTrenMotHang(chuongtrinh_t& ChuongTrinh, uint8_t SoOngTrenMotHang) {
  ChuongTrinh.rackX = SoOngTrenMotHang;
}

//* Rack Y
void _GRBL::CaiDatSoOngTrenMotCot(chuongtrinh_t& ChuongTrinh, uint8_t SoOngTrenMotCot) {
  ChuongTrinh.rackY = SoOngTrenMotCot;
}

//* Thể tích ống
void _GRBL::CaiDatTheTichOng(chuongtrinh_t& ChuongTrinh, float TheTichOng) {
  ChuongTrinh.thetich = TheTichOng;
}

//* Tốc độ bơm
void _GRBL::CaiDatTocDoBom(chuongtrinh_t& ChuongTrinh, uint16_t TocDoBom) {
  ChuongTrinh.tocdobom = TocDoBom;
}

//* Góc chống nhỏ giọt
void _GRBL::CaiDatGocChongNhoGiot(chuongtrinh_t& ChuongTrinh, float GocChongNhoGiot) {
  ChuongTrinh.chongnhogiot = GocChongNhoGiot;
}

//* Hàm gọi task Phân phối
void _GRBL::PhanPhoi(void) {
  xTaskNotifyGive(_PhanPhoiTaskHandle);
}

void _GRBL::TamDungChay(void){
  WebUI::inputBuffer.push("!"); //* Feed Hold
  delay(600); //* nếu gọi ngay lập tức thì sẽ bị Alarm, nên cần phải delay một khoảng thời gian
  WebUI::inputBuffer.write(0x18); //* Xóa các lệnh GCode đang chạy, reset mềm esp32
  delay(100);
}

void _GRBL::DiChuyenToiViTri(float X, float Y, float Z, uint16_t TocDo) {
  char* ViTri;
  asprintf(&ViTri, "G1G90X%gY%gZ%gF%d\r", X, Y, Z, TocDo); //* thêm F để đặt tôc độ trục XY
  WebUI::inputBuffer.push(ViTri);
  free(ViTri);
}

void _GRBL::Calib(chuongtrinh_t * ChuongTrinh)
{
  ChuongTrinh->TheTichBomDuocKhiQuay1Vong = ChuongTrinh->TheTichBomDuocKhiQuay1Vong = (float)(ChuongTrinh->actualweight * 1000.0f / (ChuongTrinh->density * ChuongTrinh->duongkinhong * ViTriHienTaiCuaSTEP(MayBom)));
}

void _GRBL::TestFill(chuongtrinh_t ChuongTrinh)
{
  if (sys.state == State::Idle)
  {
    char* str;
    // float SoVongQuay = ChuongTrinhDangXuLy.SoVongDeBom1ml * ChuongTrinhDangXuLy.thetich * ChuongTrinhDangXuLy.duongkinhong; //bơm đủ thể tích
    float SoVongQuay = ChuongTrinh.thetich * 1000  / (ChuongTrinh.TheTichBomDuocKhiQuay1Vong * ChuongTrinh.duongkinhong);
    asprintf(&str, "G1G91A%3.3fF%d\r", SoVongQuay, ChuongTrinh.tocdobom); 
    WebUI::inputBuffer.push(str);

    #ifdef debug_grbl
    Serial.println("TESTFILL/CALIB");
    Serial.println(str);
    #endif

    // SoVongQuay = (signbit(ChuongTrinhDangXuLy.SoVongDeBom1ml) ? 1 : -1) * ChuongTrinhDangXuLy.chongnhogiot / 360.0f; //* chông nhỏ giọt, chiều chuyển động ngược chiều bơm
    SoVongQuay = (signbit(ChuongTrinh.TheTichBomDuocKhiQuay1Vong) ? 1 : -1) * ChuongTrinh.chongnhogiot / 360.0f; //* chông nhỏ giọt, chiều chuyển động ngược chiều bơm
    asprintf(&str, "G1G91A%3.3fF%d\r", SoVongQuay, ChuongTrinh.tocdobom);
    WebUI::inputBuffer.push(str);
    free(str);
  }

  if (sys.state == State::Cycle)
  {
  _grbl.TamDungChay();
  }

}


_GRBL _grbl;
