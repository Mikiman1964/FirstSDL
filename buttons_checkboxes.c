#include "buttons_checkboxes.h"
#include "KeyboardMouse.h"
#include "mystd.h"

CHECKBOX *Checkboxes[MAX_CHECKBOXES];
BUTTON Buttons[MAX_BUTTONS];
extern INPUTSTATES InputStates;
extern uint8_t g_uIntensityProzent;
extern uint32_t ge_uXoffs;             // X-Offset für die Zentrierung von Elementen
extern uint32_t ge_uYoffs;             // X-Offset für die Zentrierung von Elementen


/*----------------------------------------------------------------------------
Name:           InitCheckboxes
------------------------------------------------------------------------------
Beschreibung: Initialisiert die Checkboxen. Wird einmalig
              vor Verwendung der Checkboxen aufgerufen.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Checkboxes[]
------------------------------------------------------------------------------*/
void InitCheckboxes(void) {
    uint32_t I;

    for (I = 0; I < MAX_CHECKBOXES; I++) {
        Checkboxes[I] = NULL;
    }
}


/*----------------------------------------------------------------------------
Name:           RegisterCheckbox
------------------------------------------------------------------------------
Beschreibung: Registriert eine Checkbox zur Verwendung.
              Wichtig: Wird eine Checkbox-Struktur lokal verwendet, muss die
              Checkbox vor verlassen der Funktion wieder deregistriert werden.
Parameter
      Eingang: pCheckbox, CHECKBOX *, Zeiger auf Checkbox-Struktur
               bActive, bool, true = Checkbox ist aktiv, also angehakt
               pszText, char *, Zeiger auf Checkboxtext
               nXpos, int, X-Position
               nYpos, int, Y-Position
               bRadio, bool, true = Checkbox hat Radiobutton-Verhalten (optisch)
               bUse, bool, true = Checkbox ist sichtbar und verwendbar, sonst versteckt und inaktiv
      Ausgang: -
Rückgabewert:  int, 0 = OK, sonst Fehler
Seiteneffekte: Checkboxes[]
------------------------------------------------------------------------------*/
int RegisterCheckbox(CHECKBOX *pCheckbox, bool bActive, char *pszText, int nXpos, int nYpos, bool bRadio, bool bUse) {
    int nErrorCode = -1;
    int I;
    bool bFound = false;

    if ((pCheckbox != NULL) && (pszText != NULL)) {
        if (strlen(pszText) < MAX_CHECKBOXTEXT_LEN) {
            // Freien Platz suchen
            for (I = 0; (I < MAX_CHECKBOXES) && (!bFound); I++) {
                if (Checkboxes[I] == NULL) {
                    bFound = true;
                    pCheckbox->bRadio = bRadio;
                    pCheckbox->nRegisterIndex = I;
                    pCheckbox->nXpos = nXpos;
                    pCheckbox->nYpos = nYpos;
                    pCheckbox->bActive = bActive;
                    pCheckbox->bPressed = false;
                    pCheckbox->bChanged = false;
                    pCheckbox->bUse = bUse;
                    strcpy(pCheckbox->szText,pszText);
                    Checkboxes[I] = pCheckbox;
                }
            }
            if (bFound) {
                nErrorCode = 0;
            }
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           DeRegisterCheckbox
------------------------------------------------------------------------------
Beschreibung: De-Registriert eine Checkbox zur Freigabe.
              Wichtig: Wird eine Checkbox-Struktur lokal verwendet, muss die
              Checkbox vor verlassen der Funktion wieder deregistriert werden.
Parameter
      Eingang: pCheckbox, CHECKBOX *, Zeiger auf Checkbox-Struktur
      Ausgang: -
Rückgabewert:  int, 0 = OK, sonst Fehler
Seiteneffekte: Checkboxes[]
------------------------------------------------------------------------------*/
int DeRegisterCheckbox(CHECKBOX *pCheckbox) {
    int nErrorCode = -1;
    int nRegisterIndex;

    if (pCheckbox != NULL) {
        nRegisterIndex = pCheckbox->nRegisterIndex;
        if ((nRegisterIndex >= 0) && (nRegisterIndex < MAX_CHECKBOXES)) {
            if (Checkboxes[nRegisterIndex] != NULL) {
                Checkboxes[nRegisterIndex] = NULL;
                nErrorCode = 0;
            }
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           ShowCheckboxes
------------------------------------------------------------------------------
Beschreibung: Zeigt alle registrierten Checkboxen an. Funktion muss in
              einer Event-Hauptschleife zyklisch aufgerufen werden.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
               nDimm, int, Helligkeit der Checkboxen
               bAbsolute, bool, true = absolute Koordinaten, d.h. es erfolgt keinte Umrechnung
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Checkboxes[], InputStates.x, ge_uXoffs, ge_uYoffs
------------------------------------------------------------------------------*/
int ShowCheckboxes(SDL_Renderer *pRenderer, int nDimm, bool bAbsolute) {
    int nErrorCode = 0;
    int I;
    uint32_t uXoffs;
    uint32_t uYoffs;
    bool bOldPressed = false;;
    bool bNewPressed = false;
    CHECKBOX *pC;
    SDL_Rect Rect;
    SDL_Rect RectSmall;

    if (bAbsolute) {
        uXoffs = 0;
        uYoffs = 0;
    } else {
        uXoffs = ge_uXoffs;
        uYoffs = ge_uYoffs;
    }
    Rect.w = 20;
    Rect.h = 20;
    RectSmall.w = 10;
    RectSmall.h = 10;
    for (I = 0; (I < MAX_CHECKBOXES) && (nErrorCode == 0); I++) {
        pC = Checkboxes[I] ;
        bNewPressed = false;
        bOldPressed = false;
        if (pC != NULL) {
            if (pC->bUse) {
                Rect.x = pC->nXpos + uXoffs;
                Rect.y = pC->nYpos + uYoffs;
                bOldPressed = pC->bPressed;    // Bisheriger gedrückter Zustand
                nErrorCode = -1;
                if (SDL_SetRenderDrawColor(pRenderer,0,(255 * nDimm) / 100,0,255) == 0) {
                    if (SDL_RenderDrawRect(pRenderer,&Rect) == 0) {
                        nErrorCode = PrintLittleFont(pRenderer,pC->nXpos + 25,pC->nYpos + 3,0,pC->szText,bAbsolute);
                        if (nErrorCode == 0) {
                            // Kreuz bzw. kleines Quadrat malen, wenn aktiv
                            if (pC->bActive) {
                                if (pC->bRadio) {
                                    RectSmall.x = pC->nXpos + 5 + uXoffs;
                                    RectSmall.y = pC->nYpos + 5 + uYoffs;
                                    nErrorCode = SDL_RenderFillRect(pRenderer,&RectSmall);
                                } else {
                                    nErrorCode = SDL_RenderDrawLine(pRenderer, pC->nXpos + uXoffs, pC->nYpos + uYoffs, pC->nXpos + 19 + uXoffs, pC->nYpos + 19 + uYoffs);
                                    if (nErrorCode == 0) {
                                        nErrorCode = SDL_RenderDrawLine(pRenderer, pC->nXpos + 19 + uXoffs, pC->nYpos + uYoffs, pC->nXpos + uXoffs, pC->nYpos + 19 + uYoffs);
                                    }
                                }
                            }
                        }
                    }
                    if (nErrorCode == 0) {
                        if (InputStates.bLeftMouseButton) {
                            if (bAbsolute) {
                                if ( (InputStates.nMouseXpos_Absolute >= pC->nXpos) && (InputStates.nMouseXpos_Absolute < (pC->nXpos + 20)) && (InputStates.nMouseYpos_Absolute >= pC->nYpos) && (InputStates.nMouseYpos_Absolute < (pC->nYpos + 20))) {
                                    bNewPressed = true;
                                }
                            } else {
                                if ( (InputStates.nMouseXpos_Relative >= pC->nXpos) && (InputStates.nMouseXpos_Relative < (pC->nXpos + 20)) && (InputStates.nMouseYpos_Relative >= pC->nYpos) && (InputStates.nMouseYpos_Relative < (pC->nYpos + 20))) {
                                    bNewPressed = true;
                                }
                            }
                        }
                        pC->bPressed = bNewPressed;
                        if ((bOldPressed) && (!bNewPressed)) {
                            pC->bActive = !(pC->bActive);
                            pC->bChanged = true;
                        }
                    }
                }
            }
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           InitButtons
------------------------------------------------------------------------------
Beschreibung: Initialisiert die Buttons. Wird einmalig
              vor Verwendung der Buttons aufgerufen.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Buttons[].x
------------------------------------------------------------------------------*/
void InitButtons(void) {
    uint32_t I;

    for (I = 0; I < MAX_BUTTONS; I++) {
        Buttons[I].pszText = NULL;
        Buttons[I].pszLabel = NULL;
        Buttons[I].bActive = false;
        Buttons[I].bPressed = false;
        Buttons[I].uXpos = 0;
        Buttons[I].uYpos = 0;
    }
}


/*----------------------------------------------------------------------------
Name:           FreeAllButtons
------------------------------------------------------------------------------
Beschreibung: Gibt alle erzeugten Buttons wieder frei.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Buttons[].x
------------------------------------------------------------------------------*/
void FreeAllButtons(void) {
    uint32_t I;

    for (I = 0; I < MAX_BUTTONS; I++) {
        if (Buttons[I].pszLabel != NULL) {  // Button belegt ?
            Buttons[I].bActive = false;
            Buttons[I].bPressed = false;
            Buttons[I].uXpos = 0;
            Buttons[I].uYpos = 0;
            SAFE_FREE(Buttons[I].pszText);
            SAFE_FREE(Buttons[I].pszLabel);
        }
    }
}


/*----------------------------------------------------------------------------
Name:           FreeButton
------------------------------------------------------------------------------
Beschreibung: Gibt einen bestimmten Button wieder frei.
Parameter
      Eingang: pszlabel, char *, Zeiger auf Label des Buttons
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Buttons[].x
------------------------------------------------------------------------------*/
void FreeButton(char *pszLabel) {
    uint32_t I;
    bool bFound;

    bFound = false;
    if (pszLabel != NULL) {
        for (I = 0; (I < MAX_BUTTONS) && (!bFound); I++) {
            if (Buttons[I].pszLabel != NULL) {
                if (strcmp(pszLabel,Buttons[I].pszLabel) == 0) {
                    Buttons[I].bActive = false;
                    Buttons[I].bPressed = false;
                    Buttons[I].uXpos = 0;
                    Buttons[I].uYpos = 0;
                    SAFE_FREE(Buttons[I].pszText);
                    SAFE_FREE(Buttons[I].pszLabel);
                    bFound = true;  // Es darf nur einen Button mit diesem Label geben
                }
            }
        }
    }
}

/*----------------------------------------------------------------------------
Name:           GetButtonIndex
------------------------------------------------------------------------------
Beschreibung: Ermittelt den Array-Index einen Buttons anhand des Button-Labels.
              ACHTUNG: Diese Funktion sollte nur innerhalb des Button-Moduls verwendet
              werden.
Parameter
      Eingang: pszlabel, char *, Zeiger auf Label des Buttons
      Ausgang: -
Rückgabewert:  Array-Index, -1 = nicht gefunden
Seiteneffekte: Buttons[].x
------------------------------------------------------------------------------*/
int GetButtonIndex(char *pszLabel) {
    int nButtonIndex;
    uint32_t I;

    nButtonIndex = -1;
    if (pszLabel != NULL) {
        I = 0;
        do {
            if (Buttons[I].pszLabel != NULL) {
                if (strcmp(pszLabel,Buttons[I].pszLabel) == 0) {
                    nButtonIndex = (int)I;
                }
            }
            I++;
        } while ((I < MAX_BUTTONS) && (nButtonIndex == -1));
    }
    return nButtonIndex;
}


/*----------------------------------------------------------------------------
Name:           GetFreeButtonIndex
------------------------------------------------------------------------------
Beschreibung: Ermittelt den Array-Index des nächsten freien Buttons.
              ACHTUNG: Diese Funktion sollte nur innerhalb des Button-Moduls verwendet
              werden.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  Array-Index des freien Platzes, -1 = keinen freien Platz gefunden
Seiteneffekte: Buttons[].x
------------------------------------------------------------------------------*/
int GetFreeButtonIndex(void) {
    int nButtonIndex;
    uint32_t I;

    nButtonIndex = -1;
    I = 0;
    do {
        if (Buttons[I].pszLabel == NULL) {
            nButtonIndex = (int)I;
        }
        I++;
    } while ((I < MAX_BUTTONS) && (nButtonIndex == -1));
    return nButtonIndex;
}


/*----------------------------------------------------------------------------
Name:           CreateButton
------------------------------------------------------------------------------
Beschreibung: Erzeugt einen Button für den Editor.
Parameter
      Eingang: pszLabel, char *, Zeiger auf gewünschtes Button-Label
               pszText, char*, Zeiger auf Buttontext
               uXpos, int32_t, X-Position des Buttons
               uYpos, int32_t, X-Position des Buttons
               bActive, bool, true = Button ist aktiv und wird angezeigt
               bWaitRelease, bool, true = wartet solange, bis Button losgelassen wird
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Buttons[].x
------------------------------------------------------------------------------*/
int CreateButton(char *pszLabel, char *pszText,uint32_t uXpos, int32_t uYpos, bool bActive, bool bWaitRelease) {
    int nErrorCode;
    int nButtonIndex;

    nErrorCode = -1;
    if ((pszText != NULL) && (pszLabel != NULL)) {
        if ((strlen(pszText) > 0) && (strlen(pszText) <= MAX_BUTTONTEXT_LEN)) {
            nButtonIndex = GetButtonIndex(pszLabel);    // darf nicht gefunden werden
            if (nButtonIndex == -1) {                   // -1 = nicht gefunden, d.h. alles OK
                nButtonIndex = GetFreeButtonIndex();
                if (nButtonIndex != -1) {
                    // Speicher für Label und Buttontext erzeugen
                    Buttons[nButtonIndex].pszLabel = malloc(strlen(pszLabel) + 1);
                    Buttons[nButtonIndex].pszText = malloc(strlen(pszText) + 1);
                    if ((Buttons[nButtonIndex].pszLabel != NULL) && (Buttons[nButtonIndex].pszText != NULL)) {
                        // Buttonlabel und Buttontext eintragen
                        strcpy(Buttons[nButtonIndex].pszLabel,pszLabel);
                        strcpy(Buttons[nButtonIndex].pszText,pszText);
                        Buttons[nButtonIndex].uXpos = uXpos;
                        Buttons[nButtonIndex].uYpos = uYpos;
                        Buttons[nButtonIndex].bActive = bActive;
                        Buttons[nButtonIndex].bWaitRelease = bWaitRelease;
                        Buttons[nButtonIndex].bPressed = false;
                        nErrorCode = 0;
                    } else {
                        SDL_Log("%s: can't not allocate memory for button label/text",__FUNCTION__);
                        SAFE_FREE(Buttons[nButtonIndex].pszLabel);
                        SAFE_FREE(Buttons[nButtonIndex].pszText);
                    }
                } else {
                    SDL_Log("%s: no more free buttons available",__FUNCTION__);
                }
            } else {
                SDL_Log("%s: button label %s already exist",__FUNCTION__,pszLabel);

            }
        } else {
            SDL_Log("%s: button text has invalid len %d, maxlen = %d chars",__FUNCTION__,(int)strlen(pszText),MAX_BUTTONTEXT_LEN);
        }
    } else {
        SDL_Log("%s: bad  pointer",__FUNCTION__);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           SetButtonActivity
------------------------------------------------------------------------------
Beschreibung:  Setzt einen Button aktiv bzw. deaktiv.
Parameter
      Eingang: pszLabel, char *, Zeiger auf Label des Buttons
               bActive, bool, true = Button wird aktiv geschaltet, sonst deaktiviert
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Buttons[].x
------------------------------------------------------------------------------*/
int SetButtonActivity(char *pszLabel, bool bActive) {
    int nErrorCode;
    uint32_t I;

    nErrorCode = -1;
    if (pszLabel != NULL) {
        I = 0;
        do {
            if (Buttons[I].pszLabel != NULL) {
                if (strcmp(pszLabel,Buttons[I].pszLabel) == 0) {
                    Buttons[I].bActive = bActive;
                    nErrorCode = 0;
                }
            }
            I++;
        } while ((I < MAX_BUTTONS) && (nErrorCode == -1));
    }
    if (nErrorCode != 0) {
        SDL_Log("%s: button not found",__FUNCTION__);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           SetButtonPosition
------------------------------------------------------------------------------
Beschreibung:  Setzt die Position eines Buttons.
Parameter
      Eingang: pszLabel, char *, Zeiger auf Label des Buttons
               uXpos, uint32_t, X-Position für Button
               uYpos, uint32_t, Y-Position für Button
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Buttons[].x
------------------------------------------------------------------------------*/
int SetButtonPosition(char *pszLabel, uint32_t uXpos, uint32_t uYpos) {
    uint32_t I;
    int nErrorCode;

    nErrorCode = -1;
    if (pszLabel != NULL) {
        I = 0;
        do {
            if (Buttons[I].pszLabel != NULL) {
                if (strcmp(pszLabel,Buttons[I].pszLabel) == 0) {
                    Buttons[I].uXpos = uXpos;
                    Buttons[I].uYpos = uYpos;
                    nErrorCode = 0;
                }
            }
            I++;
        } while ((I < MAX_BUTTONS) && (nErrorCode == -1));
    }
    if (nErrorCode != 0) {
        SDL_Log("%s: button not found",__FUNCTION__);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           ShowButtons
------------------------------------------------------------------------------
Beschreibung: Zeigt alle erzeugten und aktiven Buttons an. Funktion muss in
              einer Event-Hauptschleife zyklisch aufgerufen werden.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Buttons[].x, InputStates.x, g_uIntensityProzent, ge_uXoffs, ge_uYoffs
------------------------------------------------------------------------------*/
int ShowButtons(SDL_Renderer *pRenderer) {
    int nErrorCode;
    uint32_t I;
    uint32_t uXpos;
    uint32_t uYpos;
    uint32_t uButtonW;
    uint32_t uButtonH;
    bool bButtonArea;
    float fIntensityProzent;

    fIntensityProzent = (float)g_uIntensityProzent / 100;
    nErrorCode = 0;
    for (I = 0; (I < MAX_BUTTONS) && (nErrorCode == 0); I++) {
        Buttons[I].bPressed = false;
        if ((Buttons[I].pszLabel != NULL) && (Buttons[I].pszText != NULL) && (Buttons[I].bActive)) {
            uXpos = Buttons[I].uXpos;
            uYpos = Buttons[I].uYpos;
            uButtonW = strlen(Buttons[I].pszText) * FONT_LITTLE_559_W + FONT_LITTLE_559_W;
            uButtonH = BUTTON_H;
            bButtonArea = ((InputStates.nMouseXpos_Relative >= uXpos) && (InputStates.nMouseXpos_Relative <= (uXpos + uButtonW)) &&
                           (InputStates.nMouseYpos_Relative >= uYpos) && (InputStates.nMouseYpos_Relative <= (uYpos + uButtonH)));
            if  (InputStates.bLeftMouseButton && bButtonArea) {
                // Buttonfläche erzeugen
                Buttons[I].bPressed = true;
                nErrorCode = CopyColorRect(pRenderer,106 * fIntensityProzent,104 * fIntensityProzent,100 * fIntensityProzent,uXpos,uYpos,uButtonW,uButtonH,K_RELATIVE);
                nErrorCode = PrintLittleFont(pRenderer,uXpos + 4,uYpos + 2,1,Buttons[I].pszText,K_RELATIVE);
                ShowOtherButtons(pRenderer);
                SDL_RenderPresent(pRenderer);   // Renderer anzeigen
                if (Buttons[I].bWaitRelease) {
                    WaitNoKey();
                }
                // Falls Maus vom Button gezogen wurde, während die linke Maustaste gedrückt wurde
                Buttons[I].bPressed = ((InputStates.nMouseXpos_Relative >= uXpos) && (InputStates.nMouseXpos_Relative <= (uXpos + uButtonW)) &&
                                       (InputStates.nMouseYpos_Relative >= uYpos) && (InputStates.nMouseYpos_Relative <= (uYpos + uButtonH)));
            } else {
                // Buttonfläche erzeugen
                nErrorCode = CopyColorRect(pRenderer,212 * fIntensityProzent,208 * fIntensityProzent,200 * fIntensityProzent,uXpos,uYpos,uButtonW,uButtonH,K_RELATIVE);
            }
            // Weißer Button-Rand oben und links zeichnen
            SDL_SetRenderDrawColor(pRenderer,255 * fIntensityProzent,255 * fIntensityProzent,255 * fIntensityProzent, SDL_ALPHA_OPAQUE);  // Farbe für Line setzen
            SDL_RenderDrawLine(pRenderer, uXpos + ge_uXoffs, uYpos + ge_uYoffs, uXpos + uButtonW + 1 + ge_uXoffs, uYpos + ge_uYoffs); // oben
            SDL_RenderDrawLine(pRenderer, uXpos + ge_uXoffs, uYpos + ge_uYoffs, uXpos + ge_uXoffs, uYpos + uButtonH + 1 + ge_uYoffs); // links
            // Grauen Button-Rand unten und rechts zeichnen
            SDL_SetRenderDrawColor(pRenderer,128 * fIntensityProzent,128 * fIntensityProzent,128 * fIntensityProzent, SDL_ALPHA_OPAQUE);  // Farbe für Line setzen
            SDL_RenderDrawLine(pRenderer, uXpos + 1 + ge_uXoffs, uYpos + uButtonH + ge_uYoffs, uXpos + uButtonW + ge_uXoffs, uYpos + uButtonH + ge_uYoffs); // unten
            SDL_RenderDrawLine(pRenderer, uXpos + uButtonW + ge_uXoffs, uYpos + 1 + ge_uYoffs, uXpos + uButtonW + ge_uXoffs, uYpos + uButtonH + ge_uYoffs); // rechts
            // Dunkel-Grauen Button-Rand unten und rechts zeichnen
            SDL_SetRenderDrawColor(pRenderer,64 * fIntensityProzent,64 * fIntensityProzent,64 * fIntensityProzent, SDL_ALPHA_OPAQUE);  // Farbe für Line setzen
            SDL_RenderDrawLine(pRenderer, uXpos + 1 + ge_uXoffs, uYpos + uButtonH + 1 + ge_uYoffs, uXpos + uButtonW + ge_uXoffs, uYpos + uButtonH + 1 + ge_uYoffs); // unten
            SDL_RenderDrawLine(pRenderer, uXpos + uButtonW + 1 + ge_uXoffs, uYpos + 1 + ge_uYoffs, uXpos + uButtonW + 1 + ge_uXoffs, uYpos + uButtonH + 1 + ge_uYoffs); // rechts
            SDL_SetRenderDrawColor(pRenderer,0,0,0, SDL_ALPHA_OPAQUE);  // Farbe auf schwarz zurücksetzen
            nErrorCode = PrintLittleFont(pRenderer,uXpos + 4,uYpos + 2,1,Buttons[I].pszText,K_RELATIVE);
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
Seiteneffekte: Buttons[].x, InputStates.x, g_uIntensityProzent, ge_uXoffs, ge_uYoffs
------------------------------------------------------------------------------*/
int ShowOtherButtons(SDL_Renderer *pRenderer) {
    int nErrorCode;
    uint32_t I;
    uint32_t uXpos;
    uint32_t uYpos;
    uint32_t uButtonW;
    uint32_t uButtonH;
    float fIntensityProzent;

    fIntensityProzent = (float)g_uIntensityProzent / 100;
    nErrorCode = 0;
    for (I = 0; (I < MAX_BUTTONS) && (nErrorCode == 0); I++) {
        if ((Buttons[I].pszLabel != NULL) && (Buttons[I].pszText != NULL) && (Buttons[I].bActive) && (!Buttons[I].bPressed)) { // Nicht den gedrückten Button anzeigen
            uXpos = Buttons[I].uXpos;
            uYpos = Buttons[I].uYpos;
            uButtonW = strlen(Buttons[I].pszText) * FONT_LITTLE_559_W + FONT_LITTLE_559_W;
            uButtonH = BUTTON_H;
            // Buttonfläche erzeugen
            nErrorCode = CopyColorRect(pRenderer,212 * fIntensityProzent,208 * fIntensityProzent,200 * fIntensityProzent,uXpos,uYpos,uButtonW,uButtonH,K_RELATIVE);
            // Weißer Button-Rand oben und links zeichnen
            SDL_SetRenderDrawColor(pRenderer,255 * fIntensityProzent,255 * fIntensityProzent,255 * fIntensityProzent, SDL_ALPHA_OPAQUE);  // Farbe für Line setzen
            SDL_RenderDrawLine(pRenderer, uXpos + ge_uXoffs, uYpos + ge_uYoffs, uXpos + uButtonW + 1 + ge_uXoffs, uYpos + ge_uYoffs); // oben
            SDL_RenderDrawLine(pRenderer, uXpos + ge_uXoffs, uYpos + ge_uYoffs, uXpos + ge_uXoffs, uYpos + uButtonH + 1 + ge_uYoffs); // links
            // Grauen Button-Rand unten und rechts zeichnen
            SDL_SetRenderDrawColor(pRenderer,128 * fIntensityProzent,128 * fIntensityProzent,128 * fIntensityProzent, SDL_ALPHA_OPAQUE);  // Farbe für Line setzen
            SDL_RenderDrawLine(pRenderer, uXpos + 1 + ge_uXoffs, uYpos + uButtonH + ge_uYoffs, uXpos + uButtonW + ge_uXoffs, uYpos + uButtonH + ge_uYoffs); // unten
            SDL_RenderDrawLine(pRenderer, uXpos + uButtonW + ge_uXoffs, uYpos + 1 + ge_uYoffs, uXpos + uButtonW + ge_uXoffs, uYpos + uButtonH + ge_uYoffs); // rechts
            // Dunkel-Grauen Button-Rand unten und rechts zeichnen
            SDL_SetRenderDrawColor(pRenderer,64 * fIntensityProzent,64 * fIntensityProzent,64 * fIntensityProzent, SDL_ALPHA_OPAQUE);  // Farbe für Line setzen
            SDL_RenderDrawLine(pRenderer, uXpos + 1 + ge_uXoffs, uYpos + uButtonH + 1 + ge_uYoffs, uXpos + uButtonW + ge_uXoffs, uYpos + uButtonH + 1 + ge_uYoffs); // unten
            SDL_RenderDrawLine(pRenderer, uXpos + uButtonW + 1 + ge_uXoffs, uYpos + 1 + ge_uYoffs, uXpos + uButtonW + 1 + ge_uXoffs, uYpos + uButtonH + 1 + ge_uYoffs); // rechts
            SDL_SetRenderDrawColor(pRenderer,0,0,0, SDL_ALPHA_OPAQUE);  // Farbe auf schwarz zurücksetzen
            nErrorCode = PrintLittleFont(pRenderer,uXpos + 4,uYpos + 2,1,Buttons[I].pszText,K_RELATIVE);
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           IsButtonPressed
------------------------------------------------------------------------------
Beschreibung:  Prüft, ob ein Button gedrückt ist.
Parameter
      Eingang: pszLabel, char *, Zeiger auf Label des Buttons
      Ausgang: -
Rückgabewert:  bool, true = Button gedrückt, sonst nicht bzw. Fehler
Seiteneffekte: Buttons[].x
------------------------------------------------------------------------------*/
bool IsButtonPressed(char *pszLabel) {
    uint32_t I;
    int nErrorCode;
    bool bPressed;

    nErrorCode = -1;
    bPressed = false;
    if (pszLabel != NULL) {
        I = 0;
        do {
            if (Buttons[I].pszLabel != NULL) {
                if (strcmp(pszLabel,Buttons[I].pszLabel) == 0) {
                    bPressed = Buttons[I].bPressed;
                    nErrorCode = 0;
                }
            }
            I++;
        } while ((I < MAX_BUTTONS) && (nErrorCode == -1));
    }
    if (nErrorCode != 0) {
        SDL_Log("%s: button not found",__FUNCTION__);
    }
    return bPressed;
}
