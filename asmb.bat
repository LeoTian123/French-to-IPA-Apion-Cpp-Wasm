emcc apion_core.cpp ^
  -O1 ^
  -s WASM=1 ^
  -s MODULARIZE=1 ^
  -s EXPORT_ES6=1 ^
  -s ALLOW_MEMORY_GROWTH=1 ^
  -lembind ^
  --preload-file exceptions.bin ^
  -o web/apion_core.js ^
  -I .
