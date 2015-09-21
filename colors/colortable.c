#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

typedef struct {
    double r, g, b;
} color_t;

typedef struct {
    uint32_t words[8];
} colorset_t;

void colorset_clear(colorset_t *set) {
    memset(set, 0, sizeof *set);
}

void colorset_add(colorset_t *set, uint8_t x) {
    set->words[x >> 5] |= 1 << (x & 0x1f);
}

unsigned colorset_card(colorset_t *set) {
    unsigned total = 0;

    int i;
    for (i = 0; i < 8; i++) {
        uint32_t word = set->words[i];

        word = word - ((word >> 1) & 0x55555555);
        word = (word & 0x33333333) + ((word >> 2) & 0x33333333);
        word = (((word + (word >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;

        total += word;
    }

    return total;
}

void colorset_test() {
    colorset_t set;

    colorset_clear(&set);

    assert(colorset_card(&set) == 0);

    int i;
    for (i = 0; i < 256; i++) {
        colorset_add(&set, i);
        assert(colorset_card(&set) == i + 1);
    }

    colorset_clear(&set);
    assert(colorset_card(&set) == 0);
    colorset_add(&set, 5);
    assert(colorset_card(&set) == 1);
    colorset_add(&set, 100);
    assert(colorset_card(&set) == 2);
    colorset_add(&set, 123);
    assert(colorset_card(&set) == 3);
    colorset_add(&set, 100);
    assert(colorset_card(&set) == 3);
}

color_t table[256] = {
    {.r =   0.0/255.0, .g =   0.0/255.0, .b =   0.0/255.0},
    {.r = 205.0/255.0, .g =   0.0/255.0, .b =   0.0/255.0},
    {.r =   0.0/255.0, .g = 205.0/255.0, .b =   0.0/255.0},
    {.r = 205.0/255.0, .g = 205.0/255.0, .b =   0.0/255.0},
    {.r =   0.0/255.0, .g =   0.0/255.0, .b = 238.0/255.0},
    {.r = 205.0/255.0, .g =   0.0/255.0, .b = 205.0/255.0},
    {.r =   0.0/255.0, .g = 205.0/255.0, .b = 205.0/255.0},
    {.r = 229.0/255.0, .g = 229.0/255.0, .b = 229.0/255.0},
    {.r = 127.0/255.0, .g = 127.0/255.0, .b = 127.0/255.0},
    {.r = 255.0/255.0, .g =   0.0/255.0, .b =   0.0/255.0},
    {.r =   0.0/255.0, .g = 255.0/255.0, .b =   0.0/255.0},
    {.r = 255.0/255.0, .g = 255.0/255.0, .b =   0.0/255.0},
    {.r =  92.0/255.0, .g =  92.0/255.0, .b = 255.0/255.0},
    {.r = 255.0/255.0, .g =   0.0/255.0, .b = 255.0/255.0},
    {.r =   0.0/255.0, .g = 255.0/255.0, .b = 255.0/255.0},
    {.r = 255.0/255.0, .g = 255.0/255.0, .b = 255.0/255.0}
};

void init_table() {
    int i;
    for (i = 0; i < 216; i++) {
        table[16+i].b = ((i /  1) % 6)/5.0;
        table[16+i].g = ((i /  6) % 6)/5.0;
        table[16+i].r = ((i / 36) % 6)/5.0;
    }

    for (i = 0; i < 24; i++) {
        double j = i/23.0;

        table[232+i].r = j;
        table[232+i].g = j;
        table[232+i].b = j;
    }
}

uint8_t find_color(double r, double g, double b) {
    double   best_diff  = INFINITY;
    uint8_t best_color = 0;

    int i;
    for (i = 0; i < 256; i++) {
        double diff = powf(r - table[i].r, 2) +
                     powf(g - table[i].g, 2) +
                     powf(b - table[i].b, 2);

        if (diff < best_diff) {
            best_diff  = diff;
            best_color = i;
        }
    }

    return best_color;
}

unsigned optimize_resolution() {
    int resolution;
    for (resolution = 2; resolution < 256; resolution++) {
        colorset_t set;
        int r, g, b;

        double step = 1.0/(resolution - 1);

        colorset_clear(&set);

        for (r = 0; r < resolution; r++) {
            for (g = 0; g < resolution; g++) {
                for (b = 0; b < resolution; b++) {
                    uint8_t color = find_color(r*step, g*step, b*step);

                    colorset_add(&set, color);
                }
            }
        }

        unsigned card = colorset_card(&set);

        // 247 because the color table actually contains duplicates.
        if (card >= 247) {
            break;
        }
    }

    return resolution;
}

void create_map(unsigned resolution) {
    double step = 1.0/(resolution - 1);

    printf("uint8_t colormap[%d] = {", resolution*resolution*resolution);

    int i = 0;
    int r, g, b;
    for (r = 0; r < resolution; r++) {
        for (g = 0; g < resolution; g++) {
            for (b = 0; b < resolution; b++) {
                uint8_t color = find_color(r*step, g*step, b*step);

                printf("%3d, ", color);

                if (i % 16 == 0) {
                    printf("\n");
                }

                i++;
            }
        }
    }

    printf("};\n");
}

int main(void) {
    init_table();

    colorset_test();

    unsigned resolution = optimize_resolution();

    fprintf(stderr, "Minimum resolution: %d\n", resolution);

    create_map(resolution);

    return 0;
}

