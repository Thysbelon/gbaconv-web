# gbaconv-web
An attempt to compile the srm to sav converter gbaconv to WebAssembly. **Does not work**

Build gbaconv-web.c by running `emcc -O0 gbaconv-web.c -o gbaconv-web.js -sEXPORTED_RUNTIME_METHODS=ccall,cwrap -lidbfs.js` in a linux terminal with emscripten installed.

## Status
The app correctly reads the input save file (proof of this is that it correctly detects the save file's type), and the app can write an output save file, however, the output is incorrect.

You have to reload the webpage between each use.
