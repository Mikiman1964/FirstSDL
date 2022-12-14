#include <unistd.h>
#include <SDL2/SDL.h>
#include "mySDL.h"
#include "mystd.h"
#include "externalpointer.h" // f?r die einzubindenen Objektdateien (Grafiken, Sounds)

int g_nGfxCount = 0;         // gefundenen Grafiken
SDL_Texture **g_pTextures;   // Pointer Array f?r Texturen

extern SDL_DisplayMode ge_DisplayMode;
extern uint8_t _binary_gfx_bin_start;extern uint8_t _binary_gfx_bin_end;
extern uint32_t Gfx[];


/*----------------------------------------------------------------------------
Name:           InitSDL_Window
------------------------------------------------------------------------------
Beschreibung: Initialisiert die SDL-Umgebung und erzeugt ein Fenster.
Parameter
      Eingang: nWindowW, int, Breite des Fensters in Pixeln
               nWindowH, int, H?he des Fensters in Pixeln
               pszWindowTitle, const char *, Zeiger auf Text f?r Fenster-Titel
      Ausgang: -
R?ckgabewert:  SDL_Window * , Zeiger auf Fenster-Handle, NULL = Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
SDL_Window *InitSDL_Window(int nWindowW, int nWindowH, const char *pszWindowTitle) {
    SDL_Window *pWindow = NULL;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) == 0) {
        pWindow = SDL_CreateWindow(
                  pszWindowTitle,             // window title
                  SDL_WINDOWPOS_UNDEFINED,    // initial x position
                  SDL_WINDOWPOS_UNDEFINED,    // initial y position
                  nWindowW,                   // width, in pixels
                  nWindowH,                   // height, in pixels
                  SDL_WINDOW_OPENGL           // flags
        );
        if (pWindow == NULL) {
            SDL_Log("%s: SDL_CreateWindow() failed: %s",__FUNCTION__,SDL_GetError());
        }
    }
    return pWindow;
}


/*----------------------------------------------------------------------------
Name:           GetDisplayMode
------------------------------------------------------------------------------
Beschreibung: Ermittelt den aktuellen Anzeigemodus mit Fensterposition.
              Das Mous wird in ge_DisplayMode zur?ckgegeben.
Parameter
      Eingang: pWindow, SDL_Window *, Fenster-Handle
      Ausgang: npWindowPosX, int *, aktuelle X-Fensterposition
               npWindowPosY, int *, aktuelle Y-Fensterposition
R?ckgabewert:  int, 0 = Modus konnte ermittelt werden, sonst nicht
Seiteneffekte: ge_DisplayMode
------------------------------------------------------------------------------*/
int GetDisplayMode(SDL_Window *pWindow, int *pnWindowPosX, int *pnWindowPosY) {
    int nRet;

    nRet = -1;
    if ((pWindow != NULL) && (pnWindowPosX != NULL) && (pnWindowPosY != NULL)) {
        *pnWindowPosX = 0;
        *pnWindowPosY = 0;
        if (SDL_GetCurrentDisplayMode(0, &ge_DisplayMode) == 0) {
            SDL_GetWindowPosition(pWindow,pnWindowPosX,pnWindowPosY);   // ist void-Funktion
            SDL_Log("%s   Display width: %03d   Display height: %d     WindowsPosX: %d    WindowsPosY: %d    Refreshrate: %d",
                    __FUNCTION__,ge_DisplayMode.w,ge_DisplayMode.h,*pnWindowPosX,*pnWindowPosY,ge_DisplayMode.refresh_rate);
            nRet = 0;
        } else {
            SDL_Log("%s: SDL_GetCurrentDisplayMode failed: %s",__FUNCTION__,SDL_GetError());
        }
        // Das Schlimmste verhindern
        if ( (nRet == -1) || (ge_DisplayMode.refresh_rate == 0) ) {
            ge_DisplayMode.refresh_rate = 60;
            *pnWindowPosX = 0;
            *pnWindowPosY = 0;
        }
    } else {
        SDL_Log("%s: null pointer found",__FUNCTION__);
    }
    return nRet;
}


