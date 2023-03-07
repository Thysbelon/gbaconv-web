# gbaconv-web
An attempt to compile the srm to sav converter gbaconv to WebAssembly. **Does not work**

Build gbaconv-web.c by running `emcc -O0 gbaconv-web.c -o gbaconv-web.js -sEXPORTED_RUNTIME_METHODS=ccall,cwrap -lidbfs.js` in a linux terminal with emscripten installed.
