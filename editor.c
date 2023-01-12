#include <SDL2/SDL.h>
#include <stdio.h>
#include "buttons.h"
#include "EmeraldMine.h"
#include "editor.h"
#include "KeyboardMouse.h"
#include "loadlevel.h"
#include "md5.h"
#include "mystd.h"
#include "mySDL.h"
#include "scroller.h"
#include "zlib.h"

ED Ed;
extern BUTTONS Buttons;
extern PLAYFIELD Playfield;
extern INPUTSTATES InputStates;
extern SDL_DisplayMode ge_DisplayMode;

uint8_t g_PanelColorPatterns[] = {
//                           R    G    B
                            0x42,0x21,0x00,         // braun 1, dunkel
                            0x6C,0x36,0x00,         // braun 2
                            0x9D,0x4F,0x00,         // braun 3
                            0xDF,0x70,0x00,         // braun 4, hell
                            };

char ge_szElementNames[][64] =
                         {"no element",                         // 0x00
                          "Stone",                              // 0x01
                          "Sapphire",                           // 0x02
                          "Perl",                               // 0x03
                          "Mole up",                            // 0x04
                          "Mole right",                         // 0x05
                          "Mole down",                          // 0x06
                          "Mole left",                          // 0x07
                          "Mine up",                            // 0x08
                          "Mine right",                         // 0x09
                          "Mine down",                          // 0x0A
                          "Mine left",                          // 0x0B
                          "Light barrier switch red",           // 0x0C
                          "Light barrier switch green",         // 0x0D
                          "Light barrier switch blue",          // 0x0E
                          "Light barrier switch yellow",        // 0x0F
                          "Bomb",                               // 0x10
                          "Crystal",                            // 0x11
                          "Emerald",                            // 0x12
                          "Ruby",                               // 0x13
                          "Beetle up",                          // 0x14
                          "Beetle right",                       // 0x15
                          "Beetle down",                        // 0x16
                          "Beetle left",                        // 0x17
                          "Replicator red, top left",           // 0x18
                          "Replicator red, top middle",         // 0x19
                          "Replicator red, top right",          // 0x1A
                          "Replicator red, bottom left",        // 0x1B
                          "Replicator red, botton right",       // 0x1C
                          "Replicator red, switch",             // 0x1D
                          "Replicator yellow, top left",        // 0x1E
                          "Replicator yellow, top middle",      // 0x1F
                          "Replicator yellow, top right",       // 0x20
                          "Replicator yellow, bottom left",     // 0x21
                          "Replicator yellow, botton right",    // 0x22
                          "Replicator yellow, switch",          // 0x23
                          "Time coin",                          // 0x24
                          "Nut",                                // 0x25
                          "Alien",                              // 0x26
                          "Yam",                                // 0x27
                          "Fixed mine",                         // 0x28
                          "Magic wall, switch",                 // 0x29
                          "Magic wall, steel",                  // 0x2A
                          "Sand from mole, internal element",   // 0x2B
                          "Green cheese, disappear, int. elem.",// 0x2C
                          "Light switch",                       // 0x2D
                          "Replicator green, top left",         // 0x2E
                          "Replicator green, top middle",       // 0x2F
                          "Replicator green, top right",        // 0x30
                          "Replicator green, bottom left",      // 0x31
                          "Replicator green, botton right",     // 0x32
                          "Replicator green, switch",           // 0x33
                          "Replicator blue, top left",          // 0x34
                          "Replicator blue, top middle",        // 0x35
                          "Replicator blue, top right",         // 0x36
                          "Replicator blue, bottom left",       // 0x37
                          "Replicator blue, botton right",      // 0x38
                          "Replicator blue, switch",            // 0x39
                          "Dynamite, off",                      // 0x3A
                          "Dynamite, on",                       // 0x3B
                          "Door, wood, red",                    // 0x3C
                          "Door, wood, yellow",                 // 0x3D
                          "Door, wood, blue",                   // 0x3E
                          "Door, wood, green",                  // 0x3F
                          "Man",                                // 0x40
                          "Message 1",                          // 0x41
                          "Message 2",                          // 0x42
                          "Message 3",                          // 0x43
                          "Message 4",                          // 0x44
                          "Message 5",                          // 0x45
                          "Message 6",                          // 0x46
                          "Message 7",                          // 0x47
                          "Message 8",                          // 0x48
                          "Light barrier, red, up",             // 0x49
                          "Light barrier, red, down",           // 0x4A
                          "Light barrier, red, left",           // 0x4B
                          "Light barrier, red, right",          // 0x4C
                          "Light barrier, green, up",           // 0x4D
                          "Light barrier, green, down",         // 0x4E
                          "Light barrier, green, left",         // 0x4F
                          "Light barrier, green, right",        // 0x50
                          "Light barrier, blue, up",            // 0x51
                          "Light barrier, blue, down",          // 0x52
                          "Light barrier, blue, left",          // 0x53
                          "Light barrier, blue, right",         // 0x54
                          "Light barrier, yellow, up",          // 0x55
                          "Light barrier, yellow, down",        // 0x56
                          "Light barrier, yellow, left",        // 0x57
                          "Light barrier, yellow, right",       // 0x58
                          "Beam, red, vertical",                // 0x59
                          "Beam, red, horizontal",              // 0x5A
                          "Beam, green, vertical",              // 0x5B
                          "Beam, green, horizontal",            // 0x5C
                          "Beam, blue, vertical",               // 0x5D
                          "Beam, blue, horizontal",             // 0x5E
                          "Beam, yellow, vertical",             // 0x5F
                          "Beam, yellow, horizontal",           // 0x60
                          "Beam, cross, internal element",      // 0x61
                          "Door, Emerald",                      // 0x62
                          "Door, Multicolor",                   // 0x63
                          "Door, only up",                      // 0x64
                          "Door, only down",                    // 0x65
                          "Door, only left",                    // 0x66
                          "Door, only right",                   // 0x67
                          "Switch, Timedoor",                   // 0x68
                          "Timedoor",                           // 0x69
                          "Switched door, open",                // 0x6A
                          "Switched door, closed",              // 0x6B
                          "Switch, Switched door",              // 0x6C
                          "Enddoor, not ready, steel",          // 0x6D
                          "Enddoor, ready, steel",              // 0x6E
                          "Key, white",                         // 0x6F
                          "Door, white",                        // 0x70
                          "Door, secret, white",                // 0x71
                          "Door, wood, white",                  // 0x72
                          "Key, general",                       // 0x73
                          "Megabomb",                           // 0x74
                          "Wall, soft, cornered, with Emerald", // 0x75
                          "Wall, soft, cornered, with Ruby",    // 0x76
                          "Wall, soft, cornered, with Sapphire",// 0x77
                          "Wall, soft, cornered, with Perl",    // 0x78
                          "Acid pool destroy, internal element",// 0x79
                          "Acid pool, top left",                // 0x7A
                          "Acid pool, top middle",              // 0x7B
                          "Acid pool, top right",               // 0x7C
                          "Acid pool, bottom left",             // 0x7D
                          "Acid pool, bottom middle",           // 0x7E
                          "Acid pool, bottom right",            // 0x7F
                          "Space",                              // 0x80
                          "Wall, soft, round",                  // 0x81
                          "Sand",                               // 0x82
                          "Steel",                              // 0x83
                          "Wall, soft, cornered",               // 0x84
                          "Key, red",                           // 0x85
                          "Key, yellow",                        // 0x86
                          "Key, blue",                          // 0x87
                          "Key, green",                         // 0x88
                          "Door, red",                          // 0x89
                          "Door, yellow",                       // 0x8A
                          "Door, blue",                         // 0x8B
                          "Door, green",                        // 0x8C
                          "Hammer",                             // 0x8D
                          "Door, secret, red",                  // 0x8E
                          "Door, secret, yellow",               // 0x8F
                          "Door, secret, blue",                 // 0x90
                          "Door, secret, green",                // 0x91
                          "Magic wall",                         // 0x92
                          "Wheel",                              // 0x93
                          "Swamp",                              // 0x94
                          "Swamp with Stone",                   // 0x95
                          "Stone s.in swamp, internal element", // 0x96
                          "Stone s.out swamp, internal element",// 0x97
                          "Enddoor, not ready",                 // 0x98
                          "Green drop comes, internal element", // 0x99
                          "Green drop",                         // 0x9A
                          "Green cheese",                       // 0x9B
                          "Explosion P1, internal element",     // 0x9C
                          "Explosion P2, internal element",     // 0x9D
                          "Wall, soft, round, with pike",       // 0x9E
                          "Enddoor, ready",                     // 0x9F
                          "Steel, round, with pike",            // 0xA0
                          "Steel, round",                       // 0xA1
                          "Steel, Warning",                     // 0xA2
                          "Steel, Biohazard",                   // 0xA3
                          "Steel, Deadend",                     // 0xA4
                          "Steel, Stop",                        // 0xA5
                          "Steel, Parking",                     // 0xA6
                          "Steel, Forbidden",                   // 0xA7
                          "Steel, Exit",                        // 0xA8
                          "Steel, Radioactive",                 // 0xA9
                          "Steel, Explosion",                   // 0xAA
                          "Steel, Acid",                        // 0xAB
                          "Steel, not round",                   // 0xAC
                          "Wall, soft, not round",              // 0xAD
                          "Steel, marker left/up",              // 0xAE
                          "Steel, marker up",                   // 0xAF
                          "Steel, marker right/up",             // 0xB0
                          "Steel, marker left",                 // 0xB1
                          "Steel, marker right",                // 0xB2
                          "Steel, marker left/bottom",          // 0xB3
                          "Steel, marker bottom",               // 0xB4
                          "Steel, marker right/bottom",         // 0xB5
                          "Steel, Heart",                       // 0xB6
                          "Steel, Player head",                 // 0xB7
                          "Steel, no entry",                    // 0xB8
                          "Steel, give way",                    // 0xB9
                          "Steel, ying yang",                   // 0xBA
                          "Steel, wheelchair",                  // 0xBB
                          "Steel, arrow down",                  // 0xBC
                          "Steel, arrow up",                    // 0xBD
                          "Steel, arrow left",                  // 0xBE
                          "Steel, arrow right",                 // 0xBF
                          "Steel, invisible",                   // 0xC0
                          "Wall, invisible",                    // 0xC1
                          "Wall, with crystal",                 // 0xC2
                          "Wall, with red key ",                // 0xC3
                          "Wall, with green key",               // 0xC4
                          "Wall, with blue key",                // 0xC5
                          "Wall, with yellow key",              // 0xC6
                          "Wall, with white key",               // 0xC7
                          "Wall, with general key",             // 0xC8
                          "Wall, with bomb",                    // 0xC9
                          "Wall, with megabomb",                // 0xCA
                          "Wall, with stone",                   // 0xCB
                          "Wall, with nut",                     // 0xCC
                          "Wall, with wheel",                   // 0xCD
                          "Wall, with dynamite",                // 0xCE
                          "Wall, with enddoor",                 // 0xCF
                          "Wall, with enddoor, ready",          // 0xD0
                          "Wall, with mine up",                 // 0xD1
                          "Wall, with mole",                    // 0xD2
                          "Wall, with green cheese",            // 0xD3
                          "Wall, with beetle up",               // 0xD4
                          "Wall, with yam",                     // 0xD5
                          "Wall, with alien",                   // 0xD6
                          "no element",                         // 0xD7
                          "no element",                         // 0xD8
                          "no element",                         // 0xD9
                          "no element",                         // 0xDA
                          "no element",                         // 0xDB
                          "no element",                         // 0xDC
                          "no element",                         // 0xDD
                          "no element",                         // 0xDE
                          "no element",                         // 0xDF
                          "no element",                         // 0xE0
                          "no element",                         // 0xE1
                          "no element",                         // 0xE2
                          "no element",                         // 0xE3
                          "no element",                         // 0xE4
                          "no element",                         // 0xE5
                          "no element",                         // 0xE6
                          "no element",                         // 0xE7
                          "no element",                         // 0xE8
                          "no element",                         // 0xE9
                          "no element",                         // 0xEA
                          "no element",                         // 0xEB
                          "no element",                         // 0xEC
                          "no element",                         // 0xED
                          "no element",                         // 0xEE
                          "no element",                         // 0xEF
                          "no element",                         // 0xF0
                          "no element",                         // 0xF1
                          "no element",                         // 0xF2
                          "no element",                         // 0xF3
                          "no element",                         // 0xF4
                          "no element",                         // 0xF5
                          "no element",                         // 0xF6
                          "no element",                         // 0xF7
                          "no element",                         // 0xF8
                          "no element",                         // 0xF9
                          "no element",                         // 0xFA
                          "no element",                         // 0xFB
                          "no element",                         // 0xFC
                          "no element",                         // 0xFD
                          "no element",                         // 0xFE
                          "no element",                         // 0xFF
                          "Font, exclamation",                  // 0x100
                          "Font, arrow right",                  // 0x101
                          "Font, arrow up",                     // 0x102
                          "Font, arrow down",                   // 0x103
                          "Font, apostrophe",                   // 0x104
                          "Font, brace open",                   // 0x105
                          "Font, brace close",                  // 0x106
                          "Font, copyright",                    // 0x107
                          "Font, plus",                         // 0x108
                          "Font, comma",                        // 0x109
                          "Font, minus",                        // 0x10A
                          "Font, point",                        // 0x10B
                          "Font, slash",                        // 0x10C
                          "Font, 0",                            // 0x10D
                          "Font, 1",                            // 0x10E
                          "Font, 2",                            // 0x10F
                          "Font, 3",                            // 0x110
                          "Font, 4",                            // 0x111
                          "Font, 5",                            // 0x112
                          "Font, 6",                            // 0x113
                          "Font, 7",                            // 0x114
                          "Font, 8",                            // 0x115
                          "Font, 9",                            // 0x116
                          "Font, double point",                 // 0x117
                          "Font, plate",                        // 0x118
                          "Font, arrow left",                   // 0x119
                          "Font, paff",                         // 0x11A
                          "Font, question mark",                // 0x11B
                          "Font, A",                            // 0x11C
                          "Font, B",                            // 0x11D
                          "Font, C",                            // 0x11E
                          "Font, D",                            // 0x11F
                          "Font, E",                            // 0x120
                          "Font, F",                            // 0x121
                          "Font, G",                            // 0x122
                          "Font, H",                            // 0x123
                          "Font, I",                            // 0x124
                          "Font, J",                            // 0x125
                          "Font, K",                            // 0x126
                          "Font, L",                            // 0x127
                          "Font, M",                            // 0x128
                          "Font, N",                            // 0x129
                          "Font, O",                            // 0x12A
                          "Font, P",                            // 0x12B
                          "Font, Q",                            // 0x12C
                          "Font, R",                            // 0x12D
                          "Font, S",                            // 0x12E
                          "Font, T",                            // 0x12F
                          "Font, U",                            // 0x130
                          "Font, V",                            // 0x131
                          "Font, W",                            // 0x132
                          "Font, X",                            // 0x133
                          "Font, Y",                            // 0x134
                          "Font, Z",                            // 0x135
                          "Font, AE",                           // 0x136
                          "Font, OE",                           // 0x137
                          "Font, UE",                           // 0x138
                         };

