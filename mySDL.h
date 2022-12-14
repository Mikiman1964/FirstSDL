#ifndef MYSDL_H_INCLUDED
#define MYSDL_H_INCLUDED

#include <SDL2/SDL.h>

#define WINDOW_W            1024
#define WINDOW_H            768
#define WINDOW_TITLE        "Meine erste SDL-Anwendung"

#define FONT_W              32
#define FONT_H              32

// Font 0
#define FONT_LITTLE_347_W   10
#define FONT_LITTLE_347_H   14

// Font 1
#define FONT_LITTLE_559_W   8
#define FONT_LITTLE_559_H   14

// Button-H?he f?r Font 1
#define BUTTON_H            (FONT_LITTLE_559_H + 5)

// Prototypen
SDL_Window *InitSDL_Window(int nWindowW, int nWindowH, const char * pszWindowTitle);
int GetDisplayMode(SDL_Window *pWindow, int *pnWindowPosX, int *pnWindowPosY);
SDL_Renderer *CreateRenderer(SDL_Window * pWindow);
int LoadTextures(SDL_Renderer *pRenderer);
int CopyTexture(SDL_Renderer *pRenderer, uint32_t uTextureIndex, int nXpos, int nYpos, uint32_t uTextureW, uint32_t uTextureH, float fScaleW, float fScaleH, float fAngle);
int CopyColorRect(SDL_Renderer *pRenderer, int nRed, int nGreen, int nBlue, int nXpos, int nYpos, uint32_t uW, uint32_t uH);
void FreeTextures(void);
SDL_Texture *GetTextureByIndex(int nIndex);
int SetAllTextureColors(uint8_t uIntensityProzent);
int PrintLittleFont(SDL_Renderer *pRenderer, int nXpos, int nYpos, uint32_t uFont, char *pszText);
void GetMessageWindowSize(uint32_t *puWinW,uint32_t *puWinH, uint32_t *puLines, char *pszText);
int CreateMessageWindow(SDL_Renderer *pRenderer, int nXpos, int nYpos, uint32_t uColor, char *pszText);
#endif // MYSDL_H_INCLUDED
