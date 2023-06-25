#include "config.h"
#include "loadlevel.h"
#include "mySDL.h"
#include "mystd.h"
#include "smileys.h"

SMILEY Smileys[MAX_SMILEYS];
uint32_t g_uNextWaitTime;
uint32_t g_uLastSmileyRotation;
bool g_bT;  // Für rechts/links-Drall


extern CONFIG Config;

/*----------------------------------------------------------------------------
Name:           InitSmileys
------------------------------------------------------------------------------
Beschreibung: Initialisiert alles Smileys.
Parameter
      Eingang: -
Rückgabewert:  -
Seiteneffekte: Smileys.x, g_uLastSmileyRotation, g_uNextWaitTime
------------------------------------------------------------------------------*/
void InitSmileys(void) {
    uint32_t I;

    g_bT = false;
    memset(Smileys,0,sizeof(Smileys));
    for (I = 0; I < MAX_SMILEYS; I++) {
        Smileys[I].bReady = true;
        Smileys[I].fAngleRotate = 0;
        Smileys[I].fRotationSpeed = 0.1;
    }
    g_uLastSmileyRotation = SDL_GetTicks();
    g_uNextWaitTime = 1000;
}


/*----------------------------------------------------------------------------
Name:           BornSmileys
------------------------------------------------------------------------------
Beschreibung: Erzeugt neue Smileys.
Parameter
      Eingang: -
Rückgabewert:  -
Seiteneffekte: Smileys.x, Config.x
------------------------------------------------------------------------------*/
void BornSmileys(void) {
    uint32_t I;

    for (I = 0; I < MAX_SMILEYS; I++) {
        if (Smileys[I].bReady) {

            Smileys[I].fXSpeed = 0;
            Smileys[I].fYSpeed = 0;
            Smileys[I].fAngleRotate = 0;
            while ((Smileys[I].fXSpeed == 0) && (Smileys[I].fYSpeed == 0)) {
                Smileys[I].fXSpeed = randn(0,5);
                if (randn(0,1) == 0) {
                    Smileys[I].fXSpeed = -Smileys[I].fXSpeed;
                }
                Smileys[I].fYSpeed = randn(0,5);
                if (randn(0,1) == 0) {
                    Smileys[I].fYSpeed = -Smileys[I].fYSpeed;
                }
            }
            Smileys[I].fXpos = Config.uResX / 2;
            Smileys[I].fYpos = Config.uResY / 2;
            Smileys[I].fSize = 0;
            Smileys[I].bReady = false;
        }
    }
}


/*----------------------------------------------------------------------------
Name:           MoveSmileys
------------------------------------------------------------------------------
Beschreibung: Bewegt alle Smileys.
Parameter
      Eingang: -
Rückgabewert:  -
Seiteneffekte: Smileys.x, g_bT, Config.x, g_uNextWaitTime, g_uLastSmileyRotation
------------------------------------------------------------------------------*/
int MoveSmileys(SDL_Renderer *pRenderer) {
    int nErrorCode;
    uint32_t I;
    SDL_Rect DestR;                     // Zum Kopieren in den Renderer
    float fAngle = 0;
    float fTmp;
    float fM,fN;

    nErrorCode = 0;
    BornSmileys();
    for (I = 0; (I < MAX_SMILEYS) && (nErrorCode == 0); I++) {
        if (!Smileys[I].bReady) {
            // Position innerhalb des Renderers
            DestR.x = Smileys[I].fXpos;
            DestR.y = Smileys[I].fYpos;
            DestR.w = 64 * Smileys[I].fSize;
            DestR.h = DestR.w;
            if (SDL_RenderCopyEx(pRenderer,GetTextureByIndex(718),NULL,&DestR,fAngle,NULL, SDL_FLIP_NONE) != 0) {
                SDL_Log("%s: SDL_RenderCopyEx(standard) failed: %s",__FUNCTION__,SDL_GetError());
                nErrorCode = -1;
            }
            Smileys[I].fXpos = Smileys[I].fXpos + Smileys[I].fXSpeed;
            Smileys[I].fYpos = Smileys[I].fYpos + Smileys[I].fYSpeed;
            Smileys[I].fSize = Smileys[I].fSize + 0.003;

            Smileys[I].fAngleRotate = Smileys[I].fAngleRotate + Smileys[I].fRotationSpeed;


            // Smiley außerhalb des Sichtbereichs?
            if ( (Smileys[I].fXpos > (Config.uResX + 200)) || (Smileys[I].fXpos < - 200) || (Smileys[I].fYpos > (Config.uResY + 200)) || (Smileys[I].fYpos < - 200) ) {
                Smileys[I].bReady = true;
            }
        }
    }

    if (SDL_GetTicks() - g_uLastSmileyRotation > g_uNextWaitTime) {
        g_uNextWaitTime = randn(200,5000);
        g_bT = !g_bT;   // Für rechts/links-Drall
        if (g_bT) {
            fM = randf(1,4);
            fN = randf(-4,-1);
        } else {
            fM = randf(-4,-1);
            fN = randf(1,4);
        }
        g_uLastSmileyRotation = SDL_GetTicks();
        for (I = 0; I < MAX_SMILEYS; I++) {
            fTmp = Smileys[I].fXSpeed;
            Smileys[I].fXSpeed = Smileys[I].fYSpeed * fM;
            Smileys[I].fYSpeed = fTmp * fN;
        }
    }
    return nErrorCode;
}

