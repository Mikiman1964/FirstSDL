#include <SDL2/SDL.h>
#include <stdio.h>
#include "EmeraldMine.h"
#include "editor.h"
#include "KeyboardMouse.h"
#include "mySDL.h"
#include "scroller.h"

extern INPUTSTATES InputStates;
extern PLAYFIELD Playfield;

uint8_t g_PanelColorPatterns[] = {
//                           R    G    B
                            0x42,0x21,0x00,         // braun 1, dunkel
                            0x6C,0x36,0x00,         // braun 2
                            0x9D,0x4F,0x00,         // braun 3
                            0xDF,0x70,0x00,         // braun 4, hell
                            };

char szElementNames[256][64] =
                         {"no element",                         // 0x00
                          "Stone",                              // 0x01
                          "Sapphire",                           // 0x02
                          "free element",                       // 0x03
                          "Alien",                              // 0x04
                          "Free element",                       // 0x05
                          "Free element",                       // 0x06
                          "Free element",                       // 0x07
                          "Mine up",                            // 0x08
                          "Mine right",                         // 0x09
                          "Mine down",                          // 0x0A
                          "Mine left",                          // 0x0B
                          "Free element",                       // 0x0C
                          "Free element",                       // 0x0D
                          "Free element",                       // 0x0E
                          "Free element",                       // 0x0F
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
                          "Replicator red, buttom left",        // 0x1B
                          "Replicator red, button right",       // 0x1C
                          "Replicator red, switch",             // 0x1D
                          "Replicator yellow, top left",        // 0x1E
                          "Replicator yellow, top middle",      // 0x1F
                          "Replicator yellow, top right",       // 0x20
                          "Replicator yellow, buttom left",     // 0x21
                          "Replicator yellow, button right",    // 0x22
                          "Replicator yellow, switch",          // 0x23
                          "Time coin",                          // 0x24
                          "Nut",                                // 0x25
                          "Free element",                       // 0x26
                          "Free element",                       // 0x27
                          "Free element",                       // 0x28
                          "Yam",                                // 0x29
                          "Free element",                       // 0x2A
                          "Free element",                       // 0x2B
                          "Free element",                       // 0x2C
                          "Free element",                       // 0x2D
                          "Replicator green, top left",         // 0x2E
                          "Replicator green, top middle",       // 0x2F
                          "Replicator green, top right",        // 0x30
                          "Replicator green, buttom left",      // 0x31
                          "Replicator green, button right",     // 0x32
                          "Replicator green, switch",           // 0x33
                          "Replicator blue, top left",          // 0x34
                          "Replicator blue, top middle",        // 0x35
                          "Replicator blue, top right",         // 0x36
                          "Replicator blue, buttom left",       // 0x37
                          "Replicator blue, button right",      // 0x38
                          "Replicator blue, switch",            // 0x39
                          "Dynamite, off",                      // 0x3A
                          "Free element",                       // 0x3B
                          "Free element",                       // 0x3C
                          "Free element",                       // 0x3D
                          "Free element",                       // 0x3E
                          "Free element",                       // 0x3F
                          "Man",                                // 0x40
                          "Free element",                       // 0x41
                          "Free element",                       // 0x42
                          "Free element",                       // 0x43
                          "Free element",                       // 0x44
                          "Free element",                       // 0x45
                          "Free element",                       // 0x46
                          "Free element",                       // 0x47
                          "Free element",                       // 0x48
                          "Free element",                       // 0x49
                          "Free element",                       // 0x4A
                          "Free element",                       // 0x4B
                          "Free element",                       // 0x4C
                          "Free element",                       // 0x4D
                          "Free element",                       // 0x4E
                          "Free element",                       // 0x4F
                          "Free element",                       // 0x50
                          "Free element",                       // 0x51
                          "Free element",                       // 0x52
                          "Free element",                       // 0x53
                          "Free element",                       // 0x54
                          "Free element",                       // 0x55
                          "Free element",                       // 0x56
                          "Free element",                       // 0x57
                          "Free element",                       // 0x58
                          "Free element",                       // 0x59
                          "Free element",                       // 0x5A
                          "Free element",                       // 0x5B
                          "Free element",                       // 0x5C
                          "Free element",                       // 0x5D
                          "Free element",                       // 0x5E
                          "Free element",                       // 0x5F
                          "Free element",                       // 0x60
                          "Free element",                       // 0x61
                          "Free element",                       // 0x62
                          "Free element",                       // 0x63
                          "Free element",                       // 0x64
                          "Free element",                       // 0x65
                          "Free element",                       // 0x66
                          "Free element",                       // 0x67
                          "Free element",                       // 0x68
                          "Free element",                       // 0x69
                          "Free element",                       // 0x6A
                          "Free element",                       // 0x6B
                          "Free element",                       // 0x6C
                          "Free element",                       // 0x6D
                          "Free element",                       // 0x6E
                          "Free element",                       // 0x6F
                          "Free element",                       // 0x70
                          "Free element",                       // 0x71
                          "Free element",                       // 0x72
                          "Free element",                       // 0x73
                          "Free element",                       // 0x74
                          "Free element",                       // 0x75
                          "Free element",                       // 0x76
                          "Free element",                       // 0x77
                          "Free element",                       // 0x78
                          "Acid pool destroy, internal element",// 0x79
                          "Acid pool, top left",                // 0x7A
                          "Acid pool, top middle",              // 0x7B
                          "Acid pool, top right",               // 0x7C
                          "Acid pool, buttom left",             // 0x7D
                          "Acid pool, buttom middle",           // 0x7E
                          "Acid pool, buttom right",            // 0x7F
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
                          "Green drop",                         // 0x99
                          "Green cheese",                       // 0x9A
                          "Explosion P1, internal element",     // 0x9B
                          "Explosion P2, internal element",     // 0x9C
                          "Wall, soft, round, with pike",       // 0x9D
                          "Enddoor, ready",                     // 0x9E
                          "Steel, round, with pike",            // 0x9F
                          "Steel, round",                       // 0xA0
                          "Steel, Warning",                     // 0xA1
                          "Steel, Biohazard",                   // 0xA2
                          "Steel, Deadend",                     // 0xA3
                          "Steel, Stop",                        // 0xA4
                          "Steel, Parking",                     // 0xA5
                          "Steel, Forbidden",                   // 0xA6
                          "Steel, Exit",                        // 0xA7
                          "Steel, Radioactive",                 // 0xA8
                          "Steel, Explosion",                   // 0xA9
                          "Steel, Acid",                        // 0xAA
                          "Steel, not round",                   // 0xAB
                          "Wall, soft, not round",              // 0xAC
                          "Free element",                       // 0xAD
                          "Free element",                       // 0xAE
                          "Free element",                       // 0xAF
                          "Free element",                       // 0xB0
                          "Free element",                       // 0xB1
                          "Free element",                       // 0xB2
                          "Free element",                       // 0xB3
                          "Free element",                       // 0xB4
                          "Free element",                       // 0xB5
                          "Free element",                       // 0xB6
                          "Free element",                       // 0xB7
                          "Free element",                       // 0xB8
                          "Free element",                       // 0xB9
                          "Free element",                       // 0xBA
                          "Free element",                       // 0xBB
                          "Free element",                       // 0xBC
                          "Free element",                       // 0xBD
                          "Free element",                       // 0xBE
                          "Free element",                       // 0xBF
                          "Free element",                       // 0xC0
                          "Free element",                       // 0xC1
                          "Free element",                       // 0xC2
                          "Free element",                       // 0xC3
                          "Free element",                       // 0xC4
                          "Free element",                       // 0xC5
                          "Free element",                       // 0xC6
                          "Free element",                       // 0xC7
                          "Free element",                       // 0xC8
                          "Free element",                       // 0xC9
                          "Free element",                       // 0xCA
                          "Free element",                       // 0xCB
                          "Free element",                       // 0xCC
                          "Free element",                       // 0xCD
                          "Free element",                       // 0xCE
                          "Free element",                       // 0xCF
                          "Free element",                       // 0xD0
                          "Free element",                       // 0xD1
                          "Free element",                       // 0xD2
                          "Free element",                       // 0xD3
                          "Free element",                       // 0xD4
                          "Free element",                       // 0xD5
                          "Free element",                       // 0xD6
                          "Free element",                       // 0xD7
                          "Free element",                       // 0xD8
                          "Free element",                       // 0xD9
                          "Free element",                       // 0xDA
                          "Free element",                       // 0xDB
                          "Free element",                       // 0xDC
                          "Free element",                       // 0xDD
                          "Free element",                       // 0xDE
                          "Free element",                       // 0xDF
                          "Free element",                       // 0xE0
                          "Free element",                       // 0xE1
                          "Free element",                       // 0xE2
                          "Free element",                       // 0xE3
                          "Free element",                       // 0xE4
                          "Free element",                       // 0xE5
                          "Free element",                       // 0xE6
                          "Free element",                       // 0xE7
                          "Free element",                       // 0xE8
                          "Free element",                       // 0xE9
                          "Free element",                       // 0xEA
                          "Free element",                       // 0xEB
                          "Free element",                       // 0xEC
                          "Free element",                       // 0xED
                          "Free element",                       // 0xEE
                          "Free element",                       // 0xEF
                          "Free element",                       // 0xF0
                          "Free element",                       // 0xF1
                          "Free element",                       // 0xF2
                          "Free element",                       // 0xF3
                          "Free element",                       // 0xF4
                          "Free element",                       // 0xF5
                          "Free element",                       // 0xF6
                          "Free element",                       // 0xF7
                          "Free element",                       // 0xF8
                          "Free element",                       // 0xF9
                          "Free element",                       // 0xFA
                          "Free element",                       // 0xFB
                          "Free element",                       // 0xFC
                          "Free element",                       // 0xFD
                          "Free element",                       // 0xFE
                          "Invalid element"                     // 0xFF
                         };


