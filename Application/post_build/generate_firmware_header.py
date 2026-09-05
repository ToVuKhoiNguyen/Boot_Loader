#!/usr/bin/env python3
"""
make_fw_image.py — Post-build tool for the dual-slot bootloader project.

Prepends a 512-byte header block (magic, size, CRC32, vector_addr, version)
to a raw application .bin so it becomes a deployable slot image
(slot_a_${ProjName}.bin / slot_b_${ProjName}.bin).

CRC32 uses Python's built-in zlib.crc32(), which implements the standard
reflected CRC-32/ISO-HDLC algorithm (poly 0xEDB88320, init 0xFFFFFFFF,
final XOR 0xFFFFFFFF). The on-device software CRC32 (btl_crc32.c) MUST
use the exact same algorithm — see Section 8 of the guide.
"""
import argparse
import struct
import sys
import zlib
import time

FW_MAGIC = 0xDEADBEEF
HEADER_BLOCK_SIZE = 0x200          # 512 bytes, must match fw_header.h
HEADER_STRUCT_FMT = "<IIIII"        # magic, img_size, img_crc32, vector_addr, version
HEADER_STRUCT_SIZE = struct.calcsize(HEADER_STRUCT_FMT)   # 20 bytes

SLOT_INFO = {
    "a": {"base": 0x00010000, "size": 0x00010000},
    "b": {"base": 0x00020000, "size": 0x00010000},
}


def build_image(raw_app_bytes: bytes, slot: str, version: int) -> bytes:
    slot_base = SLOT_INFO[slot]["base"]
    slot_size = SLOT_INFO[slot]["size"]
    vector_addr = slot_base + HEADER_BLOCK_SIZE

    img_size = HEADER_BLOCK_SIZE + len(raw_app_bytes)
    if img_size > slot_size:
        sys.exit(
            f"ERROR: image size {img_size} exceeds slot {slot.upper()} "
            f"capacity {slot_size} bytes. Reduce application size."
        )

    # 1) Build header with img_crc32 = 0 for the checksum pass.
    header = bytearray(struct.pack(
        HEADER_STRUCT_FMT, FW_MAGIC, img_size, 0, vector_addr, version
    ))
    header += b"\xFF" * (HEADER_BLOCK_SIZE - len(header))   # pad to 512B

    # 2) Assemble full image (header-with-zero-crc + payload) and compute CRC32.
    full_image = bytes(header) + raw_app_bytes
    crc = zlib.crc32(full_image) & 0xFFFFFFFF

    # 3) Patch the real CRC32 into the header (offset 8, per struct layout).
    header[8:12] = struct.pack("<I", crc)

    return bytes(header) + raw_app_bytes


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--slot", choices=["a", "b"], required=True)
    parser.add_argument("--input", required=True, help="raw application .bin")
    parser.add_argument("--output", required=True, help="deployable slot_x_*.bin")
    parser.add_argument(
        "--version", type=int, default=None,
        help="build version number (default: unix timestamp, monotonic)"
    )
    args = parser.parse_args()

    version = args.version if args.version is not None else int(time.time())

    with open(args.input, "rb") as f:
        raw = f.read()

    image = build_image(raw, args.slot, version)

    with open(args.output, "wb") as f:
        f.write(image)

    print(f"[make_fw_image] slot={args.slot.upper()} "
          f"version={version} img_size={len(image)} "
          f"vector_addr=0x{SLOT_INFO[args.slot]['base'] + HEADER_BLOCK_SIZE:08X} "
          f"-> {args.output}")


if __name__ == "__main__":
    main()
