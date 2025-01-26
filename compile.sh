#!/bin/bash
# remove old apps, headers and compressed gfx/sfx files
rm -f /gfx/*.h /gfx/creategfx /gfx/objectzipper /gfx/bin2c
rm -f /sfx/*.h /sfx/createsfx /sfx/objectzipper /sfx/bin2c
rm -f /creategfx/creategfx /creategfx/*.o /createsfx/createsfx /createsfx/*.o
rm -f /bin2c/bin2c /bin2c/*.o
# compile new createsfx app
cd createsfx
make clean
make
cp createsfx ../sfx/createsfx
cd ..
# compile new creategfx app
cd creategfx
make clean
make
cp creategfx ../gfx/creategfx
cd ..
# compile new objectzipper app
cd objectzipper
make clean
make -j4
cp objectzipper ../gfx/objectzipper
cp objectzipper ../sfx/objectzipper
cd ..
# compile new bin2c app
cd bin2c
make clean
make
cp bin2c ../gfx/bin2c
cp bin2c ../sfx/bin2c
cd ..
# create new gfx header and gfx_compressed.h files
cd gfx
./creategfx
./objectzipper gfx.bin gfx_compressed.bin
./bin2c gfx_compressed.bin gfx_compressed.h gfx_compressed
rm -f gfx.bin
rm -f gfx_compressed.bin
cd ..
# create new sfx headers, music_compressed.h and gamesound_compressed.h files
cd sfx
./createsfx
./objectzipper music.bin music_compressed.bin
./bin2c music_compressed.bin music_compressed.h music_compressed
rm -f music.bin
rm -f music_compressed.bin

./objectzipper gamesound.bin gamesound_compressed.bin
./bin2c gamesound_compressed.bin gamesound_compressed.h gamesound_compressed
rm -f gamesound.bin
rm -f gamesound_compressed.bin
cd ..
# compile FirstSDL2 app
make -j4
