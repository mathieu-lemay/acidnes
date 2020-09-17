#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ppu.h"

ppu_t *ppu_init(void) {
    ppu_t *ppu = malloc(sizeof(ppu_t));

    if (ppu == NULL) {
        return ppu;
    }

    ppu->scanline = 0;
    ppu->line_position = 0;
    ppu->is_vblank = FALSE;

    return ppu;
}

void ppu_reset(ppu_t *ppu) {
    ppu->scanline = 0;
    ppu->line_position = 0;
    ppu->is_vblank = FALSE;
}

void ppu_free(ppu_t *ppu) {
    free(ppu);
}

void ppu_tick(ppu_t  *ppu) {
    if (ppu->scanline == PPU_VBLANK_SCANLINE && ppu->line_position == PPU_HBLANK_POS) {
        /* TODO: should_render */
    }
    ppu->line_position++;

    if (ppu->line_position > PPU_LAST_LINE_POS) {
        ppu->line_position = 0;
        ppu->scanline++;
    }

    if (ppu->scanline == PPU_VBLANK_SCANLINE + 1 && ppu->line_position == 1) {
        ppu->is_vblank = TRUE;
        ppu->is_nmi = TRUE;
    } else if (ppu->scanline > PPU_LAST_SCANLINE) {
        ppu->scanline = 0;
    }
}

uint8_t ppu_get_status(ppu_t *ppu) {
    uint8_t status = 0;

    status |= (uint8_t)(ppu->is_vblank ? 0x80 : 0x00);

    ppu->is_vblank = 0;

    return status;
}
