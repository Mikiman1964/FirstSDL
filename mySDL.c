#include <stdint.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include "config.h"
#include "loadlevel.h"
#include "miniz.h"
#include "mySDL.h"
#include "mystd.h"
#include "levelconverter.h"
#include "RenderLevel.h"
#include "gfx/gfx.h"
#include "gfx/gfx_compressed.h"
#include "gfx/textures.h"

int32_t g_nGfxCount = 0;         // gefundene Grafiken
uint8_t g_uIntensityProzent = 100;
uint32_t g_LastRenderTicks;
SDL_Texture **g_pTextures;   // Pointer Array für Texturen
VIDEO Video;
extern uint32_t Gfx[];
extern CONFIG Config;


/*----------------------------------------------------------------------------
Name:           InitSDL_Window
------------------------------------------------------------------------------
Beschreibung: Initialisiert die SDL-Umgebung und erzeugt ein Fenster.
              Funktion darf nur einmalig aufgerufen werden.
Parameter
      Eingang: nWindowW, int32_t, Breite des Fensters in Pixeln
               nWindowH, int32_t, Höhe des Fensters in Pixeln
               pszWindowTitle, const char *, Zeiger auf Text für Fenster-Titel
      Ausgang: -
Rückgabewert:  SDL_Window * , Zeiger auf Fenster-Handle, NULL = Fehler
Seiteneffekte: Config.x, Video.x
------------------------------------------------------------------------------*/
SDL_Window *InitSDL_Window(int32_t nWindowW, int32_t nWindowH, const char *pszWindowTitle) {
    SDL_Window *pWindow = NULL;
    int32_t nClosestWindowSizeIndex;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) == 0) {
        if (GetDesktopDisplayMode() == 0) { // Stellt auch die Anzahl der Displays fest und das zu verwendene Display (Config.uDisplayUse)
            if (GetUsableDisplayModes(Config.uDisplayUse) == 0) {
                // Falls die Konfigurationsdatei von einem anderen Rechner übernommen wurde, kann es sein, dass die eingestellte Fenstergröße,
                // die maximal Unterstützte überschreitet oder allgemein ungültig ist.
                nClosestWindowSizeIndex = GetClosestWindowSize(nWindowW,nWindowH);
                if (nClosestWindowSizeIndex >= 0) {
                    if ((nWindowW != Video.pUsableModes[nClosestWindowSizeIndex].nW) || (nWindowH != Video.pUsableModes[nClosestWindowSizeIndex].nH)) {
                        SDL_Log("%s: adjust config resolution from %u x %u to %u x %u",__FUNCTION__,nWindowW,nWindowH,Video.pUsableModes[nClosestWindowSizeIndex].nW,Video.pUsableModes[nClosestWindowSizeIndex].nH);
                    }
                    Config.uResX = Video.pUsableModes[nClosestWindowSizeIndex].nW;
                    Config.uResY = Video.pUsableModes[nClosestWindowSizeIndex].nH;
                    Video.uXoffs = (Config.uResX - DEFAULT_WINDOW_W) / 2;
                    Video.uYoffs = (Config.uResY - DEFAULT_WINDOW_H) / 2;
                    if (WriteConfigFile() != 0) {
                        return NULL;
                    }
                }
                pWindow = SDL_CreateWindow(
                          pszWindowTitle,             // window title
                          SDL_WINDOWPOS_CENTERED_DISPLAY(Config.uDisplayUse),    // Falls Display 1 nicht vorhanden, wird ohne Fehler 0 ausgewählt
                          SDL_WINDOWPOS_CENTERED_DISPLAY(Config.uDisplayUse),    // Falls Display 1 nicht vorhanden, wird ohne Fehler 0 ausgewählt
                          Config.uResX,               // width, in pixels
                          Config.uResY,               // height, in pixels
                          0                           // Flags
                );
                if (pWindow == NULL) {
                    SDL_Log("%s: SDL_CreateWindow() failed: %s",__FUNCTION__,SDL_GetError());
                }
            } else {
                SDL_Log("%s: GetUsableDisplayModes() failed",__FUNCTION__);
            }
        } else {
            SDL_Log("%s: GetDesktopDisplayMode() failed",__FUNCTION__);
        }
    } else {
        SDL_Log("%s: SDL_Init() failed: %s",__FUNCTION__,SDL_GetError());
    }
    return pWindow;
}


/*----------------------------------------------------------------------------
Name:           GetClosestWindowSize
------------------------------------------------------------------------------
Beschreibung: Ermittelt anhand einer gegebenen Fenstergröße eine passende Fenstergröße,
              die ggf. auch im Fullscreen-Modus läuft.
              Die Auswahl einer passenden Größe erfolgt über die Liste
              Video.pUsableModes.
              Der Rückgabewert ist ein Index auf Video.pUsableModes.

              Hinweis: Video.pUsableModes ist absteigend sortiert

Parameter
      Eingang: nWindowW, int32_t, Breite des Fensters in Pixeln
               nWindowH, int32_t, Höhe des Fensters in Pixeln
      Ausgang: -
Rückgabewert:  int32_t, >=0  Modus konnte ermittelt werden, ansonsten -1 bei Fehler
Seiteneffekte: Video.x
------------------------------------------------------------------------------*/
int32_t GetClosestWindowSize(int32_t nWindowW, int32_t nWindowH) {
    uint32_t uMode;
    int32_t nIndex = -1;

    SDL_Log("%s: input: w: %d  h: %d",__FUNCTION__,nWindowW,nWindowH);
    if ((Video.nUsableDisplayModeCount > 0) && (nWindowW > 0) && (nWindowH > 0)) {
        for (uMode = Video.nUsableDisplayModeCount - 1; (uMode != -1) && (nIndex == -1); uMode--) {
            if ((Video.pUsableModes[uMode].nW >= nWindowW) && (Video.pUsableModes[uMode].nH >= nWindowH)) {
                nIndex = uMode;
            }
        }
        // Falls nichts Passendes gefunden wurde, dann die höchstmögliche Fenstergröße wählen
        if (nIndex == -1) {
            SDL_Log("%s: nothing found, use index 0",__FUNCTION__);
            nIndex = 0;
        }
    } else {
        SDL_Log("%s: no useable display mode available",__FUNCTION__);
    }
    return nIndex;
}


