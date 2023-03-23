#ifndef SMILEYS_H_INCLUDED
#define SMILEYS_H_INCLUDED

#include <SDL2/SDL.h>
#include <stdbool.h>

#define MAX_SMILEYS   220

typedef struct {
    float fXSpeed;            // Geschwindigkeit des Smileys
    float fYSpeed;            // Geschwindigkeit des Smileys
    float fXpos;              // Aktuelle X-Position
    float fYpos;              // Aktuelle Y-Position
    float fAngleRotate;       // Drehwinkel für Rotation
    float fRotationSpeed;     // Drehgeschwindigkeit
    bool bReady;              // Smiley ist am Zielort angekommen und kann neu geladen werden
    float fSize;              // 0 bis 1
} SMILEY;


void InitSmileys(void);
void BornSmileys(void);
int MoveSmileys(SDL_Renderer *pRenderer);

#endif // SMILEYS_H_INCLUDED
