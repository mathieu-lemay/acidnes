#! /usr/bin/env python3

import os
import struct
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any
from zlib import crc32

mappers = [
    "NROM",
    "MMC1",
    "UxROM",
    "CNROM",
    "MMC3",
    "MMC5",
    "FFE F4xxx",
    "AxROM",
    "FFE F3xxx",
    "MMC2",
    "MMC4",
]


@dataclass
class NesHeader:
    magic: str
    nb_16k_rom_banks: int
    nb_8k_vrom_banks: int
    b6: int  # Mapper, mirroring, battery, trainer
    b7: int  # Mapper, VS/Playchoice, NES 2.0
    b8: int  # PRG-RAM size (rarely used extension)
    b9: int  # TV system (rarely used extension)
    b10: int  # TV system, PRG-RAM presence (unofficial, rarely used extension)
    padding: list[int]

    @property
    def mapper(self) -> int:
        return (self.b6 >> 4 & 0x0F) | (self.b7 & 0xF0)


def main():
    dump = False

    for arg in sys.argv[1:]:
        if arg[0] == "-":
            if arg == "-d":
                dump = True
            else:
                raise ValueError(f"Unsupported flag: {arg}")

            sys.argv.remove(arg)

    if len(sys.argv) < 2:
        raise ValueError(f"Usage: {sys.argv[0]} [-d] file")

    fp = Path(sys.argv[1])
    print_info(fp, dump)


def print_info(fp: Path, dump=False):
    with fp.open("rb") as f:
        buf = f.read(16)

    if len(buf) < 16:
        raise ValueError(f"Unable to read 16 bytes from {fp}")

    header = NesHeader(*struct.unpack(">4s7B5s", buf))

    magic = header.magic
    if magic != b"NES\x1a":
        raise ValueError(f"Invalid header for nes file: {fp}: {magic}")

    print_entry("File", fp)
    print_entry("Magic", magic)

    print_entry(
        "16KiB ROM Banks (PRG)",
        f"{header.nb_16k_rom_banks} ({header.nb_16k_rom_banks * 16} KiB)",
    )
    print_entry(
        "8KiB VROM Banks (CHR)",
        f"{header.nb_8k_vrom_banks} ({header.nb_8k_vrom_banks * 8} KiB)",
    )

    mapper = header.mapper
    print_entry("Mapper", f"{mappers[mapper]} ({mapper})")

    print_entry(
        "Mirroring", "Vertical" if _get_bit_at(header.b6, 0) == 1 else "Horizontal"
    )
    print_entry("Battery RAM at 0x6000-0x7FFF", bool(_get_bit_at(header.b6, 1)))
    print_entry("512 bytes trainer at 0x7000-0x71FF", bool(_get_bit_at(header.b6, 2)))
    print_entry("Four screen VRAM layout", bool(_get_bit_at(header.b6, 3)))

    print_entry("VS-System cartridge", bool(_get_bit_at(header.b7, 0)))
    print_entry("Reserved (must be zero)", header.b7 & 0b00001110)

    # Byte 8
    print_entry(
        "8KiB RAM Banks", f"{header.b8} ({header.b8 * 8} KiB)" if header.b8 else "0"
    )

    # Byte 9
    print_entry("Video mode", "PAL" if _get_bit_at(header.b9, 0) else "NTSC")
    print_entry("Reserved (must be zero)", header.b9 & 0b11111110)

    # Bytes 10-15
    print_entry("Reserved (must be zeroes)", sum(header.padding))

    with fp.open("rb") as f:
        f.seek(16, os.SEEK_SET)

        prg = f.read(header.nb_16k_rom_banks * 16 * 1024)
        prg_crc = crc32(prg)

        chr = f.read(header.nb_8k_vrom_banks * 8 * 1024)
        chr_crc = crc32(chr)

        if dump:
            prg_fp = fp.with_suffix(".prg.bin")
            with prg_fp.open("wb") as prg_f:
                prg_f.write(prg)

            chr_fp = fp.with_suffix(".chr.bin")
            with chr_fp.open("wb") as chr_f:
                chr_f.write(chr)

    print_entry("PRG CRC32", f"{prg_crc:x}")
    print_entry("CHR CRC32", f"{chr_crc:x}")


def print_entry(title: str, value: Any) -> None:
    print(f"{title + ':':40}{value}")


def _get_bit_at(val, pos):
    if 0 > pos >= 8:
        raise ValueError(f"pos must be between 0 and 7: {pos}")

    return val >> pos & 0x01


if __name__ == "__main__":
    main()
