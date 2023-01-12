#include "buttons.h"
#include "KeyboardMouse.h"
#include "mystd.h"


BUTTONS Buttons;
extern INPUTSTATES InputStates;


/*----------------------------------------------------------------------------
Name:           InitButtons
------------------------------------------------------------------------------
Beschreibung: Initialisiert die Buttons für den Editor. Wird einmalig
              vor Verwendung der Buttons aufgerufen.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Buttons.x
------------------------------------------------------------------------------*/
void InitButtons(void) {
    uint32_t I;

    Buttons.uButtons = 0;
    for (I = 0; I < MAX_BUTTONS; I++) {
        Buttons.Button[I].bActive = false;
        Buttons.Button[I].bPressed = false;
        Buttons.Button[I].pszText = NULL;
        Buttons.Button[I].uXpos = 0;
        Buttons.Button[I].uYpos = 0;
    }
}


/*----------------------------------------------------------------------------
Name:           FreeButtons
------------------------------------------------------------------------------
Beschreibung: Gibt erzeugte Buttons wieder frei.
              aufgerufen.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Buttons.x
------------------------------------------------------------------------------*/
void FreeButtons(void) {
    uint32_t I;

    for (I = 0; I < Buttons.uButtons; I++) {
        Buttons.Button[I].bActive = false;
        Buttons.Button[I].bPressed = false;
        SAFE_FREE(Buttons.Button[I].pszText);
    }
}


