#ifndef MYSDL_H_INCLUDED
#define MYSDL_H_INCLUDED

#include <SDL2/SDL.h>

#define DEFAULT_WINDOW_W                        1280    // Ist auch gleichzeitig die
#define DEFAULT_WINDOW_H                        768     // Mindestanforderung an die Auflösung
#define WINDOW_TITLE                            "My first SDL2 application, V "

#define FONT_W                                  32
#define FONT_H                                  32

// Little Font, schwarz, grün, rot
#define FONT_LITTLE_W                           10
#define FONT_LITTLE_H                           14

// Font Courier, schwarz
#define FONT_LITTLE_COURIER_W                   8
#define FONT_LITTLE_COURIER_H                   14

// Font Arcade, grün, nur Ziffern
#define FONT_ARCADE_W                           8
#define FONT_ARCADE_H                           8

// Button-Höhe für Font 1
#define BUTTON_H                                (FONT_LITTLE_COURIER_H + 5)

// Für Messageboxen
#define EMERALD_MAX_LINEFEEDS_IN_MESSAGE        30      // Maximal 30 Zeilenumbrüche in Message
#define EMERALD_MAX_CHARACTERS_PER_LINE         96      // Maximal 96 Zeichen pro Zeile in Message

#define MAX_SHOWABLE_DISPLAYMODES               21      // Maximale Anzahl von auswählbaren Displaymodi

#define K_ABSOLUTE                              true
#define K_RELATIVE                              false


#define VSYNC_DETECTION_FRAMES                  20     // Anzahl Frames, um V-Sync zu detektieren.
#define VSYNC_WARNING_MESSAGE                   "WARNING: NO Y-SYNC DETECTED"


typedef struct {
    int32_t nW;
    int32_t nH;
    int32_t nModeIndex;
} USABLEDISPLAYMODES;


typedef struct {
    int32_t nW;
    int32_t nH;
    int32_t nModeIndex;
} SHOWABLEDISPLAYMODES;


typedef struct {
    // Variablen für Messung der Framerate
    float fFramesPerSecond;
    float fDiffTimeSeconds;
    uint32_t uLastTicks;
    uint32_t uActTicks;
    uint32_t uFrameCount;
    char szFramesPerSecond[64];
} FPS;


typedef struct {
    SDL_DisplayMode DesktopDisplayMode;                                     // Desktop-Display-Einstellungen beim Start des Programms
    uint32_t uXoffs;                                                        // X-Offset für die Zentrierung von Elementen
    uint32_t uYoffs;                                                        // Y-Offset für die Zentrierung von Elementen
    bool bMustUseFullScreen;                                                // FullScreen muss aufgrund falscher Framerate verwendet werden
    SDL_Window *pWindow;
    // brauchbare Display-Modes
    int32_t nUsableDisplayModeCount;                                        // Anzahl der nutzbaren Display-Modes, Xres >= 1280, Yres >= 768, refreshrate = 60 Hz
    USABLEDISPLAYMODES *pUsableModes;                                       // X- und Y- Auflösung und Index
    // Anzeigbare Display-Modes
    int32_t nShowableDisplayModeCount;                                      // Anzahl der anzeigbaren Display-Modes
    SHOWABLEDISPLAYMODES ShowableDisplayModes[MAX_SHOWABLE_DISPLAYMODES];   // X- und Y- Auflösung und Index
    FPS Fps;
    bool bVsync;                                                            // V-Sync mit 60 Hz erkannt
} VIDEO;


typedef struct {
    uint8_t uRed;
    uint8_t uGreen;
    uint8_t uBlue;
    uint8_t uAlpha;
} RGBCOLOR;

// Prototypen
SDL_Window *InitSDL_Window(int32_t nWindowW, int32_t nWindowH, const char * pszWindowTitle);
int32_t GetClosestWindowSize(int32_t nWindowW, int32_t nWindowH);
int32_t GetUsableDisplayModes(uint32_t uDisplay);
int32_t GetShowableDisplayModes(void);
int32_t GetDesktopDisplayMode(void);
SDL_Renderer *CreateRenderer(SDL_Window * pWindow);
int32_t RenderPresentAndClear(SDL_Renderer *pRenderer);
int32_t LoadTextures(SDL_Renderer *pRenderer);
int32_t CopyTexture(SDL_Renderer *pRenderer, uint32_t uTextureIndex, int32_t nXpos, int32_t nYpos, uint32_t uTextureW, uint32_t uTextureH, float fScaleW, float fScaleH, float fAngle);
int32_t CopyColorRect(SDL_Renderer *pRenderer, int32_t nRed, int32_t nGreen, int32_t nBlue, int32_t nXpos, int32_t nYpos, uint32_t uW, uint32_t uH, bool bAbsolute);
void FreeTextures(void);
SDL_Texture *GetTextureByIndex(int32_t nIndex);
int32_t SetAllTextureColors(uint8_t uIntensityProzent);
int32_t PrintLittleFont(SDL_Renderer *pRenderer, int32_t nXpos, int32_t nYpos, uint32_t uFont, char *pszText,bool bAbsolute,float fSizeFactor);
void GetMessageWindowSize(uint32_t *puWinW,uint32_t *puWinH, uint32_t *puLines, char *pszText);
int32_t CreateMessageWindow(SDL_Renderer *pRenderer, int32_t nXpos, int32_t nYpos, uint32_t uFont, char *pszText, int32_t *pnXpos, int32_t *pnYpos);
int32_t DrawBeam(SDL_Renderer *pRenderer,uint32_t uXpos, uint32_t uYpos, uint32_t uWidth, uint32_t uHeight, uint8_t uRed, uint32_t uGreen, uint32_t uBlue, uint8_t uTransp, bool bAbsolute);
int32_t DrawGrid(SDL_Renderer *pRenderer, uint32_t uXpos, uint32_t uYpos, uint32_t uWidth, uint32_t uHeight, uint8_t uRed, uint8_t uGreen, uint8_t uBlue, uint8_t uAlpha, uint32_t uGridSpace);
void RestoreDesktop(void);
int32_t SetWindowDisplayMode(int32_t nWidth, int32_t nHeight, Uint32 uFormat, int32_t nRefreshRate);
int32_t CenterWindow(uint32_t uWidth, uint32_t uHeight);
void MeasureFps(void);
bool IsVsync(SDL_Renderer *pRenderer,float fCheckFramerate);
#endif // MYSDL_H_INCLUDED
