#ifndef __CARTRIDGE_H__
#define __CARTRIDGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

struct cartridge_s {
    uint8_t *rom;
    uint8_t *vrom;

    uint8_t mapper_type;
    uint8_t nb_16k_rom_banks;
    uint8_t nb_8k_vrom_banks;
    uint8_t nb_8k_ram_banks;

    bool vert_mirror;
    bool battery_ram;
    bool trainer;
    bool four_screen_vram;
    bool vs_system;
    bool is_pal;
};

typedef struct cartridge_s cartridge_t;

cartridge_t *cartridge_load(const char *file);
void cartridge_free(cartridge_t *cartridge);

#ifdef __cplusplus
}
#endif
#endif /* __CARTRIDGE_H__ */
