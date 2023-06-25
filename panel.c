#include <stdio.h>
#include "config.h"
#include "EmeraldMine.h"
#include "KeyboardMouse.h"
#include "loadlevel.h"
#include "man.h"
#include "mySDL.h"
#include "panel.h"
#include "sound.h"

extern MANKEY ManKey;
extern PLAYFIELD Playfield;
extern SDL_DisplayMode ge_DisplayMode;
extern INPUTSTATES InputStates;
extern CONFIG Config;

/*----------------------------------------------------------------------------
Name:           CheckPlayTime
------------------------------------------------------------------------------
Beschreibung: Prüft, ob noch Spielzeit verfügbar ist und verringert dann nach
              jedem Frame die Spielzeit um 1.
              Diese Funktion wird aus RenderLevel() aufgerufen, d.h. bei jedem
              Frame. Zusätzlich wird hier auch der Frame-Counter erhöht.

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void CheckPlayTime(void) {
    Playfield.uFrameCounter++;
    if (Playfield.uTimeToPlay > 0) {
        Playfield.uTimeToPlay--;
    }
}


/*----------------------------------------------------------------------------
Name:           ShowPanel
------------------------------------------------------------------------------
Beschreibung: Zeigt das Spielepanel mit den aktuellen Werten wie z.B. Time,
              Score, Emeralds, usw.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x, ge_DisplayMode.x, Config.x
------------------------------------------------------------------------------*/
int ShowPanel(SDL_Renderer *pRenderer) {
    SDL_Rect DestR;         // Zum Kopieren in den Renderer
    int nErrorCode;
    char szNumber[8];
    uint32_t uRemainingSeconds;

    // Die folgenden Werte müssen 4-stellig bleiben
    if (Playfield.uTotalScore > 9999) {
        Playfield.uTotalScore = 9999;
    }
    if (Playfield.uEmeraldsToCollect > 9999) {
        Playfield.uEmeraldsToCollect = 9999;
    }
    uRemainingSeconds = Playfield.uTimeToPlay / ge_DisplayMode.refresh_rate;
    if (uRemainingSeconds > 9999) {
        uRemainingSeconds = 9999;
    }
    // Anzeige-Panel erzeugen
    DestR.x = 0;
    DestR.y = Config.uResY - FONT_H;
    DestR.w = Config.uResX;
    DestR.h = PANEL_H;
    if (SDL_RenderCopy(pRenderer,GetTextureByIndex(83),NULL,&DestR) != 0) {
        SDL_Log("%s: SDL_RenderCopy() failed: %s",__FUNCTION__,SDL_GetError());
        return - 1;
    }
    if ((uRemainingSeconds > 0) && (uRemainingSeconds <= 10)) {
        if ((Playfield.uFrameCounter % ge_DisplayMode.refresh_rate) == 0) {
            nErrorCode = CopyColorRect(pRenderer,255,0,0,6,Config.uResY - 26,98,20,K_ABSOLUTE);
            PreparePlaySound(SOUND_END_BELL,0);
        } else {
            nErrorCode = CopyColorRect(pRenderer,255 - 4 * (Playfield.uFrameCounter % 60),0,0,6,Config.uResY - 26,98,20,K_ABSOLUTE);
        }
    } else {
        nErrorCode = 0;
    }
    if (nErrorCode == 0) {
        nErrorCode = -1;
        if (WritePanelText(pRenderer,"TIME:",8, Config.uResY - FONT_H + 8, 16, false) == 0) {
            sprintf(szNumber,"%d",uRemainingSeconds);
            if (WritePanelText(pRenderer,szNumber,8 + 80, Config.uResY - FONT_H + 8 , 16, true) == 0) {
                if (WritePanelText(pRenderer,"SCORE:",8 + 176, Config.uResY - FONT_H + 8, 16, false) == 0) {
                    sprintf(szNumber,"%d",Playfield.uTotalScore);
                    if (WritePanelText(pRenderer,szNumber,8 + 272, Config.uResY - FONT_H + 8 , 16, true) == 0) {
                        if (WritePanelText(pRenderer,"EMERALDS:",8 + 368, Config.uResY - FONT_H + 8, 16, false) == 0) {
                            sprintf(szNumber,"%d",Playfield.uEmeraldsToCollect);
                            if (WritePanelText(pRenderer,szNumber,8 + 512, Config.uResY - FONT_H + 8 , 16, true) == 0) {
                                if (WritePanelDynamitHammerKeys(pRenderer) == 0) {
                                    nErrorCode = 0;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           WritePanelDynamitHammerKeys
------------------------------------------------------------------------------
Beschreibung: Zeichnet Dynamit- und Hammer-Anzahl ins Panel.
              Zusätzlich werden noch aufgesammelte Schlüssel eingeblendet.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x, Config.x
------------------------------------------------------------------------------*/
int WritePanelDynamitHammerKeys(SDL_Renderer *pRenderer) {
    SDL_Rect DestR;         // Zum Kopieren in den Renderer
    int nErrorCode;
    uint32_t uDynamiteCount;
    uint32_t uHammerCount;
    uint32_t uWhiteKeyCount;
    char szDynamitString[16];   // ":xxx", 5 Bytes inkl. \0

    nErrorCode = -1;
    // Anzeige muss 3-stellig bleiben
    uDynamiteCount = Playfield.uDynamiteCount;
    if (uDynamiteCount > 999) {
        uDynamiteCount = 999;
    }
    uHammerCount = Playfield.uHammerCount;
    if (uHammerCount > 999) {
        uHammerCount = 999;
    }
    uWhiteKeyCount = Playfield.uWhiteKeyCount;
    if (uWhiteKeyCount > 999) {
        uWhiteKeyCount = 999;
    }
    DestR.x = 618;
    DestR.y = Config.uResY - FONT_H + 8;
    DestR.w = FONT_H / 2;
    DestR.h = FONT_H / 2;
    if (SDL_RenderCopy(pRenderer,GetTextureByIndex(286),NULL,&DestR) == 0) {
        sprintf(szDynamitString,":%d",uDynamiteCount);
        if (WritePanelText(pRenderer,szDynamitString,630, Config.uResY - FONT_H + 8 , 16, true) == 0) {
            sprintf(szDynamitString,":%d",uHammerCount);
            if (WritePanelText(pRenderer,szDynamitString,952, Config.uResY - FONT_H + 8 , 16, true) == 0) {
                sprintf(szDynamitString,":%d",uWhiteKeyCount);
                if (WritePanelText(pRenderer,szDynamitString,840, Config.uResY - FONT_H + 8 , 16, true) == 0) {
                    DestR.x = 932;
                    DestR.y = Config.uResY - FONT_H + 8;
                    DestR.w = FONT_W / 2;
                    DestR.h = FONT_H / 2;
                    if (SDL_RenderCopy(pRenderer,GetTextureByIndex(318),NULL,&DestR) == 0) { // Hammer
                        DestR.x = 820;
                        DestR.y = Config.uResY - FONT_H + 8;
                        DestR.w = FONT_W / 2;
                        DestR.h = FONT_H / 2;
                        if (SDL_RenderCopy(pRenderer,GetTextureByIndex(519),NULL,&DestR) == 0) { // Weißer Schlüssel
                            nErrorCode = 0;
                        }
                    } else {
                        SDL_Log("%s: SDL_RenderCopy(Hammer) failed: %s",__FUNCTION__,SDL_GetError());
                    }
                } else {
                    SDL_Log("%s: WritePanelText(WhiteKeyCount) failed",__FUNCTION__);
                }
            } else {
                SDL_Log("%s: WritePanelText(HammerCount) failed",__FUNCTION__);
            }
        } else {
            SDL_Log("%s: WritePanelText(DynamiteCount) failed",__FUNCTION__);
        }
    } else {
        SDL_Log("%s: SDL_RenderCopy() failed: %s",__FUNCTION__,SDL_GetError());
    }
    if ((Playfield.bHasGreenKey) && (nErrorCode == 0)) {
        DestR.x = 726;
        DestR.y = Config.uResY - FONT_H + 4;
        DestR.w = FONT_W / 3;
        DestR.h = FONT_H / 3;
        nErrorCode = SDL_RenderCopy(pRenderer,GetTextureByIndex(99),NULL,&DestR); // grüner Schlüssel
        if (nErrorCode != 0) {
            SDL_Log("%s: SDL_RenderCopy(green Key) failed: %s",__FUNCTION__,SDL_GetError());
        }
    }
    if ((Playfield.bHasRedKey) && (nErrorCode == 0)) {
        DestR.x = 726;
        DestR.y = Config.uResY - FONT_H + 16;
        DestR.w = FONT_W / 3;
        DestR.h = FONT_H / 3;
        nErrorCode = SDL_RenderCopy(pRenderer,GetTextureByIndex(98),NULL,&DestR); // roter Schlüssel
        if (nErrorCode != 0) {
            SDL_Log("%s: SDL_RenderCopy(red Key) failed: %s",__FUNCTION__,SDL_GetError());
        }
    }
    if ((Playfield.bHasBlueKey) && (nErrorCode == 0)) {
        DestR.x = 738;
        DestR.y = Config.uResY - FONT_H + 4;
        DestR.w = FONT_W / 3;
        DestR.h = FONT_H / 3;
        nErrorCode = SDL_RenderCopy(pRenderer,GetTextureByIndex(100),NULL,&DestR); // blauer Schlüssel
        if (nErrorCode != 0) {
            SDL_Log("%s: SDL_RenderCopy(blue Key) failed: %s",__FUNCTION__,SDL_GetError());
        }
    }
    if ((Playfield.bHasYellowKey) && (nErrorCode == 0)) {
        DestR.x = 738;
        DestR.y = Config.uResY - FONT_H + 16;
        DestR.w = FONT_W / 3;
        DestR.h = FONT_H / 3;
        nErrorCode = SDL_RenderCopy(pRenderer,GetTextureByIndex(101),NULL,&DestR); // gelber Schlüssel
        if (nErrorCode != 0) {
            SDL_Log("%s: SDL_RenderCopy(yellow Key) failed: %s",__FUNCTION__,SDL_GetError());
        }
    }
    if ((Playfield.bHasGeneralKey) && (nErrorCode == 0)) {
        DestR.x = 755;
        DestR.y = Config.uResY - FONT_H;
        DestR.w = FONT_W;
        DestR.h = FONT_H;
        nErrorCode = SDL_RenderCopy(pRenderer,GetTextureByIndex(520),NULL,&DestR); // General-Schlüssel
        if (nErrorCode != 0) {
            SDL_Log("%s: SDL_RenderCopy(general Key) failed: %s",__FUNCTION__,SDL_GetError());
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           WritePanelText
------------------------------------------------------------------------------
Beschreibung: Schreibt Text in den Renderer.

Parameter
      Eingang: SDL_Renderer *, pRenderer, Zeiger auf Renderer
               szText, const char *, Zeiger auf Text
               nXpos, int, X-Position in Pixeln
               nYpos, int, Y-Position in Pixeln
               nFontSize, int, Skalierung für Font (Standard = 32, für Panel = 16)
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int WritePanelText(SDL_Renderer *pRenderer, const char *szText, int nXpos, int nYpos, int nFontSize, bool bGreenNumbers) {
    int nErrorCode;
    int nI;
    SDL_Rect DestR;         // Zum Kopieren in den Renderer

    if ((pRenderer == NULL) && (szText == NULL)) {
        SDL_Log("%s: bad pointers",__FUNCTION__);
        return -1;
    }
    nI = 0;
    nErrorCode = 0;
    while ((nErrorCode == 0) && (szText[nI] != 0)) {
        DestR.x = nXpos;
        DestR.y = nYpos;
        DestR.w = nFontSize;
        DestR.h = nFontSize;
        if ((szText[nI] >= '0') && (szText[nI] <= '9') && bGreenNumbers) {
            // nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(84 - 0x30 + szText[nI]),NULL,&DestR,0,NULL, SDL_FLIP_NONE);
            nErrorCode = SDL_RenderCopy(pRenderer,GetTextureByIndex(84 - 0x30 + szText[nI]),NULL,&DestR);
            if (nErrorCode != 0) {
                SDL_Log("%s[green font]: SDL_RenderCopyEx() failed: %s",__FUNCTION__,SDL_GetError());
                nErrorCode = -1;
            }
        } else {
            // nErrorCode = SDL_RenderCopyEx(pRenderer,GetTextureByIndex(szText[nI] - 32),NULL,&DestR,0,NULL, SDL_FLIP_NONE);
            nErrorCode = SDL_RenderCopy(pRenderer,GetTextureByIndex(szText[nI] - 32),NULL,&DestR);
            if (nErrorCode != 0) {
                SDL_Log("%s: SDL_RenderCopyEx() failed: %s",__FUNCTION__,SDL_GetError());
                nErrorCode = -1;
            }
        }
        nI++;
        nXpos = nXpos + nFontSize;
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           ConfirmMessage
------------------------------------------------------------------------------
Beschreibung: Erzeugt ein Fenster mit Text und wartet auf "Fire" oder "Space".
              Wurde keine Text hinterlegt, so wird ein Standardtext angezeigt.

Parameter
      Eingang: SDL_Renderer *, pRenderer, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int ConfirmMessage(SDL_Renderer *pRenderer) {
    char szText[128];
    uint32_t uFireState;
    uint32_t uSpaceState;
    char *pszMessage;
    int nErrorCode;

    strcpy(szText,"NO MESSAGE!\n\nPRESS FIRE OR SPACE TO CONFIRM!");
    pszMessage = Playfield.pMessage[Playfield.uShowMessageNo - 1];
    if (pszMessage == NULL) {
        pszMessage = szText;
    }
    nErrorCode = -1;
    uFireState = 0;
    uSpaceState = 0;
    if (CreateMessageWindow(pRenderer,-1,-1,0,pszMessage) == 0) {
        nErrorCode = 0;
        SDL_RenderPresent(pRenderer);   // Renderer anzeigen, lässt Hauptschleife mit ~ 60 Hz (Bild-Wiederholfrequenz) laufen
        do {
            UpdateManKey();
            if ((uFireState == 0) && (!ManKey.bFire)) {
                uFireState = 1;
            } else if ((uFireState == 1) && (ManKey.bFire)) {
                uFireState = 2;
            } else if ((uFireState == 2) && (!ManKey.bFire)) {
                uFireState = 3;
            }
            if ((uSpaceState == 0) && (InputStates.pKeyboardArray[SDL_SCANCODE_SPACE] == 0)) {
                uSpaceState = 1;
            } else if ((uSpaceState == 1) && (InputStates.pKeyboardArray[SDL_SCANCODE_SPACE])) {
                uSpaceState = 2;
            } else if ((uSpaceState == 2) && (InputStates.pKeyboardArray[SDL_SCANCODE_SPACE] == 0)) {
                uSpaceState = 3;
            }
            SDL_Delay(100);
        } while ((uFireState != 3) && (uSpaceState != 3));
    }
    Playfield.uShowMessageNo = 0;
    return nErrorCode;
}

