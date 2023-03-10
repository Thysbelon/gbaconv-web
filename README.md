# gbaconv-web
The srm to sav converter gbaconv compiled to WebAssembly. Converts [libretro VBA-M](https://github.com/libretro/vbam-libretro) and [VBA-Next](https://github.com/libretro/vba-next)'s srm save files and [44vba](https://github.com/44670/44vba)'s 4gs (just a renamed srm) save files to sav. It can also convert sav to srm/4gs. [gbaconv was originally written by libretro contributors](https://web.archive.org/web/20190801135240/https://github.com/libretro/vbam-libretro/blob/master/src/libretro/gbaconv/gbaconv.c). [More info on VBA save formats here](https://emulation.gametechwiki.com/index.php/Game_Boy_Advance_emulators#Save_formats).

## For Developers:
Build gbaconv-web.c by running `emcc -O0 gbaconv-web.c -o gbaconv-web.js -sEXPORTED_RUNTIME_METHODS=ccall,cwrap -lidbfs.js` in a linux terminal with emscripten installed.
