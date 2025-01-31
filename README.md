# VWF
Simple Variable Width Font for Nintendo GameBoy, Sega Master System and Sega Game Gear

This library requires the latest [GBDK-2020 v.4.1.1](https://github.com/gbdk-2020/gbdk-2020/releases/latest/) and GNU Make. Windows users may use mingw or similar.

Features:
 - use up to 4 fonts at the same time (look at the vwf_font.c, font format is quite obvious, font tiles are 1-bit)
 - set foreground and background colors
 - you can render text to tiles and screen, or to tiles only
 - gb version supports printing onto background as well as on window layer
 - support for control characters in text for inversing text, gotoxy or activate another font

Screenshots:

![GameBoy](/gb.png) ![GameGear](/gg.png)

![MasterSystem](/sms.png)

## Encoding New Fonts

New fonts can be converted from png using the included python script.
You may be required to install `pillow` to run the script
```sh
# install dependency, if needed
$ pip install pillow

# from root directory
$ py ./utils/png2font.py -o ./assets/fonts/font_fancy.c --start 0 ./assets/fonts/font_fancy.png
```