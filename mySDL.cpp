#include <unistd.h>
#include <SDL2/SDL.h>
#include "mySDL.h"
#include "mystd.h"
#include "externalpointer.h" // für die einzubindenen Objektdateien (Grafiken, Sounds)

int g_nGfxCount = 0;         // gefundenen Grafiken
SDL_Texture **g_pTextures;   // Pointer Array für Texturen

extern SDL_DisplayMode ge_DisplayMode;

/*----------------------------------------------------------------------------
Name:           InitSDL_Window
------------------------------------------------------------------------------
Beschreibung: Initialisiert die SDL-Umgebung und erzeugt ein Fenster.
Parameter
      Eingang: nWindowW, int, Breite des Fensters in Pixeln
               nWindowH, int, Höhe des Fensters in Pixeln
               pszWindowTitle, const char *, Zeiger auf Text für Fenster-Titel
      Ausgang: -
Rückgabewert:  SDL_Window * , Zeiger auf Fenster-Handle, NULL = Fehler
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
              Das Mous wird in ge_DisplayMode zurückgegeben.
Parameter
      Eingang: pWindow, SDL_Window *, Fenster-Handle
      Ausgang: npWindowPosX, int *, aktuelle X-Fensterposition
               npWindowPosY, int *, aktuelle Y-Fensterposition
Rückgabewert:  int, 0 = Modus konnte ermittelt werden, sonst nicht
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
Beschreibung: Erzeugt einen Renderer für ein bestimmtes Fenster.
Parameter
      Eingang: pWindow, SDL_Window, Zeiger auf Fenster-Handle
      Ausgang: -
Rückgabewert:  SDL_Renderer *, Zeiger auf neuen Renderer, NULL = Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
SDL_Renderer *CreateRenderer(SDL_Window * pWindow) {
    SDL_Renderer *pRenderer = NULL;

    pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (pRenderer == NULL) {
        SDL_Log("%s: SDL_CreateWindow() failed: %s",__FUNCTION__,SDL_GetError());
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
      Rückgabewert:   SDL_Texture *, NULL = Fehler, sonst Zeiger auf Texture
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
Beschreibung: Erzeugt für allen Grafiken die Textures.

Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
      Rückgabewert:   int, 0 = alles OK, -1 = Fehler
Seiteneffekte:  g_pGfxPointer[], g_pTextures, g_nGfxCount
------------------------------------------------------------------------------*/
int LoadTextures(SDL_Renderer *pRenderer) {
    int nCount;         // Anzahl vorhandener Grafiken
    int nI;
    int nGfxSize;       // Größe der Bitmap in Bytes
    int nErrorCode;
    bool bFound;
    bool bOK;
    uint8_t *pStartGfx;  // Start-Pointer der Grafik
    uint8_t *pEndGfx;    // Endpointer-Pointer der Grafik
    void *pTextures = NULL;
    SDL_RWops* pSDLStreamPointer; // Zeigt auf Grafikanfang
    SDL_Surface *pSurface;     // Surface
    SDL_Texture *pTexture;   // Texture

    // Zunächst zählen, wieviele Grafiken vorhanden sind
    nErrorCode = -1;
    nCount = 0;
    nI = 0;
    do {
        pStartGfx = g_pGfxPointer[nI + 0];
        pEndGfx =  g_pGfxPointer[nI + 1];
        if ( (pStartGfx != NULL) && (pEndGfx != NULL) ) {
            nGfxSize = pEndGfx - pStartGfx;
            if (nGfxSize > 0) {
                bOK = true;
                bFound = true;
                nCount++;
            } else {
                bOK = false;
                bFound = false;
                SDL_Log("%s: bad gfx size: %d",__FUNCTION__,nGfxSize);
            }
        } else {
            bFound = false;
        }
        nI = nI + 2;
    } while (bOK && bFound);
    bOK = ((bOK) && (nCount > 0));
    if (bOK) {
        SDL_Log("%s: found %d gfx.",__FUNCTION__,nCount);
        // Speicher für Texturen erzeugen
        pTextures = malloc(sizeof(SDL_Texture*) * nCount);
        if (pTextures == NULL) {
          bOK = false;
        }
        // Speicher OK ?
        if (bOK) {
            g_pTextures = (SDL_Texture**)pTextures;   // Texturen
            g_nGfxCount = nCount;
            for (nI = 0; (nI < nCount) && (bOK); nI++) {
                pStartGfx = g_pGfxPointer[nI * 2 + 0];
                pEndGfx =  g_pGfxPointer[nI * 2 + 1];
                nGfxSize = pEndGfx - pStartGfx;
                //SDL_Log("%s: nI = %d   nGfxSize: %d",__FUNCTION__,nI,nGfxSize);
                pSDLStreamPointer = SDL_RWFromMem((void*)g_pGfxPointer[nI * 2],nGfxSize);// Erzeugt SDL-Speicherstruktur für Speicher (Stream)
                if (pSDLStreamPointer != NULL) {
                    pSurface = SDL_LoadBMP_RW(pSDLStreamPointer,1);              // Surface aus Stream erzeugen, gibt Speicher für Stream frei
                    if (pSurface != NULL) {
                        if (SDL_SetColorKey(pSurface, SDL_TRUE, SDL_MapRGB(pSurface->format,255,0,255)) == 0) { // violett als transparent
                            pTexture = SDL_CreateTextureFromSurface(pRenderer,pSurface);   // Texture aus Surface erzeugen
                            g_pTextures[nI] = pTexture;
                            SDL_FreeSurface(pSurface);  // Surface wird nicht mehr benötigt
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
               uTextureH, uint32_t, Höhe der Texture
               fScaleW, float, Breiten-Skalierung
               fScaleH, float, Höhen-Skalierung
               fAngle, float, Rotationswinkel in Grad
      Ausgang: -
      Rückgabewert:   int, 0 = alles OK, -1 = Fehler
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
Beschreibung: Kopiert eine gefülltes Rechteck  in den Renderer.

Parameter
      Eingang: SDL_Renderer *, pRenderer, Zeiger auf Renderer
               nRed, int, Rotanteil für Rechteck (0 - 255)
               nGreen, int, Grünanteil für Rechteck (0 - 255)
               nBlue, int, Blauanteil für Rechteck (0 - 255)
               nXpos, int, X-Position im Render wo Rechteck positioniert werden soll
               nYpos, int, Y-Position im Render wo Rechteck positioniert werden soll
               uW, uint32_t, Breite des Rechtecks in Pixeln
               uH, uint32_t, Höhe des Rechtecks in Pixeln
      Ausgang: -
      Rückgabewert:   int, 0 = alles OK, sonst Fehler
Seiteneffekte:  -
------------------------------------------------------------------------------*/
int CopyColorRect(SDL_Renderer *pRenderer, int nRed, int nGreen, int nBlue, int nXpos, int nYpos, uint32_t uW, uint32_t uH) {
    int nErrorCode;
    SDL_Rect rect;

    rect.x = nXpos;
    rect.y = nYpos;
    rect.w = uW;
    rect.h = uH;
    if (SDL_SetRenderDrawColor(pRenderer,nRed,nGreen,nBlue, SDL_ALPHA_OPAQUE) == 0){  // Farbe für Line setzen
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

    // Anschließend wieder auf schwarz stellen
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
      Rückgabewert: -
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
      Eingang: uIntensityProzent, uint8_t, 0 bis 100 % für Helligkeit
      Ausgang: -
      Rückgabewert: 0 = OK, sonst Fehler
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

