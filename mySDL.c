#include <unistd.h>
#include <SDL2/SDL.h>
#include "config.h"
#include "loadlevel.h"
#include "miniz.h"
#include "mySDL.h"
#include "mystd.h"
#include "externalpointer.h" // für die einzubindenen Objektdateien (Grafiken, Sounds)

int g_nGfxCount = 0;         // gefundenen Grafiken
uint8_t g_uIntensityProzent = 100;
SDL_Texture **g_pTextures;   // Pointer Array für Texturen
USABLEDISPLAYMODES UsableDisplayModes;
SHOWABLEDISPLAYMODES ShowableDisplayModes;

extern SDL_DisplayMode ge_DisplayMode;
extern SDL_Window *ge_pWindow;
extern uint8_t _binary_gfx_compressed_bin_start;extern uint8_t _binary_gfx_compressed_bin_end;
extern uint32_t Gfx[];
extern CONFIG Config;
extern uint32_t ge_uXoffs;             // X-Offset für die Zentrierung von Elementen
extern uint32_t ge_uYoffs;             // X-Offset für die Zentrierung von Elementen

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
Seiteneffekte: ShowableDisplayModes.x, Config.x, ge_uXoffs, ge_uYoffs
------------------------------------------------------------------------------*/
SDL_Window *InitSDL_Window(int nWindowW, int nWindowH, const char *pszWindowTitle) {
    SDL_Window *pWindow = NULL;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) == 0) {
        if (GetDesktopDisplayMode() == 0) { // Stellt auch die Anzahl der Displays fest und das zu verwendene Display (Config.uDisplayUse)
            if (GetUsableDisplayModes(Config.uDisplayUse) == 0) {
                // Falls die Konfigurationsdatei von einem anderen Rechner übernommen wurde, kann es sein, dass die eingestellte Fenstergröße,
                // die maximal Unterstützte überschreitet.
                if ((nWindowW > ShowableDisplayModes.nW[0]) || (nWindowH > ShowableDisplayModes.nH[0])) {
                    SDL_Log("%s: adjust config resolution from %u x %u to %u x %u",__FUNCTION__,nWindowW,nWindowH,ShowableDisplayModes.nW[0],ShowableDisplayModes.nH[0]);
                    // korrigieren
                    nWindowW = ShowableDisplayModes.nW[0];
                    nWindowH = ShowableDisplayModes.nH[0];
                    Config.uResX = nWindowW;
                    Config.uResY = nWindowH;
                    ge_uXoffs = (Config.uResX - DEFAULT_WINDOW_W) / 2;
                    ge_uYoffs = (Config.uResY - DEFAULT_WINDOW_H) / 2;
                    if (WriteConfigFile() != 0) {
                        return NULL;
                    }
                }
                pWindow = SDL_CreateWindow(
                          pszWindowTitle,             // window title
                          SDL_WINDOWPOS_CENTERED_DISPLAY(Config.uDisplayUse),    // Falls Display 1 nicht vorhanden, wird ohne Fehler 0 ausgewählt
                          SDL_WINDOWPOS_CENTERED_DISPLAY(Config.uDisplayUse),    // Falls Display 1 nicht vorhanden, wird ohne Fehler 0 ausgewählt
                          nWindowW,                   // width, in pixels
                          nWindowH,                   // height, in pixels
                          0                           // Flags
                );
                if (pWindow == NULL) {
                    SDL_Log("%s: SDL_CreateWindow() failed: %s",__FUNCTION__,SDL_GetError());
                }
            }
        }
    }
    return pWindow;
}


