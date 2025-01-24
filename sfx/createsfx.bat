createsfx
ObjectZipper music.bin music_compressed.bin
objcopy --input-target binary --output-target pe-x86-64 --binary-architecture i386 music_compressed.bin music_compressed.o
del music.bin
del music_compressed.bin

ObjectZipper gamesound.bin gamesound_compressed.bin
objcopy --input-target binary --output-target pe-x86-64 --binary-architecture i386 gamesound_compressed.bin gamesound_compressed.o
del gamesound.bin
del gamesound_compressed.bin
pause