/*----------------------------------------------------------------------------
Name:           CreateButton
------------------------------------------------------------------------------
Beschreibung: Erzeugt einen Button für den Editor.
Parameter
      Eingang: pszText, char*, Zeiger auf Buttontext
               uXpos, int32_t, X-Position des Buttons
               uYpos, int32_t, X-Position des Buttons
               bActive, bool, true = Button ist aktiv und wird angezeigt
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Buttons.x
------------------------------------------------------------------------------*/
int CreateButton(char *pszText,uint32_t uXpos, int32_t uYpos, bool bActive) {
    int nErrorCode;
    int nButtonIndex;

    nErrorCode = -1;
    if (pszText != NULL) {
            if (strlen(pszText) <= MAX_BUTTONTEXT_LEN) {
                nButtonIndex = Buttons.uButtons;
                if (nButtonIndex < MAX_BUTTONS) {
                    Buttons.Button[nButtonIndex].pszText = malloc(strlen(pszText) + 1);
                    if (Buttons.Button[nButtonIndex].pszText) {
                        strcpy(Buttons.Button[nButtonIndex].pszText,pszText);
                        Buttons.Button[nButtonIndex].uXpos = uXpos;
                        Buttons.Button[nButtonIndex].uYpos = uYpos;
                        Buttons.Button[nButtonIndex].bActive = bActive;
                        Buttons.uButtons++;
                    } else {
                        SDL_Log("%s: can't not allocate memory for button text",__FUNCTION__);
                    }
                } else {
                    SDL_Log("%s: can't create button, maximal amount of %d buttons reached",__FUNCTION__,MAX_BUTTONS);
                }
            } else {
                SDL_Log("%s: button text too long, %d/%d chars",__FUNCTION__,(int)strlen(pszText),MAX_BUTTONTEXT_LEN);
            }
    } else {
        SDL_Log("%s: bad text pointer",__FUNCTION__);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           SetButtonActivity
------------------------------------------------------------------------------
Beschreibung:  Setzt einen Button aktiv bzw. deaktiv.
Parameter
      Eingang: uButton, uint32_t, Button-Index, bzw. Button-Nummer
               bActive, bool, true = Button wird aktiv geschaltet, sonst deaktiviert
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Buttons.x
------------------------------------------------------------------------------*/
int SetButtonActivity(uint32_t uButton, bool bActive) {
    int nErrorCode;

    nErrorCode = -1;
    if (uButton < Buttons.uButtons) {
        Buttons.Button[uButton].bActive = bActive;
        nErrorCode = 0;
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           SetButtonPosition
------------------------------------------------------------------------------
Beschreibung:  Setzt einen Button aktiv bzw. deaktiv.
Parameter
      Eingang: uButton, uint32_t, Button-Index, bzw. Button-Nummer
               uXpos, uint32_t, X-Position für Button
               uYpos, uint32_t, Y-Position für Button
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Buttons.x
------------------------------------------------------------------------------*/
int SetButtonPosition(uint32_t uButton, uint32_t uXpos, uint32_t uYpos) {
    int nErrorCode;

    nErrorCode = -1;
    if (uButton < Buttons.uButtons) {
        Buttons.Button[uButton].uXpos = uXpos;
        Buttons.Button[uButton].uYpos = uYpos;
        nErrorCode = 0;
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           ShowButtons
------------------------------------------------------------------------------
Beschreibung: Zeigt alle erzeugten und aktiven Buttons an.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Buttons.x, InputStates.x
------------------------------------------------------------------------------*/
int ShowButtons(SDL_Renderer *pRenderer) {
    int nErrorCode;
    uint32_t I;
    uint32_t uXpos;
    uint32_t uYpos;
    uint32_t uButtonW;
    uint32_t uButtonH;
    bool bButtonArea;

    nErrorCode = 0;
    for (I = 0; (I < Buttons.uButtons) && (nErrorCode == 0); I++) {
        Buttons.Button[I].bPressed = false;
        if (Buttons.Button[I].bActive) {
            uXpos = Buttons.Button[I].uXpos;
            uYpos = Buttons.Button[I].uYpos;
            if (Buttons.Button[I].pszText != NULL) {
                uButtonW = strlen(Buttons.Button[I].pszText) * FONT_LITTLE_559_W + FONT_LITTLE_559_W;
                uButtonH = BUTTON_H;
                bButtonArea = ((InputStates.nMouseXpos >= uXpos) && (InputStates.nMouseXpos <= (uXpos + uButtonW)) &&
                               (InputStates.nMouseYpos >= uYpos) && (InputStates.nMouseYpos <= (uYpos + uButtonH)));
                if  (InputStates.bLeftMouseButton && bButtonArea) {
                    // Buttonfläche erzeugen
                    nErrorCode = CopyColorRect(pRenderer,106,104,100,uXpos,uYpos,uButtonW,uButtonH);
                    Buttons.Button[I].bPressed = true;
                    nErrorCode = PrintLittleFont(pRenderer,uXpos + 4,uYpos + 2,1,Buttons.Button[I].pszText);
                    ShowOtherButtons(pRenderer);
                    SDL_RenderPresent(pRenderer);   // Renderer anzeigen
                    WaitNoKey();
                    bButtonArea = ((InputStates.nMouseXpos >= uXpos) && (InputStates.nMouseXpos <= (uXpos + uButtonW)) &&
                                   (InputStates.nMouseYpos >= uYpos) && (InputStates.nMouseYpos <= (uYpos + uButtonH)));
                    Buttons.Button[I].bPressed = bButtonArea;
                } else {
                    // Buttonfläche erzeugen
                    nErrorCode = CopyColorRect(pRenderer,212,208,200,uXpos,uYpos,uButtonW,uButtonH);
                }
                // Weißer Button-Rand oben und links zeichnen
                SDL_SetRenderDrawColor(pRenderer,255,255,255, SDL_ALPHA_OPAQUE);  // Farbe für Line setzen
                SDL_RenderDrawLine(pRenderer, uXpos, uYpos, uXpos + uButtonW + 1, uYpos); // oben
                SDL_RenderDrawLine(pRenderer, uXpos, uYpos, uXpos, uYpos + uButtonH + 1); // links
                // Grauen Button-Rand unten und rechts zeichnen
                SDL_SetRenderDrawColor(pRenderer,128,128,128, SDL_ALPHA_OPAQUE);  // Farbe für Line setzen
                SDL_RenderDrawLine(pRenderer, uXpos + 1, uYpos + uButtonH, uXpos + uButtonW, uYpos + uButtonH); // unten
                SDL_RenderDrawLine(pRenderer, uXpos + uButtonW, uYpos + 1, uXpos + uButtonW, uYpos + uButtonH); // rechts
                // Dunkel-Grauen Button-Rand unten und rechts zeichnen
                SDL_SetRenderDrawColor(pRenderer,64,64,64, SDL_ALPHA_OPAQUE);  // Farbe für Line setzen
                SDL_RenderDrawLine(pRenderer, uXpos + 1, uYpos + uButtonH + 1, uXpos + uButtonW, uYpos + uButtonH + 1); // unten
                SDL_RenderDrawLine(pRenderer, uXpos + uButtonW + 1, uYpos + 1, uXpos + uButtonW + 1, uYpos + uButtonH + 1); // rechts
                SDL_SetRenderDrawColor(pRenderer,0,0,0, SDL_ALPHA_OPAQUE);  // Farbe auf schwarz zurücksetzen
                nErrorCode = PrintLittleFont(pRenderer,uXpos + 4,uYpos + 2,1,Buttons.Button[I].pszText);
            } else {
                nErrorCode = -1;
                SDL_Log("%s: bad text pointer for button: %u",__FUNCTION__,I);
            }
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           ShowOtherButtons
------------------------------------------------------------------------------
Beschreibung: Zeigt alle erzeugten und aktiven Buttons während einer Haltephase (WaitNoKey) an.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Buttons.x, InputStates.x
------------------------------------------------------------------------------*/
int ShowOtherButtons(SDL_Renderer *pRenderer) {
    int nErrorCode;
    uint32_t I;
    uint32_t uXpos;
    uint32_t uYpos;
    uint32_t uButtonW;
    uint32_t uButtonH;

    nErrorCode = 0;
    for (I = 0; (I < Buttons.uButtons) && (nErrorCode == 0); I++) {
        if ((Buttons.Button[I].bActive) && (!Buttons.Button[I].bPressed)) {   // Nicht den gedrückten Button anzeigen
            uXpos = Buttons.Button[I].uXpos;
            uYpos = Buttons.Button[I].uYpos;
            if (Buttons.Button[I].pszText != NULL) {
                uButtonW = strlen(Buttons.Button[I].pszText) * FONT_LITTLE_559_W + FONT_LITTLE_559_W;
                uButtonH = BUTTON_H;
                // Buttonfläche erzeugen
                nErrorCode = CopyColorRect(pRenderer,212,208,200,uXpos,uYpos,uButtonW,uButtonH);
                // Weißer Button-Rand oben und links zeichnen
                SDL_SetRenderDrawColor(pRenderer,255,255,255, SDL_ALPHA_OPAQUE);  // Farbe für Line setzen
                SDL_RenderDrawLine(pRenderer, uXpos, uYpos, uXpos + uButtonW + 1, uYpos); // oben
                SDL_RenderDrawLine(pRenderer, uXpos, uYpos, uXpos, uYpos + uButtonH + 1); // links
                // Grauen Button-Rand unten und rechts zeichnen
                SDL_SetRenderDrawColor(pRenderer,128,128,128, SDL_ALPHA_OPAQUE);  // Farbe für Line setzen
                SDL_RenderDrawLine(pRenderer, uXpos + 1, uYpos + uButtonH, uXpos + uButtonW, uYpos + uButtonH); // unten
                SDL_RenderDrawLine(pRenderer, uXpos + uButtonW, uYpos + 1, uXpos + uButtonW, uYpos + uButtonH); // rechts
                // Dunkel-Grauen Button-Rand unten und rechts zeichnen
                SDL_SetRenderDrawColor(pRenderer,64,64,64, SDL_ALPHA_OPAQUE);  // Farbe für Line setzen
                SDL_RenderDrawLine(pRenderer, uXpos + 1, uYpos + uButtonH + 1, uXpos + uButtonW, uYpos + uButtonH + 1); // unten
                SDL_RenderDrawLine(pRenderer, uXpos + uButtonW + 1, uYpos + 1, uXpos + uButtonW + 1, uYpos + uButtonH + 1); // rechts
                SDL_SetRenderDrawColor(pRenderer,0,0,0, SDL_ALPHA_OPAQUE);  // Farbe auf schwarz zurücksetzen
                nErrorCode = PrintLittleFont(pRenderer,uXpos + 4,uYpos + 2,1,Buttons.Button[I].pszText);
            } else {
                nErrorCode = -1;
                SDL_Log("%s: bad text pointer for button: %u",__FUNCTION__,I);
            }
        }
    }
    return nErrorCode;
}
