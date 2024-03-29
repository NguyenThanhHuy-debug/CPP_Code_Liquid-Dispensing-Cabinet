Luồng xử lý của chương trình

1. Trong hàm setup() (chỉ chạy 1 lần), hàm KhoiTao() trong ChuongTrinhChinh được gọi, thực hiện các công việc như sau:
    - Khởi tạo GRBL (đọc các thông số đã cài từ trước cho máy chạy trong flash của ESP32, tạo các task serial, grbl).
    - Khởi tạo ThongSoBoard(đọc các chương trình từ eeprom ngoại, nếu dữ liệu đọc được không phải ở dạng JSON gồm các keys thì tạo các chương trình mặc định và lưu lại vào eeprom).
    - Khởi tạo DWIN (tạo hàm callback khi có dữ liệu gửi xuống từ HMI).

2. Các tasks (FreeRTOS) bao gồm:
    - Loop: đọc thời gian từ RTC DS3231 mỗi 1s.
    - GRBL: xử lý các lệnh điều khiển STEP.
    - Serial: Xử lý lệnh GRBL nhận được qua serial (sử dụng WebUI::inputBuffer.* để thay cho serial uart).
    - PhanPhoiTask: điều khiển STEP phân phối tại các ống có tọa độ trong chương trình được gửi xuống từ DWIN (chạy khi được notify khi nhấn nút RUN trên màn hình).
    - vTaskDWINCode: xử lý tương tác từ người dùng trên HMI.
    - xTaskFunction_RunTime: xử lý hiển thị các ống đã được phân phối, các thông số khác trên _TrangHienThiPhanPhoiThoiGianThuc_OFF_HOME (được resume khi chạy, khi chạy xong hoặc tạm dừng thì suspend task đi).

