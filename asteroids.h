#ifndef ASTEROIDS_H_INCLUDED
#define ASTEROIDS_H_INCLUDED

#include <SDL2/SDL.h>
#include <stdbool.h>

#define MAX_ASTEROIDS   80
#define MAX_ASTEROIDLAYERS 3

typedef struct {
    int nSpeed;             // Geschwindigkeit des Astroiden
    int nXpos;              // Aktuelle X-Position
    int nXdeflection;       // Zusätzliche X-Ablenkung
    int nYdeflection;       // Zusätzliche Y-Ablenkung
    int nXmaxDeflection;    // Maximale X-Ablenkung
    int nYmaxDeflection;    // Maximale Y-Ablenkung
    int nYpos;              // Aktuelle Y-Position
    double dAngleRotate;    // Drehwinkel für Rotation
    double dRotationSpeed;  // Drehgeschwindigkeit
    float fXangle;          // Winkel für X-Ablenkung
    float fYangle;          // Winkel für Y-Ablenkung
    float fXangleSpeed;     // Winkelgeschwindigkeit für X-Ablenkung
    float fYangleSpeed;     // Winkelgeschwindigkeit für Y-Ablenkung
    int nWidth;             // Breite des Asteroiden
    int nHeight;            // Höhe des Asteroiden
    bool bReady;            // Asteroid ist am Zielort angekommen und kann neu geladen werden
} ASTEROID;


typedef struct {
  int nCount;       // Aktuelle Anzahl Asteroiden im Layer
  int nMaxCount;    // Maximale Anzahl Asteroiden im Layer
  Uint32 uLastCountIncreased;   // Damit nicht alle Asteroiden gleichzeitg starten
  ASTEROID Asteroid[MAX_ASTEROIDS];
} ASTEROID_LAYER;


void InitAsteroidLayer(void);
int MoveAsteroids(SDL_Renderer *pRenderer,SDL_Texture *pAsteroidTexture);
int SetAsteroidDeflection(int nLayer, int nAsteroid, float fXangleSpeed, float fYangleSpeed, int nXmaxDeflection, int nYmaxDeflection);
int SwitchDrunkenAsteroids(bool bOnOff);

#endif // ASTEROIDS_H_INCLUDED
