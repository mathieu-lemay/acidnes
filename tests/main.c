#include <stdio.h>

#include "cpu.h"
#include "cartridge.h"
#include "mapper.h"
#include "ppu.h"

void dump_cpu(cpu_t *cpu);

int test_1_nestest();

int main() {
    int fails = 0;
    int err;

    if ((err = test_1_nestest())) {
        fails++;
        fprintf(stderr, "test_1_nestest: FAIL (0x%04x)\n", err);
    } else {
        fprintf(stderr, "test_1_nestest: OK\n");
    }

    return fails > 0 ? 1 : 0;
}

int test_1_nestest() {
    cpu_t *cpu;
    ppu_t *ppu;
    cartridge_t *cart;

    cart = cartridge_load("tests/nestest.nes");
    if (cart == NULL) {
        return 1;
    }

    mapper_init(cart->mapper_type, cart->rom, cart->nb_16k_rom_banks * 0x4000, cart->vrom,
                cart->nb_8k_vrom_banks * 0x2000);

    ppu = ppu_init(NULL);
    if (ppu == NULL) {
        fprintf(stderr, "Unable to initialize PPU\n");
        return 1;
    }

    cpu = cpu_init(ppu);
    if (cpu == NULL) {
        fprintf(stderr, "Unable to initialize CPU\n");
        return 1;
    }

    cpu_reset(cpu);
    ppu_reset(ppu);

    cpu->ppu = ppu;
    cpu->PC = 0xc000;

    /* TODO: Figure out where the 7 cycles come from */
    cpu->clock = 7;
    ppu->line_position = cpu->clock * 3;

    uint8_t cycles;

    for (;;) {
        dump_cpu(cpu);

        cycles = cpu_tick(cpu);

        /* 3 PPU cycles for each CPU cycle */
        for (int i = 0; i < cycles * 3; i++) {
            ppu_tick(ppu);
        }

        if (cpu->PC == 0x0001) {
            break;
        }
    }

    uint16_t status_code = (uint16_t) (cpu->ram[0x02] << 8u) | cpu->ram[0x03];

    ppu_free(ppu);
    cpu_free(cpu);
    mapper_free();
    cartridge_free(cart);

    return status_code;
}

void dump_cpu(cpu_t *cpu) {
    uint8_t p = cpu->P & (uint8_t) ~((uint8_t) U);
    uint8_t op = cpu_get_u8(cpu, cpu->PC);
    char flags[9];

    flags[0] = p & (uint8_t) N ? 'N' : '-';
    flags[1] = p & (uint8_t) V ? 'V' : '-';
    flags[2] = p & (uint8_t) U ? 'U' : '-';
    flags[3] = p & (uint8_t) B ? 'B' : '-';
    flags[4] = p & (uint8_t) D ? 'D' : '-';
    flags[5] = p & (uint8_t) I ? 'I' : '-';
    flags[6] = p & (uint8_t) Z ? 'Z' : '-';
    flags[7] = p & (uint8_t) C ? 'C' : '-';
    flags[8] = '\0';

    printf("PC:%04X OP:%02X (%s) A:%02X X:%02X Y:%02X P:%02X SP:%02X PPU:%3d,%3d CYC:%lu\n",
           cpu->PC, op, OPCODES[op], cpu->A, cpu->X, cpu->Y, cpu->P, cpu->SP, cpu->ppu->scanline, cpu->ppu->line_position, cpu->clock);
}