/*----------------------------------------------------------------------------
Name:           GetDesktopDisplayMode
------------------------------------------------------------------------------
Beschreibung: Ermittelt den aktuellen Desktop-Anzeigemodus und die Fensterposition.
              Das Modus wird in ge_DisplayMode zurückgegeben.
              Zusätzlich wird die Anzahl der verfügbaren Displays ermittelt
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int, 0 = Modus konnte ermittelt werden, sonst nicht
Seiteneffekte: ge_DisplayMode, Config.x
------------------------------------------------------------------------------*/
int GetDesktopDisplayMode(void) {
    int nRet;
    int nDisplays;

    nDisplays = SDL_GetNumVideoDisplays();
    SDL_Log("%s: Displays: %d",__FUNCTION__,nDisplays);
    // Falls in der Konfiguration eine zu hohe Displaynummer eingestellt ist, wird hier korrigiert
    if (nDisplays >= 1) {
        if (Config.uDisplay > (nDisplays - 1)) {
            Config.uDisplayUse = nDisplays - 1;
            if (Config.uDisplayUse > 1) {   // Es wird primary (0) und secondary (1) Display unterstützt
                Config.uDisplayUse = 1;
            }
            SDL_Log("%s: display: %u is not available ... use display: %u",__FUNCTION__,Config.uDisplay,Config.uDisplayUse);
        }
        nRet = SDL_GetDesktopDisplayMode(Config.uDisplayUse,&ge_DisplayMode);
        if (nRet == 0) {
            SDL_Log("%s: display: %u   w: %03d   h: %d   refreshrate: %d",__FUNCTION__,Config.uDisplayUse,ge_DisplayMode.w,ge_DisplayMode.h,ge_DisplayMode.refresh_rate);
        } else {
            SDL_Log("%s: SDL_GetDesktopDisplayMode failed: %s",__FUNCTION__,SDL_GetError());
        }
    } else {
        // Es gibt kein Display !
        SDL_Log("%s: no display found",__FUNCTION__);
        nRet = -1;
    }
    // Das Schlimmste verhindern
    if ( (nRet != 0) || (ge_DisplayMode.refresh_rate == 0) ) {
        ge_DisplayMode.refresh_rate = 60;
    }
    return nRet;
}


/*----------------------------------------------------------------------------
Name:           RestoreDesktop
------------------------------------------------------------------------------
Beschreibung: Stellt die ursprüngliche Desktop-Auflösung wieder her.
              Unter Ubuntu 16.04 (vielleicht auch bei anderen Linuxen) ist es wichtig, nach
              Ausschalten des Fullscreens wieder die ursprüngliche Desktop-Auflösung herzustellen.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: ge_pWindow, ge_DisplayMode
------------------------------------------------------------------------------*/
void RestoreDesktop(void) {
    SDL_SetWindowSize(ge_pWindow,ge_DisplayMode.w,ge_DisplayMode.h);
    SDL_SetWindowFullscreen(ge_pWindow,0); //  0 = Fullscreen  ausschalten
}


