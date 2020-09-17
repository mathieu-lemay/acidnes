#include <stdio.h>
#include <sys/time.h>

#include "config.h"
#include "cpu.h"
#include "cartridge.h"
#include "mapper.h"
#include "ppu.h"

int main(int argc, char **argv) {
    cpu_t *cpu;
    ppu_t *ppu;
    cartridge_t *cart;
    uint8_t cycles;

    SDL_Event evt;
    SDL_Renderer *renderer;
    SDL_Window *window;

    if (argc > 1) {
        printf("Loading %s\n", argv[1]);
        cart = cartridge_load(argv[1]);
    } else {
        cart = cartridge_load("tests/nestest.nes");
    }

    if (cart == NULL) {
        return 1;
    }

    struct timeval t1, t2;

    gettimeofday(&t1, NULL);

    mapper_init(cart->mapper_type, cart->rom, cart->nb_16k_rom_banks * 0x4000, cart->vrom, cart->nb_8k_vrom_banks * 0x2000);

    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("AcidNES",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_W * SCALE,
                              SCREEN_H * SCALE,
                              SDL_WINDOW_OPENGL);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); // | SDL_RENDERER_PRESENTVSYNC);
    ppu = ppu_init(renderer);
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

    for (;;) {
        if (SDL_PollEvent(&evt) && (evt.type == SDL_KEYDOWN || evt.type == SDL_QUIT || evt.window.event == SDL_WINDOWEVENT_CLOSE)) {
            break;
        }

        cycles = cpu_tick(cpu);

        /* 3 PPU cycles for each CPU cycle */
        for (int i = 0; i < cycles * 3; i++) {
            ppu_tick(ppu);
        }
    }

    gettimeofday(&t2, NULL);

    double t = t2.tv_sec - t1.tv_sec + ((t2.tv_usec - t1.tv_usec) / 1000000.0);
    printf("Rendered %d frames in %.3f seconds (%.2f fps).\n", ppu->frame, t, (float)ppu->frame / t);

    ppu_free(ppu);
    cpu_free(cpu);
    mapper_free();
    cartridge_free(cart);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