// Level-Elemente, die aktuell angezeigt werden
uint16_t g_PanelElements[MAX_PANEL_ELEMENTS] = {
                            EMERALD_INVALID
                            };

// Level-Elemente, die im Hauptmenü verfügbar sind
uint16_t g_PanelElementsMain[MAX_PANEL_ELEMENTS] = {
                            EMERALD_DOOR_RED,EMERALD_DOOR_YELLOW,EMERALD_DOOR_GREEN,EMERALD_DOOR_BLUE,EMERALD_KEY_RED,EMERALD_KEY_YELLOW,EMERALD_KEY_GREEN,EMERALD_KEY_BLUE,
                            EMERALD_DOOR_GREY_RED,EMERALD_DOOR_GREY_YELLOW,EMERALD_DOOR_GREY_GREEN,EMERALD_DOOR_GREY_BLUE,EMERALD_DOOR_RED_WOOD,EMERALD_DOOR_YELLOW_WOOD,EMERALD_DOOR_GREEN_WOOD,EMERALD_DOOR_BLUE_WOOD,
                            EMERALD_DOOR_WHITE,EMERALD_DOOR_GREY_WHITE,EMERALD_DOOR_WHITE_WOOD,EMERALD_KEY_WHITE,EMERALD_KEY_GENERAL,EMERALD_DOOR_MULTICOLOR,EMERALD_DOOR_EMERALD,EMERALD_DOOR_TIME,
                            EMERALD_DOOR_END_NOT_READY,EMERALD_DOOR_END_READY,EMERALD_DOOR_END_NOT_READY_STEEL,EMERALD_DOOR_END_READY_STEEL,EMERALD_SWITCHDOOR_CLOSED,EMERALD_SWITCHDOOR_OPEN,EMERALD_SWITCH_SWITCHDOOR,EMERALD_WHEEL_TIMEDOOR,
                            EMERALD_DOOR_ONLY_LEFT,EMERALD_DOOR_ONLY_DOWN,EMERALD_DOOR_ONLY_RIGHT,EMERALD_DOOR_ONLY_DOWN,EMERALD_MAGIC_WALL,EMERALD_MAGIC_WALL_STEEL,EMERALD_MAGIC_WALL_SWITCH,EMERALD_LIGHT_SWITCH,
                            EMERALD_MINE_LEFT,EMERALD_MINE_DOWN,EMERALD_MINE_RIGHT,EMERALD_MINE_UP,EMERALD_BEETLE_LEFT,EMERALD_BEETLE_DOWN,EMERALD_BEETLE_RIGHT,EMERALD_BEETLE_UP,
                            EMERALD_MOLE_LEFT,EMERALD_MOLE_DOWN,EMERALD_MOLE_RIGHT,EMERALD_MOLE_UP,EMERALD_YAM,EMERALD_ALIEN,EMERALD_GREEN_CHEESE,EMERALD_GREEN_DROP,
                            EMERALD_MAN,EMERALD_HAMMER,EMERALD_TIME_COIN,EMERALD_DYNAMITE_OFF,EMERALD_DYNAMITE_ON,EMERALD_WHEEL,EMERALD_SPACE,EMERALD_SPACE,
                            EMERALD_REPLICATOR_RED_TOP_LEFT,EMERALD_REPLICATOR_RED_TOP_MID,EMERALD_REPLICATOR_RED_TOP_RIGHT,EMERALD_REPLICATOR_RED_SWITCH,EMERALD_REPLICATOR_YELLOW_TOP_LEFT,EMERALD_REPLICATOR_YELLOW_TOP_MID,EMERALD_REPLICATOR_YELLOW_TOP_RIGHT,EMERALD_REPLICATOR_YELLOW_SWITCH,
                            EMERALD_REPLICATOR_RED_BOTTOM_LEFT,EMERALD_SPACE,EMERALD_REPLICATOR_RED_BOTTOM_RIGHT,EMERALD_SPACE,EMERALD_REPLICATOR_YELLOW_BOTTOM_LEFT,EMERALD_SPACE,EMERALD_REPLICATOR_YELLOW_BOTTOM_RIGHT,EMERALD_SPACE,
                            EMERALD_REPLICATOR_GREEN_TOP_LEFT,EMERALD_REPLICATOR_GREEN_TOP_MID,EMERALD_REPLICATOR_GREEN_TOP_RIGHT,EMERALD_REPLICATOR_GREEN_SWITCH,EMERALD_REPLICATOR_BLUE_TOP_LEFT,EMERALD_REPLICATOR_BLUE_TOP_MID,EMERALD_REPLICATOR_BLUE_TOP_RIGHT,EMERALD_REPLICATOR_BLUE_SWITCH,
                            EMERALD_REPLICATOR_GREEN_BOTTOM_LEFT,EMERALD_SPACE,EMERALD_REPLICATOR_GREEN_BOTTOM_RIGHT,EMERALD_SPACE,EMERALD_REPLICATOR_BLUE_BOTTOM_LEFT,EMERALD_SPACE,EMERALD_REPLICATOR_BLUE_BOTTOM_RIGHT,EMERALD_SPACE,
                            EMERALD_LIGHTBARRIER_RED_LEFT,EMERALD_LIGHTBARRIER_RED_DOWN,EMERALD_LIGHTBARRIER_RED_RIGHT,EMERALD_LIGHTBARRIER_RED_DOWN,EMERALD_LIGHTBARRIER_YELLOW_LEFT,EMERALD_LIGHTBARRIER_YELLOW_DOWN,EMERALD_LIGHTBARRIER_YELLOW_RIGHT,EMERALD_LIGHTBARRIER_YELLOW_DOWN,
                            EMERALD_LIGHTBARRIER_GREEN_LEFT,EMERALD_LIGHTBARRIER_GREEN_DOWN,EMERALD_LIGHTBARRIER_GREEN_RIGHT,EMERALD_LIGHTBARRIER_GREEN_DOWN,EMERALD_LIGHTBARRIER_BLUE_LEFT,EMERALD_LIGHTBARRIER_BLUE_DOWN,EMERALD_LIGHTBARRIER_BLUE_RIGHT,EMERALD_LIGHTBARRIER_BLUE_DOWN,
                            EMERALD_LIGHTBARRIER_RED_SWITCH,EMERALD_LIGHTBARRIER_YELLOW_SWITCH,EMERALD_LIGHTBARRIER_GREEN_SWITCH,EMERALD_LIGHTBARRIER_BLUE_SWITCH,EMERALD_SPACE,EMERALD_SAND,EMERALD_SWAMP,EMERALD_SWAMP_STONE,
                            EMERALD_ACIDPOOL_TOP_LEFT,EMERALD_ACIDPOOL_TOP_MID,EMERALD_ACIDPOOL_TOP_RIGHT,EMERALD_NUT,EMERALD_BOMB,EMERALD_MEGABOMB,EMERALD_STANDMINE,EMERALD_STONE,
                            EMERALD_ACIDPOOL_BOTTOM_LEFT,EMERALD_ACIDPOOL_BOTTOM_MID,EMERALD_ACIDPOOL_BOTTOM_RIGHT,EMERALD_EMERALD,EMERALD_RUBY,EMERALD_SAPPHIRE,EMERALD_PERL,EMERALD_CRYSTAL,
                            EMERALD_MESSAGE_1,EMERALD_MESSAGE_2,EMERALD_MESSAGE_3,EMERALD_MESSAGE_4,EMERALD_MESSAGE_5,EMERALD_MESSAGE_6,EMERALD_MESSAGE_7,EMERALD_MESSAGE_8,
                            EMERALD_STEEL,EMERALD_STEEL_ROUND,EMERALD_STEEL_ROUND_PIKE,EMERALD_WALL_CORNERED,EMERALD_WALL_ROUND,EMERALD_WALL_ROUND_PIKE,EMERALD_STEEL_INVISIBLE,EMERALD_WALL_INVISIBLE,
                            EMERALD_STEEL_MARKER_LEFT_UP,EMERALD_STEEL_MARKER_UP,EMERALD_STEEL_MARKER_RIGHT_UP,EMERALD_STEEL_NOT_ROUND,EMERALD_WALL_NOT_ROUND,EMERALD_STEEL_NO_ENTRY,EMERALD_STEEL_GIVE_WAY,EMERALD_STEEL_WARNING,
                            EMERALD_STEEL_MARKER_LEFT,EMERALD_SPACE,EMERALD_STEEL_MARKER_RIGHT,EMERALD_STEEL_DEADEND,EMERALD_STEEL_PARKING,EMERALD_STEEL_WHEELCHAIR,EMERALD_STEEL_STOP,EMERALD_STEEL_FORBIDDEN,
                            EMERALD_STEEL_MARKER_LEFT_BOTTOM,EMERALD_STEEL_MARKER_BOTTOM,EMERALD_STEEL_MARKER_RIGHT_BOTTOM,EMERALD_STEEL_ARROW_DOWN,EMERALD_STEEL_ARROW_UP,EMERALD_STEEL_ARROW_LEFT,EMERALD_STEEL_ARROW_RIGHT,EMERALD_STEEL_EXIT,
                            EMERALD_STEEL_BIOHAZARD,EMERALD_STEEL_RADIOACTIVE,EMERALD_STEEL_EXPLOSION,EMERALD_STEEL_ACID,EMERALD_STEEL_PLAYERHEAD,EMERALD_STEEL_HEART,EMERALD_STEEL_YING,EMERALD_SPACE,
                            EMERALD_WALL_WITH_EMERALD,EMERALD_WALL_WITH_RUBY,EMERALD_WALL_WITH_SAPPHIRE,EMERALD_WALL_WITH_PERL,EMERALD_WALL_WITH_CRYSTAL,EMERALD_WALL_WITH_STONE,EMERALD_WALL_WITH_NUT,EMERALD_WALL_WITH_WHEEL,
                            EMERALD_WALL_WITH_KEY_RED,EMERALD_WALL_WITH_KEY_GREEN,EMERALD_WALL_WITH_KEY_BLUE,EMERALD_WALL_WITH_KEY_YELLOW,EMERALD_WALL_WITH_KEY_WHITE,EMERALD_WALL_WITH_KEY_GENERAL,EMERALD_WALL_WITH_BOMB,EMERALD_WALL_WITH_MEGABOMB,
                            EMERALD_WALL_WITH_DYNAMITE,EMERALD_WALL_WITH_ENDDOOR,EMERALD_WALL_WITH_ENDDOOR_READY,EMERALD_WALL_WITH_MINE_UP,EMERALD_WALL_WITH_BEETLE_UP,EMERALD_WALL_WITH_YAM,EMERALD_WALL_WITH_ALIEN,EMERALD_WALL_WITH_MOLE,
                            EMERALD_WALL_WITH_GREEN_CHEESE,EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,
                            EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,EMERALD_INVALID
                            };


uint16_t g_PanelElementsMachines[MAX_PANEL_ELEMENTS] = {
                            EMERALD_EMERALD,EMERALD_RUBY,EMERALD_SAPPHIRE,EMERALD_PERL,EMERALD_CRYSTAL,EMERALD_STONE,EMERALD_NUT,EMERALD_NONE,
                            EMERALD_BOMB,EMERALD_MEGABOMB,EMERALD_STANDMINE,EMERALD_NONE,EMERALD_NONE,EMERALD_NONE,EMERALD_NONE,EMERALD_NONE,
                            EMERALD_MINE_DOWN,EMERALD_BEETLE_DOWN,EMERALD_YAM,EMERALD_ALIEN,EMERALD_MOLE_DOWN,EMERALD_GREEN_DROP,EMERALD_NONE,EMERALD_NONE,
                            EMERALD_DOOR_END_NOT_READY,EMERALD_DOOR_END_READY,EMERALD_TIME_COIN,EMERALD_DYNAMITE_OFF,EMERALD_HAMMER,EMERALD_NONE,EMERALD_NONE,EMERALD_NONE,
                            EMERALD_KEY_RED,EMERALD_KEY_YELLOW,EMERALD_KEY_GREEN,EMERALD_KEY_BLUE,EMERALD_KEY_WHITE,EMERALD_KEY_GENERAL,EMERALD_NONE,EMERALD_NONE,EMERALD_NONE,

                            EMERALD_INVALID
                            };




/*----------------------------------------------------------------------------
Name:           SetPanelElements
------------------------------------------------------------------------------
Beschreibung: Legt durch Befüllen des Array g_PanelElements[] fest, welche
              Levelelemente sichtbar und auswählbar sind.
Parameter
      Eingang: uMenuState, uint32_t, Menü-Status (siehe editor.h)
Rückgabewert:  -
Seiteneffekte: g_PanelElements[], g_PanelElementsMain[], g_PanelElementsMachines[]
------------------------------------------------------------------------------*/
void SetPanelElements(uint32_t uMenuState) {


    switch (uMenuState) {
        case (MENUSTATE_LEVEL):
            memcpy(g_PanelElements,g_PanelElementsMain,sizeof(uint16_t) * MAX_PANEL_ELEMENTS);
            break;
        case (MENUSTATE_YAMS):
            memcpy(g_PanelElements,g_PanelElementsMain,sizeof(uint16_t) * MAX_PANEL_ELEMENTS);
            break;
        case (MENUSTATE_MACHINES):
            memcpy(g_PanelElements,g_PanelElementsMachines,sizeof(uint16_t) * MAX_PANEL_ELEMENTS);
            break;
        case (MENUSTATE_TIME_AND_SCORES):
            memcpy(g_PanelElements,g_PanelElementsMain,sizeof(uint16_t) * MAX_PANEL_ELEMENTS);
            break;
        default:
            memcpy(g_PanelElements,g_PanelElementsMain,sizeof(uint16_t) * MAX_PANEL_ELEMENTS);
            break;
    }
}