/*----------------------------------------------------------------------------
Name:           CenterWindow
------------------------------------------------------------------------------
Beschreibung: Zentriert das Fenster (ge_pWindow) auf dem Desktop.


Parameter
      Eingang: uWidth, uint32_t, Fensterbreite
               uHeight, uint32_t, Fensterhöhe
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: ge_pWindow, ge_DisplayMode (Desktop-Größe)
------------------------------------------------------------------------------*/
int CenterWindow(uint32_t uWidth, uint32_t uHeight) {
    int nErrorCode = -1;
    int x,y;

    if ((ge_pWindow != NULL) && (ge_DisplayMode.w > 0) && (ge_DisplayMode.h > 0)) {

        // X-Zentrierung
        if (ge_DisplayMode.w > uWidth) {
            x = (ge_DisplayMode.w - uWidth) / 2;
        } else {
            x = 0;
        }
        // Y-Zentrierung
        if (ge_DisplayMode.h > uHeight) {
            y = (ge_DisplayMode.h - uHeight) / 2;
        } else {
            y = 0;
        }
        SDL_SetWindowPosition(ge_pWindow,x,y); // Ist erst in SDL3 eine int-Funktion
        nErrorCode = 0;
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetUsableDisplayModes
------------------------------------------------------------------------------
Beschreibung: Ermittelt alle nutzbaren/brauchbaren Display-Einstellungen.
                    Nutzbar ist wie folgt definiert:
                    * Xres >= 1280
                    * Yres >= 768
                    * Refreshrate: 60 Hz
                    * 24 Bit Farbtiefe
              Vor Aufruf dieser Funktion muss das Video-Subsystem (SDL_Init)
              bereits initialisiert worden sein.
Parameter
      Eingang: uDisplay, uint32_t, Display (0 = primary oder 1 = secondary), welches genutzt werden soll
      Ausgang: -
Rückgabewert:  int , 0 = Alles OK, sonst Fehler
Seiteneffekte: UsableDisplayModes.x, ShowableDisplayModes.x
------------------------------------------------------------------------------*/
int GetUsableDisplayModes(uint32_t uDisplay) {
    int nModeIndex;
    int nDisplayModeCount;
    SDL_DisplayMode DisplayMode;
    Uint32 uFormat;

    UsableDisplayModes.nDisplayModeCount = 0;
    nDisplayModeCount = SDL_GetNumDisplayModes(uDisplay);
    if (nDisplayModeCount < 1) {
        SDL_Log("%s: SDL_GetNumDisplayModes failed: %s",__FUNCTION__,SDL_GetError());
        return -1;
    }
    for (nModeIndex = 0; (nModeIndex < nDisplayModeCount) && (UsableDisplayModes.nDisplayModeCount < MAX_USABLE_DISPLAYMODES); nModeIndex++) {
        if (SDL_GetDisplayMode(uDisplay,nModeIndex,&DisplayMode) != 0) {
            SDL_Log("%s: SDL_GetDisplayMode failed: %s",__FUNCTION__,SDL_GetError());
            return -1;
        }
        uFormat = DisplayMode.format;
        if ((DisplayMode.refresh_rate == 60) &&
            (DisplayMode.w >= DEFAULT_WINDOW_W) &&
            (DisplayMode.h >= DEFAULT_WINDOW_H) &&
            (SDL_BITSPERPIXEL(uFormat) == 24)) {
            UsableDisplayModes.nW[UsableDisplayModes.nDisplayModeCount] = DisplayMode.w;
            UsableDisplayModes.nH[UsableDisplayModes.nDisplayModeCount] = DisplayMode.h;
            UsableDisplayModes.nModeIndex[UsableDisplayModes.nDisplayModeCount] = nModeIndex;
            UsableDisplayModes.nDisplayModeCount++;
        }
    }
    // Bis hier ist alles gut gegangen -> ShowableDisplayModes.x befüllen
    //SDL_Log("%s: usable display modes: %d",__FUNCTION__,UsableDisplayModes.nDisplayModeCount);
    /*
    for (int I = 0; I < UsableDisplayModes.nDisplayModeCount; I++) {
        SDL_Log("%04d X %04d",UsableDisplayModes.nW[I],UsableDisplayModes.nH[I]);
    }
    */
    return GetShowableDisplayModes();
}


/*----------------------------------------------------------------------------
Name:           GetShowableDisplayModes
------------------------------------------------------------------------------
Beschreibung: Ermittelt aus den nutzbaren/brauchbaren Display-Einstellungen
              eine Anzeigeliste. Ggf. müssen einige Displaymodes verworfen
              werden, da die Anzeigeliste z.Z. begrenzt ist. (22 Einträge)

              Vor Aufruf dieser Funktion muss das Video-Subsystem (SDL_Init)
              bereits initialisiert und die Struktur
              UsableDisplayModes.x durch die Funktion GetUsableDisplayModes()
              befüllt worden sein.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int , 0 = Alles OK, sonst Fehler
Seiteneffekte: UsableDisplayModes.x, ShowableDisplayModes.x
------------------------------------------------------------------------------*/
int GetShowableDisplayModes(void) {
    int nErrorCode = -1;
    int I;
    int S;
    int nToChoose;
    int nDistance;  // Sprungabstand aus Usable-Liste ab Index 1
    int nW;
    int nH;

    if (UsableDisplayModes.nDisplayModeCount > 0) {
        nErrorCode = 0;
        // Können alle brauchbaren Modi in die Anzeigeliste übernommen werden?
        if (UsableDisplayModes.nDisplayModeCount <= MAX_SHOWABLE_DISPLAYMODES) {
            ShowableDisplayModes.nDisplayModeCount = 0;
            for (I = 0; I < UsableDisplayModes.nDisplayModeCount; I++) {
                ShowableDisplayModes.nW[I] = UsableDisplayModes.nW[I];
                ShowableDisplayModes.nH[I] = UsableDisplayModes.nH[I];
                ShowableDisplayModes.nModeIndex[I] = UsableDisplayModes.nModeIndex[I];
            }
            ShowableDisplayModes.nDisplayModeCount = UsableDisplayModes.nDisplayModeCount;
        } else {
            ShowableDisplayModes.nW[0] = UsableDisplayModes.nW[0];
            ShowableDisplayModes.nH[0] = UsableDisplayModes.nH[0];
            ShowableDisplayModes.nModeIndex[0] = UsableDisplayModes.nModeIndex[0];  // Displaymode mit der höchsten Auflösung übernehmen
            ShowableDisplayModes.nDisplayModeCount = 1;
            nToChoose = MAX_SHOWABLE_DISPLAYMODES - 2;
            nDistance = (UsableDisplayModes.nDisplayModeCount - 2) / nToChoose;
            I = 1;  // Index für Usable
            S = 1;  // Index für Showable
            while ((nToChoose > 0) && (S < MAX_SHOWABLE_DISPLAYMODES) && (I < MAX_USABLE_DISPLAYMODES)) {
                nW = UsableDisplayModes.nW[I];
                nH = UsableDisplayModes.nH[I];
                ShowableDisplayModes.nW[S] = nW;
                ShowableDisplayModes.nH[S] = nH;
                ShowableDisplayModes.nModeIndex[S] = UsableDisplayModes.nModeIndex[I];
                S++;
                ShowableDisplayModes.nDisplayModeCount++;
                I = I + nDistance;
                nToChoose--;
            }
            if (S < MAX_SHOWABLE_DISPLAYMODES) {
                ShowableDisplayModes.nW[S] = UsableDisplayModes.nW[UsableDisplayModes.nDisplayModeCount - 1];  // Displaymode mit der niedrigsten Auflösung übernehmen
                ShowableDisplayModes.nH[S] = UsableDisplayModes.nH[UsableDisplayModes.nDisplayModeCount - 1];  // Displaymode mit der niedrigsten Auflösung übernehmen
                ShowableDisplayModes.nModeIndex[S] = UsableDisplayModes.nModeIndex[UsableDisplayModes.nDisplayModeCount - 1];  // Displaymode mit der niedrigsten Auflösung übernehmen
                ShowableDisplayModes.nDisplayModeCount++;
            } else {
                ShowableDisplayModes.nW[MAX_SHOWABLE_DISPLAYMODES - 1] = UsableDisplayModes.nW[UsableDisplayModes.nDisplayModeCount - 1];  // Displaymode mit der niedrigsten Auflösung übernehmen
                ShowableDisplayModes.nH[MAX_SHOWABLE_DISPLAYMODES - 1] = UsableDisplayModes.nH[UsableDisplayModes.nDisplayModeCount - 1];  // Displaymode mit der niedrigsten Auflösung übernehmen
                ShowableDisplayModes.nModeIndex[MAX_SHOWABLE_DISPLAYMODES - 1] = UsableDisplayModes.nModeIndex[UsableDisplayModes.nDisplayModeCount - 1];  // Displaymode mit der niedrigsten Auflösung übernehmen
                // Letzter Eintrag wird überschrieben, daher ShowableDisplayModes.nDisplayModeCount nicht erhöhen
            }
        }
    }
    return nErrorCode;
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
    if (pRenderer != NULL) {
        // Set the blend mode used for drawing operations (Fill and Line).
        // Anmerkung: Der eingestellte Modus wirkt nur auf Flächen (Fill) oder Linien (Line), die auf
        // bestehende Grafiken drüber gemalt werden. Da die Copper-Linien (siehe copper.c und main.c) ganz
        // "unten" liegen, hat der BlendMode dort keinen Einfluss.
        // Falls das Setzen des BlendModes fehlt schlägt, wird trotzdem weiter gemacht.
        // Mögliche Blendmodi:
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
Seiteneffekte:  Gfx[], g_pTextures, g_nGfxCount
------------------------------------------------------------------------------*/
int LoadTextures(SDL_Renderer *pRenderer) {
    int nCount;                 // Anzahl vorhandener Grafiken
    int nI;
    int nGfxSize;               // Größe der Bitmap in Bytes
    int nErrorCode;
    bool bFound;
    bool bOK;
    uint8_t *pStartGfxPacket = NULL;   // Start-Pointer des dekomprimierten Grafikpaketes
    uint8_t *pStartGfx;         // Start-Pointer der Einzelgrafik
    void *pTextures = NULL;
    uint32_t uOffset;           // Offset innerhalb des Grafikpaketes, wo Grafik zu finden ist
    uint32_t uLen;              // Länge der Einzelgrafik
    uint32_t uCompressedSize;
    uint32_t uUnCompressedSize;
    SDL_RWops* pSDLStreamPointer; // Zeigt auf Grafikanfang
    SDL_Surface *pSurface;      // Surface
    SDL_Texture *pTexture;      // Texture
    uint8_t *pStartCompressedGfx;
    int nMiniz;

    pStartCompressedGfx = &_binary_gfx_compressed_bin_start;
    uCompressedSize = &_binary_gfx_compressed_bin_end - &_binary_gfx_compressed_bin_start - 4;  // -4, da am Anfang die unkomprimierte Größe eingetragen wurde
    // SDL_Log("%s: compressed gfx packet size: %u",__FUNCTION__,uCompressedSize);
    uUnCompressedSize = *(uint32_t*)pStartCompressedGfx;
    SDL_Log("%s: gfx packet size: %d Bytes / compressed: %d Bytes",__FUNCTION__,uUnCompressedSize,uCompressedSize);
    pStartGfxPacket = malloc(uUnCompressedSize);
    if (pStartGfxPacket == NULL) {
        SDL_Log("%s: can not allocate memory for decompressed graphics (%u bytes)",__FUNCTION__,uUnCompressedSize);
        return -1;
    }
    nMiniz = mz_uncompress(pStartGfxPacket,(mz_ulong*)&uUnCompressedSize,pStartCompressedGfx + 4,(mz_ulong)uCompressedSize);
    if (nMiniz != MZ_OK) {
        SDL_Log("%s: can not decompress graphics, error: %d",__FUNCTION__,nMiniz);
        SAFE_FREE(pStartGfxPacket);
        return -1;
    }
    // Zunächst zählen, wieviele Grafiken vorhanden sind
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
                pStartGfx = pStartGfxPacket + Gfx[nI * 2 + 0];  // Offset innerhalb des Paketes dazu addieren
                nGfxSize = Gfx[nI * 2 + 1];
                //SDL_Log("%s: nI = %d   nGfxSize: %d",__FUNCTION__,nI,nGfxSize);
                pSDLStreamPointer = SDL_RWFromMem((void*)pStartGfx,nGfxSize);// Erzeugt SDL-Speicherstruktur für Speicher (Stream)
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
    SAFE_FREE(pStartGfxPacket);
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           CopyTexture
------------------------------------------------------------------------------
Beschreibung: Kopiert eine Texture in den Renderer.

Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
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
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
               nRed, int, Rotanteil für Rechteck (0 - 255)
               nGreen, int, Grünanteil für Rechteck (0 - 255)
               nBlue, int, Blauanteil für Rechteck (0 - 255)
               nXpos, int, X-Position im Render wo Rechteck positioniert werden soll
               nYpos, int, Y-Position im Render wo Rechteck positioniert werden soll
               uW, uint32_t, Breite des Rechtecks in Pixeln
               uH, uint32_t, Höhe des Rechtecks in Pixeln
               bAbsolute, bool, true = absolute Koordinaten, d.h. es erfolgt keinte Umrechnung
      Ausgang: -
      Rückgabewert:   int, 0 = alles OK, sonst Fehler
Seiteneffekte:  ge_uXoffs, ge_uYoffs
------------------------------------------------------------------------------*/
int CopyColorRect(SDL_Renderer *pRenderer, int nRed, int nGreen, int nBlue, int nXpos, int nYpos, uint32_t uW, uint32_t uH, bool bAbsolute) {
    int nErrorCode;
    SDL_Rect rect;

    if (bAbsolute) {
        rect.x = nXpos;
        rect.y = nYpos;
    } else {
        rect.x = nXpos + ge_uXoffs;
        rect.y = nYpos + ge_uYoffs;
    }
    rect.w = uW;
    rect.h = uH;
    if (SDL_SetRenderDrawColor(pRenderer,nRed,nGreen,nBlue, SDL_ALPHA_OPAQUE) == 0){  // Farbe für Rechteck setzen
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
Seiteneffekte: g_nGfxCount, g_uIntensityProzent
------------------------------------------------------------------------------*/
int SetAllTextureColors(uint8_t uIntensityProzent) {
    int nErrorCode;
    int nTexture;
    Uint8 uIntensity;

    if (uIntensityProzent > 100) {
        uIntensityProzent = 100;
    }
    g_uIntensityProzent = uIntensityProzent;      // Damit Nicht-Texturen (z.B. die Buttons) wissen, welche Helligkeit gesetzt ist
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
              Die Funktion berücksichtigt auch Zeilenumbrüche mit "\n".
              Der Zeichensatz besitzt nur Großbuchstaben, Kleinbuchstaben werden
              daher gewandelt.

Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
               nXpos, int, Start-X-Position der oberen linke Ecke des Textfeldes
               nYpos, int, Start-Y-Position der oberen linke Ecke des Textfeldes
               uFont, uint32_t, Zeichensatzes
                        0 = Texture 347 - LittleFont_Green.bmp
                        1 = Texture 559 - Font_8_15_Courier_transp
                        2 = Texture 347 - LittleFont_Red.bmp
                        3 = Texture 70  - LittleFont_Black.bmp
               pszText, char *, Zeiger auf Text, der mit Stringende abgeschlossen sein muss.
               bAbsolute, bool, true = absolute Koordinaten, d.h. es erfolgt keinte Umrechnung
      Ausgang: -
      Rückgabewert: 0 = OK, sonst Fehler
Seiteneffekte: ge_uXoffs, ge_uYoffs
------------------------------------------------------------------------------*/
int PrintLittleFont(SDL_Renderer *pRenderer, int nXpos, int nYpos, uint32_t uFont, char *pszText, bool bAbsolute) {
    // Der komplette Zeichensatz liegt in Texture 347 vor. Für ein Darstellung eines Zeichens, muss die "richtige" Stelle ausgewählt werden.
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
    uint32_t uCharCountPerLine;

    if (uFont > 3) {
        uFont = 1;
    }
    if (uFont == 0) {
        uFontW = FONT_LITTLE_347_W;
        uFontH = FONT_LITTLE_347_H;
        uTextureIndex = 347;
    } else if (uFont == 1) {
        uFontW = FONT_LITTLE_559_W;
        uFontH = FONT_LITTLE_559_H;
        uTextureIndex = 559;
    } else if (uFont == 2) {
        uFontW = FONT_LITTLE_347_W;
        uFontH = FONT_LITTLE_347_H;
        uTextureIndex = 782;
    } else {
        uFontW = FONT_LITTLE_347_W;
        uFontH = FONT_LITTLE_347_H;
        uTextureIndex = 70;
    }
    nPrintXpos = nXpos;
    nPrintYpos = nYpos;
    if ((pRenderer != NULL) && (pszText != NULL)) {
        nErrorCode = 0;
        I = 0;
        uCharCountPerLine = 0;
        while ((nErrorCode == 0) && (pszText[I] != 0)) {
            cSign = 0xFF;
            if (uFont == 0) {
                if (pszText[I] == 0x0A) {
                    nPrintXpos = nXpos;
                    nPrintYpos = nPrintYpos + uFontH * fSizeFactor;
                    cSign = 0xFF;
                    uCharCountPerLine = 0;
                } else if ((pszText[I] >= ' ') && (pszText[I] <= 102)) {    // Cursor -> letztes Zeichen im Zeichensatz
                    cSign = pszText[I] - 32;
                    uCharCountPerLine++;
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
                if (bAbsolute) {
                    DestR.x = nPrintXpos;
                    DestR.y = nPrintYpos;
                } else {
                    DestR.x = nPrintXpos + ge_uXoffs;
                    DestR.y = nPrintYpos + ge_uYoffs;
                }
                DestR.w = uFontW * fSizeFactor;
                DestR.h = uFontH * fSizeFactor;
                if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(uTextureIndex),&SrcR,&DestR,0,NULL, SDL_FLIP_NONE) != 0) {
                    SDL_Log("%s: SDL_RenderCopyEx() failed: %s",__FUNCTION__,SDL_GetError());
                    nErrorCode = -1;
                }
                // X-Position für nächstes Zeichen erhöhen
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
Beschreibung: Berechnet anhand eines Textes die Fenstergröße (Breite u. Höhe)
              Die Funktion berücksichtigt auch Zeilenumbrüche mit "\n".

Parameter
      Eingang: pszText, char *, Zeiger auf Text, der mit Stringende abgeschlossen sein muss.
      Ausgang: puWinW, uint32_t *, Fensterbreite in Elementen
               puWinH, uint32_t *, Fensterhöhe in Elementen
               puLines, uint32_t *, Anzahl Text-Zeilen
      Rückgabewert: -
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
        if (pszText[I] == 0x0A)  {
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

    if (uXmax > EMERALD_MAX_CHARACTERS_PER_LINE) {
        uXmax = EMERALD_MAX_CHARACTERS_PER_LINE;
    }


    *puLines = uWinH;
    uWinW = ((uXmax * FONT_LITTLE_347_W) / (FONT_W / 2));
    if ( ((uXmax * FONT_LITTLE_347_W) % (FONT_W / 2)) != 0) {
        uWinW++;
    }
    uWinW = uWinW + 3;  // Seitenstücke dazu
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
              Die Funktion berücksichtigt auch Zeilenumbrüche mit "\n".
              Der Zeichensatz besitzt nur Großbuchstaben, Kleinbuchstaben werden
              daher gewandelt.

Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
               nXpos, int, Start-X-Position der oberen linke Ecke des Fensters, -1 = auf Fenster horizontal zentrieren
               nYpos, int, Start-Y-Position der oberen linke Ecke des Textfeldes, -1 = auf Fenster vertikal zentrieren
               uColor, uint32_t, Farbe des Zeichensatzes, wird noch nicht berücksichtigt
               pszText, char *, Zeiger auf Text, der mit Stringende abgeschlossen sein muss.
      Ausgang: -
      Rückgabewert: 0 = OK, sonst Fehler
Seiteneffekte: Config.x
------------------------------------------------------------------------------*/
int CreateMessageWindow(SDL_Renderer *pRenderer, int nXpos, int nYpos, uint32_t uColor, char *pszText) {
    int nErrorCode;
    uint32_t X,Y;
    uint32_t uWinW;                     // benötigte Fensterbreite in Elementen
    uint32_t uWinH;                     // benötigte Fensterhöhe in Elementen
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
            nXpos = (Config.uResX - (uWinW * FONT_W / 2)) / 2;
        }
        if (nYpos == - 1) {     // vertikal zentrieren?
            nYpos = (Config.uResY - (uWinH * FONT_H / 2)) / 2;
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
        // Seitenwände des Fensters zeichnen
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
            // Fensterfläche ausfüllen
            DestR.x = nXpos + FONT_W / 2;
            DestR.y = nYpos + FONT_H / 2;
            DestR.w = (uWinW - 2) * FONT_W / 2;
            DestR.h = uWinH * FONT_H / 2;
            SDL_SetRenderDrawColor(pRenderer,0x20,0x20,0xF0,0xC0);  // dunkelblaue, halbtransparente Fensterfläche
            nErrorCode = SDL_RenderFillRect(pRenderer,&DestR);
            if (nErrorCode == 0) {
                nXoffset = ((uWinH * (FONT_H / 2)) - (uLines * FONT_LITTLE_347_H)) / 2;
                //SDL_Log("Lines: %u    WinH: %u    XOffset: %d",uLines,uWinH,nXoffset);
                nErrorCode = PrintLittleFont(pRenderer, nXpos + (FONT_W / 2) + 8, nYpos + (FONT_H / 2) + nXoffset, 0,pszText,K_ABSOLUTE);
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


/*----------------------------------------------------------------------------
Name:           DrawBeam
------------------------------------------------------------------------------
Beschreibung: Zeichnet ein Rechteck / einen Balken für die Levelgruppen- und Namen-Auswahl.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
               uXpos, uint32_t, X-Position für Balken
               uYpos, uint32_t, Y-Position für Balken
               uWidth, uint32_t, Breite des Balkens
               uHeight, uint32_t, Höhe des Balkens
               uRed, uint8_t, Rot-Anteil für Farbe des Balkens
               uGreen, uint8_t, Grün-Anteil für Farbe des Balkens
               uBlue, uint8_t, Blau-Anteil für Farbe des Balkens
               uTransp, uint8_t, Transparenz der Farbe
               bAbsolute, bool, true = absolute Koordinaten, d.h. es erfolgt keinte Umrechnung
      Ausgang: -
Rückgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte: ge_uXoffs, ge_uYoffs
------------------------------------------------------------------------------*/
int DrawBeam(SDL_Renderer *pRenderer,uint32_t uXpos, uint32_t uYpos, uint32_t uWidth, uint32_t uHeight, uint8_t uRed, uint32_t uGreen, uint32_t uBlue, uint8_t uTransp, bool bAbsolute) {
    int nErrorCode = -1;
    SDL_Rect DestR;

    // Balken zeichnen
    if (bAbsolute) {
        DestR.x = uXpos;
        DestR.y = uYpos;
    } else {
        DestR.x = uXpos + ge_uXoffs;
        DestR.y = uYpos + ge_uYoffs;
    }
    DestR.w = uWidth;
    DestR.h = uHeight;
    if (SDL_SetRenderDrawColor(pRenderer,uRed,uGreen,uBlue,uTransp) == 0) {   // dunkelblaue, halbtransparente Fensterfläche
        if (SDL_RenderFillRect(pRenderer,&DestR) == 0) {
            nErrorCode = SDL_SetRenderDrawColor(pRenderer,0,0,0,SDL_ALPHA_OPAQUE);
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           DrawGrid
------------------------------------------------------------------------------
Beschreibung: Zeichnet ein Kreuzgitter mit anzugebender Farbe und Zeilen-/Spaltenabstand.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
               uXpos, uint32_t, X-Position für Gitter
               uYpos, uint32_t, Y-Position für Gitter
               uWidth, uint32_t, Breite des Gitters
               uHeight, uint32_t, Höhe des Gitters
               uRed, uint8_t, Rot-Anteil für Farbe des Gitters
               uGreen, uint8_t, Grün-Anteil für Farbe des Gitters
               uBlue, uint8_t, Blau-Anteil für Farbe des Gitters
               uGridSpace, uint32_t, Zeilen- und Spaltenabstand der Gitter-Linien
      Ausgang: -
Rückgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte: ge_uXoffs, ge_uYoffs
------------------------------------------------------------------------------*/
int DrawGrid(SDL_Renderer *pRenderer, uint32_t uXpos, uint32_t uYpos, uint32_t uWidth, uint32_t uHeight, uint8_t uRed, uint8_t uGreen, uint8_t uBlue, uint8_t uAlpha, uint32_t uGridSpace) {
    int nErrorCode = 0;
    uint32_t X,Y;

    nErrorCode = SDL_SetRenderDrawColor(pRenderer,uRed,uGreen,uBlue,uAlpha);
    // Vertikale Linien zeichnen
    for (X = uXpos; (X <= (uXpos + uWidth)) && (nErrorCode == 0); X = X + uGridSpace) {
        nErrorCode = SDL_RenderDrawLine(pRenderer,X + ge_uXoffs, uYpos + ge_uYoffs, X + ge_uXoffs, uYpos + uHeight + ge_uYoffs);
    }
    // Horizontale Linien zeichnen
    for (Y = uYpos; (Y <= (uYpos + uHeight)) && (nErrorCode == 0); Y = Y + uGridSpace) {
        nErrorCode = SDL_RenderDrawLine(pRenderer,uXpos + ge_uXoffs, Y + ge_uYoffs, uXpos + uWidth + ge_uXoffs, Y + ge_uYoffs);
    }
    return nErrorCode;
}
