#ifndef RENDERLEVEL_H_INCLUDED
#define RENDERLEVEL_H_INCLUDED
#include <SDL2/SDL.h>

#define     MAX_CHEESE_RANDOM_NUMBERS       1024
#define     TREASURECHEST_TIMER             5000        // Alle 5000 ms die Truhen öffnen
#define     TREASURECHEST_OPEN_TIMER        1500        // 1500 ms Truhe offen lassen

void FillCheeseRandomNumbers(void);
void SetTreasureChestStart(void);
int32_t RenderPipeElement(SDL_Renderer *pRenderer, uint16_t uPipeElement, uint32_t uX, uint32_t uY, int32_t nXpos, int32_t nYpos);
int32_t RenderLevel(SDL_Renderer *pRenderer, int32_t *pnXpos, int32_t *pnYpos, int32_t nAnimationCount);

#endif // RENDERLEVEL_H_INCLUDED
