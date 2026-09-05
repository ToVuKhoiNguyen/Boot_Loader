## 1. Tổng Quan Hệ Thống
Dự án này triển khai một hệ thống **Dual-Slot Bootloader** an toàn và đáng tin cậy trên vi điều khiển **Silicon Labs BGM220**. Hệ thống được thiết kế để hỗ trợ quá trình cập nhật Firmware qua chuẩn giao tiếp UART sử dụng giao thức **XMODEM-CRC**, đồng thời cung cấp cơ chế dự phòng tuyệt đối chống hiện tượng "Brick" do đứt cáp, mất điện giữa chừng hoặc nạp sai file.

### Các tính năng cốt lõi:
- **Dual-Slot Architecture:** Chia bộ nhớ Flash làm 2 phân vùng (Slot A và Slot B) hoạt động song song.
- **Smart Validation:** Trình xác thực Firmware (Magic Number, Size, CRC32, Vector Address, Stack Pointer).
- **Auto-Recovery:** Tự động phát hiện lỗi và nhảy về (Fallback) Slot dự phòng an toàn.
- **Rollback Mode:** Chế độ ẩn cho phép ép hệ thống quay về phiên bản cũ hơn.
- **Python Build Script:** Công cụ tự động đóng gói (Inject Header 512 Bytes) vào file code thô.


## 2. Kiến Trúc Bộ Nhớ
| Vùng Nhớ | Địa Chỉ Bắt Đầu | Kích Thước | Chức Năng |
| :--- | :--- | :--- | :--- |
| **Bootloader** | `0x00000000` | 64 KB | Chứa mã nguồn của Bootloader (Quản lý boot & nạp) |
| **Slot A** | `0x00010000` | 64 KB | Phân vùng Application A |
| **Slot B** | `0x00020000` | 64 KB | Phân vùng Application B |

### Cấu trúc Firmware Header (512 Bytes)
Mỗi Slot bắt đầu bằng một Header dài 512 Bytes, do script `generate_firmware_header.py` sinh ra:
1. `Magic Number` (4 bytes): Luôn là `0xDEADBEEF`.
2. `Image Size` (4 bytes): Kích thước toàn bộ Firmware.
3. `CRC32` (4 bytes): Mã kiểm tra toàn vẹn.
4. `Vector Address` (4 bytes): Địa chỉ thực của Application (Địa chỉ Slot + `0x200`).
5. `Version` (4 bytes): Phiên bản Firmware.


## 3. Máy Trạng Thái
Luồng thực thi của Bootloader (`boot.c`) được điều khiển bằng nút nhấn **BTN0**:
1. **Default Mode (Không giữ nút):** Nhảy vào Slot có Version cao hơn.
2. **Update Mode (Giữ nút > 3s lúc cấp nguồn):** Kích hoạt UART XMODEM. Chờ nhận file để ghi đè vào Slot cũ hơn.
3. **Rollback Mode (Giữ nút 2s lúc cấp nguồn):** Ép nhảy vào Slot có Version thấp hơn.
4. **Safe Mode:** Nếu cả 2 Slot đều hỏng, nháy đèn đỏ báo hiệu và ép người dùng vào Update Mode.


## 4. Cơ Chế Hoạt Động Của Mã Nguồn

Toàn bộ logic của Bootloader được module hóa chặt chẽ. Dưới đây là cách các luồng xử lý chính hoạt động ở tầng mã nguồn:

### 4.1. File `boot.c`
Đây là nơi chứa hàm `main()` và là trung tâm điều phối toàn bộ hệ thống. Khi vi điều khiển được cấp nguồn, luồng xử lý diễn ra như sau:
- **Khởi tạo phần cứng:** Gọi các hàm khởi tạo LED, Nút bấm và System Clock.
- **Quét trạng thái Nút bấm:** Sử dụng vòng lặp `while` đếm thời gian giữ nút `BTN0` để quyết định mode khởi động:
  - Giữ > 3 giây: Đặt cờ vào `UPDATE_MODE`.
  - Giữ > 2 giây: Đặt cờ vào `ROLLBACK_MODE`.
  - Không giữ: Đặt cờ vào `DEFAULT_MODE`.
