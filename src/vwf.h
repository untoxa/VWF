#ifndef _VWF_H_INCLUDE
#define _VWF_H_INCLUDE

#include <gb/gb.h>

#define __VWF_BANK_PREFIX(A) __bank_##A
#define TO_VWF_FARPTR(A) {.bank = (char)&(__VWF_BANK_PREFIX(A)), .ptr = (void *)&(A)}

#define RECODE_7BIT 1
#define FONT_VWF 2
#define FONT_VWF_1BIT 4

typedef struct vwf_farptr_t {
    UINT8 bank;
    void * ptr;
} vwf_farptr_t;

typedef struct font_desc_t {
    UBYTE attr;
    const UBYTE * recode_table;
    const UBYTE * widths;
    const UBYTE * bitmaps;
} font_desc_t;

extern vwf_farptr_t vwf_fonts[4];

void vwf_load_font(UBYTE idx, const void * font, UBYTE bank);
void vwf_activate_font(UBYTE idx);
void vwf_draw_text(UBYTE x, UBYTE y, UBYTE base_tile, const unsigned char * str);
UBYTE vwf_next_tile();

#endif