#include <SDL2/SDL.h>
#include <stdio.h>
#include "EmeraldMine.h"
#include "loadlevel.h"
#include "mySDL.h"
#include "mystd.h"

extern PLAYFIELD Playfield;
extern SDL_DisplayMode ge_DisplayMode;

 uint8_t ge_uLevel[] = {"LEVELHEADER01.00"                          // 0x0000 - 0x000F
                        "LEVELDIMENSION_X"                          // 0x0010 - 0x001F
                        "\x40\x00\x00\x00"                          // 0x0020 - 0x0023, X-Dimension (little Endian)
                        "LEVELDIMENSION_Y"                          // 0x0024 - 0x0033
                        "\x20\x00\x00\x00"                          // 0x0034 - 0x0037, Y-Dimension (little Endian)
                        "___SCORE_EMERALD"                          // 0x0038 - 0x0047
                        "\x64\x00\x00\x00"                          // 0x0048 - 0x004B, Score Emerald (little Endian)
                        "SCORE_SAPHIR"                              // 0x004C - 0x0057
                        "\xC8\x00\x00\x00"                          // 0x0058 - 0x005B, Score Saphir (little Endian)
                        "___SCORE_KEY"                              // 0x005C - 0x0067
                        "\x02\x00\x00\x00"                          // 0x0068 - 0x006B, Score Key (little Endian)
                        "__SCORE_DYNAMITE"                          // 0x006C - 0x007B
                        "\x03\x00\x00\x00"                          // 0x007C - 0x007F, Score Dynamite (little Endian)
                        "___SCORE_NUTCRACKING"                      // 0x0080 - 0x0093
                        "\x04\x00\x00\x00"                          // 0x0094 - 0x0097, Score Nutcracking (little Endian)
                        "SCORE_STONING_BEETLE"                      // 0x0098 - 0x00AB
                        "\x05\x00\x00\x00"                          // 0x00AC - 0x00AF, Score Stoning a Beetle (little Endian)
                        "__SCORE_STONING_MINE"                      // 0x00B0 - 0x00C3
                        "\x06\x00\x00\x00"                          // 0x00C4 - 0x00C7, Score Stoning a Mine (little Endian)
                        "_SCORE_STONING_ALIEN"                      // 0x00C8 - 0x00DB
                        "\x07\x00\x00\x00"                          // 0x00DC - 0x00DF, Score Stoning an Alien (little Endian)
                        "___SCORE_STONING_YAM"                      // 0x00E0 - 0x00F3
                        "\x08\x00\x00\x00"                          // 0x00F4 - 0x00F7, Score Stoning a Yam (little Endian)
                        "___SCORE_TIME_FACTOR"                      // 0x00F8 - 0x010B
                        "\x02\x00\x00\x00"                          // 0x010C - 0x010F, Time Score Factor (little Endian)
                        "_EMERALDS_TO_COLLECT"                      // 0x0110 - 0x0123
                        "\x2C\x01\x00\x00"                          // 0x0124 - 0x0127, Emerald to collect (little Endian)
                        "TIME_TO_PLAY"                              // 0x0128 - 0x0133
                        "\x90\x01\x00\x00"                          // 0x0134 - 0x0137, Time to play (little Endian)
//                        "\x10\x00\x00\x00"                          // 0x0134 - 0x0137, Time to play (little Endian)
                        "_TIME_WHEEL_ROTATION"                      // 0x0138 - 0x014B
                        "\x14\x00\x00\x00"                          // 0x014C - 0x014F, Wheel rotation time (little Endian)
                        "__TIME_BLUE_WALL"                          // 0x0150 - 0x015F
                        "\x32\x00\x00\x00"                          // 0x0160 - 0x0163, Blue wall time (little Endian)
                        "_SPEED_CHEESE_SPREAD"                      // 0x0164 - 0x0177
                        "\x0A\x00\x00\x00"                          // 0x0178 - 0x017B, Cheese spread speed (little Endian)
                        "__LEVELTITLE"                              // 0x017C - 0x0187
                        "DER BUNKER                      "          // 0x0188 - 0x01A7  maximal 32 Zeichen für Level-Titel
                        "_LEVELAUTHOR"                              // 0x01A8 - 0x01B3
                        "MIKIMANN                        "          // 0x01B4 - 0x01D3  maximal 32 Zeichen für Level-Author
                        "_LEVELHEADER_END"                          // 0x01D4 - 0x01E3
                        "_LEVEL_START"                              // 0x01E4 - 0x01EF
"\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83"
"\x83\x80\xE7\xE6\xDA\x08\xDD\xE6\xE9\x80\xDC\xE4\xDC\xE9\xD8\xE3\xDB\x80\xE4\xE0\xE5\xDC\x80\xEE\xE0\xEB\xDF\x80\xEA\xDB\xE3\x15\x83\x80\x80\x80\x80\x83\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x83"
"\x83\x83\x82\x82\x82\x80\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x80\x83\x80\x80\x80\x80\x83\x80\x80\x80\x80\x83\x80\x80\x80\x80\x83\x83\x83\x83\x83\x83\x83\x83\x83\x80\x80\x80\x83\x80\x80\x80\x83"
"\x83\x80\x83\x80\xEC\xEA\xDC\x80\xDA\xEC\xE9\xEA\xE6\xE9\x80\xE2\xDC\xF0\xEA\x80\xEB\xE6\x80\xEA\xDA\xE9\xE6\xE3\xE3\x80\x80\x83\x80\x80\x83\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x83"
"\x83\x80\x80\x83\x80\x80\x80\x80\x80\x93\x80\xEB\xDF\xE9\xE6\xEC\xDE\xDF\x80\xE3\xDC\xED\xDC\xE3\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x80\x83\x80\x80\x80\x83\x83\x83\x83\x83\x80\x80\x80\x83"
"\x83\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x40\x80\x80\x80\x80\x80\x80\x04\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x83\x83\x83\x80\x80\x80\x80\x80\x80\x80\x80\x83"
"\x83\x80\x83\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x85\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x86\x83\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x15\x83\x83\x83"
"\x83\x80\x83\x80\x80\x80\x83\x80\x80\x80\x80\x80\xE7\xE9\xDC\xEA\xEA\x80\xEA\xE7\xD8\xDA\xDC\x80\x80\x80\x80\x83\x80\x80\x80\x83\x83\x83\x83\x83\x83\x83\x80\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x80\x80\x83\x83\x80\x80\x80\x80\x80\x83\x80\x83\x80\x80\x83"
"\x83\x80\x83\x83\x83\x80\x80\x83\x80\x80\x80\xDD\xE6\xE9\x80\xE4\xD8\xE0\xE5\x80\xE7\xD8\xE9\xEB\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x83\x83\x80\x80\x80\x80\x80\x83"
"\x83\x82\x83\x82\x82\x83\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x82\x80\x15\x83\x80\x83\x83\x80\x80\x80\x80\x80\x83\x80\x80\x89\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x83\x89\x80\x80\x80\x83\x83\x83\x80\x83"
"\x83\x82\x83\x82\x82\x80\x83\x80\x80\x83\x80\x80\x80\x80\x80\x18\x19\x1A\x80\x80\x80\x82\x80\x82\x83\x80\x80\x80\x83\x83\x83\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x83\x80\x80\x80\x80\x83\x83\x80\x83\x83\x80\x83\x80\x80\x80\x83"
"\x83\x82\x83\x83\x83\x80\x83\x80\x80\x80\x83\x80\x80\x80\x80\x1B\x80\x1C\x80\x80\x80\x82\x0A\x83\x80\x80\x80\x80\x80\x80\x80\x80\x83\x83\x83\x83\x83\x89\x80\x80\x80\x80\x80\x83\x83\x83\x83\x83\x83\x83\x80\x80\x80\x83\x80\x89\x80\x80\x83\x80\x80\x80\x80\x83"
"\x83\x82\x83\x09\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x80\x83\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x80\x80\x80\x80\x80\x80\x80\x83"
"\x83\x83\x83\x83\x83\x83\x83\x83\x83\x80\x80\x80\x83\x08\x80\x80\x80\x80\x80\x80\x0A\x83\x80\x80\x83\x89\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x89\x80\x80\x80\x83\x83\x80\x80\x80\x80\x80\x80\x80\x83\x86\x80\x80\x80\x80\x80\x80\x80\x80\x82\x83\x83\x83"
"\x83\x15\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x83\x1D\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x83\x83\x83\x83\x89\x83\x83\x83\x83\x83\x80\x83\x15\x80\x80\x83"
"\x83\x82\x82\x82\x83\x80\x83\x83\x83\x80\x83\x83\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x83\x80\x89\x80\x83\x80\x80\x80\x83\x80\x80\x80\x80\x80\x89\x80\x80\x80\x83\x80\x80\x80\x80\x80\x83\x80\x80\x80\x09\x80\x80\x0B\x80\x80\x83\x83\x83\x83\x83\x83\x83"
"\x83\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x83\x80\x83\x80\x83\x80\x80\x83\x80\x80\x80\x83\x80\x80\x80\x80\x83\x80\x80\x83\x83\x83\x80\x80\x83\x83\x80\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x80\x80\x80\x09\x80\x80\x0B\x80\x80\x83\x80\x88\x80\x80\x09\x83"
"\x83\x82\x82\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x89\x80\x83\x80\x80\x80\x83\x80\x83\x80\x80\x80\x89\x80\x80\x83\x83\x80\x80\x80\x80\x80\x83\x80\x80\x80\x09\x80\x80\x0B\x80\x80\x83\x83\x83\x83\x83\x82\x83"
"\x83\x80\x08\x83\x80\x80\x80\x80\x83\x83\x83\x80\x83\x80\x80\x80\x80\x80\x83\x80\x83\x83\x83\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x83\x80\x80\x83\x83\x83\x83\x83\x8C\x83\x83\x83\x83\x80\x80\x80\x80\x80\x80\x83"
"\x83\x83\x83\x83\x83\x83\x80\x80\x83\x80\x80\x80\x80\x83\x80\x80\x80\x80\x83\x80\x80\x80\x83\x83\x83\x89\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x89\x80\x80\x83\x80\x80\x80\x83\x83\x83\x83\x80\x80\x83\x80\x80\x80\x83\x83\x80\x80\x83\x80\x80\x80\x80\x83"
"\x83\x80\x80\x83\x80\x80\x83\x80\x83\x80\x83\x83\x80\x80\x80\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x83\x83\x80\x83\x80\x83\x8A\x83\x80\x80\x83\x80\x83\x83\x83\x8B\x83"
"\x83\x80\x80\x83\x80\x80\x80\x80\x80\x80\x83\x83\x80\x80\x80\x89\x80\x80\x80\x80\x83\x83\x83\x83\x83\x89\x83\x83\x83\x83\x83\x83\x80\x80\x80\x80\x80\x83\x83\x83\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x80\x83\x80\x83\x80\x83\x80\x80\x80\x80\x80\x80\x83"
"\x83\x80\x80\x80\x80\x80\x80\x83\x83\x80\x80\x80\x80\x80\x80\x80\x83\x83\x83\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x83\x80\x80\x80\x80\x80\x83\x80\x80\x83\x80\x83\x83\x83\x83\x83\x83\x83\x83\x80\x83"
"\x83\x83\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x80\x89\x82\x82\x83\x80\x83\x80\x80\x80\x83\x89\x80\x80\x80\x80\x83\x83\x83\x83\x83\x83\x83\x80\x80\x80\x80\x80\x80\x80\x83\x80\x83\x80\x80\x83\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x87\x83\x80\x83"
"\x83\x83\x82\x09\x80\x80\x80\x80\x80\x80\x80\x80\x17\x82\x80\x80\x82\x82\x83\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x83\x83\x80\x80\x80\x80\x80\x83\x80\x83\x80\x80\x83\x80\x80\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x80\x83"
"\x83\x80\x82\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x82\x80\x89\x80\x83\x83\x80\x83\x83\x83\x83\x83\x89\x83\x83\x83\x83\x83\x80\x83\x80\x80\x80\x80\x80\x83\x83\x83\x80\x80\x80\x80\x80\x83\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x83"
"\x83\x80\x82\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x82\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x83\x80\x83\x83\x83\x83\x80\x80\x83\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x83"
"\x83\x80\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x80\x89\x80\x80\x83\x83\x83\x83\x83\x80\x80\x89\x83\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x83\x83\x80\x83\x80\x83\x83\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x83\x80\x80\x83"
"\x83\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x80\x80\x83\x80\x80\x80\x83\x82\x80\x80\x83\x80\x80\x82\x83\x80\x80\x83\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x83\x83\x83\x83\x83\x80\x80\x80\x80\x80\x83\x80\x83"
"\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x83\x83\x80\x83\x80\x82\x83\x80\x83\x82\x16\x83\x83\x83\x83\x83\x83\x83\x80\x83\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x82\x82\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83"
"\x83\x85\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x83\x0A\x82\x80\x80\x80\x82\x80\x83\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x82\xDC\xE5\xDB\xDC\x80\xDE\xDC\xE3\xD8\xDC\xE5\xDB\xDC\x83"
"\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83"
};

