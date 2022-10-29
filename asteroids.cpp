#include <SDL2/SDL.h>
#include <math.h>
#include "asteroids.h"
#include "mySDL.h"
#include "mystd.h"


ASTEROID_LAYER AsteroidLayer[MAX_ASTEROIDLAYERS];


/*----------------------------------------------------------------------------
Name:           InitAsteroidLayer
------------------------------------------------------------------------------
Beschreibung: Grundinitialiserung für die Asteroiden-Layer, muss einmalig (in main)
              aufgerufen werden.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: AsteroidLayer[MAX_ASTEROIDLAYERS]
------------------------------------------------------------------------------*/
void InitAsteroidLayer(void) {
    int nLayer;
    int nAsteroid;

    for (nLayer = 0; nLayer < MAX_ASTEROIDLAYERS; nLayer++) {
        AsteroidLayer[nLayer].nCount = 0;      // Anzahl Asteroiden
        for (nAsteroid = 0; nAsteroid < MAX_ASTEROIDS; nAsteroid++) {
            AsteroidLayer[nLayer].Asteroid[nAsteroid].dAngleRotate = 0;     // Drehwinkel für Rotation
            AsteroidLayer[nLayer].Asteroid[nAsteroid].dRotationSpeed = 0;   // Drehgeschwindigkeit
            AsteroidLayer[nLayer].Asteroid[nAsteroid].nXpos = 0;            // Aktuelle X-Position
            AsteroidLayer[nLayer].Asteroid[nAsteroid].nYpos = 0;            // Aktuelle Y-Position
            AsteroidLayer[nLayer].Asteroid[nAsteroid].nXdeflection = 0;     // Aktuelle Y-Position
            AsteroidLayer[nLayer].Asteroid[nAsteroid].nYdeflection = 0;     // Aktuelle Y-Position
            AsteroidLayer[nLayer].Asteroid[nAsteroid].nXmaxDeflection = 0;  // Maximale X-Ablenkung
            AsteroidLayer[nLayer].Asteroid[nAsteroid].nYmaxDeflection = 0;  // Maximale Y-Ablenkung
            AsteroidLayer[nLayer].Asteroid[nAsteroid].fXangle = 0;          // Winkel für X-Ablenkung
            AsteroidLayer[nLayer].Asteroid[nAsteroid].fYangle = 0;          // Winkel für X-Ablenkung
            AsteroidLayer[nLayer].Asteroid[nAsteroid].fXangleSpeed = 0;     // Winkelgeschwindigkeit für X-Ablenkung
            AsteroidLayer[nLayer].Asteroid[nAsteroid].fYangleSpeed = 0;     // Winkelgeschwindigkeit für Y-Ablenkung
            AsteroidLayer[nLayer].Asteroid[nAsteroid].bReady = true;        // Asteroid ist am Zielort angekommen und kann neu geladen werden
            AsteroidLayer[nLayer].uLastCountIncreased = SDL_GetTicks();
            switch(nLayer) {
                case(0):
                    AsteroidLayer[nLayer].nMaxCount = 20;                   // Maximale Anzahl Asteroiden
                    AsteroidLayer[nLayer].Asteroid[nAsteroid].nWidth = 50;  // Breite des Asteroiden
                    AsteroidLayer[nLayer].Asteroid[nAsteroid].nHeight = 44; // Höhe des Asteroiden
                    AsteroidLayer[nLayer].Asteroid[nAsteroid].nSpeed = 1;   // Geschwindigkeit
                    break;
                case(1):
                    AsteroidLayer[nLayer].nMaxCount = 8;                    // Maximale Anzahl Asteroiden
                    AsteroidLayer[nLayer].Asteroid[nAsteroid].nWidth = 100; // Breite des Asteroiden
                    AsteroidLayer[nLayer].Asteroid[nAsteroid].nHeight = 88; // Höhe des Asteroiden
                    AsteroidLayer[nLayer].Asteroid[nAsteroid].nSpeed = 2;   // Geschwindigkeit
                    break;
                case(2):
                    AsteroidLayer[nLayer].nMaxCount = 3;                    // Maximale Anzahl Asteroiden
                    AsteroidLayer[nLayer].Asteroid[nAsteroid].nWidth = 200; // Breite des Asteroiden
                    AsteroidLayer[nLayer].Asteroid[nAsteroid].nHeight = 176;// Höhe des Asteroiden
                    AsteroidLayer[nLayer].Asteroid[nAsteroid].nSpeed = 3;   // Geschwindigkeit
                    break;
                default:
                    AsteroidLayer[nLayer].Asteroid[nAsteroid].nWidth = 25;  // Breite des Asteroiden
                    AsteroidLayer[nLayer].Asteroid[nAsteroid].nHeight = 22; // Höhe des Asteroiden
                    AsteroidLayer[nLayer].Asteroid[nAsteroid].nSpeed = 1;   // Geschwindigkeit
                    break;
            }
        }
    }
}


