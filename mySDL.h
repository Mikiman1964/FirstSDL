#ifndef MYSDL_H_INCLUDED
#define MYSDL_H_INCLUDED

#include <SDL2/SDL.h>

#define DEFAULT_WINDOW_W            1280        // Ist auch gleichzeitig die
#define DEFAULT_WINDOW_H            768         // Mindestanforderung an die Auflösung
#define WINDOW_TITLE        "My first SDL2 application"

#define FONT_W              32
#define FONT_H              32

// Font 0
#define FONT_LITTLE_347_W   10
#define FONT_LITTLE_347_H   14

// Font 1
#define FONT_LITTLE_559_W   8
#define FONT_LITTLE_559_H   14

// Button-Höhe für Font 1
#define BUTTON_H            (FONT_LITTLE_559_H + 5)

// Für Messageboxen
#define EMERALD_MAX_LINEFEEDS_IN_MESSAGE        30              // Maximal 30 Zeilenumbrüche in Message
#define EMERALD_MAX_CHARACTERS_PER_LINE         96              // Maximal 96 Zeichen pro Zeile in Message

#define MAX_USABLE_DISPLAYMODES                 500

#define MAX_SHOWABLE_DISPLAYMODES               21              // Maximale Anzahl von auswählbaren Displaymodi

#define K_ABSOLUTE                              true
#define K_RELATIVE                              false



typedef struct {
    int nDisplayModeCount;
    int nW[MAX_USABLE_DISPLAYMODES];
    int nH[MAX_USABLE_DISPLAYMODES];
    int nModeIndex[MAX_USABLE_DISPLAYMODES];
} USABLEDISPLAYMODES;


typedef struct {
    int nDisplayModeCount;
    int nW[MAX_SHOWABLE_DISPLAYMODES];
    int nH[MAX_SHOWABLE_DISPLAYMODES];
    int nModeIndex[MAX_SHOWABLE_DISPLAYMODES];
} SHOWABLEDISPLAYMODES;


typedef struct {
    uint8_t uRed;
    uint8_t uGreen;
    uint8_t uBlue;
    uint8_t uAlpha;
} RGBCOLOR;

// Prototypen
SDL_Window *InitSDL_Window(int nWindowW, int nWindowH, const char * pszWindowTitle);
int GetUsableDisplayModes(uint32_t uDisplay);
int GetShowableDisplayModes(void);
int GetDesktopDisplayMode(void);
SDL_Renderer *CreateRenderer(SDL_Window * pWindow);
int LoadTextures(SDL_Renderer *pRenderer);
int CopyTexture(SDL_Renderer *pRenderer, uint32_t uTextureIndex, int nXpos, int nYpos, uint32_t uTextureW, uint32_t uTextureH, float fScaleW, float fScaleH, float fAngle);
int CopyColorRect(SDL_Renderer *pRenderer, int nRed, int nGreen, int nBlue, int nXpos, int nYpos, uint32_t uW, uint32_t uH, bool bAbsolute);
void FreeTextures(void);
SDL_Texture *GetTextureByIndex(int nIndex);
int SetAllTextureColors(uint8_t uIntensityProzent);
int PrintLittleFont(SDL_Renderer *pRenderer, int nXpos, int nYpos, uint32_t uFont, char *pszText,bool bAbsolute);
void GetMessageWindowSize(uint32_t *puWinW,uint32_t *puWinH, uint32_t *puLines, char *pszText);
int CreateMessageWindow(SDL_Renderer *pRenderer, int nXpos, int nYpos, uint32_t uColor, char *pszText);
int DrawBeam(SDL_Renderer *pRenderer,uint32_t uXpos, uint32_t uYpos, uint32_t uWidth, uint32_t uHeight, uint8_t uRed, uint32_t uGreen, uint32_t uBlue, uint8_t uTransp, bool bAbsolute);
int DrawGrid(SDL_Renderer *pRenderer, uint32_t uXpos, uint32_t uYpos, uint32_t uWidth, uint32_t uHeight, uint8_t uRed, uint8_t uGreen, uint8_t uBlue, uint8_t uAlpha, uint32_t uGridSpace);
void RestoreDesktop(void);
int CenterWindow(uint32_t uWidth, uint32_t uHeight);
#endif // MYSDL_H_INCLUDED