/*
 uint8_t ge_uLevel[] = {"LEVELHEADER01.00"                          // 0x0000 - 0x000F
                        "LEVELDIMENSION_X"                          // 0x0010 - 0x001F
                        "\x1B\x00\x00\x00"                          // 0x0020 - 0x0023, X-Dimension (little Endian)
                        "LEVELDIMENSION_Y"                          // 0x0024 - 0x0033
                        "\x06\x00\x00\x00"                          // 0x0034 - 0x0037, Y-Dimension (little Endian)
                        "___SCORE_EMERALD"                          // 0x0038 - 0x0047
                        "\x64\x00\x00\x00"                          // 0x0048 - 0x004B, Score Emerald (little Endian)
                        "SCORE_SAPHIR"                              // 0x004C - 0x0057
                        "\xC8\x00\x00\x00"                          // 0x0058 - 0x005B, Score Saphir (little Endian)
                        "___SCORE_KEY"                              // 0x005C - 0x0067
                        "\x02\x00\x00\x00"                          // 0x0068 - 0x006B, Score Key (little Endian)
                        "__SCORE_DYNAMITE"                          // 0x006C - 0x007B
                        "\x03\x00\x00\x00"                          // 0x007C - 0x007F, Score Dynamite (little Endian)
                        "___SCORE_NUTCRACKING"                      // 0x0080 - 0x0093
                        "\x04\x00\x00\x00"                          // 0x0094 - 0x0097, Score Nutcracking (little Endian)
                        "SCORE_STONING_BEETLE"                      // 0x0098 - 0x00AB
                        "\x05\x00\x00\x00"                          // 0x00AC - 0x00AF, Score Stoning a Beetle (little Endian)
                        "__SCORE_STONING_MINE"                      // 0x00B0 - 0x00C3
                        "\x06\x00\x00\x00"                          // 0x00C4 - 0x00C7, Score Stoning a Mine (little Endian)
                        "_SCORE_STONING_ALIEN"                      // 0x00C8 - 0x00DB
                        "\x07\x00\x00\x00"                          // 0x00DC - 0x00DF, Score Stoning an Alien (little Endian)
                        "___SCORE_STONING_YAM"                      // 0x00E0 - 0x00F3
                        "\x08\x00\x00\x00"                          // 0x00F4 - 0x00F7, Score Stoning a Yam (little Endian)
                        "___SCORE_TIME_FACTOR"                      // 0x00F8 - 0x010B
                        "\x02\x00\x00\x00"                          // 0x010C - 0x010F, Time Score Factor (little Endian)
                        "_EMERALDS_TO_COLLECT"                      // 0x0110 - 0x0123
                        "\x2C\x01\x00\x00"                          // 0x0124 - 0x0127, Emerald to collect (little Endian)
                        "TIME_TO_PLAY"                              // 0x0128 - 0x0133
                        "\x14\x00\x00\x00"                          // 0x0134 - 0x0137, Time to play (little Endian)
                        "_TIME_WHEEL_ROTATION"                      // 0x0138 - 0x014B
                        "\x64\x00\x00\x00"                          // 0x014C - 0x014F, Wheel rotation time (little Endian)
                        "__TIME_BLUE_WALL"                          // 0x0150 - 0x015F
                        "\x32\x00\x00\x00"                          // 0x0160 - 0x0163, Blue wall time (little Endian)
                        "_SPEED_CHEESE_SPREAD"                      // 0x0164 - 0x0177
                        "\x0A\x00\x00\x00"                          // 0x0178 - 0x017B, Cheese spread speed (little Endian)
                        "__LEVELTITLE"                              // 0x017C - 0x0187
                        "12345678   DER BUNKER   56789012"          // 0x0188 - 0x01A7
                        "_LEVELAUTHOR"                              // 0x01A8 - 0x01B3
                        "123456789   MIKIMANN   456789012"          // 0x01B4 - 0x01D3
                        "_LEVELHEADER_END"                          // 0x017C - 0x018B
                        "_LEVEL_START"                              // 0x018C - 0x0197

"\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83"
"\x83\x08\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83"
"\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83"
"\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x40\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83"
"\x83\x80\x80\x80\x15\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83"
"\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83"
};
*/

