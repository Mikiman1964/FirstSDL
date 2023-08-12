#include <SDL2/SDL.h>
#include <ctype.h>
#include <stdio.h>
#include "buttons_checkboxes.h"
#include "config.h"
#include "editor.h"
#include "EmeraldMine.h"
#include "EmeraldMineMainMenu.h"
#include "FillLevelArea.h"
#include "GetTextureIndexByElement.h"
#include "KeyboardMouse.h"
#include "levelconverter.h"
#include "levelgroupoperation.h"
#include "loadlevel.h"
#include "man.h"
#include "md5.h"
#include "miniz.h"
#include "modplay.h"
#include "mystd.h"
#include "mySDL.h"
#include "scroller.h"

ED Ed;
CLIPBOARD Clipboard;
extern MANKEY ManKey;
extern PLAYFIELD Playfield;
extern INPUTSTATES InputStates;
extern SDL_DisplayMode ge_DisplayMode;
extern CONFIG Config;
extern LEVELGROUP SelectedLevelgroup;
extern MAINMENU MainMenu;
extern IMPORTLEVEL ImportLevel;
extern uint32_t ge_uXoffs;             // X-Offset für die Zentrierung von Elementen
extern uint32_t ge_uYoffs;             // X-Offset für die Zentrierung von Elementen
extern LEVELFILESLIST Dc3LevelFileList[EMERALD_MAX_IMPORTFILES];


uint8_t g_PanelColorPatterns[] = {
//                           R    G    B
                            0x42,0x21,0x00,         // braun 1, dunkel
                            0x6C,0x36,0x00,         // braun 2
                            0x9D,0x4F,0x00,         // braun 3
                            0xDF,0x70,0x00,         // braun 4, hell
                            };

char ge_szElementNames[][64] =
                         {"NO ELEMENT",                         // 0X00
                          "STONE",                              // 0X01
                          "SAPPHIRE",                           // 0X02
                          "PERL",                               // 0X03
                          "MOLE UP",                            // 0X04
                          "MOLE RIGHT",                         // 0X05
                          "MOLE DOWN",                          // 0X06
                          "MOLE LEFT",                          // 0X07
                          "MINE UP",                            // 0X08
                          "MINE RIGHT",                         // 0X09
                          "MINE DOWN",                          // 0X0A
                          "MINE LEFT",                          // 0X0B
                          "LIGHT BARRIER SWITCH RED",           // 0X0C
                          "LIGHT BARRIER SWITCH GREEN",         // 0X0D
                          "LIGHT BARRIER SWITCH BLUE",          // 0X0E
                          "LIGHT BARRIER SWITCH YELLOW",        // 0X0F
                          "BOMB",                               // 0X10
                          "CRYSTAL",                            // 0X11
                          "EMERALD",                            // 0X12
                          "RUBY",                               // 0X13
                          "BEETLE UP",                          // 0X14
                          "BEETLE RIGHT",                       // 0X15
                          "BEETLE DOWN",                        // 0X16
                          "BEETLE LEFT",                        // 0X17
                          "REPLICATOR RED, TOP LEFT",           // 0X18
                          "REPLICATOR RED, TOP MIDDLE",         // 0X19
                          "REPLICATOR RED, TOP RIGHT",          // 0X1A
                          "REPLICATOR RED, BOTTOM LEFT",        // 0X1B
                          "REPLICATOR RED, BOTTON RIGHT",       // 0X1C
                          "REPLICATOR RED, SWITCH",             // 0X1D
                          "REPLICATOR YELLOW, TOP LEFT",        // 0X1E
                          "REPLICATOR YELLOW, TOP MIDDLE",      // 0X1F
                          "REPLICATOR YELLOW, TOP RIGHT",       // 0X20
                          "REPLICATOR YELLOW, BOTTOM LEFT",     // 0X21
                          "REPLICATOR YELLOW, BOTTON RIGHT",    // 0X22
                          "REPLICATOR YELLOW, SWITCH",          // 0X23
                          "TIME COIN",                          // 0X24
                          "NUT",                                // 0X25
                          "ALIEN",                              // 0X26
                          "YAM",                                // 0X27
                          "FIXED MINE",                         // 0X28
                          "MAGIC WALL, SWITCH",                 // 0X29
                          "MAGIC WALL, STEEL",                  // 0X2A
                          "SAND FROM MOLE, INTERNAL ELEMENT",   // 0X2B
                          "GREEN CHEESE, DISAPPEAR, INT. ELEM.",// 0X2C
                          "LIGHT SWITCH",                       // 0X2D
                          "REPLICATOR GREEN, TOP LEFT",         // 0X2E
                          "REPLICATOR GREEN, TOP MIDDLE",       // 0X2F
                          "REPLICATOR GREEN, TOP RIGHT",        // 0X30
                          "REPLICATOR GREEN, BOTTOM LEFT",      // 0X31
                          "REPLICATOR GREEN, BOTTON RIGHT",     // 0X32
                          "REPLICATOR GREEN, SWITCH",           // 0X33
                          "REPLICATOR BLUE, TOP LEFT",          // 0X34
                          "REPLICATOR BLUE, TOP MIDDLE",        // 0X35
                          "REPLICATOR BLUE, TOP RIGHT",         // 0X36
                          "REPLICATOR BLUE, BOTTOM LEFT",       // 0X37
                          "REPLICATOR BLUE, BOTTON RIGHT",      // 0X38
                          "REPLICATOR BLUE, SWITCH",            // 0X39
                          "DYNAMITE, OFF",                      // 0X3A
                          "DYNAMITE, ON",                       // 0X3B
                          "DOOR, WOOD, RED",                    // 0X3C
                          "DOOR, WOOD, YELLOW",                 // 0X3D
                          "DOOR, WOOD, BLUE",                   // 0X3E
                          "DOOR, WOOD, GREEN",                  // 0X3F
                          "MAN",                                // 0X40
                          "MESSAGE 1",                          // 0X41
                          "MESSAGE 2",                          // 0X42
                          "MESSAGE 3",                          // 0X43
                          "MESSAGE 4",                          // 0X44
                          "MESSAGE 5",                          // 0X45
                          "MESSAGE 6",                          // 0X46
                          "MESSAGE 7",                          // 0X47
                          "MESSAGE 8",                          // 0X48
                          "LIGHT BARRIER, RED, UP",             // 0X49
                          "LIGHT BARRIER, RED, DOWN",           // 0X4A
                          "LIGHT BARRIER, RED, LEFT",           // 0X4B
                          "LIGHT BARRIER, RED, RIGHT",          // 0X4C
                          "LIGHT BARRIER, GREEN, UP",           // 0X4D
                          "LIGHT BARRIER, GREEN, DOWN",         // 0X4E
                          "LIGHT BARRIER, GREEN, LEFT",         // 0X4F
                          "LIGHT BARRIER, GREEN, RIGHT",        // 0X50
                          "LIGHT BARRIER, BLUE, UP",            // 0X51
                          "LIGHT BARRIER, BLUE, DOWN",          // 0X52
                          "LIGHT BARRIER, BLUE, LEFT",          // 0X53
                          "LIGHT BARRIER, BLUE, RIGHT",         // 0X54
                          "LIGHT BARRIER, YELLOW, UP",          // 0X55
                          "LIGHT BARRIER, YELLOW, DOWN",        // 0X56
                          "LIGHT BARRIER, YELLOW, LEFT",        // 0X57
                          "LIGHT BARRIER, YELLOW, RIGHT",       // 0X58
                          "BEAM, RED, VERTICAL",                // 0X59
                          "BEAM, RED, HORIZONTAL",              // 0X5A
                          "BEAM, GREEN, VERTICAL",              // 0X5B
                          "BEAM, GREEN, HORIZONTAL",            // 0X5C
                          "BEAM, BLUE, VERTICAL",               // 0X5D
                          "BEAM, BLUE, HORIZONTAL",             // 0X5E
                          "BEAM, YELLOW, VERTICAL",             // 0X5F
                          "BEAM, YELLOW, HORIZONTAL",           // 0X60
                          "BEAM, CROSS, INTERNAL ELEMENT",      // 0X61
                          "DOOR, EMERALD",                      // 0X62
                          "DOOR, MULTICOLOR",                   // 0X63
                          "DOOR, ONLY UP, STEEL",               // 0X64
                          "DOOR, ONLY DOWN, STEEL",             // 0X65
                          "DOOR, ONLY LEFT, STEEL",             // 0X66
                          "DOOR, ONLY RIGHT, STEEL",            // 0X67
                          "SWITCH, TIMEDOOR",                   // 0X68
                          "TIMEDOOR",                           // 0X69
                          "SWITCHED DOOR, OPEN",                // 0X6A
                          "SWITCHED DOOR, CLOSED",              // 0X6B
                          "SWITCH, SWITCHED DOOR",              // 0X6C
                          "ENDDOOR, NOT READY, STEEL",          // 0X6D
                          "ENDDOOR, READY, STEEL",              // 0X6E
                          "KEY, WHITE",                         // 0X6F
                          "DOOR, WHITE",                        // 0X70
                          "DOOR, SECRET, WHITE",                // 0X71
                          "DOOR, WOOD, WHITE",                  // 0X72
                          "KEY, GENERAL",                       // 0X73
                          "MEGABOMB",                           // 0X74
                          "WALL, SOFT, CORNERED, WITH EMERALD", // 0X75
                          "WALL, SOFT, CORNERED, WITH RUBY",    // 0X76
                          "WALL, SOFT, CORNERED, WITH SAPPHIRE",// 0X77
                          "WALL, SOFT, CORNERED, WITH PERL",    // 0X78
                          "ACID POOL DESTROY, INTERNAL ELEMENT",// 0X79
                          "ACID POOL, TOP LEFT",                // 0X7A
                          "ACID POOL, TOP MIDDLE",              // 0X7B
                          "ACID POOL, TOP RIGHT",               // 0X7C
                          "ACID POOL, BOTTOM LEFT",             // 0X7D
                          "ACID POOL, BOTTOM MIDDLE",           // 0X7E
                          "ACID POOL, BOTTOM RIGHT",            // 0X7F
                          "SPACE",                              // 0X80
                          "WALL, SOFT, ROUND",                  // 0X81
                          "SAND",                               // 0X82
                          "STEEL",                              // 0X83
                          "WALL, SOFT, CORNERED",               // 0X84
                          "KEY, RED",                           // 0X85
                          "KEY, YELLOW",                        // 0X86
                          "KEY, BLUE",                          // 0X87
                          "KEY, GREEN",                         // 0X88
                          "DOOR, RED",                          // 0X89
                          "DOOR, YELLOW",                       // 0X8A
                          "DOOR, BLUE",                         // 0X8B
                          "DOOR, GREEN",                        // 0X8C
                          "HAMMER",                             // 0X8D
                          "DOOR, SECRET, RED",                  // 0X8E
                          "DOOR, SECRET, YELLOW",               // 0X8F
                          "DOOR, SECRET, BLUE",                 // 0X90
                          "DOOR, SECRET, GREEN",                // 0X91
                          "MAGIC WALL",                         // 0X92
                          "WHEEL",                              // 0X93
                          "SWAMP",                              // 0X94
                          "SWAMP WITH STONE",                   // 0X95
                          "STONE S.IN SWAMP, INTERNAL ELEMENT", // 0X96
                          "STONE S.OUT SWAMP, INTERNAL ELEMENT",// 0X97
                          "ENDDOOR, NOT READY",                 // 0X98
                          "GREEN DROP COMES, INTERNAL ELEMENT", // 0X99
                          "GREEN DROP",                         // 0X9A
                          "GREEN CHEESE",                       // 0X9B
                          "EXPL.TO.ELEMENT, INTERNAL ELEMENT",  // 0X9C
                          "EXPL.TO ELEMENT, INTERNAL ELEMENT",  // 0X9D
                          "WALL, SOFT, ROUND, WITH PIKE",       // 0X9E
                          "ENDDOOR, READY",                     // 0X9F
                          "STEEL, ROUND, WITH PIKE",            // 0XA0
                          "STEEL, ROUND",                       // 0XA1
                          "STEEL, WARNING",                     // 0XA2
                          "STEEL, BIOHAZARD",                   // 0XA3
                          "STEEL, DEADEND",                     // 0XA4
                          "STEEL, STOP",                        // 0XA5
                          "STEEL, PARKING",                     // 0XA6
                          "STEEL, FORBIDDEN",                   // 0XA7
                          "STEEL, EXIT",                        // 0XA8
                          "STEEL, RADIOACTIVE",                 // 0XA9
                          "STEEL, EXPLOSION",                   // 0XAA
                          "STEEL, ACID",                        // 0XAB
                          "STEEL, NOT ROUND",                   // 0XAC
                          "WALL, SOFT, NOT ROUND",              // 0XAD
                          "STEEL, MARKER LEFT/UP",              // 0XAE
                          "STEEL, MARKER UP",                   // 0XAF
                          "STEEL, MARKER RIGHT/UP",             // 0XB0
                          "STEEL, MARKER LEFT",                 // 0XB1
                          "STEEL, MARKER RIGHT",                // 0XB2
                          "STEEL, MARKER LEFT/BOTTOM",          // 0XB3
                          "STEEL, MARKER BOTTOM",               // 0XB4
                          "STEEL, MARKER RIGHT/BOTTOM",         // 0XB5
                          "STEEL, HEART",                       // 0XB6
                          "STEEL, PLAYER HEAD",                 // 0XB7
                          "STEEL, NO ENTRY",                    // 0XB8
                          "STEEL, GIVE WAY",                    // 0XB9
                          "STEEL, YING YANG",                   // 0XBA
                          "STEEL, WHEELCHAIR",                  // 0XBB
                          "STEEL, ARROW DOWN",                  // 0XBC
                          "STEEL, ARROW UP",                    // 0XBD
                          "STEEL, ARROW LEFT",                  // 0XBE
                          "STEEL, ARROW RIGHT",                 // 0XBF
                          "STEEL, INVISIBLE",                   // 0XC0
                          "WALL, INVISIBLE",                    // 0XC1
                          "WALL, WITH CRYSTAL",                 // 0XC2
                          "WALL, WITH RED KEY ",                // 0XC3
                          "WALL, WITH GREEN KEY",               // 0XC4
                          "WALL, WITH BLUE KEY",                // 0XC5
                          "WALL, WITH YELLOW KEY",              // 0XC6
                          "WALL, WITH WHITE KEY",               // 0XC7
                          "WALL, WITH GENERAL KEY",             // 0XC8
                          "WALL, WITH BOMB",                    // 0XC9
                          "WALL, WITH MEGABOMB",                // 0XCA
                          "WALL, WITH STONE",                   // 0XCB
                          "WALL, WITH NUT",                     // 0XCC
                          "WALL, WITH WHEEL",                   // 0XCD
                          "WALL, WITH DYNAMITE",                // 0XCE
                          "WALL, WITH ENDDOOR",                 // 0XCF
                          "WALL, WITH ENDDOOR, READY",          // 0XD0
                          "WALL, WITH MINE UP",                 // 0XD1
                          "WALL, WITH MOLE",                    // 0XD2
                          "WALL, WITH GREEN CHEESE",            // 0XD3
                          "WALL, WITH BEETLE UP",               // 0XD4
                          "WALL, WITH YAM",                     // 0XD5
                          "WALL, WITH ALIEN",                   // 0XD6
                          "NO ELEMENT",                         // 0XD7
                          "NO ELEMENT",                         // 0XD8
                          "NO ELEMENT",                         // 0XD9
                          "NO ELEMENT",                         // 0XDA
                          "NO ELEMENT",                         // 0XDB
                          "NO ELEMENT",                         // 0XDC
                          "NO ELEMENT",                         // 0XDD
                          "NO ELEMENT",                         // 0XDE
                          "NO ELEMENT",                         // 0XDF
                          "NO ELEMENT",                         // 0XE0
                          "NO ELEMENT",                         // 0XE1
                          "NO ELEMENT",                         // 0XE2
                          "NO ELEMENT",                         // 0XE3
                          "NO ELEMENT",                         // 0XE4
                          "NO ELEMENT",                         // 0XE5
                          "NO ELEMENT",                         // 0XE6
                          "NO ELEMENT",                         // 0XE7
                          "NO ELEMENT",                         // 0XE8
                          "NO ELEMENT",                         // 0XE9
                          "NO ELEMENT",                         // 0XEA
                          "NO ELEMENT",                         // 0XEB
                          "NO ELEMENT",                         // 0XEC
                          "NO ELEMENT",                         // 0XED
                          "NO ELEMENT",                         // 0XEE
                          "NO ELEMENT",                         // 0XEF
                          "NO ELEMENT",                         // 0XF0
                          "NO ELEMENT",                         // 0XF1
                          "NO ELEMENT",                         // 0XF2
                          "NO ELEMENT",                         // 0XF3
                          "NO ELEMENT",                         // 0XF4
                          "NO ELEMENT",                         // 0XF5
                          "NO ELEMENT",                         // 0XF6
                          "NO ELEMENT",                         // 0XF7
                          "NO ELEMENT",                         // 0XF8
                          "NO ELEMENT",                         // 0XF9
                          "NO ELEMENT",                         // 0XFA
                          "NO ELEMENT",                         // 0XFB
                          "NO ELEMENT",                         // 0XFC
                          "NO ELEMENT",                         // 0XFD
                          "NO ELEMENT",                         // 0XFE
                          "NO ELEMENT",                         // 0XFF
                          "FONT, EXCLAMATION",                  // 0X100
                          "FONT, ARROW RIGHT",                  // 0X101
                          "FONT, ARROW UP",                     // 0X102
                          "FONT, ARROW DOWN",                   // 0X103
                          "FONT, APOSTROPHE",                   // 0X104
                          "FONT, BRACE OPEN",                   // 0X105
                          "FONT, BRACE CLOSE",                  // 0X106
                          "FONT, COPYRIGHT",                    // 0X107
                          "FONT, PLUS",                         // 0X108
                          "FONT, COMMA",                        // 0X109
                          "FONT, MINUS",                        // 0X10A
                          "FONT, POINT",                        // 0X10B
                          "FONT, SLASH",                        // 0X10C
                          "FONT, 0",                            // 0X10D
                          "FONT, 1",                            // 0X10E
                          "FONT, 2",                            // 0X10F
                          "FONT, 3",                            // 0X110
                          "FONT, 4",                            // 0X111
                          "FONT, 5",                            // 0X112
                          "FONT, 6",                            // 0X113
                          "FONT, 7",                            // 0X114
                          "FONT, 8",                            // 0X115
                          "FONT, 9",                            // 0X116
                          "FONT, DOUBLE POINT",                 // 0X117
                          "FONT, PLATE",                        // 0X118
                          "FONT, ARROW LEFT",                   // 0X119
                          "FONT, PAFF",                         // 0X11A
                          "FONT, QUESTION MARK",                // 0X11B
                          "FONT, A",                            // 0X11C
                          "FONT, B",                            // 0X11D
                          "FONT, C",                            // 0X11E
                          "FONT, D",                            // 0X11F
                          "FONT, E",                            // 0X120
                          "FONT, F",                            // 0X121
                          "FONT, G",                            // 0X122
                          "FONT, H",                            // 0X123
                          "FONT, I",                            // 0X124
                          "FONT, J",                            // 0X125
                          "FONT, K",                            // 0X126
                          "FONT, L",                            // 0X127
                          "FONT, M",                            // 0X128
                          "FONT, N",                            // 0X129
                          "FONT, O",                            // 0X12A
                          "FONT, P",                            // 0X12B
                          "FONT, Q",                            // 0X12C
                          "FONT, R",                            // 0X12D
                          "FONT, S",                            // 0X12E
                          "FONT, T",                            // 0X12F
                          "FONT, U",                            // 0X130
                          "FONT, V",                            // 0X131
                          "FONT, W",                            // 0X132
                          "FONT, X",                            // 0X133
                          "FONT, Y",                            // 0X134
                          "FONT, Z",                            // 0X135
                          "FONT, AE",                           // 0X136
                          "FONT, OE",                           // 0X137
                          "FONT, UE",                           // 0X138
                          "FONT, STEEL, EXCLAMATION",           // 0X139
                          "FONT, STEEL, ARROW RIGHT",           // 0X13A
                          "FONT, STEEL, ARROW UP",              // 0X13B
                          "FONT, STEEL, ARROW DOWN",            // 0X13C
                          "FONT, STEEL, APOSTROPHE",            // 0X13D
                          "FONT, STEEL, BRACE OPEN",            // 0X13E
                          "FONT, STEEL, BRACE CLOSE",           // 0X13F
                          "FONT, STEEL, COPYRIGHT",             // 0X140
                          "FONT, STEEL, PLUS",                  // 0X141
                          "FONT, STEEL, COMMA",                 // 0X142
                          "FONT, STEEL, MINUS",                 // 0X143
                          "FONT, STEEL, POINT",                 // 0X144
                          "FONT, STEEL, SLASH",                 // 0X145
                          "FONT, STEEL, 0",                     // 0X146
                          "FONT, STEEL, 1",                     // 0X147
                          "FONT, STEEL, 2",                     // 0X148
                          "FONT, STEEL, 3",                     // 0X149
                          "FONT, STEEL, 4",                     // 0X14A
                          "FONT, STEEL, 5",                     // 0X14B
                          "FONT, STEEL, 6",                     // 0X14C
                          "FONT, STEEL, 7",                     // 0X14D
                          "FONT, STEEL, 8",                     // 0X14E
                          "FONT, STEEL, 9",                     // 0X14F
                          "FONT, STEEL, DOUBLE POINT",          // 0X150
                          "FONT, STEEL, PLATE",                 // 0X151
                          "FONT, STEEL, ARROW LEFT",            // 0X152
                          "FONT, STEEL, QUESTION MARK",         // 0X153
                          "FONT, STEEL, A",                     // 0X154
                          "FONT, STEEL, B",                     // 0X155
                          "FONT, STEEL, C",                     // 0X156
                          "FONT, STEEL, D",                     // 0X157
                          "FONT, STEEL, E",                     // 0X158
                          "FONT, STEEL, F",                     // 0X159
                          "FONT, STEEL, G",                     // 0X15A
                          "FONT, STEEL, H",                     // 0X15B
                          "FONT, STEEL, I",                     // 0X15C
                          "FONT, STEEL, J",                     // 0X15D
                          "FONT, STEEL, K",                     // 0X15E
                          "FONT, STEEL, L",                     // 0X15F
                          "FONT, STEEL, M",                     // 0X160
                          "FONT, STEEL, N",                     // 0X161
                          "FONT, STEEL, O",                     // 0X162
                          "FONT, STEEL, P",                     // 0X163
                          "FONT, STEEL, Q",                     // 0X164
                          "FONT, STEEL, R",                     // 0X165
                          "FONT, STEEL, S",                     // 0X166
                          "FONT, STEEL, T",                     // 0X167
                          "FONT, STEEL, U",                     // 0X168
                          "FONT, STEEL, V",                     // 0X169
                          "FONT, STEEL, W",                     // 0X16A
                          "FONT, STEEL, X",                     // 0X16B
                          "FONT, STEEL, Y",                     // 0X16C
                          "FONT, STEEL, Z",                     // 0X16D
                          "FONT, STEEL, AE",                    // 0X16E
                          "FONT, STEEL, OE",                    // 0X16F
                          "FONT, STEEL, UE",                    // 0X170
                          "FONT, GREEN, EXCLAMATION",           // 0X171
                          "FONT, GREEN, ARROW RIGHT",           // 0X172
                          "FONT, GREEN, ARROW UP",              // 0X173
                          "FONT, GREEN, ARROW DOWN",            // 0X174
                          "FONT, GREEN, APOSTROPHE",            // 0X175
                          "FONT, GREEN, BRACE OPEN",            // 0X176
                          "FONT, GREEN, BRACE CLOSE",           // 0X177
                          "FONT, GREEN, COPYRIGHT",             // 0X178
                          "FONT, GREEN, PLUS",                  // 0X179
                          "FONT, GREEN, COMMA",                 // 0X17A
                          "FONT, GREEN, MINUS",                 // 0X17B
                          "FONT, GREEN, POINT",                 // 0X17C
                          "FONT, GREEN, SLASH",                 // 0X17D
                          "FONT, GREEN, 0",                     // 0X17E
                          "FONT, GREEN, 1",                     // 0X17F
                          "FONT, GREEN, 2",                     // 0X180
                          "FONT, GREEN, 3",                     // 0X181
                          "FONT, GREEN, 4",                     // 0X182
                          "FONT, GREEN, 5",                     // 0X183
                          "FONT, GREEN, 6",                     // 0X184
                          "FONT, GREEN, 7",                     // 0X185
                          "FONT, GREEN, 8",                     // 0X186
                          "FONT, GREEN, 9",                     // 0X187
                          "FONT, GREEN, DOUBLE POINT",          // 0X188
                          "FONT, GREEN, PLATE",                 // 0X189
                          "FONT, GREEN, ARROW LEFT",            // 0X18A
                          "FONT, GREEN, QUESTION MARK",         // 0X18B
                          "FONT, GREEN, A",                     // 0X18C
                          "FONT, GREEN, B",                     // 0X18D
                          "FONT, GREEN, C",                     // 0X18E
                          "FONT, GREEN, D",                     // 0X18F
                          "FONT, GREEN, E",                     // 0X190
                          "FONT, GREEN, F",                     // 0X191
                          "FONT, GREEN, G",                     // 0X192
                          "FONT, GREEN, H",                     // 0X193
                          "FONT, GREEN, I",                     // 0X194
                          "FONT, GREEN, J",                     // 0X195
                          "FONT, GREEN, K",                     // 0X196
                          "FONT, GREEN, L",                     // 0X197
                          "FONT, GREEN, M",                     // 0X198
                          "FONT, GREEN, N",                     // 0X199
                          "FONT, GREEN, O",                     // 0X19A
                          "FONT, GREEN, P",                     // 0X19B
                          "FONT, GREEN, Q",                     // 0X19C
                          "FONT, GREEN, R",                     // 0X19D
                          "FONT, GREEN, S",                     // 0X19E
                          "FONT, GREEN, T",                     // 0X19F
                          "FONT, GREEN, U",                     // 0X1A0
                          "FONT, GREEN, V",                     // 0X1A1
                          "FONT, GREEN, W",                     // 0X1A2
                          "FONT, GREEN, X",                     // 0X1A3
                          "FONT, GREEN, Y",                     // 0X1A4
                          "FONT, GREEN, Z",                     // 0X1A5
                          "FONT, GREEN, AE",                    // 0X1A6
                          "FONT, GREEN, OE",                    // 0X1A7
                          "FONT, GREEN, UE",                    // 0X1A8
                          "FONT, STEEL, GREEN, EXCLAMATION",    // 0X1A9
                          "FONT, STEEL, GREEN, ARROW RIGHT",    // 0X1AA
                          "FONT, STEEL, GREEN, ARROW UP",       // 0X1AB
                          "FONT, STEEL, GREEN, ARROW DOWN",     // 0X1AC
                          "FONT, STEEL, GREEN, APOSTROPHE",     // 0X1AD
                          "FONT, STEEL, GREEN, BRACE OPEN",     // 0X1AE
                          "FONT, STEEL, GREEN, BRACE CLOSE",    // 0X1AF
                          "FONT, STEEL, GREEN, COPYRIGHT",      // 0X1B0
                          "FONT, STEEL, GREEN, PLUS",           // 0X1B1
                          "FONT, STEEL, GREEN, COMMA",          // 0X1B2
                          "FONT, STEEL, GREEN, MINUS",          // 0X1B3
                          "FONT, STEEL, GREEN, POINT",          // 0X1B4
                          "FONT, STEEL, GREEN, SLASH",          // 0X1B5
                          "FONT, STEEL, GREEN, 0",              // 0X1B6
                          "FONT, STEEL, GREEN, 1",              // 0X1B7
                          "FONT, STEEL, GREEN, 2",              // 0X1B8
                          "FONT, STEEL, GREEN, 3",              // 0X1B9
                          "FONT, STEEL, GREEN, 4",              // 0X1BA
                          "FONT, STEEL, GREEN, 5",              // 0X1BB
                          "FONT, STEEL, GREEN, 6",              // 0X1BC
                          "FONT, STEEL, GREEN, 7",              // 0X1BD
                          "FONT, STEEL, GREEN, 8",              // 0X1BE
                          "FONT, STEEL, GREEN, 9",              // 0X1BF
                          "FONT, STEEL, GREEN, DOUBLE POINT",   // 0X1C0
                          "FONT, STEEL, GREEN, PLATE",          // 0X1C1
                          "FONT, STEEL, GREEN, ARROW LEFT",     // 0X1C2
                          "FONT, STEEL, GREEN, QUESTION MARK",  // 0X1C3
                          "FONT, STEEL, GREEN, A",              // 0X1C4
                          "FONT, STEEL, GREEN, B",              // 0X1C5
                          "FONT, STEEL, GREEN, C",              // 0X1C6
                          "FONT, STEEL, GREEN, D",              // 0X1C7
                          "FONT, STEEL, GREEN, E",              // 0X1C8
                          "FONT, STEEL, GREEN, F",              // 0X1C9
                          "FONT, STEEL, GREEN, G",              // 0X1CA
                          "FONT, STEEL, GREEN, H",              // 0X1CB
                          "FONT, STEEL, GREEN, I",              // 0X1CC
                          "FONT, STEEL, GREEN, J",              // 0X1CD
                          "FONT, STEEL, GREEN, K",              // 0X1CE
                          "FONT, STEEL, GREEN, L",              // 0X1CF
                          "FONT, STEEL, GREEN, M",              // 0X1D0
                          "FONT, STEEL, GREEN, N",              // 0X1D1
                          "FONT, STEEL, GREEN, O",              // 0X1D2
                          "FONT, STEEL, GREEN, P",              // 0X1D3
                          "FONT, STEEL, GREEN, Q",              // 0X1D4
                          "FONT, STEEL, GREEN, R",              // 0X1D5
                          "FONT, STEEL, GREEN, S",              // 0X1D6
                          "FONT, STEEL, GREEN, T",              // 0X1D7
                          "FONT, STEEL, GREEN, U",              // 0X1D8
                          "FONT, STEEL, GREEN, V",              // 0X1D9
                          "FONT, STEEL, GREEN, W",              // 0X1DA
                          "FONT, STEEL, GREEN, X",              // 0X1DB
                          "FONT, STEEL, GREEN, Y",              // 0X1DC
                          "FONT, STEEL, GREEN, Z",              // 0X1DD
                          "FONT, STEEL, GREEN, AE",             // 0X1DE
                          "FONT, STEEL, GREEN, OE",             // 0X1DF
                          "FONT, STEEL, GREEN, UE",             // 0X1E0
                          "EXPL. CENTRAL, INTERNAL ELEMENT",    // 0X1E1
                          "EXPL. MEGACENTRAL, INT. ELEMENT",    // 0X1E2
                          "EXPL. CENTRAL BEETLE, INT. ELEMENT", // 0X1E3
                          "MAN DIES, INTERNAL ELEMENT",         // 0X1E4
                          "ALIEN KILLS MAN, INTERNAL ELEMENT",  // 0X1E5
                          "YAM KILLS MAN, INTERNAL ELEMENT",    // 0X1E6
                          "WALL, WITH TIME COIN",               // 0X1E7
                          "STEEL, TRASHCAN",                    // 0X1E8
                          "STEEL, JOYSTICK",                    // 0X1E9
                          "STEEL, EDIT LEVEL",                  // 0X1EA
                          "STEEL, MOVE LEVEL",                  // 0X1EB
                          "STEEL, ADD LEVEL",                   // 0X1EC
                          "STEEL, COPY LEVEL",                  // 0X1ED
                          "STEEL, MSDOS IMPORT",                // 0X1EE
                          "STEEL, DC3 IMPORT",                  // 0X1EF
                          "STEEL, RENAME LEVELGROUP",           // 0X1F0
                          "STEEL, PASSWORD",                    // 0X1F1
                          "GRASS",                              // 0X1F2
                          "GRASS COMES, INTERNAL ELEMENT",      // 0X1F3
                          "SANDMINE",                           // 0X1F4
                          "SAND, INVISIBLE",                    // 0X1F5
                          "CONVEYOR BELT, RED",                 // 0X1F6
                          "CONVEYOR BELT SWITCH, RED",          // 0X1F7
                          "CONVEYOR BELT, GREEN",               // 0X1F8
                          "CONVEYOR BELT SWITCH, GREEN",        // 0X1F9
                          "CONVEYOR BELT, BLUE",                // 0X1FA
                          "CONVEYOR BELT SWITCH, BLUE",         // 0X1FB
                          "CONVEYOR BELT, YELLOW",              // 0X1FC
                          "CONVEYOR BELT SWITCH, YELLOW",       // 0X1FD
                          "EDITOR MESSAGE 1, INTERNAL ELEMENT", // 0X1FE
                          "EDITOR MESSAGE 2, INTERNAL ELEMENT", // 0X1FF
                          "EDITOR MESSAGE 3, INTERNAL ELEMENT", // 0X200
                          "EDITOR MESSAGE 4, INTERNAL ELEMENT", // 0X201
                          "STEEL, GROW LEFT",                   // 0X202
                          "STEEL, GROW RIGHT",                  // 0X203
                          "STEEL, GROW UP",                     // 0X204
                          "STEEL, GROW DOWN",                   // 0X205
                          "STEEL, GROW LEFT AND RIGHT",         // 0X206
                          "STEEL, GROW UP AND DOWN",            // 0X207
                          "STEEL, GROW ALL DIRECTIONS",         // 0X208
                          "STEEL, GROWING LEFT, INT. ELEMENT",  // 0X209
                          "STEEL, GROWING RIGHT, INT. ELEMENT", // 0X20A
                          "STEEL, GROWING UP, INT. ELEMENT",    // 0X20B
                          "STEEL, GROWING DOWN, INT. ELEMENT",  // 0X20C
                          "WALL, GROW LEFT",                    // 0X20D
                          "WALL, GROW RIGHT",                   // 0X20E
                          "WALL, GROW UP",                      // 0X20F
                          "WALL, GROW DOWN",                    // 0X210
                          "WALL, GROW LEFT AND RIGHT",          // 0X211
                          "WALL, GROW UP AND DOWN",             // 0X212
                          "WALL, GROW ALL DIRECTIONS",          // 0X213
                          "WALL, GROWING LEFT, INT. ELEMENT",   // 0X214
                          "WALL, GROWING RIGHT, INT. ELEMENT",  // 0X215
                          "WALL, GROWING UP, INT. ELEMENT",     // 0X216
                          "WALL, GROWING DOWN, INT. ELEMENT",   // 0X217
                          "DOOR, ONLY UP, WALL",                // 0X218
                          "DOOR, ONLY DOWN, WALL",              // 0X219
                          "DOOR, ONLY LEFT, WALL",              // 0X21A
                          "DOOR, ONLY RIGHT, WALL",             // 0X21B
                          "TELEPORTER, RED",                    // 0X21C
                          "TELEPORTER, YELLOW",                 // 0X21D
                          "TELEPORTER, GREEN",                  // 0X21E
                          "TELEPORTER, BLUE",                   // 0X21F
                          "SWITCH, REMOTE BOMB, UP",            // 0X220
                          "SWITCH, REMOTE BOMB, DOWN",          // 0X221
                          "SWITCH, REMOTE BOMB, LEFT",          // 0X222
                          "SWITCH, REMOTE BOMB, RIGHT",         // 0X223
                          "SWITCH, REMOTE BOMB, IGNITION",      // 0X224
                          "REMOTE BOMB",                        // 0X225
                          "STEEL, MODERN, LEFT, END",           // 0X226
                          "STEEL, MODERN, LEFT/RIGHT",          // 0X227
                          "STEEL, MODERN, RIGHT, END",          // 0X228
                          "STEEL, MODERN, UP, END",             // 0X229
                          "STEEL, MODERN, UP/DOWN",             // 0X22A
                          "STEEL, MODERN, DOWN, END",           // 0X22B
                          "STEEL, MODERN, MIDDLE",              // 0X22C
                          };