/*----------------------------------------------------------------------------
Name:           CreateRenderer
------------------------------------------------------------------------------
Beschreibung: Erzeugt einen Renderer f?r ein bestimmtes Fenster.
Parameter
      Eingang: pWindow, SDL_Window, Zeiger auf Fenster-Handle
      Ausgang: -
R?ckgabewert:  SDL_Renderer *, Zeiger auf neuen Renderer, NULL = Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
SDL_Renderer *CreateRenderer(SDL_Window * pWindow) {
    SDL_Renderer *pRenderer = NULL;

    pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (pRenderer != NULL) {
        // Set the blend mode used for drawing operations (Fill and Line).
        // Anmerkung: Der eingestellte Modus wirkt nur auf Fl?chen (Fill) oder Linien (Line), die auf
        // bestehende Grafiken dr?ber gemalt werden. Da die Copper-Linien (siehe copper.c und main.c) ganz
        // "unten" liegen, hat der BlendMode dort keinen Einfluss.
        // Falls das Setzen des BlendModes fehlt schl?gt, wird trotzdem weiter gemacht.
        // M?gliche Blendmodi:
        // SDL_BLENDMODE_NONE
        // SDL_BLENDMODE_BLEND
        // SDL_BLENDMODE_ADD
        // SDL_BLENDMODE_MOD
        if (SDL_SetRenderDrawBlendMode(pRenderer,SDL_BLENDMODE_BLEND) != 0) {
            SDL_Log("%s: Warning: SDL_SetRenderDrawBlendMode() failed: %s",__FUNCTION__,SDL_GetError());
        }
    } else {
        SDL_Log("%s: SDL_CreateRenderer() failed: %s",__FUNCTION__,SDL_GetError());
    }
    return pRenderer;
}


/*----------------------------------------------------------------------------
Name:           GetTextureByIndex
------------------------------------------------------------------------------
Beschreibung: Holt den Pointer eines bereits erzeugten Texture anhand
              eines Index. Der Index ergibt sich durch das Array g_pGfxPointer[]
              (siehe dort).
              Die Funktion LoadTextures() muss bereits erfolgreich
              durchlaufen worden sein.
Parameter
      Eingang: Index, int, Index
      Ausgang: -
      R?ckgabewert:   SDL_Texture *, NULL = Fehler, sonst Zeiger auf Texture
Seiteneffekte:  g_pTextures[], g_nGfxCount
------------------------------------------------------------------------------*/
SDL_Texture *GetTextureByIndex(int nIndex) {
    if ((nIndex >= 0) && (nIndex < g_nGfxCount)) {  // deckt auch den Fall g_nGfxCount = 0 ab.
        return g_pTextures[nIndex];
    } else {
        return NULL;
    }
}