uint8_t g_PanelElements[] = {
                            EMERALD_DOOR_RED,EMERALD_DOOR_YELLOW,EMERALD_DOOR_GREEN,EMERALD_DOOR_BLUE,
                            EMERALD_KEY_RED,EMERALD_KEY_YELLOW,EMERALD_KEY_GREEN,EMERALD_KEY_BLUE,
                            EMERALD_DOOR_GREY_RED,EMERALD_DOOR_GREY_YELLOW,EMERALD_DOOR_GREY_GREEN,EMERALD_DOOR_GREY_BLUE,
                            EMERALD_MINE_LEFT,EMERALD_MINE_DOWN,EMERALD_MINE_RIGHT,EMERALD_MINE_UP,
                            EMERALD_BEETLE_LEFT,EMERALD_BEETLE_DOWN,EMERALD_BEETLE_RIGHT,EMERALD_BEETLE_UP,
                            EMERALD_REPLICATOR_RED_TOP_LEFT,EMERALD_REPLICATOR_RED_TOP_MID,EMERALD_REPLICATOR_RED_TOP_RIGHT,EMERALD_REPLICATOR_RED_SWITCH,
                            EMERALD_REPLICATOR_RED_BUTTOM_LEFT,EMERALD_SPACE,EMERALD_REPLICATOR_RED_BUTTOM_RIGHT,EMERALD_WHEEL,
                            EMERALD_REPLICATOR_YELLOW_TOP_LEFT,EMERALD_REPLICATOR_YELLOW_TOP_MID,EMERALD_REPLICATOR_YELLOW_TOP_RIGHT,EMERALD_REPLICATOR_YELLOW_SWITCH,
                            EMERALD_REPLICATOR_YELLOW_BUTTOM_LEFT,EMERALD_SPACE,EMERALD_REPLICATOR_YELLOW_BUTTOM_RIGHT,EMERALD_ALIEN,
                            EMERALD_REPLICATOR_GREEN_TOP_LEFT,EMERALD_REPLICATOR_GREEN_TOP_MID,EMERALD_REPLICATOR_GREEN_TOP_RIGHT,EMERALD_REPLICATOR_GREEN_SWITCH,
                            EMERALD_REPLICATOR_GREEN_BUTTOM_LEFT,EMERALD_SPACE,EMERALD_REPLICATOR_GREEN_BUTTOM_RIGHT,EMERALD_MAN,
                            EMERALD_REPLICATOR_BLUE_TOP_LEFT,EMERALD_REPLICATOR_BLUE_TOP_MID,EMERALD_REPLICATOR_BLUE_TOP_RIGHT,EMERALD_REPLICATOR_BLUE_SWITCH,
                            EMERALD_REPLICATOR_BLUE_BUTTOM_LEFT,EMERALD_SPACE,EMERALD_REPLICATOR_BLUE_BUTTOM_RIGHT,EMERALD_SPACE,
                            EMERALD_ACIDPOOL_TOP_LEFT,EMERALD_ACIDPOOL_TOP_MID,EMERALD_ACIDPOOL_TOP_RIGHT,EMERALD_SAND,
                            EMERALD_ACIDPOOL_BUTTOM_LEFT,EMERALD_ACIDPOOL_BUTTOM_MID,EMERALD_ACIDPOOL_BUTTOM_RIGHT,EMERALD_SWAMP,
                            EMERALD_STONE,EMERALD_NUT,EMERALD_BOMB,EMERALD_SPACE,
                            EMERALD_EMERALD,EMERALD_RUBY,EMERALD_SAPPHIRE,EMERALD_CRYSTAL,
                            EMERALD_MAGIC_WALL, EMERALD_WALL_SOFT_CORNERED,EMERALD_WALL_SOFT_ROUND,EMERALD_WALL_SOFT_ROUND_PIKE,
                            EMERALD_HAMMER,EMERALD_TIME_COIN,EMERALD_DOOR_END_NOT_READY,EMERALD_DOOR_END_READY,
                            EMERALD_DYNAMITE_OFF,EMERALD_SPACE,EMERALD_GREEN_CHEESE,EMERALD_GREEN_DROP,
                            EMERALD_WALL_HARD,EMERALD_STEEL_ROUND_PIKE,EMERALD_STEEL_ROUND,EMERALD_STEEL_WARNING,
                            EMERALD_STEEL_BIOHAZARD,EMERALD_STEEL_DEADEND,EMERALD_STEEL_STOP,EMERALD_STEEL_PARKING,
                            EMERALD_STEEL_FORBIDDEN,EMERALD_STEEL_EXIT,EMERALD_STEEL_RADIOACTIVE,EMERALD_STEEL_EXPLOSION,
                            EMERALD_STEEL_ACID,EMERALD_STEEL_NOT_ROUND,EMERALD_WALL_SOFT_NOT_ROUND,EMERALD_SPACE,
                            EMERALD_STEEL_MARKER_LEFT_UP,EMERALD_STEEL_MARKER_UP,EMERALD_STEEL_MARKER_RIGHT_UP,EMERALD_SPACE,
                            EMERALD_STEEL_MARKER_LEFT,EMERALD_SPACE,EMERALD_STEEL_MARKER_RIGHT,EMERALD_SPACE,
                            //EMERALD_STEEL_MARKER_LEFT_BUTTOM,EMERALD_STEEL_MARKER_BUTTOM,EMERALD_STEEL_MARKER_RIGHT_BUTTOM,EMERALD_SPACE,
                            EMERALD_INVALID
                            };