// Level-Elemente, die aktuell angezeigt werden
uint16_t g_PanelElements[MAX_PANEL_ELEMENTS + 1] = {
                            EMERALD_INVALID
                            };

// Level-Elemente, die im Hauptmenü verfügbar sind
uint16_t g_PanelElementsMain[MAX_PANEL_ELEMENTS + 1] = {
                            EMERALD_DOOR_RED,EMERALD_DOOR_YELLOW,EMERALD_DOOR_GREEN,EMERALD_DOOR_BLUE,EMERALD_KEY_RED,EMERALD_KEY_YELLOW,EMERALD_KEY_GREEN,EMERALD_KEY_BLUE,
                            EMERALD_DOOR_GREY_RED,EMERALD_DOOR_GREY_YELLOW,EMERALD_DOOR_GREY_GREEN,EMERALD_DOOR_GREY_BLUE,EMERALD_DOOR_RED_WOOD,EMERALD_DOOR_YELLOW_WOOD,EMERALD_DOOR_GREEN_WOOD,EMERALD_DOOR_BLUE_WOOD,
                            EMERALD_DOOR_WHITE,EMERALD_DOOR_GREY_WHITE,EMERALD_DOOR_WHITE_WOOD,EMERALD_KEY_WHITE,EMERALD_KEY_GENERAL,EMERALD_DOOR_MULTICOLOR,EMERALD_DOOR_EMERALD,EMERALD_DOOR_TIME,
                            EMERALD_DOOR_END_NOT_READY,EMERALD_DOOR_END_READY,EMERALD_DOOR_END_NOT_READY_STEEL,EMERALD_DOOR_END_READY_STEEL,EMERALD_SWITCHDOOR_CLOSED,EMERALD_SWITCHDOOR_OPEN,EMERALD_SWITCH_SWITCHDOOR,EMERALD_WHEEL_TIMEDOOR,
                            EMERALD_DOOR_ONLY_LEFT_STEEL,EMERALD_DOOR_ONLY_DOWN_STEEL,EMERALD_DOOR_ONLY_RIGHT_STEEL,EMERALD_DOOR_ONLY_UP_STEEL,EMERALD_DOOR_ONLY_LEFT_WALL,EMERALD_DOOR_ONLY_DOWN_WALL,EMERALD_DOOR_ONLY_RIGHT_WALL,EMERALD_DOOR_ONLY_UP_WALL,
                            EMERALD_MAGIC_WALL,EMERALD_MAGIC_WALL_STEEL,EMERALD_MAGIC_WALL_SWITCH,EMERALD_LIGHT_SWITCH,EMERALD_TELEPORTER_RED,EMERALD_TELEPORTER_YELLOW,EMERALD_TELEPORTER_GREEN,EMERALD_TELEPORTER_BLUE,
                            EMERALD_MINE_LEFT,EMERALD_MINE_DOWN,EMERALD_MINE_RIGHT,EMERALD_MINE_UP,EMERALD_BEETLE_LEFT,EMERALD_BEETLE_DOWN,EMERALD_BEETLE_RIGHT,EMERALD_BEETLE_UP,
                            EMERALD_MOLE_LEFT,EMERALD_MOLE_DOWN,EMERALD_MOLE_RIGHT,EMERALD_MOLE_UP,EMERALD_YAM,EMERALD_ALIEN,EMERALD_GREEN_CHEESE,EMERALD_GREEN_DROP,
                            EMERALD_MAN,EMERALD_HAMMER,EMERALD_TIME_COIN,EMERALD_DYNAMITE_OFF,EMERALD_DYNAMITE_ON,EMERALD_WHEEL,EMERALD_GRASS,EMERALD_SAND_INVISIBLE,
                            EMERALD_REPLICATOR_RED_TOP_LEFT,EMERALD_REPLICATOR_RED_TOP_MID,EMERALD_REPLICATOR_RED_TOP_RIGHT,EMERALD_REPLICATOR_RED_SWITCH,EMERALD_REPLICATOR_YELLOW_TOP_LEFT,EMERALD_REPLICATOR_YELLOW_TOP_MID,EMERALD_REPLICATOR_YELLOW_TOP_RIGHT,EMERALD_REPLICATOR_YELLOW_SWITCH,
                            EMERALD_REPLICATOR_RED_BOTTOM_LEFT,EMERALD_SPACE,EMERALD_REPLICATOR_RED_BOTTOM_RIGHT,EMERALD_SPACE,EMERALD_REPLICATOR_YELLOW_BOTTOM_LEFT,EMERALD_SPACE,EMERALD_REPLICATOR_YELLOW_BOTTOM_RIGHT,EMERALD_SPACE,
                            EMERALD_REPLICATOR_GREEN_TOP_LEFT,EMERALD_REPLICATOR_GREEN_TOP_MID,EMERALD_REPLICATOR_GREEN_TOP_RIGHT,EMERALD_REPLICATOR_GREEN_SWITCH,EMERALD_REPLICATOR_BLUE_TOP_LEFT,EMERALD_REPLICATOR_BLUE_TOP_MID,EMERALD_REPLICATOR_BLUE_TOP_RIGHT,EMERALD_REPLICATOR_BLUE_SWITCH,
                            EMERALD_REPLICATOR_GREEN_BOTTOM_LEFT,EMERALD_SPACE,EMERALD_REPLICATOR_GREEN_BOTTOM_RIGHT,EMERALD_SPACE,EMERALD_REPLICATOR_BLUE_BOTTOM_LEFT,EMERALD_SPACE,EMERALD_REPLICATOR_BLUE_BOTTOM_RIGHT,EMERALD_SPACE,
                            EMERALD_CONVEYORBELT_RED,EMERALD_CONVEYORBELT_SWITCH_RED,EMERALD_CONVEYORBELT_YELLOW,EMERALD_CONVEYORBELT_SWITCH_YELLOW,EMERALD_CONVEYORBELT_GREEN,EMERALD_CONVEYORBELT_SWITCH_GREEN,EMERALD_CONVEYORBELT_BLUE,EMERALD_CONVEYORBELT_SWITCH_BLUE,
                            EMERALD_LIGHTBARRIER_RED_LEFT,EMERALD_LIGHTBARRIER_RED_DOWN,EMERALD_LIGHTBARRIER_RED_RIGHT,EMERALD_LIGHTBARRIER_RED_UP,EMERALD_LIGHTBARRIER_YELLOW_LEFT,EMERALD_LIGHTBARRIER_YELLOW_DOWN,EMERALD_LIGHTBARRIER_YELLOW_RIGHT,EMERALD_LIGHTBARRIER_YELLOW_UP,
                            EMERALD_LIGHTBARRIER_GREEN_LEFT,EMERALD_LIGHTBARRIER_GREEN_DOWN,EMERALD_LIGHTBARRIER_GREEN_RIGHT,EMERALD_LIGHTBARRIER_GREEN_UP,EMERALD_LIGHTBARRIER_BLUE_LEFT,EMERALD_LIGHTBARRIER_BLUE_DOWN,EMERALD_LIGHTBARRIER_BLUE_RIGHT,EMERALD_LIGHTBARRIER_BLUE_UP,
                            EMERALD_LIGHTBARRIER_RED_SWITCH,EMERALD_LIGHTBARRIER_YELLOW_SWITCH,EMERALD_LIGHTBARRIER_GREEN_SWITCH,EMERALD_LIGHTBARRIER_BLUE_SWITCH,EMERALD_SANDMINE,EMERALD_SAND,EMERALD_SWAMP,EMERALD_SWAMP_STONE,
                            EMERALD_ACIDPOOL_TOP_LEFT,EMERALD_ACIDPOOL_TOP_MID,EMERALD_ACIDPOOL_TOP_RIGHT,EMERALD_NUT,EMERALD_BOMB,EMERALD_MEGABOMB,EMERALD_STANDMINE,EMERALD_STONE,
                            EMERALD_ACIDPOOL_BOTTOM_LEFT,EMERALD_ACIDPOOL_BOTTOM_MID,EMERALD_ACIDPOOL_BOTTOM_RIGHT,EMERALD_EMERALD,EMERALD_RUBY,EMERALD_SAPPHIRE,EMERALD_PERL,EMERALD_CRYSTAL,
                            EMERALD_MESSAGE_1,EMERALD_MESSAGE_2,EMERALD_MESSAGE_3,EMERALD_MESSAGE_4,EMERALD_MESSAGE_5,EMERALD_MESSAGE_6,EMERALD_MESSAGE_7,EMERALD_MESSAGE_8,
                            EMERALD_STEEL,EMERALD_STEEL_ROUND,EMERALD_STEEL_ROUND_PIKE,EMERALD_WALL_CORNERED,EMERALD_WALL_ROUND,EMERALD_WALL_ROUND_PIKE,EMERALD_STEEL_INVISIBLE,EMERALD_WALL_INVISIBLE,
                            EMERALD_STEEL_MARKER_LEFT_UP,EMERALD_STEEL_MARKER_UP,EMERALD_STEEL_MARKER_RIGHT_UP,EMERALD_STEEL_NOT_ROUND,EMERALD_WALL_NOT_ROUND,EMERALD_STEEL_NO_ENTRY,EMERALD_STEEL_GIVE_WAY,EMERALD_STEEL_WARNING,
                            EMERALD_STEEL_MARKER_LEFT,EMERALD_SPACE,EMERALD_STEEL_MARKER_RIGHT,EMERALD_STEEL_DEADEND,EMERALD_STEEL_PARKING,EMERALD_STEEL_WHEELCHAIR,EMERALD_STEEL_STOP,EMERALD_STEEL_FORBIDDEN,
                            EMERALD_STEEL_MARKER_LEFT_BOTTOM,EMERALD_STEEL_MARKER_BOTTOM,EMERALD_STEEL_MARKER_RIGHT_BOTTOM,EMERALD_STEEL_ARROW_DOWN,EMERALD_STEEL_ARROW_UP,EMERALD_STEEL_ARROW_LEFT,EMERALD_STEEL_ARROW_RIGHT,EMERALD_STEEL_EXIT,
                            EMERALD_STEEL_BIOHAZARD,EMERALD_STEEL_RADIOACTIVE,EMERALD_STEEL_EXPLOSION,EMERALD_STEEL_ACID,EMERALD_STEEL_PLAYERHEAD,EMERALD_STEEL_HEART,EMERALD_STEEL_YING,EMERALD_SPACE,
                            EMERALD_WALL_GROW_LEFT,EMERALD_WALL_GROW_DOWN,EMERALD_WALL_GROW_RIGHT,EMERALD_WALL_GROW_UP,EMERALD_WALL_GROW_LEFT_RIGHT,EMERALD_WALL_GROW_UP_DOWN,EMERALD_WALL_GROW_ALL,EMERALD_SPACE,
                            EMERALD_STEEL_GROW_LEFT,EMERALD_STEEL_GROW_DOWN,EMERALD_STEEL_GROW_RIGHT,EMERALD_STEEL_GROW_UP,EMERALD_STEEL_GROW_LEFT_RIGHT,EMERALD_STEEL_GROW_UP_DOWN,EMERALD_STEEL_GROW_ALL,EMERALD_SPACE,
                            EMERALD_STEEL_MODERN_LEFT_END,EMERALD_STEEL_MODERN_LEFT_RIGHT,EMERALD_STEEL_MODERN_RIGHT_END,EMERALD_STEEL_MODERN_UP_END,EMERALD_STEEL_MODERN_UP_DOWN,EMERALD_STEEL_MODERN_DOWN_END,EMERALD_STEEL_MODERN_MIDDLE,EMERALD_SPACE,
                            EMERALD_SWITCH_REMOTEBOMB_LEFT,EMERALD_SWITCH_REMOTEBOMB_DOWN,EMERALD_SWITCH_REMOTEBOMB_RIGHT,EMERALD_SWITCH_REMOTEBOMB_UP,EMERALD_SWITCH_REMOTEBOMB_IGNITION,EMERALD_REMOTEBOMB,EMERALD_SPACE,EMERALD_SPACE,
                            EMERALD_WALL_WITH_EMERALD,EMERALD_WALL_WITH_RUBY,EMERALD_WALL_WITH_SAPPHIRE,EMERALD_WALL_WITH_PERL,EMERALD_WALL_WITH_CRYSTAL,EMERALD_WALL_WITH_STONE,EMERALD_WALL_WITH_NUT,EMERALD_WALL_WITH_WHEEL,
                            EMERALD_WALL_WITH_KEY_RED,EMERALD_WALL_WITH_KEY_GREEN,EMERALD_WALL_WITH_KEY_BLUE,EMERALD_WALL_WITH_KEY_YELLOW,EMERALD_WALL_WITH_KEY_WHITE,EMERALD_WALL_WITH_KEY_GENERAL,EMERALD_WALL_WITH_BOMB,EMERALD_WALL_WITH_MEGABOMB,
                            EMERALD_WALL_WITH_DYNAMITE,EMERALD_WALL_WITH_ENDDOOR,EMERALD_WALL_WITH_ENDDOOR_READY,EMERALD_WALL_WITH_MINE_UP,EMERALD_WALL_WITH_BEETLE_UP,EMERALD_WALL_WITH_YAM,EMERALD_WALL_WITH_ALIEN,EMERALD_WALL_WITH_MOLE_UP,
                            EMERALD_WALL_WITH_GREEN_CHEESE,EMERALD_WALL_WITH_TIME_COIN,EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,
                            EMERALD_INVALID
                            };

uint16_t g_PanelElementsMachines[MAX_PANEL_ELEMENTS + 1] = {
                            EMERALD_EMERALD,EMERALD_RUBY,EMERALD_SAPPHIRE,EMERALD_PERL,EMERALD_CRYSTAL,EMERALD_STONE,EMERALD_NUT,EMERALD_SPACE,
                            EMERALD_BOMB,EMERALD_MEGABOMB,EMERALD_STANDMINE,EMERALD_REMOTEBOMB,EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,
                            EMERALD_MINE_DOWN,EMERALD_BEETLE_DOWN,EMERALD_YAM,EMERALD_ALIEN,EMERALD_MOLE_DOWN,EMERALD_GREEN_DROP,EMERALD_SPACE,EMERALD_SPACE,
                            EMERALD_DOOR_END_NOT_READY,EMERALD_DOOR_END_READY,EMERALD_TIME_COIN,EMERALD_DYNAMITE_OFF,EMERALD_HAMMER,EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,
                            EMERALD_KEY_RED,EMERALD_KEY_YELLOW,EMERALD_KEY_GREEN,EMERALD_KEY_BLUE,EMERALD_KEY_WHITE,EMERALD_KEY_GENERAL,EMERALD_SPACE,EMERALD_SPACE,
                            EMERALD_WALL_GROW_DOWN,EMERALD_STEEL_GROW_DOWN,EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,
                            EMERALD_INVALID
                            };


uint16_t g_PanelElementsTimesScores[MAX_PANEL_ELEMENTS + 1] = {EMERALD_INVALID};


