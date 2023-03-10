# gbaconv-web
The srm to sav converter gbaconv compiled to WebAssembly.

Build gbaconv-web.c by running `emcc -O0 gbaconv-web.c -o gbaconv-web.js -sEXPORTED_RUNTIME_METHODS=ccall,cwrap -lidbfs.js` in a linux terminal with emscripten installed.
