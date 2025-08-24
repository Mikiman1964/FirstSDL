#ifndef ASTEROIDS_H_INCLUDED
#define ASTEROIDS_H_INCLUDED

#include <SDL2/SDL.h>
#include <stdbool.h>

#define MAX_ASTEROIDS   80
#define MAX_ASTEROIDLAYERS 3

typedef struct {
    int32_t nSpeed;             // Geschwindigkeit des Astroiden
    int32_t nXpos;              // Aktuelle X-Position
    int32_t nXdeflection;       // Zusätzliche X-Ablenkung
    int32_t nYdeflection;       // Zusätzliche Y-Ablenkung
    int32_t nXmaxDeflection;    // Maximale X-Ablenkung
    int32_t nYmaxDeflection;    // Maximale Y-Ablenkung
    int32_t nYpos;              // Aktuelle Y-Position
    double dAngleRotate;        // Drehwinkel für Rotation
    double dRotationSpeed;      // Drehgeschwindigkeit
    float fXangle;              // Winkel für X-Ablenkung
    float fYangle;              // Winkel für Y-Ablenkung
    float fXangleSpeed;         // Winkelgeschwindigkeit für X-Ablenkung
    float fYangleSpeed;         // Winkelgeschwindigkeit für Y-Ablenkung
    int32_t nWidth;             // Breite des Asteroiden
    int32_t nHeight;            // Höhe des Asteroiden
    bool bReady;                // Asteroid ist am Zielort angekommen und kann neu geladen werden
} ASTEROID;


typedef struct {
  int32_t nCount;               // Aktuelle Anzahl Asteroiden im Layer
  int32_t nMaxCount;            // Maximale Anzahl Asteroiden im Layer
  Uint32 uLastCountIncreased;   // Damit nicht alle Asteroiden gleichzeitg starten
  ASTEROID Asteroid[MAX_ASTEROIDS];
} ASTEROID_LAYER;


void InitAsteroidLayer(void);
int32_t MoveAsteroids(SDL_Renderer *pRenderer,SDL_Texture *pAsteroidTexture);
int32_t SetAsteroidDeflection(int32_t nLayer, int32_t nAsteroid, float fXangleSpeed, float fYangleSpeed, int32_t nXmaxDeflection, int32_t nYmaxDeflection);
int32_t SwitchDrunkenAsteroids(bool bOnOff);

#endif // ASTEROIDS_H_INCLUDED