/*----------------------------------------------------------------------------
Name:           ClearOldMan
------------------------------------------------------------------------------
Beschreibung: Beim Setzen eines neuen Mannes, muss der Alte durch Space ersetzt werden.

Parameter
      Eingang: -
Rückgabewert:  -
Seiteneffekte: Ed.x
------------------------------------------------------------------------------*/
void ClearOldMan(void) {
    uint32_t I;

    for (I = 0; I < Ed.uLevel_X_Dimension * Ed.uLevel_Y_Dimension; I++) {
        if (Ed.pLevel[I] == EMERALD_MAN) {
            Ed.pLevel[I] = EMERALD_SPACE;
        }
    }
}


/*----------------------------------------------------------------------------
Name:           FillLevelArea
------------------------------------------------------------------------------
Beschreibung: Füllt einen (begrenzten) Levelbereich mit einem bestimmten Element.
              Die Routine stammt von HN2 (Helge Nickel) einem ehemaligen Kommilitonen
              aus dem Jahr 1995.
Parameter
      Eingang: x, int, X-Position im Level
               y, int, Y-Position im Level
               uFillElement, uint16_t, Element, mit dem gefüllt werden soll
               uGroundElement, uint16_t, Grund-Element, dass ersetzt werden soll
Rückgabewert:  -
Seiteneffekte: Ed.x
------------------------------------------------------------------------------*/
void FillLevelArea(int x,int y,uint16_t uFillElement,uint16_t uGroundElement)
{
    if (uFillElement == uGroundElement) {
        return;
    }
    Ed.pLevel[Ed.uLevel_X_Dimension * y + x] = uFillElement;
    if ((Ed.pLevel[Ed.uLevel_X_Dimension * y + x + 1] == uGroundElement) && (( x + 1) < (Ed.uLevel_X_Dimension - 1))) {
        //SDL_Log("rechts");
        FillLevelArea(x + 1, y ,uFillElement,uGroundElement);  // rechts
    }
    if ((Ed.pLevel[Ed.uLevel_X_Dimension * y + x - 1] == uGroundElement) && (( x - 1) > 0)) {
        //SDL_Log("links");
        FillLevelArea(x - 1, y ,uFillElement,uGroundElement);  // rechts
    }
    if ((Ed.pLevel[Ed.uLevel_X_Dimension * (y - 1) + x] == uGroundElement) && (( y - 1) > 0)) {
        //SDL_Log("oben");
        FillLevelArea(x, y - 1 ,uFillElement,uGroundElement);  // oben
    }
    if ((Ed.pLevel[Ed.uLevel_X_Dimension * (y + 1) + x] == uGroundElement) && (( y + 1) < Ed.uLevel_Y_Dimension - 1)) {
        //SDL_Log("unten");
        FillLevelArea(x, y + 1 ,uFillElement,uGroundElement);  // unten
    }
}


