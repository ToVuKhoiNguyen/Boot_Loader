#include "xmodem.h"
#include "flash.h"
#include "uart.h"

#define SOH 0x01
#define EOT 0x04
#define ACK 0x06
#define NAK 0x15
#define CAN 0x18
#define XMODEM_C 0x43

#define PACKET_DATA_SIZE 128U
#define MAX_HANDSHAKE_RETRY 20U
#define MAX_PACKET_RETRY 10U
#define BYTE_TIMEOUT_MS 1000U
#define HANDSHAKE_PERIOD_MS 2000U

static uint16_t crc16_ccitt(const uint8_t *data, uint32_t len) {
  uint16_t crc = 0x0000;
  for (uint32_t i = 0; i < len; i++) {
    crc ^= (uint16_t)data[i] << 8;
    for (int b = 0; b < 8; b++) {
      crc = (crc & 0x8000) ? (uint16_t)((crc << 1) ^ 0x1021) : (uint16_t)(crc << 1);
    }
  }
  return crc;
}

static int read_byte(void) { return UART_ReceiveByteTimeout(BYTE_TIMEOUT_MS); }

xmodem_result_t XMODEM_Receive(uint32_t dest_addr, uint32_t max_len, uint32_t *out_received) {
  uint8_t packet[PACKET_DATA_SIZE];
  uint32_t received = 0;
  uint8_t expected_block = 1;

  // Handshake gửi C 20 lần nếu máy tính chịu truyền thì nhận SOH
  bool handshake_ok = false;
  for (uint32_t attempt = 0; attempt < MAX_HANDSHAKE_RETRY; attempt++) {
    UART_SendByte(XMODEM_C);
    int b = UART_ReceiveByteTimeout(HANDSHAKE_PERIOD_MS);
    if (b == SOH) {
      handshake_ok = true;
      break;
    }
    if (b == CAN) {
      return XMODEM_ERR_CANCELLED;
    }
  }
  if (!handshake_ok) {
    return XMODEM_ERR_TIMEOUT;
  }

  // Nhận packet 128 byte và check CRC
  int leading_byte = SOH;

  while (true) {
    if (leading_byte == EOT) {
      UART_SendByte(ACK);
      *out_received = received;
      return XMODEM_OK;
    }
    if (leading_byte == CAN) {
      return XMODEM_ERR_CANCELLED;
    }
    if (leading_byte != SOH) {
      UART_SendByte(NAK);
      leading_byte = read_byte();
      continue;
    }

    uint32_t retry = 0;
    bool packet_ok = false;

    while (retry < MAX_PACKET_RETRY && !packet_ok) {
      // Bước 1: Đọc 2 byte đầu tiên của gói (Số thứ tự gói và phần bù của nó)
      int blk = read_byte();
      int blk_c = read_byte(); // Phần bù của blk (255 - blk)

      // Kiểm tra xem số thứ tự có hợp lệ không (blk + blk_c phải bằng 255)
      if (blk < 0 || blk_c < 0 || (blk + blk_c) != 0xFF) {
        UART_SendByte(NAK);
        retry++;
        leading_byte = read_byte();
        if (leading_byte != SOH) {
          break;
        }
        continue;
      }

      // Bước 2: Đọc đúng 128 byte dữ liệu của gói
      bool read_fail = false;
      for (uint32_t i = 0; i < PACKET_DATA_SIZE; i++) {
        int d = read_byte();
        if (d < 0) {
          read_fail = true;
          break;
        }
        packet[i] = (uint8_t)d;
      }
      // Bước 3: Đọc 2 byte chứa mã CRC16 do máy tính gửi xuống
      int crc_hi = read_byte();
      int crc_lo = read_byte();

      if (read_fail || crc_hi < 0 || crc_lo < 0) {
        UART_SendByte(NAK);
        retry++;
        leading_byte = read_byte();
        continue;
      }

      // Bước 4: Mạch tự tính lại mã CRC16 của 128 byte vừa nhận
      uint16_t recv_crc = (uint16_t)((crc_hi << 8) | crc_lo);
      uint16_t calc_crc = crc16_ccitt(packet, PACKET_DATA_SIZE);

      // Nếu mã CRC tự tính KHÔNG KHỚP với mã máy tính gửi -> Báo lỗi NAK yêu
      // cầu gửi lại
      if (recv_crc != calc_crc) {
        UART_SendByte(NAK);
        retry++;
        leading_byte = read_byte();
        continue;
      }

      // Bước 5: Gói tin hợp lệ. Bắt đầu kiểm tra số thứ tự của gói (block
      // sequence).
      if ((uint8_t)blk == expected_block) {
        if (received + PACKET_DATA_SIZE > max_len) {
          return XMODEM_ERR_TOO_LARGE; // Tràn bộ nhớ Flash
        }
        // GHI THẲNG 128 BYTE VÀO FLASH (Không cần chờ nhận hết cả file)
        if (!FLASH_Write(dest_addr + received, packet, PACKET_DATA_SIZE)) {
          return XMODEM_ERR_TIMEOUT; // Lỗi ghi Flash -> Kết thúc phiên truyền
        }
        received += PACKET_DATA_SIZE;
        expected_block++;
      }
      // Nếu máy tính gửi lại gói cũ (do bị mất chữ ACK trước đó),
      // Mạch vẫn gửi lại ACK để đồng bộ, nhưng KHÔNG ghi đè dữ liệu xuống Flash nữa

      UART_SendByte(ACK);
      packet_ok = true;
    }

    if (!packet_ok) {
      return XMODEM_ERR_TOO_MANY_RETRIES;
    }

    leading_byte = read_byte();
  }
}
