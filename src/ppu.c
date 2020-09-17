#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "ppu.h"

/* Scan Position */
bool is_start_of_frame(ppu_t *ppu);
bool is_in_visible_area(ppu_t *ppu);
bool should_render(ppu_t *ppu);
void increase_scan(ppu_t *ppu);

/* Rendering */
void clear_screen(ppu_t *ppu);
void draw_pixel(ppu_t *ppu, uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);
void update_screen(ppu_t *ppu);

ppu_t *ppu_init(SDL_Renderer *renderer) {
    ppu_t *ppu = malloc(sizeof(ppu_t));

    if (ppu == NULL) {
        return ppu;
    }

    ppu->renderer = renderer;
    ppu->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, SCREEN_W, SCREEN_H);

    return ppu;
}

void ppu_reset(ppu_t *ppu) {
    ppu->frame = 0;
    ppu->scanline = 0;
    ppu->line_position = 0;
    ppu->is_vblank = FALSE;

    clear_screen(ppu);
}

void ppu_free(ppu_t *ppu) {
    SDL_DestroyTexture(ppu->texture);
    free(ppu);
}

void ppu_tick(ppu_t *ppu) {
    if (is_in_visible_area(ppu)) {
        int x = ppu->line_position - 1;
        int y = ppu->scanline - 1;
        int16_t c = (ppu->frame + x + y) % 510 - 255;

        if (c < 0) c *= -1;

        draw_pixel(ppu, x, y, c, c, c);
    }

    if (should_render(ppu)) {
        update_screen(ppu);

        ppu->frame++;
    }

    increase_scan(ppu);
}

uint8_t ppu_get_status(ppu_t *ppu) {
    uint8_t status = 0;

    status |= (uint8_t) (ppu->is_vblank ? 0x80 : 0x00);

    ppu->is_vblank = 0;

    return status;
}

/* Scan Position */
inline bool is_start_of_frame(ppu_t *ppu) {
    return ppu->scanline == 0 && ppu->line_position == 0;
}

inline bool is_in_visible_area(ppu_t *ppu) {
    return ppu->scanline > 0 && ppu->scanline <= PPU_VBLANK_SCANLINE && ppu->line_position > 0 && ppu->line_position <= PPU_HBLANK_POS;
}

inline bool should_render(ppu_t *ppu) {
    return ppu->scanline == PPU_VBLANK_SCANLINE && ppu->line_position == PPU_HBLANK_POS;
}

void increase_scan(ppu_t *ppu) {
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

/* Rendering */
void clear_screen(ppu_t *ppu) {
    SDL_SetRenderDrawColor(ppu->renderer, 0, 0, 0, 0);
    SDL_RenderClear(ppu->renderer);
    SDL_RenderPresent(ppu->renderer);
    SDL_SetRenderTarget(ppu->renderer, ppu->texture);
}

void draw_pixel(ppu_t *ppu, uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
    SDL_SetRenderDrawColor(ppu->renderer, r, g, b, 255);
    SDL_RenderDrawPoint(ppu->renderer, x, y);
}

void update_screen(ppu_t *ppu) {
    SDL_SetRenderTarget(ppu->renderer, NULL);
    SDL_RenderCopy(ppu->renderer, ppu->texture, NULL, NULL);
    SDL_RenderPresent(ppu->renderer);
    SDL_SetRenderTarget(ppu->renderer, ppu->texture);
}
