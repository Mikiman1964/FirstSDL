#include <SDL2/SDL.h>
#include <stdio.h>
#include "EmeraldMine.h"
#include "loadlevel.h"
#include "mySDL.h"
#include "mystd.h"

extern PLAYFIELD Playfield;
extern SDL_DisplayMode ge_DisplayMode;

 uint8_t ge_uLevel[] = {"LEVELHEADER01.00"
                        "LEVELDIMENSION_X"
                        "\x40\x00\x00\x00"
                        "LEVELDIMENSION_Y"
                        "\x20\x00\x00\x00"
                        "SCORE_EMERALD"
                        "\x64\x00\x00\x00"
                        "SCORE_RUBY"
                        "\x30\x00\x00\x00"
                        "SCORE_SAPHIR"
                        "\xC8\x00\x00\x00"
                        "SCORE_CRYSTAL"
                        "\x08\x00\x00\x00"
                        "SCORE_MESSAGE"
                        "\x64\x00\x00\x00"
                        "SCORE_KEY"
                        "\x02\x00\x00\x00"
                        "SCORE_DYNAMITE"
                        "\x03\x00\x00\x00"
                        "SCORE_HAMMER"
                        "\x03\x00\x00\x00"
                        "SCORE_NUTCRACKING"
                        "\x04\x00\x00\x00"
                        "SCORE_STONING_BEETLE"
                        "\x05\x00\x00\x00"
                        "SCORE_STONING_MINE"
                        "\x06\x00\x00\x00"
                        "SCORE_STONING_ALIEN"
                        "\x07\x00\x00\x00"
                        "SCORE_STONING_YAM"
                        "\x08\x00\x00\x00"
                        "SCORE_TIME_FACTOR"
                        "\x02\x00\x00\x00"
                        "EMERALDS_TO_COLLECT"
                        "\x14\x00\x00\x00"
                        "TIME_TO_PLAY"
                        "\x90\x01\x00\x00"
                        "TIME_WHEEL_ROTATION"
                        "\x14\x00\x00\x00"
                        "TIME_MAGIC_WALL"
                        "\x10\x00\x00\x00"
                        "SPEED_CHEESE_SPREAD"
                        "\x00\x00\x00\x00"
                        //"\xE8\x03\x00\x00"
                        "TIME_COIN_SCORE"
                        "\x03\x00\x00\x00"
                        "TIME_COIN_TIME"
                        "\x32\x00\x00\x00"
                        "DYNAMITE_COUNT"
                        "\x3C\x00\x00\x00"
                        "HAMMER_COUNT"
                        "\x02\x00\x00\x00"
                        "LEVELTITLE"
                        "DER BUNKER                      "
                        "LEVELAUTHOR"
                        "MIKIMANN                        "
                        "REPLICATOR_RED"
                        "\x00\x02\x00\x00"
                        "REPLICATOR_GREEN"
                        "\x00\x0A\x00\x00"
                        "REPLICATOR_BLUE"
                        "\x00\x04\x00\x00"
                        "REPLICATOR_YELLOW"
                        "\x00\x25\x00\x00"
                        "LIGHTBARRIER_RED"
                        "\x00\x00\x00\x00"
                        "LIGHTBARRIER_GREEN"
                        "\x01\x00\x00\x00"
                        "LIGHTBARRIER_BLUE"
                        "\x01\x00\x00\x00"
                        "LIGHTBARRIER_YELLOW"
                        "\x00\x00\x00\x00"
                        "MESSAGE_1_TEXT"
                        "Willkommen im ersten Testlevel!\nDie Monster sind zur Zeit noch harmlos,\naber es funktionieren bereits viele Elemente.\n\nPress Fire or Space to confirm!\0"
                        "MESSAGE_2_TEXT"
                        "Dies ist die zweite Textbox!\n\nPress Fire or Space to confirm!\0"
                        "MESSAGE_3_TEXT"
                        "\0"
                        "MESSAGE_4_TEXT"
                        "\0"
                        "MESSAGE_5_TEXT"
                        "\0"
                        "MESSAGE_6_TEXT"
                        "\0"
                        "MESSAGE_7_TEXT"
                        "\0"
                        "MESSAGE_8_TEXT"
                        "\0"
                        "LEVELHEADER_END"
                        "LEVEL_START"
"\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83"
"\x83\x80\xE7\xE6\xDA\x08\xDD\xE6\xE9\x80\xDC\xE4\xDC\xE9\xD8\xE3\xDB\x80\xE4\xE0\xE5\xDC\x80\xEE\xE0\xEB\xDF\x80\xEA\xDB\xE3\x15\x83\x80\x80\x80\x80\x83\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x83"
"\x83\x83\x82\x82\x82\x80\x82\x80\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x80\x83\x80\x80\x80\x80\x83\x80\x80\x80\x80\x83\x80\x80\x80\x80\x83\x83\x83\x83\x83\x83\x83\x83\x83\x80\x80\x80\x83\x80\x80\x80\x83"
"\x83\x80\x83\x80\xEC\xEA\xDC\x82\xDA\xEC\xE9\xEA\xE6\xE9\x80\xE2\xDC\xF0\xEA\x80\xEB\xE6\x80\xEA\xDA\xE9\xE6\xE3\xE3\x80\x02\x83\x80\x80\x83\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x83"
"\x83\x80\x80\x83\x12\x80\x80\x80\x02\x93\x85\xEB\xDF\xE9\xE6\xEC\xDE\xDF\x01\xE3\xDC\xED\xDC\xE3\x01\x01\x99\x80\x80\x80\x83\x80\x80\x80\x80\x80\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x80\x83\x80\x80\x80\x83\x83\x83\x83\x83\x80\x80\x80\x83"
"\x83\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x08\x80\x80\x80\x80\x80\x80\x80\x99\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x83\x83\x83\x80\x80\x80\x80\x80\x80\x80\x80\x83"
"\x83\x80\x83\x80\x80\x83\x80\x80\x80\x80\x80\x80\x82\x80\x80\x80\x85\x86\x25\x80\x80\x25\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x15\x83\x83\x83"
"\x83\x80\x83\x80\x13\x80\x83\x80\x80\x82\x80\x80\xE7\xE9\xDC\xEA\xEA\x80\xEA\xE7\xD8\xDA\xDC\x80\x80\x80\x80\x83\x80\x80\x80\x83\x83\x83\x83\x83\x83\x83\x80\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x80\x80\x83\x83\x80\x80\x80\x80\x80\x83\x80\x83\x80\x80\x83"
"\x83\x80\x83\x83\x83\x80\x80\x83\x80\x80\x80\xDD\xE6\xE9\x80\xE4\xD8\xE0\xE5\x80\xE7\xD8\xE9\xEB\x80\x80\xA5\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x83\x83\x80\x80\x80\x80\x80\x83"
"\x83\x82\x83\x82\x82\x83\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x82\x80\x25\x82\x29\x83\x83\x80\x80\x80\x80\x80\x83\x80\x80\x89\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x83\x89\x80\x80\x80\x83\x83\x83\x80\x83"
"\x83\x82\x83\x82\x82\x80\x83\x80\x80\x34\x35\x36\x2E\x2F\x30\x18\x19\x1A\x1E\x1F\x20\x82\x80\x82\x83\x02\x02\x80\x83\x83\x83\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x83\x80\x80\x80\x80\x83\x83\x80\x83\x83\x80\x83\x80\x80\x80\x83"
"\x83\x82\x83\x83\x83\x80\x83\x80\x80\x37\x80\x38\x31\x80\x32\x1B\x80\x1C\x21\x80\x22\x82\x0A\xA0\x02\x02\x02\x02\x02\x02\x02\x02\x83\x83\x83\x83\x83\x89\x80\x80\x80\x80\x80\x83\x83\x83\x83\x83\x83\x83\x80\x80\x80\x83\x80\x89\x80\x80\x83\x80\x80\x80\x80\x83"
"\x83\x82\x83\x09\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x25\x80\x83\x25\xA4\x02\x89\x02\x8E\x80\x8E\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x80\x80\x80\x80\x80\x80\x80\x83"
"\x83\x83\x83\x83\x83\x83\x83\x83\x83\x80\x83\x99\x83\x08\x80\x80\x11\x80\x80\x24\x0A\xA1\x80\x80\x83\x89\x83\x83\x83\x4A\x83\x83\x83\x83\x83\x83\x83\x89\x80\x80\x80\x57\x83\x80\x80\x80\x80\x80\x80\x80\x83\x86\x80\x80\x80\x80\x80\x80\x80\x80\x82\x83\x83\x83"
"\x83\x15\x80\x80\x83\x9A\x9A\x84\x84\x84\x80\x80\x23\x83\x39\x80\x40\x80\x80\x13\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x83\x83\x83\x83\x89\x83\x83\x83\x83\x83\x80\x83\x15\x80\x80\x83"
"\x83\x82\x82\x82\x83\x9A\x9A\x02\x84\x84\x84\x11\x42\x80\x80\x13\x80\x80\x41\xAB\x80\x80\x80\x9E\x80\x89\x80\x54\x80\x80\x80\x0E\x80\x80\x80\x80\x80\x89\x80\x80\x80\x83\x80\x80\x80\x80\x80\x83\x80\x80\x80\x09\x80\x80\x0B\x80\x80\x83\x83\x83\x83\x83\x83\x83"
"\x83\x80\x80\x80\x80\x84\x84\x84\x84\x84\x84\x80\x33\x83\x1D\x83\x92\x92\x83\x03\x80\x80\x83\x80\x80\x80\x80\x83\x80\x80\x0C\x83\x0D\x80\x80\x83\x83\x80\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x80\x80\x80\x09\x80\x80\x0B\x80\x80\x83\x80\x88\x80\x80\x09\x83"
"\x83\x82\x82\x80\x83\x84\x84\x84\x84\x84\x84\x80\x80\x8D\x8D\x8D\x8D\x8D\x8D\x80\x9D\x80\x83\x80\x80\x89\x80\x83\x80\x80\x80\x0F\x80\x83\x80\x80\x80\x89\x80\x80\x83\x83\x80\x80\x80\x80\x80\x83\x80\x80\x80\x09\x80\x80\x0B\x80\x80\x83\x83\x83\x83\x83\x82\x83"
"\x83\x80\x08\x83\x80\x84\x84\x84\x83\x83\x83\x80\x83\x8D\x8D\x3A\x3A\x3A\x83\x80\x83\x83\x83\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x83\x80\x80\x83\x83\x83\x83\x83\x8C\x83\x83\x83\x83\x80\x80\x80\x80\x80\x80\x83"
"\x83\x83\x83\x83\x83\x83\x84\x84\x83\x80\x80\x80\x80\x83\x80\x80\x80\x80\x83\x80\x80\x80\x83\x83\x83\x8E\x83\x83\x83\x83\x83\x83\x83\x83\x4D\x83\x83\x89\x80\x80\x83\x80\x80\x80\x83\x83\x83\x83\x80\x80\x83\x80\x80\x80\x83\x83\x80\x80\x83\x80\x80\x80\x80\x83"
"\x83\x80\x80\x83\x80\x02\x83\x80\x83\x80\xA3\x83\x80\x80\x80\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x83\x83\x80\x83\x80\x83\x8A\x83\x80\x80\x83\x80\x83\x83\x83\x8B\x83"
"\x83\x80\x80\x83\x80\x80\x80\x80\x80\x80\x83\x83\xA7\x80\x80\x89\x80\x80\x80\x80\xA2\x83\x83\x83\x83\x89\x83\x83\x83\x83\x83\x83\x80\x80\x80\x80\x80\x83\x83\x83\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x80\x83\x80\x83\x80\x83\x80\x80\x80\x80\x80\x80\x83"
"\x83\x80\x80\x80\x80\xAC\x80\x83\x83\x80\x98\x9F\x80\x80\x80\x80\xAA\x83\xA6\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x83\x80\x80\x80\x80\x80\x83\x80\x80\x83\x80\x83\x83\x83\x83\x83\x83\x83\x83\x80\x83"
"\x83\x83\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x80\x89\x82\x82\x83\x80\x83\x80\x80\x80\x83\x89\x80\x80\x80\x80\x83\x83\x83\x83\x83\x83\x83\x80\x80\x80\x80\x80\x80\x80\x83\x80\x83\x80\x80\x83\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x87\x83\x80\x83"
"\x83\x83\x82\x09\x80\x80\x80\x80\x80\x80\x80\x80\x17\x82\x80\x80\x1E\x1F\x20\x80\xA9\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x83\x83\x80\x80\x80\x80\x80\x83\x80\x83\x80\x80\x83\x80\x80\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x80\x83"
"\x83\x80\x82\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x82\x80\x89\x21\x29\x22\x80\x83\x83\x83\x83\x83\x89\x83\x83\x83\x83\x83\x80\x83\x80\x80\x80\x80\x80\x83\x83\x83\x80\x80\x80\x80\x80\x83\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x83"
"\x83\x80\x82\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x82\x80\x80\x80\x82\x80\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x83\x80\x83\x83\x83\x83\x80\x80\x83\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x83"
"\x83\x80\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x82\x80\x89\x80\x80\x80\x80\x80\x80\x83\x80\x80\x89\x83\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x83\x83\x80\x83\x80\x83\x83\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x83\x80\x80\x83"
"\x83\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x89\x80\x80\x80\x80\x80\x80\x80\x83\x82\x80\x80\x83\x80\x80\x82\x83\x80\x80\x83\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x83\x83\x83\x83\x83\x80\x80\x80\x80\x80\x83\x80\x83"
"\x83\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x23\x80\x80\x7A\x7B\x7B\x7B\x7C\x83\x80\x83\x80\x82\x83\x80\x83\x82\x16\x83\x83\x83\x83\x83\x83\x83\x80\x83\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x82\x82\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x83"
"\x83\x85\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x7D\x7E\x7E\x7E\x7F\x80\x80\x83\x0A\x82\x80\x80\x80\x82\x80\x83\x80\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x80\x80\x80\x83\x80\x80\x80\x82\xDC\xE5\xDB\xDC\x80\xDE\xDC\xE3\xD8\xDC\xE5\xDB\xDC\x83"
"\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83"
};


