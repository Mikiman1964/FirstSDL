#ifndef COPPER_H_INCLUDED
#define COPPER_H_INCLUDED

#include <stdbool.h>

#define COPPER_COLORLINES_PER_SEGMENT       26                                              // Anzahl Faaben pro Segment
#define COPPER_PRECALCULATED_SEGMENTS       ((COPPER_COLORLINES_PER_SEGMENT * 2) + (COPPER_COLORLINES_PER_SEGMENT - 2))


typedef struct {
    int32_t nRed;
    int32_t nGreen;
    int32_t nBlue;
} COLORFLOW;


typedef struct {
    int32_t nRed;
    int32_t nGreen;
    int32_t nBlue;
} COPPERLINE;


typedef struct {
    COPPERLINE CopperLine[COPPER_COLORLINES_PER_SEGMENT];
} COPPERSEGMENT;


int32_t InitVisibibleCopperSegments(void);
void CalculateColorFlow(int32_t nMaxRed, int32_t nMaxGreen, int32_t nMaxBlue);
void PrecalculateSegments(int32_t nMaxRed, int32_t nMaxGreen, int32_t nMaxBlue);
void ScrollVisibleSegments(void);
int32_t DoCopper(SDL_Renderer *pRenderer,bool bScroll);
void FreeCopper(void);
#endif // COPPER_H_INCLUDED
