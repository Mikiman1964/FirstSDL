#ifndef MYSDL_H_INCLUDED
#define MYSDL_H_INCLUDED

#include <SDL2/SDL.h>

#define DEFAULT_WINDOW_W            1280        // Ist auch gleichzeitig die
#define DEFAULT_WINDOW_H            768         // Mindestanforderung an die Auflösung
#define WINDOW_TITLE        "My first SDL2 application"

#define FONT_W              32
#define FONT_H              32

// Little Font, schwarz, grün, rot
#define FONT_LITTLE_W       10
#define FONT_LITTLE_H       14

// Font Courier, schwarz
#define FONT_LITTLE_COURIER_W   8
#define FONT_LITTLE_COURIER_H   14

// Button-Höhe für Font 1
#define BUTTON_H            (FONT_LITTLE_COURIER_H + 5)

// Für Messageboxen
#define EMERALD_MAX_LINEFEEDS_IN_MESSAGE        30                          // Maximal 30 Zeilenumbrüche in Message
#define EMERALD_MAX_CHARACTERS_PER_LINE         96                          // Maximal 96 Zeichen pro Zeile in Message

#define MAX_SHOWABLE_DISPLAYMODES               21                          // Maximale Anzahl von auswählbaren Displaymodi

#define K_ABSOLUTE                              true
#define K_RELATIVE                              false


typedef struct {
    // Variablen für Messung der Framerate
    float fFramesPerSecond;
    float fDiffTimeSeconds;
    uint32_t uLastTicks;
    uint32_t uActTicks;
    uint32_t uFrameCount;
    char szFrameaPerSecond[32];
} FPS;


typedef struct {
    int nW;
    int nH;
    int nModeIndex;
} USABLEDISPLAYMODES;


typedef struct {
    int nW;
    int nH;
    int nModeIndex;
} SHOWABLEDISPLAYMODES;


typedef struct {
    SDL_DisplayMode DesktopDisplayMode;                                     // Desktop-Display-Einstellungen beim Start des Programms
    uint32_t uXoffs;                                                        // X-Offset für die Zentrierung von Elementen
    uint32_t uYoffs;                                                        // Y-Offset für die Zentrierung von Elementen
    bool bMustUseFullScreen;                                                // FullScreen muss aufgrund falscher Framerate verwendet werden
    SDL_Window *pWindow;
    // brauchbare Display-Modes
    int nUsableDisplayModeCount;                                            // Anzahl der nutzbaren Display-Modes, Xres >= 1280, Yres >= 768, refreshrate = 60 Hz
    USABLEDISPLAYMODES *pUsableModes;                                       // X- und Y- Auflösung und Index
    // Anzeigbare Display-Modes
    int nShowableDisplayModeCount;                                          // Anzahl der anzeigbaren Display-Modes
    SHOWABLEDISPLAYMODES ShowableDisplayModes[MAX_SHOWABLE_DISPLAYMODES];   // X- und Y- Auflösung und Index
} VIDEO;


typedef struct {
    uint8_t uRed;
    uint8_t uGreen;
    uint8_t uBlue;
    uint8_t uAlpha;
} RGBCOLOR;

// Prototypen
SDL_Window *InitSDL_Window(int nWindowW, int nWindowH, const char * pszWindowTitle);
int GetClosestWindowSize(int nWindowW, int nWindowH);
int GetUsableDisplayModes(uint32_t uDisplay);
int GetShowableDisplayModes(void);
int GetDesktopDisplayMode(void);
SDL_Renderer *CreateRenderer(SDL_Window * pWindow);
int RenderPresentAndClear(SDL_Renderer *pRenderer);
int LoadTextures(SDL_Renderer *pRenderer);
int CopyTexture(SDL_Renderer *pRenderer, uint32_t uTextureIndex, int nXpos, int nYpos, uint32_t uTextureW, uint32_t uTextureH, float fScaleW, float fScaleH, float fAngle);
int CopyColorRect(SDL_Renderer *pRenderer, int nRed, int nGreen, int nBlue, int nXpos, int nYpos, uint32_t uW, uint32_t uH, bool bAbsolute);
void FreeTextures(void);
SDL_Texture *GetTextureByIndex(int nIndex);
int SetAllTextureColors(uint8_t uIntensityProzent);
int PrintLittleFont(SDL_Renderer *pRenderer, int nXpos, int nYpos, uint32_t uFont, char *pszText,bool bAbsolute,float fSizeFactor);
void GetMessageWindowSize(uint32_t *puWinW,uint32_t *puWinH, uint32_t *puLines, char *pszText);
int CreateMessageWindow(SDL_Renderer *pRenderer, int nXpos, int nYpos, uint32_t uFont, char *pszText, int *pnXpos, int *pnYpos);
int DrawBeam(SDL_Renderer *pRenderer,uint32_t uXpos, uint32_t uYpos, uint32_t uWidth, uint32_t uHeight, uint8_t uRed, uint32_t uGreen, uint32_t uBlue, uint8_t uTransp, bool bAbsolute);
int DrawGrid(SDL_Renderer *pRenderer, uint32_t uXpos, uint32_t uYpos, uint32_t uWidth, uint32_t uHeight, uint8_t uRed, uint8_t uGreen, uint8_t uBlue, uint8_t uAlpha, uint32_t uGridSpace);
void RestoreDesktop(void);
int SetWindowDisplayMode(int nWidth, int nHeight, Uint32 uFormat, int nRefreshRate);
int CenterWindow(uint32_t uWidth, uint32_t uHeight);
void MeasureFps(void);
#endif // MYSDL_H_INCLUDED