/*----------------------------------------------------------------------------
Name:           GetElementByMouseposition
------------------------------------------------------------------------------
Beschreibung: Ermittelt anhand der Mausposition das Level-Element, auf das der
              Mauspfeil zeigt.
Parameter
      Eingang: nMouseXpos, int, X-Position des Mauspfeils
               nMouseYpos, int, Y-Position des Mauspfeils
      Ausgang: -
Rückgabewert:  uint32_t, Levelelement, wenn keines ermittelt werden kann,
                         wird EMERALD_INVALID zurück gegeben.
Seiteneffekte: g_PanelElements[]
------------------------------------------------------------------------------*/
uint32_t GetElementByMouseposition(int nMouseXpos, int nMouseYpos) {
    uint32_t uElement;
    int uX;
    int uY;

    uElement = EMERALD_INVALID;
    if ((nMouseXpos >= 734) && (nMouseXpos <= 830)) {
        uX = (nMouseXpos - 734) / 24;
    } else {
        uX = -1;
    }
    if ((nMouseYpos >= 10) && (nMouseYpos <= 578)) {
        uY = (nMouseYpos - 10) / 24;
    } else {
        uY = -1;
    }
    if ((uX != -1) && (uY != -1)) {
        uElement = g_PanelElements[(uY * 4) + uX];
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
Seiteneffekte: g_PanelElements[]
------------------------------------------------------------------------------*/
int FillPanel(SDL_Renderer *pRenderer) {
    uint32_t uI;
    uint32_t uElements;
    uint32_t uX;
    uint32_t uY;
    uint32_t uTextureIndex;
    int nErrorCode;
    SDL_Rect DestR;                     // Zum Kopieren in den Renderer
    float fAngle;

    // Bei der aktuellen Fenster (832 * 640)- und Panelgröße sind 26 * 4 = 104 Elemente möglich

    uElements = (uint32_t)sizeof(g_PanelElements) - 1;
    if (uElements > MAX_PANEL_ELEMENTS) {
        SDL_Log("%s: to many panel elements: %u",__FUNCTION__,uElements);
        return -1;
    }
    nErrorCode = 0;
    for (uI = 0; uI < uElements; uI++) {
        uX = uI % 4;
        uY = uI / 4;
        // Position innerhalb des Renderers
        DestR.x = EDITORPANEL_X + 12 + uX * 24;
        DestR.y = EDITORPANEL_Y + 10 + uY * 24;
        DestR.w = FONT_W / 2;
        DestR.h = FONT_H / 2;
        uTextureIndex = GetTextureIndexByElement(g_PanelElements[uI],Playfield.uFrameCounter % 16,&fAngle);
        if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&DestR,fAngle,NULL,SDL_FLIP_NONE) != 0) {
            SDL_Log("%s: SDL_RenderCopyEx(standard) failed: %s",__FUNCTION__,SDL_GetError());
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
    int nErrorCode = 0;
    SDL_Rect Rect;

    for (I = 0; (I < 4) && (nErrorCode == 0); I++) {
        if (SDL_SetRenderDrawColor(pRenderer,g_PanelColorPatterns[I * 3 + 0],g_PanelColorPatterns[I * 3 + 1],g_PanelColorPatterns[I * 3 + 2], SDL_ALPHA_OPAQUE) == 0) {  // Farbe für Rechteck setzen
            Rect.x = EDITORPANEL_X + 1 + 4 - I;
            Rect.y = EDITORPANEL_Y + 3 - I;
            Rect.w = EDITORPANEL_W - 8 + I * 2;    // -8, da 2x Wandstärke
            Rect.h = EDITORPANEL_H - 6 + I * 2;    // -8, da 2x Wandstärke
            nErrorCode = SDL_RenderDrawRect(pRenderer,&Rect);
            if (nErrorCode != 0) {
                SDL_Log("%s: SDL_RenderDrawRect() failed: %s",__FUNCTION__,SDL_GetError());
            }
        } else {
            SDL_Log("%s: SDL_SetRenderDrawColor() failed: %s",__FUNCTION__,SDL_GetError());
            nErrorCode = -1;
        }
    }
    if (nErrorCode == 0) {
        if (SDL_SetRenderDrawColor(pRenderer,0,0,0, SDL_ALPHA_OPAQUE) != 0) {  // Farbe für Rechteck setzen
            SDL_Log("%s: SDL_SetRenderDrawColor() failed: %s",__FUNCTION__,SDL_GetError());
            nErrorCode = -1;
        }
    }
    if (nErrorCode == 0) {
        nErrorCode = FillPanel(pRenderer);
    }
    return nErrorCode;
}









int Editor(SDL_Window *pWindow, SDL_Renderer *pRenderer) {
    char szElementInfo[80];
    bool bEditorRun;
    uint32_t uTextureIndex;
    uint32_t uX = WINDOW_W - 32;
    uint32_t uY = 0;
    uint32_t I;
    SDL_Rect DestR;                     // Zum Kopieren in den Renderer
    SDL_RendererFlip Flip = SDL_FLIP_NONE;
    float fAngle = 0;
    float fScaleW = 0.5;
    float fScaleH = 0.5;
    uint32_t uShowFrame;
    uint32_t uMouseElement;             // Levelelement, auf das Mauspfeil zeigt

    char szText[100];


    uTextureIndex = 3; // Stahl
    // Position innerhalb des Renderers
    DestR.x = uX;
    DestR.y = uY;
    DestR.w = FONT_W * fScaleW;
    DestR.h = FONT_H * fScaleH;
    if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&DestR,fAngle,NULL, Flip) != 0) {
        SDL_Log("%s: SDL_RenderCopyEx(standard) failed: %s",__FUNCTION__,SDL_GetError());
    }

    bEditorRun = true;
    uShowFrame = 0;

    while (bEditorRun) {
        UpdateInputStates();
        if (uShowFrame == 0) {

            uMouseElement = GetElementByMouseposition(InputStates.nMouseXpos, InputStates.nMouseYpos);

            sprintf(szElementInfo,"%s",szElementNames[uMouseElement]);
            if (InputStates.pKeyboardArray[SDL_SCANCODE_ESCAPE])  {
                bEditorRun = false;
            }
            for (I = 0; I < strlen(szElementInfo); I++) {
                DestR.x = I * FONT_W / 2;
                DestR.y = WINDOW_H - FONT_H / 2;
                DestR.w = FONT_W * fScaleW;
                DestR.h = FONT_H * fScaleH;

                DrawEditorPanel(pRenderer);
                uTextureIndex = ConvertASCII(szElementInfo[I]);
                if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&DestR,fAngle,NULL, Flip) != 0) {
                    SDL_Log("%s: SDL_RenderCopyEx(standard) failed: %s",__FUNCTION__,SDL_GetError());
                }
            }

            // Hallo, dies ist ein Test.
            // Hier folgt die naechste Zeile.
            // 1234567890
            // Noch eine Zeile.
            strcpy(szText,"Hallo, dies ist ein Test.\nHier folgt die naechste Zeile.\n1234567890\nNoch eine Zeile.");
            CreateMessageWindow(pRenderer,-1,-1,0,szText);


            // Ganz oben ein Stück Mauer zeichnen
            for (I = 0; (I < MAX_X_ELEMENTS); I++) {
                uTextureIndex = 3; // Stahl
                // Position innerhalb des Renderers
                DestR.x = I * FONT_W / 2;
                DestR.y = 0;
                DestR.w = FONT_W / 2;
                DestR.h = FONT_H / 2;
                if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&DestR,0,NULL, Flip) != 0) {
                    SDL_Log("%s: SDL_RenderCopyEx(standard) failed: %s",__FUNCTION__,SDL_GetError());
                }
            }
            SDL_RenderPresent(pRenderer);   // Renderer anzeigen, lässt Hauptschleife mit ~ 60 Hz (Bild-Wiederholfrequenz) laufen
            SDL_RenderClear(pRenderer);     // Renderer für nächstes Frame löschen
            Playfield.uFrameCounter++;
        }
        uShowFrame++;
        if (uShowFrame == 2) {
            uShowFrame = 0;
        }
        SDL_Delay(5);
    }
    return 0;
}
