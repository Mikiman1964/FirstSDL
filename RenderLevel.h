#ifndef RENDERLEVEL_H_INCLUDED
#define RENDERLEVEL_H_INCLUDED
#include <SDL2/SDL.h>

#define     MAX_CHEESE_RANDOM_NUMBERS   1024


void FillCheeseRandomNumbers(void);
int RenderPipeElement(SDL_Renderer *pRenderer, uint16_t uPipeElement, uint32_t uX, uint32_t uY, int nXpos, int nYpos);
int RenderLevel(SDL_Renderer *pRenderer, int *pnXpos, int *pnYpos, int nAnimationCount);

#endif // RENDERLEVEL_H_INCLUDED
