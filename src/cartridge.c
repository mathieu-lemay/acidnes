#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "cartridge.h"

cartridge_t *cartridge_load(const char *file) {
    FILE *f_nes;
    cartridge_t *cart = NULL;
    uint8_t buffer[16];

    f_nes = fopen(file, "rb");
    if (f_nes == NULL) {
        perror(file);

        return NULL;
    }

    fread(buffer, 16, 1, f_nes);

    /* Validate file is a iNes file */
    if (buffer[0] != 'N' || buffer[1] != 'E' || buffer[2] != 'S' || buffer[3] != 0x1a) {
        fprintf(stderr, "Not a valid NES file: %s\n", file);

        fclose(f_nes);

        return NULL;
    }

    cart = malloc(sizeof(cartridge_t));
    if (cart == NULL) {
        fclose(f_nes);
        return NULL;
    }

    cart->nb_16k_rom_banks = buffer[4];
    cart->nb_8k_vrom_banks = buffer[5];
    cart->nb_8k_ram_banks = buffer[8];

    cart->vert_mirror = get_bit_at(buffer[6], 0);
    cart->battery_ram = get_bit_at(buffer[6], 1);
    cart->trainer = get_bit_at(buffer[6], 2);
    cart->four_screen_vram = get_bit_at(buffer[6], 3);
    cart->vs_system = get_bit_at(buffer[7], 0);
    cart->mapper_type = (buffer[6] >> 4 & 0x0f) | (buffer[7] & 0xf0);
    cart->is_pal = get_bit_at(buffer[9], 0);

    if (cart->mapper_type != 0) {
        fprintf(stderr, "Unsupported mapper: %02x\n", cart->mapper_type);

        free(cart);
        fclose(f_nes);

        return NULL;
    }

    /* Read banks */
    fseek(f_nes, cart->trainer ? 512 : 0, SEEK_CUR);

    cart->rom = malloc(cart->nb_16k_rom_banks * 0x4000);
    if (fread(cart->rom, 0x4000, cart->nb_16k_rom_banks, f_nes) != cart->nb_16k_rom_banks) {
        perror("rom");

        free(cart->rom);
        free(cart);
        fclose(f_nes);

        return NULL;
    }

    cart->vrom = malloc(cart->nb_8k_vrom_banks * 0x2000);
    if (fread(cart->vrom, 0x2000, cart->nb_8k_vrom_banks, f_nes) != cart->nb_8k_vrom_banks) {
        perror("vrom");

        free(cart->rom);
        free(cart->vrom);
        free(cart);
        fclose(f_nes);

        return NULL;
    }

    fclose(f_nes);

    return cart;
}

void cartridge_free(cartridge_t *cartridge) {
    free(cartridge->rom);
    free(cartridge->vrom);
    free(cartridge);
}