/*----------------------------------------------------------------------------
Name:           LoadTextures
------------------------------------------------------------------------------
Beschreibung: Erzeugt f?r allen Grafiken die Textures.

Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
      R?ckgabewert:   int, 0 = alles OK, -1 = Fehler
Seiteneffekte:  Gfx[], g_pTextures, g_nGfxCount
------------------------------------------------------------------------------*/
int LoadTextures(SDL_Renderer *pRenderer) {
    int nCount;                 // Anzahl vorhandener Grafiken
    int nI;
    int nGfxSize;               // Gr??e der Bitmap in Bytes
    int nErrorCode;
    bool bFound;
    bool bOK;
    uint8_t *pStartGfxPacket;   // Start-Pointer des Grafikpaketes
    uint8_t *pEndGfxPacket;     // Endpointer des Grafikpaketes
    uint8_t *pStartGfx;         // Start-Pointer der Einzelgrafik
    void *pTextures = NULL;
    uint32_t uOffset;           // Offset innerhalb des Grafikpaketes, wo Grafik zu finden ist
    uint32_t uLen;              // L?nge der Einzelgrafik

    SDL_RWops* pSDLStreamPointer; // Zeigt auf Grafikanfang
    SDL_Surface *pSurface;      // Surface
    SDL_Texture *pTexture;      // Texture

    pStartGfxPacket = &_binary_gfx_bin_start;
    pEndGfxPacket = &_binary_gfx_bin_end;
    nGfxSize = pEndGfxPacket - pStartGfxPacket;
    SDL_Log("%s: gfx packet size: %d Bytes",__FUNCTION__,nGfxSize);

    // Zun?chst z?hlen, wieviele Grafiken vorhanden sind
    nErrorCode = -1;
    nCount = 0;
    nI = 0;
    do {
        uOffset = Gfx[nI];
        uLen = Gfx[nI + 1];
        if ( (uOffset != 0xFFFFFFFF) && (uLen != 0xFFFFFFFF) ) {
            bFound = true;
            nCount++;
        } else {
            bFound = false;
        }
        nI = nI + 2;
    } while (bFound);
    bOK = (nCount > 0);
    if (bOK) {
        SDL_Log("%s: found %d gfx.",__FUNCTION__,nCount);
        // Speicher f?r Texturen erzeugen
        pTextures = malloc(sizeof(SDL_Texture*) * nCount);
        if (pTextures == NULL) {
          bOK = false;
        }
        // Speicher OK ?
        if (bOK) {
            g_pTextures = (SDL_Texture**)pTextures;   // Texturen
            g_nGfxCount = nCount;
            for (nI = 0; (nI < nCount) && (bOK); nI++) {
                pStartGfx = pStartGfxPacket + Gfx[nI * 2 + 0];  // Offset innerhalb des Paketes dazu addieren
                nGfxSize =  Gfx[nI * 2 + 1];
                //SDL_Log("%s: nI = %d   nGfxSize: %d",__FUNCTION__,nI,nGfxSize);
                pSDLStreamPointer = SDL_RWFromMem((void*)pStartGfx,nGfxSize);// Erzeugt SDL-Speicherstruktur f?r Speicher (Stream)
                if (pSDLStreamPointer != NULL) {
                    pSurface = SDL_LoadBMP_RW(pSDLStreamPointer,1);              // Surface aus Stream erzeugen, gibt Speicher f?r Stream frei
                    if (pSurface != NULL) {
                        if (SDL_SetColorKey(pSurface, SDL_TRUE, SDL_MapRGB(pSurface->format,255,0,255)) == 0) { // violett als transparent
                            pTexture = SDL_CreateTextureFromSurface(pRenderer,pSurface);   // Texture aus Surface erzeugen
                            g_pTextures[nI] = pTexture;
                            SDL_FreeSurface(pSurface);  // Surface wird nicht mehr ben?tigt
                            if (pTexture == NULL) {
                              SDL_Log("%s: SDL_CreateTextureFromSurface() failed: %s   nI = %d",__FUNCTION__,SDL_GetError(),nI);
                              bOK = false;
                            }
                        } else {
                            SDL_Log("%s: SDL_SetColorKey() failed: %s   nI = %d",__FUNCTION__,SDL_GetError(),nI);
                            bOK = false;
                        }
                    } else {
                        SDL_Log("%s: SDL_LoadBMP_RW() failed: %s   nI = %d",__FUNCTION__,SDL_GetError(),nI);
                        bOK = false;
                    }
                } else {
                    SDL_Log("%s: SDL_RWFromMem() failed: %s   nI = %d",__FUNCTION__,SDL_GetError(),nI);
                    bOK = false;
                }
            }
        }
        if (bOK) {
            nErrorCode = 0;
        }
    }
    if (!bOK) {
        SAFE_FREE(pTextures);
        g_pTextures = NULL;
        g_nGfxCount = 0;
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           CopyTexture
------------------------------------------------------------------------------
Beschreibung: Kopiert eine Texture in den Renderer.

Parameter
      Eingang: SDL_Renderer *, pRenderer, Zeiger auf Renderer
               uTextureIndex, uint32_t, Textureindex (siehe externalpointer.h)
               nXpos, int, X-Position im Render wo Texture positioniert werden soll
               nYpos, int, Y-Position im Render wo Texture positioniert werden soll
               uTextureW, uint32_t, Breite der Texture
               uTextureH, uint32_t, H?he der Texture
               fScaleW, float, Breiten-Skalierung
               fScaleH, float, H?hen-Skalierung
               fAngle, float, Rotationswinkel in Grad
      Ausgang: -
      R?ckgabewert:   int, 0 = alles OK, -1 = Fehler
Seiteneffekte:  -
------------------------------------------------------------------------------*/
int CopyTexture(SDL_Renderer *pRenderer, uint32_t uTextureIndex, int nXpos, int nYpos, uint32_t uTextureW, uint32_t uTextureH, float fScaleW, float fScaleH, float fAngle) {
    int nErrorCode = 0;
    SDL_Rect DestR;

    DestR.x = nXpos;
    DestR.y = nYpos;
    DestR.w = uTextureW * fScaleW;
    DestR.h = uTextureH * fScaleH;
    if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&DestR,fAngle,NULL, SDL_FLIP_NONE) != 0) {
        SDL_Log("%s: SDL_RenderCopyEx() failed: %s",__FUNCTION__,SDL_GetError());
        nErrorCode = -1;
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           CopyColorRect
------------------------------------------------------------------------------
Beschreibung: Kopiert eine gef?lltes Rechteck  in den Renderer.

Parameter
      Eingang: SDL_Renderer *, pRenderer, Zeiger auf Renderer
               nRed, int, Rotanteil f?r Rechteck (0 - 255)
               nGreen, int, Gr?nanteil f?r Rechteck (0 - 255)
               nBlue, int, Blauanteil f?r Rechteck (0 - 255)
               nXpos, int, X-Position im Render wo Rechteck positioniert werden soll
               nYpos, int, Y-Position im Render wo Rechteck positioniert werden soll
               uW, uint32_t, Breite des Rechtecks in Pixeln
               uH, uint32_t, H?he des Rechtecks in Pixeln
      Ausgang: -
      R?ckgabewert:   int, 0 = alles OK, sonst Fehler
Seiteneffekte:  -
------------------------------------------------------------------------------*/
int CopyColorRect(SDL_Renderer *pRenderer, int nRed, int nGreen, int nBlue, int nXpos, int nYpos, uint32_t uW, uint32_t uH) {
    int nErrorCode;
    SDL_Rect rect;

    rect.x = nXpos;
    rect.y = nYpos;
    rect.w = uW;
    rect.h = uH;
    if (SDL_SetRenderDrawColor(pRenderer,nRed,nGreen,nBlue, SDL_ALPHA_OPAQUE) == 0){  // Farbe f?r Rechteck setzen
        if (SDL_RenderFillRect(pRenderer,&rect) != 0) {
            nErrorCode = -1;
            SDL_Log("%s: SDL_RenderFillRect() failed: %s",__FUNCTION__,SDL_GetError());
        } else {
            nErrorCode = 0;
        }
    } else {
        nErrorCode = -1;
        SDL_Log("%s: SDL_SetRenderDrawColor() failed: %s",__FUNCTION__,SDL_GetError());
    }

    // Anschlie?end wieder auf schwarz stellen
    if (nErrorCode == 0) {
        if (SDL_SetRenderDrawColor(pRenderer,0,0,0, SDL_ALPHA_OPAQUE) != 0) {
            nErrorCode = -1;
            SDL_Log("%s: SDL_SetRenderDrawColor() failed: %s",__FUNCTION__,SDL_GetError());
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           FreeTextures
------------------------------------------------------------------------------
Beschreibung: Gibt die modulglobalen Texturen frei.

Parameter
      Eingang: -
      Ausgang: -
      R?ckgabewert: -
Seiteneffekte:  g_pTextures, g_nGfxCount
------------------------------------------------------------------------------*/
void FreeTextures(void) {
    int nI;

    if ( (g_nGfxCount > 0) && (g_pTextures != NULL) ) {
        for (nI = 0; nI < g_nGfxCount; nI++) {
            if (g_pTextures[nI] != NULL) {
                SDL_DestroyTexture(g_pTextures[nI]);
            }
        }
        // Pointer-Arrays freigeben
        SAFE_FREE(g_pTextures);
    }
}


/*----------------------------------------------------------------------------
Name:           SetAllTextureColors
------------------------------------------------------------------------------
Beschreibung: Die Helligkeit aller vorhandenen Texturen kann festgelegt werden.

Parameter
      Eingang: uIntensityProzent, uint8_t, 0 bis 100 % f?r Helligkeit
      Ausgang: -
      R?ckgabewert: 0 = OK, sonst Fehler
Seiteneffekte: g_nGfxCount
------------------------------------------------------------------------------*/
int SetAllTextureColors(uint8_t uIntensityProzent) {
    int nErrorCode;
    int nTexture;
    Uint8 uIntensity;

    if (uIntensityProzent > 100) {
        uIntensityProzent = 100;
    }
    uIntensity = (255 * uIntensityProzent) / 100;
    nErrorCode = 0;
    for (nTexture = 0; (nTexture < g_nGfxCount) && (nErrorCode == 0); nTexture++) {
        nErrorCode = SDL_SetTextureColorMod(GetTextureByIndex(nTexture),uIntensity,uIntensity,uIntensity);
        if (nErrorCode != 0) {
            SDL_Log("%s: SDL_SetTextureColorMod() failed: %s   nTexture = %d",__FUNCTION__,SDL_GetError(),nTexture);
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           PrintLittleFont
------------------------------------------------------------------------------
Beschreibung: Schreibt einen Text mit dem "kleinen Zeichensatz" in den Renderer.
              Die Funktion ber?cksichtigt auch Zeilenumbr?che mit "\n".
              Der Zeichensatz besitzt nur Gro?buchstaben, Kleinbuchstaben werden
              daher gewandelt.

Parameter
      Eingang: SDL_Renderer *, pRenderer, Zeiger auf Renderer
               nXpos, int, Start-X-Position der oberen linke Ecke des Textfeldes
               nYpos, int, Start-Y-Position der oberen linke Ecke des Textfeldes
               uFont, uint32_t, Zeichensatzes
                        0 = Texture 347 - LittleFont_Green.bmp
                        1 = Texture 559 - Font_8_15_Courier_transp
               pszText, char *, Zeiger auf Text, der mit Stringende abgeschlossen sein muss.
      Ausgang: -
      R?ckgabewert: 0 = OK, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int PrintLittleFont(SDL_Renderer *pRenderer, int nXpos, int nYpos, uint32_t uFont, char *pszText) {
    // Der komplette Zeichensatz liegt in Texture 347 vor. F?r ein Darstellung eines Zeichens, muss die "richtige" Stelle ausgew?hlt werden.
    // Der Zeichensatz ist so aufgebaut, dass alle vorhandenen Zeichen in einer Zeile vorliegen.
    int nErrorCode;
    uint32_t I;                         // Index auf Text
    SDL_Rect SrcR;                      // Quellbereich aus Texture 347
    SDL_Rect DestR;                     // Zielbereich, zum Kopieren in den Renderer
    uint8_t cSign;
    int nPrintXpos;
    int nPrintYpos;
    float fSizeFactor = 1;
    uint32_t uFontW;
    uint32_t uFontH;
    uint32_t uTextureIndex;

    if (uFont > 1) {
        uFont = 1;
    }
    if (uFont == 0) {
        uFontW = FONT_LITTLE_347_W;
        uFontH = FONT_LITTLE_347_H;
        uTextureIndex = 347;
    } else {
        uFontW = FONT_LITTLE_559_W;
        uFontH = FONT_LITTLE_559_H;
        uTextureIndex = 559;
    }
    nPrintXpos = nXpos;
    nPrintYpos = nYpos;
    if ((pRenderer != NULL) && (pszText != NULL)) {
        nErrorCode = 0;
        I = 0;
        while ((nErrorCode == 0) && (pszText[I] != 0)) {
            cSign = 0xFF;
            if (uFont == 0) {
                // Kleinbuchstaben wandeln
                if ((pszText[I] >= 'a') && (pszText[I] <= 'z')) {
                    cSign = pszText[I] - 64;
                } else if (pszText[I] == 0x0A) {
                    nPrintXpos = nXpos;
                    nPrintYpos = nPrintYpos + uFontH * fSizeFactor;
                    cSign = 0xFF;
                } else if ((pszText[I] >= ' ') && (pszText[I] <= 'Z')) {
                    cSign = pszText[I] - 32;
                }
            } else {
                if ((pszText[I] >= 32) && (pszText[I] <= 125)) {
                    cSign = pszText[I] - 32;
                } else if (pszText[I] == 0x0A) {
                    nPrintXpos = nXpos;
                    nPrintYpos = nPrintYpos + uFontH * fSizeFactor;
                    cSign = 0xFF;
                } else {
                    cSign = 0xFF;
                }
            }
            if (cSign != 0xFF) {
                // Quellbereich aus Texture berechnen
                SrcR.x =  (uint32_t)(cSign) * uFontW;
                SrcR.y =  0;        // Ist immer 0, da alle vorhandenen Zeichen in einer Zeile vorliegen
                SrcR.w = uFontW;
                SrcR.h = uFontH;
                // Zielbereich im Renderer
                DestR.x = nPrintXpos;
                DestR.y = nPrintYpos;
                DestR.w = uFontW * fSizeFactor;
                DestR.h = uFontH * fSizeFactor;
                if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),&SrcR,&DestR,0,NULL, SDL_FLIP_NONE) != 0) {
                    SDL_Log("%s: SDL_RenderCopyEx() failed: %s",__FUNCTION__,SDL_GetError());
                    nErrorCode = -1;
                }
                // X-Position f?r n?chstes Zeichen erh?hen
                nPrintXpos = nPrintXpos + uFontW * fSizeFactor;
            }
            I++;
        }
    } else {
        SDL_Log("%s: bad input parameter, null pointer found.",__FUNCTION__);
        nErrorCode = -1;
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetMessageWindowSize
------------------------------------------------------------------------------
Beschreibung: Berechnet anhand eines Textes die Fenstergr??e (Breite u. H?he)
              Die Funktion ber?cksichtigt auch Zeilenumbr?che mit "\n".

Parameter
      Eingang: pszText, char *, Zeiger auf Text, der mit Stringende abgeschlossen sein muss.
      Ausgang: puWinW, uint32_t *, Fensterbreite in Elementen
               puWinH, uint32_t *, Fensterh?he in Elementen
               puLines, uint32_t *, Anzahl Text-Zeilen
      R?ckgabewert: -
Seiteneffekte: -
------------------------------------------------------------------------------*/
void GetMessageWindowSize(uint32_t *puWinW,uint32_t *puWinH, uint32_t *puLines, char *pszText) {
    uint32_t uCharsInLine;
    uint32_t uXmax;
    uint32_t I;
    uint32_t uWinW;
    uint32_t uWinH;

    uXmax = 0;
    uCharsInLine = 0;
    *puLines = 0;
    uWinW = 0;
    uWinH = 0;
    I = 0;
    // Fensterbreite anhand des Textes berechnen
    while (pszText[I] != 0) {
        if (pszText[I] == 0x0A) {
            uWinH++;
            if (uCharsInLine > uXmax) {
                uXmax = uCharsInLine;
            }
            uCharsInLine = 0;
        } else {
            if (uWinH == 0) {
                uWinH++;
            }
            uCharsInLine++;
        }
        I++;
    }
    if (uCharsInLine > uXmax) {
        uXmax = uCharsInLine;
    }
    *puLines = uWinH;
    uWinW = ((uXmax * FONT_LITTLE_347_W) / (FONT_W / 2));
    if ( ((uXmax * FONT_LITTLE_347_W) % (FONT_W / 2)) != 0) {
        uWinW++;
    }
    uWinW = uWinW + 3;  // Seitenst?cke dazu
    if ( ((uWinH * FONT_LITTLE_347_H) % (FONT_H / 2)) == 0) {
        uWinH = ((uWinH * FONT_LITTLE_347_H) / (FONT_H / 2));
    } else {
        uWinH = ((uWinH * FONT_LITTLE_347_H) / (FONT_H / 2)) + 1;
    }
    *puWinW = uWinW;
    *puWinH = uWinH;
}


/*----------------------------------------------------------------------------
Name:           CreateMessageWindow
------------------------------------------------------------------------------
Beschreibung: Erzeugt ein Fenster mit Text.
              Die Funktion ber?cksichtigt auch Zeilenumbr?che mit "\n".
              Der Zeichensatz besitzt nur Gro?buchstaben, Kleinbuchstaben werden
              daher gewandelt.

Parameter
      Eingang: SDL_Renderer *, pRenderer, Zeiger auf Renderer
               nXpos, int, Start-X-Position der oberen linke Ecke des Fensters, -1 = auf Fenster horizontal zentrieren
               nYpos, int, Start-Y-Position der oberen linke Ecke des Textfeldes, -1 = auf Fenster vertikal zentrieren
               uColor, uint32_t, Farbe des Zeichensatzes, wird noch nicht ber?cksichtigt
               pszText, char *, Zeiger auf Text, der mit Stringende abgeschlossen sein muss.
      Ausgang: -
      R?ckgabewert: 0 = OK, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int CreateMessageWindow(SDL_Renderer *pRenderer, int nXpos, int nYpos, uint32_t uColor, char *pszText) {
    int nErrorCode;
    uint32_t X,Y;
    uint32_t uWinW;                     // ben?tigte Fensterbreite in Elementen
    uint32_t uWinH;                     // ben?tigte Fensterh?he in Elementen
    SDL_Rect DestR;                     // Zielbereich, zum Kopieren in den Renderer
    uint32_t uTextureIndex;
    uint32_t uLines;
    int nXoffset;
    int nPrintXpos;
    int nPrintYpos;

    if ((pRenderer != NULL) && (pszText != NULL)) {
        nErrorCode = 0;
        GetMessageWindowSize(&uWinW,&uWinH,&uLines,pszText);
        // Zentrierung
        if (nXpos == -1) {      // horizontal zentrieren?
            nXpos = (WINDOW_W - (uWinW * FONT_W / 2)) / 2;
        }
        if (nYpos == - 1) {     // vertikal zentrieren?
            nYpos = (WINDOW_H - (uWinH * FONT_H / 2)) / 2;
        }
        nPrintXpos = nXpos;
        nPrintYpos = nYpos;
        // Oberste Zeile des Fensters zeichnen
        for (X = 0; X < uWinW && (nErrorCode == 0); X++) {
            if (X == 0) {
                uTextureIndex = 348;
            } else if ((X + 1) >= uWinW) {
                uTextureIndex = 350;
            } else {
                uTextureIndex = 349;
            }
            DestR.x = nPrintXpos;
            DestR.y = nPrintYpos;
            DestR.w = FONT_W / 2;
            DestR.h = FONT_H / 2;
            if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&DestR,0,NULL, SDL_FLIP_NONE) != 0) {
                SDL_Log("%s: SDL_RenderCopyEx() failed: %s",__FUNCTION__,SDL_GetError());
                nErrorCode = -1;
            }
            nPrintXpos = nPrintXpos + FONT_W / 2;
        }
        nPrintXpos = nXpos;
        nPrintYpos = nPrintYpos + FONT_H / 2;
        // Seitenw?nde des Fensters zeichnen
        for (Y = 0; Y < uWinH && (nErrorCode == 0); Y++) {
            DestR.x = nPrintXpos;
            DestR.y = nPrintYpos;
            DestR.w = FONT_W / 2;
            DestR.h = FONT_H / 2;
            if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(351),NULL,&DestR,0,NULL, SDL_FLIP_NONE) != 0) {
                SDL_Log("%s: SDL_RenderCopyEx() failed: %s",__FUNCTION__,SDL_GetError());
                nErrorCode = -1;
            }
            DestR.x = nPrintXpos + (uWinW - 1) * (FONT_W / 2);
            DestR.y = nPrintYpos;
            DestR.w = FONT_W / 2;
            DestR.h = FONT_H / 2;
            if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(352),NULL,&DestR,0,NULL, SDL_FLIP_NONE) != 0) {
                SDL_Log("%s: SDL_RenderCopyEx() failed: %s",__FUNCTION__,SDL_GetError());
                nErrorCode = -1;
            }
            nPrintYpos = nPrintYpos + FONT_H / 2;
        }
        // Unterste Zeile des Fensters zeichnen
        nPrintXpos = nXpos;
        for (X = 0; X < uWinW && (nErrorCode == 0); X++) {
            if (X == 0) {
                uTextureIndex = 353;
            } else if ((X + 1) >= uWinW) {
                uTextureIndex = 355;
            } else {
                uTextureIndex = 354;
            }
            DestR.x = nPrintXpos;
            DestR.y = nPrintYpos;
            DestR.w = FONT_W / 2;
            DestR.h = FONT_H / 2;
            if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),NULL,&DestR,0,NULL, SDL_FLIP_NONE) != 0) {
                SDL_Log("%s: SDL_RenderCopyEx() failed: %s",__FUNCTION__,SDL_GetError());
                nErrorCode = -1;
            }
            nPrintXpos = nPrintXpos + FONT_W / 2;
        }
        if (nErrorCode == 0) {
            // Fensterfl?che ausf?llen
            DestR.x = nXpos + FONT_W / 2;
            DestR.y = nYpos + FONT_H / 2;
            DestR.w = (uWinW - 2) * FONT_W / 2;
            DestR.h = uWinH * FONT_H / 2;
            SDL_SetRenderDrawColor(pRenderer,0x20,0x20,0xF0,0xC0);  // dunkelblaue, halbtransparente Fensterfl?che
            nErrorCode = SDL_RenderFillRect(pRenderer,&DestR);
            if (nErrorCode == 0) {
                nXoffset = ((uWinH * (FONT_H / 2)) - (uLines * FONT_LITTLE_347_H)) / 2;
                //SDL_Log("Lines: %u    WinH: %u    XOffset: %d",uLines,uWinH,nXoffset);
                nErrorCode = PrintLittleFont(pRenderer, nXpos + (FONT_W / 2) + 8, nYpos + (FONT_H / 2) + nXoffset, 0,pszText);
            } else {
                SDL_Log("%s: SDL_RenderFillRect() failed: %s",__FUNCTION__,SDL_GetError());
            }
            SDL_SetRenderDrawColor(pRenderer,0,0,0,SDL_ALPHA_OPAQUE);
        }
    } else {
        SDL_Log("%s: bad input parameter, null pointer found.",__FUNCTION__);
        nErrorCode = -1;
    }
    return nErrorCode;
}
