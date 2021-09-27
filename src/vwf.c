#include "vwf.h"

#if defined(NINTENDO)
#define DEVICE_TILE_SIZE 16u
#define VWF_DEFAULT_BASE_ADDRESS 0x9800
#elif defined(SEGA)
#define DEVICE_TILE_SIZE 32u
#define VWF_DEFAULT_BASE_ADDRESS 0x7800
#endif

vwf_farptr_t vwf_fonts[4];

static uint8_t vwf_current_offset;
static uint8_t vwf_tile_data[DEVICE_TILE_SIZE * 2];
uint8_t vwf_current_mask;
uint8_t vwf_current_rotate;
uint8_t vwf_inverse_map = 0;
uint8_t vwf_current_tile;

uint8_t * vwf_render_base_address = VWF_DEFAULT_BASE_ADDRESS;

font_desc_t vwf_current_font_desc;
uint8_t vwf_current_font_bank;

#if defined(NINTENDO)
void vwf_print_shift_char(void * dest, const void * src, uint8_t bank) OLDCALL;
void vwf_memcpy(void* to, const void* from, size_t n, uint8_t bank) OLDCALL;
uint8_t vwf_read_banked_ubyte(const void * src, uint8_t bank) OLDCALL __preserves_regs(b, c) ;
uint8_t * vwf_get_win_addr() OLDCALL __preserves_regs(b, c, h, l) ;
uint8_t * vwf_get_bkg_addr() OLDCALL __preserves_regs(b, c, h, l) ;
void vwf_set_banked_data(uint8_t i, uint8_t l, const unsigned char* ptr, uint8_t bank) OLDCALL;
#elif defined(SEGA)
void vwf_print_shift_char(void * dest, const void * src, uint8_t bank) __z88dk_callee;
void vwf_memcpy(void* to, const void* from, size_t n, uint8_t bank) __z88dk_callee;
uint8_t vwf_read_banked_ubyte(const void * src, uint8_t bank) __z88dk_callee;
uint8_t * vwf_get_win_addr() OLDCALL;
uint8_t * vwf_get_bkg_addr() OLDCALL;
void vwf_set_banked_data(uint8_t i, uint8_t l, const unsigned char* ptr, uint8_t bank) __z88dk_callee;
#endif


void vwf_set_destination(vwf_reder_dest_e destination) {
    vwf_render_base_address = (destination == VWF_RENDER_BKG) ? vwf_get_bkg_addr() : vwf_get_win_addr();
}

void vwf_print_reset(uint8_t tile) {
    vwf_current_tile = tile;
    vwf_current_offset = 0;
    memset(vwf_tile_data, vwf_inverse_map, sizeof(vwf_tile_data));
}

uint8_t vwf_print_render(const unsigned char ch) {
    uint8_t letter = vwf_read_banked_ubyte(vwf_current_font_desc.recode_table + (ch & ((vwf_current_font_desc.attr & RECODE_7BIT) ? 0x7fu : 0xffu)), vwf_current_font_bank);
    const uint8_t * bitmap = vwf_current_font_desc.bitmaps + (uint16_t)letter * 8;
    if (vwf_current_font_desc.attr & FONT_VWF) {
        uint8_t width = vwf_read_banked_ubyte(vwf_current_font_desc.widths + letter, vwf_current_font_bank);
        uint8_t dx = (8u - vwf_current_offset);
        vwf_current_mask = (0xffu << dx) | (0xffu >> (vwf_current_offset + width));

        vwf_current_rotate = vwf_current_offset;
        vwf_print_shift_char(vwf_tile_data, bitmap, vwf_current_font_bank);
        if ((uint8_t)(vwf_current_offset + width) > 8u) {
            vwf_current_rotate = dx | 0x80u;
            vwf_current_mask = 0xffu >> (width - dx);
            vwf_print_shift_char(vwf_tile_data + DEVICE_TILE_SIZE, bitmap, vwf_current_font_bank);
        }
        vwf_current_offset += width;

        set_native_tile_data(vwf_current_tile, 1, vwf_tile_data);
        if (vwf_current_offset > 7u) {
            memcpy(vwf_tile_data, vwf_tile_data + DEVICE_TILE_SIZE, DEVICE_TILE_SIZE);
            memset(vwf_tile_data + DEVICE_TILE_SIZE, vwf_inverse_map, DEVICE_TILE_SIZE);
            vwf_current_offset -= 8u;
            vwf_current_tile++;
            if (vwf_current_offset) set_native_tile_data(vwf_current_tile, 1, vwf_tile_data);
            return TRUE;
        } 
        return FALSE;
    } else {
        vwf_set_banked_data(vwf_current_tile++, 1, bitmap, vwf_current_font_bank);
        vwf_current_offset = 0;
        return TRUE;
    }
}

void vwf_draw_text(uint8_t x, uint8_t y, uint8_t base_tile, const unsigned char * str) {
    static uint8_t * ui_dest_base, *ui_dest_ptr;
    static const uint8_t * ui_text_ptr;
    ui_dest_ptr = ui_dest_base = vwf_render_base_address + (y + DEVICE_SCREEN_Y_OFFSET) * (DEVICE_SCREEN_BUFFER_WIDTH * DEVICE_SCREEN_MAP_ENTRY_SIZE) + ((x + DEVICE_SCREEN_X_OFFSET) * DEVICE_SCREEN_MAP_ENTRY_SIZE);
    ui_text_ptr = str;

    vwf_print_reset(base_tile);
    while (*ui_text_ptr) {
        switch (*ui_text_ptr) {
            case 0x01:
                vwf_activate_font(*++ui_text_ptr);
                break;
            case 0x02:
                ui_dest_ptr = ui_dest_base = vwf_render_base_address + (*++ui_text_ptr + DEVICE_SCREEN_Y_OFFSET) * (DEVICE_SCREEN_BUFFER_WIDTH * DEVICE_SCREEN_MAP_ENTRY_SIZE) + ((*++ui_text_ptr + DEVICE_SCREEN_X_OFFSET) * DEVICE_SCREEN_MAP_ENTRY_SIZE);
                if (vwf_current_offset) vwf_print_reset(vwf_current_tile + 1u);
                break; 
            case 0x03:
                vwf_inverse_map = *++ui_text_ptr;
                break;
            case '\n':
                ui_dest_ptr = ui_dest_base += (DEVICE_SCREEN_BUFFER_WIDTH * DEVICE_SCREEN_MAP_ENTRY_SIZE);
                if (vwf_current_offset) vwf_print_reset(vwf_current_tile + 1u);
                break; 
            default:
                if (vwf_print_render(*ui_text_ptr)) {
                    set_vram_byte(ui_dest_ptr, vwf_current_tile - 1);
                    ui_dest_ptr += DEVICE_SCREEN_MAP_ENTRY_SIZE;
                }
                if (vwf_current_offset) set_vram_byte(ui_dest_ptr, vwf_current_tile);
                break;
        }
        ui_text_ptr++;
    }
}

void vwf_load_font(uint8_t idx, const void * font, uint8_t bank) {
    vwf_fonts[idx].bank = bank;
    vwf_fonts[idx].ptr = (void *)font;
    vwf_activate_font(idx); 
}

void vwf_activate_font(uint8_t idx) {
    vwf_current_font_bank = vwf_fonts[idx].bank;
    vwf_memcpy(&vwf_current_font_desc, vwf_fonts[idx].ptr, sizeof(font_desc_t), vwf_current_font_bank);    
}

uint8_t vwf_next_tile() {
    return (vwf_current_offset) ? vwf_current_tile + 1u : vwf_current_tile;
}