- **Ra quyết định:** Dựa trên Mode đã chọn, Bootloader đọc thông tin Header từ 2 Slot bằng hàm `FW_ReadHeader()`. Tùy thuộc vào trạng thái Valid của từng Slot mà thuật toán sẽ chọn `target_base` (địa chỉ để nhảy tới boot hoặc địa chỉ làm mục tiêu để ghi đè file OTA).
- **Thực thi:** Gọi module `jump.c` để nhường quyền điều khiển cho Application.

### 4.2. File `validate.c`
Module này chịu trách nhiệm xác thực tính toàn vẹn của Firmware thông qua hàm `FW_Validate()`. Quá trình kiểm tra diễn ra khắt khe với 5 chốt chặn:
1. **Magic Number Check:** Đọc 4 bytes đầu tiên. Nếu khác `0xDEADBEEF`, từ chối ngay lập tức.
2. **Boundary Check:** Đảm bảo `Image Size` không vượt quá kích thước vật lý của 1 Slot (64KB).
3. **Vector Address Check:** So sánh địa chỉ Vector Table ghi trong Header xem có khớp với vị trí thực tế của Slot đang đứng hay không (chống nạp chéo phân vùng).
4. **Stack Pointer Check:** Kiểm tra giá trị Stack Pointer của Application có nằm trong vùng RAM hợp lệ hay không.
5. **CRC32 Check:** Nếu 4 bước trên đều qua, hệ thống chạy thuật toán mã vòng CRC-32/ISO-HDLC (Đa thức `0xEDB88320`) cho toàn bộ dung lượng Firmware và đối chiếu với mã Hash đã lưu trong Header.

### 4.3. File `uart.c` & `xmodem.c`
- **`uart.c`:** Cung cấp hàm `BTL_Printf` siêu nhẹ thay thế cho `printf` của `<stdio.h>`. Do Bootloader trên BGM220 có tài nguyên RAM/Heap vô cùng hạn hẹp, việc sử dụng `printf` tiêu chuẩn sẽ gây ra lỗi sập nguồn (HardFault). `BTL_Printf` được thiết kế để đẩy thẳng từng ký tự xuống thanh ghi UART, giúp hệ thống in log thoải mái mà không lo hết RAM.
- **`xmodem.c`:** Quản lý giao thức XMODEM-CRC truyền file. Khi mạch vào `UPDATE_MODE`:
  - Liên tục gửi cờ `C` để yêu cầu máy tính bắt đầu truyền.
  - Mỗi packet nhận về (128 bytes payload) đều được kiểm tra mã Sequence (thứ tự gói) và mã CRC16.
  - Nếu đúng, gửi `ACK` và gọi hàm ghi vào bộ nhớ. Nếu sai, gửi `NAK` yêu cầu truyền lại gói đó.

### 4.4. File `flash.c` (Quản lý bộ nhớ)
Xử lý các thao tác tương tác vật lý với vi điều khiển:
- **`FLASH_Erase()`:** Xóa trắng toàn bộ 64KB của Slot mục tiêu (Target) trước khi nạp. Bắt buộc xóa theo từng Page (8KB/Page trên BGM220).
- **`FLASH_WriteWord()`:** Ghi tuần tự từng khối dữ liệu 128 bytes từ XMODEM vào vùng nhớ Flash đã xóa một cách an toàn.

### 4.5. File `jump.c`
Chứa hàm `jump_to_app(uint32_t vector_table_address)`. Để vi điều khiển có thể chạy được đoạn code mới, hàm này phải can thiệp sâu vào thanh ghi lõi (Core Registers) của ARM Cortex-M:
- **Dịch chuyển Vector Table:** Ghi địa chỉ mới vào thanh ghi `SCB->VTOR`.
- **Cập nhật Stack Pointer:** Đọc giá trị đầu tiên của Vector Table và gán vào thanh ghi bằng `__set_MSP()`.
- **Kích hoạt Application:** Đọc địa chỉ thứ hai của Vector Table (tức là hàm `Reset_Handler` của App) và thực hiện một cú nhảy (Jump) bằng con trỏ hàm. 

### 4.6. Python Script
Code thô (`raw.bin`) do trình biên dịch sinh ra không thể tự chạy trong hệ thống Dual-Slot. Script Python này có nhiệm vụ:
- Đọc nội dung file `raw.bin`.
- Tính toán kích thước (Image Size) và mã vòng (CRC32) của toàn bộ file.
- Ghép các thông tin này cùng với `Magic Number`, `Version` (do người dùng truyền vào) và `Vector Address` thành một Header dài chính xác 512 Bytes.
- Nối Header này lên đầu file `raw.bin` để tạo thành file `.bin` hoàn chỉnh có thể truyền qua XMODEM.



