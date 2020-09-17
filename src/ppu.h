#ifndef __PPU_H__
#define __PPU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <SDL.h>

#include "types.h"

#define PPU_VBLANK_SCANLINE 240
#define PPU_HBLANK_POS 256

#define PPU_LAST_SCANLINE 261
#define PPU_LAST_LINE_POS 340

struct ppu_s {
    uint32_t frame;
    uint16_t scanline;
    uint16_t line_position;

    bool is_vblank;
    bool is_nmi;

    uint8_t ram[0x2000];

    SDL_Renderer *renderer;
    SDL_Texture *texture;
};
typedef struct ppu_s ppu_t;

ppu_t *ppu_init(SDL_Renderer *renderer);
void ppu_free(ppu_t *ppu);
void ppu_reset(ppu_t *ppu);

void ppu_tick(ppu_t *ppu);
uint8_t ppu_get_status(ppu_t *ppu);

#ifdef __cplusplus
}
#endif
#endif /* __PPU_H__ */