/*
 uint8_t ge_uLevel[] = {"LEVELHEADER01.00"
                        "LEVELDIMENSION_X"
                        "\x1B\x00\x00\x00"
                        "LEVELDIMENSION_Y"
                        "\x09\x00\x00\x00"
                        "SCORE_EMERALD"
                        "\x64\x00\x00\x00"
                        "SCORE_RUBY"
                        "\x30\x00\x00\x00"
                        "SCORE_SAPHIR"
                        "\xC8\x00\x00\x00"
                        "SCORE_CRYSTAL"
                        "\x08\x00\x00\x00"
                        "SCORE_MESSAGE"
                        "\x64\x00\x00\x00"
                        "SCORE_KEY"
                        "\x02\x00\x00\x00"
                        "SCORE_DYNAMITE"
                        "\x03\x00\x00\x00"
                        "SCORE_HAMMER"
                        "\x03\x00\x00\x00"
                        "SCORE_NUTCRACKING"
                        "\x04\x00\x00\x00"
                        "SCORE_STONING_BEETLE"
                        "\x05\x00\x00\x00"
                        "SCORE_STONING_MINE"
                        "\x06\x00\x00\x00"
                        "SCORE_STONING_ALIEN"
                        "\x07\x00\x00\x00"
                        "SCORE_STONING_YAM"
                        "\x08\x00\x00\x00"
                        "SCORE_TIME_FACTOR"
                        "\x02\x00\x00\x00"
                        "EMERALDS_TO_COLLECT"
                        "\x2C\x01\x00\x00"
                        "TIME_TO_PLAY"
                        "\xFF\x00\x00\x00"
                        "TIME_WHEEL_ROTATION"
                        "\x64\x00\x00\x00"
                        "TIME_MAGIC_WALL"
                        "\x32\x00\x00\x00"
                        "SPEED_CHEESE_SPREAD"
                        "\x0A\x00\x00\x00"
                        "TIME_COIN_SCORE"
                        "\x03\x00\x00\x00"
                        "TIME_COIN_TIME"
                        "\x32\x00\x00\x00"
                        "DYNAMITE_COUNT"
                        "\x3C\x00\x00\x00"
                        "HAMMER_COUNT"
                        "\x50\x00\x00\x00"
                        "LEVELTITLE"
                        "12345678   DER BUNKER   56789012"
                        "LEVELAUTHOR"
                        "123456789   MIKIMANN   456789012"
                        "REPLICATOR_RED"
                        "\x00\x16\x00\x00"
                        "REPLICATOR_GREEN"
                        "\x00\x02\x00\x00"
                        "REPLICATOR_BLUE"
                        "\x00\x04\x00\x00"
                        "REPLICATOR_YELLOW"
                        "\x00\x12\x00\x00"
                        "LIGHTBARRIER_RED"
                        "\x00\x00\x00\x00"
                        "LIGHTBARRIER_GREEN"
                        "\x01\x00\x00\x00"
                        "LIGHTBARRIER_BLUE"
                        "\x01\x00\x00\x00"
                        "LIGHTBARRIER_YELLOW"
                        "\x00\x00\x00\x00"
                        "MESSAGE_1_TEXT"
                        "DIES IST DER TEXT FUER MESSAGE 1!\n\nPress Fire or Space to confirm!\0"
                        "MESSAGE_2_TEXT"
                        "\0"
                        "MESSAGE_3_TEXT"
                        "\0"
                        "MESSAGE_4_TEXT"
                        "\0"
                        "MESSAGE_5_TEXT"
                        "\0"
                        "MESSAGE_6_TEXT"
                        "\0"
                        "MESSAGE_7_TEXT"
                        "\0"
                        "MESSAGE_8_TEXT"
                        "\0"
                        "LEVELHEADER_END"
                        "LEVEL_START"
"\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83\x83"
"\x83\x80\x25\x80\x80\x01\x80\x10\x80\x80\x01\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x2E\x2F\x30\x80\x80\x83"
"\x83\x82\x82\x80\x80\x82\x82\x82\x82\x80\x82\x12\x12\x12\x82\x12\x12\x12\x12\x12\x82\x31\x80\x32\x80\x80\x83"
"\x83\x82\x98\x80\x80\x94\x82\x40\x82\x12\x94\x12\x12\x82\x82\x01\x12\x12\x12\x12\x82\x80\x80\x80\x80\x80\x83"
"\x83\x80\x80\x80\x80\x94\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x12\x12\x12\x85\x86\x87\x88\x80\x80\x33\x83"
"\x83\x80\x80\x80\x80\x02\x80\x80\x80\x80\x82\x82\x80\x82\x80\x80\x12\x12\x12\x82\x82\x80\x80\x80\x80\x80\x83"
"\x83\x80\x7A\x7B\x7B\x7B\x7C\x82\x82\x80\x80\x82\x80\x82\x80\x80\x12\x12\x12\x82\x82\x80\x80\x80\x80\x80\x83"
"\x83\x14\x7D\x7E\x7E\x7E\x7F\x82\x82\x80\x0B\x82\x82\x82\x80\x02\x12\x12\x12\x12\x82\x80\x80\x80\x80\x08\x83"
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
    uint32_t I;
    uint32_t uMessageIndex;
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
        ,"TIME_MAGIC_WALL"          // 16
        ,"SPEED_CHEESE_SPREAD"      // 17
        ,"LEVELTITLE"               // 18
        ,"LEVELAUTHOR"              // 19
        ,"LEVELHEADER_END"          // 20
        ,"LEVEL_START"              // 21
        ,"REPLICATOR_RED"           // 22
        ,"REPLICATOR_GREEN"         // 23
        ,"REPLICATOR_BLUE"          // 24
        ,"REPLICATOR_YELLOW"        // 25
        ,"SCORE_RUBY"               // 26
        ,"SCORE_CRYSTAL"            // 27
        ,"TIME_COIN_SCORE"          // 28
        ,"TIME_COIN_TIME"           // 29
        ,"DYNAMITE_COUNT"           // 30
        ,"HAMMER_COUNT"             // 31
        ,"SCORE_HAMMER"             // 32
        ,"SCORE_MESSAGE"            // 33
        ,"MESSAGE_1_TEXT"           // 34
        ,"MESSAGE_2_TEXT"           // 35
        ,"MESSAGE_3_TEXT"           // 36
        ,"MESSAGE_4_TEXT"           // 37
        ,"MESSAGE_5_TEXT"           // 38
        ,"MESSAGE_6_TEXT"           // 39
        ,"MESSAGE_7_TEXT"           // 40
        ,"MESSAGE_8_TEXT"           // 41
        ,"LIGHTBARRIER_RED"         // 42
        ,"LIGHTBARRIER_GREEN"       // 43
        ,"LIGHTBARRIER_BLUE"        // 44
        ,"LIGHTBARRIER_YELLOW"      // 45
        ,NULL                       // Endekennung
    };

    if (puLevel == NULL) {
        return -1;
    }
    uMessageIndex = 0;
    memset(&Playfield,0,sizeof(Playfield));
    for (I = 0; I < EMERALD_MAX_MESSAGES; I++) {
        Playfield.pMessage[I] = NULL;
    }
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
                    Playfield.uTimeToPlay = *(uint32_t *)(pFound) * ge_DisplayMode.refresh_rate;
                    break;
                case(15):            // TIME_WHEEL_ROTATION
                    Playfield.uTimeWheelRotation = (*(uint32_t *)(pFound)) * ge_DisplayMode.refresh_rate;
                    break;
                case(16):            // TIME_BLUE_WALL
                    Playfield.uTimeMagicWall = (*(uint32_t *)(pFound)) * ge_DisplayMode.refresh_rate;
                    break;
                case(17):            // SPEED_CHEESE_SPREAD
                    Playfield.uCheeseSpreadSpeed = *(uint32_t *)(pFound);
                    if (Playfield.uCheeseSpreadSpeed > 1000) {
                        Playfield.uCheeseSpreadSpeed = 1000;
                    }
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
                case(22):            // REPLICATOR_RED
                    Playfield.bReplicatorRedOn = (pFound[0] != 0);
                    Playfield.uReplicatorRedObject = pFound[1];
                    break;
                case(23):            // REPLICATOR_GREEN
                    Playfield.bReplicatorGreenOn = (pFound[0] != 0);
                    Playfield.uReplicatorGreenObject = pFound[1];
                    break;
                case(24):            // REPLICATOR_BLUE
                    Playfield.bReplicatorBlueOn = (pFound[0] != 0);
                    Playfield.uReplicatorBlueObject = pFound[1];
                    break;
                case(25):            // REPLICATOR_YELOW
                    Playfield.bReplicatorYellowOn = (pFound[0] != 0);
                    Playfield.uReplicatorYellowObject = pFound[1];
                    break;
                case(26):            // SCORE_RUBY
                    Playfield.uScoreRuby = *(uint32_t *)(pFound);
                    break;
                case(27):            // SCORE_CRYSTAL
                    Playfield.uScoreCrystal = *(uint32_t *)(pFound);
                    break;
                case(28):            // TIME_COIN_SCORE
                    Playfield.uScoreTimeCoin = *(uint32_t *)(pFound);
                    break;
                case(29):            // TIME_COIN_TIME
                    Playfield.uAdditonalTimeCoinTime = *(uint32_t *)(pFound) * ge_DisplayMode.refresh_rate;
                    break;
                case(30):            // DYNAMIT_COUNT
                    Playfield.uDynamiteCount = *(uint32_t *)(pFound);
                    break;
                case(31):            // HAMMER_COUNT
                    Playfield.uHammerCount = *(uint32_t *)(pFound);
                    break;
                case(32):            // HAMMER_SCORE
                    Playfield.uScoreHammer = *(uint32_t *)(pFound);
                    break;
                case(33):            // SCORE_MESSAGE
                    Playfield.uScoreMessage = *(uint32_t *)(pFound);
                    break;
                case(34):            // MESSAGE_1_TEXT
                case(35):            // MESSAGE_2_TEXT
                case(36):            // MESSAGE_3_TEXT
                case(37):            // MESSAGE_4_TEXT
                case(38):            // MESSAGE_5_TEXT
                case(39):            // MESSAGE_6_TEXT
                case(40):            // MESSAGE_7_TEXT
                case(41):            // MESSAGE_8_TEXT
                    uMessageIndex = nLabel - 34;
                    if (strlen(pFound) > 0) {
                        Playfield.pMessage[uMessageIndex] = (char*)malloc(strlen(pFound) + 1);
                        if (Playfield.pMessage[uMessageIndex] != NULL) {
                            strcpy(Playfield.pMessage[uMessageIndex],pFound);
                        }
                    }
                    break;
                case(42):           // LIGHTBARRIER_RED
                    Playfield.bLightBarrierRedOn = (pFound[0] != 0);
                    break;
                case(43):           // LIGHTBARRIER_GREEN
                    Playfield.bLightBarrierGreenOn = (pFound[0] != 0);
                    break;
                case(44):           // LIGHTBARRIER_BLUE
                    Playfield.bLightBarrierBlueOn = (pFound[0] != 0);
                    break;
                case(45):           // LIGHTBARRIER_YELLOW
                    Playfield.bLightBarrierYellowOn = (pFound[0] != 0);
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
        Playfield.pStatusAnimation = (uint32_t*)malloc(Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(uint32_t));
        Playfield.pPostAnimation = (POSTANIMATION*)malloc(Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(POSTANIMATION));
        if ((Playfield.pLevel != NULL) && (Playfield.pStatusAnimation != NULL) && (Playfield.pPostAnimation != NULL)) {
            memset(Playfield.pStatusAnimation,0,Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(uint32_t));
            memcpy(Playfield.pLevel,pLevelStart,Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension);
            memset(Playfield.pPostAnimation,0,Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(POSTANIMATION));
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
            SDL_Log("%s: malloc failed for pLevel (p = %p) or pStatusAnimation (p = %p)",__FUNCTION__,Playfield.pLevel,Playfield.pStatusAnimation);
        }
    }
    if (nErrorCode == 0) {
        Playfield.bInitOK = true;
    } else {
        SAFE_FREE(Playfield.pLevel);
        SAFE_FREE(Playfield.pStatusAnimation);
        SAFE_FREE(Playfield.pPostAnimation);
    }
    // Startposition im Level
    if (Playfield.uManXpos >= Playfield.uVisibleCenterX) {
        Playfield.nTopLeftXpos = (Playfield.uManXpos - Playfield.uVisibleCenterX) * FONT_W;
    } else {
        Playfield.nTopLeftXpos = 0;
    }
    if (Playfield.uManYpos >= Playfield.uVisibleCenterY) {
        Playfield.nTopLeftYpos = (Playfield.uManYpos - Playfield.uVisibleCenterY) * FONT_H;
    } else {
        Playfield.nTopLeftYpos = 0;
    }
    Playfield.uFrameCounter = 0;
    Playfield.bMagicWallRunning = false;
    Playfield.bMagicWallWasOn = false;
    Playfield.uTimeMagicWallLeft = 0;
    Playfield.bWheelRunning = false;
    Playfield.uTimeWheelRotationLeft = 0;
    Playfield.uWheelRunningXpos = 0;      // nur gültig, wenn bWheelRunning = true
    Playfield.uWheelRunningYpos = 0;      // nur gültig, wenn bWheelRunning = true
    Playfield.bHasRedKey = false;
    Playfield.bHasGreenKey = false;
    Playfield.bHasBlueKey = false;
    Playfield.bHasYellowKey = false;
    Playfield.bManDead = false;
    Playfield.uShowMessageNo = 0;   // TODO
    PrintPlayfieldValues();
    InitRollUnderground();
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
        if (Playfield.pLevel[I] != EMERALD_WALL_HARD) {
            SDL_Log("%s[top line]  invalid element (%x) found at %d",__FUNCTION__,Playfield.pLevel[I],I);
            nErrorCode = -1;
        }
    }
    // Untere Zeile prüfen
    for (I = Playfield.uLevel_X_Dimension * (Playfield.uLevel_Y_Dimension - 1); (I < Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension) && (nErrorCode == 0); I++) {
        if (Playfield.pLevel[I] != EMERALD_WALL_HARD) {
            SDL_Log("%s[buttom line]  invalid element (%x) found at %d",__FUNCTION__,Playfield.pLevel[I],I);
            nErrorCode = -1;
        }
    }
    // Rechte und linke Spalte prüfen
    for (I = 0; (I < Playfield.uLevel_Y_Dimension) && (nErrorCode == 0); I++) {
        if (Playfield.pLevel[I * Playfield.uLevel_X_Dimension] != EMERALD_WALL_HARD) {
            SDL_Log("%s[left column]  invalid element (%x) found at %d",__FUNCTION__,Playfield.pLevel[I],I);
            nErrorCode = -1;
        }
    }
    // Rechte Spalte prüfen
    for (I = 1; (I <= Playfield.uLevel_Y_Dimension) && (nErrorCode == 0); I++) {
        if (Playfield.pLevel[I * Playfield.uLevel_X_Dimension - 1] != EMERALD_WALL_HARD) {
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
        printf("InitOK:                   %d\r\n",Playfield.bInitOK);
        printf("Version:                  %s\r\n",Playfield.szVersion);       // z.B. "01.00"
        printf("Level title:              %s\r\n",Playfield.szLevelTitle);
        printf("Level author:             %s\r\n",Playfield.szLevelAuthor);
        printf("Score Emerald:            %d\r\n",Playfield.uScoreEmerald);
        printf("Score Ruby:               %d\r\n",Playfield.uScoreRuby);
        printf("Score crystal:            %d\r\n",Playfield.uScoreCrystal);
        printf("Score Saphire:            %d\r\n",Playfield.uScoreSaphir);
        printf("Score Key:                %d\r\n",Playfield.uScoreKey);
        printf("Score Dynamite:           %d\r\n",Playfield.uScoreDynamite);
        printf("Score Hammer:             %d\r\n",Playfield.uScoreHammer);
        printf("Score Nut cracking:       %d\r\n",Playfield.uScoreNutCracking);
        printf("Score stoning beetle:     %d\r\n",Playfield.uScoreStoningBeetle);
        printf("Score stoning mine:       %d\r\n",Playfield.uScoreStoningMine);
        printf("Score stoning alien:      %d\r\n",Playfield.uScoreStoningAlien);
        printf("Score stoning yam:        %d\r\n",Playfield.uScoreStoningYam);
        printf("Score time coin:          %d\r\n",Playfield.uScoreTimeCoin);
        printf("Additional time for coin: %d\r\n",Playfield.uAdditonalTimeCoinTime);
        printf("Emerald to collect:       %d\r\n",Playfield.uEmeraldsToCollect);
        printf("Dynamite Count:           %d\r\n",Playfield.uDynamiteCount);
        printf("Hammer Count:             %d\r\n",Playfield.uHammerCount);
        printf("Time Score Factor:        %d\r\n",Playfield.uTimeScoreFactor);
        printf("Cheese spread speed:      %d\r\n",Playfield.uCheeseSpreadSpeed);
        printf("Time to play:             %d\r\n",Playfield.uTimeToPlay);
        printf("Time Wheel Rotation:      %d\r\n",Playfield.uTimeWheelRotation);
        printf("Time magic wall:          %d\r\n",Playfield.uTimeMagicWall);
        printf("Total Score:              %d\r\n",Playfield.uTotalScore);            // Total-Score
        printf("Level Dimension:          X(%d) x Y(%d)\r\n",Playfield.uLevel_X_Dimension,Playfield.uLevel_Y_Dimension);
        printf("visible Dimension:        X(%d) x Y(%d)\r\n",Playfield.uVisibleX,Playfield.uVisibleY);   // Sichtbarer Bereich (nur von Fenstergröße abhängig)
        printf("visible Center:           X(%d) / Y(%d)\r\n",Playfield.uVisibleCenterX,Playfield.uVisibleCenterY);   // Zentrum des sichtbaren Bereichs (nur von Fenstergröße abhängig)
        printf("top left Pixel Position:  X(%d) / Y(%d)\r\n",Playfield.nTopLeftXpos,Playfield.nTopLeftYpos);           // aktuelle X/Y-Pixelposition, abhängig von Man position
        printf("Man Position:             X(%d) / Y(%d)\r\n",Playfield.uManXpos,Playfield.uManYpos);   // Man-X/Y-Element-Koordinate
        printf("FrameCounter:             %u\r\n",Playfield.uFrameCounter);
    } else {
        printf("Error in level data, can't show playfield values\r\n");
    }
}
