#ifndef __MAPPER_H__
#define __MAPPER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

void mapper_init(uint8_t mapper_type, uint8_t *prg_rom, uint32_t prg_rom_size, uint8_t *chr_rom, uint32_t chr_rom_size);
void mapper_free(void);

uint8_t get_prg_u8(uint16_t addr);
uint16_t get_prg_u16(uint16_t addr);
void set_prg_u8(uint16_t addr, uint8_t value);

uint8_t get_chr_u8(uint16_t addr);
uint16_t get_chr_u16(uint16_t addr);

uint8_t get_ex_ram_u8(uint16_t addr);
uint16_t get_ex_ram_u16(uint16_t addr);
void set_ex_ram_u8(uint16_t addr, uint8_t value);

bool has_mirroring(void);

#ifdef __cplusplus
}
#endif
#endif /* __MAPPER_H__ */
