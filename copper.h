#ifndef COPPER_H_INCLUDED
#define COPPER_H_INCLUDED

#include <stdbool.h>

#define COPPER_COLORLINES_PER_SEGMENT       26                                              // Anzahl Faaben pro Segment
#define COPPER_PRECALCULATED_SEGMENTS       ((COPPER_COLORLINES_PER_SEGMENT * 2) + (COPPER_COLORLINES_PER_SEGMENT - 2))


typedef struct {
    int nRed;
    int nGreen;
    int nBlue;
} COLORFLOW;


typedef struct {
    int nRed;
    int nGreen;
    int nBlue;
} COPPERLINE;


typedef struct {
    COPPERLINE CopperLine[COPPER_COLORLINES_PER_SEGMENT];
} COPPERSEGMENT;


int InitVisibibleCopperSegments();
void CalculateColorFlow(int nMaxRed, int nMaxGreen, int nMaxBlue);
void PrecalculateSegments(int nMaxRed, int nMaxGreen, int nMaxBlue);
void ScrollVisibleSegments(void);
int DoCopper(SDL_Renderer *pRenderer,bool bScroll);
void FreeCopper(void);
#endif // COPPER_H_INCLUDED