## 5. Chạy Thử và Kiểm Tra

Để đảm bảo source code chạy trơn tru trên một máy tính hoàn toàn mới, làm theo các bước sau:

### Bước 5.1: Import Project vào Simplicity Studio v5
1. Giải nén thư mục dự án (`Mock_Project`).
2. Mở Simplicity Studio v5. Chọn **File -> Import...**
3. Browse tới thư mục `Mock_Project`, tích chọn cả 2 project là `Boot_Loader` và `Application`.
4. Nhấn Finish.

### Bước 5.2: Biên dịch và nạp Bootloader
1. Ở cửa sổ *Project Explorer*, chuột phải vào project **`Boot_Loader`** -> Chọn **Build Project** (Biểu tượng Cây búa).
2. Cắm cáp kết nối mạch BGM220 vào máy tính.
3. Chuột phải vào project **`Boot_Loader`** -> Chọn **Run As** -> **Silicon Labs ARM Program**.
4. *Dấu hiệu thành công:* Đèn LED trên mạch sẽ chớp đúp liên tục (Báo hiệu hệ thống đang ở Safe Mode do chưa có Firmware ứng dụng nào được nạp).

### Bước 5.3: Biên dịch Application và Đóng gói
1. Tại project **`Application`**, chuột phải -> **Build Configurations** -> **Set Active** -> Chọn **`Slot_A`**. Nhấn nút **Build**.
2. Đổi cấu hình sang **`Slot_B`**. Nhấn nút **Build**.
3. Mở Terminal / PowerShell / CMD, sử dụng lệnh `cd` để đi vào bên trong thư mục `Application` của dự án.
4. Chạy lệnh sau để đóng gói Firmware:
   ```powershell
   # Sinh ra file test cho Slot A (version 1)
   python post_build/generate_firmware_header.py --slot a --input "GNU ARM v12.2.1 - Slot_A/raw_a.bin" --output slot_a_v1.bin --version 1
   
   # Sinh ra file test cho Slot B (version 2)
   python post_build/generate_firmware_header.py --slot b --input "GNU ARM v12.2.1 - Slot_B/raw_b.bin" --output slot_b_v2.bin --version 2
   ```

### Bước 5.4: Truyền file qua TeraTerm và Kiểm chứng
1. Mở phần mềm TeraTerm, kết nối cổng JLink CDC UART Port (Baudrate **115200**, Data 8 bit, None Parity).
2. Trên mạch BGM220: **Giữ đè nút BTN0** khoảng 3 giây rồi ấn nút Reset (hoặc giữ BTN0 rồi cắm điện).
3. Màn hình TeraTerm hiển thị `UPDATE MODE` và liên tục in ra các chữ `C C C C...` chờ dữ liệu.
4. Trên TeraTerm, chọn **File -> Transfer -> XMODEM -> Send...**. Chọn file `slot_a_v1.bin` (Vừa tạo ở Bước 5.3).
5. Quá trình truyền hoàn tất, hệ thống tự động Reset. Màn hình TeraTerm in ra bảng `SYSTEM STATUS REPORT` thông báo nạp thành công và Bootloader nhảy vào Slot A (Đèn LED nhấp nháy).
6. Lặp lại bước vào Update Mode và gửi tiếp `slot_b_v2.bin`. Hệ thống sẽ tự phân luồng, nạp vào Slot B và chuyển sang đèn Breathing (Sáng mờ dần).

## 6. Kịch Bản Kiểm Thử Bắt Buộc

Để nghiệm thu hệ thống, cần tiến hành kiểm thử toàn diện 12 bài test dưới đây. Lưu ý: **App 1 (Slot A) = Nhấp nháy; App 2 (Slot B) = Breathing.**

### GIAI ĐOẠN 1: KHỞI TẠO & CẬP NHẬT CƠ BẢN
**[Test Case 1]: Khởi tạo nhà máy**
- **Hành động:** Xóa toàn bộ Flash bằng Simplicity Commander. Cắm điện mạch.
- **Kỳ vọng:** TeraTerm báo `Both slots are invalid`. Hệ thống tự vào SAFE MODE.
- **Trạng thái LED:** Nháy đúp liên tục báo lỗi.