/*----------------------------------------------------------------------------
Name:           WriteLevelXmlFile
------------------------------------------------------------------------------
Beschreibung: Schreibt eine Level-XML-Datei. Die Leveldaten werden der Struktur
              Ed.x entnommen.

Parameter
      Eingang: szFilename, char *, Zeiger auf Dateiname
      Ausgang: -
Rückgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte: Ed.x, ge_DisplayMode.x
------------------------------------------------------------------------------*/
int WriteLevelXmlFile(const char *szFilename) {
    int nErrorCode;
    char szXML[64 * 1024];     // Ohne Level-Daten, reine Kopfdaten ca. 2 KB, ca. 300 Bytes / pro YAM-Explosion
    char szNum[16];
    uint32_t uBase64Len;
    uint32_t uBase64MessageLen;
    uint32_t I;
    uint32_t E;
    uint8_t *pcBaseMessage64 = NULL;
    uint8_t *pcBase64 = NULL;
    uint8_t *pcComprossedLevel = NULL;
    uint32_t uMaxCompressedSize;
    MD5Context MD5Leveldata;
    char szM[4];    // 'm1' bis 'm8'
    char szTag[32];

    szM[0] = 'm';
    szM[1] = 'x';
    szM[2] = 0;
    nErrorCode = -1;
    memset(szXML,0,sizeof(szXML));
    strcat(szXML,"<?xml version=\"1.0\"?>\n");
    strcat(szXML,"<level>\n");
    strcat(szXML,"  <title>");strcat(szXML,Ed.szLevelTitle);strcat(szXML,"</title>\n");
    strcat(szXML,"  <author>");strcat(szXML,Ed.szLevelAuthor);strcat(szXML,"</author>\n");
    strcat(szXML,"  <version>");strcat(szXML,Ed.szVersion);strcat(szXML,"</version>\n");
    strcat(szXML,"  <values>\n");
    sprintf(szNum,"%u",Ed.uEmeraldsToCollect);
    strcat(szXML,"    <emeralds_to_collect>");strcat(szXML,szNum);strcat(szXML,"</emeralds_to_collect>\n");
    sprintf(szNum,"%u",Ed.uTimeScoreFactor);
    strcat(szXML,"    <score_time_factor>");strcat(szXML,szNum);strcat(szXML,"</score_time_factor>\n");
    sprintf(szNum,"%u",Ed.uCheeseSpreadSpeed);
    strcat(szXML,"    <speed_cheese_spread>");strcat(szXML,szNum);strcat(szXML,"</speed_cheese_spread>\n");
    strcat(szXML,"  </values>\n");
    strcat(szXML,"  <leveldimension>\n");
    sprintf(szNum,"%u",Ed.uLevel_X_Dimension);
    strcat(szXML,"    <x>");strcat(szXML,szNum);strcat(szXML,"</x>\n");
    sprintf(szNum,"%u",Ed.uLevel_Y_Dimension);
    strcat(szXML,"    <y>");strcat(szXML,szNum);strcat(szXML,"</y>\n");
    strcat(szXML,"  </leveldimension>\n");
    strcat(szXML,"  <scores>\n");
    sprintf(szNum,"%u",Ed.uScoreEmerald);
    strcat(szXML,"    <emerald>");strcat(szXML,szNum);strcat(szXML,"</emerald>\n");
    sprintf(szNum,"%u",Ed.uScoreRuby);
    strcat(szXML,"    <ruby>");strcat(szXML,szNum);strcat(szXML,"</ruby>\n");
    sprintf(szNum,"%u",Ed.uScoreSaphir);
    strcat(szXML,"    <sapphire>");strcat(szXML,szNum);strcat(szXML,"</sapphire>\n");
    sprintf(szNum,"%u",Ed.uScorePerl);
    strcat(szXML,"    <perl>");strcat(szXML,szNum);strcat(szXML,"</perl>\n");
    sprintf(szNum,"%u",Ed.uScoreCrystal);
    strcat(szXML,"    <crystal>");strcat(szXML,szNum);strcat(szXML,"</crystal>\n");
    sprintf(szNum,"%u",Ed.uScoreMessage);
    strcat(szXML,"    <letter>");strcat(szXML,szNum);strcat(szXML,"</letter>\n");
    sprintf(szNum,"%u",Ed.uScoreKey);
    strcat(szXML,"    <key>");strcat(szXML,szNum);strcat(szXML,"</key>\n");
    sprintf(szNum,"%u",Ed.uScoreDynamite);
    strcat(szXML,"    <dynamite>");strcat(szXML,szNum);strcat(szXML,"</dynamite>\n");
    sprintf(szNum,"%u",Ed.uScoreHammer);
    strcat(szXML,"    <hammer>");strcat(szXML,szNum);strcat(szXML,"</hammer>\n");
    sprintf(szNum,"%u",Ed.uScoreNutCracking);
    strcat(szXML,"    <nutcracking>");strcat(szXML,szNum);strcat(szXML,"</nutcracking>\n");
    sprintf(szNum,"%u",Ed.uScoreStoningBeetle);
    strcat(szXML,"    <stoning_beetle>");strcat(szXML,szNum);strcat(szXML,"</stoning_beetle>\n");
    sprintf(szNum,"%u",Ed.uScoreStoningMine);
    strcat(szXML,"    <stoning_mine>");strcat(szXML,szNum);strcat(szXML,"</stoning_mine>\n");
    sprintf(szNum,"%u",Ed.uScoreStoningAlien);
    strcat(szXML,"    <stoning_alien>");strcat(szXML,szNum);strcat(szXML,"</stoning_alien>\n");
    sprintf(szNum,"%u",Ed.uScoreStoningYam);
    strcat(szXML,"    <stoning_yam>");strcat(szXML,szNum);strcat(szXML,"</stoning_yam>\n");
    sprintf(szNum,"%u",Ed.uScoreTimeCoin);
    strcat(szXML,"    <timecoin>");strcat(szXML,szNum);strcat(szXML,"</timecoin>\n");
    strcat(szXML,"  </scores>\n");
    strcat(szXML,"  <times>\n");
    sprintf(szNum,"%u",Ed.uTimeToPlay / ge_DisplayMode.refresh_rate);
    strcat(szXML,"    <to_play>");strcat(szXML,szNum);strcat(szXML,"</to_play>\n");
    sprintf(szNum,"%u",Ed.uTimeWheelRotation / ge_DisplayMode.refresh_rate);
    strcat(szXML,"    <wheel_rotation>");strcat(szXML,szNum);strcat(szXML,"</wheel_rotation>\n");
    sprintf(szNum,"%u",Ed.uTimeMagicWall / ge_DisplayMode.refresh_rate);
    strcat(szXML,"    <magic_wall>");strcat(szXML,szNum);strcat(szXML,"</magic_wall>\n");
    sprintf(szNum,"%u",Ed.uTimeLight / ge_DisplayMode.refresh_rate);
    strcat(szXML,"    <light>");strcat(szXML,szNum);strcat(szXML,"</light>\n");
    sprintf(szNum,"%u",Ed.uTimeDoorTime / ge_DisplayMode.refresh_rate);
    strcat(szXML,"    <timedoor>");strcat(szXML,szNum);strcat(szXML,"</timedoor>\n");
    sprintf(szNum,"%u",Ed.uAdditonalTimeCoinTime / ge_DisplayMode.refresh_rate);
    strcat(szXML,"    <timecoin>");strcat(szXML,szNum);strcat(szXML,"</timecoin>\n");
    strcat(szXML,"  </times>\n");
    strcat(szXML,"  <inventory>\n");
    sprintf(szNum,"%u",Ed.uDynamiteCount);
    strcat(szXML,"    <dynamite>");strcat(szXML,szNum);strcat(szXML,"</dynamite>\n");
    sprintf(szNum,"%u",Ed.uHammerCount);
    strcat(szXML,"    <hammer>");strcat(szXML,szNum);strcat(szXML,"</hammer>\n");
    sprintf(szNum,"%u",Ed.uWhiteKeyCount);
    strcat(szXML,"    <white_key>");strcat(szXML,szNum);strcat(szXML,"</white_key>\n");
    strcat(szXML,"  </inventory>\n");
    strcat(szXML,"  <replicators>\n");
    strcat(szXML,"    <red>\n");
    if (Ed.bReplicatorRedOn) {
        strcpy(szNum,"1");
    } else {
        strcpy(szNum,"0");
    }
    strcat(szXML,"      <active>");strcat(szXML,szNum);strcat(szXML,"</active>\n");
    sprintf(szNum,"%u",Ed.uReplicatorRedObject);
    strcat(szXML,"      <element>");strcat(szXML,szNum);strcat(szXML,"</element>\n");
    strcat(szXML,"    </red>\n");
    strcat(szXML,"    <green>\n");
    if (Ed.bReplicatorGreenOn) {
        strcpy(szNum,"1");
    } else {
        strcpy(szNum,"0");
    }
    strcat(szXML,"      <active>");strcat(szXML,szNum);strcat(szXML,"</active>\n");
    sprintf(szNum,"%u",Ed.uReplicatorGreenObject);
    strcat(szXML,"      <element>");strcat(szXML,szNum);strcat(szXML,"</element>\n");
    strcat(szXML,"    </green>\n");
    strcat(szXML,"    <blue>\n");
    if (Ed.bReplicatorBlueOn) {
        strcpy(szNum,"1");
    } else {
        strcpy(szNum,"0");
    }
    strcat(szXML,"      <active>");strcat(szXML,szNum);strcat(szXML,"</active>\n");
    sprintf(szNum,"%u",Ed.uReplicatorBlueObject);
    strcat(szXML,"      <element>");strcat(szXML,szNum);strcat(szXML,"</element>\n");
    strcat(szXML,"    </blue>\n");
    strcat(szXML,"    <yellow>\n");
    if (Ed.bReplicatorYellowOn) {
        strcpy(szNum,"1");
    } else {
        strcpy(szNum,"0");
    }
    strcat(szXML,"      <active>");strcat(szXML,szNum);strcat(szXML,"</active>\n");
    sprintf(szNum,"%u",Ed.uReplicatorYellowObject);
    strcat(szXML,"      <element>");strcat(szXML,szNum);strcat(szXML,"</element>\n");
    strcat(szXML,"    </yellow>\n");
    strcat(szXML,"  </replicators>\n");
    strcat(szXML,"  <lightbarriers>\n");
    strcat(szXML,"    <red>\n");
    if (Ed.bLightBarrierRedOn) {
        strcpy(szNum,"1");
    } else {
        strcpy(szNum,"0");
    }
    strcat(szXML,"      <active>");strcat(szXML,szNum);strcat(szXML,"</active>\n");
    strcat(szXML,"    </red>\n");
    strcat(szXML,"    <green>\n");
    if (Ed.bLightBarrierGreenOn) {
        strcpy(szNum,"1");
    } else {
        strcpy(szNum,"0");
    }
    strcat(szXML,"      <active>");strcat(szXML,szNum);strcat(szXML,"</active>\n");
    strcat(szXML,"    </green>\n");
    strcat(szXML,"    <blue>\n");
    if (Ed.bLightBarrierBlueOn) {
        strcpy(szNum,"1");
    } else {
        strcpy(szNum,"0");
    }
    strcat(szXML,"      <active>");strcat(szXML,szNum);strcat(szXML,"</active>\n");
    strcat(szXML,"    </blue>\n");
    strcat(szXML,"    <yellow>\n");
    if (Ed.bLightBarrierYellowOn) {
        strcpy(szNum,"1");
    } else {
        strcpy(szNum,"0");
    }
    strcat(szXML,"      <active>");strcat(szXML,szNum);strcat(szXML,"</active>\n");
    strcat(szXML,"    </yellow>\n");
    strcat(szXML,"  </lightbarriers>\n");
    strcat(szXML,"  <messages>\n");
    for (I = 1; (I <= EMERALD_MAX_MESSAGES) && (nErrorCode == -1); I++) {
        szM[1] = I + 0x30;
        sprintf(szTag,"    <%s>",szM);
        strcat(szXML,szTag);                 // Öffnungstag schreiben
        if (Ed.pMessage[I - 1] != NULL) {
            pcBaseMessage64 = BinToBase64((uint8_t*)Ed.pMessage[I - 1], strlen(Ed.pMessage[I - 1]),&uBase64MessageLen,false);
            if (pcBaseMessage64 != NULL) {
                strcat(szXML,(char*)pcBaseMessage64);
                SAFE_FREE(pcBaseMessage64);
                sprintf(szTag,"</%s>\n",szM);
                strcat(szXML,szTag);                 // Schließungstag schreiben
            } else {
                SDL_Log("%s: BinToBase64() for message %s failed",__FUNCTION__,szTag);
                return -1;
            }
        } else {
            sprintf(szTag,"</%s>\n",szM);
            strcat(szXML,szTag);                 // Schließungstag schreiben
        }
    }
    strcat(szXML,"  </messages>\n");
    sprintf(szNum,"%u",Ed.uMaxYamExplosionIndex + 1);
    strcat(szXML,"  <max_yam_explosions>");strcat(szXML,szNum);strcat(szXML,"</max_yam_explosions>\n");
    strcat(szXML,"  <yam_explosions>\n");
    for (I = 0; (I <= Ed.uMaxYamExplosionIndex); I++) {
        sprintf(szTag,"    <explosion%02u>\n",I + 1);
        strcat(szXML,szTag);
        for (E = 0; E < 9; E++) {
            sprintf(szTag,"      <element%02u>",E + 1);
            strcat(szXML,szTag);
            sprintf(szNum,"%d",Ed.YamExplosions[I].uElement[E]);
            strcat(szXML,szNum);
            sprintf(szTag,"</element%02u>\n",E + 1);
            strcat(szXML,szTag);
        }
        sprintf(szTag,"    </explosion%02u>\n",I + 1);
        strcat(szXML,szTag);
    }
    strcat(szXML,"  </yam_explosions>\n");
    strcat(szXML,"  <leveldata>");
    uMaxCompressedSize = Ed.uLevel_X_Dimension * Ed.uLevel_Y_Dimension * sizeof(uint16_t) * 2 + 16 * KB;
    pcComprossedLevel = malloc(uMaxCompressedSize);
    if (pcComprossedLevel != NULL) {
        // uMaxCompressedSize ist hier noch Eingabeparameter, damit compress() weiß, wieviel Puffer zum Komprimieren bereit steht
        if (compress(pcComprossedLevel,(uLongf*)&uMaxCompressedSize,(uint8_t*)Ed.pLevel,Ed.uLevel_X_Dimension * Ed.uLevel_Y_Dimension * sizeof(uint16_t)) == Z_OK) {
            SDL_Log("%s:c compressed level size: %u",__FUNCTION__,uMaxCompressedSize);
            if (WriteFile(szFilename,(uint8_t*)szXML,strlen(szXML),false) == 0) {
                pcBase64 = BinToBase64((uint8_t *)pcComprossedLevel,uMaxCompressedSize,&uBase64Len,false);
                if (pcBase64 != NULL) {
                    SDL_Log("base64Len = %u",uBase64Len);
                    if (WriteFile(szFilename,pcBase64,uBase64Len,true) == 0) {
                        strcpy(szXML,"</leveldata>\n");
                        strcat(szXML,"  <leveldata_md5_hash>");
                        md5Init(&MD5Leveldata);
                        md5Update(&MD5Leveldata,pcBase64,uBase64Len);
                        md5Finalize(&MD5Leveldata);
                        for (I = 0; I < 16; I++) {
                            sprintf(szNum,"%02X",MD5Leveldata.digest[I]);
                            strcat(szXML,szNum);
                        }
                        strcat(szXML,"</leveldata_md5_hash>\n");
                        strcat(szXML,"</level>");
                        if (WriteFile(szFilename,(uint8_t*)szXML,strlen(szXML),true) == 0) {
                            SDL_Log("Write file OK");
                            nErrorCode = 0;
                        } else {
                            SDL_Log("%s: Write file ERROR, xml data (part 2)",__FUNCTION__);
                        }
                    } else {
                        SDL_Log("%s: Write file ERROR, base64 data",__FUNCTION__);
                    }
                } else {
                    SDL_Log("%s: BinToBase64 failed",__FUNCTION__);
                }
            } else {
                SDL_Log("%s: Write file ERROR, xml data (part 1)",__FUNCTION__);
            }
        } else {
            SDL_Log("%s: compress() failed",__FUNCTION__);
        }
    } else {
        SDL_Log("%s: malloc() failed (compress memory)",__FUNCTION__);
    }
    SAFE_FREE(pcComprossedLevel);
    SAFE_FREE(pcBase64);
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           ShowEditorStatus
------------------------------------------------------------------------------
Beschreibung: Zeit Die Werte der Struktur Ed.x an.

Parameter
      Eingang: -
               -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Ed.x
------------------------------------------------------------------------------*/
void ShowEditorStatus(void) {

    printf("\r\n\r\nEDITOR-VALUES\r\n");
    printf("=================================\r\n");
    printf("Half size active:       %d\r\n",Ed.bHalfSize);
    printf("Level-X-Dimension:      %d\r\n",Ed.uLevel_X_Dimension);
    printf("Level-Y-Dimension:      %d\r\n",Ed.uLevel_Y_Dimension);
    printf("uVisibleX:              %d\r\n",Ed.uVisibleX);
    printf("uVisibleY:              %d\r\n",Ed.uVisibleY);
    printf("uVisibleCenterX:        %d\r\n",Ed.uVisibleCenterX);
    printf("uVisibleCenterY:        %d\r\n",Ed.uVisibleCenterY);
    printf("uShiftLevelXpix:        %d\r\n",Ed.uShiftLevelXpix);
    printf("uShiftLevelYpix:        %d\r\n",Ed.uShiftLevelYpix);
    printf("uUpperLeftLevelIndex:   %d\r\n",Ed.uUpperLeftLevelIndex);
    printf("nMinXLevel:             %d\r\n",Ed.nMinXLevel);
    printf("nManXLevel:             %d\r\n",Ed.nMaxXLevel);
    printf("nMinYLevel:             %d\r\n",Ed.nMinYLevel);
    printf("nManYLevel:             %d\r\n",Ed.nMaxYLevel);
}


/*----------------------------------------------------------------------------
Name:           CopyPlayfieldValueToEditor
------------------------------------------------------------------------------
Beschreibung: Kopiert die Werte der Playfield.x-Struktur in die Ed.x-Struktur-

Parameter
      Eingang: -
               -
      Ausgang: -
Rückgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte: Playfield.x, Ed.x
------------------------------------------------------------------------------*/
int CopyPlayfieldValueToEditor(void) {
    uint32_t I;

    if ((Playfield.bInitOK) && (Playfield.pLevel != NULL) &&
        (Playfield.uLevel_X_Dimension >= MIN_LEVEL_W) && (Playfield.uLevel_X_Dimension <= MAX_LEVEL_W) &&
        (Playfield.uLevel_Y_Dimension >= MIN_LEVEL_H) && (Playfield.uLevel_Y_Dimension <= MAX_LEVEL_H)) {
        Ed.pLevel = Playfield.pLevel;
        Ed.uLevel_X_Dimension = Playfield.uLevel_X_Dimension;
        Ed.uLevel_Y_Dimension = Playfield.uLevel_Y_Dimension;
        Ed.bLightBarrierRedOn = Playfield.bLightBarrierRedOn;
        Ed.bLightBarrierGreenOn = Playfield.bLightBarrierGreenOn;
        Ed.bLightBarrierBlueOn = Playfield.bLightBarrierBlueOn;
        Ed.bLightBarrierYellowOn = Playfield.bLightBarrierYellowOn;
        Ed.bReplicatorRedOn = Playfield.bReplicatorRedOn;
        Ed.uReplicatorRedObject = Playfield.uReplicatorRedObject;
        Ed.bReplicatorGreenOn = Playfield.bReplicatorGreenOn;
        Ed.uReplicatorGreenObject = Playfield.uReplicatorGreenObject;
        Ed.bReplicatorBlueOn = Playfield.bReplicatorBlueOn;
        Ed.uReplicatorBlueObject = Playfield.uReplicatorBlueObject;
        Ed.bReplicatorYellowOn = Playfield.bReplicatorYellowOn;
        Ed.uReplicatorYellowObject = Playfield.uReplicatorYellowObject;
        strcpy(Ed.szLevelTitle,Playfield.szLevelTitle);
        strcpy(Ed.szLevelAuthor,Playfield.szLevelAuthor);
        strcpy(Ed.szVersion,Playfield.szVersion);
        for (I = 0; I <EMERALD_MAX_MESSAGES; I++) {
            Ed.pMessage[I] = Playfield.pMessage[I];
        }
        Ed.uScoreEmerald = Playfield.uScoreEmerald;
        Ed.uScoreRuby = Playfield.uScoreRuby;
        Ed.uScoreSaphir = Playfield.uScoreSaphir;
        Ed.uScorePerl = Playfield.uScorePerl;
        Ed.uScoreCrystal = Playfield.uScoreCrystal;
        Ed.uScoreKey = Playfield.uScoreKey;
        Ed.uScoreDynamite = Playfield.uScoreDynamite;
        Ed.uScoreHammer = Playfield.uScoreHammer;
        Ed.uScoreNutCracking = Playfield.uScoreNutCracking;
        Ed.uScoreStoningBeetle = Playfield.uScoreStoningBeetle;
        Ed.uScoreStoningMine = Playfield.uScoreStoningMine;
        Ed.uScoreStoningAlien = Playfield.uScoreStoningAlien;
        Ed.uScoreStoningYam = Playfield.uScoreStoningYam;
        Ed.uScoreTimeCoin = Playfield.uScoreTimeCoin;
        Ed.uScoreMessage = Playfield.uScoreMessage;
        Ed.uEmeraldsToCollect = Playfield.uEmeraldsToCollect;
        Ed.uTimeScoreFactor = Playfield.uTimeScoreFactor;
        Ed.uCheeseSpreadSpeed = Playfield.uCheeseSpreadSpeed;
        Ed.uTimeToPlay = Playfield.uTimeToPlay;
        Ed.uAdditonalTimeCoinTime = Playfield.uAdditonalTimeCoinTime;
        Ed.uTimeWheelRotation = Playfield.uTimeWheelRotation;
        Ed.uTimeDoorTime = Playfield.uTimeDoorTime;
        Ed.uTimeMagicWall = Playfield.uTimeMagicWall;
        Ed.uTimeLight = Playfield.uTimeLight;
        Ed.uDynamiteCount = Playfield.uDynamiteCount;
        Ed.uHammerCount = Playfield.uHammerCount;
        Ed.uWhiteKeyCount = Playfield.uWhiteKeyCount;
        Ed.uMaxYamExplosionIndex = Playfield.uMaxYamExplosionIndex;
        memcpy(Ed.YamExplosions,Playfield.YamExplosions,sizeof(Playfield.YamExplosions));
        return 0;
    } else {
        SDL_Log("%s: bad values (bInitOK: %d, pLevel: %p, x: %u    y: %u) in playfield structure found",
                __FUNCTION__,Playfield.bInitOK,Playfield.pLevel,Playfield.uLevel_X_Dimension,Playfield.uLevel_Y_Dimension);
        return -1;
    }
}


/*----------------------------------------------------------------------------
Name:           InitEditor
------------------------------------------------------------------------------
Beschreibung: Initialisiert die Struktur Ed.x für den Editor.

Parameter
      Eingang: bNewLevel, bool, TRUE = Ein neues Level wird erzeugt mit den Abmaßen uXdim und uYdim.
               uXdim, X-Dimension für neues Level, wird nur verwendet, wenn bNewLevel = true
               uYdim, Y-Dimension für neues Level, wird nur verwendet, wenn bNewLevel = true
               pszFilename, char*, Zeiger auf Level-Dateinamen, wird nur verwendet, wenn bNewLevel = false
      Ausgang: -
Rückgabewert:  0 = alles OK, sonst Fehler
Seiteneffekte: Ed.x
------------------------------------------------------------------------------*/
int InitEditor(bool bNewLevel, uint32_t uXdim, uint32_t uYdim, char *pszFilename) {
    int nErrorCode;

    nErrorCode = -1;
    memset(&Ed,0,sizeof(Ed));
    InitYamExplosions(Ed.YamExplosions);
    Ed.uMenuState = MENUSTATE_LEVEL;
    SetPanelElements(MENUSTATE_LEVEL);
    Ed.uMaxYamExplosionIndex = 0;
    InitButtons();
    if ((bNewLevel) && (uXdim >= MIN_LEVEL_W) && (uXdim <= MAX_LEVEL_W) && (uYdim >= MIN_LEVEL_H) && (uYdim <= MAX_LEVEL_H)) {
        Ed.uLevel_X_Dimension = uXdim;
        Ed.uLevel_Y_Dimension = uYdim;
        Ed.pLevel = (uint16_t*)malloc(Ed.uLevel_X_Dimension * Ed.uLevel_Y_Dimension * sizeof(uint16_t));
        if (Ed.pLevel != NULL) {
            SetLevelBorder();
            nErrorCode = 0;
        } else {
            SDL_Log("%s: malloc() failed for new level (x: %u  y: %u)",__FUNCTION__,uXdim,uYdim);
            return -1;
        }
    } else {
        if (InitialisePlayfield(pszFilename) == 0) {
            if (CopyPlayfieldValueToEditor() == 0) {
                nErrorCode = 0;
            }
        } else {
            SDL_Log("%s: InitialisePlayfield() failed",__FUNCTION__);
            return -1;
        }
    }
    if (nErrorCode == 0) {
        Ed.uFrameCounter = 0;
        Ed.bHalfSize = true;
        Ed.uSelectedElementLeft[MENUSTATE_LEVEL] = EMERALD_SAND;
        Ed.uSelectedElementMiddle[MENUSTATE_LEVEL] = EMERALD_STONE;
        Ed.uSelectedElementRight[MENUSTATE_LEVEL] = EMERALD_STEEL;
        Ed.uSelectedElementLeft[MENUSTATE_YAMS] = EMERALD_STONE;
        Ed.uSelectedElementMiddle[MENUSTATE_YAMS] = EMERALD_EMERALD;
        Ed.uSelectedElementRight[MENUSTATE_YAMS] = EMERALD_SAPPHIRE;
        Ed.uSelectedElementLeft[MENUSTATE_MACHINES] = EMERALD_STONE;
        Ed.uSelectedElementMiddle[MENUSTATE_MACHINES] = EMERALD_EMERALD;
        Ed.uSelectedElementRight[MENUSTATE_MACHINES] = EMERALD_SAPPHIRE;
        Ed.uSelectedElementLeft[MENUSTATE_TIME_AND_SCORES] = EMERALD_STONE;
        Ed.uSelectedElementMiddle[MENUSTATE_TIME_AND_SCORES] = EMERALD_STONE;
        Ed.uSelectedElementRight[MENUSTATE_TIME_AND_SCORES] = EMERALD_STONE;
        // Sichtbaren Bereich berechnen
        if (Ed.bHalfSize) {
            Ed.uFont_W = FONT_W / 2;
            Ed.uFont_H = FONT_H / 2;
            Ed.uScrollPixelX = 4;              // Scrollt bei Tastendruck x Pixel horizontal
            Ed.uScrollPixelFastX = 8;          // Scrollt bei Tastendruck x Pixel horizontal in hoher Geschwindigkeit
            Ed.uScrollPixelY = 4;              // Scrollt bei Tastendruck x Pixel vertikal
            Ed.uScrollPixelFastY = 8;          // Scrollt bei Tastendruck x Pixel vertikal in hoher Geschwindigkeit
        } else {
            Ed.uFont_W = FONT_W;
            Ed.uFont_H = FONT_H;
            Ed.uScrollPixelX = 8;              // Scrollt bei Tastendruck x Pixel horizontal
            Ed.uScrollPixelFastX = 16;          // Scrollt bei Tastendruck x Pixel horizontal in hoher Geschwindigkeit
            Ed.uScrollPixelY = 8;              // Scrollt bei Tastendruck x Pixel vertikal
            Ed.uScrollPixelFastY = 16;          // Scrollt bei Tastendruck x Pixel vertikal in hoher Geschwindigkeit
        }
        Ed.uVisibleX = (WINDOW_W - EDITORPANEL_W) / Ed.uFont_W;
        Ed.uVisibleY = WINDOW_H / Ed.uFont_H;

        Ed.uVisibleCenterY = Ed.uVisibleY / 2;
        Ed.uVisibleCenterX = Ed.uVisibleX / 2;

        // Y-Zentrierung des Levels im Anzeigebereich
        if (Ed.uVisibleY >= Ed.uLevel_Y_Dimension) {    // Level ist in Y-Richtung kleiner bzw. gleich der Anzeigehöhe (in Y-Richtung wird nicht gescrollt)
            Ed.uShiftLevelYpix = ((Ed.uVisibleY - Ed.uLevel_Y_Dimension) / 2) * Ed.uFont_W;    // Level im Anzeigebereich zentrieren
        } else {
            // Level ist in Y-Richtung größer als Anzeigebereich, daher oben anfangen (es muss ggf. in Y-Richtung gescrollt werden)
            Ed.uShiftLevelYpix = 0;
        }
        // X-Zentrierung des Levels im Anzeigebereich
        if (Ed.uVisibleX >= Ed.uLevel_X_Dimension) {    // Level ist in X-Richtung kleiner bzw. gleich der Anzeigebreite (in X-Richtung wird nicht gescrollt)
            Ed.uShiftLevelXpix = ((Ed.uVisibleX - Ed.uLevel_X_Dimension) / 2) * Ed.uFont_H;    // Level im Anzeigebereich zentrieren
        } else {
            // Level ist in X-Richtung größer als Anzeigebereich, daher oben anfangen (es muss ggf. in Y-Richtung gescrollt werden)
            Ed.uShiftLevelXpix = 0;
        }
        // Positionsüberläufe abfangen
        Ed.nMaxXpos = (Ed.uLevel_X_Dimension * Ed.uFont_W) - WINDOW_W + EDITORPANEL_W; // + Ed.uFont_W;
        if (Ed.nMaxXpos < 0) {
            Ed.nMaxXpos = 0;
        }
        Ed.nMaxYpos = (Ed.uLevel_Y_Dimension * Ed.uFont_H) - WINDOW_H + PANEL_H;
        if (Ed.nMaxYpos < 0) {
            Ed.nMaxYpos = 0;
        }
        Ed.nMinXLevel = Ed.uShiftLevelXpix;                                         // Minimale X-Position innerhalb des Levela
        Ed.nMaxXLevel = Ed.nMinXLevel + Ed.uVisibleX * Ed.uFont_W;                  // Maximale X-Position innerhalb des Levela
        Ed.nMinYLevel = Ed.uShiftLevelYpix;                                         // Minimale Y-Position innerhalb des Levela
        Ed.nMaxYLevel = Ed.nMinYLevel + Ed.uLevel_Y_Dimension * Ed.uFont_W;         // Maximale Y-Position innerhalb des Levela
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           SetLevelBorder
------------------------------------------------------------------------------
Beschreibung: Setzt die Level-Umrandung mit EMERALD_STEEL.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Ed.x
------------------------------------------------------------------------------*/
void SetLevelBorder(void) {
    uint32_t I;

    for (I = 0; I < Ed.uLevel_X_Dimension * Ed.uLevel_Y_Dimension; I++) {
        Ed.pLevel[I] = EMERALD_SPACE;
    }
    // Obere Zeile setzen
    for (I = 0; (I < Ed.uLevel_X_Dimension); I++) {
        Ed.pLevel[I] = EMERALD_STEEL;
    }
    // Untere Zeile setzen
    for (I = Ed.uLevel_X_Dimension * (Ed.uLevel_Y_Dimension - 1); (I < Ed.uLevel_X_Dimension * Ed.uLevel_Y_Dimension); I++) {
        Ed.pLevel[I] = EMERALD_STEEL;
    }
    // Linke Spalte setzen
    for (I = 0; (I < Ed.uLevel_Y_Dimension); I++) {
        Ed.pLevel[I * Ed.uLevel_X_Dimension] = EMERALD_STEEL;
    }
    // Rechte Spalte setzen
    for (I = 1; (I <= Ed.uLevel_Y_Dimension); I++) {
        Ed.pLevel[I * Ed.uLevel_X_Dimension - 1] = EMERALD_STEEL;
    }
}


/*----------------------------------------------------------------------------
Name:           GetElementByMouseposition
------------------------------------------------------------------------------
Beschreibung: Ermittelt anhand der Mausposition das Level-Element, auf das der
              Mauspfeil zeigt.
Parameter
      Eingang: nMouseXpos, int, X-Position des Mauspfeils
               nMouseYpos, int, Y-Position des Mauspfeils
      Ausgang: -
Rückgabewert:  uint16_t, Levelelement, wenn keines ermittelt werden kann,
                         wird EMERALD_INVALID zurück gegeben.
Seiteneffekte: g_PanelElements[]
------------------------------------------------------------------------------*/
uint16_t GetElementByMouseposition(int nMouseXpos, int nMouseYpos) {
    uint32_t uElement;
    int uX;
    int uY;

    uElement = EMERALD_NONE;
    if ((nMouseXpos >= 840) && (nMouseXpos <= 1016)) {
        uX = (nMouseXpos - 840) / 23;
    } else {
        uX = -1;
    }
    if ((nMouseYpos >= 8) && (nMouseYpos <= 620)) {
        uY = (nMouseYpos - 8) / 23;
    } else {
        uY = -1;
    }
    if ((uX != -1) && (uY != -1)) {
        uElement = g_PanelElements[(uY * 8) + uX];
    }
    return uElement;
}


/*----------------------------------------------------------------------------
Name:           DrawEditorPanel
------------------------------------------------------------------------------
Beschreibung: Füllt das Editor-Panel mit Elementen aus dem Array g_PanelElements[].
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: g_PanelElements[], Playfiel.x
------------------------------------------------------------------------------*/
int FillPanel(SDL_Renderer *pRenderer) {
    uint32_t uI;
    uint32_t uX;
    uint32_t uY;
    uint32_t uTextureIndex;
    int nErrorCode;
    SDL_Rect DestR;                     // Zum Kopieren in den Renderer
    float fAngle;

    // Bei der aktuellen Fenster- (1024 * 768) und Panelgröße sind 32 * 8 = 256 Elemente möglich
    nErrorCode = 0;
    for (uI = 0; (g_PanelElements[uI] != EMERALD_INVALID) && (nErrorCode == 0); uI++) {
        if (g_PanelElements[uI] != EMERALD_NONE) {
            uX = uI % 8;
            uY = uI / 8;
            // Position innerhalb des Renderers
            DestR.x = EDITORPANEL_X + 8 + uX * 23;
            DestR.y = EDITORPANEL_Y + 8 + uY * 23;
            DestR.w = FONT_W / 2;
            DestR.h = FONT_H / 2;
            uTextureIndex = GetTextureIndexByElement(g_PanelElements[uI],Ed.uFrameCounter % 16,&fAngle);
            if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&DestR,fAngle,NULL,SDL_FLIP_NONE) != 0) {
                SDL_Log("%s: SDL_RenderCopyEx(standard) failed: %s",__FUNCTION__,SDL_GetError());
                nErrorCode = -1;
            }
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           DrawEditorPanel
------------------------------------------------------------------------------
Beschreibung: Zeichnet das Editor-Panel.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: g_PanelColorPatterns[]
------------------------------------------------------------------------------*/
int DrawEditorPanel(SDL_Renderer *pRenderer) {
    uint32_t I;
    float fAngle;
    int nErrorCode = 0;
    SDL_Rect Rect;
    uint32_t uTextureIndex;

    for (I = 0; (I < 4) && (nErrorCode == 0); I++) {
        SDL_SetRenderDrawColor(pRenderer,g_PanelColorPatterns[I * 3 + 0],g_PanelColorPatterns[I * 3 + 1],g_PanelColorPatterns[I * 3 + 2], SDL_ALPHA_OPAQUE);  // Farbe für Rechteck setzen
        Rect.x = EDITORPANEL_X + 1 + 4 - I;
        Rect.y = EDITORPANEL_Y + 3 - I;
        Rect.w = EDITORPANEL_W - 8 + I * 2;    // -8, da 2x Wandstärke
        Rect.h = EDITORPANEL_H - 6 + I * 2;    // -8, da 2x Wandstärke
        nErrorCode = SDL_RenderDrawRect(pRenderer,&Rect);
    }
    if (nErrorCode == 0) {
        nErrorCode = FillPanel(pRenderer);
        for (I = 0; (I < 4) && (nErrorCode == 0); I++) {
            // linkes Quadrat, selected Element
            SDL_SetRenderDrawColor(pRenderer,g_PanelColorPatterns[I * 3 + 0],g_PanelColorPatterns[I * 3 + 1],g_PanelColorPatterns[I * 3 + 2], SDL_ALPHA_OPAQUE);  // Farbe für Rechteck setzen
            Rect.x = WINDOW_W - 177 - I;
            Rect.y = WINDOW_H - FONT_H - 15 - I;
            Rect.w = FONT_W + 2 + I * 2;
            Rect.h = FONT_H + 2 + I * 2;
            nErrorCode = SDL_RenderDrawRect(pRenderer,&Rect);
            // rechtes Quadrat, selected Element
            SDL_SetRenderDrawColor(pRenderer,g_PanelColorPatterns[I * 3 + 0],g_PanelColorPatterns[I * 3 + 1],g_PanelColorPatterns[I * 3 + 2], SDL_ALPHA_OPAQUE);  // Farbe für Rechteck setzen
            Rect.x = WINDOW_W - 48 - I;
            Rect.y = WINDOW_H - FONT_H - 15 - I;
            Rect.w = FONT_W + 2 + I * 2;
            Rect.h = FONT_H + 2 + I * 2;
            nErrorCode = SDL_RenderDrawRect(pRenderer,&Rect);
            // mittleres Quadrat, selected Element
            SDL_SetRenderDrawColor(pRenderer,g_PanelColorPatterns[I * 3 + 0],g_PanelColorPatterns[I * 3 + 1],g_PanelColorPatterns[I * 3 + 2], SDL_ALPHA_OPAQUE);  // Farbe für Rechteck setzen
            Rect.x = WINDOW_W - 112 - I;
            Rect.y = WINDOW_H - FONT_H - 15 - I;
            Rect.w = FONT_W + 2 + I * 2;
            Rect.h = FONT_H + 2 + I * 2;
            nErrorCode = SDL_RenderDrawRect(pRenderer,&Rect);
        }
        if (nErrorCode == 0) {
            // selected element, left
            Rect.x = WINDOW_W - 177 + 1;
            Rect.y = WINDOW_H - FONT_H - 15 + 1;
            Rect.w = FONT_W;
            Rect.h = FONT_H;
            uTextureIndex = GetTextureIndexByElement(Ed.uSelectedElementLeft[Ed.uMenuState],Ed.uFrameCounter % 16,&fAngle);
            nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&Rect,fAngle,NULL, SDL_FLIP_NONE);
            // selected element, right
            Rect.x = WINDOW_W - 48 + 1;
            Rect.y = WINDOW_H - FONT_H - 15 + 1;
            Rect.w = FONT_W;
            Rect.h = FONT_H;
            uTextureIndex = GetTextureIndexByElement(Ed.uSelectedElementRight[Ed.uMenuState],Ed.uFrameCounter % 16,&fAngle);
            nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&Rect,fAngle,NULL, SDL_FLIP_NONE);
            // selected element, middle
            Rect.x = WINDOW_W - 112 + 1;
            Rect.y = WINDOW_H - FONT_H - 15 + 1;
            Rect.w = FONT_W;
            Rect.h = FONT_H;
            uTextureIndex = GetTextureIndexByElement(Ed.uSelectedElementMiddle[Ed.uMenuState],Ed.uFrameCounter % 16,&fAngle);
            nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&Rect,0,NULL, SDL_FLIP_NONE);
        }
    }
    if (nErrorCode == 0) {
        nErrorCode = -1;
        if (PrintLittleFont(pRenderer,842,697,0,(char*)"Selected Elements") == 0) {
            nErrorCode = SDL_SetRenderDrawColor(pRenderer,0,0,0, SDL_ALPHA_OPAQUE);
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           RenderEditorLevel
------------------------------------------------------------------------------
Beschreibung: Kopiert den sichtbaren Teil des Editor-Levels in den Renderer
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
               pnXpos, int *, Pixel-Positionierung X (obere linke Ecke des Levelausschnitts)
               pnYpos, int *, Pixel-Positionierung Y (obere linke Ecke des Levelausschnitts)
               nAnimationCount, int, 0 - 15 für Animationsstufe
      Ausgang: pnXpos, int *, ggf. korrigierte Pixel-Positionierung X (obere linke Ecke des Levelausschnitts)
               pnYpos, int *, ggf. korrigierte Pixel-Positionierung Y (obere linke Ecke des Levelausschnitts)
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Ed.
------------------------------------------------------------------------------*/
int RenderEditorLevel(SDL_Renderer *pRenderer, int *pnXpos, int *pnYpos, int nAnimationCount)
{
    int nErrorCode;
    uint32_t uX;
    uint32_t uY;
    uint32_t I;                         // Levelindex
    SDL_Rect DestR;                     // Zum Kopieren in den Renderer
    uint16_t uLevelElement;
    float fAngle;

    // Die Eingangsparameter "grob" prüfen, damit nichts Schlimmes passiert
    if ((pRenderer == NULL) || (Ed.pLevel == NULL)) {
        SDL_Log("%s: bad input parameters",__FUNCTION__);
        return -1;
    }
    // Positionsüberläufe abfangen
    if (*pnXpos < 0) {
        *pnXpos = 0;
    } else if (*pnXpos > Ed.nMaxXpos) {
        *pnXpos = Ed.nMaxXpos;
    }
    if (*pnYpos < 0) {
        *pnYpos = 0;
    } else if (*pnYpos > Ed.nMaxYpos) {
        *pnYpos = Ed.nMaxYpos;
    }
    // oberstes linkes Element, welches entweder komplett oder dessen untere rechte Ecke noch gerade sichtbar ist
    Ed.uUpperLeftLevelIndex = (*pnXpos / Ed.uFont_W) + (*pnYpos / Ed.uFont_H) * Ed.uLevel_X_Dimension;
    nErrorCode = 0;
    // Den sichtbaren Teil des Levels in den Renderer kopieren.
    for (uY = 0; (uY <= (WINDOW_H / Ed.uFont_H) && (uY < Ed.uLevel_Y_Dimension) && (nErrorCode == 0)); uY++) {
        for (uX = 0; (uX <  Ed.uVisibleX) && (uX < Ed.uLevel_X_Dimension) && (nErrorCode == 0); uX++) {
            // Levelindex berechnen
            I = Ed.uUpperLeftLevelIndex + uY * Ed.uLevel_X_Dimension + uX;
            if (I > ((Ed.uLevel_X_Dimension * Ed.uLevel_Y_Dimension) - 1)) {
                //SDL_Log("%s: Warning: Level-Overflow, V:%u   X:%u   Y:%u  T:%u",__FUNCTION__,I,uX,uY,SDL_GetTicks());
                break;
            }
            uLevelElement = Ed.pLevel[I];
            if ((uLevelElement != EMERALD_SPACE) && (uLevelElement != EMERALD_BEAM_CROSS) && (nErrorCode == 0)) {   // Space muss nicht gerendert werden, da nach jedem Bild sowieso Alles gelöscht wird
                // Position innerhalb des Renderers
                DestR.x = uX * Ed.uFont_W - (*pnXpos % Ed.uFont_W) + Ed.uShiftLevelXpix;
                DestR.y = uY * Ed.uFont_H - (*pnYpos % Ed.uFont_H) + Ed.uShiftLevelYpix;
                DestR.w = Ed.uFont_W;
                DestR.h = Ed.uFont_H;
                if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(GetTextureIndexByElement(uLevelElement,nAnimationCount,&fAngle)),NULL,&DestR,0,NULL, SDL_FLIP_NONE) != 0) {
                    SDL_Log("%s: SDL_RenderCopyEx(standard) failed: %s",__FUNCTION__,SDL_GetError());
                    nErrorCode = -1;
                }
            }
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           EditorStateLevel
------------------------------------------------------------------------------
Beschreibung: Wenn sich der Leveleditor auf Menustate = MENUSTATE_LEVEL befindet,
              wird diese Funktion ausgeführt. Sie ermöglicht das Zeichnen von Elementen
              in einem Level.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Ed.x, Buttons.x, InputStates.x
------------------------------------------------------------------------------*/
int EditorStateLevel(SDL_Renderer *pRenderer) {
    char szText[100];
    uint32_t uLevelX;
    uint32_t uLevelY;
    bool bBorder;

    //sprintf(szText,"x: %d   y: %d",InputStates.nMouseXpos,InputStates.nMouseYpos);
    //PrintLittleFont(pRenderer,10,10,0,szText); // Maus-Koordinate anzeigen

    if (InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE])  {
        Ed.bEditorRun = false;
    }
    if (InputStates.pKeyboardArray[SDL_SCANCODE_RIGHT])  {
        if ((InputStates.pKeyboardArray[SDL_SCANCODE_LSHIFT]) || (InputStates.pKeyboardArray[SDL_SCANCODE_RSHIFT])) {
            Ed.nXpos = Ed.nXpos + Ed.uScrollPixelFastX;
        } else {
            Ed.nXpos = Ed.nXpos + Ed.uScrollPixelX;
        }
    }
    if (InputStates.pKeyboardArray[SDL_SCANCODE_LEFT])  {
        if ((InputStates.pKeyboardArray[SDL_SCANCODE_LSHIFT]) || (InputStates.pKeyboardArray[SDL_SCANCODE_RSHIFT])) {
            Ed.nXpos = Ed.nXpos - Ed.uScrollPixelFastX;
        } else {
            Ed.nXpos = Ed.nXpos - Ed.uScrollPixelX;
        }
    }
    if ((InputStates.pKeyboardArray[SDL_SCANCODE_UP]) || (InputStates.nMouseWheelY < 0))  {
        if ((InputStates.pKeyboardArray[SDL_SCANCODE_LSHIFT]) || (InputStates.pKeyboardArray[SDL_SCANCODE_RSHIFT])) {
            Ed.nYpos = Ed.nYpos - Ed.uScrollPixelFastY;
        } else {
            Ed.nYpos = Ed.nYpos - Ed.uScrollPixelY;
        }
    }
    if ((InputStates.pKeyboardArray[SDL_SCANCODE_DOWN]) || (InputStates.nMouseWheelY > 0))  {
        if ((InputStates.pKeyboardArray[SDL_SCANCODE_LSHIFT]) || (InputStates.pKeyboardArray[SDL_SCANCODE_RSHIFT])) {
            Ed.nYpos = Ed.nYpos + Ed.uScrollPixelFastY;
        } else {
            Ed.nYpos = Ed.nYpos + Ed.uScrollPixelY;
        }
    }
    RenderEditorLevel(pRenderer,&Ed.nXpos,&Ed.nYpos,Ed.uFrameCounter % 16);
    sprintf(szText,"X: -  Y: -");
    if ((InputStates.nMouseXpos >= Ed.nMinXLevel) && (InputStates.nMouseXpos <= Ed.nMaxXLevel) &&
        (InputStates.nMouseYpos >= Ed.nMinYLevel) && (InputStates.nMouseYpos <= Ed.nMaxYLevel)) {
        uLevelX = Ed.uUpperLeftLevelIndex % Ed.uLevel_X_Dimension + (InputStates.nMouseXpos - Ed.uShiftLevelXpix) / Ed.uFont_W;
        uLevelY = Ed.uUpperLeftLevelIndex / Ed.uLevel_X_Dimension + (InputStates.nMouseYpos - Ed.uShiftLevelYpix) / Ed.uFont_H;
        // SDL_Log("uLevelX : %u/%u   uLevelY: %u/%u",uLevelX, Ed.uLevel_X_Dimension,uLevelY, Ed.uLevel_Y_Dimension);
        if ((uLevelX < Ed.uLevel_X_Dimension) && (uLevelY < Ed.uLevel_Y_Dimension)) {
            bBorder = ((uLevelX == 0) || (uLevelY == 0) || (uLevelX == (Ed.uLevel_X_Dimension - 1)) || (uLevelY == (Ed.uLevel_Y_Dimension - 1)));
            sprintf(szText,"X: %03d   Y: %03d",uLevelX,uLevelY);
            if (InputStates.bLeftMouseButton) {
                if (((bBorder) && IsSteel(Ed.uSelectedElementLeft[Ed.uMenuState])) || (!bBorder)) {
                    if (Ed.uSelectedElementLeft[Ed.uMenuState] == EMERALD_MAN) {
                        ClearOldMan();
                    }
                    Ed.pLevel[uLevelY * Ed.uLevel_X_Dimension + uLevelX] = Ed.uSelectedElementLeft[Ed.uMenuState];
                }
            } else if (InputStates.bMidMouseButton) {
                if (((bBorder) && IsSteel(Ed.uSelectedElementMiddle[Ed.uMenuState])) || (!bBorder)) {
                    if (Ed.uSelectedElementMiddle[Ed.uMenuState] == EMERALD_MAN) {
                        ClearOldMan();
                    }
                    Ed.pLevel[uLevelY * Ed.uLevel_X_Dimension + uLevelX] = Ed.uSelectedElementMiddle[Ed.uMenuState];
                }
            } else if (InputStates.bRightMouseButton) {
                if ((!bBorder) && (Ed.uSelectedElementRight[Ed.uMenuState] != EMERALD_MAN)) {
                    SDL_Log("x: %u    y: %u     Fill:%02x    Ground: %02x",uLevelX,uLevelY,Ed.uSelectedElementRight[Ed.uMenuState],Ed.pLevel[uLevelY * Ed.uLevel_X_Dimension + uLevelX]);
                    FillLevelArea(uLevelX,uLevelY,Ed.uSelectedElementRight[Ed.uMenuState],Ed.pLevel[uLevelY * Ed.uLevel_X_Dimension + uLevelX]);
                }
            }
        }
    }
    if (CheckReplicators(Ed.pLevel,Ed.uLevel_X_Dimension,Ed.uLevel_Y_Dimension) == 0) {
        if (GetManCoordinates(Ed.pLevel,Ed.uLevel_X_Dimension,Ed.uLevel_Y_Dimension,NULL,NULL) == 0) {
            if (CheckAcidPools(Ed.pLevel,Ed.uLevel_X_Dimension,Ed.uLevel_Y_Dimension) == 0) {
                PrintLittleFont(pRenderer,WINDOW_W - 640, WINDOW_H - 16,0,"LEVEL: OK");
            } else {
                PrintLittleFont(pRenderer,WINDOW_W - 640, WINDOW_H - 16,0,"LEVEL: ACID POOL ERROR");
                Ed.bFoundError = true;
            }
        } else {
            PrintLittleFont(pRenderer,WINDOW_W - 640, WINDOW_H - 16,0,"LEVEL: MAN ERROR");
            Ed.bFoundError = true;
        }
    } else {
        PrintLittleFont(pRenderer,WINDOW_W - 640, WINDOW_H - 16,0,"LEVEL: REPLICATOR ERROR");
        Ed.bFoundError = true;
    }
    return PrintLittleFont(pRenderer,WINDOW_W - 350, WINDOW_H - 16,0,szText); // Level-Koordinaten anzeigen
}


/*----------------------------------------------------------------------------
Name:           PreCalcYamCoords
------------------------------------------------------------------------------
Beschreibung: Berechnet die Koordinaten der Kästchen für den YAM-Editor vor.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Ed.x
------------------------------------------------------------------------------*/
void PreCalcYamCoords(void) {
    uint32_t I;
    uint32_t uXpos;
    uint32_t uYpos;

    for (I = 0; (I < EMERALD_MAX_YAM_EXPLOSIONS); I++) {
        uXpos = (I % 10);
        uYpos = (I / 10);
        Ed.YamCoords[I].uXstart = uXpos * 80 + 4;   // Spaltenabstand beträgt 80 Pixel
        Ed.YamCoords[I].uXend = Ed.YamCoords[I].uXstart + (FONT_W / 2) * 3 - 1;
        Ed.YamCoords[I].uYstart = uYpos * 74 + 20;  // Reihenabstand beträgt 74 Pixel
        Ed.YamCoords[I].uYend = Ed.YamCoords[I].uYstart + (FONT_H / 2) * 3 - 1;
    }
 }


/*----------------------------------------------------------------------------
Name:           GetYamExplosionFromMousepointer
------------------------------------------------------------------------------
Beschreibung: Ermittelt anhand der Mausposition, welche Yam-Explosion angesprochen wird.
Parameter
      Eingang: -
      Ausgang: puElementIndex, uint8_t *, Elementindex innerhalb der gefundenen Yam-Explosion,
               nur gültig, wenn Rückgabe nicht -1
Rückgabewert:  int, Yam-Explosion, falls -1, dann befindet sich Mauseposition nicht auf Yam-Explosion
Seiteneffekte: Ed.x, InputStates.x
------------------------------------------------------------------------------*/
 int GetYamExplosionFromMousepointer(uint8_t *puElementIndex) {
    uint32_t I;
    bool bFound;
    uint32_t uLine;
    uint32_t uColumn;

    I = 0;
    bFound = false;
    do {
        if ((InputStates.nMouseXpos >= Ed.YamCoords[I].uXstart) && (InputStates.nMouseXpos <= Ed.YamCoords[I].uXend) &&
           (InputStates.nMouseYpos >= Ed.YamCoords[I].uYstart) && (InputStates.nMouseYpos <= Ed.YamCoords[I].uYend)) {
            uLine = (InputStates.nMouseYpos - Ed.YamCoords[I].uYstart) / (FONT_H / 2);
            uColumn = (InputStates.nMouseXpos - Ed.YamCoords[I].uXstart) / (FONT_H / 2);
            *puElementIndex = uLine * 3 + uColumn;
            bFound = true;
        } else {
            I++;
        }
    } while ((I <= Ed.uMaxYamExplosionIndex) && (!bFound));
    if (bFound) {
        return (int)I;
    } else {
        return -1;
    }
 }


/*----------------------------------------------------------------------------
Name:           EditorStateYams
------------------------------------------------------------------------------
Beschreibung: Wenn sich der Leveleditor auf Menustate = MENUSTATE_YAMS befindet,
              wird diese Funktion ausgeführt. Sie ermöglicht das Zeichnen von Elementen
              für explodierende Yams.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Ed.x, Buttons.x, InputStates.x
------------------------------------------------------------------------------*/
int EditorStateYams(SDL_Renderer *pRenderer) {
    char szText[64];
    uint32_t I;
    uint32_t uLine;
    int nErrorCode;
    uint32_t uXpos;
    uint32_t uYpos;
    uint32_t E;
    uint16_t uElement;
    int nYamExplosion;
    uint8_t uElementIndex;
    SDL_Rect Rect;  // Für Quadrate
    SDL_Rect DestR;  // Für Elemente
    float fAngle;
    uint32_t uTextureIndex;

    nErrorCode = 0;
    for (I = 0; (I <= Ed.uMaxYamExplosionIndex) && (nErrorCode == 0); I++) {
        sprintf(szText,"Yam%02d",I);

        uXpos = ((I % 10) * 80) + 4;
        uYpos = ((I / 10) * 74) + 4;
        PrintLittleFont(pRenderer,uXpos,uYpos,0,szText);

        for (uLine = 0; (uLine < 4) && (nErrorCode == 0); uLine++) {
            // Quadrat für Yam
            SDL_SetRenderDrawColor(pRenderer,g_PanelColorPatterns[uLine * 3 + 0],g_PanelColorPatterns[uLine * 3 + 1],g_PanelColorPatterns[uLine * 3 + 2], SDL_ALPHA_OPAQUE);  // Farbe für Rechteck setzen
            Rect.x = uXpos - uLine;
            Rect.y = uYpos - uLine;
            Rect.w = FONT_LITTLE_347_W * 5 + uLine * 2;
            Rect.h = FONT_LITTLE_347_W * 5 + 16 + uLine * 2;
            nErrorCode = SDL_RenderDrawRect(pRenderer,&Rect);
        }
        for (uLine = 0; (uLine < 4) && (nErrorCode == 0); uLine++) {
            SDL_SetRenderDrawColor(pRenderer,g_PanelColorPatterns[uLine * 3 + 0],g_PanelColorPatterns[uLine * 3 + 1],g_PanelColorPatterns[uLine * 3 + 2], SDL_ALPHA_OPAQUE);  // Farbe für Rechteck setzen
            SDL_RenderDrawLine(pRenderer, uXpos + 1, uYpos + FONT_H / 2 - uLine, uXpos + 3 * FONT_W / 2, uYpos + FONT_H / 2 - uLine);
        }
        for (E = 0; (E < 9) && (nErrorCode == 0); E++) {
            uElement = Ed.YamExplosions[I].uElement[E];
            DestR.x = uXpos + (E % 3) * FONT_W / 2 + 1;
            DestR.y = uYpos + (E / 3) * FONT_H / 2 + FONT_H / 2 + 1;
            DestR.w = FONT_W / 2;
            DestR.h = FONT_H / 2;
            uTextureIndex = GetTextureIndexByElement(uElement,0,&fAngle);
            if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&DestR,fAngle,NULL, SDL_FLIP_NONE) != 0) {
                SDL_Log("%s: SDL_RenderCopyEx(standard) failed: %s",__FUNCTION__,SDL_GetError());
                nErrorCode = -1;
            }
        }
    }
    nYamExplosion = GetYamExplosionFromMousepointer(&uElementIndex);
    if (nYamExplosion != -1) {
        if (InputStates.bLeftMouseButton) {
            Ed.YamExplosions[nYamExplosion].uElement[uElementIndex] = Ed.uSelectedElementLeft[Ed.uMenuState];
        } else if (InputStates.bMidMouseButton) {
            Ed.YamExplosions[nYamExplosion].uElement[uElementIndex] = Ed.uSelectedElementMiddle[Ed.uMenuState];
        } else if (InputStates.bRightMouseButton) {
            Ed.YamExplosions[nYamExplosion].uElement[uElementIndex] = Ed.uSelectedElementRight[Ed.uMenuState];
        }
    }
    sprintf(szText,"YAMS: %02u",Ed.uMaxYamExplosionIndex);
    PrintLittleFont(pRenderer,WINDOW_W - 355,WINDOW_H - 20,0,szText);
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           EditorStateMachines
------------------------------------------------------------------------------
Beschreibung: Wenn sich der Leveleditor auf Menustate = MENUSTATE_MACHINES befindet,
              wird diese Funktion ausgeführt. Sie ermöglicht die Konfiguration von
              Replikatoren und Lichtschranken.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Ed.x, Buttons.x, InputStates.x
------------------------------------------------------------------------------*/
int EditorStateMachines(SDL_Renderer *pRenderer) {
    int nErrorCode;
    uint32_t uTextureIndex;
    uint32_t uElement;
    uint32_t I;
    bool bXmouseReplicatorSwitch;
    bool bXmouseReplicatorObject;
    bool bYmouseRedReplicator;
    bool bYmouseYellowReplicator;
    bool bYmouseGreenReplicator;
    bool bYmouseBlueReplicator;
    float fAngle;
    SDL_Rect DestR;
    uint32_t uPositionsAndElements[] = {// Replicator red
                                        64,64,EMERALD_REPLICATOR_RED_TOP_LEFT,
                                        64 + FONT_W,64,EMERALD_REPLICATOR_RED_TOP_MID,
                                        64 + 2 * FONT_W,64,EMERALD_REPLICATOR_RED_TOP_RIGHT,
                                        64, 64 + FONT_H,EMERALD_REPLICATOR_RED_BOTTOM_LEFT,
                                        64 + 2 * FONT_W,64 + FONT_H,EMERALD_REPLICATOR_RED_BOTTOM_RIGHT,
                                        // Replicator yellow
                                        64,160,EMERALD_REPLICATOR_YELLOW_TOP_LEFT,
                                        64 + FONT_W,160,EMERALD_REPLICATOR_YELLOW_TOP_MID,
                                        64 + 2 * FONT_W,160,EMERALD_REPLICATOR_YELLOW_TOP_RIGHT,
                                        64, 160 + FONT_H,EMERALD_REPLICATOR_YELLOW_BOTTOM_LEFT,
                                        64 + 2 * FONT_W,160 + FONT_H,EMERALD_REPLICATOR_YELLOW_BOTTOM_RIGHT,
                                        // Replicator green
                                        64,256,EMERALD_REPLICATOR_GREEN_TOP_LEFT,
                                        64 + FONT_W,256,EMERALD_REPLICATOR_GREEN_TOP_MID,
                                        64 + 2 * FONT_W,256,EMERALD_REPLICATOR_GREEN_TOP_RIGHT,
                                        64, 256 + FONT_H,EMERALD_REPLICATOR_GREEN_BOTTOM_LEFT,
                                        64 + 2 * FONT_W,256 + FONT_H,EMERALD_REPLICATOR_GREEN_BOTTOM_RIGHT,
                                        // Replicator blue
                                        64,352,EMERALD_REPLICATOR_BLUE_TOP_LEFT,
                                        64 + FONT_W,352,EMERALD_REPLICATOR_BLUE_TOP_MID,
                                        64 + 2 * FONT_W,352,EMERALD_REPLICATOR_BLUE_TOP_RIGHT,
                                        64, 352 + FONT_H,EMERALD_REPLICATOR_BLUE_BOTTOM_LEFT,
                                        64 + 2 * FONT_W,352 + FONT_H,EMERALD_REPLICATOR_BLUE_BOTTOM_RIGHT,
                                        // Replicator switches
                                        192,64,EMERALD_REPLICATOR_RED_SWITCH,
                                        192,160,EMERALD_REPLICATOR_YELLOW_SWITCH,
                                        192,256,EMERALD_REPLICATOR_GREEN_SWITCH,
                                        192,352,EMERALD_REPLICATOR_BLUE_SWITCH,
                                        // Light barrier switches
                                        192,512,EMERALD_LIGHTBARRIER_RED_SWITCH,
                                        192,576,EMERALD_LIGHTBARRIER_YELLOW_SWITCH,
                                        192,640,EMERALD_LIGHTBARRIER_GREEN_SWITCH,
                                        192,704,EMERALD_LIGHTBARRIER_BLUE_SWITCH,
                                        // Light barriers
                                        96,512,EMERALD_LIGHTBARRIER_RED_RIGHT,
                                        96,576,EMERALD_LIGHTBARRIER_YELLOW_RIGHT,
                                        96,640,EMERALD_LIGHTBARRIER_GREEN_RIGHT,
                                        96,704,EMERALD_LIGHTBARRIER_BLUE_RIGHT,
                                        // Replicator objects
                                        64 + FONT_W,64 + FONT_H,EMERALD_SPACE,
                                        64 + FONT_W,64 + 4 * FONT_H,EMERALD_SPACE,
                                        64 + FONT_W,64 + 7 * FONT_H,EMERALD_SPACE,
                                        64 + FONT_W,64 + 10 * FONT_H,EMERALD_SPACE,
                                        // Light barriers streams
                                        64 + 2 * FONT_W,64 + 14 * FONT_H,EMERALD_BEAM_RED_HORIZONTAL,
                                        64 + 3 * FONT_W,64 + 14 * FONT_H,EMERALD_BEAM_RED_HORIZONTAL,
                                        64 + 2 * FONT_W,64 + 16 * FONT_H,EMERALD_BEAM_YELLOW_HORIZONTAL,
                                        64 + 3 * FONT_W,64 + 16 * FONT_H,EMERALD_BEAM_YELLOW_HORIZONTAL,
                                        64 + 2 * FONT_W,64 + 18 * FONT_H,EMERALD_BEAM_GREEN_HORIZONTAL,
                                        64 + 3 * FONT_W,64 + 18 * FONT_H,EMERALD_BEAM_GREEN_HORIZONTAL,
                                        64 + 2 * FONT_W,64 + 20 * FONT_H,EMERALD_BEAM_BLUE_HORIZONTAL,
                                        64 + 3 * FONT_W,64 + 20 * FONT_H,EMERALD_BEAM_BLUE_HORIZONTAL
                                        };
    uPositionsAndElements[32 * 3 + 2] = Ed.uReplicatorRedObject;
    uPositionsAndElements[33 * 3 + 2] = Ed.uReplicatorYellowObject;
    uPositionsAndElements[34 * 3 + 2] = Ed.uReplicatorGreenObject;
    uPositionsAndElements[35 * 3 + 2] = Ed.uReplicatorBlueObject;;
    if (!Ed.bLightBarrierRedOn) {
        uPositionsAndElements[36 * 3 + 2] = EMERALD_SPACE;
        uPositionsAndElements[37 * 3 + 2] = EMERALD_SPACE;
    }
    if (!Ed.bLightBarrierYellowOn) {
        uPositionsAndElements[38 * 3 + 2] = EMERALD_SPACE;
        uPositionsAndElements[39 * 3 + 2] = EMERALD_SPACE;
    }
    if (!Ed.bLightBarrierGreenOn) {
        uPositionsAndElements[40 * 3 + 2] = EMERALD_SPACE;
        uPositionsAndElements[41 * 3 + 2] = EMERALD_SPACE;
    }
    if (!Ed.bLightBarrierBlueOn) {
        uPositionsAndElements[42 * 3 + 2] = EMERALD_SPACE;
        uPositionsAndElements[43 * 3 + 2] = EMERALD_SPACE;
    }
    // Replikatoren
    PrintLittleFont(pRenderer,368,24,0,"REPLICATORS");
    PrintLittleFont(pRenderer,192,112,0,"ACTIVE AT START:");
    if (Ed.bReplicatorRedOn) {
        PrintLittleFont(pRenderer,352,112,0,"YES");
    } else {
        PrintLittleFont(pRenderer,352,112,0,"NO");
    }
    PrintLittleFont(pRenderer,192,208,0,"ACTIVE AT START:");
    if (Ed.bReplicatorYellowOn) {
        PrintLittleFont(pRenderer,352,208,0,"YES");
    } else {
        PrintLittleFont(pRenderer,352,208,0,"NO");
    }
    PrintLittleFont(pRenderer,192,304,0,"ACTIVE AT START:");
    if (Ed.bReplicatorGreenOn) {
        PrintLittleFont(pRenderer,352,304,0,"YES");
    } else {
        PrintLittleFont(pRenderer,352,304,0,"NO");
    }
    PrintLittleFont(pRenderer,192,400,0,"ACTIVE AT START:");
    if (Ed.bReplicatorBlueOn) {
        PrintLittleFont(pRenderer,352,400,0,"YES");
    } else {
        PrintLittleFont(pRenderer,352,400,0,"NO");
    }
    // Lichtschranken
    PrintLittleFont(pRenderer,360,464,0,"LIGHT BARRIERS");
    PrintLittleFont(pRenderer,240,520,0,"ACTIVE AT START:");
    if (Ed.bLightBarrierRedOn) {
        PrintLittleFont(pRenderer,400,520,0,"YES");
    } else {
        PrintLittleFont(pRenderer,400,520,0,"NO");
    }
    PrintLittleFont(pRenderer,240,584,0,"ACTIVE AT START:");
    if (Ed.bLightBarrierYellowOn) {
        PrintLittleFont(pRenderer,400,584,0,"YES");
    } else {
        PrintLittleFont(pRenderer,400,584,0,"NO");
    }
    PrintLittleFont(pRenderer,240,648,0,"ACTIVE AT START:");
    if (Ed.bLightBarrierGreenOn) {
        PrintLittleFont(pRenderer,400,648,0,"YES");
    } else {
        PrintLittleFont(pRenderer,400,648,0,"NO");
    }
    PrintLittleFont(pRenderer,240,712,0,"ACTIVE AT START:");
    if (Ed.bLightBarrierBlueOn) {
        nErrorCode = PrintLittleFont(pRenderer,400,712,0,"YES");
    } else {
        nErrorCode = PrintLittleFont(pRenderer,400,712,0,"NO");
    }
    for (I = 0; (I < 44) && (nErrorCode == 0); I++) {
        uElement = uPositionsAndElements[I * 3 + 2];
        uTextureIndex = GetTextureIndexByElement(uPositionsAndElements[I * 3 + 2],0,&fAngle);
        if ((uElement == EMERALD_REPLICATOR_RED_SWITCH) && Ed.bReplicatorRedOn) {
            uTextureIndex++;
        } else if ((uElement == EMERALD_REPLICATOR_YELLOW_SWITCH) && Ed.bReplicatorYellowOn) {
            uTextureIndex++;
        } else if ((uElement == EMERALD_REPLICATOR_GREEN_SWITCH) && Ed.bReplicatorGreenOn) {
            uTextureIndex++;
        } else if ((uElement == EMERALD_REPLICATOR_BLUE_SWITCH) && Ed.bReplicatorBlueOn) {
            uTextureIndex++;
        } else if ((uElement == EMERALD_LIGHTBARRIER_RED_SWITCH) && Ed.bLightBarrierRedOn) {
            uTextureIndex++;
        } else if ((uElement == EMERALD_LIGHTBARRIER_YELLOW_SWITCH) && Ed.bLightBarrierYellowOn) {
            uTextureIndex++;
        } else if ((uElement == EMERALD_LIGHTBARRIER_GREEN_SWITCH) && Ed.bLightBarrierGreenOn) {
            uTextureIndex++;
        } else if ((uElement == EMERALD_LIGHTBARRIER_BLUE_SWITCH) && Ed.bLightBarrierBlueOn) {
            uTextureIndex++;
        }
        DestR.x = uPositionsAndElements[I * 3 + 0];
        DestR.y = uPositionsAndElements[I * 3 + 1];
        DestR.w = FONT_W;
        DestR.h = FONT_H;
        if (nErrorCode == 0) {
            if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&DestR,fAngle,NULL, SDL_FLIP_NONE) != 0) {
                nErrorCode = -1;
                SDL_Log("%s: SDL_RenderCopyEx() failed: %s",__FUNCTION__,SDL_GetError());
            }
        }
    }
    // Dieser Block kann die Aktivität der Replikatoren umschalten
    bXmouseReplicatorSwitch = ((InputStates.nMouseXpos >= 192) && (InputStates.nMouseXpos <= 224));
    if ((bXmouseReplicatorSwitch) && (InputStates.bLeftMouseButton)) {
        if ((InputStates.nMouseYpos >= 64) && (InputStates.nMouseYpos <= 96)) {
            Ed.bReplicatorRedOn = !Ed.bReplicatorRedOn;
            WaitNoKey();
        } else if ((InputStates.nMouseYpos >= 160) && (InputStates.nMouseYpos <= 192)) {
            Ed.bReplicatorYellowOn = !Ed.bReplicatorYellowOn;
            WaitNoKey();
        } else if ((InputStates.nMouseYpos >= 256) && (InputStates.nMouseYpos <= 288)) {
            Ed.bReplicatorGreenOn = !Ed.bReplicatorGreenOn;
            WaitNoKey();
        } else if ((InputStates.nMouseYpos >= 352) && (InputStates.nMouseYpos <= 384)) {
            Ed.bReplicatorBlueOn = !Ed.bReplicatorBlueOn;
            WaitNoKey();
        // Dieser Block kann die Aktivität der Lichtschranken umschalten
        } else if ((InputStates.nMouseYpos >= 512) && (InputStates.nMouseYpos <= 544)) {
            Ed.bLightBarrierRedOn = !Ed.bLightBarrierRedOn;
            WaitNoKey();
        } else if ((InputStates.nMouseYpos >= 576) && (InputStates.nMouseYpos <= 608)) {
            Ed.bLightBarrierYellowOn = !Ed.bLightBarrierYellowOn;
            WaitNoKey();
        } else if ((InputStates.nMouseYpos >= 640) && (InputStates.nMouseYpos <= 672)) {
            Ed.bLightBarrierGreenOn = !Ed.bLightBarrierGreenOn;
            WaitNoKey();
        } else if ((InputStates.nMouseYpos >= 704) && (InputStates.nMouseYpos <= 736)) {
            Ed.bLightBarrierBlueOn = !Ed.bLightBarrierBlueOn;
            WaitNoKey();
        }
    }
    // Dieser Block kann das Objekt der Replikatoren umschalten
    bXmouseReplicatorObject = ((InputStates.nMouseXpos >= 96) && (InputStates.nMouseXpos <= 128));
    bYmouseRedReplicator = ((InputStates.nMouseYpos >= 96) && (InputStates.nMouseYpos <= 128));
    bYmouseYellowReplicator = ((InputStates.nMouseYpos >= 192) && (InputStates.nMouseYpos <= 224));;
    bYmouseGreenReplicator = ((InputStates.nMouseYpos >= 288) && (InputStates.nMouseYpos <= 320));;
    bYmouseBlueReplicator = ((InputStates.nMouseYpos >= 384) && (InputStates.nMouseYpos <= 416));;
    if (bXmouseReplicatorObject) {
        if (bYmouseRedReplicator) {
            if (InputStates.bLeftMouseButton) {
                Ed.uReplicatorRedObject = Ed.uSelectedElementLeft[Ed.uMenuState];
            } else if (InputStates.bMidMouseButton) {
                Ed.uReplicatorRedObject = Ed.uSelectedElementMiddle[Ed.uMenuState];
            } else if (InputStates.bRightMouseButton) {
                Ed.uReplicatorRedObject = Ed.uSelectedElementRight[Ed.uMenuState];
            }
        } else if (bYmouseYellowReplicator) {
            if (InputStates.bLeftMouseButton) {
                Ed.uReplicatorYellowObject = Ed.uSelectedElementLeft[Ed.uMenuState];
            } else if (InputStates.bMidMouseButton) {
                Ed.uReplicatorYellowObject = Ed.uSelectedElementMiddle[Ed.uMenuState];
            } else if (InputStates.bRightMouseButton) {
                Ed.uReplicatorYellowObject = Ed.uSelectedElementRight[Ed.uMenuState];
            }
        } else if (bYmouseGreenReplicator) {
            if (InputStates.bLeftMouseButton) {
                Ed.uReplicatorGreenObject = Ed.uSelectedElementLeft[Ed.uMenuState];
            } else if (InputStates.bMidMouseButton) {
                Ed.uReplicatorGreenObject = Ed.uSelectedElementMiddle[Ed.uMenuState];
            } else if (InputStates.bRightMouseButton) {
                Ed.uReplicatorGreenObject = Ed.uSelectedElementRight[Ed.uMenuState];
            }
        } else if (bYmouseBlueReplicator) {
            if (InputStates.bLeftMouseButton) {
                Ed.uReplicatorBlueObject = Ed.uSelectedElementLeft[Ed.uMenuState];
            } else if (InputStates.bMidMouseButton) {
                Ed.uReplicatorBlueObject = Ed.uSelectedElementMiddle[Ed.uMenuState];
            } else if (InputStates.bRightMouseButton) {
                Ed.uReplicatorBlueObject = Ed.uSelectedElementRight[Ed.uMenuState];
            }
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           EditorStateTimeAndScores
------------------------------------------------------------------------------
Beschreibung: Wenn sich der Leveleditor auf Menustate = MENUSTATE_TIME_AND_SCORES befindet,
              wird diese Funktion ausgeführt. Sie ermöglicht die Konfiguration von
              Zeiten und Scores.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Ed.x, Buttons.x, InputStates.x
------------------------------------------------------------------------------*/
int EditorStateTimeAndScores(SDL_Renderer *pRenderer) {
    int nErrorCode;

    nErrorCode = PrintLittleFont(pRenderer,368,24,0,"TODO");
    return nErrorCode;
}





int Editor(SDL_Window *pWindow, SDL_Renderer *pRenderer) {
    uint32_t uMouseElement;             // Levelelement, auf das Mauspfeil zeigt

    Ed.nXpos = 0;
    Ed.nYpos = 0;
    Ed.bEditorRun = (InitEditor(false,0,0,LEVEL_XML_FILENAME) == 0);
    //Ed.bEditorRun = (InitEditor(true,110,110,NULL) == 0);
    CreateButton("Save and play",WINDOW_W - 184,WINDOW_H - 100,true);
    CreateButton("Yams",WINDOW_W - 48,WINDOW_H - 100,true);
    CreateButton("Return to level",WINDOW_W - 184,WINDOW_H - 100,false);
    CreateButton("-",WINDOW_W - 382,WINDOW_H - FONT_H + 8,false);
    CreateButton("+",WINDOW_W - 265,WINDOW_H - FONT_H + 8,false);
    CreateButton("Machines",WINDOW_W - 184,WINDOW_H - 130,true);
    CreateButton("Time+Scores",WINDOW_W - 104,WINDOW_H - 130,true);
    PreCalcYamCoords();
    while (Ed.bEditorRun) {
        UpdateInputStates();
        uMouseElement = GetElementByMouseposition(InputStates.nMouseXpos, InputStates.nMouseYpos);
        Ed.bFoundError = false;
        if ((InputStates.bLeftMouseButton) && (uMouseElement != EMERALD_NONE)) {
            Ed.uSelectedElementLeft[Ed.uMenuState] = uMouseElement;
        }
        if ((InputStates.bMidMouseButton) && (uMouseElement != EMERALD_NONE)) {
            Ed.uSelectedElementMiddle[Ed.uMenuState] = uMouseElement;
        }
        if ((InputStates.bRightMouseButton) && (uMouseElement != EMERALD_NONE)) {
            Ed.uSelectedElementRight[Ed.uMenuState] = uMouseElement;
        }
        if (Ed.uMenuState == MENUSTATE_LEVEL) {
            Ed.bEditorRun = (EditorStateLevel(pRenderer) == 0);
        } else if (Ed.uMenuState == MENUSTATE_YAMS) {
            Ed.bEditorRun = (EditorStateYams(pRenderer) == 0);
        } else if (Ed.uMenuState == MENUSTATE_MACHINES) {
            Ed.bEditorRun = (EditorStateMachines(pRenderer) == 0);
        } else if (Ed.uMenuState == MENUSTATE_TIME_AND_SCORES) {
            Ed.bEditorRun = (EditorStateTimeAndScores(pRenderer) == 0);
        }
        DrawEditorPanel(pRenderer);
        PrintLittleFont(pRenderer, 0, WINDOW_H - FONT_LITTLE_347_H,0,ge_szElementNames[uMouseElement]);// Element unten links anzeigen
        ShowButtons(pRenderer);
        if ((Buttons.Button[BUTTON_SAVE_AND_PLAY].bPressed) && (!Ed.bFoundError)) {
            WriteLevelXmlFile(LEVEL_XML_FILENAME);
            Ed.bEditorRun = false;
        } else if (Buttons.Button[BUTTON_YAMS].bPressed) {
            Ed.uMenuState = MENUSTATE_YAMS;
            SetPanelElements(Ed.uMenuState);
            SetButtonActivity(BUTTON_MACHINES,false);
            SetButtonActivity(BUTTON_TIME_AND_SCORE,false);
            SetButtonActivity(BUTTON_SAVE_AND_PLAY,false);
            SetButtonActivity(BUTTON_YAMS,false);
            SetButtonActivity(BUTTON_RETURN_TO_LEVEL,true);
            SetButtonActivity(BUTTON_PLUS,true);
            SetButtonActivity(BUTTON_MINUS,true);
        } else if (Buttons.Button[BUTTON_RETURN_TO_LEVEL].bPressed) {
            Ed.uMenuState = MENUSTATE_LEVEL;
            SetPanelElements(Ed.uMenuState);
            SetButtonActivity(BUTTON_SAVE_AND_PLAY,true);
            SetButtonActivity(BUTTON_YAMS,true);
            SetButtonActivity(BUTTON_RETURN_TO_LEVEL,false);
            SetButtonActivity(BUTTON_PLUS,false);
            SetButtonActivity(BUTTON_MINUS,false);
            SetButtonActivity(BUTTON_MACHINES,true);
            SetButtonActivity(BUTTON_TIME_AND_SCORE,true);
        } else if (Buttons.Button[BUTTON_PLUS].bPressed) {
            if (Ed.uMaxYamExplosionIndex < (EMERALD_MAX_YAM_EXPLOSIONS - 1)) {
                Ed.uMaxYamExplosionIndex++;
            }
        } else if (Buttons.Button[BUTTON_MINUS].bPressed) {
            if (Ed.uMaxYamExplosionIndex > 0) {
                Ed.uMaxYamExplosionIndex--;
            }
        } else if (Buttons.Button[BUTTON_MACHINES].bPressed) {
            Ed.uMenuState = MENUSTATE_MACHINES;
            SetPanelElements(Ed.uMenuState);
            SetButtonActivity(BUTTON_RETURN_TO_LEVEL,true);
            SetButtonActivity(BUTTON_MACHINES,false);
            SetButtonActivity(BUTTON_TIME_AND_SCORE,false);
            SetButtonActivity(BUTTON_YAMS,false);
            SetButtonActivity(BUTTON_SAVE_AND_PLAY,false);
        } else if (Buttons.Button[BUTTON_TIME_AND_SCORE].bPressed) {
            Ed.uMenuState = MENUSTATE_TIME_AND_SCORES;
            SetButtonActivity(BUTTON_RETURN_TO_LEVEL,true);
            SetButtonActivity(BUTTON_MACHINES,false);
            SetButtonActivity(BUTTON_TIME_AND_SCORE,false);
            SetButtonActivity(BUTTON_YAMS,false);
            SetButtonActivity(BUTTON_SAVE_AND_PLAY,false);
        }
        SDL_RenderPresent(pRenderer);   // Renderer anzeigen, lässt Hauptschleife mit ~ 60 Hz (Bild-Wiederholfrequenz) laufen
        SDL_RenderClear(pRenderer);     // Renderer für nächstes Frame löschen
        Ed.uFrameCounter++;
        Playfield.uFrameCounter++;      // Einige Elemente benötigen den Playfield-Counter in der Funktion GetTextureIndexByElement();
        SDL_Delay(5);
    }
    FreeButtons();
    SAFE_FREE(Ed.pLevel);
    WaitNoKey();
    return 0;
}
