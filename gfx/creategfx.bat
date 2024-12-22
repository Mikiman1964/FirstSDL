CreateGfx
ObjectZipper gfx.bin gfx_compressed.bin
objcopy --input-target binary --output-target pe-x86-64 --binary-architecture i386 gfx_compressed.bin gfx_compressed.o
del gfx.bin
del gfx_compressed.bin