#! /usr/bin/env python3

import struct
import sys

mappers = ["NROM", "MMC1", "UxROM", "CNROM", "MMC3", "MMC5", "FFE F4xxx", "AxROM", "FFE F3xxx", "MMC2", "MMC4"]


def main():
    dump = False

    for arg in sys.argv[1:]:
        if arg[0] == '-':
            if arg == '-d':
                dump = True
            else:
                raise ValueError(f"Unsupported flag: {arg}")

            sys.argv.remove(arg)

    if len(sys.argv) < 2:
        raise ValueError(f"Usage: {sys.argv[0]} [-d] file")

    print_info(sys.argv[1], dump)


def print_info(fp, dump=False):
    with open(fp, 'rb') as f:
        buf = f.read(16)

    if len(buf) < 16:
        raise ValueError(f"Unable to read 16 bytes from {fp}")

    header, nb_16k_rom_banks, nb_8k_vrom_banks, b6, b7, nb_8k_ram_banks, video_mode, *padding = struct.unpack(">4s12B", buf)
    if header != b'NES\x1a':
        raise ValueError(f"Invalid header for nes file: {fp}: {header}")

    print("%-40s%s" % ("File:", fp))
    print("%-40s%s" % ("Header:", header))

    # Byte 4
    print("%-40s%d" % ("16KiB ROM Banks:", nb_16k_rom_banks))

    # Byte 5
    print("%-40s%d" % ("8KiB VROM Banks:", nb_8k_vrom_banks))

    # Byte 6
    print("%-40s%s" % ("Mirroring:", 'Vertical' if _get_bit_at(b6, 0) == 1 else "Horizontal"))
    print("%-40s%s" % ("Battery RAM at 0x6000-0x7FFF:", bool(_get_bit_at(b6, 1))))
    print("%-40s%s" % ("512 bytes trainer at 0x7000-0x71FF:", bool(_get_bit_at(b6, 2))))
    print("%-40s%s" % ("Four screen VRAM layout:", bool(_get_bit_at(b6, 3))))

    # Byte 7
    print("%-40s%s" % ("VS-System cartridge:", bool(_get_bit_at(b7, 0))))
    print("%-40s%s" % ("Reserved (must be zero):", b7 & 0b00001110))

    # Bytes 6 and 7, bits 4-7
    mapper = (b6 >> 4 & 0x0f) | (b7 & 0xf0)
    print("%-40s%s (%d)" % ("Mapper:", mappers[mapper], mapper))

    # Byte 8
    print("%-40s%d" % ("8KiB RAM Banks:", nb_8k_ram_banks))

    # Byte 9
    print("%-40s%s" % ("Video mode:", "PAL" if _get_bit_at(video_mode, 0) else "NTSC"))
    print("%-40s%s" % ("Reserved (must be zero):", video_mode & 0b11111110))

    # Bytes 10-15
    print("%-40s%s" % ("Reserved (must be zeroes):", padding))


def _get_bit_at(val, pos):
    if 0 > pos >=8:
        raise ValueError(f"pos must be between 0 and 7: {pos}")

    return val >> pos & 0x01



if __name__ == "__main__":
    main()
