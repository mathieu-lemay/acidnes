#include <stdio.h>

#include "cpu.h"
#include "cartridge.h"
#include "mapper.h"
#include "ppu.h"

int main(int argc, char **argv) {
    cpu_t *cpu;
    ppu_t *ppu;
    cartridge_t *cart;
    uint8_t cycles;

    if (argc > 1) {
        printf("Loading %s\n", argv[1]);
        cart = cartridge_load(argv[1]);
    } else {
        cart = cartridge_load("tests/nestest.nes");
    }

    if (cart == NULL) {
        return 1;
    }

    mapper_init(cart->mapper_type, cart->rom, cart->nb_16k_rom_banks * 0x4000, cart->vrom, cart->nb_8k_vrom_banks * 0x2000);

    cpu = cpu_init();
    if (cpu == NULL) {
        fprintf(stderr, "Unable to initialize CPU\n");
        return 1;
    }

    cpu_reset(cpu);

    ppu = ppu_init();
    if (ppu == NULL) {
        fprintf(stderr, "Unable to initialize PPU\n");
        return 1;
    }

    cpu->ppu = ppu;

    for (;;) {
        cycles = cpu_tick(cpu);
        if (cycles == 255) {
            break;
        }

        /* 3 PPU cycles for each CPU cycle */
        for (int i = 0; i < cycles * 3; i++) {
            ppu_tick(ppu);
        }
    }

    ppu_free(ppu);
    cpu_free(cpu);
    mapper_free();
    cartridge_free(cart);

    return 0;
}