**[Test Case 2]: Nạp bản cập nhật đầu tiên (v1)**
- **Hành động:** Ở Safe Mode, giữ `BTN0` > 3s để vào Update Mode. Gửi file `slot_a_v1.bin`.
- **Kỳ vọng:** Mạch tự chọn `Target: SLOT A`. Nạp xong báo `VALID` v1.
- **Trạng thái LED:** Nhấp nháy (App Slot A).

**[Test Case 3]: Cập nhật phiên bản cao hơn (v2)**
- **Hành động:** Giữ `BTN0` > 3s vào Update Mode. Gửi file `slot_b_v2.bin`.
- **Kỳ vọng:** Mạch tự chọn `Target: SLOT B` (tránh ghi đè v1). Nạp xong báo `VALID` v2.
- **Trạng thái LED:** Chuyển sang Breathing (App Slot B).

**[Test Case 4]: Tự động boot phiên bản cao nhất**
- **Hành động:** Nhấn nút Reset bình thường (không giữ gì).
- **Kỳ vọng:** Thuật toán so sánh, quyết định nhảy vào Slot B (v2 > v1).
- **Trạng thái LED:** Tiếp tục Breathing.

### GIAI ĐOẠN 2: GHI ĐÈ & BẰNG PHIÊN BẢN
**[Test Case 5]: Ghi đè phiên bản cũ nhất**
- **Hành động:** Giữ `BTN0` > 3s vào Update Mode. Nạp file `slot_a_v3.bin`.
- **Kỳ vọng:** Mạch chỉ định xóa Slot A (vì v1 < v2). Ghi v3 vào Slot A.

**[Test Case 6]: Cập nhật khi 2 Slot bằng Version**
- **Hành động:** Nạp file `slot_b_v3.bin`. Khi vào Update Mode ở lần tiếp theo, log in ra thông báo 2 bản bằng nhau và tự động chọn hy sinh **Slot B** làm mục tiêu ghi đè.

**[Test Case 7]: Boot thông thường khi bằng Version**
- **Hành động:** Mạch đang chứa A(v3) và B(v3). Nhấn nút Reset (không giữ).
- **Kỳ vọng:** Thuật toán quy định ưu tiên Slot A khi hòa nhau. Nhảy vào Slot A.
- **Trạng thái LED:** Nhấp nháy.

**[Test Case 8]: Khôi phục (Rollback) khi bằng Version**
- **Hành động:** Mạch chứa A(v3) và B(v3). Giữ `BTN0` **2 giây** lúc Reset rồi nhả.
- **Kỳ vọng:** Chế độ ROLLBACK. Ưu tiên Slot B khi hòa nhau.
- **Trạng thái LED:** Breathing.

### GIAI ĐOẠN 3: TÍNH NĂNG ROLLBACK & ANTI-BRICK
**[Test Case 9]: Kích hoạt Rollback khẩn cấp**
- **Tiền đề:** Slot A (v5), Slot B (v4).
- **Hành động:** Reset + Giữ `BTN0` 2 giây rồi nhả.
- **Kỳ vọng:** Mạch nhảy vào bản thấp hơn (v4) ở Slot B. Đèn Breathing.

**[Test Case 10]: Anti-Brick - Mất điện giữa chừng**
- **Hành động:** Bắt đầu nạp `slot_a_v6.bin`. Nạp 30% thì **rút mạnh cáp USB**.
- **Kỳ vọng:** Báo cáo hiện Slot A `ERROR: Bad CRC32`. Thuật toán vứt Slot A, fallback cứu hộ nhảy vào Slot B (v4). Mạch sống sót (Breathing).

**[Test Case 11]: Anti-Brick - File bị hỏng**
- **Hành động:** Dùng HxD sửa sai 1 byte dữ liệu trong file `.bin` rồi nạp.
- **Kỳ vọng:** Nhận 100% nhưng cuối cùng báo lỗi CRC. Từ chối file.

**[Test Case 12]: Chặn ghi sai phân vùng**
- **Hành động:** Update Mode báo `Target: SLOT B`. Cố tình nạp file `slot_a_v6.bin` (Vector của Slot A).
- **Kỳ vọng:** Nạp xong 100%. Báo `ERROR: Bad Vector Address`. Bị từ chối.