/*----------------------------------------------------------------------------
Name:           SetAsteroidDeflection
------------------------------------------------------------------------------
Beschreibung: Setzt die Sinus-Ablenkung für einen Asteroiden
Parameter
      Eingang: int, nLayer, Asteroidenlayer 0 bis MAX_ASTEROIDLAYERS
               int, nAsteroid, Asteroid 0 bis MAX_ASTEROIDS
               float, fXangleSpeed, Winkelgeschwindigkeit für X-Ablenkung
               float, fYangleSpeed, Winkelgeschwindigkeit für Y-Ablenkung
               int, nXmaxDeflection, Maximale X-Ablenkung
               int, nYmaxDeflection, Maximale Y-Ablenkung
      Ausgang: -
Rückgabewert:  int, 0 = OK, sonst Fehler
Seiteneffekte: AsteroidLayer[MAX_ASTEROIDLAYERS]
------------------------------------------------------------------------------*/
int SetAsteroidDeflection(int nLayer, int nAsteroid, float fXangleSpeed, float fYangleSpeed, int nXmaxDeflection, int nYmaxDeflection) {
    if ( (nLayer >= 0) && (nLayer < MAX_ASTEROIDLAYERS) && (nAsteroid >= 0) && (nAsteroid < MAX_ASTEROIDS) ) {
        AsteroidLayer[nLayer].Asteroid[nAsteroid].nXmaxDeflection = nXmaxDeflection;    // Maximale X-Ablenkung
        AsteroidLayer[nLayer].Asteroid[nAsteroid].nYmaxDeflection = nYmaxDeflection;    // Maximale Y-Ablenkung
        AsteroidLayer[nLayer].Asteroid[nAsteroid].fXangleSpeed = fXangleSpeed;          // Winkelgeschwindigkeit für X-Ablenkung
        AsteroidLayer[nLayer].Asteroid[nAsteroid].fYangleSpeed = fYangleSpeed;          // Winkelgeschwindigkeit für Y-Ablenkung
        return 0;
    } else {
        return -1;
    }
}


/*----------------------------------------------------------------------------
Name:           SwitchDrunkenAsteroids
------------------------------------------------------------------------------
Beschreibung: Setzt die Sinus-Ablenkung für einen Asteroiden
Parameter
      Eingang: bool, bOn, (De)aktiviert bei allen Asteroiden den Trunkenmodus
      Ausgang: -
Rückgabewert:  int, 0 = OK, sonst Fehler
Seiteneffekte: AsteroidLayer[MAX_ASTEROIDLAYERS]
------------------------------------------------------------------------------*/
int SwitchDrunkenAsteroids(bool bOn) {
    int nRet = 0;
    int nLayer;
    int nAsteroid;
    float fAngleSpeed;
    for (nLayer = 0; (nLayer < MAX_ASTEROIDLAYERS) && (nRet == 0); nLayer++) {    // Erstmal nur 1 Layer
        for (nAsteroid = 0; (nAsteroid < AsteroidLayer[nLayer].nMaxCount) && (nRet == 0); nAsteroid++) {
            if (bOn) {
                fAngleSpeed = randf(0.02,0.08);
                nRet = SetAsteroidDeflection(nLayer, nAsteroid, fAngleSpeed, fAngleSpeed, 50, 100);
            } else {
                nRet = SetAsteroidDeflection(nLayer, nAsteroid, 0.00, 0.00, 50, 100);
            }
        }
    }
    return nRet;
}


