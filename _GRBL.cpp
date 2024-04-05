#include "_GRBL.h"
#define debug_grbl
bool _GRBL::VeViTriHome()
{
  if (sys.state == State::Homing)
     return false; //* tránh set home liên tục
  if (sys.state == State::Alarm)
  {
    WebUI::inputBuffer.write(0x18);
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

//* Hàm này sẽ khởi tạo các tọa độ sẽ đi qua tùy vào người dùng bấm chọn và sẽ lưu tọa độ vào 1 vector
//* Sau đó sẽ sử dụng vector đó và được sắp xếp dựa vào thuật toán tìm đường ngắn nhất
double KhoangCachGiua2Diem(const toadoINT_t& Diem1, const toadoINT_t& Diem2) {
  double dx = Diem1.X - Diem2.X;
  double dy = Diem1.Y - Diem2.Y;
  return sqrt(dx * dx + dy * dy);
}

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


  //* Xử lý vụ multi với simple
  char* ViTri;
  char* SoVongDeBomDungTheTich;
  char* ChongNhoGiot;

  DaChayXongCacKhuVuc = false;

  for (auto i : KhuVucChay)
  {
    Serial.printf("Thu tu chay: %d\n", i);
  }
  OngDaPhanPhoi = -1;
  // Chạy các chương trình theo thứ tự đã sắp xếp
  for (uint8_t i = 0; i < sizeof(KhuVucChay); i++)
  {
    int8_t IdxChuongTrinh = KhuVucChay[i];
    if (IdxChuongTrinh == -1) continue;
    //* Nếu chương trình không được chọn hoặc không có ống cần phân phối thì bỏ qua


    OngDaPhanPhoi = -1;

    TinhKhoangCachGiuaHaiOng(_DSChuongTrinh[IdxChuongTrinh], i);
    //* Sắp xếp lại thứ tự di chuyển của các ống cần phân phối
    SapXepThuTuOng(_DSChuongTrinh[IdxChuongTrinh].DsToaDo[i]);

    // SoVongDeChongNhoGiot = (signbit(_DSChuongTrinh[i].SoVongDeBom1ml) ? 1 : -1) * (_DSChuongTrinh[i].chongnhogiot / 360.0); //* đảm bảo chiều chống nhỏ giọt luôn ngược với chiều bơm
    float SoVongDeChongNhoGiot = (signbit(_DSChuongTrinh[IdxChuongTrinh].TheTichBomDuocKhiQuay1Vong) ? 1 : -1) * (_DSChuongTrinh[IdxChuongTrinh].chongnhogiot / 360.0); //* đảm bảo chiều chống nhỏ giọt luôn ngược với chiều bơm
    

    // SoVongCanBom = _DSChuongTrinh[i].SoVongDeBom1ml * _DSChuongTrinh[i].thetich * 1000 * _DSChuongTrinh[i].duongkinhong;
    float SoVongCanBom = (_DSChuongTrinh[IdxChuongTrinh].thetich * 1000 / (_DSChuongTrinh[IdxChuongTrinh].TheTichBomDuocKhiQuay1Vong * _DSChuongTrinh[IdxChuongTrinh].duongkinhong)) ;

    vector<toadoINT_t> DSToaDoCanChay = _DSChuongTrinh[IdxChuongTrinh].DsToaDo[i];

    for (auto ToaDo: _DSChuongTrinh[IdxChuongTrinh].DsToaDo[i])
    {
      Serial.printf("Toa do: %d %d\n", ToaDo.X, ToaDo.Y);
    }

    toado_t ViTriACanChay = _DSChuongTrinh[IdxChuongTrinh].ViTriA[i];
    toado_t ViTriBCanChay = _DSChuongTrinh[IdxChuongTrinh].ViTriB[i];
    toado_t ViTriCCanChay = _DSChuongTrinh[IdxChuongTrinh].ViTriC[i];
    float ViTriHCanChay = _DSChuongTrinh[IdxChuongTrinh].ViTriH[i];

    for (uint8_t ToaDoHienTai = 0; ToaDoHienTai < DSToaDoCanChay.size(); ToaDoHienTai++)
    {

      Serial.printf("ToaDoXDaChay: %d, ToaDoYDaChay: %d\n", ToaDoXDaChay, ToaDoYDaChay);
      //* Di chuyển đến vị trí X Y Z
      asprintf(&ViTri, "G1G90X%gY%gZ%gF%d\r", ViTriACanChay.X + DSToaDoCanChay[ToaDoHienTai].X * _KhoangCachGiuaHaiOng.X,
                                              ViTriACanChay.Y + DSToaDoCanChay[ToaDoHienTai].Y * _KhoangCachGiuaHaiOng.Y, ViTriHCanChay, 
                                              _DSChuongTrinh[IdxChuongTrinh].tocdoXY); //thêm F để đặt tôc độ trục XY
      WebUI::inputBuffer.push(ViTri);
      free(ViTri);

      //* Xoay ống bơm 
      asprintf(&SoVongDeBomDungTheTich, "G1G91A%gF%d\r", SoVongCanBom,
        _DSChuongTrinh[IdxChuongTrinh].tocdobom); //thêm F để đặt tôc độ trục A
      WebUI::inputBuffer.push(SoVongDeBomDungTheTich);
      free(SoVongDeBomDungTheTich);

      asprintf(&ChongNhoGiot, "G1G91A%gF%d\r", SoVongDeChongNhoGiot, _DSChuongTrinh[IdxChuongTrinh].tocdobom);
      WebUI::inputBuffer.push(ChongNhoGiot);
      free(ChongNhoGiot);
      //* phải delay để grbl xử lý xong lệnh
      delay(200);

      while ((sys.state == State::Cycle) || (sys.state == State::Hold) ) 
      {
        if (CoBaoDungPhanPhoi == true) {
          CoBaoDungPhanPhoi = false;
          if (CoBaoPhanPhoiLaiOng == true)
          {
            WebUI::inputBuffer.push("~\n");
            CoBaoPhanPhoiLaiOng = false;
            return;
          }
          Serial.println("TaskPhanPhoi DungPhanPhoi");
          return;
        }
        if (CoBaoPhanPhoiLaiOng == true)
          {
            break;
          }
        delay(10);
      }

      if (CoBaoPhanPhoiLaiOng == true)
      {
        ToaDoHienTai--;
        CoBaoPhanPhoiLaiOng = false;
        continue;
      }

      OngDaPhanPhoi = ToaDoHienTai;
      ToaDoXDaChay = _DSChuongTrinh[IdxChuongTrinh].DsToaDo[i][ToaDoHienTai].X;
      ToaDoYDaChay = _DSChuongTrinh[IdxChuongTrinh].DsToaDo[i][ToaDoHienTai].Y;
      KhuVucDangChay = i;
      Serial.printf("OngDaPhanPhoi: %d\n", OngDaPhanPhoi);
    }
  }
  DaChayXongCacKhuVuc = true;
  VeToaDoGoc();
}
//* Trả về vị trí hiện tại của STEP
float _GRBL::ViTriHienTaiCuaSTEP(uint8_t STEP) {
  return sys_position[STEP] / axis_settings[STEP]->steps_per_mm->get();
}


void _GRBL::TamDungPhanPhoi(void) {
  if (CoBaoPhanPhoiLaiOng == true)
  {
    vTaskSuspend(_PhanPhoiTaskHandle);
  }
  WebUI::inputBuffer.push("!\r");
}

void _GRBL::TiepTucPhanPhoi(void) {
  if (CoBaoPhanPhoiLaiOng == true)
  {
    vTaskResume(_PhanPhoiTaskHandle);
    return;
  }
  WebUI::inputBuffer.push("~\n");
  // vTaskResume(_PhanPhoiTaskHandle);
}

void _GRBL::DungPhanPhoi(void) {

  if (CoBaoPhanPhoiLaiOng == true)
  {
    WebUI::inputBuffer.push("!\n");
    CoBaoDungPhanPhoi = true;
    delay(100);
    vTaskResume(_PhanPhoiTaskHandle);
    return;
  }
  CoBaoDungPhanPhoi = true;
  delay(100); /* NOTE delay(100)*/
  WebUI::inputBuffer.write(0x18);
  delay(100); /* NOTE delay(100)*/
  WebUI::inputBuffer.push("$X\n");
  delay(100);
}


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

//* Hàm tính khoảng cách giữa 2 ống theo X, Y dựa vào vị trí A, B, C của chương trình
void _GRBL::TinhKhoangCachGiuaHaiOng(ChuongTrinh_t ChuongTrinh, uint8_t KhuVuc) {

  if (ChuongTrinh.rackX == 1)
  {
    _KhoangCachGiuaHaiOng.X = 0;
  }
  else
  {
  _KhoangCachGiuaHaiOng.X = (ChuongTrinh.ViTriC[KhuVuc].X - ChuongTrinh.ViTriA[KhuVuc].X) / (float)(ChuongTrinh.rackX - 1);
  }

  if (ChuongTrinh.rackY == 1)
  {
    _KhoangCachGiuaHaiOng.Y = 0;
  }
  else
  {
  _KhoangCachGiuaHaiOng.Y = (ChuongTrinh.ViTriC[KhuVuc].Y - ChuongTrinh.ViTriA[KhuVuc].Y) / (float)(ChuongTrinh.rackY - 1);
  }

  #ifdef debug_grbl
  Serial.printf("Khoang cach giua hai ong: %g %g\n", _KhoangCachGiuaHaiOng.X, _KhoangCachGiuaHaiOng.Y);
  #endif
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

void _GRBL::Calib(ChuongTrinh_t * ChuongTrinh)
{
  ChuongTrinh->TheTichBomDuocKhiQuay1Vong = (float)(ChuongTrinh->actualweight * 1000.0f / (ChuongTrinh->density * ChuongTrinh->duongkinhong * ViTriHienTaiCuaSTEP(MayBom)));
}

void _GRBL::TestFill(ChuongTrinh_t ChuongTrinh)
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

void _GRBL::VeToaDoGoc(void)
{
  if (sys.state == State::Alarm)
  {
    WebUI::inputBuffer.write(0x18);
    delay(100);
    WebUI::inputBuffer.push("$H\n");
    delay(100);
    return;
  }
  DiChuyenToiViTri(ToaDoGoc, TocDoTrungBinh);
}

_GRBL _grbl;
