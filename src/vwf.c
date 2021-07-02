#include <gb/gb.h>
#include <string.h>

#include "vwf.h"

vwf_farptr_t vwf_fonts[4];

static UBYTE vwf_current_offset;
static UBYTE vwf_tile_data[16 * 2];
UBYTE vwf_current_mask;
UBYTE vwf_current_rotate;
UBYTE vwf_inverse_map;
UBYTE vwf_current_tile;
UBYTE vwf_text_bkg_fill = 0;

font_desc_t vwf_current_font_desc;
UBYTE vwf_current_font_bank;

void vwf_print_shift_char(void * dest, const void * src, UBYTE bank);
void vwf_memcpy(void* to, const void* from, size_t n, UBYTE bank);
UBYTE vwf_read_banked_ubyte(const void * src, UBYTE bank) __preserves_regs(b, c);
UBYTE * vwf_get_win_addr() __preserves_regs(b, c, h, l);
UBYTE * vwf_get_bkg_addr() __preserves_regs(b, c, h, l);
void vwf_set_banked_bkg_data(UBYTE i, UBYTE l, const unsigned char* ptr, UBYTE bank);
void vwf_set_banked_win_data(UBYTE i, UBYTE l, const unsigned char* ptr, UBYTE bank);

void vwf_print_reset(UBYTE tile) {
    vwf_current_tile = tile;
    vwf_current_offset = 0;
    memset(vwf_tile_data, vwf_text_bkg_fill, sizeof(vwf_tile_data));
}

UBYTE vwf_print_render(const unsigned char ch) {
    UBYTE letter = vwf_read_banked_ubyte(vwf_current_font_desc.recode_table + (ch & ((vwf_current_font_desc.attr & RECODE_7BIT) ? 0x7fu : 0xffu)), vwf_current_font_bank);
    const UBYTE * bitmap = vwf_current_font_desc.bitmaps + letter * 16u;
    if (vwf_current_font_desc.attr & FONT_VWF) {
        vwf_inverse_map = (vwf_current_font_desc.attr & FONT_VWF_1BIT) ? vwf_text_bkg_fill : 0;
        UBYTE width = vwf_read_banked_ubyte(vwf_current_font_desc.widths + letter, vwf_current_font_bank);
        UBYTE dx = (8u - vwf_current_offset);
        vwf_current_mask = (0xffu << dx) | (0xffu >> (vwf_current_offset + width));

        vwf_current_rotate = vwf_current_offset;
        vwf_print_shift_char(vwf_tile_data, bitmap, vwf_current_font_bank);
        if ((UBYTE)(vwf_current_offset + width) > 8u) {
            vwf_current_rotate = dx | 0x80u;
            vwf_current_mask = 0xffu >> (width - dx);
            vwf_print_shift_char(vwf_tile_data + 16u, bitmap, vwf_current_font_bank);
        }
        vwf_current_offset += width;

        set_bkg_data(vwf_current_tile, 1, vwf_tile_data);
        if (vwf_current_offset > 7u) {
            memcpy(vwf_tile_data, vwf_tile_data + 16u, 16);
            memset(vwf_tile_data + 16u, vwf_text_bkg_fill, 16);
            vwf_current_offset -= 8u;
            vwf_current_tile++;
            if (vwf_current_offset) set_bkg_data(vwf_current_tile, 1, vwf_tile_data);
            return TRUE;
        } 
        return FALSE;
    } else {
        vwf_set_banked_bkg_data(vwf_current_tile++, 1, bitmap, vwf_current_font_bank);
        vwf_current_offset = 0;
        return TRUE;
    }
}

void vwf_draw_text(UBYTE x, UBYTE y, UBYTE base_tile, const unsigned char * str) {
    static UBYTE * ui_dest_base, *ui_dest_ptr;
    static const UBYTE * ui_text_ptr;
    ui_dest_ptr = ui_dest_base = vwf_get_bkg_addr() + y * 32 + x;
    ui_text_ptr = str;

    vwf_print_reset(base_tile);
    while (*ui_text_ptr) {
        switch (*ui_text_ptr) {
            case 0x01:
                vwf_activate_font(*++ui_text_ptr);
                break;
            case 0x02:
                ui_dest_ptr = ui_dest_base = vwf_get_bkg_addr() + *++ui_text_ptr * 32 + *++ui_text_ptr;
                if (vwf_current_offset) vwf_print_reset(vwf_current_tile + 1u);
                break; 
            case 0x03:
                vwf_text_bkg_fill = *++ui_text_ptr;
                break;
            case '\n':
                ui_dest_ptr = ui_dest_base += 32;
                if (vwf_current_offset) vwf_print_reset(vwf_current_tile + 1u);
                break; 
            default:
                if (vwf_print_render(*ui_text_ptr)) {
                    set_vram_byte(ui_dest_ptr++, vwf_current_tile - 1);
                }
                if (vwf_current_offset) set_vram_byte(ui_dest_ptr, vwf_current_tile);
                break;
        }
        ui_text_ptr++;
    }
}

void vwf_load_font(UBYTE idx, const void * font, UBYTE bank) {
    vwf_fonts[idx].bank = bank;
    vwf_fonts[idx].ptr = (void *)font;
    vwf_activate_font(idx); 
}

void vwf_activate_font(UBYTE idx) {
    vwf_current_font_bank = vwf_fonts[idx].bank;
    vwf_memcpy(&vwf_current_font_desc, vwf_fonts[idx].ptr, sizeof(font_desc_t), vwf_current_font_bank);    
}

UBYTE vwf_next_tile() {
    return (vwf_current_offset) ? vwf_current_tile + 1u : vwf_current_tile;
}