/*----------------------------------------------------------------------------
Name:           GetDesktopDisplayMode
------------------------------------------------------------------------------
Beschreibung: Ermittelt den aktuellen Desktop-Anzeigemodus und die Fensterposition.
              Das Modus wird in Video.DesktopDisplayMode zurückgegeben.
              Zusätzlich wird die Anzahl der verfügbaren Displays ermittelt
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int32_t, 0 = Modus konnte ermittelt werden, sonst nicht
Seiteneffekte: Video.x, Config.x
------------------------------------------------------------------------------*/
int32_t GetDesktopDisplayMode(void) {
    int32_t nRet;
    int32_t nDisplays;

    nDisplays = SDL_GetNumVideoDisplays();
    // SDL_Log("%s: Displays: %d",__FUNCTION__,nDisplays);
    // Falls in der Konfiguration eine zu hohe Displaynummer eingestellt ist, wird hier korrigiert
    if (nDisplays >= 1) {
        if (Config.uDisplay > (nDisplays - 1)) {
            Config.uDisplayUse = nDisplays - 1;
            if (Config.uDisplayUse > 1) {   // Es wird primary (0) und secondary (1) Display unterstützt
                Config.uDisplayUse = 1;
            }
            // SDL_Log("%s: display: %u is not available ... use display: %u",__FUNCTION__,Config.uDisplay,Config.uDisplayUse);
        }
        nRet = SDL_GetDesktopDisplayMode(Config.uDisplayUse,&Video.DesktopDisplayMode);
        if (nRet == 0) {
            SDL_Log("%s: display: %u   w: %03d   h: %d   refreshrate: %d",__FUNCTION__,Config.uDisplayUse,Video.DesktopDisplayMode.w,Video.DesktopDisplayMode.h,Video.DesktopDisplayMode.refresh_rate);
            Video.bMustUseFullScreen = (Video.DesktopDisplayMode.refresh_rate != 60);
            //Video.bMustUseFullScreen = false;
        } else {
            SDL_Log("%s: SDL_GetDesktopDisplayMode failed: %s",__FUNCTION__,SDL_GetError());
        }
    } else {
        // Es gibt kein Display !
        SDL_Log("%s: no display found",__FUNCTION__);
        nRet = -1;
    }
    // Das Schlimmste verhindern
    if ( (nRet != 0) || (Video.DesktopDisplayMode.refresh_rate == 0) ) {
        Video.DesktopDisplayMode.refresh_rate = 60;
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
Seiteneffekte: Video.x
------------------------------------------------------------------------------*/
void RestoreDesktop(void) {
    SDL_SetWindowSize(Video.pWindow,Video.DesktopDisplayMode.w,Video.DesktopDisplayMode.h);
    SDL_SetWindowFullscreen(Video.pWindow,0); //  0 = Fullscreen  ausschalten
}


/*----------------------------------------------------------------------------
Name:           SetWindowDisplayMode
------------------------------------------------------------------------------
Beschreibung: Setzt den Display-Modus für ein Fenster, das sich im Vollbild-Modus
              befindet.


Parameter
      Eingang: nWidth, int32_t, Display-Breite
               nHeight, int32_t, Display-Höhe
               uFormat, Uint32, Pixelformat, z.B. SDL_PIXELFORMAT_RGB888
               nRefreshrate, int32_t, Bildwiederholrate
      Ausgang: -
Rückgabewert:  int32_t , 0 = Alles OK, sonst Fehler
Seiteneffekte: Video.x für Fenster-Handle
------------------------------------------------------------------------------*/
int32_t SetWindowDisplayMode(int32_t nWidth, int32_t nHeight, Uint32 uFormat, int32_t nRefreshRate) {
    SDL_DisplayMode NewDisplayMode;
    int32_t nErrorCode;

    NewDisplayMode.format = uFormat;
    NewDisplayMode.w = nWidth;
    NewDisplayMode.h = nHeight;
    NewDisplayMode.refresh_rate = nRefreshRate;
    NewDisplayMode.driverdata = NULL;
    nErrorCode = SDL_SetWindowDisplayMode(Video.pWindow,&NewDisplayMode); // anwenden, um sicher zu stellen, dass richtige Framerate läuft
    if (nErrorCode != 0) {
        SDL_Log("%s: SDL_SetWindowDisplayMode failed: %s",__FUNCTION__,SDL_GetError());
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           CenterWindow
------------------------------------------------------------------------------
Beschreibung: Zentriert das Fenster (Video.pWindow) auf dem Desktop.


Parameter
      Eingang: uWidth, uint32_t, Fensterbreite
               uHeight, uint32_t, Fensterhöhe
      Ausgang: -
Rückgabewert:  int32_t, 0 = Alles OK, sonst Fehler
Seiteneffekte: Video.x für Desktop-Größe
------------------------------------------------------------------------------*/
int32_t CenterWindow(uint32_t uWidth, uint32_t uHeight) {
    int32_t nErrorCode = -1;
    int32_t x,y;

    if ((Video.pWindow != NULL) && (Video.DesktopDisplayMode.w > 0) && (Video.DesktopDisplayMode.h > 0)) {
        // X-Zentrierung
        if (Video.DesktopDisplayMode.w > uWidth) {
            x = (Video.DesktopDisplayMode.w - uWidth) / 2;
        } else {
            x = 0;
        }
        // Y-Zentrierung
        if (Video.DesktopDisplayMode.h > uHeight) {
            y = (Video.DesktopDisplayMode.h - uHeight) / 2;
        } else {
            y = 0;
        }
        SDL_SetWindowPosition(Video.pWindow,x,y); // Ist erst in SDL3 eine int-Funktion
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
Rückgabewert:  int32_t, 0 = Alles OK, sonst Fehler
Seiteneffekte: ShowableDisplayModes.x, Video.x
------------------------------------------------------------------------------*/
int32_t GetUsableDisplayModes(uint32_t uDisplay) {
    int32_t nModeIndex;
    int32_t nDisplayModeCount;
    SDL_DisplayMode DisplayMode;
    Uint32 uFormat;

    Video.nUsableDisplayModeCount = 0;
    nDisplayModeCount = SDL_GetNumDisplayModes(uDisplay);
    if (nDisplayModeCount < 1) {
        SDL_Log("%s: SDL_GetNumDisplayModes failed: %s",__FUNCTION__,SDL_GetError());
        return -1;
    }
    for (nModeIndex = 0; (nModeIndex < nDisplayModeCount); nModeIndex++) {
        if (SDL_GetDisplayMode(uDisplay,nModeIndex,&DisplayMode) != 0) {
            SDL_Log("%s: SDL_GetDisplayMode failed: %s",__FUNCTION__,SDL_GetError());
            return -1;
        }
        uFormat = DisplayMode.format;
        if ((DisplayMode.refresh_rate == 60) && (DisplayMode.w >= DEFAULT_WINDOW_W) && (DisplayMode.h >= DEFAULT_WINDOW_H) && (SDL_BITSPERPIXEL(uFormat) == 24)) {
            Video.nUsableDisplayModeCount++;
            Video.pUsableModes = realloc(Video.pUsableModes,Video.nUsableDisplayModeCount * sizeof(Video));
            if (Video.pUsableModes != NULL) {
                Video.pUsableModes[Video.nUsableDisplayModeCount - 1].nW = DisplayMode.w;
                Video.pUsableModes[Video.nUsableDisplayModeCount - 1].nH = DisplayMode.h;
                Video.pUsableModes[Video.nUsableDisplayModeCount - 1].nModeIndex = nModeIndex;
            } else {
                SDL_Log("%s: realloc() failed:",__FUNCTION__);
                return -1;
            }
        }
    }
    // Bis hier ist alles gut gegangen -> ShowableDisplayModes.x befüllen
    /*
    SDL_Log("%s: UsableDisplayModeCount: %d",__FUNCTION__,Video.nUsableDisplayModeCount);
    for (nModeIndex = 0; nModeIndex < Video.nUsableDisplayModeCount; nModeIndex++) {
        SDL_Log("%s: Index: %02u    XRES: %04d    YRES: %04d",__FUNCTION__,Video.pUsableModes[nModeIndex].nModeIndex,Video.pUsableModes[nModeIndex].nW,Video.pUsableModes[nModeIndex].nH);
    }
    */
    if (Video.nUsableDisplayModeCount > 0) {
        return GetShowableDisplayModes();
    } else {
		// nDisplayModeCount ist hier mindestens 1
		// SDL2 hat bei bestimmten Umgebungen Probleme die DisplayModes zu bestimmen (bei HDMI-Anschluss?)
		// In diesem Fall Minimal-Konfiguration anbieten
        Video.pUsableModes = realloc(Video.pUsableModes,sizeof(Video));
        if (Video.pUsableModes != NULL) {
            Video.pUsableModes[0].nW = DEFAULT_WINDOW_W;
            Video.pUsableModes[0].nH = DEFAULT_WINDOW_H;
            Video.pUsableModes[0].nModeIndex = 0;
            Video.nUsableDisplayModeCount = 1;
            return GetShowableDisplayModes();
        } else {
            SDL_Log("%s: realloc() failed:",__FUNCTION__);
            return -1;
        }
    }
}


/*----------------------------------------------------------------------------
Name:           GetShowableDisplayModes
------------------------------------------------------------------------------
Beschreibung: Ermittelt aus den nutzbaren/brauchbaren Display-Einstellungen
              eine Anzeigeliste. Ggf. müssen einige Displaymodes verworfen
              werden, da die Anzeigeliste z.Z. begrenzt ist. (22 Einträge)

              Vor Aufruf dieser Funktion muss das Video-Subsystem (SDL_Init)
              bereits initialisiert und die Struktur
              Video.x durch die Funktion GetUsableDisplayModes()
              befüllt worden sein.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int32_t, 0 = Alles OK, sonst Fehler
Seiteneffekte: Video.x
------------------------------------------------------------------------------*/
int32_t GetShowableDisplayModes(void) {
    int32_t nErrorCode = -1;
    int32_t nI;
    int32_t nS;
    int32_t nToChoose;
    int32_t nDistance;  // Sprungabstand aus Usable-Liste ab Index 1
    int32_t nW;
    int32_t nH;

    if (Video.nUsableDisplayModeCount > 0) {
        nErrorCode = 0;
        // Können alle brauchbaren Modi in die Anzeigeliste übernommen werden?
        if (Video.nUsableDisplayModeCount <= MAX_SHOWABLE_DISPLAYMODES) {
            Video.nShowableDisplayModeCount = 0;
            for (nI = 0; nI < Video.nUsableDisplayModeCount; nI++) {
                Video.ShowableDisplayModes[nI].nW = Video.pUsableModes[nI].nW;
                Video.ShowableDisplayModes[nI].nH = Video.pUsableModes[nI].nH;
                Video.ShowableDisplayModes[nI].nModeIndex = Video.pUsableModes[nI].nModeIndex;
            }
            Video.nShowableDisplayModeCount = Video.nUsableDisplayModeCount;
        } else {
            Video.ShowableDisplayModes[0].nW = Video.pUsableModes[0].nW;
            Video.ShowableDisplayModes[0].nH = Video.pUsableModes[0].nH;
            Video.ShowableDisplayModes[0].nModeIndex = Video.pUsableModes[0].nModeIndex;  // Displaymode mit der höchsten Auflösung übernehmen
            Video.nShowableDisplayModeCount = 1;
            nToChoose = MAX_SHOWABLE_DISPLAYMODES - 2;
            nDistance = (Video.nUsableDisplayModeCount - 2) / nToChoose;
            nI = 1;  // Index für Usable
            nS = 1;  // Index für Showable
            while ((nToChoose > 0) && (nS < MAX_SHOWABLE_DISPLAYMODES) && (nI < Video.nUsableDisplayModeCount)) {
                nW = Video.pUsableModes[nI].nW;
                nH = Video.pUsableModes[nI].nH;
                Video.ShowableDisplayModes[nS].nW = nW;
                Video.ShowableDisplayModes[nS].nH = nH;
                Video.ShowableDisplayModes[nS].nModeIndex = Video.pUsableModes[nI].nModeIndex;
                nS++;
                Video.nShowableDisplayModeCount++;
                nI = nI + nDistance;
                nToChoose--;
            }
            if (nS < MAX_SHOWABLE_DISPLAYMODES) {
                Video.ShowableDisplayModes[nS].nW = Video.pUsableModes[Video.nUsableDisplayModeCount - 1].nW;  // Displaymode mit der niedrigsten Auflösung übernehmen
                Video.ShowableDisplayModes[nS].nH = Video.pUsableModes[Video.nUsableDisplayModeCount - 1].nH;  // Displaymode mit der niedrigsten Auflösung übernehmen
                Video.ShowableDisplayModes[nS].nModeIndex = Video.pUsableModes[Video.nUsableDisplayModeCount - 1].nModeIndex;  // Displaymode mit der niedrigsten Auflösung übernehmen
                Video.nShowableDisplayModeCount++;
            } else {
                Video.ShowableDisplayModes[MAX_SHOWABLE_DISPLAYMODES - 1].nW = Video.pUsableModes[Video.nUsableDisplayModeCount - 1].nW;  // Displaymode mit der niedrigsten Auflösung übernehmen
                Video.ShowableDisplayModes[MAX_SHOWABLE_DISPLAYMODES - 1].nH = Video.pUsableModes[Video.nUsableDisplayModeCount - 1].nH;  // Displaymode mit der niedrigsten Auflösung übernehmen
                Video.ShowableDisplayModes[MAX_SHOWABLE_DISPLAYMODES - 1].nModeIndex = Video.pUsableModes[Video.nUsableDisplayModeCount - 1].nModeIndex;  // Displaymode mit der niedrigsten Auflösung übernehmen
                // Letzter Eintrag wird überschrieben, daher Video.nShowableDisplayModeCount nicht erhöhen
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
Name:           RenderPresentAndClear
------------------------------------------------------------------------------
Beschreibung: Zeigt den Inhalt des Renderers an und löscht diesen anschließend.
              Wenn der Renderer mit dem Flag SDL_RENDERER_PRESENTVSYNC erstellt
              wurde, so synchronisiert diese Funktion den Programmablauf mit
              der Bildschirmwiederholfrequenz (default 60 Hz).
              Sollte die Bildschirmwiederholfrquenz deutlich zu hoch sein (> 125 Hz),
              bzw. VSYNC nicht funktionieren, dann wartet die Funktion hier eine
              entsprechende Zeit ab und begrenzt auf 8 ms = 125 Frames/sec.
              Wegen g_LastRenderTicks wäre die Funktion nicht threadsicher!

Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
      Ausgang: -
      Rückgabewert: int32_t, 0 = alles OK, -1 = Fehler
Seiteneffekte: g_LastRenderTicks
------------------------------------------------------------------------------*/
int32_t RenderPresentAndClear(SDL_Renderer *pRenderer) {   // Renderer anzeigen, lässt Hauptschleife mit ~ 60 Hz (Bild-Wiederholfrequenz) laufen
    uint32_t uDiffTicks;    // Differenz zwischen aktuellem und letzten Aufruf in Ticks
    uint32_t uTicks;

    uTicks = SDL_GetTicks();
    uDiffTicks = uTicks - g_LastRenderTicks;

    if (uDiffTicks < 8) {   // Kann auch durch Verschieben des Fensters auftreten
        SDL_Delay(8 - uDiffTicks);
        // SDL_Log("%s:DiffTicks: %u",__FUNCTION__,uDiffTicks);
    }
    g_LastRenderTicks = uTicks;
    SDL_RenderPresent(pRenderer);       // Renderer anzeigen, lässt Hauptschleife mit ~ 60 Hz (Bild-Wiederholfrequenz) laufen, hat keinen Rückgabewert
    return SDL_RenderClear(pRenderer);  // Renderer für nächstes Frame löschen
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
      Eingang: Index, int32_t, Index
      Ausgang: -
      Rückgabewert:   SDL_Texture *, NULL = Fehler, sonst Zeiger auf Texture
Seiteneffekte:  g_pTextures[], g_nGfxCount
------------------------------------------------------------------------------*/
SDL_Texture *GetTextureByIndex(int32_t nIndex) {
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
      Rückgabewert:  int32_t, 0 = alles OK, -1 = Fehler
Seiteneffekte:  Gfx[], g_pTextures, g_nGfxCount, gfx_compressed[]
------------------------------------------------------------------------------*/
int32_t LoadTextures(SDL_Renderer *pRenderer) {
    int32_t nCount;                 // Anzahl vorhandener Grafiken
    int32_t nI;
    int32_t nGfxSize;               // Größe der Bitmap in Bytes
    int32_t nErrorCode;
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
    int32_t nMiniz;

    pStartCompressedGfx = gfx_compressed;
    uCompressedSize = sizeof(gfx_compressed);
    // SDL_Log("%s: compressed gfx packet size: %u",__FUNCTION__,uCompressedSize);
    uUnCompressedSize = *(uint32_t*)pStartCompressedGfx;
    // SDL_Log("%s: gfx packet size: %d Bytes / compressed: %d Bytes",__FUNCTION__,uUnCompressedSize,uCompressedSize);
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
        // SDL_Log("%s: found %d gfx.",__FUNCTION__,nCount);
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
               nXpos, int32_t, X-Position im Render wo Texture positioniert werden soll
               nYpos, int32_t, Y-Position im Render wo Texture positioniert werden soll
               uTextureW, uint32_t, Breite der Texture
               uTextureH, uint32_t, Höhe der Texture
               fScaleW, float, Breiten-Skalierung
               fScaleH, float, Höhen-Skalierung
               fAngle, float, Rotationswinkel in Grad
      Ausgang: -
      Rückgabewert:   int32_t, 0 = alles OK, -1 = Fehler
Seiteneffekte:  -
------------------------------------------------------------------------------*/
int32_t CopyTexture(SDL_Renderer *pRenderer, uint32_t uTextureIndex, int32_t nXpos, int32_t nYpos, uint32_t uTextureW, uint32_t uTextureH, float fScaleW, float fScaleH, float fAngle) {
    int32_t nErrorCode = 0;
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
               nRed, int32_t, Rotanteil für Rechteck (0 - 255)
               nGreen, int32_t, Grünanteil für Rechteck (0 - 255)
               nBlue, int32_t, Blauanteil für Rechteck (0 - 255)
               nXpos, int32_t, X-Position im Render wo Rechteck positioniert werden soll
               nYpos, int32_t, Y-Position im Render wo Rechteck positioniert werden soll
               uW, uint32_t, Breite des Rechtecks in Pixeln
               uH, uint32_t, Höhe des Rechtecks in Pixeln
               bAbsolute, bool, true = absolute Koordinaten, d.h. es erfolgt keinte Umrechnung
      Ausgang: -
      Rückgabewert:   int32_t, 0 = alles OK, sonst Fehler
Seiteneffekte:  Video.x
------------------------------------------------------------------------------*/
int32_t CopyColorRect(SDL_Renderer *pRenderer, int32_t nRed, int32_t nGreen, int32_t nBlue, int32_t nXpos, int32_t nYpos, uint32_t uW, uint32_t uH, bool bAbsolute) {
    int32_t nErrorCode;
    SDL_Rect rect;

    if (bAbsolute) {
        rect.x = nXpos;
        rect.y = nYpos;
    } else {
        rect.x = nXpos + Video.uXoffs;
        rect.y = nYpos + Video.uYoffs;
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
    int32_t nI;

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
      Rückgabewert: int32_t, 0 = OK, sonst Fehler
Seiteneffekte: g_nGfxCount, g_uIntensityProzent
------------------------------------------------------------------------------*/
int32_t SetAllTextureColors(uint8_t uIntensityProzent) {
    int32_t nErrorCode;
    int32_t nTexture;
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
               nXpos, int32_t, Start-X-Position der oberen linke Ecke des Textfeldes
               nYpos, int32_t, Start-Y-Position der oberen linke Ecke des Textfeldes
               uFont, uint32_t, Zeichensatzes
                        0 = LittleFont_Green.bmp
                        1 = Font_8_15_Courier_transp
                        2 = LittleFont_Red.bmp
                        3 = LittleFont_Black.bmp
                        4 = font_arcade_numbers_green.bmp, kann nur Ziffern und Minus (Bindestrich)
               pszText, char *, Zeiger auf Text, der mit Stringende abgeschlossen sein muss.
               bAbsolute, bool, true = absolute Koordinaten, d.h. es erfolgt keine Umrechnung
               fSizeFactor, float, Vergrößerung- bzw. Verkleinerungsfaktor
      Ausgang: -
      Rückgabewert: int32_t, 0 = OK, sonst Fehler
Seiteneffekte: Video.x
------------------------------------------------------------------------------*/
int32_t PrintLittleFont(SDL_Renderer *pRenderer, int32_t nXpos, int32_t nYpos, uint32_t uFont, char *pszText, bool bAbsolute, float fSizeFactor) {
    // Der komplette Zeichensatz liegt in Texture 347 vor. Für ein Darstellung eines Zeichens, muss die "richtige" Stelle ausgewählt werden.
    // Der Zeichensatz ist so aufgebaut, dass alle vorhandenen Zeichen in einer Zeile vorliegen.
    int32_t nErrorCode;
    uint32_t I;                         // Index auf Text
    SDL_Rect SrcR;                      // Quellbereich aus Texture 347
    SDL_Rect DestR;                     // Zielbereich, zum Kopieren in den Renderer
    uint8_t cSign;
    int32_t nPrintXpos;
    int32_t nPrintYpos;
    uint32_t uFontW;
    uint32_t uFontH;
    uint32_t uTextureIndex;
    uint32_t uCharCountPerLine;

    if (uFont > 4) {
        uFont = 1;
    }
    if (uFont == 0) {
        uFontW = FONT_LITTLE_W;
        uFontH = FONT_LITTLE_H;
        uTextureIndex = TEX_FONT_LITTLE_GREEN;
    } else if (uFont == 1) {
        uFontW = FONT_LITTLE_COURIER_W;
        uFontH = FONT_LITTLE_COURIER_H;
        uTextureIndex = TEX_FONT_LITTLE_8_14_COURIER_BLACK;
    } else if (uFont == 2) {
        uFontW = FONT_LITTLE_W;
        uFontH = FONT_LITTLE_H;
        uTextureIndex = TEX_FONT_LITTLE_RED;
    } else if (uFont == 4) {
        uFontW = FONT_ARCADE_W;
        uFontH = FONT_ARCADE_H;
        uTextureIndex = TEX_FONT_ARCADE_NUMBERS_GREEN;
    } else {
        uFontW = FONT_LITTLE_W;
        uFontH = FONT_LITTLE_H;
        uTextureIndex = TEX_FONT_LITTLE_BLACK;
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
            } else if (uFont == 4) {
                // Arcade-Ziffern
                if ((pszText[I] >= '0') && (pszText[I] <= '9')) {
                    cSign = pszText[I] - 0x30 + 1;
                } else if (pszText[I] == '-') {
                    cSign = 0;
                } else {
                    cSign = 0xFF;
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
                    DestR.x = nPrintXpos + Video.uXoffs;
                    DestR.y = nPrintYpos + Video.uYoffs;
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
    uWinW = ((uXmax * FONT_LITTLE_W) / (FONT_W / 2));
    if ( ((uXmax * FONT_LITTLE_W) % (FONT_W / 2)) != 0) {
        uWinW++;
    }
    uWinW = uWinW + 3;  // Seitenstücke dazu
    if ( ((uWinH * FONT_LITTLE_H) % (FONT_H / 2)) == 0) {
        uWinH = ((uWinH * FONT_LITTLE_H) / (FONT_H / 2));
    } else {
        uWinH = ((uWinH * FONT_LITTLE_H) / (FONT_H / 2)) + 1;
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
               nXpos, int32_t, Start-X-Position der oberen linke Ecke des Fensters, -1 = auf Fenster horizontal zentrieren
               nYpos, int32_t, Start-Y-Position der oberen linke Ecke des Textfeldes, -1 = auf Fenster vertikal zentrieren
               uFont, uint32_t, Zeichensatz
               pszText, char *, Zeiger auf Text, der mit Stringende abgeschlossen sein muss.
      Ausgang: pnXpos, int32_t *, berechnete X-Position des Fensters, wenn Zentrierung aktiv, darf NULL sein
               pnXpos, int32_t *, berechnete XYPosition des Fensters, wenn Zentrierung aktiv, darf NULL sein
      Rückgabewert: 0 = OK, sonst Fehler
Seiteneffekte: Config.x
------------------------------------------------------------------------------*/
int32_t CreateMessageWindow(SDL_Renderer *pRenderer, int32_t nXpos, int32_t nYpos, uint32_t uFont, char *pszText, int32_t *pnXpos, int32_t *pnYpos) {
    int32_t nErrorCode;
    uint32_t X,Y;
    uint32_t uWinW;                     // benötigte Fensterbreite in Elementen
    uint32_t uWinH;                     // benötigte Fensterhöhe in Elementen
    SDL_Rect DestR;                     // Zielbereich, zum Kopieren in den Renderer
    uint32_t uTextureIndex;
    uint32_t uLines;
    int32_t nXoffset;
    int32_t nPrintXpos;
    int32_t nPrintYpos;

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
        if (pnXpos != NULL) {
            *pnXpos = nPrintXpos;
        }
        if (pnYpos != NULL) {
            *pnYpos = nPrintYpos;
        }
        // Oberste Zeile des Fensters zeichnen
        for (X = 0; X < uWinW && (nErrorCode == 0); X++) {
            if (X == 0) {
                uTextureIndex = TEX_STEEL_STRIPE_LEFT_TOP;
            } else if ((X + 1) >= uWinW) {
                uTextureIndex = TEX_STEEL_STRIPE_RIGHT_TOP;
            } else {
                uTextureIndex = TEX_STEEL_STRIPE_TOP;
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
            if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(TEX_STEEL_STRIPE_LEFT),NULL,&DestR,0,NULL, SDL_FLIP_NONE) != 0) {
                SDL_Log("%s: SDL_RenderCopyEx() failed: %s",__FUNCTION__,SDL_GetError());
                nErrorCode = -1;
            }
            DestR.x = nPrintXpos + (uWinW - 1) * (FONT_W / 2);
            DestR.y = nPrintYpos;
            DestR.w = FONT_W / 2;
            DestR.h = FONT_H / 2;
            if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(TEX_STEEL_STRIPE_RIGHT),NULL,&DestR,0,NULL, SDL_FLIP_NONE) != 0) {
                SDL_Log("%s: SDL_RenderCopyEx() failed: %s",__FUNCTION__,SDL_GetError());
                nErrorCode = -1;
            }
            nPrintYpos = nPrintYpos + FONT_H / 2;
        }
        // Unterste Zeile des Fensters zeichnen
        nPrintXpos = nXpos;
        for (X = 0; X < uWinW && (nErrorCode == 0); X++) {
            if (X == 0) {
                uTextureIndex = TEX_STEEL_STRIPE_LEFT_BOTTOM;
            } else if ((X + 1) >= uWinW) {
                uTextureIndex = TEX_STEEL_STRIPE_RIGHT_BOTTOM;
            } else {
                uTextureIndex = TEX_STEEL_STRIPE_BOTTOM;
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
                nXoffset = ((uWinH * (FONT_H / 2)) - (uLines * FONT_LITTLE_H)) / 2;
                //SDL_Log("Lines: %u    WinH: %u    XOffset: %d",uLines,uWinH,nXoffset);
                nErrorCode = PrintLittleFont(pRenderer, nXpos + (FONT_W / 2) + 8, nYpos + (FONT_H / 2) + nXoffset, uFont ,pszText,K_ABSOLUTE,1);
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
Rückgabewert:  int32_t, 0 = Alles OK, sonst Fehler
Seiteneffekte: Video.x
------------------------------------------------------------------------------*/
int32_t DrawBeam(SDL_Renderer *pRenderer,uint32_t uXpos, uint32_t uYpos, uint32_t uWidth, uint32_t uHeight, uint8_t uRed, uint32_t uGreen, uint32_t uBlue, uint8_t uTransp, bool bAbsolute) {
    int32_t nErrorCode = -1;
    SDL_Rect DestR;

    // Balken zeichnen
    if (bAbsolute) {
        DestR.x = uXpos;
        DestR.y = uYpos;
    } else {
        DestR.x = uXpos + Video.uXoffs;
        DestR.y = uYpos + Video.uYoffs;
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
Rückgabewert:  int32_t, 0 = Alles OK, sonst Fehler
Seiteneffekte: Video.x
------------------------------------------------------------------------------*/
int32_t DrawGrid(SDL_Renderer *pRenderer, uint32_t uXpos, uint32_t uYpos, uint32_t uWidth, uint32_t uHeight, uint8_t uRed, uint8_t uGreen, uint8_t uBlue, uint8_t uAlpha, uint32_t uGridSpace) {
    int32_t nErrorCode = 0;
    uint32_t X,Y;

    nErrorCode = SDL_SetRenderDrawColor(pRenderer,uRed,uGreen,uBlue,uAlpha);
    // Vertikale Linien zeichnen
    for (X = uXpos; (X <= (uXpos + uWidth)) && (nErrorCode == 0); X = X + uGridSpace) {
        nErrorCode = SDL_RenderDrawLine(pRenderer,X + Video.uXoffs, uYpos + Video.uYoffs, X + Video.uXoffs, uYpos + uHeight + Video.uYoffs);
    }
    // Horizontale Linien zeichnen
    for (Y = uYpos; (Y <= (uYpos + uHeight)) && (nErrorCode == 0); Y = Y + uGridSpace) {
        nErrorCode = SDL_RenderDrawLine(pRenderer,uXpos + Video.uXoffs, Y + Video.uYoffs, uXpos + uWidth + Video.uXoffs, Y + Video.uYoffs);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           MeasureFps
------------------------------------------------------------------------------
Beschreibung: Misst die aaktuelle Framerate. Diese Funkton wird innerhalb einer
              Anzeigeschleife aufgerufen und ist nicht threadsicher!
              Vor Verwendung dieser Funktion sollte ein
              memset(&Video.Fps,0,sizeof(Video.Fps));
              erfolgen.
Parameter
      Eingang: -
      Ausgang: Video.Fps.fFramesPerSecond
Rückgabewert:  -
Seiteneffekte: Video.x
------------------------------------------------------------------------------*/
void MeasureFps(void) {
    uint32_t uActTicks;
    float fDiffTimeSeconds;

    Video.Fps.uFrameCount++;
    if (Video.Fps.uFrameCount == 100) {     // 100 Frames abwarten
        uActTicks = SDL_GetTicks();
        fDiffTimeSeconds = ((float)uActTicks - (float)Video.Fps.uLastTicks) / (Video.Fps.uFrameCount * 1000);
        if (fDiffTimeSeconds > 0) {
            Video.Fps.fFramesPerSecond = 1 / fDiffTimeSeconds;
            sprintf(Video.Fps.szFramesPerSecond,"FRAMERATE: %.2f",Video.Fps.fFramesPerSecond);
        }
        Video.Fps.uLastTicks = uActTicks;
        Video.Fps.uFrameCount = 0;
    }
}


/*
void ShowFormat(Uint32 uFormat) {
    switch (uFormat) {
        case (SDL_PIXELFORMAT_UNKNOWN):
            SDL_Log("SDL_PIXELFORMAT_UNKNOWN");
            break;
        case (SDL_PIXELFORMAT_INDEX1LSB):
            SDL_Log("SDL_PIXELFORMAT_INDEX1LSB");
            break;
        case (SDL_PIXELFORMAT_INDEX1MSB):
            SDL_Log("SDL_PIXELFORMAT_INDEX1MSB");
            break;
        case (SDL_PIXELFORMAT_INDEX4LSB):
            SDL_Log("SDL_PIXELFORMAT_INDEX4LSB");
            break;
        case (SDL_PIXELFORMAT_INDEX4MSB):
            SDL_Log("SDL_PIXELFORMAT_INDEX4MSB");
            break;
        case (SDL_PIXELFORMAT_INDEX8):
            SDL_Log("SDL_PIXELFORMAT_INDEX8");
            break;
        case (SDL_PIXELFORMAT_RGB332):
            SDL_Log("SDL_PIXELFORMAT_RGB332");
            break;
        case (SDL_PIXELFORMAT_RGB444):
            SDL_Log("SDL_PIXELFORMAT_RGB444");
            break;
        case (SDL_PIXELFORMAT_RGB555):
            SDL_Log("SDL_PIXELFORMAT_RGB555");
            break;
        case (SDL_PIXELFORMAT_BGR555):
            SDL_Log("SDL_PIXELFORMAT_BGR555");
            break;
        case (SDL_PIXELFORMAT_ARGB4444):
            SDL_Log("SDL_PIXELFORMAT_ARGB4444");
            break;
        case (SDL_PIXELFORMAT_RGBA4444):
            SDL_Log("SDL_PIXELFORMAT_RGBA4444");
            break;
        case (SDL_PIXELFORMAT_ABGR4444):
            SDL_Log("SDL_PIXELFORMAT_ABGR4444");
            break;
        case (SDL_PIXELFORMAT_BGRA4444):
            SDL_Log("SDL_PIXELFORMAT_BGRA4444");
            break;
        case (SDL_PIXELFORMAT_ARGB1555):
            SDL_Log("SDL_PIXELFORMAT_ARGB1555");
            break;
        case (SDL_PIXELFORMAT_RGBA5551):
            SDL_Log("SDL_PIXELFORMAT_RGBA5551");
            break;
        case (SDL_PIXELFORMAT_ABGR1555):
            SDL_Log("SDL_PIXELFORMAT_ABGR1555");
            break;
        case (SDL_PIXELFORMAT_BGRA5551):
            SDL_Log("SDL_PIXELFORMAT_BGRA5551");
            break;
        case (SDL_PIXELFORMAT_RGB565):
            SDL_Log("SDL_PIXELFORMAT_RGB565");
            break;
        case (SDL_PIXELFORMAT_BGR565):
            SDL_Log("SDL_PIXELFORMAT_BGR565");
            break;
        case (SDL_PIXELFORMAT_RGB24):
            SDL_Log("SDL_PIXELFORMAT_RGB24");
            break;
        case (SDL_PIXELFORMAT_BGR24):
            SDL_Log("SDL_PIXELFORMAT_BGR24");
            break;
        case (SDL_PIXELFORMAT_RGB888):
            SDL_Log("SDL_PIXELFORMAT_RGB888");
            break;
        case (SDL_PIXELFORMAT_RGBX8888):
            SDL_Log("SDL_PIXELFORMAT_RGBX8888");
            break;
        case (SDL_PIXELFORMAT_BGR888):
            SDL_Log("SDL_PIXELFORMAT_BGR888");
            break;
        case (SDL_PIXELFORMAT_BGRX8888):
            SDL_Log("SDL_PIXELFORMAT_BGRX8888");
            break;
        case (SDL_PIXELFORMAT_ARGB8888):
            SDL_Log("SDL_PIXELFORMAT_ARGB8888");
            break;
        case (SDL_PIXELFORMAT_RGBA8888):
            SDL_Log("SDL_PIXELFORMAT_RGBA8888");
            break;
        case (SDL_PIXELFORMAT_ABGR8888):
            SDL_Log("SDL_PIXELFORMAT_ABGR8888");
            break;
        case (SDL_PIXELFORMAT_BGRA8888):
            SDL_Log("SDL_PIXELFORMAT_BGRA8888");
            break;
        case (SDL_PIXELFORMAT_ARGB2101010):
            SDL_Log("SDL_PIXELFORMAT_ARGB2101010");
            break;
//      case (SDL_PIXELFORMAT_RGBA32):
//          SDL_Log("SDL_PIXELFORMAT_RGBA32");
//          break;
//        case (SDL_PIXELFORMAT_ARGB32):
//           SDL_Log("SDL_PIXELFORMAT_ARGB32");
//           break;
//        case (SDL_PIXELFORMAT_BGRA32):
//           SDL_Log("SDL_PIXELFORMAT_BGRA32");
//           break;
//        case (SDL_PIXELFORMAT_ABGR32):
//           SDL_Log("SDL_PIXELFORMAT_ABGR32");
//           break;
        case (SDL_PIXELFORMAT_YV12):
            SDL_Log("SDL_PIXELFORMAT_YV12");
            break;
        case (SDL_PIXELFORMAT_IYUV):
            SDL_Log("SDL_PIXELFORMAT_IYUV");
            break;
        case (SDL_PIXELFORMAT_YUY2):
            SDL_Log("SDL_PIXELFORMAT_YUY2");
            break;
        case (SDL_PIXELFORMAT_UYVY):
            SDL_Log("SDL_PIXELFORMAT_UYVY");
            break;
        case (SDL_PIXELFORMAT_YVYU):
            SDL_Log("SDL_PIXELFORMAT_YVYU");
            break;
        case (SDL_PIXELFORMAT_NV12):
            SDL_Log("SDL_PIXELFORMAT_NV12");
            break;
        case (SDL_PIXELFORMAT_NV21):
            SDL_Log("SDL_PIXELFORMAT_NV21");
            break;
        default:
            SDL_Log("unknown");
    }
}
*/
