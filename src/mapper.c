#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "mapper.h"

uint8_t *_prg_rom = NULL;
uint8_t *_chr_rom = NULL;
uint8_t *_ex_ram = NULL;

bool _has_mirroring;

void mapper_init(uint8_t mapper_type, uint8_t *prg_rom, uint32_t prg_rom_size, uint8_t *chr_rom, uint32_t chr_rom_size) {
    _has_mirroring = FALSE;

    mapper_free();

    if (prg_rom_size == 0x4000) {
        _prg_rom = malloc(sizeof(uint8_t) * 0x8000);

        memcpy(_prg_rom, prg_rom, 0x4000);
        memcpy(_prg_rom + 0x4000, prg_rom, 0x4000);
    } else {
        _prg_rom = malloc(sizeof(uint8_t) * prg_rom_size);
        memcpy(_prg_rom, prg_rom, prg_rom_size);
    }

    _chr_rom = malloc(sizeof(uint8_t) * chr_rom_size);
    memcpy(_chr_rom, chr_rom, chr_rom_size);

    _ex_ram = malloc(sizeof(uint8_t) * 0x1fe0);
}

void mapper_free(void) {
    if (_prg_rom) free(_prg_rom);
    if (_chr_rom) free(_chr_rom);
    if (_ex_ram) free(_ex_ram);
}

uint8_t get_prg_u8(uint16_t addr) {
    return _prg_rom[addr];
}

uint16_t get_prg_u16(uint16_t addr) {
    return (uint16_t) ((_prg_rom[addr + 1] << 8u) + _prg_rom[addr]);
}

inline void set_prg_u8(uint16_t addr, uint8_t value) {} /* do nothing */

uint8_t get_chr_u8(uint16_t addr) {
    return _chr_rom[addr];
}

uint16_t get_chr_u16(uint16_t addr) {
    return (uint16_t) ((_chr_rom[addr + 1] << 8u) + _chr_rom[addr]);
}

uint8_t get_ex_ram_u8(uint16_t addr) {
    return _ex_ram[addr];
}

uint16_t get_ex_ram_u16(uint16_t addr) {
    return (uint16_t) ((_ex_ram[addr + 1] << 8u) + _ex_ram[addr]);
}

void set_ex_ram_u8(uint16_t addr, uint8_t value) {
    _ex_ram[addr] = value;
}

bool has_mirroring(void) {
    return FALSE;
}