uint16_t g_PanelElementsText[MAX_PANEL_ELEMENTS + 1] = {
                            EMERALD_FONT_EXCLAMATION,EMERALD_FONT_ARROW_RIGHT,EMERALD_FONT_ARROW_UP,EMERALD_FONT_ARROW_DOWN,EMERALD_FONT_ARROW_LEFT,EMERALD_FONT_BRACE_OPEN,EMERALD_FONT_BRACE_CLOSE,EMERALD_FONT_COPYRIGHT,
                            EMERALD_FONT_PLUS,EMERALD_FONT_COMMA,EMERALD_FONT_MINUS,EMERALD_FONT_POINT,EMERALD_FONT_SLASH,EMERALD_FONT_DOUBLE_POINT,EMERALD_FONT_PLATE,EMERALD_FONT_APOSTROPHE,
                            EMERALD_FONT_0,EMERALD_FONT_1,EMERALD_FONT_2,EMERALD_FONT_3,EMERALD_FONT_4,EMERALD_FONT_5,EMERALD_FONT_6,EMERALD_FONT_7,
                            EMERALD_FONT_8,EMERALD_FONT_9,EMERALD_FONT_QUESTION_MARK,EMERALD_FONT_A,EMERALD_FONT_B,EMERALD_FONT_C,EMERALD_FONT_D,EMERALD_FONT_E,
                            EMERALD_FONT_F,EMERALD_FONT_G,EMERALD_FONT_H,EMERALD_FONT_I,EMERALD_FONT_J,EMERALD_FONT_K,EMERALD_FONT_L,EMERALD_FONT_M,
                            EMERALD_FONT_N,EMERALD_FONT_O,EMERALD_FONT_P,EMERALD_FONT_Q,EMERALD_FONT_R,EMERALD_FONT_S,EMERALD_FONT_T,EMERALD_FONT_U,
                            EMERALD_FONT_V,EMERALD_FONT_W,EMERALD_FONT_X,EMERALD_FONT_Y,EMERALD_FONT_Z,EMERALD_FONT_AE,EMERALD_FONT_OE,EMERALD_FONT_UE,

                            EMERALD_FONT_STEEL_EXCLAMATION,EMERALD_FONT_STEEL_ARROW_RIGHT,EMERALD_FONT_STEEL_ARROW_UP,EMERALD_FONT_STEEL_ARROW_DOWN,EMERALD_FONT_STEEL_ARROW_LEFT,EMERALD_FONT_STEEL_BRACE_OPEN,EMERALD_FONT_STEEL_BRACE_CLOSE,EMERALD_FONT_STEEL_COPYRIGHT,
                            EMERALD_FONT_STEEL_PLUS,EMERALD_FONT_STEEL_COMMA,EMERALD_FONT_STEEL_MINUS,EMERALD_FONT_STEEL_POINT,EMERALD_FONT_STEEL_SLASH,EMERALD_FONT_STEEL_DOUBLE_POINT,EMERALD_FONT_STEEL_PLATE,EMERALD_FONT_STEEL_APOSTROPHE,
                            EMERALD_FONT_STEEL_0,EMERALD_FONT_STEEL_1,EMERALD_FONT_STEEL_2,EMERALD_FONT_STEEL_3,EMERALD_FONT_STEEL_4,EMERALD_FONT_STEEL_5,EMERALD_FONT_STEEL_6,EMERALD_FONT_STEEL_7,
                            EMERALD_FONT_STEEL_8,EMERALD_FONT_STEEL_9,EMERALD_FONT_STEEL_QUESTION_MARK,EMERALD_FONT_STEEL_A,EMERALD_FONT_STEEL_B,EMERALD_FONT_STEEL_C,EMERALD_FONT_STEEL_D,EMERALD_FONT_STEEL_E,
                            EMERALD_FONT_STEEL_F,EMERALD_FONT_STEEL_G,EMERALD_FONT_STEEL_H,EMERALD_FONT_STEEL_I,EMERALD_FONT_STEEL_J,EMERALD_FONT_STEEL_K,EMERALD_FONT_STEEL_L,EMERALD_FONT_STEEL_M,
                            EMERALD_FONT_STEEL_N,EMERALD_FONT_STEEL_O,EMERALD_FONT_STEEL_P,EMERALD_FONT_STEEL_Q,EMERALD_FONT_STEEL_R,EMERALD_FONT_STEEL_S,EMERALD_FONT_STEEL_T,EMERALD_FONT_STEEL_U,
                            EMERALD_FONT_STEEL_V,EMERALD_FONT_STEEL_W,EMERALD_FONT_STEEL_X,EMERALD_FONT_STEEL_Y,EMERALD_FONT_STEEL_Z,EMERALD_FONT_STEEL_AE,EMERALD_FONT_STEEL_OE,EMERALD_FONT_STEEL_UE,

                            EMERALD_FONT_GREEN_EXCLAMATION,EMERALD_FONT_GREEN_ARROW_RIGHT,EMERALD_FONT_GREEN_ARROW_UP,EMERALD_FONT_GREEN_ARROW_DOWN,EMERALD_FONT_GREEN_ARROW_LEFT,EMERALD_FONT_GREEN_BRACE_OPEN,EMERALD_FONT_GREEN_BRACE_CLOSE,EMERALD_FONT_GREEN_COPYRIGHT,
                            EMERALD_FONT_GREEN_PLUS,EMERALD_FONT_GREEN_COMMA,EMERALD_FONT_GREEN_MINUS,EMERALD_FONT_GREEN_POINT,EMERALD_FONT_GREEN_SLASH,EMERALD_FONT_GREEN_DOUBLE_POINT,EMERALD_FONT_GREEN_PLATE,EMERALD_FONT_GREEN_APOSTROPHE,
                            EMERALD_FONT_GREEN_0,EMERALD_FONT_GREEN_1,EMERALD_FONT_GREEN_2,EMERALD_FONT_GREEN_3,EMERALD_FONT_GREEN_4,EMERALD_FONT_GREEN_5,EMERALD_FONT_GREEN_6,EMERALD_FONT_GREEN_7,
                            EMERALD_FONT_GREEN_8,EMERALD_FONT_GREEN_9,EMERALD_FONT_GREEN_QUESTION_MARK,EMERALD_FONT_GREEN_A,EMERALD_FONT_GREEN_B,EMERALD_FONT_GREEN_C,EMERALD_FONT_GREEN_D,EMERALD_FONT_GREEN_E,
                            EMERALD_FONT_GREEN_F,EMERALD_FONT_GREEN_G,EMERALD_FONT_GREEN_H,EMERALD_FONT_GREEN_I,EMERALD_FONT_GREEN_J,EMERALD_FONT_GREEN_K,EMERALD_FONT_GREEN_L,EMERALD_FONT_GREEN_M,
                            EMERALD_FONT_GREEN_N,EMERALD_FONT_GREEN_O,EMERALD_FONT_GREEN_P,EMERALD_FONT_GREEN_Q,EMERALD_FONT_GREEN_R,EMERALD_FONT_GREEN_S,EMERALD_FONT_GREEN_T,EMERALD_FONT_GREEN_U,
                            EMERALD_FONT_GREEN_V,EMERALD_FONT_GREEN_W,EMERALD_FONT_GREEN_X,EMERALD_FONT_GREEN_Y,EMERALD_FONT_GREEN_Z,EMERALD_FONT_GREEN_AE,EMERALD_FONT_GREEN_OE,EMERALD_FONT_GREEN_UE,

                            EMERALD_FONT_STEEL_GREEN_EXCLAMATION,EMERALD_FONT_STEEL_GREEN_ARROW_RIGHT,EMERALD_FONT_STEEL_GREEN_ARROW_UP,EMERALD_FONT_STEEL_GREEN_ARROW_DOWN,EMERALD_FONT_STEEL_GREEN_ARROW_LEFT,EMERALD_FONT_STEEL_GREEN_BRACE_OPEN,EMERALD_FONT_STEEL_GREEN_BRACE_CLOSE,EMERALD_FONT_STEEL_GREEN_COPYRIGHT,
                            EMERALD_FONT_STEEL_GREEN_PLUS,EMERALD_FONT_STEEL_GREEN_COMMA,EMERALD_FONT_STEEL_GREEN_MINUS,EMERALD_FONT_STEEL_GREEN_POINT,EMERALD_FONT_STEEL_GREEN_SLASH,EMERALD_FONT_STEEL_GREEN_DOUBLE_POINT,EMERALD_FONT_STEEL_GREEN_PLATE,EMERALD_FONT_STEEL_GREEN_APOSTROPHE,
                            EMERALD_FONT_STEEL_GREEN_0,EMERALD_FONT_STEEL_GREEN_1,EMERALD_FONT_STEEL_GREEN_2,EMERALD_FONT_STEEL_GREEN_3,EMERALD_FONT_STEEL_GREEN_4,EMERALD_FONT_STEEL_GREEN_5,EMERALD_FONT_STEEL_GREEN_6,EMERALD_FONT_STEEL_GREEN_7,
                            EMERALD_FONT_STEEL_GREEN_8,EMERALD_FONT_STEEL_GREEN_9,EMERALD_FONT_STEEL_GREEN_QUESTION_MARK,EMERALD_FONT_STEEL_GREEN_A,EMERALD_FONT_STEEL_GREEN_B,EMERALD_FONT_STEEL_GREEN_C,EMERALD_FONT_STEEL_GREEN_D,EMERALD_FONT_STEEL_GREEN_E,
                            EMERALD_FONT_STEEL_GREEN_F,EMERALD_FONT_STEEL_GREEN_G,EMERALD_FONT_STEEL_GREEN_H,EMERALD_FONT_STEEL_GREEN_I,EMERALD_FONT_STEEL_GREEN_J,EMERALD_FONT_STEEL_GREEN_K,EMERALD_FONT_STEEL_GREEN_L,EMERALD_FONT_STEEL_GREEN_M,
                            EMERALD_FONT_STEEL_GREEN_N,EMERALD_FONT_STEEL_GREEN_O,EMERALD_FONT_STEEL_GREEN_P,EMERALD_FONT_STEEL_GREEN_Q,EMERALD_FONT_STEEL_GREEN_R,EMERALD_FONT_STEEL_GREEN_S,EMERALD_FONT_STEEL_GREEN_T,EMERALD_FONT_STEEL_GREEN_U,
                            EMERALD_FONT_STEEL_GREEN_V,EMERALD_FONT_STEEL_GREEN_W,EMERALD_FONT_STEEL_GREEN_X,EMERALD_FONT_STEEL_GREEN_Y,EMERALD_FONT_STEEL_GREEN_Z,EMERALD_FONT_STEEL_GREEN_AE,EMERALD_FONT_STEEL_GREEN_OE,EMERALD_FONT_STEEL_GREEN_UE,
                            EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,EMERALD_SPACE,
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
    uint32_t K;

    switch (uMenuState) {
        case (MENUSTATE_LEVEL_STD):
            memcpy(g_PanelElements,g_PanelElementsMain,sizeof(uint16_t) * (MAX_PANEL_ELEMENTS + 1));
            break;
        case (MENUSTATE_LEVEL_TEXT):
        case (MENUSTATE_YAMS_TEXT):
            memcpy(g_PanelElements,g_PanelElementsText,sizeof(uint16_t) * (MAX_PANEL_ELEMENTS + 1));
            break;
        case (MENUSTATE_YAMS):
            memcpy(g_PanelElements,g_PanelElementsMain,sizeof(uint16_t) * (MAX_PANEL_ELEMENTS + 1));
            for (K = 0; K < MAX_PANEL_ELEMENTS; K++) {
                // Yam-Menü soll einige Elemente nicht anbieten
                switch (g_PanelElements[K]) {
                    case (EMERALD_MAN):
                    /*
                    case (EMERALD_TELEPORTER_RED):
                    case (EMERALD_TELEPORTER_YELLOW):
                    case (EMERALD_TELEPORTER_GREEN):
                    case (EMERALD_TELEPORTER_BLUE):
                    */
                        g_PanelElements[K] = EMERALD_SPACE; // Im Yam-Editor das entsprechende Element durch Space ersetzen
                        break;
                }
            }
            break;
        case (MENUSTATE_MACHINES):
            memcpy(g_PanelElements,g_PanelElementsMachines,sizeof(uint16_t) * (MAX_PANEL_ELEMENTS + 1));
            break;
        case (MENUSTATE_TIME_AND_SCORES):
            memcpy(g_PanelElements,g_PanelElementsTimesScores,sizeof(uint16_t) * (MAX_PANEL_ELEMENTS + 1));
            break;
        default:
            memcpy(g_PanelElements,g_PanelElementsMain,sizeof(uint16_t) * (MAX_PANEL_ELEMENTS + 1));
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
Name:           GetLevelXmlFromEditor
------------------------------------------------------------------------------
Beschreibung: Gibt ein Level in XML zurück, welches sich in der Struktur
              Ed.x befindet. Bei Erfolg muss der XML-Speicher außerhalb
              dieser Funktion wieder freigegeben werden.

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  DYNSTRING *, Zeiger auf XML-Leveldaten, NULL = Fehler
Seiteneffekte: Ed.x
------------------------------------------------------------------------------*/
DYNSTRING *GetLevelXmlFromEditor(void) {
    char szNum[32];
    char szNum2[32];
    uint32_t uBase64Len;
    uint32_t uBase64MessageLen;
    uint32_t I;
    uint32_t E;
    uint8_t *pcBaseMessage64 = NULL;
    uint8_t *pcBase64 = NULL;
    uint8_t *pcCompressedLevel = NULL;
    uint32_t uMaxCompressedSize;
    MD5Context MD5Leveldata;
    char szM[4];    // 'm1' bis 'm8'
    char szTag[32];
    DYNSTRING *XML = NULL;
    bool bError = false;
    int nMiniz;

    XML = DynStringInit();
    if (XML == NULL) {
        return NULL;
    }
    szM[0] = 'm';
    szM[1] = 'x';
    szM[2] = 0;
    DynStringAdd(XML,"  <title>");DynStringAdd(XML,Ed.szLevelTitle);DynStringAdd(XML,"</title>\r\n");
    DynStringAdd(XML,"  <author>");DynStringAdd(XML,Ed.szLevelAuthor);DynStringAdd(XML,"</author>\r\n");
    DynStringAdd(XML,"  <version>");DynStringAdd(XML,Ed.szVersion);DynStringAdd(XML,"</version>\r\n");
    DynStringAdd(XML,"  <values>\r\n");
    sprintf(szNum,"%u",Ed.uEmeraldsToCollect);
    DynStringAdd(XML,"    <emeralds_to_collect>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</emeralds_to_collect>\r\n");
    sprintf(szNum,"%u",Ed.uTimeScoreFactor);
    DynStringAdd(XML,"    <score_time_factor>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</score_time_factor>\r\n");
    sprintf(szNum,"%u",Ed.uCheeseSpreadSpeed);
    DynStringAdd(XML,"    <speed_cheese_spread>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</speed_cheese_spread>\r\n");
    sprintf(szNum,"%u",Ed.uGrassSpreadSpeed);
    DynStringAdd(XML,"    <speed_grass_spread>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</speed_grass_spread>\r\n");
    DynStringAdd(XML,"  </values>\r\n");
    DynStringAdd(XML,"  <leveldimension>\r\n");
    sprintf(szNum,"%u",Ed.uLevel_X_Dimension);
    DynStringAdd(XML,"    <x>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</x>\r\n");
    sprintf(szNum,"%u",Ed.uLevel_Y_Dimension);
    DynStringAdd(XML,"    <y>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</y>\r\n");
    DynStringAdd(XML,"  </leveldimension>\r\n");
    DynStringAdd(XML,"  <scores>\r\n");
    sprintf(szNum,"%u",Ed.uScoreEmerald);
    DynStringAdd(XML,"    <emerald>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</emerald>\r\n");
    sprintf(szNum,"%u",Ed.uScoreRuby);
    DynStringAdd(XML,"    <ruby>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</ruby>\r\n");
    sprintf(szNum,"%u",Ed.uScoreSaphir);
    DynStringAdd(XML,"    <sapphire>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</sapphire>\r\n");
    sprintf(szNum,"%u",Ed.uScorePerl);
    DynStringAdd(XML,"    <perl>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</perl>\r\n");
    sprintf(szNum,"%u",Ed.uScoreCrystal);
    DynStringAdd(XML,"    <crystal>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</crystal>\r\n");
    sprintf(szNum,"%u",Ed.uScoreMessage);
    DynStringAdd(XML,"    <letter>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</letter>\r\n");
    sprintf(szNum,"%u",Ed.uScoreKey);
    DynStringAdd(XML,"    <key>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</key>\r\n");
    sprintf(szNum,"%u",Ed.uScoreDynamite);
    DynStringAdd(XML,"    <dynamite>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</dynamite>\r\n");
    sprintf(szNum,"%u",Ed.uScoreHammer);
    DynStringAdd(XML,"    <hammer>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</hammer>\r\n");
    sprintf(szNum,"%u",Ed.uScoreNutCracking);
    DynStringAdd(XML,"    <nutcracking>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</nutcracking>\r\n");
    sprintf(szNum,"%u",Ed.uScoreStoningBeetle);
    DynStringAdd(XML,"    <stoning_beetle>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</stoning_beetle>\r\n");
    sprintf(szNum,"%u",Ed.uScoreStoningMine);
    DynStringAdd(XML,"    <stoning_mine>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</stoning_mine>\r\n");
    sprintf(szNum,"%u",Ed.uScoreStoningAlien);
    DynStringAdd(XML,"    <stoning_alien>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</stoning_alien>\r\n");
    sprintf(szNum,"%u",Ed.uScoreStoningYam);
    DynStringAdd(XML,"    <stoning_yam>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</stoning_yam>\r\n");
    sprintf(szNum,"%u",Ed.uScoreTimeCoin);
    DynStringAdd(XML,"    <timecoin>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</timecoin>\r\n");
    DynStringAdd(XML,"  </scores>\r\n");
    DynStringAdd(XML,"  <times>\r\n");
    sprintf(szNum,"%u",Ed.uTimeToPlay);
    DynStringAdd(XML,"    <to_play>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</to_play>\r\n");
    sprintf(szNum,"%u",Ed.uTimeWheelRotation);
    DynStringAdd(XML,"    <wheel_rotation>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</wheel_rotation>\r\n");
    sprintf(szNum,"%u",Ed.uTimeMagicWall);
    DynStringAdd(XML,"    <magic_wall>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</magic_wall>\r\n");
    sprintf(szNum,"%u",Ed.uTimeLight);
    DynStringAdd(XML,"    <light>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</light>\r\n");
    sprintf(szNum,"%u",Ed.uTimeDoorTime);
    DynStringAdd(XML,"    <timedoor>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</timedoor>\r\n");
    sprintf(szNum,"%u",Ed.uAdditonalTimeCoinTime);
    DynStringAdd(XML,"    <timecoin>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</timecoin>\r\n");
    DynStringAdd(XML,"  </times>\r\n");
    DynStringAdd(XML,"  <inventory>\r\n");
    sprintf(szNum,"%u",Ed.uDynamiteCount);
    DynStringAdd(XML,"    <dynamite>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</dynamite>\r\n");
    sprintf(szNum,"%u",Ed.uHammerCount);
    DynStringAdd(XML,"    <hammer>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</hammer>\r\n");
    sprintf(szNum,"%u",Ed.uWhiteKeyCount);
    DynStringAdd(XML,"    <white_key>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</white_key>\r\n");
    DynStringAdd(XML,"  </inventory>\r\n");
    DynStringAdd(XML,"  <replicators>\r\n");
    DynStringAdd(XML,"    <red>\r\n");
    if (Ed.bReplicatorRedOn) {
        strcpy(szNum,"1");
    } else {
        strcpy(szNum,"0");
    }
    DynStringAdd(XML,"      <active>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</active>\r\n");
    sprintf(szNum,"%u",Ed.uReplicatorRedObject);
    DynStringAdd(XML,"      <element>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</element>\r\n");
    DynStringAdd(XML,"    </red>\r\n");
    DynStringAdd(XML,"    <green>\r\n");
    if (Ed.bReplicatorGreenOn) {
        strcpy(szNum,"1");
    } else {
        strcpy(szNum,"0");
    }
    DynStringAdd(XML,"      <active>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</active>\r\n");
    sprintf(szNum,"%u",Ed.uReplicatorGreenObject);
    DynStringAdd(XML,"      <element>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</element>\r\n");
    DynStringAdd(XML,"    </green>\r\n");
    DynStringAdd(XML,"    <blue>\r\n");
    if (Ed.bReplicatorBlueOn) {
        strcpy(szNum,"1");
    } else {
        strcpy(szNum,"0");
    }
    DynStringAdd(XML,"      <active>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</active>\r\n");
    sprintf(szNum,"%u",Ed.uReplicatorBlueObject);
    DynStringAdd(XML,"      <element>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</element>\r\n");
    DynStringAdd(XML,"    </blue>\r\n");
    DynStringAdd(XML,"    <yellow>\r\n");
    if (Ed.bReplicatorYellowOn) {
        strcpy(szNum,"1");
    } else {
        strcpy(szNum,"0");
    }
    DynStringAdd(XML,"      <active>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</active>\r\n");
    sprintf(szNum,"%u",Ed.uReplicatorYellowObject);
    DynStringAdd(XML,"      <element>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</element>\r\n");
    DynStringAdd(XML,"    </yellow>\r\n");
    DynStringAdd(XML,"  </replicators>\r\n");
    DynStringAdd(XML,"  <lightbarriers>\r\n");
    DynStringAdd(XML,"    <red>\r\n");
    if (Ed.bLightBarrierRedOn) {
        strcpy(szNum,"1");
    } else {
        strcpy(szNum,"0");
    }
    DynStringAdd(XML,"      <active>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</active>\r\n");
    DynStringAdd(XML,"    </red>\r\n");
    DynStringAdd(XML,"    <green>\r\n");
    if (Ed.bLightBarrierGreenOn) {
        strcpy(szNum,"1");
    } else {
        strcpy(szNum,"0");
    }
    DynStringAdd(XML,"      <active>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</active>\r\n");
    DynStringAdd(XML,"    </green>\r\n");
    DynStringAdd(XML,"    <blue>\r\n");
    if (Ed.bLightBarrierBlueOn) {
        strcpy(szNum,"1");
    } else {
        strcpy(szNum,"0");
    }
    DynStringAdd(XML,"      <active>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</active>\r\n");
    DynStringAdd(XML,"    </blue>\r\n");
    DynStringAdd(XML,"    <yellow>\r\n");
    if (Ed.bLightBarrierYellowOn) {
        strcpy(szNum,"1");
    } else {
        strcpy(szNum,"0");
    }
    DynStringAdd(XML,"      <active>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</active>\r\n");
    DynStringAdd(XML,"    </yellow>\r\n");
    DynStringAdd(XML,"  </lightbarriers>\r\n");
    DynStringAdd(XML,"  <conveyorbelts>\r\n");
    DynStringAdd(XML,"    <red>\r\n");
    switch (Ed.uConveybeltRedState) {
        case (EMERALD_CONVEYBELT_LEFT):
            strcpy(szNum,"left");
            strcpy(szNum2,"right");
            break;
        case (EMERALD_CONVEYBELT_RIGHT):
            strcpy(szNum,"right");
            strcpy(szNum2,"left");
            break;
        case (EMERALD_CONVEYBELT_OFF):
            strcpy(szNum,"off");
            if (Ed.uConveybeltRedDirection == EMERALD_CONVEYBELT_TO_LEFT) {
                strcpy(szNum2,"left");
            } else {
                strcpy(szNum2,"right");
            }
            break;
        default:
            strcpy(szNum,"off");
            if (Ed.uConveybeltRedDirection == EMERALD_CONVEYBELT_TO_LEFT) {
                strcpy(szNum2,"left");
            } else {
                strcpy(szNum2,"right");
            }
            break;
    }
    DynStringAdd(XML,"      <direction_at_start>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</direction_at_start>\r\n");
    DynStringAdd(XML,"      <next_direction>");DynStringAdd(XML,szNum2);DynStringAdd(XML,"</next_direction>\r\n");
    DynStringAdd(XML,"    </red>\r\n");
    DynStringAdd(XML,"    <green>\r\n");
    switch (Ed.uConveybeltGreenState) {
        case (EMERALD_CONVEYBELT_LEFT):
            strcpy(szNum,"left");
            strcpy(szNum2,"right");
            break;
        case (EMERALD_CONVEYBELT_RIGHT):
            strcpy(szNum,"right");
            strcpy(szNum2,"left");
            break;
        case (EMERALD_CONVEYBELT_OFF):
            strcpy(szNum,"off");
            if (Ed.uConveybeltGreenDirection == EMERALD_CONVEYBELT_TO_LEFT) {
                strcpy(szNum2,"left");
            } else {
                strcpy(szNum2,"right");
            }
            break;
        default:
            strcpy(szNum,"off");
            if (Ed.uConveybeltGreenDirection == EMERALD_CONVEYBELT_TO_LEFT) {
                strcpy(szNum2,"left");
            } else {
                strcpy(szNum2,"right");
            }
            break;
    }
    DynStringAdd(XML,"      <direction_at_start>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</direction_at_start>\r\n");
    DynStringAdd(XML,"      <next_direction>");DynStringAdd(XML,szNum2);DynStringAdd(XML,"</next_direction>\r\n");
    DynStringAdd(XML,"    </green>\r\n");
    DynStringAdd(XML,"    <blue>\r\n");
    switch (Ed.uConveybeltBlueState) {
        case (EMERALD_CONVEYBELT_LEFT):
            strcpy(szNum,"left");
            strcpy(szNum2,"right");
            break;
        case (EMERALD_CONVEYBELT_RIGHT):
            strcpy(szNum,"right");
            strcpy(szNum2,"left");
            break;
        case (EMERALD_CONVEYBELT_OFF):
            strcpy(szNum,"off");
            if (Ed.uConveybeltBlueDirection == EMERALD_CONVEYBELT_TO_LEFT) {
                strcpy(szNum2,"left");
            } else {
                strcpy(szNum2,"right");
            }
            break;
        default:
            strcpy(szNum,"off");
            if (Ed.uConveybeltBlueDirection == EMERALD_CONVEYBELT_TO_LEFT) {
                strcpy(szNum2,"left");
            } else {
                strcpy(szNum2,"right");
            }
            break;
    }
    DynStringAdd(XML,"      <direction_at_start>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</direction_at_start>\r\n");
    DynStringAdd(XML,"      <next_direction>");DynStringAdd(XML,szNum2);DynStringAdd(XML,"</next_direction>\r\n");
    DynStringAdd(XML,"    </blue>\r\n");
    DynStringAdd(XML,"    <yellow>\r\n");
    switch (Ed.uConveybeltYellowState) {
        case (EMERALD_CONVEYBELT_LEFT):
            strcpy(szNum,"left");
            strcpy(szNum2,"right");
            break;
        case (EMERALD_CONVEYBELT_RIGHT):
            strcpy(szNum,"right");
            strcpy(szNum2,"left");
            break;
        case (EMERALD_CONVEYBELT_OFF):
            strcpy(szNum,"off");
            if (Ed.uConveybeltYellowDirection == EMERALD_CONVEYBELT_TO_LEFT) {
                strcpy(szNum2,"left");
            } else {
                strcpy(szNum2,"right");
            }
            break;
        default:
            strcpy(szNum,"off");
            if (Ed.uConveybeltYellowDirection == EMERALD_CONVEYBELT_TO_LEFT) {
                strcpy(szNum2,"left");
            } else {
                strcpy(szNum2,"right");
            }
            break;
    }
    DynStringAdd(XML,"      <direction_at_start>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</direction_at_start>\r\n");
    DynStringAdd(XML,"      <next_direction>");DynStringAdd(XML,szNum2);DynStringAdd(XML,"</next_direction>\r\n");
    DynStringAdd(XML,"    </yellow>\r\n");
    DynStringAdd(XML,"  </conveyorbelts>\r\n");
    DynStringAdd(XML,"  <messages>\r\n");
    for (I = 1; (I <= EMERALD_MAX_MESSAGES) && (!bError); I++) {
        szM[1] = I + 0x30;
        sprintf(szTag,"    <%s>",szM);
        DynStringAdd(XML,szTag);             // Öffnungstag schreiben
        if (Ed.pMessage[I - 1] != NULL) {
            pcBaseMessage64 = BinToBase64((uint8_t*)Ed.pMessage[I - 1], strlen(Ed.pMessage[I - 1]),&uBase64MessageLen,false);
            if (pcBaseMessage64 != NULL) {
                DynStringAdd(XML,(char*)pcBaseMessage64);
                SAFE_FREE(pcBaseMessage64);
                sprintf(szTag,"</%s>\r\n",szM);
                DynStringAdd(XML,szTag);     // Schließungstag schreiben
            } else {
                SDL_Log("%s: BinToBase64() for message %s failed",__FUNCTION__,szTag);
                bError = true;
            }
        } else {
            sprintf(szTag,"</%s>\r\n",szM);
            DynStringAdd(XML,szTag);     // Schließungstag schreiben
        }
    }
    if (!bError) {
        DynStringAdd(XML,"  </messages>\r\n");
        sprintf(szNum,"%u",Ed.uMaxYamExplosionIndex + 1);
        DynStringAdd(XML,"  <max_yam_explosions>");DynStringAdd(XML,szNum);DynStringAdd(XML,"</max_yam_explosions>\r\n");
        DynStringAdd(XML,"  <yam_explosions>\r\n");
        for (I = 0; (I <= Ed.uMaxYamExplosionIndex); I++) {
            sprintf(szTag,"    <explosion%02u>\r\n",I + 1);
            DynStringAdd(XML,szTag);
            for (E = 0; E < 9; E++) {
                sprintf(szTag,"      <element%02u>",E + 1);
                DynStringAdd(XML,szTag);
                sprintf(szNum,"%d",Ed.YamExplosions[I].uElement[E]);
                DynStringAdd(XML,szNum);
                sprintf(szTag,"</element%02u>\r\n",E + 1);
                DynStringAdd(XML,szTag);
            }
            sprintf(szTag,"    </explosion%02u>\r\n",I + 1);
            DynStringAdd(XML,szTag);
        }
        DynStringAdd(XML,"  </yam_explosions>\r\n");
        DynStringAdd(XML,"  <leveldata>");
        uMaxCompressedSize = Ed.uLevel_X_Dimension * Ed.uLevel_Y_Dimension * sizeof(uint16_t) * 2 + 16 * KB;
        pcCompressedLevel = malloc(uMaxCompressedSize);
        if (pcCompressedLevel != NULL) {
            // uMaxCompressedSize ist hier noch Eingabeparameter, damit mz_compress2() weiß, wieviel Puffer zum Komprimieren bereit steht
            nMiniz = mz_compress2(pcCompressedLevel,(mz_ulong*)&uMaxCompressedSize,(uint8_t*)Ed.pLevel,Ed.uLevel_X_Dimension * Ed.uLevel_Y_Dimension * sizeof(uint16_t),MZ_UBER_COMPRESSION);
            if (nMiniz == MZ_OK) {
                SDL_Log("%s:c compressed level size: %u",__FUNCTION__,uMaxCompressedSize);
                pcBase64 = BinToBase64((uint8_t *)pcCompressedLevel,uMaxCompressedSize,&uBase64Len,false);
                if (pcBase64 != NULL) {
                    DynStringAdd(XML,(char*)pcBase64); // pcBase64 ist 0-terminiert
                    DynStringAdd(XML,"</leveldata>\r\n");
                    DynStringAdd(XML,"  <leveldata_md5_hash>");

                    md5Init(&MD5Leveldata);
                    md5Update(&MD5Leveldata,pcBase64,uBase64Len);
                    md5Finalize(&MD5Leveldata);
                    for (I = 0; I < 16; I++) {
                        sprintf(szNum,"%02X",MD5Leveldata.digest[I]);
                        DynStringAdd(XML,szNum);
                    }
                    DynStringAdd(XML,"</leveldata_md5_hash>\r\n");
                } else {
                    SDL_Log("%s: BinToBase64 failed",__FUNCTION__);
                    bError = true;
                }
            } else {
                SDL_Log("%s: mz_compress2() failed, Error: %d",__FUNCTION__,nMiniz);
                bError = true;
            }
        } else {
            SDL_Log("%s: malloc() failed (mz_compress2 memory)",__FUNCTION__);
            bError = true;
        }
    }
    SAFE_FREE(pcCompressedLevel);
    SAFE_FREE(pcBase64);
    if (bError) {
        SAFE_FREE(XML);
        XML = NULL;
    }
    return XML;
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
Seiteneffekte: Playfield.x, Ed.x, ge_DisplayMode.x
------------------------------------------------------------------------------*/
int CopyPlayfieldValueToEditor(void) {
    uint32_t I;

    if ((Playfield.bInitOK) && (Playfield.pLevel != NULL) &&
        (Playfield.uLevel_X_Dimension >= MIN_LEVEL_W) && (Playfield.uLevel_X_Dimension <= MAX_LEVEL_W) &&
        (Playfield.uLevel_Y_Dimension >= MIN_LEVEL_H) && (Playfield.uLevel_Y_Dimension <= MAX_LEVEL_H)) {
        Ed.pLevel = Playfield.pLevel;
        Ed.uLevel_X_Dimension = Playfield.uLevel_X_Dimension;
        Ed.uLevel_Y_Dimension = Playfield.uLevel_Y_Dimension;
        Ed.uTmpLevel_X_Dimension = Ed.uLevel_X_Dimension;
        Ed.uTmpLevel_Y_Dimension = Ed.uLevel_Y_Dimension;
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
        Ed.uConveybeltRedState = Playfield.uConveybeltRedState;
        Ed.uConveybeltRedDirection = Playfield.uConveybeltRedDirection;
        Ed.uConveybeltGreenState = Playfield.uConveybeltGreenState;
        Ed.uConveybeltGreenDirection = Playfield.uConveybeltGreenDirection;
        Ed.uConveybeltBlueState = Playfield.uConveybeltBlueState;
        Ed.uConveybeltBlueDirection = Playfield.uConveybeltBlueDirection;
        Ed.uConveybeltYellowState = Playfield.uConveybeltYellowState;
        Ed.uConveybeltYellowDirection = Playfield.uConveybeltYellowDirection;
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
        Ed.uGrassSpreadSpeed = Playfield.uGrassSpreadSpeed;;
        Ed.uTimeToPlay = Playfield.uTimeToPlay / ge_DisplayMode.refresh_rate;
        Ed.uAdditonalTimeCoinTime = Playfield.uAdditonalTimeCoinTime / ge_DisplayMode.refresh_rate;
        Ed.uTimeWheelRotation = Playfield.uTimeWheelRotation / ge_DisplayMode.refresh_rate;
        Ed.uTimeDoorTime = Playfield.uTimeDoorTime / ge_DisplayMode.refresh_rate;
        Ed.uTimeMagicWall = Playfield.uTimeMagicWall / ge_DisplayMode.refresh_rate;
        Ed.uTimeLight = Playfield.uTimeLight / ge_DisplayMode.refresh_rate;
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
Name:           InitMessageEditor
------------------------------------------------------------------------------
Beschreibung: Initialisiert die Struktur Ed.MessageEditor für den Nachrichten-Editor.

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Ed.x
------------------------------------------------------------------------------*/
void InitMessageEditor(void) {
    Ed.MessageEditor.nEditMessage = -1;
    Ed.MessageEditor.bCursor = false;
    Ed.MessageEditor.bInsert = true;
    Ed.MessageEditor.uFrameCounter = 0;             // Bildzähler
    Ed.MessageEditor.uCursorFlashSpeedFrames = 15;  // Nach X Frames wird Cursor umgeschaltet
    Ed.MessageEditor.uLastToggleCursorFrame = 0;    // Letztes Frame, bei dem Cursor umgeschaltet wurde
}


/*----------------------------------------------------------------------------
Name:           CalcEditorViewArea
------------------------------------------------------------------------------
Beschreibung: Berechnet den Sichtbereich und die Positionierung des Levels im Editor.
              Die Leveldimension (Ed.uLevel_X_Dimension und Ed.uLevel_Y_Dimension)
              und der Wert Ed.bHalfSize muss bereits initialisiert sein.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Ed.x, Config.x
------------------------------------------------------------------------------*/
void CalcEditorViewArea(void) {

    uint32_t uX;


    // Sichtbaren Bereich berechnen
    if (Ed.bHalfSize) {
        Ed.uFont_W = FONT_W / 2;
        Ed.uFont_H = FONT_H / 2;
        Ed.uScrollPixelX = 16;              // Scrollt bei Tastendruck x Pixel horizontal
        Ed.uScrollPixelFastX = 32;          // Scrollt bei Tastendruck x Pixel horizontal in hoher Geschwindigkeit
        Ed.uScrollPixelY = 16;              // Scrollt bei Tastendruck x Pixel vertikal
        Ed.uScrollPixelFastY = 32;          // Scrollt bei Tastendruck x Pixel vertikal in hoher Geschwindigkeit
    } else {
        Ed.uFont_W = FONT_W;
        Ed.uFont_H = FONT_H;
        Ed.uScrollPixelX = 32;              // Scrollt bei Tastendruck x Pixel horizontal
        Ed.uScrollPixelFastX = 64;          // Scrollt bei Tastendruck x Pixel horizontal in hoher Geschwindigkeit
        Ed.uScrollPixelY = 32;              // Scrollt bei Tastendruck x Pixel vertikal
        Ed.uScrollPixelFastY = 64;          // Scrollt bei Tastendruck x Pixel vertikal in hoher Geschwindigkeit
    }
    Ed.uVisibleX = (Config.uResX - Ed.uPanelW) / Ed.uFont_W;
    Ed.uVisibleY = Config.uResY / Ed.uFont_H;

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
    if ((Config.uResX % FONT_W) != 0) {
        uX = Config.uResX;
        uX = ((uX + FONT_W - 1) / FONT_W) * FONT_W;     // Aufrunden auf nächste passende X-Auflösung
        Ed.nMaxXpos = (Ed.uLevel_X_Dimension * Ed.uFont_W) - uX + Ed.uPanelW + Ed.uFont_W;
    } else {
        Ed.nMaxXpos = (Ed.uLevel_X_Dimension * Ed.uFont_W) - Config.uResX + Ed.uPanelW;
    }
    if (Ed.nMaxXpos < 0) {
        Ed.nMaxXpos = 0;
    }
    Ed.nMaxYpos = (Ed.uLevel_Y_Dimension * Ed.uFont_H) - Config.uResY + PANEL_H;
    if (Ed.nMaxYpos < 0) {
        Ed.nMaxYpos = 0;
    }
    Ed.nMinXLevel = Ed.uShiftLevelXpix;                                         // Minimale X-Position innerhalb des Levela
    Ed.nMaxXLevel = Ed.nMinXLevel + Ed.uVisibleX * Ed.uFont_W;                  // Maximale X-Position innerhalb des Levela
    Ed.nMinYLevel = Ed.uShiftLevelYpix;                                         // Minimale Y-Position innerhalb des Levela
    Ed.nMaxYLevel = Ed.nMinYLevel + Ed.uLevel_Y_Dimension * Ed.uFont_W;         // Maximale Y-Position innerhalb des Levela
}


/*----------------------------------------------------------------------------
Name:           SetLevelBorder
------------------------------------------------------------------------------
Beschreibung: Setzt die Level-Umrandung mit EMERALD_STEEL.
Parameter
      Eingang: pLevel, uint16_t *, Zeiger auf Levelspeicher
               bClear, bool, true = Spielfeld auf EMERALD_SPACE setzen
               bAlwaysSteel, bool,
                true = der Außenrand wird immer auf EMERALD_STEEL gesetzt, auch wenn bereits ein kompatibles Stahl-Element (IsSteel()) vorhanden ist
                false = falls der Außenrand bereits ein kompatibles Stahl-Element (IsSteel()) enhält, wird dieses Element belassen.
      Ausgang: -
Rückgabewert:  int, 0 = alles OK, sonst Fehler
Seiteneffekte: Ed.x
------------------------------------------------------------------------------*/
int SetLevelBorder(uint16_t *pLevel, bool bClear, bool AlwaysSteel) {
    uint32_t I;
    uint32_t X,Y;


    if (pLevel == NULL) {
        SDL_Log("%s: null pointer found",__FUNCTION__);
        return -1;
    }
    if (bClear) {
        for (Y = 1; Y < (Ed.uLevel_Y_Dimension - 1); Y++) {
            for (X = 1; X < (Ed.uLevel_X_Dimension - 1); X++) {
                pLevel[Y * Ed.uLevel_X_Dimension + X] = EMERALD_SPACE;
            }
        }
    }
    // Obere Zeile setzen
    for (I = 0; (I < Ed.uLevel_X_Dimension); I++) {
        if (AlwaysSteel || (!IsSteel(pLevel[I]))) {
            pLevel[I] = EMERALD_STEEL;
        }
    }
    // Untere Zeile setzen
    for (I = Ed.uLevel_X_Dimension * (Ed.uLevel_Y_Dimension - 1); (I < Ed.uLevel_X_Dimension * Ed.uLevel_Y_Dimension); I++) {
        if (AlwaysSteel || (!IsSteel(pLevel[I]))) {
            pLevel[I] = EMERALD_STEEL;
        }
    }
    // Linke Spalte setzen
    for (I = 0; (I < Ed.uLevel_Y_Dimension); I++) {
        if (AlwaysSteel || (!IsSteel(pLevel[I * Ed.uLevel_X_Dimension]))) {
            pLevel[I * Ed.uLevel_X_Dimension] = EMERALD_STEEL;
        }
    }
    // Rechte Spalte setzen
    for (I = 1; (I <= Ed.uLevel_Y_Dimension); I++) {
        if (AlwaysSteel || (!IsSteel(pLevel[I * Ed.uLevel_X_Dimension - 1]))) {
            pLevel[I * Ed.uLevel_X_Dimension - 1] = EMERALD_STEEL;
        }
    }
    return 0;
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
Seiteneffekte: Ed.x, g_PanelElements[], Config.x
------------------------------------------------------------------------------*/
uint16_t GetElementByMouseposition(int nMouseXpos, int nMouseYpos) {
    uint32_t uElement;
    int nX;
    int nY;
    int nMaxY;

    // SDL_Log("uMenuState: %d     nMouseYpos: %d",Ed.uMenuState,nMouseYpos);
    if ((Ed.uMenuState == 1) || (Ed.uMenuState == 3)) {     // Zeichensatz bei Standard und Yam
        nMaxY = 655;    // da vom oberen Rand gerechnet, Y-Werte sind absolut
    } else {
        nMaxY = 760;    // da vom oberen Rand gerechnet, Y-Werte sind absolut
    }
    uElement = EMERALD_NONE;
    if ((nMouseXpos >= Config.uResX - 184) && (nMouseXpos <= Config.uResX - 8)) {
        nX = (nMouseXpos - (Config.uResX - 184)) / 23;
    } else {
        nX = -1;
    }
    if ((nMouseYpos >= 8) && (nMouseYpos <= nMaxY)) {
        nY = (nMouseYpos - 8) / 23;
    } else {
        nY = -1;
    }
    if ((nX != -1) && (nY != -1)) {
        uElement = g_PanelElements[(nY * 8) + nX];
    }
    return uElement;
}


/*----------------------------------------------------------------------------
Name:           FillPanel
------------------------------------------------------------------------------
Beschreibung: Füllt das Editor-Panel mit Elementen aus dem Array g_PanelElements[].
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Ed.x, g_PanelElements[], Playfiel.x
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
            DestR.x = Ed.uPanelXpos + 8 + uX * 23;
            DestR.y = Ed.uPanelYpos + 8 + uY * 23;
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
Seiteneffekte: Ed.x, g_PanelColorPatterns[], Config.x
------------------------------------------------------------------------------*/
int DrawEditorPanel(SDL_Renderer *pRenderer) {
    uint32_t I;
    float fAngle;
    int nErrorCode = 0;
    SDL_Rect Rect;
    uint32_t uTextureIndex;

    for (I = 0; (I < 4) && (nErrorCode == 0); I++) {
        SDL_SetRenderDrawColor(pRenderer,g_PanelColorPatterns[I * 3 + 0],g_PanelColorPatterns[I * 3 + 1],g_PanelColorPatterns[I * 3 + 2], SDL_ALPHA_OPAQUE);  // Farbe für Rechteck setzen
        Rect.x = Ed.uPanelXpos + 1 + 4 - I;
        Rect.y = Ed.uPanelYpos + 3 - I;
        Rect.w = Ed.uPanelW - 8 + I * 2;
        Rect.h = Ed.uPanelH - 6 + I * 2;
        nErrorCode = SDL_RenderDrawRect(pRenderer,&Rect);
    }
    if (nErrorCode == 0) {
        nErrorCode = FillPanel(pRenderer);
        for (I = 0; (I < 4) && (nErrorCode == 0); I++) {
            // linkes Quadrat, selected Element
            SDL_SetRenderDrawColor(pRenderer,g_PanelColorPatterns[I * 3 + 0],g_PanelColorPatterns[I * 3 + 1],g_PanelColorPatterns[I * 3 + 2], SDL_ALPHA_OPAQUE);  // Farbe für Rechteck setzen
            Rect.x = Config.uResX - 177 - I;
            Rect.y = Config.uResY - FONT_H - 15 - I;
            Rect.w = FONT_W + 2 + I * 2;
            Rect.h = FONT_H + 2 + I * 2;
            nErrorCode = SDL_RenderDrawRect(pRenderer,&Rect);
            // rechtes Quadrat, selected Element
            SDL_SetRenderDrawColor(pRenderer,g_PanelColorPatterns[I * 3 + 0],g_PanelColorPatterns[I * 3 + 1],g_PanelColorPatterns[I * 3 + 2], SDL_ALPHA_OPAQUE);  // Farbe für Rechteck setzen
            Rect.x = Config.uResX - 48 - I;
            Rect.y = Config.uResY - FONT_H - 15 - I;
            Rect.w = FONT_W + 2 + I * 2;
            Rect.h = FONT_H + 2 + I * 2;
            nErrorCode = SDL_RenderDrawRect(pRenderer,&Rect);
            // mittleres Quadrat, selected Element
            SDL_SetRenderDrawColor(pRenderer,g_PanelColorPatterns[I * 3 + 0],g_PanelColorPatterns[I * 3 + 1],g_PanelColorPatterns[I * 3 + 2], SDL_ALPHA_OPAQUE);  // Farbe für Rechteck setzen
            Rect.x = Config.uResX - 112 - I;
            Rect.y = Config.uResY - FONT_H - 15 - I;
            Rect.w = FONT_W + 2 + I * 2;
            Rect.h = FONT_H + 2 + I * 2;
            nErrorCode = SDL_RenderDrawRect(pRenderer,&Rect);
        }
        if (nErrorCode == 0) {
            // selected element, left
            Rect.x = Config.uResX - 177 + 1;
            Rect.y = Config.uResY - FONT_H - 15 + 1;
            Rect.w = FONT_W;
            Rect.h = FONT_H;
            uTextureIndex = GetTextureIndexByElement(Ed.uSelectedElementLeft[Ed.uMenuState],Ed.uFrameCounter % 16,&fAngle);
            nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&Rect,fAngle,NULL, SDL_FLIP_NONE);
            // selected element, right
            Rect.x = Config.uResX - 48 + 1;
            Rect.y = Config.uResY - FONT_H - 15 + 1;
            Rect.w = FONT_W;
            Rect.h = FONT_H;
            uTextureIndex = GetTextureIndexByElement(Ed.uSelectedElementRight[Ed.uMenuState],Ed.uFrameCounter % 16,&fAngle);
            nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&Rect,fAngle,NULL, SDL_FLIP_NONE);
            // selected element, middle
            Rect.x = Config.uResX - 112 + 1;
            Rect.y = Config.uResY - FONT_H - 15 + 1;
            Rect.w = FONT_W;
            Rect.h = FONT_H;
            uTextureIndex = GetTextureIndexByElement(Ed.uSelectedElementMiddle[Ed.uMenuState],Ed.uFrameCounter % 16,&fAngle);
            nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&Rect,0,NULL, SDL_FLIP_NONE);
        }
    }
    if (nErrorCode == 0) {
        nErrorCode = -1;
        if (PrintLittleFont(pRenderer,Config.uResX - 180,Config.uResY - 71,0,(char*)"SELECTED ELEMENTS",K_ABSOLUTE) == 0) {
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
Seiteneffekte: Ed.x, Config.x
------------------------------------------------------------------------------*/
int RenderEditorLevel(SDL_Renderer *pRenderer, int *pnXpos, int *pnYpos, int nAnimationCount)
{
    int nErrorCode;
    SDL_Texture *pTexture;
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
    for (uY = 0; (uY <= (Config.uResY / Ed.uFont_H) && (uY < Ed.uLevel_Y_Dimension) && (nErrorCode == 0)); uY++) {
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
                pTexture = GetTextureByIndex(GetTextureIndexByElement(uLevelElement,nAnimationCount,&fAngle));
                if (SDL_RenderCopyEx(pRenderer,pTexture,NULL,&DestR,fAngle,NULL, SDL_FLIP_NONE) != 0) {
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
Beschreibung: Wenn sich der Leveleditor in Menustate = MENUSTATE_LEVEL befindet,
              wird diese Funktion ausgeführt. Sie ermöglicht das Zeichnen von Elementen
              in einem Level.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Ed.x, InputStates.x, Config.x
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
    if ((InputStates.nMouseXpos_Absolute >= Ed.nMinXLevel) && (InputStates.nMouseXpos_Absolute <= Ed.nMaxXLevel) &&
        (InputStates.nMouseYpos_Absolute >= Ed.nMinYLevel) && (InputStates.nMouseYpos_Absolute <= Ed.nMaxYLevel)) {
        uLevelX = Ed.uUpperLeftLevelIndex % Ed.uLevel_X_Dimension + (InputStates.nMouseXpos_Absolute - Ed.uShiftLevelXpix) / Ed.uFont_W;
        uLevelY = Ed.uUpperLeftLevelIndex / Ed.uLevel_X_Dimension + (InputStates.nMouseYpos_Absolute - Ed.uShiftLevelYpix) / Ed.uFont_H;
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
                    FillLevelArea(uLevelX,uLevelY,Ed.uSelectedElementRight[Ed.uMenuState],Ed.pLevel[uLevelY * Ed.uLevel_X_Dimension + uLevelX]);
                }
            }
        }
    }
    if (CheckReplicators(Ed.pLevel,Ed.uLevel_X_Dimension,Ed.uLevel_Y_Dimension) == 0) {
        if (GetManCoordinates(Ed.pLevel,Ed.uLevel_X_Dimension,Ed.uLevel_Y_Dimension,NULL,NULL) == 0) {
            if (CheckAcidPools(Ed.pLevel,Ed.uLevel_X_Dimension,Ed.uLevel_Y_Dimension) == 0) {
                PrintLittleFont(pRenderer,Config.uResX - 640, Config.uResY - 16,0,"LEVEL: OK",K_ABSOLUTE);
            } else {
                PrintLittleFont(pRenderer,Config.uResX - 640, Config.uResY - 16,0,"LEVEL: ACID POOL ERROR",K_ABSOLUTE);
                Ed.bFoundError = true;
            }
        } else {
            PrintLittleFont(pRenderer,Config.uResX - 640, Config.uResY - 16,0,"LEVEL: MAN ERROR",K_ABSOLUTE);
            Ed.bFoundError = true;
        }
    } else {
        PrintLittleFont(pRenderer,Config.uResX - 640, Config.uResY - 16,0,"LEVEL: REPLICATOR ERROR",K_ABSOLUTE);
        Ed.bFoundError = true;
    }
    if (Ed.bHalfSize) {
        PrintLittleFont(pRenderer,Config.uResX - 832, Config.uResY - 16,0,"(Z)OOM OFF",K_ABSOLUTE);
    } else {
        PrintLittleFont(pRenderer,Config.uResX - 832, Config.uResY - 16,0,"(Z)OOM ON",K_ABSOLUTE);
    }
    return PrintLittleFont(pRenderer,Config.uResX - 350, Config.uResY - 16,0,szText,K_ABSOLUTE); // Level-Koordinaten anzeigen
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
        if ((InputStates.nMouseXpos_Absolute >= Ed.YamCoords[I].uXstart) && (InputStates.nMouseXpos_Absolute <= Ed.YamCoords[I].uXend) &&
           (InputStates.nMouseYpos_Absolute >= Ed.YamCoords[I].uYstart) && (InputStates.nMouseYpos_Absolute <= Ed.YamCoords[I].uYend)) {
            uLine = (InputStates.nMouseYpos_Absolute - Ed.YamCoords[I].uYstart) / (FONT_H / 2);
            uColumn = (InputStates.nMouseXpos_Absolute - Ed.YamCoords[I].uXstart) / (FONT_H / 2);
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
Beschreibung: Wenn sich der Leveleditor in Menustate = MENUSTATE_YAMS befindet,
              wird diese Funktion ausgeführt. Sie ermöglicht das Zeichnen von Elementen
              für explodierende Yams.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Ed.x, InputStates.x, Config.x
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
    bool bFoundError;
    bool bReplicatorError;
    uint32_t uYamErrorIndex;

    uYamErrorIndex = 0;
    bFoundError = false;
    bReplicatorError = false;
    nErrorCode = 0;
    for (I = 0; (I <= Ed.uMaxYamExplosionIndex) && (nErrorCode == 0); I++) {
        sprintf(szText,"YAM%02d",I);
        uXpos = ((I % 10) * 80) + 4;
        uYpos = ((I / 10) * 74) + 4;
        PrintLittleFont(pRenderer,uXpos,uYpos,0,szText,K_ABSOLUTE);

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
        if (!bFoundError) {
            if (CheckReplicators(Ed.YamExplosions[I].uElement,3,3) != 0) {
                uYamErrorIndex = I;
                bFoundError = true;
                bReplicatorError = true;
            }
        }
        if (!bFoundError) {
            if (CheckAcidPools(Ed.YamExplosions[I].uElement,3,3) != 0) {
                uYamErrorIndex = I;
                bFoundError = true;
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
    Ed.bFoundError = bFoundError;
    if (bFoundError) {
        if (bReplicatorError) {
            sprintf(szText,"YAM[%02u]:REPLICATOR ERROR",uYamErrorIndex);
        } else {
            sprintf(szText,"YYAM[%02u]:ACID POOL ERROR",uYamErrorIndex);
        }
        PrintLittleFont(pRenderer,Config.uResX - 740, Config.uResY - 16,0,szText,K_ABSOLUTE);
    }
    sprintf(szText,"YAMS: %02u",Ed.uMaxYamExplosionIndex);
    PrintLittleFont(pRenderer,Config.uResX  - 355,Config.uResY - 20,0,szText,K_ABSOLUTE);
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           EditorStateMachines
------------------------------------------------------------------------------
Beschreibung: Wenn sich der Leveleditor in Menustate = MENUSTATE_MACHINES befindet,
              wird diese Funktion ausgeführt. Sie ermöglicht die Konfiguration von
              Replikatoren, Lichtschranken und Laufbändern.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Ed.x, InputStates.x
------------------------------------------------------------------------------*/
int EditorStateMachines(SDL_Renderer *pRenderer) {
    int nErrorCode;
    uint32_t uTextureIndex;
    uint32_t uElement;
    uint32_t I;
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
                                        64,512,EMERALD_LIGHTBARRIER_RED_RIGHT,
                                        64,576,EMERALD_LIGHTBARRIER_YELLOW_RIGHT,
                                        64,640,EMERALD_LIGHTBARRIER_GREEN_RIGHT,
                                        64,704,EMERALD_LIGHTBARRIER_BLUE_RIGHT,
                                        // Replicator objects
                                        64 + FONT_W,64 + FONT_H,EMERALD_SPACE,
                                        64 + FONT_W,64 + 4 * FONT_H,EMERALD_SPACE,
                                        64 + FONT_W,64 + 7 * FONT_H,EMERALD_SPACE,
                                        64 + FONT_W,64 + 10 * FONT_H,EMERALD_SPACE,
                                        // Light barriers streams
                                        64 + 1 * FONT_W,64 + 14 * FONT_H,EMERALD_BEAM_RED_HORIZONTAL,
                                        64 + 2 * FONT_W,64 + 14 * FONT_H,EMERALD_BEAM_RED_HORIZONTAL,
                                        64 + 3 * FONT_W,64 + 14 * FONT_H,EMERALD_BEAM_RED_HORIZONTAL,
                                        64 + 1 * FONT_W,64 + 16 * FONT_H,EMERALD_BEAM_YELLOW_HORIZONTAL,
                                        64 + 2 * FONT_W,64 + 16 * FONT_H,EMERALD_BEAM_YELLOW_HORIZONTAL,
                                        64 + 3 * FONT_W,64 + 16 * FONT_H,EMERALD_BEAM_YELLOW_HORIZONTAL,
                                        64 + 1 * FONT_W,64 + 18 * FONT_H,EMERALD_BEAM_GREEN_HORIZONTAL,
                                        64 + 2 * FONT_W,64 + 18 * FONT_H,EMERALD_BEAM_GREEN_HORIZONTAL,
                                        64 + 3 * FONT_W,64 + 18 * FONT_H,EMERALD_BEAM_GREEN_HORIZONTAL,
                                        64 + 1 * FONT_W,64 + 20 * FONT_H,EMERALD_BEAM_BLUE_HORIZONTAL,
                                        64 + 2 * FONT_W,64 + 20 * FONT_H,EMERALD_BEAM_BLUE_HORIZONTAL,
                                        64 + 3 * FONT_W,64 + 20 * FONT_H,EMERALD_BEAM_BLUE_HORIZONTAL,
                                        672,64,EMERALD_CONVEYORBELT_RED,
                                        704,64,EMERALD_CONVEYORBELT_RED,
                                        736,64,EMERALD_CONVEYORBELT_RED,
                                        800,64,EMERALD_CONVEYORBELT_SWITCH_RED,
                                        672,160,EMERALD_CONVEYORBELT_YELLOW,
                                        704,160,EMERALD_CONVEYORBELT_YELLOW,
                                        736,160,EMERALD_CONVEYORBELT_YELLOW,
                                        800,160,EMERALD_CONVEYORBELT_SWITCH_YELLOW,
                                        672,256,EMERALD_CONVEYORBELT_GREEN,
                                        704,256,EMERALD_CONVEYORBELT_GREEN,
                                        736,256,EMERALD_CONVEYORBELT_GREEN,
                                        800,256,EMERALD_CONVEYORBELT_SWITCH_GREEN,
                                        672,352,EMERALD_CONVEYORBELT_BLUE,
                                        704,352,EMERALD_CONVEYORBELT_BLUE,
                                        736,352,EMERALD_CONVEYORBELT_BLUE,
                                        800,352,EMERALD_CONVEYORBELT_SWITCH_BLUE,
                                        };

    uPositionsAndElements[32 * 3 + 2] = Ed.uReplicatorRedObject;
    uPositionsAndElements[33 * 3 + 2] = Ed.uReplicatorYellowObject;
    uPositionsAndElements[34 * 3 + 2] = Ed.uReplicatorGreenObject;
    uPositionsAndElements[35 * 3 + 2] = Ed.uReplicatorBlueObject;;
    if (!Ed.bLightBarrierRedOn) {
        uPositionsAndElements[36 * 3 + 2] = EMERALD_SPACE;
        uPositionsAndElements[37 * 3 + 2] = EMERALD_SPACE;
        uPositionsAndElements[38 * 3 + 2] = EMERALD_SPACE;
    }
    if (!Ed.bLightBarrierYellowOn) {
        uPositionsAndElements[39 * 3 + 2] = EMERALD_SPACE;
        uPositionsAndElements[40 * 3 + 2] = EMERALD_SPACE;
        uPositionsAndElements[41 * 3 + 2] = EMERALD_SPACE;
    }
    if (!Ed.bLightBarrierGreenOn) {
        uPositionsAndElements[42 * 3 + 2] = EMERALD_SPACE;
        uPositionsAndElements[43 * 3 + 2] = EMERALD_SPACE;
        uPositionsAndElements[44 * 3 + 2] = EMERALD_SPACE;
    }
    if (!Ed.bLightBarrierBlueOn) {
        uPositionsAndElements[45 * 3 + 2] = EMERALD_SPACE;
        uPositionsAndElements[46 * 3 + 2] = EMERALD_SPACE;
        uPositionsAndElements[47 * 3 + 2] = EMERALD_SPACE;
    }

    // Replikatoren
    PrintLittleFont(pRenderer,80,24,0,"REPLICATORS",K_ABSOLUTE);
    PrintLittleFont(pRenderer,192,112,0,"ACTIVE AT START:",K_ABSOLUTE);
    if (Ed.bReplicatorRedOn) {
        PrintLittleFont(pRenderer,352,112,0,"YES",K_ABSOLUTE);
    } else {
        PrintLittleFont(pRenderer,352,112,0,"NO",K_ABSOLUTE);
    }
    PrintLittleFont(pRenderer,192,208,0,"ACTIVE AT START:",K_ABSOLUTE);
    if (Ed.bReplicatorYellowOn) {
        PrintLittleFont(pRenderer,352,208,0,"YES",K_ABSOLUTE);
    } else {
        PrintLittleFont(pRenderer,352,208,0,"NO",K_ABSOLUTE);
    }
    PrintLittleFont(pRenderer,192,304,0,"ACTIVE AT START:",K_ABSOLUTE);
    if (Ed.bReplicatorGreenOn) {
        PrintLittleFont(pRenderer,352,304,0,"YES",K_ABSOLUTE);
    } else {
        PrintLittleFont(pRenderer,352,304,0,"NO",K_ABSOLUTE);
    }
    PrintLittleFont(pRenderer,192,400,0,"ACTIVE AT START:",K_ABSOLUTE);
    if (Ed.bReplicatorBlueOn) {
        PrintLittleFont(pRenderer,352,400,0,"YES",K_ABSOLUTE);
    } else {
        PrintLittleFont(pRenderer,352,400,0,"NO",K_ABSOLUTE);
    }
    // Lichtschranken
    PrintLittleFont(pRenderer,80,464,0,"LIGHT BARRIERS",K_ABSOLUTE);
    PrintLittleFont(pRenderer,240,520,0,"ACTIVE AT START:",K_ABSOLUTE);
    if (Ed.bLightBarrierRedOn) {
        PrintLittleFont(pRenderer,400,520,0,"YES",K_ABSOLUTE);
    } else {
        PrintLittleFont(pRenderer,400,520,0,"NO",K_ABSOLUTE);
    }
    PrintLittleFont(pRenderer,240,584,0,"ACTIVE AT START:",K_ABSOLUTE);
    if (Ed.bLightBarrierYellowOn) {
        PrintLittleFont(pRenderer,400,584,0,"YES",K_ABSOLUTE);
    } else {
        PrintLittleFont(pRenderer,400,584,0,"NO",K_ABSOLUTE);
    }
    PrintLittleFont(pRenderer,240,648,0,"ACTIVE AT START:",K_ABSOLUTE);
    if (Ed.bLightBarrierGreenOn) {
        PrintLittleFont(pRenderer,400,648,0,"YES",K_ABSOLUTE);
    } else {
        PrintLittleFont(pRenderer,400,648,0,"NO",K_ABSOLUTE);
    }
    PrintLittleFont(pRenderer,240,712,0,"ACTIVE AT START:",K_ABSOLUTE);
    if (Ed.bLightBarrierBlueOn) {
        nErrorCode = PrintLittleFont(pRenderer,400,712,0,"YES",K_ABSOLUTE);
    } else {
        nErrorCode = PrintLittleFont(pRenderer,400,712,0,"NO",K_ABSOLUTE);
    }
    // Laufbänder
    PrintLittleFont(pRenderer,682,24,0,"CONVEYOR BELTS",K_ABSOLUTE);
    PrintLittleFont(pRenderer,848,72,0,"ACTIVE AT START:",K_ABSOLUTE);
    if (Ed.uConveybeltRedState == EMERALD_CONVEYBELT_LEFT) {
        PrintLittleFont(pRenderer,848 + 160,72,0,"LEFT",K_ABSOLUTE);
    } else if (Ed.uConveybeltRedState == EMERALD_CONVEYBELT_RIGHT) {
        PrintLittleFont(pRenderer,848 + 160,72,0,"RIGHT",K_ABSOLUTE);
    } else {
        PrintLittleFont(pRenderer,848 + 160,72,0,"NO",K_ABSOLUTE);
        if (Ed.uConveybeltRedDirection == EMERALD_CONVEYBELT_TO_LEFT) {
            PrintLittleFont(pRenderer,672,110,0,"NEXT DIRECTION:LEFT",K_ABSOLUTE);
        } else {
            PrintLittleFont(pRenderer,672,110,0,"NEXT DIRECTION:RIGHT",K_ABSOLUTE);
        }
    }
    PrintLittleFont(pRenderer,848,168,0,"ACTIVE AT START:",K_ABSOLUTE);
    if (Ed.uConveybeltYellowState == EMERALD_CONVEYBELT_LEFT) {
        PrintLittleFont(pRenderer,848 + 160,168,0,"LEFT",K_ABSOLUTE);
    } else if (Ed.uConveybeltYellowState == EMERALD_CONVEYBELT_RIGHT) {
        PrintLittleFont(pRenderer,848 + 160,168,0,"RIGHT",K_ABSOLUTE);
    } else {
        PrintLittleFont(pRenderer,848 + 160,168,0,"NO",K_ABSOLUTE);
        if (Ed.uConveybeltYellowDirection == EMERALD_CONVEYBELT_TO_LEFT) {
            PrintLittleFont(pRenderer,672,206,0,"NEXT DIRECTION:LEFT",K_ABSOLUTE);
        } else {
            PrintLittleFont(pRenderer,672,206,0,"NEXT DIRECTION:RIGHT",K_ABSOLUTE);
        }
    }
    PrintLittleFont(pRenderer,848,264,0,"ACTIVE AT START:",K_ABSOLUTE);
    if (Ed.uConveybeltGreenState == EMERALD_CONVEYBELT_LEFT) {
        PrintLittleFont(pRenderer,848 + 160,264,0,"LEFT",K_ABSOLUTE);
    } else if (Ed.uConveybeltGreenState == EMERALD_CONVEYBELT_RIGHT) {
        PrintLittleFont(pRenderer,848 + 160,264,0,"RIGHT",K_ABSOLUTE);
    } else {
        PrintLittleFont(pRenderer,848 + 160,264,0,"NO",K_ABSOLUTE);
        if (Ed.uConveybeltGreenDirection == EMERALD_CONVEYBELT_TO_LEFT) {
            PrintLittleFont(pRenderer,672,302,0,"NEXT DIRECTION:LEFT",K_ABSOLUTE);
        } else {
            PrintLittleFont(pRenderer,672,302,0,"NEXT DIRECTION:RIGHT",K_ABSOLUTE);
        }
    }
    PrintLittleFont(pRenderer,848,360,0,"ACTIVE AT START:",K_ABSOLUTE);
    if (Ed.uConveybeltBlueState == EMERALD_CONVEYBELT_LEFT) {
        PrintLittleFont(pRenderer,848 + 160,360,0,"LEFT",K_ABSOLUTE);
    } else if (Ed.uConveybeltBlueState == EMERALD_CONVEYBELT_RIGHT) {
        PrintLittleFont(pRenderer,848 + 160,360,0,"RIGHT",K_ABSOLUTE);
    } else {
        PrintLittleFont(pRenderer,848 + 160,360,0,"NO",K_ABSOLUTE);
        if (Ed.uConveybeltBlueDirection == EMERALD_CONVEYBELT_TO_LEFT) {
            PrintLittleFont(pRenderer,672,398,0,"NEXT DIRECTION:LEFT",K_ABSOLUTE);
        } else {
            PrintLittleFont(pRenderer,672,398,0,"NEXT DIRECTION:RIGHT",K_ABSOLUTE);
        }
    }
    for (I = 0; (I < sizeof(uPositionsAndElements) / (3 * sizeof(uint32_t))) && (nErrorCode == 0); I++) {
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
        } else if ((uElement == EMERALD_CONVEYORBELT_SWITCH_RED) && (Ed.uConveybeltRedState == EMERALD_CONVEYBELT_LEFT)) {
            uTextureIndex++;
        } else if ((uElement == EMERALD_CONVEYORBELT_SWITCH_RED) && (Ed.uConveybeltRedState == EMERALD_CONVEYBELT_RIGHT)) {
            uTextureIndex = uTextureIndex + 2;
        } else if ((uElement == EMERALD_CONVEYORBELT_SWITCH_YELLOW) && (Ed.uConveybeltYellowState == EMERALD_CONVEYBELT_LEFT)) {
            uTextureIndex++;
        } else if ((uElement == EMERALD_CONVEYORBELT_SWITCH_YELLOW) && (Ed.uConveybeltYellowState == EMERALD_CONVEYBELT_RIGHT)) {
            uTextureIndex = uTextureIndex + 2;
        } else if ((uElement == EMERALD_CONVEYORBELT_SWITCH_GREEN) && (Ed.uConveybeltGreenState == EMERALD_CONVEYBELT_LEFT)) {
            uTextureIndex++;
        } else if ((uElement == EMERALD_CONVEYORBELT_SWITCH_GREEN) && (Ed.uConveybeltGreenState == EMERALD_CONVEYBELT_RIGHT)) {
            uTextureIndex = uTextureIndex + 2;
        } else if ((uElement == EMERALD_CONVEYORBELT_SWITCH_BLUE) && (Ed.uConveybeltBlueState == EMERALD_CONVEYBELT_LEFT)) {
            uTextureIndex++;
        } else if ((uElement == EMERALD_CONVEYORBELT_SWITCH_BLUE) && (Ed.uConveybeltBlueState == EMERALD_CONVEYBELT_RIGHT)) {
            uTextureIndex = uTextureIndex + 2;
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
    // Dieser Block kann die Aktivität der Laufbänder umschalten
    if (((InputStates.nMouseXpos_Absolute >= 800) && (InputStates.nMouseXpos_Absolute < (800 + FONT_W))) && (InputStates.bLeftMouseButton)) {
        if ((InputStates.nMouseYpos_Absolute >= 64) && (InputStates.nMouseYpos_Absolute < (64 + FONT_H))) {
            switch (Ed.uConveybeltRedState) {
                case(EMERALD_CONVEYBELT_OFF):
                    Ed.uConveybeltRedState = EMERALD_CONVEYBELT_RIGHT;
                    break;
                case(EMERALD_CONVEYBELT_LEFT):
                    Ed.uConveybeltRedState = EMERALD_CONVEYBELT_OFF;
                    if (Ed.uConveybeltRedDirection == EMERALD_CONVEYBELT_TO_LEFT) {
                        Ed.uConveybeltRedDirection = EMERALD_CONVEYBELT_TO_RIGHT;
                    } else {
                        Ed.uConveybeltRedDirection = EMERALD_CONVEYBELT_TO_LEFT;
                    }
                    break;
                case(EMERALD_CONVEYBELT_RIGHT):
                    Ed.uConveybeltRedState = EMERALD_CONVEYBELT_LEFT;
                    break;
            }
            WaitNoKey();
        } else if ((InputStates.nMouseYpos_Absolute >= 160) && (InputStates.nMouseYpos_Absolute < (160 + FONT_H))) {
            switch (Ed.uConveybeltYellowState) {
                case(EMERALD_CONVEYBELT_OFF):
                    Ed.uConveybeltYellowState = EMERALD_CONVEYBELT_RIGHT;
                    break;
                case(EMERALD_CONVEYBELT_LEFT):
                    Ed.uConveybeltYellowState = EMERALD_CONVEYBELT_OFF;
                    if (Ed.uConveybeltYellowDirection == EMERALD_CONVEYBELT_TO_LEFT) {
                        Ed.uConveybeltYellowDirection = EMERALD_CONVEYBELT_TO_RIGHT;
                    } else {
                        Ed.uConveybeltYellowDirection = EMERALD_CONVEYBELT_TO_LEFT;
                    }
                    break;
                case(EMERALD_CONVEYBELT_RIGHT):
                    Ed.uConveybeltYellowState = EMERALD_CONVEYBELT_LEFT;
                    break;
            }
            WaitNoKey();
        } else if ((InputStates.nMouseYpos_Absolute >= 256) && (InputStates.nMouseYpos_Absolute < (256 + FONT_H))) {
            switch (Ed.uConveybeltGreenState) {
                case(EMERALD_CONVEYBELT_OFF):
                    Ed.uConveybeltGreenState = EMERALD_CONVEYBELT_RIGHT;
                    break;
                case(EMERALD_CONVEYBELT_LEFT):
                    Ed.uConveybeltGreenState = EMERALD_CONVEYBELT_OFF;
                    if (Ed.uConveybeltGreenDirection == EMERALD_CONVEYBELT_TO_LEFT) {
                        Ed.uConveybeltGreenDirection = EMERALD_CONVEYBELT_TO_RIGHT;
                    } else {
                        Ed.uConveybeltGreenDirection = EMERALD_CONVEYBELT_TO_LEFT;
                    }
                    break;
                case(EMERALD_CONVEYBELT_RIGHT):
                    Ed.uConveybeltGreenState = EMERALD_CONVEYBELT_LEFT;
                    break;
            }
            WaitNoKey();
        } else if ((InputStates.nMouseYpos_Absolute >= 352) && (InputStates.nMouseYpos_Absolute < (352 + FONT_H))) {
            switch (Ed.uConveybeltBlueState) {
                case(EMERALD_CONVEYBELT_OFF):
                    Ed.uConveybeltBlueState = EMERALD_CONVEYBELT_RIGHT;
                    break;
                case(EMERALD_CONVEYBELT_LEFT):
                    Ed.uConveybeltBlueState = EMERALD_CONVEYBELT_OFF;
                    if (Ed.uConveybeltBlueDirection == EMERALD_CONVEYBELT_TO_LEFT) {
                        Ed.uConveybeltBlueDirection = EMERALD_CONVEYBELT_TO_RIGHT;
                    } else {
                        Ed.uConveybeltBlueDirection = EMERALD_CONVEYBELT_TO_LEFT;
                    }
                    break;
                case(EMERALD_CONVEYBELT_RIGHT):
                    Ed.uConveybeltBlueState = EMERALD_CONVEYBELT_LEFT;
                    break;
            }
            WaitNoKey();
        }
    }

    // Dieser Block kann die Aktivität der Replikatoren umschalten
    if (((InputStates.nMouseXpos_Absolute >= 192) && (InputStates.nMouseXpos_Absolute <= 224)) && (InputStates.bLeftMouseButton)) {
        if ((InputStates.nMouseYpos_Absolute >= 64) && (InputStates.nMouseYpos_Absolute <= 96)) {
            Ed.bReplicatorRedOn = !Ed.bReplicatorRedOn;
            WaitNoKey();
        } else if ((InputStates.nMouseYpos_Absolute >= 160) && (InputStates.nMouseYpos_Absolute <= 192)) {
            Ed.bReplicatorYellowOn = !Ed.bReplicatorYellowOn;
            WaitNoKey();
        } else if ((InputStates.nMouseYpos_Absolute >= 256) && (InputStates.nMouseYpos_Absolute <= 288)) {
            Ed.bReplicatorGreenOn = !Ed.bReplicatorGreenOn;
            WaitNoKey();
        } else if ((InputStates.nMouseYpos_Absolute >= 352) && (InputStates.nMouseYpos_Absolute <= 384)) {
            Ed.bReplicatorBlueOn = !Ed.bReplicatorBlueOn;
            WaitNoKey();
        // Dieser Block kann die Aktivität der Lichtschranken umschalten
        } else if ((InputStates.nMouseYpos_Absolute >= 512) && (InputStates.nMouseYpos_Absolute <= 544)) {
            Ed.bLightBarrierRedOn = !Ed.bLightBarrierRedOn;
            WaitNoKey();
        } else if ((InputStates.nMouseYpos_Absolute >= 576) && (InputStates.nMouseYpos_Absolute <= 608)) {
            Ed.bLightBarrierYellowOn = !Ed.bLightBarrierYellowOn;
            WaitNoKey();
        } else if ((InputStates.nMouseYpos_Absolute >= 640) && (InputStates.nMouseYpos_Absolute <= 672)) {
            Ed.bLightBarrierGreenOn = !Ed.bLightBarrierGreenOn;
            WaitNoKey();
        } else if ((InputStates.nMouseYpos_Absolute >= 704) && (InputStates.nMouseYpos_Absolute <= 736)) {
            Ed.bLightBarrierBlueOn = !Ed.bLightBarrierBlueOn;
            WaitNoKey();
        }
    }
    // Dieser Block kann das Objekt der Replikatoren umschalten
    bXmouseReplicatorObject = ((InputStates.nMouseXpos_Absolute >= 96) && (InputStates.nMouseXpos_Absolute <= 128));
    bYmouseRedReplicator = ((InputStates.nMouseYpos_Absolute >= 96) && (InputStates.nMouseYpos_Absolute <= 128));
    bYmouseYellowReplicator = ((InputStates.nMouseYpos_Absolute >= 192) && (InputStates.nMouseYpos_Absolute <= 224));;
    bYmouseGreenReplicator = ((InputStates.nMouseYpos_Absolute >= 288) && (InputStates.nMouseYpos_Absolute <= 320));;
    bYmouseBlueReplicator = ((InputStates.nMouseYpos_Absolute >= 384) && (InputStates.nMouseYpos_Absolute <= 416));;
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
Beschreibung: Wenn sich der Leveleditor in Menustate = MENUSTATE_TIME_AND_SCORES befindet,
              wird diese Funktion ausgeführt. Sie ermöglicht die Konfiguration von
              Zeiten und Scores.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Ed.x, InputStates.x
------------------------------------------------------------------------------*/
int EditorStateTimeAndScores(SDL_Renderer *pRenderer) {
    int nErrorCode;
    uint32_t uKey;
    uint32_t I;
    uint32_t uCursorPos;
    int nE;
    uint32_t uMinX,uMaxX;
    uint32_t uMinY,uMaxY;
    int nSwitchField;
    uint32_t uSwitchFieldOffset;
    uint32_t uTextureIndex;
    char szText[128];
    char szCursor[2];
    float fAngle;
    SDL_Rect DestR;
    uint32_t uPositionsAndElements[] = {// 1. Zeile
                                        32,64,EMERALD_EMERALD,
                                        64,64,EMERALD_STEEL_ARROW_RIGHT,
                                        192,64,EMERALD_RUBY,
                                        224,64,EMERALD_STEEL_ARROW_RIGHT,
                                        352,64,EMERALD_SAPPHIRE,
                                        384,64,EMERALD_STEEL_ARROW_RIGHT,
                                        512,64,EMERALD_PERL,
                                        544,64,EMERALD_STEEL_ARROW_RIGHT,
                                        672,64,EMERALD_CRYSTAL,
                                        704,64,EMERALD_STEEL_ARROW_RIGHT,
                                        // Messages 1 - 8
                                        896,64,EMERALD_FONT_GREEN_1,
                                        928,64,EMERALD_MESSAGE_1,
                                        896,128,EMERALD_FONT_GREEN_2,
                                        928,128,EMERALD_MESSAGE_2,
                                        896,192,EMERALD_FONT_GREEN_3,
                                        928,192,EMERALD_MESSAGE_3,
                                        896,256,EMERALD_FONT_GREEN_4,
                                        928,256,EMERALD_MESSAGE_4,
                                        896,320,EMERALD_FONT_GREEN_5,
                                        928,320,EMERALD_MESSAGE_5,
                                        896,384,EMERALD_FONT_GREEN_6,
                                        928,384,EMERALD_MESSAGE_6,
                                        896,448,EMERALD_FONT_GREEN_7,
                                        928,448,EMERALD_MESSAGE_7,
                                        896,512,EMERALD_FONT_GREEN_8,
                                        928,512,EMERALD_MESSAGE_8,
                                        // 2. Zeile
                                        32,128,EMERALD_MESSAGE_1,
                                        64,128,EMERALD_STEEL_ARROW_RIGHT,
                                        192,128,EMERALD_KEY_RED,
                                        224,128,EMERALD_STEEL_ARROW_RIGHT,
                                        352,128,EMERALD_DYNAMITE_OFF,
                                        384,128,EMERALD_STEEL_ARROW_RIGHT,
                                        512,128,EMERALD_HAMMER,
                                        544,128,EMERALD_STEEL_ARROW_RIGHT,
                                        672,128,EMERALD_TIME_COIN,
                                        704,128,EMERALD_STEEL_ARROW_RIGHT,
                                        // 3.Zeile
                                        32,192,EMERALD_NUT,
                                        64,192,EMERALD_STEEL_ARROW_RIGHT,
                                        192,192,EMERALD_BEETLE_LEFT,
                                        224,192,EMERALD_STEEL_ARROW_RIGHT,
                                        352,192,EMERALD_MINE_LEFT,
                                        384,192,EMERALD_STEEL_ARROW_RIGHT,
                                        512,192,EMERALD_ALIEN,
                                        544,192,EMERALD_STEEL_ARROW_RIGHT,
                                        672,192,EMERALD_YAM,
                                        704,192,EMERALD_STEEL_ARROW_RIGHT,
                                        // 4.Zeile
                                        32,288,EMERALD_MAN,
                                        64,288,EMERALD_STEEL_ARROW_RIGHT,
                                        192,288,EMERALD_WHEEL,
                                        224,288,EMERALD_STEEL_ARROW_RIGHT,
                                        352,288,EMERALD_MAGIC_WALL,
                                        384,288,EMERALD_STEEL_ARROW_RIGHT,
                                        512,288,EMERALD_LIGHT_SWITCH,
                                        544,288,EMERALD_STEEL_ARROW_RIGHT,
                                        672,288,EMERALD_DOOR_TIME,
                                        704,288,EMERALD_STEEL_ARROW_RIGHT,
                                        // 5.Zeile
                                        32,352,EMERALD_TIME_COIN,
                                        64,352,EMERALD_STEEL_ARROW_RIGHT,
                                        352,352,EMERALD_DYNAMITE_OFF,
                                        384,352,EMERALD_STEEL_ARROW_RIGHT,
                                        512,352,EMERALD_HAMMER,
                                        544,352,EMERALD_STEEL_ARROW_RIGHT,
                                        672,352,EMERALD_KEY_WHITE,
                                        704,352,EMERALD_STEEL_ARROW_RIGHT,
                                        // 6.Zeile
                                        352,416,EMERALD_GREEN_CHEESE,
                                        512,416,EMERALD_GRASS,
                                        // 7. Zeile
                                        32,480,EMERALD_EMERALD,
                                        64,480,EMERALD_RUBY,
                                        96,480,EMERALD_SAPPHIRE,
                                        128,480,EMERALD_PERL,
                                        160,480,EMERALD_CRYSTAL,
                                        192,480,EMERALD_STEEL_ARROW_RIGHT,
                                        // Stahl-Quadrat
                                        512,480,EMERALD_STEEL_INVISIBLE,  // oben links
                                        544,480,EMERALD_STEEL_INVISIBLE,  // oben X--->
                                        576,480,EMERALD_STEEL_INVISIBLE,
                                        608,480,EMERALD_STEEL_INVISIBLE,
                                        640,480,EMERALD_STEEL_INVISIBLE,
                                        672,480,EMERALD_STEEL_INVISIBLE,
                                        704,480,EMERALD_STEEL_INVISIBLE,
                                        736,480,EMERALD_STEEL_INVISIBLE,
                                        768,480,EMERALD_STEEL_INVISIBLE,
                                        512,512,EMERALD_STEEL_INVISIBLE,  // links Y
                                        512,544,EMERALD_STEEL_INVISIBLE,
                                        512,576,EMERALD_STEEL_INVISIBLE,
                                        512,608,EMERALD_STEEL_INVISIBLE,
                                        512,640,EMERALD_STEEL_INVISIBLE,
                                        512,672,EMERALD_STEEL_INVISIBLE,
                                        512,704,EMERALD_STEEL_INVISIBLE,
                                        544,704,EMERALD_STEEL_INVISIBLE,  // unten X--->
                                        576,704,EMERALD_STEEL_INVISIBLE,
                                        608,704,EMERALD_STEEL_INVISIBLE,
                                        640,704,EMERALD_STEEL_INVISIBLE,
                                        672,704,EMERALD_STEEL_INVISIBLE,
                                        704,704,EMERALD_STEEL_INVISIBLE,
                                        736,704,EMERALD_STEEL_INVISIBLE,
                                        768,704,EMERALD_STEEL_INVISIBLE,
                                        768,512,EMERALD_STEEL_INVISIBLE,  // rechts Y
                                        768,544,EMERALD_STEEL_INVISIBLE,
                                        768,576,EMERALD_STEEL_INVISIBLE,
                                        768,608,EMERALD_STEEL_INVISIBLE,
                                        768,640,EMERALD_STEEL_INVISIBLE,
                                        768,672,EMERALD_STEEL_INVISIBLE,
                                        768,704,EMERALD_STEEL_INVISIBLE,
                                        // Dimension
                                        560,544,EMERALD_FONT_X,
                                        592,544,EMERALD_FONT_DOUBLE_POINT,
                                        624,544,EMERALD_FONT_0,             // 88. Element
                                        656,544,EMERALD_FONT_0,
                                        688,544,EMERALD_FONT_0,
                                        720,544,EMERALD_FONT_0,
                                        // Pfeil hoch über X
                                        624,512,EMERALD_STEEL_ARROW_UP,
                                        656,512,EMERALD_STEEL_ARROW_UP,
                                        688,512,EMERALD_STEEL_ARROW_UP,
                                        720,512,EMERALD_STEEL_ARROW_UP,
                                        // Pfeil runter unter X
                                        624,576,EMERALD_STEEL_ARROW_DOWN,
                                        656,576,EMERALD_STEEL_ARROW_DOWN,
                                        688,576,EMERALD_STEEL_ARROW_DOWN,
                                        720,576,EMERALD_STEEL_ARROW_DOWN,
                                        // Pfeil hoch über Y
                                        624,608,EMERALD_STEEL_ARROW_UP,
                                        656,608,EMERALD_STEEL_ARROW_UP,
                                        688,608,EMERALD_STEEL_ARROW_UP,
                                        720,608,EMERALD_STEEL_ARROW_UP,
                                        // Pfeil runter unter Y
                                        624,672,EMERALD_STEEL_ARROW_DOWN,
                                        656,672,EMERALD_STEEL_ARROW_DOWN,
                                        688,672,EMERALD_STEEL_ARROW_DOWN,
                                        720,672,EMERALD_STEEL_ARROW_DOWN,
                                        560,640,EMERALD_FONT_Y,
                                        592,640,EMERALD_FONT_DOUBLE_POINT,
                                        624,640,EMERALD_FONT_0,             // 110. Element
                                        656,640,EMERALD_FONT_0,
                                        688,640,EMERALD_FONT_0,
                                        720,640,EMERALD_FONT_0,
                                        };
                // Aufbau:  Xmin,Xmax,Ymin,Ymax
    uint32_t uSwitches[] = {100,109,60,70,110,119,60,70,120,129,60,70,130,139,60,70, // ++++ Emerald
                            100,109,86,96,110,119,86,96,120,129,86,96,130,139,86,96, // ---- Emerald
                            260,269,60,70,270,279,60,70,280,289,60,70,290,299,60,70, // ++++ Rubin
                            260,269,86,96,270,279,86,96,280,289,86,96,290,299,86,96, // ---- Rubin
                            420,429,60,70,430,439,60,70,440,449,60,70,450,459,60,70, // ++++ Saphir
                            420,429,86,96,430,439,86,96,440,449,86,96,450,459,86,96, // ---- Saphir
                            580,589,60,70,590,599,60,70,600,609,60,70,610,619,60,70, // ++++ Perle
                            580,589,86,96,590,599,86,96,600,609,86,96,610,619,86,96, // ---- Perle
                            740,749,60,70,750,759,60,70,760,769,60,70,770,779,60,70, // ++++ Kristal
                            740,749,86,96,750,759,86,96,760,769,86,96,770,779,86,96, // ---- Kristal
                            // 2.Zeile
                            100,109,124,134,110,119,124,134,120,129,124,134,130,139,124,134, // ++++ Nachricht
                            100,109,150,160,110,119,150,160,120,129,150,160,130,139,150,160, // ---- Nachricht
                            260,269,124,134,270,279,124,134,280,289,124,134,290,299,124,134, // ++++ Schlüssel
                            260,269,150,160,270,279,150,160,280,289,150,160,290,299,150,160, // ---- Schlüssel
                            420,429,124,134,430,439,124,134,440,449,124,134,450,459,124,134, // ++++ Dynamit
                            420,429,150,160,430,439,150,160,440,449,150,160,450,459,150,160, // ---- Dynamit
                            580,589,124,134,590,599,124,134,600,609,124,134,610,619,124,134, // ++++ Hammer
                            580,589,150,160,590,599,150,160,600,609,150,160,610,619,150,160, // ---- Hammer
                            740,749,124,134,750,759,124,134,760,769,124,134,770,779,124,134, // ++++ Time-Coin
                            740,749,150,160,750,759,150,160,760,769,150,160,770,779,150,160, // ---- Time-Coin
                            // 3. Zeile
                            100,109,188,198,110,119,188,198,120,129,188,198,130,139,188,198, // ++++ Nuss
                            100,109,214,224,110,119,214,224,120,129,214,224,130,139,214,224, // ---- Nuss
                            260,269,188,198,270,279,188,198,280,289,188,198,290,299,188,198, // ++++ Käfer
                            260,269,214,224,270,279,214,224,280,289,214,224,290,299,214,224, // ---- Käfer
                            420,429,188,198,430,439,188,198,440,449,188,198,450,459,188,198, // ++++ Mine
                            420,429,214,224,430,439,214,224,440,449,214,224,450,459,214,224, // ---- Mine
                            580,589,188,198,590,599,188,198,600,609,188,198,610,619,188,198, // ++++ Alien
                            580,589,214,224,590,599,214,224,600,609,214,224,610,619,214,224, // ---- Alien
                            740,749,188,198,750,759,188,198,760,769,188,198,770,779,188,198, // ++++ Yam
                            740,749,214,224,750,759,214,224,760,769,214,224,770,779,214,224, // ---- Yam
                            // 4. Zeile
                            100,109,286,296,110,119,286,296,120,129,286,296,130,139,286,296, // ++++ Time to play
                            100,109,312,322,110,119,312,322,120,129,312,322,130,139,312,322, // ---- Time to play
                            260,269,286,296,270,279,286,296,280,289,286,296,290,299,286,296, // ++++ Wheel rotation
                            260,269,312,322,270,279,312,322,280,289,312,322,290,299,312,322, // ---- Wheel rotation
                            420,429,286,296,430,439,286,296,440,449,286,296,450,459,286,296, // ++++ Magic Wall
                            420,429,312,322,430,439,312,322,440,449,312,322,450,459,312,322, // ---- Magic Wall
                            580,589,286,296,590,599,286,296,600,609,286,296,610,619,286,296, // ++++ Licht
                            580,589,312,322,590,599,312,322,600,609,312,322,610,619,312,322, // ---- Licht
                            740,749,286,296,750,759,286,296,760,769,286,296,770,779,286,296, // ++++ Door time
                            740,749,312,322,750,759,312,322,760,769,312,322,770,779,312,322, // ---- Door time
                            // 5. Zeile
                            100,109,350,360,110,119,350,360,120,129,350,360,130,139,350,360, // ++++ Additional Time
                            100,109,376,386,110,119,376,386,120,129,376,386,130,139,376,386, // ---- Additional Time
                            420,429,350,360,430,439,350,360,440,449,350,360,450,459,350,360, // ++++ Dynamite Count
                            420,429,376,386,430,439,376,386,440,449,376,386,450,459,376,386, // ---- Dynamite Count
                            580,589,350,360,590,599,350,360,600,609,350,360,610,619,350,360, // ++++ Hammer Count
                            580,589,376,386,590,599,376,386,600,609,376,386,610,619,376,386, // ---- Hammer Count
                            740,749,350,360,750,759,350,360,760,769,350,360,770,779,350,360, // ++++ White Keys
                            740,749,376,386,750,759,376,386,760,769,376,386,770,779,376,386, // ---- White Keys
                            // Remaing time factor & Cheese spread speed
                            252,261,411,421,262,271,411,421,272,281,411,421,282,291,411,421, // ++++ Remaining time factor
                            252,261,437,447,262,271,437,447,272,281,437,447,282,291,437,447, // ---- Remaining time factor
                            446,455,411,421,456,465,411,421,466,475,411,421,476,485,411,421, // ++++ Cheese spread speed
                            446,455,437,447,456,465,437,447,466,475,437,447,476,485,437,447, // ---- Cheese spread speed
                            // Emerald to collect
                            350,359,475,485,360,369,475,485,370,379,475,485,380,389,475,485, // ++++ Emeralds to collect
                            350,359,501,511,360,369,501,511,370,379,501,511,380,389,501,511, // ---- Emeralds to collect
                            // Level-Dimension X
                            624,655,512,543,656,687,512,543,688,719,512,543,720,751,512,543, // ++++ Level-Dimension X
                            624,655,576,607,656,687,576,607,688,719,576,607,720,751,576,607, // ---- Level-Dimension X
                            // Level-Dimension Y
                            624,655,608,639,656,687,608,639,688,719,608,639,720,751,608,639, // ++++ Level-Dimension Y
                            624,655,672,703,656,687,672,703,688,719,672,703,720,751,672,703, // ---- Level-Dimension Y
                            // Message 1, Message 2       Message3        Message 4       Message 5       Message6        Message 7       Message 8
                            896,960,64,96,896,960,128,160,896,960,192,224,896,960,256,288,896,960,320,352,896,960,384,416,896,960,448,480,896,960,512,544,
                            // Grass spread speed
                            606,615,411,421,616,625,411,421,626,635,411,421,636,645,411,421, // ++++ Grass spread speed
                            606,615,437,447,616,625,437,447,626,635,437,447,636,645,437,447, // ---- Grass spread speed
                            };

    // Rahmen und Füllung für Level-Title und Level Author zeichnen
    // Rechtecke in Farbe 20,200,20 zeichnen
    if (Ed.uMenuState != MENUSTATE_TIME_AND_SCORES_MESSAGE) {
        SDL_SetRenderDrawColor(pRenderer,20,200,20, SDL_ALPHA_OPAQUE);  // Farbe für Rechteck setzen
        DestR.x = 28;
        DestR.y = 572;
        DestR.w = (EMERALD_TITLE_LEN + 15) * FONT_LITTLE_347_W;
        DestR.h = FONT_LITTLE_347_H + 8;
        SDL_RenderDrawRect(pRenderer,&DestR);
        DestR.x = 28;
        DestR.y = 668;
        DestR.w = (EMERALD_AUTHOR_LEN + 15) * FONT_LITTLE_347_W;
        DestR.h = FONT_LITTLE_347_H + 8;
        nErrorCode = SDL_RenderDrawRect(pRenderer,&DestR);
        // Senkrechte Linien hinter Doppelpunkt setzen
        SDL_RenderDrawLine(pRenderer,31 + 13 * FONT_LITTLE_347_W,572,31 + 13 *FONT_LITTLE_347_W,572 + FONT_LITTLE_347_H + 6); // Level-Title
        SDL_RenderDrawLine(pRenderer,31 + 13 * FONT_LITTLE_347_W,668,31 + 13 *FONT_LITTLE_347_W,668 + FONT_LITTLE_347_H + 6); // Level-Author
        // Untergründe für Title und Author (vor Doppelunkt) zeichnen
        DrawBeam(pRenderer,29, 573, 13 * FONT_LITTLE_347_W + 2,FONT_LITTLE_347_H + 6, 0,0,255,255,K_ABSOLUTE); // Level-Title
        DrawBeam(pRenderer,29, 669, 13 * FONT_LITTLE_347_W + 2,FONT_LITTLE_347_H + 6, 0,0,255,255,K_ABSOLUTE); // Level-Title
        // Untergründe für Title und Author (nach Doppelunkt) zeichnen
        DrawBeam(pRenderer,32 + 13 * FONT_LITTLE_347_W, 573, (EMERALD_TITLE_LEN + 1) * FONT_LITTLE_347_W + 5,FONT_LITTLE_347_H + 6, 30,30,255,255,K_ABSOLUTE); // Level-Title
        DrawBeam(pRenderer,32 + 13 * FONT_LITTLE_347_W, 669, (EMERALD_AUTHOR_LEN + 1) * FONT_LITTLE_347_W + 5,FONT_LITTLE_347_H + 6, 30,30,255,255,K_ABSOLUTE); // Level-Title
    }
    if (Ed.uMenuState == MENUSTATE_TIME_AND_SCORES_MESSAGE) {
        nErrorCode = CreateMessageWindow(pRenderer,-1,-1,0,Ed.MessageEditor.szMessageEditorMem);
        if (Ed.MessageEditor.bInsert) {
            PrintLittleFont(pRenderer,10,10,0,"MODE: INSERT",K_ABSOLUTE);
        } else {
            PrintLittleFont(pRenderer,10,10,0,"MODE: OVERWRITE",K_ABSOLUTE);
        }
        Ed.MessageEditor.uFrameCounter++;
        if (Ed.MessageEditor.uFrameCounter - Ed.MessageEditor.uLastToggleCursorFrame >= Ed.MessageEditor.uCursorFlashSpeedFrames) {
            Ed.MessageEditor.uLastToggleCursorFrame = Ed.MessageEditor.uFrameCounter;
            Ed.MessageEditor.bCursor = !Ed.MessageEditor.bCursor;
        }
        // Cursor blinken nur, wenn Cursor nicht auf Zeilenumbruch steht
        if ((Ed.MessageEditor.bCursor) && (Ed.MessageEditor.cCharAtCursor != 0x0A)) {
            Ed.MessageEditor.szMessageEditorMem[Ed.MessageEditor.uCursorPos] = 102; // Cursor, letztes Zeichen im Zeichensatz
        } else {
            Ed.MessageEditor.szMessageEditorMem[Ed.MessageEditor.uCursorPos] = Ed.MessageEditor.cCharAtCursor;
        }
        if (InputStates.pKeyboardArray[SDL_SCANCODE_RIGHT])  {
            if (Ed.MessageEditor.uCursorPos < (Ed.MessageEditor.uMessageLen - 1)) {
                // unabhängig vom Zustand des Cursor das originale Zeichen in den Puffer zurückschreiben
                Ed.MessageEditor.szMessageEditorMem[Ed.MessageEditor.uCursorPos] = Ed.MessageEditor.cCharAtCursor;
                Ed.MessageEditor.uCursorPos++;
                // Neues Zeichen unter Cursor sichern
                Ed.MessageEditor.cCharAtCursor = Ed.MessageEditor.szMessageEditorMem[Ed.MessageEditor.uCursorPos];
                WaitNoKey();
            }
        } else if (InputStates.pKeyboardArray[SDL_SCANCODE_LEFT]) {
            if (Ed.MessageEditor.uCursorPos > 0) {
                // unabhängig vom Zustand des Cursor das originale Zeichen in den Puffer zurückschreiben
                Ed.MessageEditor.szMessageEditorMem[Ed.MessageEditor.uCursorPos] = Ed.MessageEditor.cCharAtCursor;
                Ed.MessageEditor.uCursorPos--;
                // Neues Zeichen unter Cursor sichern
                Ed.MessageEditor.cCharAtCursor = Ed.MessageEditor.szMessageEditorMem[Ed.MessageEditor.uCursorPos];
                WaitNoKey();
            }
        } else if (InputStates.pKeyboardArray[SDL_SCANCODE_UP]) {
            // Setzt den Cursor auf den Start der aktuellen Zeile
            if (Ed.MessageEditor.uCursorPos > 0) {
                // unabhängig vom Zustand des Cursor das originale Zeichen in den Puffer zurückschreiben
                Ed.MessageEditor.szMessageEditorMem[Ed.MessageEditor.uCursorPos] = Ed.MessageEditor.cCharAtCursor;
                for (nE = Ed.MessageEditor.uCursorPos; nE >= 0; nE--) {
                    if (Ed.MessageEditor.szMessageEditorMem[nE] == 0x0A) {
                        break;
                    }
                }
                if (nE > 0) {
                    nE--;
                } else if (nE < 0) {
                    nE = 0;
                }
                Ed.MessageEditor.uCursorPos = nE;
                // Neues Zeichen unter Cursor sichern
                Ed.MessageEditor.cCharAtCursor = Ed.MessageEditor.szMessageEditorMem[Ed.MessageEditor.uCursorPos];
                WaitNoKey();
            }
        } else if (InputStates.pKeyboardArray[SDL_SCANCODE_DOWN]) {
            // Setzt den Cursor auf den Start der nächsten Zeile
            if (Ed.MessageEditor.uCursorPos < Ed.MessageEditor.uMessageLen) {
                // unabhängig vom Zustand des Cursor das originale Zeichen in den Puffer zurückschreiben
                Ed.MessageEditor.szMessageEditorMem[Ed.MessageEditor.uCursorPos] = Ed.MessageEditor.cCharAtCursor;
                for (nE = Ed.MessageEditor.uCursorPos; nE < Ed.MessageEditor.uMessageLen; nE++) {
                    if (Ed.MessageEditor.szMessageEditorMem[nE] == 0x0A) {
                        break;
                    }
                }
                if (nE > (Ed.MessageEditor.uMessageLen - 1)) {
                    nE = Ed.MessageEditor.uMessageLen - 1;
                }
                if (nE < (Ed.MessageEditor.uMessageLen - 2)) {
                    nE++;
                }
                Ed.MessageEditor.uCursorPos = nE;
                // Neues Zeichen unter Cursor sichern
                Ed.MessageEditor.cCharAtCursor = Ed.MessageEditor.szMessageEditorMem[Ed.MessageEditor.uCursorPos];
                WaitNoKey();
            }
        } else if (InputStates.pKeyboardArray[SDL_SCANCODE_INSERT]) {
            Ed.MessageEditor.bInsert = !Ed.MessageEditor.bInsert;
            WaitNoSpecialKey(SDL_SCANCODE_INSERT);
        } else if (InputStates.pKeyboardArray[SDL_SCANCODE_BACKSPACE]) {    // Zeichen links vom Cursor löschen
            if (Ed.MessageEditor.uCursorPos > 0) {
                // unabhängig vom Zustand des Cursor das originale Zeichen in den Puffer zurückschreiben
                Ed.MessageEditor.szMessageEditorMem[Ed.MessageEditor.uCursorPos] = Ed.MessageEditor.cCharAtCursor;
                for (nE = Ed.MessageEditor.uCursorPos - 1; nE <= Ed.MessageEditor.uMessageLen; nE++) {
                    Ed.MessageEditor.szMessageEditorMem[nE] = Ed.MessageEditor.szMessageEditorMem[nE + 1];
                }
                Ed.MessageEditor.uMessageLen = strlen(Ed.MessageEditor.szMessageEditorMem);
                Ed.MessageEditor.uCursorPos--;
                // Neues Zeichen unter Cursor sichern
                Ed.MessageEditor.cCharAtCursor = Ed.MessageEditor.szMessageEditorMem[Ed.MessageEditor.uCursorPos];
                //WaitNoKey();
                SDL_Delay(50);
            }
        } else if (InputStates.pKeyboardArray[SDL_SCANCODE_DELETE]) {    // Zeichen rechts vom Cursor löschen
            if (Ed.MessageEditor.uCursorPos < (Ed.MessageEditor.uMessageLen - 2)) {
                // unabhängig vom Zustand des Cursor das originale Zeichen in den Puffer zurückschreiben
                Ed.MessageEditor.szMessageEditorMem[Ed.MessageEditor.uCursorPos] = Ed.MessageEditor.cCharAtCursor;
                for (nE = Ed.MessageEditor.uCursorPos; nE < Ed.MessageEditor.uMessageLen; nE++) {
                    Ed.MessageEditor.szMessageEditorMem[nE] = Ed.MessageEditor.szMessageEditorMem[nE + 1];
                }
                Ed.MessageEditor.uMessageLen = strlen(Ed.MessageEditor.szMessageEditorMem);
                //Ed.MessageEditor.uCursorPos--;
                // Neues Zeichen unter Cursor sichern
                Ed.MessageEditor.cCharAtCursor = Ed.MessageEditor.szMessageEditorMem[Ed.MessageEditor.uCursorPos];
                WaitNoKey();//SDL_Delay(50);
            }

        } else if (InputStates.pKeyboardArray[SDL_SCANCODE_HOME]) {
            // unabhängig vom Zustand des Cursor das originale Zeichen in den Puffer zurückschreiben
            Ed.MessageEditor.szMessageEditorMem[Ed.MessageEditor.uCursorPos] = Ed.MessageEditor.cCharAtCursor;
            Ed.MessageEditor.uCursorPos = 0;
            // Neues Zeichen unter Cursor sichern
            Ed.MessageEditor.cCharAtCursor = Ed.MessageEditor.szMessageEditorMem[Ed.MessageEditor.uCursorPos];
        } else if (InputStates.pKeyboardArray[SDL_SCANCODE_END]) {
            // unabhängig vom Zustand des Cursor das originale Zeichen in den Puffer zurückschreiben
            Ed.MessageEditor.szMessageEditorMem[Ed.MessageEditor.uCursorPos] = Ed.MessageEditor.cCharAtCursor;
            Ed.MessageEditor.uCursorPos = Ed.MessageEditor.uMessageLen - 1;
            // Neues Zeichen unter Cursor sichern
            Ed.MessageEditor.cCharAtCursor = Ed.MessageEditor.szMessageEditorMem[Ed.MessageEditor.uCursorPos];
        }
        uKey = GetKey();
        //                    Hier wird auf maximal 30 Linefeeds begrenzt, da sonst die Nachricht zu hoch wird.
        if ( (uKey != 0) &&  (!((uKey == 0x0A) && (GetLineFeedCount(Ed.MessageEditor.szMessageEditorMem) >= EMERALD_MAX_LINEFEEDS_IN_MESSAGE))) ) {
            if (Ed.MessageEditor.bInsert) {
                if ((GetLineLen(Ed.MessageEditor.szMessageEditorMem,Ed.MessageEditor.uCursorPos) < EMERALD_MAX_CHARACTERS_PER_LINE) || (uKey == 0x0A)) {
                    if ( (Ed.MessageEditor.uCursorPos < (Ed.MessageEditor.uMessageLen)) && ((Ed.MessageEditor.uMessageLen) < EDITOR_MEM_SIZE - 2) && (!((Ed.MessageEditor.uCursorPos == 0) && (uKey == 0x0A))) ) {
                        // unabhängig vom Zustand des Cursor das originale Zeichen in den Puffer zurückschreiben
                        Ed.MessageEditor.szMessageEditorMem[Ed.MessageEditor.uCursorPos] = Ed.MessageEditor.cCharAtCursor;
                        // nE ist beim Start der Schleife Zeiger auf letztes Zeichen im Text
                        for (nE = Ed.MessageEditor.uMessageLen - 1; (nE >= Ed.MessageEditor.uCursorPos) && (nE >= 0); nE--) {
                            Ed.MessageEditor.szMessageEditorMem[nE + 1] = Ed.MessageEditor.szMessageEditorMem[nE];
                        }
                        Ed.MessageEditor.szMessageEditorMem[Ed.MessageEditor.uCursorPos] = uKey;
                        Ed.MessageEditor.uMessageLen = strlen(Ed.MessageEditor.szMessageEditorMem);
                        Ed.MessageEditor.uCursorPos++;
                        // Neues Zeichen unter Cursor sichern
                        Ed.MessageEditor.cCharAtCursor = Ed.MessageEditor.szMessageEditorMem[Ed.MessageEditor.uCursorPos];
                    }
                }
            } else {
                if (!((Ed.MessageEditor.uCursorPos == 0) && (uKey == 0x0A))) {
                    // Überschreib-Modus
                    Ed.MessageEditor.szMessageEditorMem[Ed.MessageEditor.uCursorPos] = uKey;
                    if (Ed.MessageEditor.uCursorPos < (Ed.MessageEditor.uMessageLen - 1)) {
                        Ed.MessageEditor.uCursorPos++;
                        // Neues Zeichen unter Cursor sichern
                        Ed.MessageEditor.cCharAtCursor = Ed.MessageEditor.szMessageEditorMem[Ed.MessageEditor.uCursorPos];
                    }
                }
            }
        }
        while (GetKey() != 0);
        // SDL_Log("LineLen: %d  textlen: %d   LastKey: %u",GetLineLen(Ed.MessageEditor.szMessageEditorMem,Ed.MessageEditor.uCursorPos),Ed.MessageEditor.uMessageLen,g_uLastKey);
        // SDL_Log("CurPos: %03d     strlen: %03d     real_strlen: %03d",Ed.MessageEditor.uCursorPos,Ed.MessageEditor.uMessageLen,strlen(Ed.MessageEditor.szMessageEditorMem));
    } else {
        nErrorCode = PrintLittleFont(pRenderer,890,30,0,"MESSAGES",K_ABSOLUTE);
        sprintf(szText,"X: %d   Y: %d",InputStates.nMouseXpos_Absolute,InputStates.nMouseYpos_Absolute);
        PrintLittleFont(pRenderer,10,10,0,szText,K_ABSOLUTE); // Maus-Koordinate anzeigen

        // Level-Dimension prüfen und ggf. anpassen
        if (Ed.uTmpLevel_X_Dimension > MAX_LEVEL_W) {
            Ed.uTmpLevel_X_Dimension = MAX_LEVEL_W;
        } else if (Ed.uTmpLevel_X_Dimension < MIN_LEVEL_W) {
            Ed.uTmpLevel_X_Dimension = MIN_LEVEL_W;
        }
        if (Ed.uTmpLevel_Y_Dimension > MAX_LEVEL_H) {
            Ed.uTmpLevel_Y_Dimension = MAX_LEVEL_H;
        } else if (Ed.uTmpLevel_Y_Dimension < MIN_LEVEL_H) {
            Ed.uTmpLevel_Y_Dimension = MIN_LEVEL_H;
        }
        sprintf(szText,"%04d",Ed.uTmpLevel_X_Dimension);
        for (I = 0; I < 4; I++) {
            uPositionsAndElements[(105 + I) * 3 + 2] = EMERALD_FONT_0 + szText[I] - 0x30;
        }
        sprintf(szText,"%04d",Ed.uTmpLevel_Y_Dimension);
        for (I = 0; I < 4; I++) {
            uPositionsAndElements[(127 + I) * 3 + 2] = EMERALD_FONT_0 + szText[I] - 0x30;
        }
        nErrorCode = PrintLittleFont(pRenderer,380,24,0,"SCORES",K_ABSOLUTE);
        // 1. Zeile
        // Emerald
        PrintLittleFont(pRenderer,100,60,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,100,86,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uScoreEmerald);
        PrintLittleFont(pRenderer,100,73,0,szText,K_ABSOLUTE);
        // Rubin
        PrintLittleFont(pRenderer,260,60,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,260,86,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uScoreRuby);
        PrintLittleFont(pRenderer,260,73,0,szText,K_ABSOLUTE);
        // Saphir
        PrintLittleFont(pRenderer,420,60,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,420,86,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uScoreSaphir);
        PrintLittleFont(pRenderer,420,73,0,szText,K_ABSOLUTE);
        // Perle
        PrintLittleFont(pRenderer,580,60,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,580,86,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uScorePerl);
        PrintLittleFont(pRenderer,580,73,0,szText,K_ABSOLUTE);
        // Kristal
        PrintLittleFont(pRenderer,740,60,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,740,86,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uScoreCrystal);
        PrintLittleFont(pRenderer,740,73,0,szText,K_ABSOLUTE);
        // 2. Zeile
        // Nachricht
        PrintLittleFont(pRenderer,100,124,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,100,150,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uScoreMessage);
        PrintLittleFont(pRenderer,100,137,0,szText,K_ABSOLUTE);
        // Schlüssel
        PrintLittleFont(pRenderer,260,124,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,260,150,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uScoreKey);
        PrintLittleFont(pRenderer,260,137,0,szText,K_ABSOLUTE);
        // Dynamit
        PrintLittleFont(pRenderer,420,124,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,420,150,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uScoreDynamite);
        PrintLittleFont(pRenderer,420,137,0,szText,K_ABSOLUTE);
        // Hammer
        PrintLittleFont(pRenderer,580,124,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,580,150,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uScoreHammer);
        PrintLittleFont(pRenderer,580,137,0,szText,K_ABSOLUTE);
        // Time-Coin
        PrintLittleFont(pRenderer,740,124,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,740,150,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uScoreTimeCoin);
        PrintLittleFont(pRenderer,740,137,0,szText,K_ABSOLUTE);
        // 3. Zeile
        // Nuss knacken
        PrintLittleFont(pRenderer,100,188,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,100,214,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uScoreNutCracking);
        PrintLittleFont(pRenderer,100,201,0,szText,K_ABSOLUTE);
        // Stoning Beetle
        PrintLittleFont(pRenderer,260,188,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,260,214,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uScoreStoningBeetle);
        PrintLittleFont(pRenderer,260,201,0,szText,K_ABSOLUTE);
        // Stoning Mine
        PrintLittleFont(pRenderer,420,188,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,420,214,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uScoreStoningMine);
        PrintLittleFont(pRenderer,420,201,0,szText,K_ABSOLUTE);
        // Stoning Alien
        PrintLittleFont(pRenderer,580,188,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,580,214,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uScoreStoningAlien);
        PrintLittleFont(pRenderer,580,201,0,szText,K_ABSOLUTE);
        // Stoning Yam
        PrintLittleFont(pRenderer,740,188,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,740,214,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uScoreStoningYam);
        PrintLittleFont(pRenderer,740,201,0,szText,K_ABSOLUTE);
        PrintLittleFont(pRenderer,300,250,0,"TIMES (SECS) & INVENTORY",K_ABSOLUTE);
        nErrorCode = PrintLittleFont(pRenderer,200,363,0,"INVENTORY -->",K_ABSOLUTE);
        // 4. Zeile
        // Time to Play
        PrintLittleFont(pRenderer,100,286,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,100,312,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uTimeToPlay);
        PrintLittleFont(pRenderer,100,299,0,szText,K_ABSOLUTE);
        // Wheel rotation time
        PrintLittleFont(pRenderer,260,286,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,260,312,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uTimeWheelRotation);
        PrintLittleFont(pRenderer,260,299,0,szText,K_ABSOLUTE);
        // Magic wall time
        PrintLittleFont(pRenderer,420,286,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,420,312,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uTimeMagicWall);
        PrintLittleFont(pRenderer,420,299,0,szText,K_ABSOLUTE);
        // Light time
        PrintLittleFont(pRenderer,580,286,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,580,312,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uTimeLight);
        PrintLittleFont(pRenderer,580,299,0,szText,K_ABSOLUTE);
        // Time door time
        PrintLittleFont(pRenderer,740,286,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,740,312,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uTimeDoorTime);
        PrintLittleFont(pRenderer,740,299,0,szText,K_ABSOLUTE);
        // 5. Zeile
        // Additional Time for time coin
        PrintLittleFont(pRenderer,100,350,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,100,376,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uAdditonalTimeCoinTime);
        PrintLittleFont(pRenderer,100,363,0,szText,K_ABSOLUTE);
        // Dynamite Count
        PrintLittleFont(pRenderer,420,350,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,420,376,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uDynamiteCount);
        PrintLittleFont(pRenderer,420,363,0,szText,K_ABSOLUTE);
        // Hammer Count
        PrintLittleFont(pRenderer,580,350,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,580,376,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uHammerCount);
        PrintLittleFont(pRenderer,580,363,0,szText,K_ABSOLUTE);
        // Dynamite Count
        PrintLittleFont(pRenderer,740,350,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,740,376,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uWhiteKeyCount);
        PrintLittleFont(pRenderer,740,363,0,szText,K_ABSOLUTE);
        // Remaining Time factor
        nErrorCode = PrintLittleFont(pRenderer,32,424,0,"REMAINING TIME FACTOR:",K_ABSOLUTE);
        PrintLittleFont(pRenderer,252,411,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,252,437,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uTimeScoreFactor);
        PrintLittleFont(pRenderer,252,424,0,szText,K_ABSOLUTE);
        // Cheese spread speed
        nErrorCode = PrintLittleFont(pRenderer,386,424,0,"SPEED:",K_ABSOLUTE);
        PrintLittleFont(pRenderer,446,411,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,446,437,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uCheeseSpreadSpeed);
        PrintLittleFont(pRenderer,446,424,0,szText,K_ABSOLUTE);
        // Grass spread speed
        nErrorCode = PrintLittleFont(pRenderer,546,424,0,"SPEED:",K_ABSOLUTE);
        PrintLittleFont(pRenderer,606,411,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,606,437,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uGrassSpreadSpeed);
        PrintLittleFont(pRenderer,606,424,0,szText,K_ABSOLUTE);
        // Emeralds to collect
        nErrorCode = PrintLittleFont(pRenderer,240,488,0,"TO COLLECT:",K_ABSOLUTE);
        PrintLittleFont(pRenderer,350,475,0,"++++",K_ABSOLUTE);
        PrintLittleFont(pRenderer,350,501,0,"----",K_ABSOLUTE);
        sprintf(szText,"%04d",Ed.uEmeraldsToCollect);
        PrintLittleFont(pRenderer,350,488,0,szText,K_ABSOLUTE);
        // Level-Title
        PrintLittleFont(pRenderer,32,576,0,"LEVEL-TITLE :",K_ABSOLUTE);
        nErrorCode = PrintLittleFont(pRenderer,164,576,0,Ed.szLevelTitle,K_ABSOLUTE);
        // Level-Author
        PrintLittleFont(pRenderer,32,672,0,"LEVEL-AUTHOR:",K_ABSOLUTE);
        nErrorCode = PrintLittleFont(pRenderer,164,672,0,Ed.szLevelAuthor,K_ABSOLUTE);
        for (I = 0; (I < sizeof(uPositionsAndElements) / (sizeof(uint32_t) * 3)) && (nErrorCode == 0); I++) {
            uTextureIndex = GetTextureIndexByElement(uPositionsAndElements[I * 3 + 2],Ed.uFrameCounter % 16,&fAngle);
            DestR.x = uPositionsAndElements[I * 3 + 0];
            DestR.y = uPositionsAndElements[I * 3 + 1];
            DestR.w = FONT_W;
            DestR.h = FONT_H;
            if (nErrorCode == 0) {
                if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&DestR,fAngle,NULL, SDL_FLIP_NONE) != 0) {
                    nErrorCode = -1;
                    SDL_Log("%s: SDL_RenderCopyEx() failed: %s",__FUNCTION__,SDL_GetError());
                    SDL_Log("%s: SDL_RenderCopyEx() failed: %s",__FUNCTION__,SDL_GetError());
                }
            }
        }
        nSwitchField = -1;
        if (InputStates.bLeftMouseButton) {
            for (I = 0; I < (sizeof(uSwitches) / (sizeof(uint32_t) * 4)); I++) {
                uMinX = uSwitches[I * 4 + 0];
                uMaxX = uSwitches[I * 4 + 1];
                uMinY = uSwitches[I * 4 + 2];
                uMaxY = uSwitches[I * 4 + 3];
                if ((InputStates.nMouseXpos_Absolute >= uMinX) && (InputStates.nMouseXpos_Absolute <= uMaxX) && (InputStates.nMouseYpos_Absolute >= uMinY) && (InputStates.nMouseYpos_Absolute <= uMaxY)) {
                    nSwitchField = GetTimeScoresSwitchfieldAndOffset(I,&uSwitchFieldOffset);
                    // SDL_Log("Switch: %u   nSwitchField: %d   uSwitchFieldOffset: %u  ",I,nSwitchField,uSwitchFieldOffset);
                    if (nSwitchField != -1) {
                        if (((nSwitchField >= 0) && (nSwitchField <= 28)) || (nSwitchField == 30)) {    // Feld 30 = Gras-Speed
                            ChangeTimeScoresValue(nSwitchField,uSwitchFieldOffset);
                        } else if (nSwitchField == 29) {    // Messages 1 - 8
                            Ed.MessageEditor.nEditMessage = uSwitchFieldOffset;
                            SDL_Log("edit Message %d",Ed.MessageEditor.nEditMessage);
                            Ed.uMenuState = MENUSTATE_TIME_AND_SCORES_MESSAGE;
                            SetButtonActivity(BUTTONLABEL_RETURN_TO_LEVEL,false);
                            SetButtonActivity(BUTTONLABEL_SAVE_MESSAGE,true);
                            SetButtonActivity(BUTTONLABEL_CANCEL_MESSAGE,true);
                            memset(Ed.MessageEditor.szMessageEditorMem,0,EDITOR_MEM_SIZE);
                            GetKey();   // Letztes Zeichen im Tastaturpuffer löschen
                            if ((Ed.MessageEditor.nEditMessage >= 0) && (Ed.MessageEditor.nEditMessage < EMERALD_MAX_MESSAGES)) {
                                if (Ed.pMessage[Ed.MessageEditor.nEditMessage] != NULL) {
                                    if (strlen(Ed.pMessage[Ed.MessageEditor.nEditMessage]) < (EDITOR_MEM_SIZE - 2) ) {
                                        strcpy(Ed.MessageEditor.szMessageEditorMem,Ed.pMessage[Ed.MessageEditor.nEditMessage]);
                                        strcat(Ed.MessageEditor.szMessageEditorMem," ");
                                    }
                                }
                                if (strlen(Ed.MessageEditor.szMessageEditorMem) == 0) {
                                    strcpy(Ed.MessageEditor.szMessageEditorMem," ");
                                }
                                // Es ist mindestens 1 Zeichen (" ") im Editor-Speicher
                                Ed.MessageEditor.uMessageLen = (uint32_t)strlen(Ed.MessageEditor.szMessageEditorMem);
                                Ed.MessageEditor.uCursorPos = Ed.MessageEditor.uMessageLen - 1;
                                Ed.MessageEditor.cCharAtCursor = Ed.MessageEditor.szMessageEditorMem[Ed.MessageEditor.uCursorPos];
                            } else {
                                nErrorCode = -1;
                                SDL_Log("%s: invalid Message: %d",__FUNCTION__,Ed.MessageEditor.nEditMessage);
                            }
                        }
                        // SDL_Log("found switchfield no.: %u / %u",nSwitchField,uSwitchFieldOffset);
                    }
                    WaitNoKey();
                    break;
                }
            }
            if (Ed.uMenuState != MENUSTATE_TIME_AND_SCORES_MESSAGE) {
                Ed.uMenuState = MENUSTATE_TIME_AND_SCORES;
            }
            // Mausklick in Level-Title oder Level-Author
            if ((InputStates.nMouseXpos_Absolute >= 166) && (InputStates.nMouseXpos_Absolute < 483)) {
                if ((InputStates.nMouseYpos_Absolute >= 576) && (InputStates.nMouseYpos_Absolute < 589)) {
                    Ed.uMenuState = MENUSTATE_TIME_AND_SCORES_EDIT_TITLE;
                } else if ((InputStates.nMouseYpos_Absolute >= 672) && (InputStates.nMouseYpos_Absolute < 685)) {
                    Ed.uMenuState = MENUSTATE_TIME_AND_SCORES_EDIT_AUTHOR;
                }
            }
        }

        if (Ed.uMenuState == MENUSTATE_TIME_AND_SCORES_EDIT_TITLE) {
            uCursorPos = strlen(Ed.szLevelTitle);
            if (((Playfield.uFrameCounter >> 4) & 0x01) == 0) {
                szCursor[0] = ' ';  // Cursor aus
            } else {
                szCursor[0] = 102;  // Cursor an
            }
            szCursor[1] = 0;
            PrintLittleFont(pRenderer,32 + 10 * (13 + uCursorPos),576,0,szCursor,K_ABSOLUTE);
            uKey = FilterBigFontKey(GetKey());
            if (uKey != 0) {
               if (uCursorPos < EMERALD_TITLE_LEN) {
                    Ed.szLevelTitle[uCursorPos] = uKey;
               }
            } else if (InputStates.pKeyboardArray[SDL_SCANCODE_BACKSPACE]) {
                if (uCursorPos > 0) {
                    Ed.szLevelTitle[uCursorPos - 1] = 0;
                }
                do {
                    UpdateInputStates();
                } while (InputStates.pKeyboardArray[SDL_SCANCODE_BACKSPACE]);
            } else if ((InputStates.pKeyboardArray[SDL_SCANCODE_RETURN]) || (InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE])) {
                Ed.uMenuState = MENUSTATE_TIME_AND_SCORES; // Eingabe bei Enter und Escape verlassen
            }
        } else if (Ed.uMenuState == MENUSTATE_TIME_AND_SCORES_EDIT_AUTHOR) {
            uCursorPos = strlen(Ed.szLevelAuthor);
            if (((Playfield.uFrameCounter >> 4) & 0x01) == 0) {
                szCursor[0] = ' ';  // Cursor aus
            } else {
                szCursor[0] = 102;  // Cursor an
            }
            szCursor[1] = 0;
            PrintLittleFont(pRenderer,32 + 10 * (13 + uCursorPos),672,0,szCursor,K_ABSOLUTE);
            uKey = FilterBigFontKey(GetKey());
            if (uKey != 0) {
                if (uCursorPos < EMERALD_AUTHOR_LEN) {
                    Ed.szLevelAuthor[uCursorPos] = uKey;
                }
            } else if (InputStates.pKeyboardArray[SDL_SCANCODE_BACKSPACE]) {
                if (uCursorPos > 0) {
                    Ed.szLevelAuthor[uCursorPos - 1] = 0;
                }
                do {
                    UpdateInputStates();
                } while (InputStates.pKeyboardArray[SDL_SCANCODE_BACKSPACE]);
            } else if ((InputStates.pKeyboardArray[SDL_SCANCODE_RETURN]) || (InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE])) {
                Ed.uMenuState = MENUSTATE_TIME_AND_SCORES; // Eingabe bei Enter und Escape verlassen
            }
        }
        // Level-Dimension
        nErrorCode = PrintLittleFont(pRenderer,580,488,0,"LEVEL DIMENSION",K_ABSOLUTE);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           ChangeTimeScoresValue
------------------------------------------------------------------------------
Beschreibung: Ändert einen Wert im Menü Times & Scores anhand eines Schaltersfeldes
              und den Schalteroffset.
Parameter
      Eingang: nSwitchField, int, Schalterfeld im Menü Times & Scores, 0 bis 30
               uSwitchFieldOffset, uint32_t, Schalteroffset
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Ed.x
------------------------------------------------------------------------------*/
void ChangeTimeScoresValue(int nSwitchField,uint32_t uSwitchFieldOffset) {
    uint32_t uValue;

    switch (nSwitchField) {
        case(0):
            uValue = Ed.uScoreEmerald;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uScoreEmerald = uValue;
            break;
        case(1):
            uValue = Ed.uScoreRuby;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uScoreRuby = uValue;
            break;
        case(2):
            uValue = Ed.uScoreSaphir;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uScoreSaphir = uValue;
            break;
        case(3):
            uValue = Ed.uScorePerl;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uScorePerl = uValue;
            break;
        case(4):
            uValue = Ed.uScoreCrystal;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uScoreCrystal = uValue;
            break;
        case(5):
            uValue = Ed.uScoreMessage;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uScoreMessage = uValue;
            break;
        case(6):
            uValue = Ed.uScoreKey;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uScoreKey = uValue;
            break;
        case(7):
            uValue = Ed.uScoreDynamite;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uScoreDynamite = uValue;
            break;
        case(8):
            uValue = Ed.uScoreHammer;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uScoreHammer = uValue;
            break;
        case(9):
            uValue = Ed.uScoreTimeCoin;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uScoreTimeCoin = uValue;
            break;
        case(10):
            uValue = Ed.uScoreNutCracking;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uScoreNutCracking = uValue;
            break;
        case(11):
            uValue = Ed.uScoreStoningBeetle;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uScoreStoningBeetle = uValue;
            break;
        case(12):
            uValue = Ed.uScoreStoningMine;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uScoreStoningMine = uValue;
            break;
        case(13):
            uValue = Ed.uScoreStoningAlien;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uScoreStoningAlien = uValue;
            break;
        case(14):
            uValue = Ed.uScoreStoningYam;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uScoreStoningYam = uValue;
            break;
        case(15):
            uValue = Ed.uTimeToPlay;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uTimeToPlay = uValue;
            break;
        case(16):
            uValue = Ed.uTimeWheelRotation;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uTimeWheelRotation = uValue;
            break;
        case(17):
            uValue = Ed.uTimeMagicWall;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uTimeMagicWall = uValue;
            break;
        case(18):
            uValue = Ed.uTimeLight;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uTimeLight = uValue;
            break;
        case(19):
            uValue = Ed.uTimeDoorTime;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uTimeDoorTime = uValue;
            break;
        case(20):
            uValue = Ed.uAdditonalTimeCoinTime;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uAdditonalTimeCoinTime = uValue;
            break;
        case(21):
            uValue = Ed.uDynamiteCount;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uDynamiteCount = uValue;
            break;
        case(22):
            uValue = Ed.uHammerCount;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uHammerCount = uValue;
            break;
        case(23):
            uValue = Ed.uWhiteKeyCount;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uWhiteKeyCount = uValue;
            break;
        case(24):
            uValue = Ed.uTimeScoreFactor;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uTimeScoreFactor = uValue;
            break;
        case(25):
            uValue = Ed.uCheeseSpreadSpeed;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uCheeseSpreadSpeed = uValue;
            break;
        case(26):
            uValue = Ed.uEmeraldsToCollect;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uEmeraldsToCollect = uValue;
            break;
        case(27):
            uValue = Ed.uTmpLevel_X_Dimension;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uTmpLevel_X_Dimension = uValue;
            break;
        case(28):
            uValue = Ed.uTmpLevel_Y_Dimension;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uTmpLevel_Y_Dimension = uValue;
            break;
        case(30):
            uValue = Ed.uGrassSpreadSpeed;
            uValue = IncreaseOrDecreaseTimeScoreValue(uValue,uSwitchFieldOffset);
            Ed.uGrassSpreadSpeed = uValue;
            break;
    }
}


/*----------------------------------------------------------------------------
Name:           IncreaseOrDecreaseTimeScoreValue
------------------------------------------------------------------------------
Beschreibung: Erhöht oder erniedrigt einen Wert für das Menü Times & Scores.

Parameter
      Eingang: uSValue, uint32_t, Wert, der verändert werden soll.
               uSwitchOffset, uint32_t, Schalteroffset, 0 bis 7
      Ausgang: -
Rückgabewert:  uint32_t , veränderter Wert
Seiteneffekte: -
------------------------------------------------------------------------------*/
uint32_t IncreaseOrDecreaseTimeScoreValue(uint32_t uValue, uint32_t uSwitchOffset) {
    char szValue[8];

    if (uValue > 9999) {
        uValue = 9999;
    }
    sprintf(szValue,"%04d",uValue);
    switch (uSwitchOffset) {
        case(0):        // +1000
            if (szValue[0] == 0x39) {   // Überlauf?
                szValue[0] = 0x30;
            } else {
                szValue[0] = szValue[0] + 1;
            }
            break;
        case(1):        // +100
            if (szValue[1] == 0x39) {   // Überlauf?
                szValue[1] = 0x30;
            } else {
                szValue[1] = szValue[1] + 1;
            }
            break;
        case(2):        // +10
            if (szValue[2] == 0x39) {   // Überlauf?
                szValue[2] = 0x30;
            } else {
                szValue[2] = szValue[2] + 1;
            }
            break;
        case(3):        // +1
            if (szValue[3] == 0x39) {   // Überlauf?
                szValue[3] = 0x30;
            } else {
                szValue[3] = szValue[3] + 1;
            }
            break;
        case(4):        // -1000
            if (szValue[0] == 0x30) {   // Unterlauf?
                szValue[0] = 0x39;
            } else {
                szValue[0] = szValue[0] - 1;
            }
            break;
        case(5):        // -100
            if (szValue[1] == 0x30) {   // Unterlauf?
                szValue[1] = 0x39;
            } else {
                szValue[1] = szValue[1] - 1;
            }
            break;
        case(6):        // -10
            if (szValue[2] == 0x30) {   // Unterlauf?
                szValue[2] = 0x39;
            } else {
                szValue[2] = szValue[2] - 1;
            }
            break;
        case(7):        // -1
            if (szValue[3] == 0x30) {   // Unterlauf?
                szValue[3] = 0x39;
            } else {
                szValue[3] = szValue[3] - 1;
            }
            break;
    }
    uValue = strtol(szValue,NULL,10);
    return uValue;
}


/*----------------------------------------------------------------------------
Name:           GetTimeScoresSwitchfieldAndOffset
------------------------------------------------------------------------------
Beschreibung: Ermittelt im Menü Times & Scores anhand eines Schalters das Schalterfeld
              und den Schalteroffset.
Parameter
      Eingang: uSwitch, uint32_t, Schalter im Menü Times & Scores, 0 bis 359
      Ausgang: puSwitchOffset, uint32_t *, Schalteroffset innerhalb des Schalterfelds
Rückgabewert:  int , > 0 = Schaltergruppe, -1 = nichts gefunden
Seiteneffekte: -
------------------------------------------------------------------------------*/
int GetTimeScoresSwitchfieldAndOffset(uint32_t uSwitch,uint32_t *puSwitchOffset) {
    int nSwitchfield;

    nSwitchfield = -1;
    if ((uSwitch <= 247) && (puSwitchOffset != NULL)) {
        if (uSwitch <= 231) {        // Die ersten 29 Felder mit 8 Schaltern
            nSwitchfield = uSwitch / 8; // Switchfield 0 bis 28
            *puSwitchOffset = uSwitch % 8;
        } else if ((uSwitch >= 232) && (uSwitch <= 239)) {  // Messages 1 - 8
            nSwitchfield = 29;
            *puSwitchOffset = (uSwitch - 232) % 8;
        } else if ((uSwitch >= 240) && (uSwitch <= 247)) {  // Spread gras
            nSwitchfield = 30;
            *puSwitchOffset = (uSwitch - 240) % 8;
        }
    }
    return nSwitchfield;
}


/*----------------------------------------------------------------------------
Name:           EditorStateConfirmNewLevelDimension
------------------------------------------------------------------------------
Beschreibung: Wenn sich der Leveleditor in Menustate = MENUSTATE_CONFIRM_NEWLEVEL_DIMENSION
              befindet, wird diese Funktion ausgeführt. Sie ermöglicht die Bestätigigung oder
              Ablehnung der neuen Level-Dimension.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Ed.x, MainMenu.x
------------------------------------------------------------------------------*/
int EditorStateConfirmNewLevelDimension(SDL_Renderer *pRenderer) {
    int nErrorCode;
    char szText[256];

    SetMenuBorderAndClear();
    // Bomben
    MainMenu.uMenuScreen[4 * MainMenu.uXdim + 12] = EMERALD_BOMB;
    MainMenu.uMenuScreen[4 * MainMenu.uXdim + 13] = EMERALD_BOMB;
    MainMenu.uMenuScreen[4 * MainMenu.uXdim + 14] = EMERALD_BOMB;
    SetMenuText(MainMenu.uMenuScreen,"WARNING",16,4,EMERALD_FONT_BLUE);
    MainMenu.uMenuScreen[4 * MainMenu.uXdim + 24] = EMERALD_BOMB;
    MainMenu.uMenuScreen[4 * MainMenu.uXdim + 25] = EMERALD_BOMB;
    MainMenu.uMenuScreen[4 * MainMenu.uXdim + 26] = EMERALD_BOMB;
    // Obere Wand
    MainMenu.uMenuScreen[6 * MainMenu.uXdim + 10] = EMERALD_STEEL_MARKER_LEFT_UP;
    MainMenu.uMenuScreen[6 * MainMenu.uXdim + 11] = EMERALD_STEEL_MARKER_UP;
    MainMenu.uMenuScreen[6 * MainMenu.uXdim + 12] = EMERALD_STEEL_MARKER_UP;
    MainMenu.uMenuScreen[6 * MainMenu.uXdim + 13] = EMERALD_STEEL_MARKER_UP;
    MainMenu.uMenuScreen[6 * MainMenu.uXdim + 14] = EMERALD_STEEL_MARKER_UP;
    MainMenu.uMenuScreen[6 * MainMenu.uXdim + 15] = EMERALD_STEEL_MARKER_UP;
    MainMenu.uMenuScreen[6 * MainMenu.uXdim + 16] = EMERALD_STEEL_MARKER_UP;
    MainMenu.uMenuScreen[6 * MainMenu.uXdim + 17] = EMERALD_STEEL_MARKER_UP;
    MainMenu.uMenuScreen[6 * MainMenu.uXdim + 18] = EMERALD_STEEL_MARKER_UP;
    MainMenu.uMenuScreen[6 * MainMenu.uXdim + 19] = EMERALD_STEEL_MARKER_UP;
    MainMenu.uMenuScreen[6 * MainMenu.uXdim + 20] = EMERALD_STEEL_MARKER_UP;
    MainMenu.uMenuScreen[6 * MainMenu.uXdim + 21] = EMERALD_STEEL_MARKER_UP;
    MainMenu.uMenuScreen[6 * MainMenu.uXdim + 22] = EMERALD_STEEL_MARKER_UP;
    MainMenu.uMenuScreen[6 * MainMenu.uXdim + 23] = EMERALD_STEEL_MARKER_UP;
    MainMenu.uMenuScreen[6 * MainMenu.uXdim + 24] = EMERALD_STEEL_MARKER_UP;
    MainMenu.uMenuScreen[6 * MainMenu.uXdim + 25] = EMERALD_STEEL_MARKER_UP;
    MainMenu.uMenuScreen[6 * MainMenu.uXdim + 26] = EMERALD_STEEL_MARKER_UP;
    MainMenu.uMenuScreen[6 * MainMenu.uXdim + 27] = EMERALD_STEEL_MARKER_UP;
    MainMenu.uMenuScreen[6 * MainMenu.uXdim + 28] = EMERALD_STEEL_MARKER_RIGHT_UP;
    //Seitenwände
    MainMenu.uMenuScreen[7 * MainMenu.uXdim + 10] = EMERALD_STEEL_MARKER_LEFT;
    MainMenu.uMenuScreen[8 * MainMenu.uXdim + 10] = EMERALD_STEEL_MARKER_LEFT;
    MainMenu.uMenuScreen[9 * MainMenu.uXdim + 10] = EMERALD_STEEL_MARKER_LEFT;
    MainMenu.uMenuScreen[10 * MainMenu.uXdim + 10] = EMERALD_STEEL_MARKER_LEFT;
    MainMenu.uMenuScreen[11 * MainMenu.uXdim + 10] = EMERALD_STEEL_MARKER_LEFT;
    MainMenu.uMenuScreen[12 * MainMenu.uXdim + 10] = EMERALD_STEEL_MARKER_LEFT;
    MainMenu.uMenuScreen[13 * MainMenu.uXdim + 10] = EMERALD_STEEL_MARKER_LEFT;
    MainMenu.uMenuScreen[7 * MainMenu.uXdim + 28] = EMERALD_STEEL_MARKER_RIGHT;
    MainMenu.uMenuScreen[8 * MainMenu.uXdim + 28] = EMERALD_STEEL_MARKER_RIGHT;
    MainMenu.uMenuScreen[9 * MainMenu.uXdim + 28] = EMERALD_STEEL_MARKER_RIGHT;
    MainMenu.uMenuScreen[10 * MainMenu.uXdim + 28] = EMERALD_STEEL_MARKER_RIGHT;
    MainMenu.uMenuScreen[11 * MainMenu.uXdim + 28] = EMERALD_STEEL_MARKER_RIGHT;
    MainMenu.uMenuScreen[12 * MainMenu.uXdim + 28] = EMERALD_STEEL_MARKER_RIGHT;
    MainMenu.uMenuScreen[13 * MainMenu.uXdim + 28] = EMERALD_STEEL_MARKER_RIGHT;
    // untere Wand
    MainMenu.uMenuScreen[14 * MainMenu.uXdim + 10] = EMERALD_STEEL_MARKER_LEFT_BOTTOM;
    MainMenu.uMenuScreen[14 * MainMenu.uXdim + 11] = EMERALD_STEEL_MARKER_BOTTOM;
    MainMenu.uMenuScreen[14 * MainMenu.uXdim + 12] = EMERALD_STEEL_MARKER_BOTTOM;
    MainMenu.uMenuScreen[14 * MainMenu.uXdim + 13] = EMERALD_STEEL_MARKER_BOTTOM;
    MainMenu.uMenuScreen[14 * MainMenu.uXdim + 14] = EMERALD_STEEL_MARKER_BOTTOM;
    MainMenu.uMenuScreen[14 * MainMenu.uXdim + 15] = EMERALD_STEEL_MARKER_BOTTOM;
    MainMenu.uMenuScreen[14 * MainMenu.uXdim + 16] = EMERALD_STEEL_MARKER_BOTTOM;
    MainMenu.uMenuScreen[14 * MainMenu.uXdim + 17] = EMERALD_STEEL_MARKER_BOTTOM;
    MainMenu.uMenuScreen[14 * MainMenu.uXdim + 18] = EMERALD_STEEL_MARKER_BOTTOM;
    MainMenu.uMenuScreen[14 * MainMenu.uXdim + 19] = EMERALD_STEEL_MARKER_BOTTOM;
    MainMenu.uMenuScreen[14 * MainMenu.uXdim + 20] = EMERALD_STEEL_MARKER_BOTTOM;
    MainMenu.uMenuScreen[14 * MainMenu.uXdim + 21] = EMERALD_STEEL_MARKER_BOTTOM;
    MainMenu.uMenuScreen[14 * MainMenu.uXdim + 22] = EMERALD_STEEL_MARKER_BOTTOM;
    MainMenu.uMenuScreen[14 * MainMenu.uXdim + 23] = EMERALD_STEEL_MARKER_BOTTOM;
    MainMenu.uMenuScreen[14 * MainMenu.uXdim + 24] = EMERALD_STEEL_MARKER_BOTTOM;
    MainMenu.uMenuScreen[14 * MainMenu.uXdim + 25] = EMERALD_STEEL_MARKER_BOTTOM;
    MainMenu.uMenuScreen[14 * MainMenu.uXdim + 26] = EMERALD_STEEL_MARKER_BOTTOM;
    MainMenu.uMenuScreen[14 * MainMenu.uXdim + 27] = EMERALD_STEEL_MARKER_BOTTOM;
    MainMenu.uMenuScreen[14 * MainMenu.uXdim + 28] = EMERALD_STEEL_MARKER_RIGHT_BOTTOM;
    nErrorCode = RenderMenuElements(pRenderer);
    if (nErrorCode == 0) {
        // 20 Pixel Zeilenabstand für LittleFont
        nErrorCode = PrintLittleFont(pRenderer,364,234,0,"LEVEL DIMENSION WAS CHANGED FROM",K_RELATIVE);
        sprintf(szText,"%d X %d  TO  %d X %d.",Ed.uLevel_X_Dimension,Ed.uLevel_Y_Dimension,Ed.uTmpLevel_X_Dimension,Ed.uTmpLevel_Y_Dimension);
        PrintLittleFont(pRenderer,364,254,0,szText,K_RELATIVE);
        PrintLittleFont(pRenderer,404,294,0,"COPY EXISTING LEVEL DATA FROM UPPER LEFT TO",K_RELATIVE);
        PrintLittleFont(pRenderer,404,314,0,"NEW LEVEL (AS MUCH AS POSSIBLE)",K_RELATIVE);
        PrintLittleFont(pRenderer,404,354,0,"CLEAR NEW LEVEL",K_RELATIVE);
        PrintLittleFont(pRenderer,404,394,0,"DON'T CHANGE LEVEL DIMENSION AND",K_RELATIVE);
        PrintLittleFont(pRenderer,404,414,0,"KEEP EXISTING LEVEL DATA",K_RELATIVE);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           CreateEditorButtons
------------------------------------------------------------------------------
Beschreibung: Erzeugt alle notwenigen Buttons für den Editor.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Config.x, ge_uXoffs, ge_uYoffs
------------------------------------------------------------------------------*/
int CreateEditorButtons(void) {
    int nErrors;
    // Da hier absolute Koordinaten verwendet werden, müssen die Offsets wieder abgezogen werden.

    nErrors = CreateButton(BUTTONLABEL_EDITOR_SAVE,"Save",Config.uResX - 184 - ge_uXoffs,Config.uResY - 100 - ge_uYoffs,true,true);
    nErrors = nErrors + CreateButton(BUTTONLABEL_EDITOR_QUIT,"Quit",Config.uResX - 139 - ge_uXoffs,Config.uResY - 100 - ge_uYoffs,true,true);
    nErrors = nErrors + CreateButton(BUTTONLABEL_EDITOR_YAMS,"Yams",Config.uResX - 48 - ge_uXoffs,Config.uResY - 100 - ge_uYoffs,true,true);
    nErrors = nErrors + CreateButton(BUTTONLABEL_RETURN_TO_LEVEL,"Return to level",Config.uResX - 184 - ge_uXoffs,Config.uResY - 100 - ge_uYoffs,false,true);
    nErrors = nErrors + CreateButton(BUTTONLABEL_EDITOR_YAM_MINUS,"-",Config.uResX - 382 - ge_uXoffs,Config.uResY - FONT_H + 8 - ge_uYoffs,false,true);
    nErrors = nErrors + CreateButton(BUTTONLABEL_EDITOR_YAM_PLUS,"+",Config.uResX - 265 - ge_uXoffs,Config.uResY - FONT_H + 8 - ge_uYoffs,false,true);
    nErrors = nErrors + CreateButton(BUTTONLABEL_EDITOR_MACHINES,"Machines",Config.uResX - 184 - ge_uXoffs,Config.uResY - 130 - ge_uYoffs,true,true);
    nErrors = nErrors + CreateButton(BUTTONLABEL_TIME_AND_SCORES,"Time+Scores",Config.uResX - 104 - ge_uXoffs,Config.uResY - 130 - ge_uYoffs,true,true);
    nErrors = nErrors + CreateButton(BUTTONLABEL_EDITOR_TEXT,"Text",Config.uResX - 94 - ge_uXoffs,Config.uResY - 100 - ge_uYoffs,true,true);
    nErrors = nErrors + CreateButton(BUTTONLABEL_EDITOR_STD,"Std.",Config.uResX - 49 - ge_uXoffs,Config.uResY - 100 - ge_uYoffs,false,true);
    nErrors = nErrors + CreateButton(BUTTONLABEL_EDITOR_OPTION_1,"-->",362,290,false,true);
    nErrors = nErrors + CreateButton(BUTTONLABEL_EDITOR_OPTION_2,"-->",362,350,false,true);
    nErrors = nErrors + CreateButton(BUTTONLABEL_EDITOR_OPTION_3,"-->",362,390,false,true);
    nErrors = nErrors + CreateButton(BUTTONLABEL_SAVE_MESSAGE,"Save message",(Config.uResX / 2) - 100 ,736,false,true);
    nErrors = nErrors + CreateButton(BUTTONLABEL_CANCEL_MESSAGE,"Cancel",(Config.uResX / 2) + 100 ,736,false,true);
    return nErrors;
}


/*----------------------------------------------------------------------------
Name:           FreeEditorButtons
------------------------------------------------------------------------------
Beschreibung: Gibt alle Buttons des Editors frei.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: -
------------------------------------------------------------------------------*/
void FreeEditorButtons(void) {
    FreeButton(BUTTONLABEL_EDITOR_SAVE);
    FreeButton(BUTTONLABEL_EDITOR_QUIT);
    FreeButton(BUTTONLABEL_EDITOR_YAMS);
    FreeButton(BUTTONLABEL_RETURN_TO_LEVEL);
    FreeButton(BUTTONLABEL_EDITOR_YAM_MINUS);
    FreeButton(BUTTONLABEL_EDITOR_YAM_PLUS);
    FreeButton(BUTTONLABEL_EDITOR_MACHINES);
    FreeButton(BUTTONLABEL_TIME_AND_SCORES);
    FreeButton(BUTTONLABEL_EDITOR_TEXT);
    FreeButton(BUTTONLABEL_EDITOR_STD);
    FreeButton(BUTTONLABEL_EDITOR_OPTION_1);
    FreeButton(BUTTONLABEL_EDITOR_OPTION_2);
    FreeButton(BUTTONLABEL_EDITOR_OPTION_3);
    FreeButton(BUTTONLABEL_SAVE_MESSAGE);
    FreeButton(BUTTONLABEL_CANCEL_MESSAGE);
}


/*----------------------------------------------------------------------------
Name:           CreateNewLevel
------------------------------------------------------------------------------
Beschreibung: Erzeugt ein neues Spielfeld für den Leveleditor nach den
              Abmaßen die in Ed.uTmpLevel_X_Dimension und Ed.uTmpLevel_Y_Dimension
              gesetzt sind.

Parameter
      Eingang: bCopyOldLeveldata, bool, true = kopiere die bestehenden Leveldaten ab
                    oben links in das neue Level, false = leeres Level
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Ed.x
------------------------------------------------------------------------------*/
int CreateNewLevel(bool bCopyOldLeveldata) {
    int nErrorCode;
    uint16_t *pLevel;
    uint32_t I;
    uint32_t uOldX_Dimension;
    uint32_t uOldY_Dimension;
    uint32_t uX;
    uint32_t uY;

    uOldX_Dimension = Ed.uLevel_X_Dimension;
    uOldY_Dimension = Ed.uLevel_Y_Dimension;
    nErrorCode = -1;
    if ( (Ed.uTmpLevel_X_Dimension >= MIN_LEVEL_W) && (Ed.uTmpLevel_X_Dimension <= MAX_LEVEL_W) &&
        (Ed.uTmpLevel_Y_Dimension >= MIN_LEVEL_H) && (Ed.uTmpLevel_Y_Dimension <= MAX_LEVEL_H) ) {
        pLevel = (uint16_t*)malloc(Ed.uTmpLevel_X_Dimension * Ed.uTmpLevel_Y_Dimension * sizeof(uint16_t));
        if (pLevel != NULL) {
            SDL_Log("malloc successfull for x:%d  y:%d",Ed.uTmpLevel_X_Dimension,Ed.uTmpLevel_Y_Dimension);
            // Neue Leveldimension setzen
            Ed.uLevel_X_Dimension = Ed.uTmpLevel_X_Dimension;
            Ed.uLevel_Y_Dimension = Ed.uTmpLevel_Y_Dimension;
            // Komplettes Level auf EMERALD_SPACE setzen
            for (I = 0; I < Ed.uLevel_X_Dimension * Ed.uLevel_Y_Dimension; I++) {
                pLevel[I] = EMERALD_SPACE;
            }
            if (SetLevelBorder(pLevel,true,true) == 0) { // true = Spielfeld auf EMERALD_SPACE setzen, 2. true = Border aus EMERALD_STEEL
                if (bCopyOldLeveldata) {
                    for (uY = 1; (uY < (uOldY_Dimension - 1)) && (uY < (Ed.uLevel_Y_Dimension - 1)); uY++) {
                        for (uX = 1; (uX < (uOldX_Dimension - 1)) && (uX < (Ed.uLevel_X_Dimension - 1)); uX++) {
                            if (Ed.pLevel[uY * uOldX_Dimension + uX] != EMERALD_MAN) {
                                // Man nicht mit kopieren
                                pLevel[uY * Ed.uLevel_X_Dimension + uX] = Ed.pLevel[uY * uOldX_Dimension + uX];
                            }
                        }
                    }
                }
                // Einen Man oben links setzen
                pLevel[Ed.uLevel_X_Dimension + 1] = EMERALD_MAN;
                // Wenn alles gut gegangen ist, kann das alte Spielfeld gelöscht werden
                SAFE_FREE(Ed.pLevel);
                // dann neues Spielfeld zuweisen
                Ed.pLevel = pLevel;
                //  Sichtbereich für Editor neu berechnen
                CalcEditorViewArea();
                // Oben links positionieren
                Ed.nXpos = 0;
                Ed.nYpos = 0;
                nErrorCode = 0;
            } else {
                SDL_Log("%s: SetLevelBorder() failed, p: %p    x: %d    y: %d",__FUNCTION__,pLevel,Ed.uTmpLevel_X_Dimension,Ed.uTmpLevel_Y_Dimension);
            }
        } else {
            SDL_Log("%s: malloc failed for level dimensions   x: %d    y: %d",__FUNCTION__,Ed.uTmpLevel_X_Dimension,Ed.uTmpLevel_Y_Dimension);
        }
    } else {
        SDL_Log("%s: bad level dimensions   x: %d    y: %d",__FUNCTION__,Ed.uTmpLevel_X_Dimension,Ed.uTmpLevel_Y_Dimension);
    }
    if (nErrorCode != 0) {
        // Bei Fehler alte Leveldaten nicht löschen und alte Level-Dimension setzen
        Ed.uLevel_X_Dimension = uOldX_Dimension;
        Ed.uLevel_Y_Dimension = uOldY_Dimension;
        Ed.uTmpLevel_X_Dimension = uOldX_Dimension;
        Ed.uTmpLevel_Y_Dimension = uOldY_Dimension;
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           SaveNewMessage
------------------------------------------------------------------------------
Beschreibung: Speichert eine aktuellen/geänderte Nachricht.

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Ed.x
------------------------------------------------------------------------------*/
int SaveNewMessage(void) {
    int nErrorCode;
    int nPos;
    int nE;

    nErrorCode = 0;
    if ((Ed.MessageEditor.nEditMessage >= 0) && (Ed.MessageEditor.nEditMessage < EMERALD_MAX_MESSAGES)) {
        nPos = strlen(Ed.MessageEditor.szMessageEditorMem) - 1; // Auf letztes Zeichen des Strings setzen
        SDL_Log("Save message no. %d   MessageLen: %d",Ed.MessageEditor.nEditMessage,(int)strlen(Ed.MessageEditor.szMessageEditorMem));
        if (nPos >= 0) {    // Wenn nPos = 0, dann gibt es ein Zeichen
            // Von hinten Spaces entfernen
            for (nE = nPos; nE >= 0; nE--) {
                if (Ed.MessageEditor.szMessageEditorMem[nE] == ' ') {  // Space ?
                    Ed.MessageEditor.szMessageEditorMem[nE] = 0;
                } else {
                    break;
                }
            }
            if (strlen(Ed.MessageEditor.szMessageEditorMem) > 0) {
                // aktuellen Message-Speicher freigeben, wenn einer existiert
                SAFE_FREE(Ed.pMessage[Ed.MessageEditor.nEditMessage]);
                Ed.pMessage[Ed.MessageEditor.nEditMessage] = malloc(strlen(Ed.MessageEditor.szMessageEditorMem) + 1);
                if (Ed.pMessage[Ed.MessageEditor.nEditMessage] != NULL) {
                    strcpy(Ed.pMessage[Ed.MessageEditor.nEditMessage],Ed.MessageEditor.szMessageEditorMem);
                } else {
                    SDL_Log("%s: malloc failed for message no. %d  len = %d",__FUNCTION__,Ed.MessageEditor.nEditMessage,(int)strlen(Ed.MessageEditor.szMessageEditorMem));
                    nErrorCode = -1;
                }
            } else {
                // String ist nun leer
                SAFE_FREE(Ed.pMessage[Ed.MessageEditor.nEditMessage]);
            }
        } else {
            // String ist leer
            SAFE_FREE(Ed.pMessage[Ed.MessageEditor.nEditMessage]);
        }
        SDL_Log("Message no. %d saved   MessageLen: %d",Ed.MessageEditor.nEditMessage,(int)strlen(Ed.MessageEditor.szMessageEditorMem));
    }
    Ed.MessageEditor.nEditMessage = -1;
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           InitEditor
------------------------------------------------------------------------------
Beschreibung: Initialisiert die Struktur Ed.x für den Editor.

Parameter
      Eingang: bNewLevel, bool, TRUE = Ein neues Level wird erzeugt mit den Abmaßen uXdim und uYdim.
               uXdim, X-Dimension für neues Level, wird nur verwendet, wenn bNewLevel = true
               uYdim, Y-Dimension für neues Level, wird nur verwendet, wenn bNewLevel = true
               nLevel, int, Levelnummer innerhalb einer gewählten Levelgruppe, wird nur verwendet, wenn bNewLevel = false
      Ausgang: -
Rückgabewert:  0 = alles OK, sonst Fehler
Seiteneffekte: Ed.x, Config.x
------------------------------------------------------------------------------*/
int InitEditor(bool bNewLevel, uint32_t uXdim, uint32_t uYdim, int nLevel) {
    int nErrorCode;

    nErrorCode = -1;
    memset(&Ed,0,sizeof(Ed));
    Ed.uPanelW = 192;                                       // Breite des Panels im Editor
    Ed.uPanelH = Config.uResX;                              // Höhe des Panels im Editor
    Ed.uPanelXpos = Config.uResX - Ed.uPanelW;              // X-Positionierung des Panels im Editor
    Ed.uPanelYpos = 0;                                      // Y-Positionierung des Panels im Editor
    InitYamExplosions(Ed.YamExplosions);
    Ed.uReplicatorRedObject = EMERALD_SPACE;
    Ed.uReplicatorGreenObject = EMERALD_SPACE;
    Ed.uReplicatorBlueObject = EMERALD_SPACE;
    Ed.uReplicatorYellowObject = EMERALD_SPACE;
    Ed.uConveybeltRedState = EMERALD_CONVEYBELT_OFF;
    Ed.uConveybeltRedDirection = EMERALD_CONVEYBELT_TO_LEFT;
    Ed.uConveybeltGreenState = EMERALD_CONVEYBELT_OFF;
    Ed.uConveybeltGreenDirection = EMERALD_CONVEYBELT_TO_LEFT;
    Ed.uConveybeltBlueState = EMERALD_CONVEYBELT_OFF;
    Ed.uConveybeltBlueDirection = EMERALD_CONVEYBELT_TO_LEFT;
    Ed.uConveybeltYellowState = EMERALD_CONVEYBELT_OFF;
    Ed.uConveybeltYellowDirection = EMERALD_CONVEYBELT_TO_LEFT;
    memset(Ed.szLevelTitle,0,sizeof(Ed.szLevelTitle));      // z.B. "Der Bunker"
    memset(Ed.szLevelAuthor,0,sizeof(Ed.szLevelAuthor));    // z.B. "Mikiman"
    strcpy(Ed.szVersion,EMERALD_VERSION);                   // z.B. "01.00"
    InitMessageEditor();
    Ed.uMenuState = MENUSTATE_LEVEL_STD;
    SetPanelElements(Ed.uMenuState);
    Ed.uMaxYamExplosionIndex = 0;
    if ((bNewLevel) && (uXdim >= MIN_LEVEL_W) && (uXdim <= MAX_LEVEL_W) && (uYdim >= MIN_LEVEL_H) && (uYdim <= MAX_LEVEL_H)) {
        Ed.uLevel_X_Dimension = uXdim;
        Ed.uLevel_Y_Dimension = uYdim;
        Ed.pLevel = (uint16_t*)malloc(Ed.uLevel_X_Dimension * Ed.uLevel_Y_Dimension * sizeof(uint16_t));
        if (Ed.pLevel != NULL) {
            SetLevelBorder(Ed.pLevel,true,true); // true = Spielfeld auf EMERALD_SPACE setzen, 2. true = Border aus EMERALD_STEEL
            nErrorCode = 0;
        } else {
            SDL_Log("%s: malloc() failed for new level (x: %u  y: %u)",__FUNCTION__,uXdim,uYdim);
            return -1;
        }
    } else {
        if (InitialisePlayfield(nLevel) == 0) {
            if (CopyPlayfieldValueToEditor() == 0) {
                nErrorCode = 0;
            }
            // Playfield.pLevel darf hier nicht freigegeben werden, da dieser Pointer sich in Ed.pLevel befindet
            SAFE_FREE(Playfield.pInvalidElement);
            SAFE_FREE(Playfield.pStatusAnimation);
            SAFE_FREE(Playfield.pPostAnimation);
        } else {
            SDL_Log("%s: InitialisePlayfield() failed",__FUNCTION__);
            return -1;
        }
    }
    if (nErrorCode == 0) {
        Ed.uFrameCounter = 0;
        Ed.bHalfSize = !Config.bEditorZoom;
        Ed.uSelectedElementLeft[MENUSTATE_LEVEL_STD] = EMERALD_SAND;
        Ed.uSelectedElementMiddle[MENUSTATE_LEVEL_STD] = EMERALD_STONE;
        Ed.uSelectedElementRight[MENUSTATE_LEVEL_STD] = EMERALD_STEEL;

        Ed.uSelectedElementLeft[MENUSTATE_LEVEL_TEXT] = EMERALD_SPACE;
        Ed.uSelectedElementMiddle[MENUSTATE_LEVEL_TEXT] = EMERALD_SPACE;
        Ed.uSelectedElementRight[MENUSTATE_LEVEL_TEXT] = EMERALD_SAND;

        Ed.uSelectedElementLeft[MENUSTATE_YAMS] = EMERALD_STONE;
        Ed.uSelectedElementMiddle[MENUSTATE_YAMS] = EMERALD_EMERALD;
        Ed.uSelectedElementRight[MENUSTATE_YAMS] = EMERALD_SAPPHIRE;

        Ed.uSelectedElementLeft[MENUSTATE_YAMS_TEXT] = EMERALD_SPACE;
        Ed.uSelectedElementMiddle[MENUSTATE_YAMS_TEXT] = EMERALD_SPACE;
        Ed.uSelectedElementRight[MENUSTATE_YAMS_TEXT] = EMERALD_SPACE;

        Ed.uSelectedElementLeft[MENUSTATE_MACHINES] = EMERALD_STONE;
        Ed.uSelectedElementMiddle[MENUSTATE_MACHINES] = EMERALD_EMERALD;
        Ed.uSelectedElementRight[MENUSTATE_MACHINES] = EMERALD_SAPPHIRE;

        Ed.uSelectedElementLeft[MENUSTATE_TIME_AND_SCORES] = EMERALD_SPACE;
        Ed.uSelectedElementMiddle[MENUSTATE_TIME_AND_SCORES] = EMERALD_SPACE;
        Ed.uSelectedElementRight[MENUSTATE_TIME_AND_SCORES] = EMERALD_SPACE;

        Ed.uSelectedElementLeft[MENUSTATE_CONFIRM_NEWLEVEL_DIMENSION] = EMERALD_SPACE;
        Ed.uSelectedElementMiddle[MENUSTATE_CONFIRM_NEWLEVEL_DIMENSION] = EMERALD_SPACE;
        Ed.uSelectedElementRight[MENUSTATE_CONFIRM_NEWLEVEL_DIMENSION] = EMERALD_SPACE;

        Ed.uSelectedElementLeft[MENUSTATE_TIME_AND_SCORES_MESSAGE] = EMERALD_SPACE;
        Ed.uSelectedElementMiddle[MENUSTATE_TIME_AND_SCORES_MESSAGE] = EMERALD_SPACE;
        Ed.uSelectedElementRight[MENUSTATE_TIME_AND_SCORES_MESSAGE] = EMERALD_SPACE;

        Ed.uSelectedElementLeft[MENUSTATE_TIME_AND_SCORES_EDIT_TITLE] = EMERALD_SPACE;
        Ed.uSelectedElementMiddle[MENUSTATE_TIME_AND_SCORES_EDIT_TITLE] = EMERALD_SPACE;
        Ed.uSelectedElementRight[MENUSTATE_TIME_AND_SCORES_EDIT_TITLE] = EMERALD_SPACE;

        Ed.uSelectedElementLeft[MENUSTATE_TIME_AND_SCORES_EDIT_AUTHOR] = EMERALD_SPACE;
        Ed.uSelectedElementMiddle[MENUSTATE_TIME_AND_SCORES_EDIT_AUTHOR] = EMERALD_SPACE;
        Ed.uSelectedElementRight[MENUSTATE_TIME_AND_SCORES_EDIT_AUTHOR] = EMERALD_SPACE;


        CalcEditorViewArea();
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           Editor
------------------------------------------------------------------------------
Beschreibung: Hauptfunktion (Einsprungsfunktion) für den Level-Editor.
              Diese Funktion alloziert Speicher über die Funktion GetLevelXmlFile(),
              der wieder freigegeben werdn muss.
Parameter
      Eingang: SDL_Renderer *, pRenderer, Zeiger auf Renderer
               nLevel, int, Levelnummer innerhalb einer gewählten Levelgruppe, -1 = Level wird nicht verwendet
      Ausgang: -
Rückgabewert:  DYNSTRING , XML-Daten des Levels, NULL = Fehler oder Quit
Seiteneffekte: Ed.x, InputStates.x, Config.x, ge_uXoffs, ge_uYoffs
------------------------------------------------------------------------------*/
DYNSTRING *Editor(SDL_Renderer *pRenderer, int nLevel) {
    uint32_t uMouseElement;             // Levelelement, auf das Mauspfeil zeigt
    uint32_t I;
    DYNSTRING *XML = NULL;

    Ed.nXpos = 0;
    Ed.nYpos = 0;
    if (nLevel >= 0) {
        Ed.bEditorRun = (InitEditor(false,0,0,nLevel) == 0);
    } else {
        Ed.bEditorRun = (InitEditor(true,110,110,-1) == 0);
    }
    if (Ed.bEditorRun) {
        Ed.bEditorRun = (CreateEditorButtons() == 0);
    }
    PreCalcYamCoords();
    SetAllTextureColors(100);
    while (Ed.bEditorRun) {
        UpdateInputStates();
        uMouseElement = GetElementByMouseposition(InputStates.nMouseXpos_Absolute, InputStates.nMouseYpos_Absolute);
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
        if ((Ed.uMenuState == MENUSTATE_LEVEL_STD) || (Ed.uMenuState == MENUSTATE_LEVEL_TEXT))  {
            Ed.bEditorRun = (EditorStateLevel(pRenderer) == 0);
            if (GetKey() == 'Z') {  // Zoom toggeln
                do {
                    UpdateInputStates();
                    SDL_Delay(5);
                } while ((InputStates.pKeyboardArray[SDL_SCANCODE_Z]) || (InputStates.pKeyboardArray[SDL_SCANCODE_Y]));
                Config.bEditorZoom = !Config.bEditorZoom;
                Ed.bHalfSize = !Config.bEditorZoom;
                Ed.bEditorRun = (WriteConfigFile() == 0);
                CalcEditorViewArea();
            }
        } else if ((Ed.uMenuState == MENUSTATE_YAMS) || (Ed.uMenuState == MENUSTATE_YAMS_TEXT)) {
            Ed.bEditorRun = (EditorStateYams(pRenderer) == 0);
        } else if (Ed.uMenuState == MENUSTATE_MACHINES) {
            Ed.bEditorRun = (EditorStateMachines(pRenderer) == 0);
        } else if ((Ed.uMenuState == MENUSTATE_TIME_AND_SCORES) || (Ed.uMenuState == MENUSTATE_TIME_AND_SCORES_MESSAGE) ||
                   (Ed.uMenuState == MENUSTATE_TIME_AND_SCORES_EDIT_TITLE) || (Ed.uMenuState == MENUSTATE_TIME_AND_SCORES_EDIT_AUTHOR)) {
            Ed.bEditorRun = (EditorStateTimeAndScores(pRenderer) == 0);
        } else if (Ed.uMenuState == MENUSTATE_CONFIRM_NEWLEVEL_DIMENSION) {
            Ed.bEditorRun = (EditorStateConfirmNewLevelDimension(pRenderer) == 0);
        }
        if ((Ed.uMenuState != MENUSTATE_CONFIRM_NEWLEVEL_DIMENSION) && (Ed.uMenuState != MENUSTATE_TIME_AND_SCORES_MESSAGE)) {
            DrawEditorPanel(pRenderer);
            PrintLittleFont(pRenderer, 0, Config.uResY - FONT_LITTLE_347_H,0,ge_szElementNames[uMouseElement],K_ABSOLUTE);// Element unten links anzeigen
        }
        ShowButtons(pRenderer);
        if (IsButtonPressed(BUTTONLABEL_EDITOR_SAVE) && (!Ed.bFoundError)) {
            XML = GetLevelXmlFromEditor();
            Ed.bEditorRun = false;
        } else if (IsButtonPressed(BUTTONLABEL_EDITOR_QUIT) && (!Ed.bFoundError)) {
            Ed.bEditorRun = false;
        } else if (IsButtonPressed(BUTTONLABEL_EDITOR_YAMS)) {
            Ed.uMenuState = MENUSTATE_YAMS;
            SetPanelElements(Ed.uMenuState);
            SetButtonActivity(BUTTONLABEL_EDITOR_MACHINES,false);
            SetButtonActivity(BUTTONLABEL_TIME_AND_SCORES,false);
            SetButtonActivity(BUTTONLABEL_EDITOR_SAVE,false);
            SetButtonActivity(BUTTONLABEL_EDITOR_QUIT,false);
            SetButtonActivity(BUTTONLABEL_EDITOR_YAMS,false);
            SetButtonActivity(BUTTONLABEL_RETURN_TO_LEVEL,true);
            SetButtonActivity(BUTTONLABEL_EDITOR_YAM_PLUS,true);
            SetButtonActivity(BUTTONLABEL_EDITOR_YAM_MINUS,true);
            SetButtonActivity(BUTTONLABEL_EDITOR_TEXT,true);
            SetButtonPosition(BUTTONLABEL_EDITOR_TEXT,Config.uResX - 49 - ge_uXoffs,Config.uResY - 100 - ge_uYoffs);
        } else if ((IsButtonPressed(BUTTONLABEL_RETURN_TO_LEVEL)) && (!Ed.bFoundError)) {
            // Wenn sich Level-Dimension geändert hat, dann Warnung bzw. weiteres Vorgehen anbieten
            if ((Ed.uTmpLevel_X_Dimension != Ed.uLevel_X_Dimension) || (Ed.uTmpLevel_Y_Dimension != Ed.uLevel_Y_Dimension)) {
                Ed.uMenuState = MENUSTATE_CONFIRM_NEWLEVEL_DIMENSION;
                SetButtonActivity(BUTTONLABEL_EDITOR_OPTION_1,true);
                SetButtonActivity(BUTTONLABEL_EDITOR_OPTION_2,true);
                SetButtonActivity(BUTTONLABEL_EDITOR_OPTION_3,true);
            } else {
                Ed.uMenuState = MENUSTATE_LEVEL_STD;
                SetPanelElements(Ed.uMenuState);
                SetButtonActivity(BUTTONLABEL_EDITOR_SAVE,true);
                SetButtonActivity(BUTTONLABEL_EDITOR_QUIT,true);
                SetButtonActivity(BUTTONLABEL_EDITOR_YAMS,true);
                SetButtonActivity(BUTTONLABEL_EDITOR_YAM_PLUS,false);
                SetButtonActivity(BUTTONLABEL_EDITOR_YAM_MINUS,false);
                SetButtonActivity(BUTTONLABEL_EDITOR_MACHINES,true);
                SetButtonActivity(BUTTONLABEL_TIME_AND_SCORES,true);
                SetButtonActivity(BUTTONLABEL_EDITOR_TEXT,true);
                SetButtonActivity(BUTTONLABEL_EDITOR_STD,false);
                SetButtonPosition(BUTTONLABEL_EDITOR_TEXT,Config.uResX - 94 - ge_uXoffs,Config.uResY - 100 - ge_uYoffs);
            }
            SetButtonActivity(BUTTONLABEL_RETURN_TO_LEVEL,false);
        } else if (IsButtonPressed(BUTTONLABEL_EDITOR_YAM_PLUS)) {
            if (Ed.uMaxYamExplosionIndex < (EMERALD_MAX_YAM_EXPLOSIONS - 1)) {
                Ed.uMaxYamExplosionIndex++;
            }
        } else if (IsButtonPressed(BUTTONLABEL_EDITOR_YAM_MINUS)) {
            if (Ed.uMaxYamExplosionIndex > 0) {
                Ed.uMaxYamExplosionIndex--;
            }
        } else if (IsButtonPressed(BUTTONLABEL_EDITOR_MACHINES)) {
            Ed.uMenuState = MENUSTATE_MACHINES;
            SetPanelElements(Ed.uMenuState);
            SetButtonActivity(BUTTONLABEL_RETURN_TO_LEVEL,true);
            SetButtonActivity(BUTTONLABEL_EDITOR_MACHINES,false);
            SetButtonActivity(BUTTONLABEL_TIME_AND_SCORES,false);
            SetButtonActivity(BUTTONLABEL_EDITOR_YAMS,false);
            SetButtonActivity(BUTTONLABEL_EDITOR_SAVE,false);
            SetButtonActivity(BUTTONLABEL_EDITOR_QUIT,false);
            SetButtonActivity(BUTTONLABEL_EDITOR_TEXT,false);
        } else if ((IsButtonPressed(BUTTONLABEL_TIME_AND_SCORES)) || (IsButtonPressed(BUTTONLABEL_SAVE_MESSAGE)) || (IsButtonPressed(BUTTONLABEL_CANCEL_MESSAGE))) {
            Ed.uMenuState = MENUSTATE_TIME_AND_SCORES;
            SetPanelElements(Ed.uMenuState);
            SetButtonActivity(BUTTONLABEL_RETURN_TO_LEVEL,true);
            SetButtonActivity(BUTTONLABEL_EDITOR_MACHINES,false);
            SetButtonActivity(BUTTONLABEL_TIME_AND_SCORES,false);
            SetButtonActivity(BUTTONLABEL_EDITOR_YAMS,false);
            SetButtonActivity(BUTTONLABEL_EDITOR_SAVE,false);
            SetButtonActivity(BUTTONLABEL_EDITOR_QUIT,false);
            SetButtonActivity(BUTTONLABEL_EDITOR_TEXT,false);
            SetButtonActivity(BUTTONLABEL_SAVE_MESSAGE,false);
            SetButtonActivity(BUTTONLABEL_CANCEL_MESSAGE,false);
            // unabhängig vom Zustand des Cursor das originale Zeichen in den Puffer zurückschreiben
            Ed.MessageEditor.szMessageEditorMem[Ed.MessageEditor.uCursorPos] = Ed.MessageEditor.cCharAtCursor;
            if (IsButtonPressed(BUTTONLABEL_SAVE_MESSAGE)) {
                Ed.bEditorRun = (SaveNewMessage() == 0);
            }
        } else if (IsButtonPressed(BUTTONLABEL_EDITOR_TEXT)) {
            // Prüfen, in welchem Menü der Text-Button gedrückt wurde
            if (Ed.uMenuState == MENUSTATE_LEVEL_STD) {
                Ed.uMenuState = MENUSTATE_LEVEL_TEXT;
            } else if (Ed.uMenuState == MENUSTATE_YAMS) {
                Ed.uMenuState = MENUSTATE_YAMS_TEXT;
                SetButtonActivity(BUTTONLABEL_EDITOR_STD,true);
            }
            SetPanelElements(Ed.uMenuState);
            SetButtonActivity(BUTTONLABEL_RETURN_TO_LEVEL,true);
            SetButtonActivity(BUTTONLABEL_EDITOR_MACHINES,false);
            SetButtonActivity(BUTTONLABEL_TIME_AND_SCORES,false);
            SetButtonActivity(BUTTONLABEL_EDITOR_YAMS,false);
            SetButtonActivity(BUTTONLABEL_EDITOR_SAVE,false);
            SetButtonActivity(BUTTONLABEL_EDITOR_QUIT,false);
            SetButtonActivity(BUTTONLABEL_EDITOR_TEXT,false);
        } else if (IsButtonPressed(BUTTONLABEL_EDITOR_STD)) {
            // Kann nur aus Yam-Menü gedrückt werden
            if (Ed.uMenuState == MENUSTATE_YAMS_TEXT) {
                Ed.uMenuState = MENUSTATE_YAMS;
                SetPanelElements(Ed.uMenuState);
                SetButtonActivity(BUTTONLABEL_EDITOR_TEXT,true);
                SetButtonActivity(BUTTONLABEL_EDITOR_STD,false);
            }
        } else if ( (IsButtonPressed(BUTTONLABEL_EDITOR_OPTION_1)) || (IsButtonPressed(BUTTONLABEL_EDITOR_OPTION_2)) ) {
            if (IsButtonPressed(BUTTONLABEL_EDITOR_OPTION_1)) {
                Ed.bEditorRun = (CreateNewLevel(true) == 0);
            } else {
                Ed.bEditorRun = (CreateNewLevel(false) == 0);
            }
            SetButtonActivity(BUTTONLABEL_EDITOR_OPTION_1,false);
            SetButtonActivity(BUTTONLABEL_EDITOR_OPTION_2,false);
            SetButtonActivity(BUTTONLABEL_EDITOR_OPTION_3,false);
            Ed.uMenuState = MENUSTATE_LEVEL_STD;
            SetPanelElements(Ed.uMenuState);
            SetButtonActivity(BUTTONLABEL_EDITOR_SAVE,true);
            SetButtonActivity(BUTTONLABEL_EDITOR_QUIT,true);
            SetButtonActivity(BUTTONLABEL_EDITOR_YAMS,true);
            SetButtonActivity(BUTTONLABEL_EDITOR_YAM_PLUS,false);
            SetButtonActivity(BUTTONLABEL_EDITOR_YAM_MINUS,false);
            SetButtonActivity(BUTTONLABEL_EDITOR_MACHINES,true);
            SetButtonActivity(BUTTONLABEL_TIME_AND_SCORES,true);
            SetButtonActivity(BUTTONLABEL_EDITOR_TEXT,true);
            SetButtonActivity(BUTTONLABEL_EDITOR_STD,false);
            SetButtonPosition(BUTTONLABEL_EDITOR_TEXT,Config.uResX - 94,Config.uResY - 100);
        } else if (IsButtonPressed(BUTTONLABEL_EDITOR_OPTION_3)) {
            Ed.uTmpLevel_X_Dimension = Ed.uLevel_X_Dimension;
            Ed.uTmpLevel_Y_Dimension = Ed.uLevel_Y_Dimension;
            SetButtonActivity(BUTTONLABEL_EDITOR_OPTION_1,false);
            SetButtonActivity(BUTTONLABEL_EDITOR_OPTION_2,false);
            SetButtonActivity(BUTTONLABEL_EDITOR_OPTION_3,false);
            Ed.uMenuState = MENUSTATE_LEVEL_STD;
            SetPanelElements(Ed.uMenuState);
            SetButtonActivity(BUTTONLABEL_EDITOR_SAVE,true);
            SetButtonActivity(BUTTONLABEL_EDITOR_QUIT,true);
            SetButtonActivity(BUTTONLABEL_EDITOR_YAMS,true);
            SetButtonActivity(BUTTONLABEL_EDITOR_YAM_PLUS,false);
            SetButtonActivity(BUTTONLABEL_EDITOR_YAM_MINUS,false);
            SetButtonActivity(BUTTONLABEL_EDITOR_MACHINES,true);
            SetButtonActivity(BUTTONLABEL_TIME_AND_SCORES,true);
            SetButtonActivity(BUTTONLABEL_EDITOR_TEXT,true);
            SetButtonActivity(BUTTONLABEL_EDITOR_STD,false);
            SetButtonPosition(BUTTONLABEL_EDITOR_TEXT,Config.uResX - 94,Config.uResY - 100);
        }
        SDL_RenderPresent(pRenderer);   // Renderer anzeigen, lässt Hauptschleife mit ~ 60 Hz (Bild-Wiederholfrequenz) laufen
        SDL_RenderClear(pRenderer);     // Renderer für nächstes Frame löschen
        Ed.uFrameCounter++;
        Playfield.uFrameCounter++;      // Einige Elemente benötigen den Playfield-Counter in der Funktion GetTextureIndexByElement();
        SDL_Delay(5);
    }
    FreeEditorButtons();
    SAFE_FREE(Ed.pLevel);
    for (I = 0; I <EMERALD_MAX_MESSAGES; I++) {
        if (Ed.pMessage[I] != NULL) {
            SAFE_FREE(Ed.pMessage[I]);
        }
    }
    WaitNoKey();
    return XML;
}


/*----------------------------------------------------------------------------
Name:           UpdateLevelgroupHash
------------------------------------------------------------------------------
Beschreibung: Aktualisiert den MD5-Hash in einer XML-Levelgruppendatei.
Parameter
      Eingang: pszInput, uint8_t *, Zeiger auf XML-Daten, muss \0 terminiert sein
      Ausgang: puCalculatedHash, uint_t *, Zeiger auf berechneten MD5-Levelgruppen-Hash, mindestens 16 Bytes
                    Dieser Pointer darf NULL sein.
Rückgabewert:  int, 0 =  Alles OK, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int UpdateLevelgroupHash(uint8_t *pszInput, uint8_t *puCalculatedHash) {
    int nErrorCode = -1;
    uint8_t *pHashStartTag;
    uint8_t *phashEndTag;
    uint8_t uHash[16];
    char szHashString[32 + 1];

    if (pszInput != NULL) {
        pHashStartTag = (uint8_t*)strstr((char*)pszInput,"<levelgroup_md5_hash>");
        phashEndTag = (uint8_t*)strstr((char*)pszInput,"</levelgroup_md5_hash>");
        if ((pHashStartTag != NULL) && (phashEndTag != NULL)) {
            if ((phashEndTag - pHashStartTag) == 53) {
                if (CalculateLevelGroupMd5Hash(pszInput,uHash) == 0) {
                    if (puCalculatedHash != NULL) {
                        memcpy(puCalculatedHash,uHash,16);
                    }
                    GetMd5String(uHash,szHashString);
                    memcpy(pHashStartTag + strlen("<levelgroup_md5_hash>"),szHashString,32);
                    nErrorCode = 0;
                }
            }
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           UpdateCreateTimestamp
------------------------------------------------------------------------------
Beschreibung: Aktualisiert den Zeitstempel in einer XML-Levelgruppendatei.
Parameter
      Eingang: pszInput, uint8_t *, Zeiger auf XML-Daten, muss \0 terminiert sein
      Ausgang: -
Rückgabewert:  int, 0 =  Alles OK, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int UpdateCreateTimestamp(uint8_t *pszInput) {
    int nErrorCode = -1;
    uint8_t *pDateStartTag;
    uint8_t *pDateEndTag;
    char szTimestamp[16];

    if (pszInput != NULL) {
        pDateStartTag = (uint8_t*)strstr((char*)pszInput,"<create_timestamp>");
        pDateEndTag = (uint8_t*)strstr((char*)pszInput,"</create_timestamp>");
        if ((pDateStartTag != NULL) && (pDateEndTag != NULL)) {
            if ((pDateEndTag - pDateStartTag) == 33) {
                GetActualTimestamp(szTimestamp);
                memcpy(pDateStartTag + strlen("<create_timestamp>"),szTimestamp,15);
                nErrorCode = 0;
            }
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetStartOrEndLevelTag
------------------------------------------------------------------------------
Beschreibung: Sucht innerhalb einer XML-Datei ein Level-Start/End-Tag.
Parameter
      Eingang: pszInput, uint8_t *, Zeiger auf XML-Daten, muss \0 terminiert sein
               pEndpointer, uint8_t *, Zeiger auf Ende der XML-Daten (soweit darf gesucht werden)
               bStartFlag, bool, true = Start-Flag, sonst End-Flag
      Ausgang: -
Rückgabewert:  uint8_t *, Zeiger auf Level-Start/End-Tag, NULL = nicht gefunden
Seiteneffekte: -
------------------------------------------------------------------------------*/
uint8_t *GetStartOrEndLevelTag(uint8_t *pszInput, uint8_t *pEndpointer, bool bStartFlag) {
    uint8_t *pFound = NULL;
    uint8_t *pSearch;
    char szTag[16];     //     <level998> || </level998>

    if ((pszInput != NULL) && (pEndpointer != NULL)) {
        if (pEndpointer > pszInput) {
            if (bStartFlag) {
                strcpy(szTag,"<level");
            } else {
                strcpy(szTag,"</level");
            }
            pSearch = (uint8_t*)strstr((char*)pszInput,szTag);
            do {
                if (pSearch != NULL) {
                    if ((pSearch + 10) < pEndpointer) {
                        if ((bStartFlag) && (pSearch[9] == '>') && (isdigit(pSearch[6])) && (isdigit(pSearch[7])) && (isdigit(pSearch[8]))) {
                            pFound = pSearch;
                        } else if ((!bStartFlag) && (pSearch[10] == '>') && (isdigit(pSearch[7])) && (isdigit(pSearch[8])) && (isdigit(pSearch[9]))) {
                            pFound = pSearch;
                        } else {
                            pSearch = pSearch + 9;  // ist für Start- und Ende-Tag OK
                            pSearch = (uint8_t*)strstr((char*)pSearch,szTag);
                        }
                    } else {
                        pSearch = NULL; // Suche abbrechen
                    }
                }
            } while ((pFound == NULL) && (pSearch != NULL));
        }
    }
    return pFound;
}


/*----------------------------------------------------------------------------
Name:           RenumLevelgroup
------------------------------------------------------------------------------
Beschreibung: Re-nummeriert die Levelnummern einer Levelgruppe.
Parameter
      Eingang: pszXml, uint8_t *, Zeiger auf XML-Daten einer Levelgruppe, muss \0 terminiert sein
      Ausgang: pszXml, uint8_t *, Zeiger auf XML-Daten der Eingangs-Levelgruppe (jetzt neu nummeriert)
Rückgabewert:  > 0 = Alles OK, sonst Fehler (Anzahl gezählter Level)
Seiteneffekte: -
------------------------------------------------------------------------------*/
int RenumLevelgroup(uint8_t *pszXml) {
    int nRet = -1;
    uint8_t *puLevelgroupMd5TagStart;
    uint8_t *puLastTag;
    uint8_t *pSearch;
    uint32_t uLevelNum = 0;
    char szLevelNum[8];
    int nCount = 0;
    bool bStartFlag = true;

    if (pszXml != NULL) {
        SDL_Log("%s: Start, input len: %u",__FUNCTION__,(uint32_t)strlen((char*)pszXml));
        // XML zunächst grob prüfen
        puLevelgroupMd5TagStart = (uint8_t*)strstr((char*)pszXml,"<levelgroup_md5_hash>");
        puLastTag = (uint8_t*)strstr((char*)pszXml,"</levelgroup>");     // "Höchster" Pointer
        if ((strstr((char*)pszXml,"<levelgroup>") != NULL) && (strstr((char*)pszXml,"<groupname>") != NULL) && (strstr((char*)pszXml,"<create_timestamp>") != NULL) && puLevelgroupMd5TagStart && puLastTag) {
            pSearch = GetStartOrEndLevelTag(pszXml,puLastTag,bStartFlag);
            do {
                if (pSearch != NULL) {
                    sprintf(szLevelNum,"%03u",uLevelNum);
                    if (bStartFlag) {
                        memcpy(pSearch + 6,szLevelNum,3);
                    } else {
                        memcpy(pSearch + 7,szLevelNum,3);
                        uLevelNum++;
                    }
                    nCount++;
                    pSearch = pSearch + 9;
                    if (pSearch < puLastTag) {
                        bStartFlag = !bStartFlag;
                        pSearch = GetStartOrEndLevelTag(pSearch,puLastTag,bStartFlag);
                    }
                }
            } while (pSearch != NULL);
            // SDL_Log("Count: %d, Levels: %u",nCount,uLevelNum);
            if (uLevelNum > 0) {
                if (UpdateCreateTimestamp(pszXml) == 0) {
                    if (UpdateLevelgroupHash(pszXml,NULL) == 0) {
                        nRet = (int)uLevelNum;
                    }
                }
            }
        } else {
            SDL_Log("%s: bad pointer found, p1: %p, p2: %p, p3: %p, puLevelgroupMd5TagStart: %p, puLastTag: %p",
                    __FUNCTION__,strstr((char*)pszXml,"<levelgroup>"),strstr((char*)pszXml,"<groupname>"),strstr((char*)pszXml,"<create_timestamp>"),puLevelgroupMd5TagStart,puLastTag);
        }
    }
    return nRet;
}


/*----------------------------------------------------------------------------
Name:           GetLevelnameBeamPosition
------------------------------------------------------------------------------
Beschreibung: Ermittelt, ob und wo ein Balken eingeblendet werden muss, wenn der Mauspfeil sich über
              der Liste der Level-Namen befindet.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  uint32_t, 0xFFFFFFFF = keine Einblendung, sonst 0 bis MAX_LEVELTITLES_IN_LIST - 1 für einen der MAX_LEVELTITLES_IN_LIST Level-Namen
Seiteneffekte: InputStates.x, MainMenu.x
------------------------------------------------------------------------------*/
uint32_t GetLevelnameBeamPosition(void) {
    uint32_t uBeamPosition = 0xFFFFFFFF;
    uint32_t I;
    bool bMouseFound = false;

    // Prüfen, ob Mauspfeil über Levelnamenliste steht und ggf. Balken einblenden
    if ((InputStates.nMouseXpos_Relative >= FONT_W) && (InputStates.nMouseXpos_Relative < 512 )) {
        for (I = 0; (I < MAX_LEVELTITLES_IN_LIST) && (!bMouseFound); I++) {
            if ((InputStates.nMouseYpos_Relative >= (110 + (I * 20))) && (InputStates.nMouseYpos_Relative < (130 + (I * 20)))) {
                bMouseFound = true;
                if (MainMenu.uLevelTitleList[I] != 0xFFFF) {
                    uBeamPosition = I;
                }
            }
        }
    }
    return uBeamPosition;
}


/*----------------------------------------------------------------------------
Name:           GetImportFilesBeamPosition
------------------------------------------------------------------------------
Beschreibung: Ermittelt, ob und wo ein Balken eingeblendet werden muss, wenn der Mauspfeil sich über
              der Liste der Import-Dateiliste befindet.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  uint32_t, 0xFFFFFFFF = keine Einblendung,
               sonst 0 bis EMERALD_MAX_MAXIMPORTFILES_IN_LIST - 1 für einen der EMERALD_MAX_MAXIMPORTFILES_IN_LIST Import-Dateinamen
Seiteneffekte: InputStates.x, MainMenu.x
------------------------------------------------------------------------------*/
uint32_t GetImportFilesBeamPosition(void) {
    uint32_t uBeamPosition = 0xFFFFFFFF;
    uint32_t I;
    bool bMouseFound = false;

    // Prüfen, ob Mauspfeil über Levelnamenliste steht und ggf. Balken einblenden
    if ((InputStates.nMouseXpos_Relative >= FONT_W) && (InputStates.nMouseXpos_Relative < (DEFAULT_WINDOW_W - FONT_W) )) {
        for (I = 0; (I < EMERALD_MAX_MAXIMPORTFILES_IN_LIST) && (!bMouseFound); I++) {
            if ((InputStates.nMouseYpos_Relative >= (35 + (I * 20))) && (InputStates.nMouseYpos_Relative < (55 + (I * 20)))) {
                bMouseFound = true;
                if (MainMenu.uImportFileListDc3[I] != 0xFFFF) {
                    uBeamPosition = I;
                }
            }
        }
    }
    return uBeamPosition;
}


/*----------------------------------------------------------------------------
Name:           MenuSelectLevelname
------------------------------------------------------------------------------
Beschreibung: Erledigt die Auswahl eines Levels aus dem Pre-Editor-Menü und
              blendet die Balken für die Auswahl eines Levels ein, wenn sich
              der Mauspfeil über den Namen einer Gruppe befindet.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: puBeamPosition, uint32_t *, Zeiger auf Beam-Position
Rückgabewert:  int, 0 = kein Fehler, sonst Fehler
Seiteneffekte: MainMenu.x, InputStates.x
------------------------------------------------------------------------------*/
int MenuSelectLevelname(SDL_Renderer *pRenderer, uint32_t *puBeamPosition) {
    int nErrorCode = -1;
    uint32_t uBeamPosition;

    if (puBeamPosition != NULL) {
        nErrorCode = 0;
        *puBeamPosition = 0xFFFFFFFF;
        uBeamPosition = GetLevelnameBeamPosition();
        if (uBeamPosition != 0xFFFFFFFF) {
            nErrorCode = DrawBeam(pRenderer,FONT_W,107 + (FONT_LITTLE_347_H + 6) * uBeamPosition, FONT_W * 15, FONT_LITTLE_347_H + 6, 0x20,0x20,0xFF,0x60,K_RELATIVE);
            if ((InputStates.bLeftMouseButton) && (nErrorCode == 0)) {
                *puBeamPosition = uBeamPosition;
            }
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:          ImportMenuSelectFile
------------------------------------------------------------------------------
Beschreibung: Erledigt die Auswahl einer Datei aus dem Import-Level-Menü
              und blendet die Balken für die Auswahl einer Datei ein, wenn sich
              der Mauspfeil über den Namen einer Datei befindet.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: puBeamPosition, uint32_t *, Zeiger auf Beam-Position
Rückgabewert:  int, 0 = kein Fehler, sonst Fehler
Seiteneffekte: MainMenu.x, InputStates.x
------------------------------------------------------------------------------*/
int ImportMenuSelectFile(SDL_Renderer *pRenderer, uint32_t *puBeamPosition) {
    int nErrorCode = -1;
    uint32_t uBeamPosition;

    if (puBeamPosition != NULL) {
        nErrorCode = 0;
        *puBeamPosition = 0xFFFFFFFF;
        uBeamPosition = GetImportFilesBeamPosition();
        if (uBeamPosition != 0xFFFFFFFF) {
            nErrorCode = DrawBeam(pRenderer,FONT_W,35 + (FONT_LITTLE_347_H + 6) * uBeamPosition, FONT_W * 38, FONT_LITTLE_347_H + 6, 0x20,0x20,0xFF,0x60,K_RELATIVE);
            if ((InputStates.bLeftMouseButton) && (nErrorCode == 0)) {
                *puBeamPosition = uBeamPosition;
            }
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           InitLevelTitleList
------------------------------------------------------------------------------
Beschreibung: Initialisiert die Anzeigeliste für die Level-Titel.
              Vor Aufruf dieser Funktion muss erfolgreich eine Levelgruppe
              ausgewählt worden sein.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: MainMenu.x,  SelectedLevelgroup.x
------------------------------------------------------------------------------*/
void InitLevelTitleList(void) {
    uint32_t I;

    // Namensliste vorbereiten
    memset(MainMenu.uLevelTitleList,0xFF,sizeof(MainMenu.uLevelTitleList));
    if (SelectedLevelgroup.bOK) {
        for (I = 0; (I < MAX_LEVELTITLES_IN_LIST) && (I < SelectedLevelgroup.uLevelCount); I++) {
            MainMenu.uLevelTitleList[I] = I;
        }
    }
}


/*----------------------------------------------------------------------------
Name:           HandlePreEditorButtons
------------------------------------------------------------------------------
Beschreibung: Zeigt die Buttons im Pre-Editor-Menü an und wertet diese aus.
Parameter
      Eingang: nSelectedLevel, int, ausgewähltes Level, -1 = kein Level ausgewählt
      Ausgang: -
Rückgabewert:  int, gedrückter Button
                    0 = kein Button
                    1 = Create Levelgroup
                    2 = Rename Levelgroup
                    3 = Clear/Set Levelgroup Password
                    4 = Test Level
                    5 = Edit Level
                    6 = Move Level
                    7 = Copy Level
                    8 = Delete Level
                    9 = Markiertes Level in Clipboard
                   10 = Import DC3-Level
                   11 = Levelnamensliste Pfeil hoch
                   12 = Levelnamensliste Pfeil runter
Seiteneffekte: InputStates.x, SelectedLevelgroup, MainMenu.x, ImportLevel.x,
               ge_uXoffs, ge_uYoffs
------------------------------------------------------------------------------*/
int HandlePreEditorButtons(int nSelectedLevel) {
    uint32_t B;
    int nButton = 0;
    char szText[64];
    bool bButtons[11];   // Es sind 10 Buttons, Index 0 wird nicht verwendet:   Buttons sichtbar (true) / unsichtbar (false)
    bool bButtonFound;

    memset(bButtons,false,sizeof(bButtons));
    // Die folgenden Buttons werden unabhängig eines ausgewählten levels angezeigt
    MainMenu.uMenuScreen[4 * MainMenu.uXdim + 18] = EMERALD_STEEL_ADD_LEVELGROUP;
    SetMenuText(MainMenu.uMenuScreen,"NEW LEVELGROUP",20,4,EMERALD_FONT_BLUE);
    bButtons[1] = true;
    MainMenu.uMenuScreen[6 * MainMenu.uXdim + 18] = EMERALD_STEEL_RENAME_LEVELGROUP;
    SetMenuText(MainMenu.uMenuScreen,"RENAME LEVELGROUP",20,6,EMERALD_FONT_BLUE);
    bButtons[2] = true;
    MainMenu.uMenuScreen[8 * MainMenu.uXdim + 18] = EMERALD_STEEL_PASSWORD;
    if (strlen(SelectedLevelgroup.szPasswordHash) > 0) {
        SetMenuText(MainMenu.uMenuScreen,"CLEAR LEVELGROUP PW",20,8,EMERALD_FONT_BLUE);
    } else {
        SetMenuText(MainMenu.uMenuScreen,"SET LEVELGROUP PW",20,8,EMERALD_FONT_BLUE);
    }
    bButtons[3] = true;
    // Die restlichen Buttons zunächst unsichtbar schalten
    MainMenu.uMenuScreen[10 * MainMenu.uXdim + 18] = EMERALD_SPACE;  // Joystick
    SetMenuText(MainMenu.uMenuScreen,"                   ",20,10,EMERALD_FONT_BLUE);
    MainMenu.uMenuScreen[12 * MainMenu.uXdim + 18] = EMERALD_SPACE; // Edit
    SetMenuText(MainMenu.uMenuScreen,"                   ",20,12,EMERALD_FONT_BLUE);
    MainMenu.uMenuScreen[14 * MainMenu.uXdim + 18] = EMERALD_SPACE; // Move
    SetMenuText(MainMenu.uMenuScreen,"                   ",20,14,EMERALD_FONT_BLUE);
    MainMenu.uMenuScreen[16 * MainMenu.uXdim + 18] = EMERALD_SPACE; // Copy / new level
    SetMenuText(MainMenu.uMenuScreen,"                   ",20,16,EMERALD_FONT_BLUE);
    MainMenu.uMenuScreen[18 * MainMenu.uXdim + 18] = EMERALD_SPACE; // Delete
    SetMenuText(MainMenu.uMenuScreen,"                   ",20,18,EMERALD_FONT_BLUE);
    MainMenu.uMenuScreen[20 * MainMenu.uXdim + 18] = EMERALD_SPACE; // Clipboard
    SetMenuText(MainMenu.uMenuScreen,"                   ",20,20,EMERALD_FONT_BLUE);
    MainMenu.uMenuScreen[22 * MainMenu.uXdim + 18] = EMERALD_SPACE; // DC3-Level-Import
    SetMenuText(MainMenu.uMenuScreen,"                   ",20,22,EMERALD_FONT_BLUE);
    // Obere Leiste mit Levelgruppennamen und Levelnummer
    if (nSelectedLevel == -1) { // Ist ein Level ausgewählt?
        // Kein Level ausgewählt
        sprintf(szText,"%s/LEVEL:-",SelectedLevelgroup.szLevelgroupname);
    } else {
        // Level ausgewählt
        sprintf(szText,"%s/LEVEL:%03u",SelectedLevelgroup.szLevelgroupname,nSelectedLevel);
        MainMenu.uMenuScreen[10 * MainMenu.uXdim + 18] = EMERALD_STEEL_JOYSTICK;
        SetMenuText(MainMenu.uMenuScreen,"TEST LEVEL",20,10,EMERALD_FONT_BLUE);
        bButtons[4] = true;
        MainMenu.uMenuScreen[12 * MainMenu.uXdim + 18] = EMERALD_STEEL_EDIT_LEVEL;
        SetMenuText(MainMenu.uMenuScreen,"EDIT LEVEL",20,12,EMERALD_FONT_BLUE);
        bButtons[5] = true;
        MainMenu.uMenuScreen[16 * MainMenu.uXdim + 18] = EMERALD_STEEL_COPY_LEVEL;
        SetMenuText(MainMenu.uMenuScreen,"COPY / NEW LEVEL",20,16,EMERALD_FONT_BLUE);
        bButtons[7] = true;

        MainMenu.uMenuScreen[20 * MainMenu.uXdim + 18] = EMERALD_STEEL_CLIPBOARD_LEVEL;
        SetMenuText(MainMenu.uMenuScreen,"LEVEL TO CLIPBOARD",20,20,EMERALD_FONT_BLUE);
        bButtons[9] = true;


        if (SelectedLevelgroup.uLevelCount > 1) {
            MainMenu.uMenuScreen[14 * MainMenu.uXdim + 18] = EMERALD_STEEL_MOVE_LEVEL;
            SetMenuText(MainMenu.uMenuScreen,"MOVE LEVEL",20,14,EMERALD_FONT_BLUE);
            bButtons[6] = true;
            MainMenu.uMenuScreen[18 * MainMenu.uXdim + 18] = EMERALD_STEEL_TRASHCAN;
            SetMenuText(MainMenu.uMenuScreen,"DELETE LEVEL",20,18,EMERALD_FONT_BLUE);
            bButtons[8] = true;
        }
    }
    SetMenuText(MainMenu.uMenuScreen,"                                      ",1,1,EMERALD_FONT_BLUE);
    SetMenuText(MainMenu.uMenuScreen,szText,-1,1,EMERALD_FONT_BLUE);
    if (ImportLevel.uDc3FileCount > 0) {
        MainMenu.uMenuScreen[22 * MainMenu.uXdim + 18] = EMERALD_STEEL_DC3_IMPORT;
        SetMenuText(MainMenu.uMenuScreen,"IMPORT DC3-LEVEL",20,22,EMERALD_FONT_BLUE);
        bButtons[10] = true;
    }
    if (InputStates.bLeftMouseButton) {
        // Prüfen, ob Mauspfeil auf richtiger X-Position steht
        if ((InputStates.nMouseXpos_Relative >= 576) && (InputStates.nMouseXpos_Relative < (576 + FONT_W))) {
            bButtonFound = false;
            for (B = 1; (B <= 10) && (!bButtonFound); B++) {
                if (bButtons[B] && (InputStates.nMouseYpos_Relative >= (128 + (B - 1) * (2 * FONT_H))) && (InputStates.nMouseYpos_Relative < (128 + FONT_H + (B - 1) * (2 * FONT_H)))) {
                    bButtonFound = true;
                    nButton = (int)B;
                }
            }
        }
        // Scroll-Buttons für Levelnamensliste
        if ((!bButtonFound) && (SelectedLevelgroup.uLevelCount > MAX_LEVELTITLES_IN_LIST)) {
            if ((InputStates.nMouseXpos_Relative >= 512) && (InputStates.nMouseXpos_Relative < (512 + FONT_W))) {
                if ((InputStates.nMouseYpos_Relative >= 96) && (InputStates.nMouseYpos_Relative < (96 + FONT_H))) {
                    nButton = 11;
                } else if ((InputStates.nMouseYpos_Relative >= 160) && (InputStates.nMouseYpos_Relative < (160 + FONT_H))) {
                    nButton = 12;
                }
            }
        }
    }
    return nButton;
}


/*----------------------------------------------------------------------------
Name:           PreparePreEditormenu
------------------------------------------------------------------------------
Beschreibung: Zeichnet ein paar Mauern und Beschriftungen für das PreEditorMenu.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: MainMenu.x, SelectedLevelgroup.x
------------------------------------------------------------------------------*/
void PreparePreEditormenu(void) {
    uint32_t I;

    SetMenuBorderAndClear();
    SetMenuText(MainMenu.uMenuScreen,"NO.  LEVELNAME                        ",1,2,EMERALD_FONT_STEEL_BLUE);
    for (I = 0; I < 20; I++) {
        MainMenu.uMenuScreen[(3 + I) * MainMenu.uXdim + 4] = EMERALD_STEEL;
        MainMenu.uMenuScreen[(3 + I) * MainMenu.uXdim + 16] = EMERALD_STEEL;
    }
    if (SelectedLevelgroup.uLevelCount > MAX_LEVELTITLES_IN_LIST) {
        MainMenu.uMenuScreen[3 * MainMenu.uXdim + 16] = EMERALD_STEEL_ARROW_UP;
        MainMenu.uMenuScreen[5 * MainMenu.uXdim + 16] = EMERALD_STEEL_ARROW_DOWN;
    }
    MainMenu.uMenuScreen[23 * MainMenu.uXdim + 4] = EMERALD_STEEL_CLIPBOARD_LEVEL;
}


/*----------------------------------------------------------------------------
Name:           PreEditorMenu
------------------------------------------------------------------------------
Beschreibung: Vor-Menü für den Leveleditor. Hier können die vorhandenen Level
              der SelectedLevelgroup angezeigt und ausgewählt werden.

Parameter
      Eingang: SDL_Renderer *, pRenderer, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: InputStates.x, SelectedLevelgroup, MainMenu.x, Playfield.x
               ImportLevel.x, ManKey.x, Clipboard.x
------------------------------------------------------------------------------*/
int PreEditorMenu(SDL_Renderer *pRenderer) {
    uint32_t I;
    int nRet;
    int nSelectedLevel = -1;
    int nErrorCode = -1;
    int nColorDimm = 0;
    int nButton = 0;
    int nScrollLevelTitles;
    int nLastButton = 0;
    int nMoveState = 0;
    int nMoveSrcLevel = -1;
    int nMoveDestLevel = -1;
    uint32_t uModVolume;
    uint32_t uBeamPosition;
    int nSelectedBeamPosition = -1;
    bool bPrepareExit = false;
    bool bExit = false;
    char szText[64];
    uint32_t uScrollFastCounter;
    DYNSTRING *XML;
    bool bWarnDefaultLevelGroup;
    bool bColorToggle;
    uint8_t uFontColor;
    char szFullDefaultLevelgroupFilename[EMERALD_MAX_FILENAME_LEN * 2];

    bColorToggle = false;
    uScrollFastCounter = 0;
    InitLevelTitleList();
    uModVolume = 0;
    SetModVolume(uModVolume);
    if (SetModMusic(4) != 0) {
        return -1;
    }
    nRet = LevelgroupOperaton_AskPassword(pRenderer);
    if (nRet == -1) {   // Fehler?
        return -1;
    } else if (nRet == -2) {   // Passwort falsch?
        return 0;
    }
    if (CreateButton(BUTTONLABEL_EXIT_HIGHSCORES,"Back to main menu",1100,742,true,false) != 0) {
        return -1;
    }
    PreparePreEditormenu();
    sprintf(szFullDefaultLevelgroupFilename,"%s/%s",EMERALD_LEVELGROUPS_DIRECTORYNAME,EMERALD_DEFAULT_LEVELGROUP_FILENAME);
    bWarnDefaultLevelGroup = (strcmp(SelectedLevelgroup.szFilename,szFullDefaultLevelgroupFilename) == 0);
    do {
        UpdateManKey(); // ruft auch UpdateInputStates(); auf
        //printf("x:%u  y:%u   BeamPos: %d\n",InputStates.nMouseXpos,InputStates.nMouseYpos,nSelectedBeamPosition);
        // Hervorhebung für ausgewähltes Level
        if ((nSelectedBeamPosition >= 0) && (nSelectedBeamPosition < MAX_LEVELTITLES_IN_LIST)) {
            if (nMoveState == 0) {
                DrawBeam(pRenderer,FONT_W,107 + (FONT_LITTLE_347_H + 6) * nSelectedBeamPosition, FONT_W * 15, FONT_LITTLE_347_H + 6, 0xF0,0x20,0x20,0x60,K_RELATIVE);
            } else {
                if (((Playfield.uFrameCounter >> 4) & 0x01) == 0) {
                    DrawBeam(pRenderer,FONT_W,107 + (FONT_LITTLE_347_H + 6) * nSelectedBeamPosition, FONT_W * 15, FONT_LITTLE_347_H + 6, 0xF0,0xF0,0x20,0x60,K_RELATIVE);
                } else {
                    DrawBeam(pRenderer,FONT_W,107 + (FONT_LITTLE_347_H + 6) * nSelectedBeamPosition, FONT_W * 15, FONT_LITTLE_347_H + 6, 0x20,0xF0,0x20,0x60,K_RELATIVE);
                }
            }
        }
        nLastButton = nButton;
        nButton = HandlePreEditorButtons(nSelectedLevel);
        if ((nButton == 0) && (ManKey.bFire) && (nSelectedBeamPosition != -1)) {
            nButton = 4;    // Feuertaste kann ebenfalls Leveltest aufrufen
        }
        // Für das schnelle Scrollen der Level-Liste
        if ( ((nLastButton == 12) && (nButton == 12)) || ((nLastButton == 11) && (nButton == 11)) ) {
            uScrollFastCounter++;
        } else {
            uScrollFastCounter = 0;
        }
        if ( ((nLastButton == 0) || (uScrollFastCounter > 10)) && (nButton != 0)) {
            switch (nButton) {
                case (1):
                    nMoveState = 0;
                    nMoveSrcLevel = -1;
                    nMoveDestLevel = -1;
                    SDL_Log("%s:   create levelgroup",__FUNCTION__);
                    DimmMainMenu(pRenderer,false);
                    LevelgroupOperaton_NewGroup();
                    nColorDimm = 0;
                    uModVolume = 0;
                    nSelectedBeamPosition = -1;
                    nSelectedLevel = -1;
                    bWarnDefaultLevelGroup = false;
                    break;
                case (2):
                    nMoveState = 0;
                    nMoveSrcLevel = -1;
                    nMoveDestLevel = -1;
                    SDL_Log("%s:   rename levelgroup",__FUNCTION__);
                    DimmMainMenu(pRenderer,false);
                    LevelgroupOperaton_RenameGroupname(pRenderer);
                    PreparePreEditormenu();
                    nColorDimm = 0;
                    uModVolume = 0;
                    nSelectedBeamPosition = -1;
                    nSelectedLevel = -1;
                    break;
                case (3):
                    nMoveState = 0;
                    nMoveSrcLevel = -1;
                    nMoveDestLevel = -1;
                    SDL_Log("%s:   set/clr password",__FUNCTION__);
                    DimmMainMenu(pRenderer,false);
                    nErrorCode = LevelgroupOperaton_Password(pRenderer);
                    PreparePreEditormenu();
                    nColorDimm = 0;
                    uModVolume = 0;
                    nSelectedBeamPosition = -1;
                    nSelectedLevel = -1;
                    break;
                case (4):
                    nMoveState = 0;
                    nMoveSrcLevel = -1;
                    nMoveDestLevel = -1;
                    SDL_Log("%s:   test level",__FUNCTION__);
                    DimmMainMenu(pRenderer,false);
                    nErrorCode = RunGame(pRenderer,nSelectedLevel);
                    PreparePreEditormenu();
                    nColorDimm = 0;
                    uModVolume = 0;
                    break;
                case (5):
                    nMoveState = 0;
                    nMoveSrcLevel = -1;
                    nMoveDestLevel = -1;
                    SDL_Log("%s:   edit level",__FUNCTION__);
                    SetButtonActivity(BUTTONLABEL_EXIT_HIGHSCORES,false);
                    DimmMainMenu(pRenderer,false);
                    // Level-Listenpositrion retten, da diese in LevelgroupOperaton_Edit() initialisiert wird
                    memcpy(MainMenu.uLevelTitleListCopy,MainMenu.uLevelTitleList,sizeof(MainMenu.uLevelTitleList));
                    XML = Editor(pRenderer,nSelectedLevel);
                    if (XML != NULL) {  // Hat Editor XML-Leveldaten bereitgestellt?
                        LevelgroupOperaton_Edit(nSelectedLevel,XML);    // ruft InitLists() -> InitLevelTitleList() auf !
                        DynStringFree(XML);
                    }
                    SetButtonActivity(BUTTONLABEL_EXIT_HIGHSCORES,true);
                    PreparePreEditormenu();

                    // Beim "Edit" bleibt die Anzahl in der Leveliste konstant, d.h. sie kann wieder mit derselben Position angezeigt werden
                    memcpy(MainMenu.uLevelTitleList,MainMenu.uLevelTitleListCopy,sizeof(MainMenu.uLevelTitleList));
                    nColorDimm = 0;
                    uModVolume = 0;
                    break;
                case (6):
                    SDL_Log("%s:   move level",__FUNCTION__);
                    if (nMoveState == 0) {
                        nMoveState = 1;
                        nMoveSrcLevel = nSelectedLevel;
                    }
                    break;
                case (7):
                    nMoveState = 0;
                    nMoveSrcLevel = -1;
                    nMoveDestLevel = -1;
                    SDL_Log("%s:   copy level",__FUNCTION__);
                    SetButtonActivity(BUTTONLABEL_EXIT_HIGHSCORES,false);
                    DimmMainMenu(pRenderer,false);
                    if (Clipboard.pLevelXml != NULL) {
                        nErrorCode = LevelgroupOperaton_CopyClipboard(nSelectedLevel);
                    } else {
                        nErrorCode = LevelgroupOperaton_Copy(nSelectedLevel);
                    }
                    InitLevelTitleList();
                    SetButtonActivity(BUTTONLABEL_EXIT_HIGHSCORES,true);
                    PreparePreEditormenu();
                    nColorDimm = 0;
                    uModVolume = 0;
                    nSelectedBeamPosition = -1;
                    nSelectedLevel = -1;
                    break;
                case (8):
                    nMoveState = 0;
                    nMoveSrcLevel = -1;
                    nMoveDestLevel = -1;
                    SDL_Log("%s:   delete level",__FUNCTION__);
                    SetButtonActivity(BUTTONLABEL_EXIT_HIGHSCORES,false);
                    DimmMainMenu(pRenderer,false);
                    nErrorCode = LevelgroupOperaton_Delete(nSelectedLevel);
                    InitLevelTitleList();
                    SetButtonActivity(BUTTONLABEL_EXIT_HIGHSCORES,true);
                    PreparePreEditormenu();
                    nColorDimm = 0;
                    uModVolume = 0;
                    nSelectedBeamPosition = -1;
                    nSelectedLevel = -1;
                    break;
                case (9):
                    SDL_Log("%s:   level to clipboard",__FUNCTION__);
                    nErrorCode = PutLevelToClipboard(nSelectedLevel);
                    ShowClipboard();
                    nMoveState = 0;
                    nMoveSrcLevel = -1;
                    nMoveDestLevel = -1;
                    break;
                case (10):
                    nMoveState = 0;
                    nMoveSrcLevel = -1;
                    nMoveDestLevel = -1;
                    nSelectedBeamPosition = -1;
                    nSelectedLevel = -1;
                    DimmMainMenu(pRenderer,false);
                    SDL_Log("%s:   import diamond caves 3 level",__FUNCTION__);
                    nErrorCode = LevelgroupOperaton_ImportDC3(pRenderer);
                    InitLevelTitleList();
                    PreparePreEditormenu();
                    nColorDimm = 0;
                    uModVolume = 0;
                    break;
                case (11):
                    SDL_Log("%s:   level list up",__FUNCTION__);
                    nScrollLevelTitles = ScrollLevelTitleList(EMERALD_STEEL_ARROW_UP_PRESSED);
                    if ((nScrollLevelTitles != 0)) {
                        nSelectedBeamPosition++;
                    }
                    break;
                case (12):
                    SDL_Log("%s:   level list down",__FUNCTION__);
                    nScrollLevelTitles = ScrollLevelTitleList(EMERALD_STEEL_ARROW_DOWN_PRESSED);
                    if ((nScrollLevelTitles != 0)) {
                        nSelectedBeamPosition--;
                    }
                    break;
            }
        }

        // Levelnamen auflisten
        for (I = 0; I < MAX_LEVELTITLES_IN_LIST; I++) {
            if (MainMenu.uLevelTitleList[I] != 0xFFFF) {
                sprintf(szText,"%03u",MainMenu.uLevelTitleList[I]);
                PrintLittleFont(pRenderer,62,110 + I * 20,0,szText,K_RELATIVE);
                PrintLittleFont(pRenderer,176,110 + I * 20,0,SelectedLevelgroup.szLevelTitle[MainMenu.uLevelTitleList[I]],K_RELATIVE);
            }
        }
        nErrorCode = MenuSelectLevelname(pRenderer,&uBeamPosition);
        if (uBeamPosition != 0xFFFFFFFF) {
            nSelectedBeamPosition = uBeamPosition;
            nSelectedLevel = MainMenu.uLevelTitleList[nSelectedBeamPosition];
            // Erledigt das Verschieben eines Levels
            if (nMoveState == 1) {
                nMoveDestLevel = nSelectedLevel;
                SetButtonActivity(BUTTONLABEL_EXIT_HIGHSCORES,false);
                DimmMainMenu(pRenderer,false);
                nErrorCode = LevelgroupOperaton_Move(nMoveSrcLevel,nMoveDestLevel);
                InitLevelTitleList();
                SetButtonActivity(BUTTONLABEL_EXIT_HIGHSCORES,true);
                PreparePreEditormenu();
                nColorDimm = 0;
                uModVolume = 0;
                nSelectedBeamPosition = -1;
                nSelectedLevel = -1;
                nMoveState = 0;
                nMoveSrcLevel = -1;
                nMoveDestLevel = -1;
            }
        }

        if (InputStates.bRightMouseButton) {
            nSelectedBeamPosition = -1;
            nSelectedLevel = -1;
            nMoveState = 0;
            nMoveSrcLevel = -1;
            nMoveDestLevel = -1;
            FreeClipboard();
        }

        if ((!bPrepareExit) && (nColorDimm < 100)) {
            nColorDimm = nColorDimm + 4;
            SetAllTextureColors(nColorDimm);
            uModVolume = uModVolume + 4;
            SetModVolume(uModVolume);
        } else if (bPrepareExit) {
            if (nColorDimm > 0) {
                nColorDimm = nColorDimm - 4;
                SetAllTextureColors(nColorDimm);
                uModVolume = uModVolume -4;
                SetModVolume(uModVolume);
            } else {
                bExit = true;
            }
        }
        PlayMusic(false);
        if (nErrorCode == 0) {
            nErrorCode = RenderMenuElements(pRenderer);
        }

        if (bWarnDefaultLevelGroup) {
            if (bColorToggle) {
                uFontColor = 0;
            } else {
                uFontColor = 2;
            }
            if ((Playfield.uFrameCounter % 10) == 0) {
                bColorToggle = !bColorToggle;
            }
            PrintLittleFont(pRenderer,260,8,uFontColor,"WARNING: THE DEFAULT LEVELGROUP WILL BE OVERWRITTEN NEXT PROGRAM START !",K_RELATIVE);

        }
        if (Clipboard.pLevelXml != NULL) {
            strcpy(szText,Clipboard.szLevelTitle);
        } else {
            strcpy(szText,"CLIPBOARD IS EMPTY");
        }
        PrintLittleFont(pRenderer,176,745,0,szText,K_RELATIVE);
        PrintLittleFont(pRenderer,177,746,3,szText,K_RELATIVE);


        if ((IsButtonPressed(BUTTONLABEL_EXIT_HIGHSCORES)) ||  ((InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE]) || InputStates.bQuit)) {
            bPrepareExit = true;
        }
        ShowButtons(pRenderer);
        SDL_RenderPresent(pRenderer);   // Renderer anzeigen, lässt Hauptschleife mit ~ 60 Hz (Bild-Wiederholfrequenz) laufen
        SDL_RenderClear(pRenderer);     // Renderer für nächstes Frame löschen
        SDL_Delay(5);
        Playfield.uFrameCounter++;
    } while ((!bExit) && (nErrorCode == 0));

    FreeButton(BUTTONLABEL_EXIT_HIGHSCORES);
    WaitNoKey();
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           ScrollLevelTitleList
------------------------------------------------------------------------------
Beschreibung: Scrollt die Levelnamen im Pre-Editor-Menü.
Parameter
      Eingang: nButton, int, Levelnamenlisten-Button, der gedrückt wurde
                        0 = kein Button gedrückt, 1 = EMERALD_STEEL_ARROW_DOWN_PRESSED, 2 = EMERALD_STEEL_ARROW_UP_PRESSED
      Ausgang: -
Rückgabewert:  int, 0 = nicht gescrollt
                    1 = runter gescrollt
                    2 = hoch gescrollt
Seiteneffekte: SelectedLevelgroup, MainMenu.x
------------------------------------------------------------------------------*/
int ScrollLevelTitleList(int nButton) {
    uint32_t I;
    int nScroll = 0;

    if (SelectedLevelgroup.uLevelCount > MAX_LEVELTITLES_IN_LIST) {
        if (nButton == EMERALD_STEEL_ARROW_UP_PRESSED) {            // Button Levelnamenlisten Pfeil hoch?
            if (MainMenu.uLevelTitleList[0] > 0) {
                nScroll = EMERALD_STEEL_ARROW_UP_PRESSED;
                for (I = 0; I < MAX_LEVELTITLES_IN_LIST; I++) {
                    if (MainMenu.uLevelTitleList[I] != 0xFFFF) {
                        MainMenu.uLevelTitleList[I]--;
                    }
                }
            }
        } else if (nButton == EMERALD_STEEL_ARROW_DOWN_PRESSED) {   // Button Levelnamenlisten Pfeil runter?
            if (MainMenu.uLevelTitleList[MAX_LEVELTITLES_IN_LIST - 1] < (SelectedLevelgroup.uLevelCount - 1)) {
                nScroll = EMERALD_STEEL_ARROW_DOWN_PRESSED;
                for (I = 0; I < MAX_LEVELTITLES_IN_LIST; I++) {
                    if (MainMenu.uLevelTitleList[I] != 0xFFFF) {
                        MainMenu.uLevelTitleList[I]++;
                    }
                }
            }
        }
    }
    return nScroll;
}


/*----------------------------------------------------------------------------
Name:           InitClipboard
------------------------------------------------------------------------------
Beschreibung: Initialisiert die Struktur Clipboard.x.
              Wird einmalig aus EmeraldMineMainMenu() aufgerufen.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Clipboard.x
------------------------------------------------------------------------------*/
void InitClipboard(void) {
    Clipboard.pLevelXml = NULL;
    memset(Clipboard.szLevelTitle,0,sizeof(Clipboard.szLevelTitle));
}


/*----------------------------------------------------------------------------
Name:           PutLevelToClipboard
------------------------------------------------------------------------------
Beschreibung: Fügt ein Level aus der aktuell selektierten Lvelgruppe in die Struktur
              Clipboard.x ein. Falls sich bereits Daten im Clipboard befinden,
              wird der Clipboard-Speicher vorher freigegeben und neuer Speicher alloziert.
              Im Erfolgsfall muss der Clipboard-Speicher außerhalb dieser
              Funktion wieder freigegeben werden.
Parameter
      Eingang: nLevelNumber, int, Levelnummer in der selektierten Levelgruppe.
      Ausgang: -
Rückgabewert:  int, 0 = OK, sonst Fehler
Seiteneffekte: Clipboard.x, SelectedLevelgroup.x
------------------------------------------------------------------------------*/
int PutLevelToClipboard(int nLevelNumber) {
    int nErrorCode;
    uint32_t uXmlLen;
    uint32_t uTitleLen;
    uint32_t uLevelLen;
    ezxml_t xml;
    ezxml_t node;
    char *pAttr;
    uint8_t *pLevelgroupXml = NULL;
    uint8_t *pLevelXml = NULL;

    nErrorCode = -1;
    if ((nLevelNumber >= 0) && (nLevelNumber <EMERALD_MAX_LEVELCOUNT)) {
        if ((pLevelgroupXml = ReadFile(SelectedLevelgroup.szFilename,&uXmlLen)) != NULL) {     // Levelgruppen-Datei einlesen
            if ((pLevelXml = GetLevelFromLevelgroup(pLevelgroupXml,nLevelNumber)) != NULL) {
                // Ab hier stehen die XML-Leveldaten bereit
                SAFE_FREE(Clipboard.pLevelXml);
                memset(Clipboard.szLevelTitle,0,sizeof(Clipboard.szLevelTitle));
                uLevelLen = strlen((char*)pLevelXml);
                // Neuen Speicher erzeugen und Daten ins Clipboard kopieren
                Clipboard.pLevelXml = malloc(uLevelLen + 1);
                if (Clipboard.pLevelXml != NULL) {
                    Clipboard.pLevelXml[uLevelLen] = 0; // Stellt Stringende sicher
                    memcpy(Clipboard.pLevelXml,pLevelXml,uLevelLen);
                    nErrorCode = 0;
                    // Leveltitel ermitteln
                    xml = ezxml_parse_str((char*)pLevelXml,uLevelLen);
                    if (xml != NULL) {
                        node = ezxml_child(xml,"title");
                        if (node != NULL) {
                            pAttr = node->txt;
                            uTitleLen = (uint32_t)strlen(pAttr);
                            if (uTitleLen == 0) {
                                strcpy(Clipboard.szLevelTitle,"UNKNOWN TITLE");
                            } else if (uTitleLen > EMERALD_TITLE_LEN) {
                                memcpy(Clipboard.szLevelTitle,pAttr,EMERALD_TITLE_LEN);
                            } else {
                                strcpy(Clipboard.szLevelTitle,pAttr);
                            }
                        }
                        ezxml_free(xml);    // Prüft selbst, ob Pointer NULL ist
                    }
                }
            }
        }
        SAFE_FREE(pLevelXml);
        SAFE_FREE(pLevelgroupXml);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           FreeClipboard
------------------------------------------------------------------------------
Beschreibung: Gibt ggf. den Speicher des Clipboards wieder frei.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Clipboard.x
------------------------------------------------------------------------------*/
void FreeClipboard(void) {
    SAFE_FREE(Clipboard.pLevelXml);
    memset(Clipboard.szLevelTitle,0,sizeof(Clipboard.szLevelTitle));
}


/*----------------------------------------------------------------------------
Name:           ShowClipboard
------------------------------------------------------------------------------
Beschreibung: Zeigt den Inhalt des Clipboards. (Debugfunktion)
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Clipboard.x
------------------------------------------------------------------------------*/
void ShowClipboard(void) {
    uint32_t uLen;

    if (Clipboard.pLevelXml != NULL) {
        uLen = strlen((char*)Clipboard.pLevelXml);
        SDL_Log("Leveltitle:%s",Clipboard.szLevelTitle);
        SDL_Log("LevelLen:  %u",uLen);
        if (uLen > 128) {
            uLen = 128;
        }
        DumpMem(Clipboard.pLevelXml,uLen);
    } else {
        SDL_Log("Clipboard is empty!");
    }
}