/*----------------------------------------------------------------------------
Name:           InitialisePlayfield
------------------------------------------------------------------------------
Beschreibung: Lädt die Leveldaten und initialisiert das Spielfeld. Das Ergebnis
              wird in der Struktur Playfield.x abgelegt.
              Diese Funktion alloziert Speicher, der später wieder freigegebem
              werden muss: Playfield.pLevel und Playfield.pStatusAnimation
Parameter
      Eingang: puLevel, uint8_t *, Zeiger auf Level-Rohdaten (aus Leveldatei)
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x, ge_DisplayMode
------------------------------------------------------------------------------*/
int InitialisePlayfield(uint8_t *puLevel) {
    int nErrorCode;
    int nLabel;
    size_t nLabelLen;
    const char *pFound;
    uint8_t *pLevelStart = NULL;
    const char *szLevelLabel[]= {
        "LEVELHEADER"               // 0
        ,"LEVELDIMENSION_X"         // 1
        ,"LEVELDIMENSION_Y"         // 2
        ,"SCORE_EMERALD"            // 3
        ,"SCORE_SAPHIR"             // 4
        ,"SCORE_KEY"                // 5
        ,"SCORE_DYNAMITE"           // 6
        ,"SCORE_NUTCRACKING"        // 7
        ,"SCORE_STONING_BEETLE"     // 8
        ,"SCORE_STONING_MINE"       // 9
        ,"SCORE_STONING_ALIEN"      // 10
        ,"SCORE_STONING_YAM"        // 11
        ,"SCORE_TIME_FACTOR"        // 12
        ,"EMERALDS_TO_COLLECT"      // 13
        ,"TIME_TO_PLAY"             // 14
        ,"TIME_WHEEL_ROTATION"      // 15
        ,"TIME_BLUE_WALL"           // 16
        ,"SPEED_CHEESE_SPREAD"      // 17
        ,"LEVELTITLE"               // 18
        ,"LEVELAUTHOR"              // 19
        ,"LEVELHEADER_END"          // 20
        ,"LEVEL_START"              // 21
        ,NULL                       // Endekennung
    };

    if (puLevel == NULL) {
        return -1;
    }
    memset(&Playfield,0,sizeof(Playfield));
    nErrorCode = 0;
    Playfield.bInitOK = false;
    Playfield.uTotalScore = 0;
    nLabel = 0;
    while ((szLevelLabel[nLabel] != NULL) && (nErrorCode == 0)) {
        pFound = memmem((const char*)puLevel,EMERALD_MAX_HEADER_SIZE,szLevelLabel[nLabel],strlen(szLevelLabel[nLabel]));
        if (pFound != NULL) {
            nLabelLen = strlen(szLevelLabel[nLabel]);
            pFound = pFound + nLabelLen;
            switch (nLabel) {
                case(0):            // LEVELHEADER
                    memcpy(Playfield.szVersion,pFound,5);
                    Playfield.szVersion[5] = 0; // Stringende
                    break;
                case(1):            // LEVELDIMENSION_X
                    Playfield.uLevel_X_Dimension = *(uint32_t *)(pFound);
                    if ((Playfield.uLevel_X_Dimension < MIN_LEVEL_W) || (Playfield.uLevel_X_Dimension > MAX_LEVEL_W)) {
                        SDL_Log("%s: bad uLevel_X_Dimension: %d",__FUNCTION__,Playfield.uLevel_X_Dimension);
                        nErrorCode = -1;
                    }
                    break;
                case(2):            // LEVELDIMENSION_Y
                    Playfield.uLevel_Y_Dimension = *(uint32_t *)(pFound);
                    if ((Playfield.uLevel_Y_Dimension < MIN_LEVEL_H) || (Playfield.uLevel_Y_Dimension > MAX_LEVEL_H)) {
                        SDL_Log("%s: bad Level_Y_Dimension: %d",__FUNCTION__,Playfield.uLevel_Y_Dimension);
                        nErrorCode = -1;
                    }
                    break;
                case(3):            // SCORE_EMERALD
                    Playfield.uScoreEmerald = *(uint32_t *)(pFound);
                    break;
                case(4):            // SCORE_SAPHIR
                    Playfield.uScoreSaphir = *(uint32_t *)(pFound);
                    break;
                case(5):            // SCORE_KEY
                    Playfield.uScoreKey = *(uint32_t *)(pFound);
                    break;
                case(6):            // SCORE_DYNAMITE
                    Playfield.uScoreDynamite = *(uint32_t *)(pFound);
                    break;
                case(7):            // SCORE_NUTCRACKING
                    Playfield.uScoreNutCracking = *(uint32_t *)(pFound);
                    break;
                case(8):            // SCORE_STONING_BEETLE
                    Playfield.uScoreStoningBeetle = *(uint32_t *)(pFound);
                    break;
                case(9):            // SCORE_STONING_MINE
                    Playfield.uScoreStoningMine = *(uint32_t *)(pFound);
                    break;
                case(10):            // SCORE_STONING_ALIEN
                    Playfield.uScoreStoningAlien = *(uint32_t *)(pFound);
                    break;
                case(11):            // SCORE_STONING_YAM
                    Playfield.uScoreStoningYam = *(uint32_t *)(pFound);
                    break;
                case(12):            // SCORE_TIME_FACTOR
                    Playfield.uTimeScoreFactor = *(uint32_t *)(pFound);
                    break;
                case(13):            // EMERALDS_TO_COLLECT
                    Playfield.uEmeraldsToCollect = *(uint32_t *)(pFound);
                    break;
                case(14):            // TIME_TO_PLAY
                    Playfield.uTimeToPlay = *(uint32_t *)(pFound);
                    break;
                case(15):            // TIME_WHEEL_ROTATION
                    Playfield.uTimeWheelRotation = (*(uint32_t *)(pFound)) * ge_DisplayMode.refresh_rate;
                    break;
                case(16):            // TIME_BLUE_WALL
                    Playfield.uTimeBlueWall = (*(uint32_t *)(pFound)) * ge_DisplayMode.refresh_rate;
                    break;
                case(17):            // SPEED_CHEESE_SPREAD
                    Playfield.uCheeseSpreadSpeed = *(uint32_t *)(pFound);
                    break;
                case(18):            // LEVELTITLE
                    memset(Playfield.szLevelTitle,0,sizeof(Playfield.szLevelTitle));
                    memcpy(Playfield.szLevelTitle,pFound,sizeof(Playfield.szLevelTitle) - 1);
                    break;
                case(19):            // LEVELAUTHOR
                    memset(Playfield.szLevelAuthor,0,sizeof(Playfield.szLevelAuthor));
                    memcpy(Playfield.szLevelAuthor,pFound,sizeof(Playfield.szLevelAuthor) - 1);
                    break;
                case(20):            // LEVELHEADER_END
                    break;
                case(21):            // LEVEL_START
                    pLevelStart = (uint8_t*)pFound;
                    break;
                default:
                    break;
            }

        } else {
            SDL_Log("%s: Label: %s   not found",__FUNCTION__,szLevelLabel[nLabel]);
            nErrorCode = -1;
        }
        nLabel++;
    }
    if (pLevelStart == NULL) {
        nErrorCode = -1;
    }
    if (nErrorCode == 0) {
        Playfield.pLevel = (uint8_t*)malloc(Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension);
        if (Playfield.pLevel != NULL) {
            memcpy(Playfield.pLevel,pLevelStart,Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension);
            Playfield.pStatusAnimation = (uint32_t*)malloc(Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(uint32_t));
            if (Playfield.pStatusAnimation != NULL) {
                memset(Playfield.pStatusAnimation,0,Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(uint32_t));
                // Sichtbaren Bereich berechnen
                Playfield.uVisibleY = ((WINDOW_H - PANEL_H) / FONT_H);
                Playfield.uVisibleX = WINDOW_W / FONT_W;
                Playfield.uVisibleCenterY = Playfield.uVisibleY / 2;
                Playfield.uVisibleCenterX = Playfield.uVisibleX / 2;
                nErrorCode = GetManCoordinates();
                if (nErrorCode == 0) {
                    CheckLevelBorder();
                }
            } else {
                nErrorCode = -1;
                SDL_Log("%s: malloc failed for pStatusAnimation",__FUNCTION__);
            }
        } else {
            nErrorCode = -1;
            SDL_Log("%s: malloc failed for pLevel",__FUNCTION__);
        }
    }
    if (nErrorCode == 0) {
        Playfield.bInitOK = true;
    } else {
        SAFE_FREE(Playfield.pLevel);
        SAFE_FREE(Playfield.pStatusAnimation);
    }
    // Startposition im Level
    if (Playfield.uManXpos >= Playfield.uVisibleCenterX) {
        Playfield.nXpos = (Playfield.uManXpos - Playfield.uVisibleCenterX) * FONT_W;
    } else {
        Playfield.nXpos = 0;
    }
    if (Playfield.uManYpos >= Playfield.uVisibleCenterY) {
        Playfield.nYpos = (Playfield.uManYpos - Playfield.uVisibleCenterY) * FONT_H;
    } else {
        Playfield.nYpos = 0;
    }
    Playfield.uFrameCounter = 0;
    Playfield.bWheelRunning = false;
    Playfield.uTimeWheelRotationLeft = 0;
    Playfield.uWheelRunningXpos = 0;      // nur gültig, wenn bWheelRunning = true
    Playfield.uWheelRunningYpos = 0;      // nur gültig, wenn bWheelRunning = true
    Playfield.bHasRedKey = false;
    Playfield.bHasGreenKey = false;
    Playfield.bHasBlueKey = false;
    Playfield.bHasYellowKey = false;
    Playfield.bReplicatorRedOn = false;
    Playfield.bReplicatorGreenOn = false;
    Playfield.bReplicatorBlueOn = false;
    Playfield.bReplicatorYellowOn = false;
    PrintPlayfieldValues();
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           CheckLevelBorder
------------------------------------------------------------------------------
Beschreibung: Prüft, ob die Level-Umrandung mit "harter Mauer" an jeder Stelle abgrenzt.
              in der Struktur Playfield.x hinterlegt.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int CheckLevelBorder(void) {
    uint32_t I;
    int nErrorCode;

    nErrorCode = 0;
    // Obere Zeile prüfen
    for (I = 0; (I < Playfield.uLevel_X_Dimension) && (nErrorCode == 0); I++) {
        if (Playfield.pLevel[I] != EMERALD_HARDWALL) {
            SDL_Log("%s[top line]  invalid element (%x) found at %d",__FUNCTION__,Playfield.pLevel[I],I);
            nErrorCode = -1;
        }
    }
    // Untere Zeile prüfen
    for (I = Playfield.uLevel_X_Dimension * (Playfield.uLevel_Y_Dimension - 1); (I < Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension) && (nErrorCode == 0); I++) {
        if (Playfield.pLevel[I] != EMERALD_HARDWALL) {
            SDL_Log("%s[buttom line]  invalid element (%x) found at %d",__FUNCTION__,Playfield.pLevel[I],I);
            nErrorCode = -1;
        }
    }
    // Rechte und linke Spalte prüfen
    for (I = 0; (I < Playfield.uLevel_Y_Dimension) && (nErrorCode == 0); I++) {
        if (Playfield.pLevel[I * Playfield.uLevel_X_Dimension] != EMERALD_HARDWALL) {
            SDL_Log("%s[left column]  invalid element (%x) found at %d",__FUNCTION__,Playfield.pLevel[I],I);
            nErrorCode = -1;
        }
    }
    // Rechte Spalte prüfen
    for (I = 1; (I <= Playfield.uLevel_Y_Dimension) && (nErrorCode == 0); I++) {
        if (Playfield.pLevel[I * Playfield.uLevel_X_Dimension - 1] != EMERALD_HARDWALL) {
            SDL_Log("%s[right column]  invalid element (%x) found at %d",__FUNCTION__,Playfield.pLevel[I],I);
            nErrorCode = -1;
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetManCoordinates
------------------------------------------------------------------------------
Beschreibung: Ermittelt die X- und Y-Koordinate des Man im Level. Diese werden
              in der Struktur Playfield.x hinterlegt.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int GetManCoordinates(void) {
    int nErrorcode;
    int nManCount;
    uint32_t I;                 // Index im Level
    uint32_t uLinCoordinate;    // Lineare Koordinate

    nManCount = 0;
    nErrorcode = -1;
    // Das Gröbste wurde bereits in InitialisePlayfield() geprüft
    if (Playfield.pLevel != NULL) {
        // Prüfen, ob sich genau ein Man im Level befindet
        for (I = 0; I < (Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension); I++) {
            if (Playfield.pLevel[I] == EMERALD_MAN) {
                uLinCoordinate = I;
                nManCount++;
            }
        }
        if (nManCount == 1) {
            // Lineare Koordinate in X- und Y-Koordinate zerlegen
            Playfield.uManXpos = uLinCoordinate % Playfield.uLevel_X_Dimension;
            Playfield.uManYpos = uLinCoordinate / Playfield.uLevel_X_Dimension;
            nErrorcode = 0;
        } else {
            SDL_Log("%s: invalid man count: %d",__FUNCTION__,nManCount);
        }

    }
    return nErrorcode;
}


/*----------------------------------------------------------------------------
Name:           PrintPlayfieldValues
------------------------------------------------------------------------------
Beschreibung: Zeigt die Werte der Struktur Playfield.x an.

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void PrintPlayfieldValues() {
    if (Playfield.bInitOK) {
        printf("Playfield-Values\r\n");
        printf("================\r\n");
        printf("InitOK:                  %d\r\n",Playfield.bInitOK);
        printf("Version:                 %s\r\n",Playfield.szVersion);       // z.B. "01.00"
        printf("Level title:             %s\r\n",Playfield.szLevelTitle);
        printf("Level author:            %s\r\n",Playfield.szLevelAuthor);
        printf("Score Emerald:           %d\r\n",Playfield.uScoreEmerald);
        printf("Score Saphire:           %d\r\n",Playfield.uScoreSaphir);
        printf("Score Key:               %d\r\n",Playfield.uScoreKey);
        printf("Score Dynamite:          %d\r\n",Playfield.uScoreDynamite);
        printf("Score Nut cracking:      %d\r\n",Playfield.uScoreNutCracking);
        printf("Score stoning beetle:    %d\r\n",Playfield.uScoreStoningBeetle);
        printf("Score stoning mine:      %d\r\n",Playfield.uScoreStoningMine);
        printf("Score stoning alien:     %d\r\n",Playfield.uScoreStoningAlien);
        printf("Score stoning yam:       %d\r\n",Playfield.uScoreStoningYam);
        printf("Emerald to collect:      %d\r\n",Playfield.uEmeraldsToCollect);
        printf("Time Score Factor:       %d\r\n",Playfield.uTimeScoreFactor);
        printf("Cheese spread speed:     %d\r\n",Playfield.uCheeseSpreadSpeed);
        printf("Time to play:            %d\r\n",Playfield.uTimeToPlay);
        printf("Time Wheel Rotation:     %d\r\n",Playfield.uTimeWheelRotation);
        printf("Time blue wall:          %d\r\n",Playfield.uTimeBlueWall);
        printf("Total Score:             %d\r\n",Playfield.uTotalScore);            // Total-Score
        printf("Level Dimension:         X(%d) x Y(%d)\r\n",Playfield.uLevel_X_Dimension,Playfield.uLevel_Y_Dimension);
        printf("visible Dimension:       X(%d) x Y(%d)\r\n",Playfield.uVisibleX,Playfield.uVisibleY);   // Sichtbarer Bereich (nur von Fenstergröße abhängig)
        printf("visible Center:          X(%d) / Y(%d)\r\n",Playfield.uVisibleCenterX,Playfield.uVisibleCenterY);   // Zentrum des sichtbaren Bereichs (nur von Fenstergröße abhängig)
        printf("top left Pixel Position: X(%d) / Y(%d)\r\n",Playfield.nXpos,Playfield.nYpos);           // aktuelle X/Y-Pixelposition, abhängig von Man position
        printf("Man Position:            X(%d) / Y(%d)\r\n",Playfield.uManXpos,Playfield.uManYpos);   // Man-X/Y-Element-Koordinate
        printf("FrameCounter:            %u\r\n",Playfield.uFrameCounter);
    } else {
        printf("Error in level data, can't show playfield values\r\n");
    }
}
