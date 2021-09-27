        .include        "global.s"

        .globl _vwf_current_rotate, _vwf_current_mask, _vwf_inverse_map
        .globl _memcpy
        .globl _set_tile_1bpp_data, __current_1bpp_colors

        .ez80

        .area _DATA
        
__save: 
        .ds 0x01 

        .area _CODE

; void vwf_memcpy(void* to, const void* from, size_t n, UBYTE bank)  __z88dk_callee;
_vwf_memcpy::
        pop iy
        pop de
        pop hl
        pop bc
        dec sp
        ex (sp), iy

        ld  a, (.MAP_FRAME1)
        ld  (#__save), a
        ld  a, iyh
        ld (.MAP_FRAME1), a

        ldir

        ld  a, (#__save)
        ld  (.MAP_FRAME1),a
        ret

; UBYTE vwf_read_banked_ubyte(const void * src, UBYTE bank) __z88dk_callee; 
_vwf_read_banked_ubyte::
        ld  a, (.MAP_FRAME1)
        ld  (#__save), a

        pop hl
        pop de
        dec sp
        ex (sp), hl

        ld a, h
        ld (.MAP_FRAME1), a

        ex de, hl
        ld l, (hl)

        ld  a, (#__save)
        ld (.MAP_FRAME1), a
        ret

; void vwf_set_banked_bkg_data(UBYTE i, UBYTE l, const unsigned char* ptr, UBYTE bank)  __z88dk_callee;
_vwf_set_banked_data::
        ld  a, (.MAP_FRAME1)
        ld  (#__save), a

        pop hl
        pop de
        pop bc
        dec sp
        ex (sp), hl

        ld a, h
        ld (.MAP_FRAME1), a

        ld hl, (__current_1bpp_colors)
        push hl
        push bc
        ld h, #0
        ld l, d
        push hl
        ld l, e
        push hl

        call  _set_tile_1bpp_data

        ld  a, (#__save)
        ld (.MAP_FRAME1), a
        ret

; void vwf_print_shift_char(void * dest, const void * src, UBYTE bank);
_vwf_print_shift_char::
        ld  a, (.MAP_FRAME1)
        ld  (#__save), a

        pop hl
        pop de
        pop bc
        dec sp
        ex (sp), hl

        ld a, h
        ld (.MAP_FRAME1), a

        ex de, hl
        ld d, b
        ld e, c

        ld a, #8
3$:
        push af

        ld a, (de)
        ld c, a
        ld a, (_vwf_inverse_map)
        xor c
        ld c, a
                
        ld a, (de)
        ld b, a
        ld a, (_vwf_inverse_map)
        xor b
        ld b, a
        inc de

        ld a, (_vwf_current_rotate)
        sla a
        jr z, 1$
        jr c, 4$
        srl a
        srl a
        jr nc, 6$
        srl c
        srl b
6$:
        or a
        jr z, 1$
2$:
        srl c
        srl b
        srl c
        srl b
        dec a
        jr nz, 2$
        jr 1$
4$:
        srl a
        srl a
        jr nc, 7$
        sla c
        sla b
7$:     or a
        jr z, 1$
5$:
        sla c
        sla b
        sla c
        sla b
        dec a
        jr nz, 5$
1$:
        ld a, (_vwf_current_mask)
        and (hl)
        or c
        ld (hl), a
        inc hl

        ld a, (_vwf_current_mask)
        and (hl)
        or b
        ld (hl), a
        inc hl

        xor a
        ld (hl), a
        inc hl
        ld (hl), a
        inc hl

        pop af
        dec a
        jr nz, 3$

        ld  a, (#__save)
        ld (.MAP_FRAME1), a

        ret

_vwf_get_win_addr::
_vwf_get_bkg_addr::
        ld hl, #.VDP_TILEMAP
        ret
