#include <gbdk/platform.h>

#include "vwf.h"
#include "vwf_font.h"
#include "vwf_font_bold.h"
#include "vwf_font_ru.h"

void main(void) {
    fill_bkg_rect(0, 0, DEVICE_SCREEN_WIDTH, DEVICE_SCREEN_HEIGHT, 0x00u);
    SHOW_BKG;

    vwf_set_destination(VWF_RENDER_BKG);

    vwf_load_font(0, vwf_font, BANK(vwf_font));
    vwf_load_font(1, vwf_font_bold, BANK(vwf_font_bold));
    vwf_load_font(2, font_ru, BANK(font_ru));
    vwf_activate_font(2);

//    vwf_set_mode(VWF_MODE_RENDER);
//    vwf_set_colors(2, 1);
    vwf_draw_text(3, 3, 1, "Hello, world!\n\x01\x01\BOLD\x01\x00\n\x03\xffthis is a \x01\x01test\x01\x00\x03\x00\nof the new line\ncontrol symbol");
    vwf_draw_text(8, 9, vwf_next_tile(), "This is another\ntext block that is\nprinted in the other\nplace on screen");
    vwf_draw_text(12, 1, vwf_next_tile(), "The third\ntext block\nto the right\nof the first\none");

#if defined(NINTENDO)
    fill_win_rect(0, 0, DEVICE_SCREEN_WIDTH, DEVICE_SCREEN_HEIGHT, 0x00u);

    vwf_set_destination(VWF_RENDER_WIN);
    vwf_draw_text(1, 0, vwf_next_tile(), "This text should be rendered\non the window layer.");

    WX_REG = 7u; WY_REG = 144;
    SHOW_WIN; 
    for (uint8_t i = 143; i != 103; i--) {
        WY_REG = i;
        wait_vbl_done();
    }
#endif
}