3. Luồng xử lý (không bao gồm Loop, GRBL, Serial):
    - PhanPhoiTask: Khi nhận được notify, hàm BatDauPhanPhoi được gọi. Lý do dùng hàm là vì phải kết thúc task khi cần người dùng không muốn chạy phân phối nữa (Nhấn RUN để dừng, sau đó nhấn EXIT để thoát), khi đó chỉ cần return hàm BatDauPhanPhoi để task tiếp tục chờ notify (nếu không dùng hàm mà chạy lệnh trực tiếp trong task thì khi return sẽ làm RTOS bị lỗi, reset lại ESP32). Quá trình chạy hàm BatDauPhanPhoi:
        + Sắp xếp thứ tự chạy của các chương trình được chọn dựa vào TrangThaiChuongTrinh và ThuTuChuongTrinhCanChay của từng chương trình.
        + Chạy các chương trình theo thứ tự đã sắp xếp.
        + Phân phối tại các tọa độ trong DanhSachToaDo của chương trình.
        + Tính khoảng cách giữa các ống theo theo chiều X và Y.
        + Sắp xếp lại thứ tự phân phối để có được quãng đường chạy ngắn nhất.
        + Tính SoVongDeChongNhoGiot antidrop = chongnhogiot (góc quay ngược trong chương trình) / 360. Do 1 đơn vị của trục A là quay 1 vòng, nên muốn quay bao nhiêu độ thì chia cho 360 độ (1 vòng). Sau đó đảo dấu ngược dấu của TheTichBomDuocKhiQuay1Vong để quay ngược lại.
        + Tính SoVongCanBom (số vòng để bơm được đúng thể tích) = thể tích cần bơm * 1000 / (TheTichBomDuocKhiQuay1Vong * đường kính ống). Trong đó, TheTichBomDuocKhiQuay1Vong là thể tích dung dịch bơm được từ ống khi máy bơm quay 1 vòng rồi chia cho hệ số đường kính ống.
        + Tạo các chuỗi lệnh GRBL để điều khiển máy sau đó push vào inputBuffer để thực hiện. Ở cuối các chuỗi phải có '\r' hoặc '\n' để GRBL xử lý.
        + Tạo chuỗi lệnh GRBL để di chuyển tới vị trí của ống với format "G1G90X(ToaDoX)Y(ToaDoY)Z(ToaDoZ)F(TocDo)\r". Công thức tính các tọa độ:
            * ToaDoX = Vị trí X của điểm A + Tọa độ ma trận X của điểm cần đến * Khoảng cách giữa 2 ống theo X.
            * ToaDoY = Vị trí Y của điểm A + Tọa độ ma trận Y của điểm cần đến * Khoảng cách giữa 2 ống theo Y.            
            * ToaDoZ = Độ cao H trong chương trình.
            * TocDo = Tốc độ chạy của trục XY trong chương trình.
        + Tạo chuỗi lệnh GRBL để quay máy bơm với format "G1G91A(SoVong)F(TocDo)\r":
            * SoVong: là SoVongCanBom đã tính được ở trên.
            * TocDo: tốc độ bơm trong chương trình.
        + Delay(100) để chờ GRBL nhận và xử lý lệnh.
        + Chờ GRBL phân phối xong.
        + Tạo chuỗi lệnh GRBL để quay ngược chống nhỏ giọt với format "G1G91A(SoVong)F(TocDo)\r":
            * SoVong: là SoVongDeChongNhoGiot đã tính được ở trên.
            * TocDo: tốc độ bơm trong chương trình.
        + Chờ GRBL chống nhỏ giọt xong.
        + Cập nhật ToaDoHienTai là tọa độ đã bơm được.
        + Bước chờ GRBL chạy xong là do không thể gửi hết tất cả lệnh xuống một lúc được mà phải gửi rồi chờ đến khi thực hiện xong.
        + Trong khi chờ, chương trình liên tục kiểm tra xem người dùng có nhấn EXIT không thông qua biến CoBaoDungPhanPhoi, nếu có thì return ngay lập tức, không bị delay.


    - vTaskDwinCode: 
        + Các biến quan trọng, được dùng thường xuyên:
            * IndexChuongTrinhDangXuLy: index của _ChuongTrinhEEPROM đang xử lý (từ 0 -> 9).
            * Mode: _CheDoHoatDongSimple hoặc _CheDoHoatDongMulti.
            * IdxKhuVucDangXuLy: index của _ChuongTrinhEEPROM được chọn khi nhấn các nút MODIFY1 -> 5.
            * DSIndexChuongTrinhHienThi: array chứa index của các _ChuongTrinhEEPROM hiện ở trang _TrangCaiDatCheDoChuongTrinh_HOME.
            * CheDoVaoTrangChonChuongTrinh: là trang hiện 10 chương trình để chọn, tương ứng với mỗi nút nhấn mà có 1 chế độ riêng như thay đổi chương trình chạy, chọn chương trình để SETUP, COPY, sau khi nhấn nút OK hoặc EXIT lúc cài đặt thông số chương trình, nút CHANGE đổi chương trình khác lúc chọn vị trí phân phối, nút SELECT chọn chương trình CALIB, nút SELECT chọn chương trình trang CONFIG.
            * codeHMI: nằm trong dữ liệu nhân được từ DWIN, thường là keycode của nút vừa nhấn.
            * lastcodeHMI: keycode của nút nhấn trước đó. Do DWIN hay gửi dữ liệu rác hoặc trùng lặp xuống nên cần dùng để lọc ra và thực hiện một số chức năng khác.

        + LangNgheDWIN: đọc dữ liệu trả về từ DWIN.
        + ChuyenTrangTheoBamChonCuaUserTrenHMI: khi nhấn 1 nút, nếu đủ điều kiện thì sẽ dẫn đến trang tiếp theo.
        + XuLySetHome: (Xử lý ở trang _TrangBatDauChaySetHome)
            * Khi đang ở trang _TrangBatDauChaySetHome thì hiện ngày, giờ, Step, Feed, Feed mặc định ở trang MANUAL CONFIG và trang Get ABCH, vẽ khung hình chữ nhật ở phần chọn tọa độ phân phối, đảm bảo DanhSachToaDo của các trang đều full (chỉ chạy một lần).
            * Khi nhấn nút GO HOME thì gửi lệnh xuống cho GRBL thực hiện lệnh HOMING, sau khi thực hiện xong thì gửi lên TrangThaiXacNhanSetHome, sau đó mới chuyển sang trang _TrangCaiDatCheDoChuongTrinh_HOME.
            * Khi nhấn GO HOME hoặc _HMIBamNutBackTrangCaiDat thì hiển thị lại tên các chương trình ở các ô 1 -> 5 trang _TrangCaiDatCheDoChuongTrinh_HOME.
        + XuLyChonMode: (xử lý ở trang _TrangCaiDatCheDoChuongTrinh_HOME)
            * Khi nhấn nút _HMIBamNutModeTrangCaiDatCheDoChuongTrinh (nút đổi chế độ giữa SIMPLE và MULTI) thì tên và nội dung chương trình ở phần bên trái bị xóa đi, đồng thời reset TrangThaiChuongTrinh và ThuTuChay của tất cả các chương trình hiển thị 1 -> 5, đồng thời đảo Mode thành _CheDoHoatDongMulti hoặc _CheDoHoatDongSimple.
        + XuLyChonChuongTrinhSeChay: (xử lý ở trang _TrangCaiDatCheDoChuongTrinh_HOME)
            * Khi thoát ra từ _TrangHienThiPhanPhoiThoiGianThuc_OFF_HOME (EXIT) hoặc trang _TrangCaiDat (BACK) thì xóa tên và nội dung chương trình được chọn ở phần bên trái trang _TrangCaiDatCheDoChuongTrinh_HOME.
            * Khi nhấn vào nút chọn chương trình (dấu tick V hoặc X): cập nhật IndexChuongTrinhDangXuLy, đảo TrangThaiChuongTrinh của chương trình đó, đổi icon V hoặc X tương ứng. Nếu đang ở chế độ SIMPLE thì có chương trình được chọn là DuocChon, còn lại là KhongDuocChon. Nếu TrangThaiChuongTrinh của chương trình vừa chọn là KhongDuocChon (bỏ tick V) thì xóa tên và nội dung chương trình bên trái. Còn nếu là DuocChon thì hiện tên và nội dung.
            * Nếu chế độ hiện tại là vào trang từ các nút nhấn PROGRAM 1 -> 5 thì cập nhật IndexChuongTrinhDangXuLy dựa vào nút đã nhấn, và hiện tên 10 chương trình. Nếu chọn 1 trong 10 chương trình thì hiện thông tin chương trình đó bên phải. Nếu nhấn nút OK kiểm tra chương trình được chọn có trùng với các chương trình hiển thị bên ngoài không (dựa vào DSIndexChuongTrinhHienThi), nếu không thì set TrangThai của chương trình cũ là KhongDuocChon, cập nhật DSIndexChuongTrinhHienThi, hiện lại tên 5 chương trình.
        + XuLyThietLapChuongTrinhCheDoMulti: (xử lý ở trang _TrangThietLapCheDoChayMulti)
            * Nếu nhấn vào các nút MODIFY 1 -> 5 thì cập nhật IndxChuongTrinhDangModify, hiện tên chương trình ở trang _TrangPhanPhoiViTri.
            * Nếu nhấn nút X ở mục TIME LOOP, thì cho phép chạy MULTI 1 lần
            * Nếu nhấn nút Second ở mục TIME LOOP để đặt thời gian thì chạy MULTI nhiều lần.
        + XuLyChonViTriPhanPhoi: (xử lý ở trang _TrangPhanPhoiViTri)
            * Sử dụng biến static tạm để chứa ChuongTrinhDangXuLy và KhayChonTamThoi(chứa ống đã chọn) để backup khi người dùng nhấn EXIT nhằm giữ lại giá trị tọa độ ban đầu.
            * Khi bấm nút RUN ở trang _TrangCaiDatCheDoChuongTrinh_HOME, nếu đang ở chế độ SIMPLE, quét qua các chương trình kiểm tra chương trình nào được chọn thì cho ThuTuChay = 0 để gửi xuống GRBL, sau đó hiển thị thông tin ở trang chọn vị trí, sau đó vẽ các ô để chọn ống cần phân phối.
             Nếu ở chế độ MULTI, clear DSIndexChuongTrinhDuocChon để sau đó push_back lại tất cả và set ThuTuChay tương ứng. Sau đó hiển thị thông tin chương trình lên 5 ô MODIFY.
             Khi nhấn các nút MODIFY 1->5, cập nhật IndexChuongTrinhDangXuLy và backup chương trình đang chọn, khaychon bằng chương trình tạm thời, KhayChonTamThoi. Vẽ các ô để chọn dựa vào số hàng và cột của ChuongTrinh. Trạng thái chọn của ống vẫn giữ nguyên từ trước.
             Khi nhấn FULL, clear rồi push_back lại toàn bộ DsToaDo, đổi màu thành Đỏ(chọn).
             Khi nhấn CLEAR, clear DsToaDo rồi đổi thành màu Trắng(không chọn).
             Khi nhấn nút ADD, tùy thuộc vào lần nhấn trước đó mà sẽ vào chế độ ADD, SUB, hoặc tắt chức năng này đi. Khi vào ADD hoặc SUB và nhấn vào vùng chọn vị trí ống, HMI sẽ gửi xuống phản hồi có chứa vị trí vừa nhấn, nếu vị trí này nằm trong khoảng của hình chữ nhật thì sẽ push_back hoặc remove vị trí này trong DsToaDo.
             Khi nhấn EXIT, cập nhật lại ChuongTrinh và khaychon bằng 2 biến backup TamThoi như trên. Tức là nếu nhấn OK, thay đổi có hiệu lực trên ChuongTrinh gốc, còn nếu nhấn EXIT thì hủy bỏ thay đổi đó đi.
             Khi nhấn OK, nếu ở MULTI, hiện thị lại thông tin ở 5 chương trình MODIFY. Còn nếu ở SIMPLE, thì cập nhật lại DSIndexChuongTrinhDuocChon, vẽ khung ở trang _TrangHienThiPhanPhoiThoiGianThuc_OFF_HOME, hiển thị lại màu được chọn.
        + XuLyConfigChuongTrinh:
            * Nếu nhấn nút _HMIBamNutSetupProgramTrangCaiDat, thì hiện tên các chương trình 1->10.
            * Nếu chọn Chương trình 1->10 thì hiện thông tin chương trình tương ứng.
            * Nếu nhấn OK thì hiện thị thông tin chi tiết chương trình ở ô bên phải trang _VPTextThongSoTrangThietLapChuongTrinh.
        + XuLySetupProgram:
            * Ở chế độ COPY, khi nhấn nút COPY, cập nhật lastprogram, nếu nhấn chương trình 1->10, cập nhật IndexChuongTrinhDangXuLy, hiển thị thông tin chương trình. Nếu nhấn OK, thì chương trình trước đó đã được chọn để COPY sẽ thay đổi thành chương trình vừa được chọn. Nếu nhấn EXIT thì cập nhật lại IndexChuongTrinhDangXuLy.
            * Nếu nhấn _HMIBamNutNEXTTrangLayViTriChonDiem thì hiện thông tin chi tiết chương trình.
            * Nếu nhấn _HMIBamNutNEXTTrangThietLapChuongTrinhDiChuyen thì hiện thông tin ABC của chương trình.
            * Nếu nhấn LOW, MEDIUM, HIGH thì thay đổi STEP và FEED tương ứng.
            * Nếu nhấn GET thì cập nhật vị trí A, B, C, H tương ứng.