/*----------------------------------------------------------------------------
Name:           MoveAsteroids
------------------------------------------------------------------------------
Beschreibung: Bewegt die Asteroiden.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer des Windows
               pAsteroidTexture, SDL_Texture *, Zeiger auf Texture des Asteroiden

      Ausgang: -
Rückgabewert:  int, 0 = OK, sonst Fehler
Seiteneffekte: AsteroidLayer[MAX_ASTEROIDLAYERS]
------------------------------------------------------------------------------*/
int MoveAsteroids(SDL_Renderer *pRenderer,SDL_Texture *pAsteroidTexture) {
    SDL_Rect DestR_Asteroid;
    int nLayer;
    int nAsteroid;
    int nRet = -1;

    for (nLayer = 0; nLayer < 3; nLayer++) {    // Erstmal nur 1 Layer
        for (nAsteroid = 0; nAsteroid < AsteroidLayer[nLayer].nCount; nAsteroid++) {
            if (AsteroidLayer[nLayer].Asteroid[nAsteroid].bReady == true) {
                // Asteroid neu starten
                // Zunächst festlegen, ob Asteroid ganz links oder unten starten soll
                if ((rand() & 1) == 0) {
                    // Bei geraden Zahl ganz links, wir benötigen eine Zufallszahl für Y-Start
                    AsteroidLayer[nLayer].Asteroid[nAsteroid].nXpos = -200;
                    AsteroidLayer[nLayer].Asteroid[nAsteroid].nYpos = (int)randf(0,WINDOW_H);
                } else {
                    // Bei geraden Zahl ganz unten
                    AsteroidLayer[nLayer].Asteroid[nAsteroid].nXpos = (int)randf(0,WINDOW_W);
                    AsteroidLayer[nLayer].Asteroid[nAsteroid].nYpos = WINDOW_H + 200;
                }
                AsteroidLayer[nLayer].Asteroid[nAsteroid].dRotationSpeed = randf(-3,3); // Rotation-Geschwindigkeit kann negativ (Gegen-Uhrzeigersinn) werden
                AsteroidLayer[nLayer].Asteroid[nAsteroid].bReady = false;
            } else {
                AsteroidLayer[nLayer].Asteroid[nAsteroid].fXangle = AsteroidLayer[nLayer].Asteroid[nAsteroid].fXangle + AsteroidLayer[nLayer].Asteroid[nAsteroid].fXangleSpeed;
                AsteroidLayer[nLayer].Asteroid[nAsteroid].fYangle = AsteroidLayer[nLayer].Asteroid[nAsteroid].fYangle + AsteroidLayer[nLayer].Asteroid[nAsteroid].fYangleSpeed;
                AsteroidLayer[nLayer].Asteroid[nAsteroid].nXpos = AsteroidLayer[nLayer].Asteroid[nAsteroid].nXpos + AsteroidLayer[nLayer].Asteroid[nAsteroid].nSpeed;
                AsteroidLayer[nLayer].Asteroid[nAsteroid].nYpos = AsteroidLayer[nLayer].Asteroid[nAsteroid].nYpos - AsteroidLayer[nLayer].Asteroid[nAsteroid].nSpeed;
                AsteroidLayer[nLayer].Asteroid[nAsteroid].nXdeflection = sin(AsteroidLayer[nLayer].Asteroid[nAsteroid].fXangle) * AsteroidLayer[nLayer].Asteroid[nAsteroid].nXmaxDeflection;
                AsteroidLayer[nLayer].Asteroid[nAsteroid].nYdeflection = sin(AsteroidLayer[nLayer].Asteroid[nAsteroid].fYangle) * AsteroidLayer[nLayer].Asteroid[nAsteroid].nYmaxDeflection;

                DestR_Asteroid.x = AsteroidLayer[nLayer].Asteroid[nAsteroid].nXpos + AsteroidLayer[nLayer].Asteroid[nAsteroid].nXdeflection;
                DestR_Asteroid.y = AsteroidLayer[nLayer].Asteroid[nAsteroid].nYpos + AsteroidLayer[nLayer].Asteroid[nAsteroid].nYdeflection;
                DestR_Asteroid.w = AsteroidLayer[nLayer].Asteroid[nAsteroid].nWidth;
                DestR_Asteroid.h = AsteroidLayer[nLayer].Asteroid[nAsteroid].nHeight;

                // Asteroid drehen
                AsteroidLayer[nLayer].Asteroid[nAsteroid].dAngleRotate = AsteroidLayer[nLayer].Asteroid[nAsteroid].dAngleRotate + AsteroidLayer[nLayer].Asteroid[nAsteroid].dRotationSpeed;

                if ((AsteroidLayer[nLayer].Asteroid[nAsteroid].nXpos > (WINDOW_W + 400)) || (AsteroidLayer[nLayer].Asteroid[nAsteroid].nYpos < -400)) {
                    AsteroidLayer[nLayer].Asteroid[nAsteroid].bReady = true;
                }
                // Asteroid in Renderer kopieren
                // SDL_RenderCopy(pRenderer,pAsteroidTexture,NULL,&DestR_Asteroid);
                if (SDL_RenderCopyEx(pRenderer, pAsteroidTexture, NULL, &DestR_Asteroid, AsteroidLayer[nLayer].Asteroid[nAsteroid].dAngleRotate, NULL, SDL_FLIP_NONE) == 0) {
                    nRet = 0; // Alles OK
                } else {
                    SDL_Log("%s: SDL_RenderCopyEx() failed: %s",__FUNCTION__,SDL_GetError());
                }
            }
        }
        if ( (AsteroidLayer[nLayer].nCount < AsteroidLayer[nLayer].nMaxCount) && (SDL_GetTicks() -  AsteroidLayer[nLayer].uLastCountIncreased > 2000) ) {
            AsteroidLayer[nLayer].nCount++;
            AsteroidLayer[nLayer].uLastCountIncreased = SDL_GetTicks();
        }
    }
    return nRet;
}


