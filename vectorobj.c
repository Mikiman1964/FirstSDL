#include  <math.h>
#include <SDL2/SDL.h>
#include "config.h"
#include "vectorobj.h"
#include "mySDL.h"
#include "mystd.h"


extern VIDEO Video;
extern CONFIG Config;
VECTOROBJ *Cube[MAX_VECTOR_OBJECTS];
VECTOROBJ *Rhombus[MAX_VECTOR_OBJECTS];
bool g_bObjectCube;
uint32_t g_uObjectChangeTime;


// Würfel-Eckpunkte (8 Punkte, Seitenlänge 2, Zentrum bei 0,0,0)
POINT3D CubePoints[8] = {
 // Würfel
    {-1, -1, -1}, // 0
    { 1, -1, -1}, // 1
    { 1, -1,  1}, // 2
    {-1, -1,  1}, // 3
    {-1,  1, -1}, // 4
    { 1,  1, -1}, // 5
    { 1,  1,  1}, // 6
    {-1,  1,  1}  // 7
};

// Kanten des Würfels (jede Kante verbindet zwei Eckpunkte)
int32_t CubeEdges[12][2] = {
    {0,1}, {1,2}, {2,3}, {3,0}, // untere Fläche
    {4,5}, {5,6}, {6,7}, {7,4}, // obere Fläche
    {0,4}, {1,5}, {2,6}, {3,7}  // vertikale Kanten
};

/////////////////////////////////////////////////////////////////////////////

POINT3D RhombusPoints[10] = {
// Sechseck
    {1,   3, 0}, // 0
    {3,   1, 0}, // 1
    {3,  -1, 0}, // 2
    {1,  -3, 0}, // 3
    {-1, -3, 0}, // 4
    {-3, -1, 0}, // 5
    {-3,  1, 0}, // 6
    {-1,  3, 0}, // 7
// Spitze oben
    {0,  0,  5}, // 8
// Spitze unten
    {0,  0,  -5} // 9
};

int32_t RhombusEdges[24][2] = {
    //Streben zur oberen Spitze
    {0,8}, {1,8}, {2,8}, {3,8},
    {4,8}, {5,8}, {6,8}, {7,8},
    // Kanten des SechsecksSechseck
    {0,1}, {1,2}, {2,3}, {3,4},
    {4,5}, {5,6}, {6,7}, {7,0},
    // Streben zur unteren Spitze
    {0,9}, {1,9}, {2,9}, {3,9},
    {4,9}, {5,9}, {6,9}, {7,9},
};
/////////////////////////////////////////////////////////////////////////////


/*----------------------------------------------------------------------------
Name:           InitVectorObjects
------------------------------------------------------------------------------
Beschreibung: Erzeugt und initialisiert verschiedene Vektorobjekte (Würfel, Rhombus).

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int32_t, 0 = Alles OK, sonst Fehler
Seiteneffekte: Cube[].x, Rhombus[].x,
               CubePoints[],CubeEdges[],
               RhombusPoints[],RhombusEdges
               Config.x, g_bObjectCube, g_uObjectChangeTime
------------------------------------------------------------------------------*/
int32_t InitVectorObjects(void) {
    uint32_t I;
    int32_t nErrorCode;

    for (I = 0; I < MAX_VECTOR_OBJECTS; I++) {
        Cube[I] = NULL;
        Rhombus[I] = NULL;
    }
    nErrorCode = 0;
    for (I = 0; (I < MAX_VECTOR_OBJECTS) && (nErrorCode == 0); I++) {
        Cube[I] = CreateVectorObject(8,12);
        Rhombus[I] = CreateVectorObject(10,24);
        if ((Cube[I] != NULL) && (Rhombus[I] != NULL)) {
            // Größe des Objekts
            Cube[I]->nSize = VECTOR_OBJECT_SIZE;
            // Farben
            Cube[I]->nRed = 0;
            Cube[I]->nGreen = 0;
            Cube[I]->nBlue = 0;
            // Rotationswinkel
            Cube[I]->fAngleX = (float)I * 0.004;
            Cube[I]->fAngleY = (float)I * 0.004;
            Cube[I]->fAngleZ = (float)I * 0.004;
            // Position, Offset vom Mittelpunkt
            Cube[I]->nXoffs = -400;
            Cube[I]->nYoffs = -400;
            // Bewegungsstatus
            Cube[I]->uMovingState = VECTOROBJECT_MOVE_RIGHT_DOWN;
            // Punkte und Kanten übergeben
            nErrorCode = SetVectorObjectPointsEdges(Cube[I],CubePoints,CubeEdges);

            // Größe des Objekts
            Rhombus[I]->nSize = VECTOR_OBJECT_SIZE;
            // Farben
            Rhombus[I]->nRed = 0;
            Rhombus[I]->nGreen = 0;
            Rhombus[I]->nBlue = 0;
            // Rotationswinkel
            Rhombus[I]->fAngleX = (float)I * 0.004;
            Rhombus[I]->fAngleY = (float)I * 0.004;
            Rhombus[I]->fAngleZ = (float)I * 0.004;
            // Position, Offset vom Mittelpunkt
            Rhombus[I]->nXoffs = Config.uResX + 400;
            Rhombus[I]->nYoffs = -400;
            // Bewegungsstatus
            Rhombus[I]->uMovingState = VECTOROBJECT_MOVE_LEFT_DOWN;
            // Punkte und Kanten übergeben
            nErrorCode = SetVectorObjectPointsEdges(Rhombus[I],RhombusPoints,RhombusEdges);
        } else {
            nErrorCode = -1;
        }
    }
    g_bObjectCube = true;
    g_uObjectChangeTime = SDL_GetTicks();
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           CreateVectorObject
------------------------------------------------------------------------------
Beschreibung: Erstellt ein Vektorobjekt.

Parameter
      Eingang: nPointCount, int32_t, Anzahl der 3D-Punkte
               nEdgeCount, int32_t_ Anzahl der Kanten
      Ausgang: -
Rückgabewert:  VECTOROBJ *, NULL = Fehler, sonst Zeiger auf Vektorobjekt
Seiteneffekte: -
------------------------------------------------------------------------------*/
VECTOROBJ *CreateVectorObject(int32_t nPointCount, int32_t nEdgeCount) {
    VECTOROBJ *pV;

    pV = NULL;
    if ((nPointCount >= 1) && (nEdgeCount >= 0)) {
        // Eigentliches Objekt erzeugen
        pV = malloc(sizeof(VECTOROBJ));
        if (pV != NULL) {
            memset(pV,0,sizeof(VECTOROBJ));
            pV->pPoints3D = malloc(sizeof(POINT3D) * nPointCount);
            pV->pPoints2D = malloc(sizeof(POINT2D) * nPointCount);
            if (nEdgeCount > 0) {
                pV->pEdges = malloc(sizeof(int32_t) * 2 * nEdgeCount); // *2 wegen 2 Werte pro Kante
            }
            if ((pV->pPoints3D) && (pV->pPoints2D)) {
                pV->nPointCount = nPointCount;
                pV->nEdgeCount = nEdgeCount;
                // Default ist weißes Objekt
                pV->nRed = 255;
                pV->nGreen = 255;
                pV->nBlue = 255;
            } else {
                SAFE_FREE(pV->pPoints3D);
                SAFE_FREE(pV->pPoints2D);
                SAFE_FREE(pV);
            }
        }
    }
    return pV;
}


/*----------------------------------------------------------------------------
Name:           FreeVectorObject
------------------------------------------------------------------------------
Beschreibung: Gibt ein Vektorobjekt wieder frei.

Parameter
      Eingang: pV, VECTOROBJ *, Zeiger auf Vektorobjekt
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: -
------------------------------------------------------------------------------*/
void FreeVectorObject(VECTOROBJ *pV) {
    if (pV != NULL) {
        SAFE_FREE(pV->pPoints3D);
        SAFE_FREE(pV->pPoints2D);
        SAFE_FREE(pV->pEdges);
        SAFE_FREE(pV);
    }
}


/*----------------------------------------------------------------------------
Name:           FreeAllVectorObjects
------------------------------------------------------------------------------
Beschreibung: Gibt alle  Vektorobjekte wieder frei.

Parameter
      Eingang: pV, VECTOROBJ *, Zeiger auf Vektorobjekt
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Cube[].x, Rhombus[].x
------------------------------------------------------------------------------*/
void FreeAllVectorObjects(void) {
    uint32_t I;

    for (I = 0; I < MAX_VECTOR_OBJECTS; I++) {
        FreeVectorObject(Cube[I]);
        FreeVectorObject(Rhombus[I]);
    }
}


/*----------------------------------------------------------------------------
Name:           SetVectorObjectPointsEdges
------------------------------------------------------------------------------
Beschreibung: Setzt die Punkte und Kanten eines Vektorobjekts.

Parameter
      Eingang: pV, VECTOROBJ *, Zeiger auf Vektorobjekt
               pPoints3D, POINT3D *, Zeiger auf 3D-Punkte
               pEdges, int32_t[][2], Zeiger auf Kanten, darf NULL sein, wenn Anzahl Kanten im V-Objekt = 0
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: -
------------------------------------------------------------------------------*/
int32_t SetVectorObjectPointsEdges(VECTOROBJ *pV, POINT3D *pPoints3D, int32_t Edges[][2]) {
    int32_t nErrorCode;
    uint32_t I;

    nErrorCode = -1;
    if ((pV != NULL) && (pPoints3D != NULL)) {
        if (((pV->nEdgeCount > 0) && (pV->pEdges != NULL)) || (pV->nEdgeCount == 0)) {
            //3D-Punkte in Vektorobjekt kopieren
            for (I = 0; I < pV->nPointCount; I++) {
                pV->pPoints3D[I] = pPoints3D[I];
            }
            // Kanten in Vektorobjekt kopieren
            for (I = 0; I < pV->nEdgeCount; I++) {
                pV->pEdges[I * 2 + 0] = Edges[I][0];
                pV->pEdges[I * 2 + 1] = Edges[I][1];
            }
            nErrorCode = 0;
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           DrawVectorObject
------------------------------------------------------------------------------
Beschreibung: Zeichnet ein Vektorobjekt.

Parameter
      Eingang: pV, VECTOROBJ *, Zeiger auf Vektorobjekt
      Ausgang: -
Rückgabewert:  int32_t, 0 = Alles OK, sonst Fehler
Seiteneffekte: Video.x
------------------------------------------------------------------------------*/
int32_t DrawVectorObject(VECTOROBJ *pV) {
    int32_t nErrorCode = -1;;
    uint32_t I;

    if (pV != NULL) {
        // Alle 3D-Punkte rotieren und projizieren
        for (I = 0; I < pV->nPointCount; I++) {
            POINT3D p = pV->pPoints3D[I];
            p = RotateX(p, pV->fAngleX);
            p = RotateY(p, pV->fAngleY);
            p = RotateZ(p, pV->fAngleZ);
            pV->pPoints2D[I] = Project(p,pV->nSize,pV->nSize);
        }
        // Alle Kanten zeichnen
        nErrorCode = 0;
        for (I = 0; (I < pV->nEdgeCount) && (nErrorCode == 0); I++) {
            int32_t v1 = pV->pEdges[I * 2 + 0];
            int32_t v2 = pV->pEdges[I * 2 + 1];
            // Farbe für Line setzen
            if (SDL_SetRenderDrawColor(Video.pRenderer,pV->nRed,pV->nGreen,pV->nBlue,SDL_ALPHA_OPAQUE) == 0) {
                nErrorCode = SDL_RenderDrawLine(Video.pRenderer,pV->pPoints2D[v1].nX + pV->nXoffs, pV->pPoints2D[v1].nY + pV->nYoffs, pV->pPoints2D[v2].nX + pV->nXoffs, pV->pPoints2D[v2].nY + pV->nYoffs);
            } else {
                nErrorCode = -1;
            }
        }
        if (nErrorCode == 0) {
            nErrorCode = SDL_SetRenderDrawColor(Video.pRenderer,0,0,0, SDL_ALPHA_OPAQUE);  // Farbe für Renderer setzen
        }
    }
    return nErrorCode;
}


// Funktion: 3D-Punkt um X-Achse rotieren
POINT3D RotateX(POINT3D p, float fAngle) {
    POINT3D result;
    result.fX = p.fX;
    result.fY = p.fY * cos(fAngle) - p.fZ * sin(fAngle);
    result.fZ = p.fY * sin(fAngle) + p.fZ * cos(fAngle);
    return result;
}


// Funktion: 3D-Punkt um Y-Achse rotieren
POINT3D RotateY(POINT3D p, float fAngle) {
    POINT3D result;
    result.fX = p.fX * cos(fAngle) + p.fZ * sin(fAngle);
    result.fY = p.fY;
    result.fZ = -p.fX * sin(fAngle) + p.fZ * cos(fAngle);
    return result;
}

// Funktion: 3D-Punkt um Z-Achse rotieren
POINT3D RotateZ(POINT3D p, float fAngle) {
    POINT3D result;
    result.fX = p.fX * cos(fAngle) - p.fY * sin(fAngle);
    result.fY = p.fX * sin(fAngle) + p.fY * cos(fAngle);
    result.fZ = p.fZ;
    return result;
}

// Funktion: 3D-Punkt auf 2D-Bildschirm projizieren (einfache perspektivische Projektion)
POINT2D Project(POINT3D p, int32_t nWidth, int32_t nHeight) {
    POINT2D result;
    result.nX = (int32_t)(nWidth / 2 + p.fX * nWidth / 4);
    result.nY = (int32_t)(nHeight / 2 - p.fY * nHeight / 4);
    return result;
}


int32_t DoVectorObject(void) {
    VECTOROBJ **pV;
    int32_t c;
    int32_t nErrorCode = 0;
    int32_t nSizeForAll;
    uint32_t uRGB;

    if (g_bObjectCube) {
        pV = Cube;
    } else {
        pV = Rhombus;
    }

    nSizeForAll = VECTOR_OBJECT_SIZE - cos(pV[0]->fAngleX * 2) * (VECTOR_OBJECT_SIZE - 10);
    // Prüfen, ob Objekt (Würfel/Rhombus) gewechselt werden soll
    if ((SDL_GetTicks() - g_uObjectChangeTime > 10000) && (nSizeForAll <= 10)) {
        g_bObjectCube = !g_bObjectCube;
        g_uObjectChangeTime = SDL_GetTicks();
        if (g_bObjectCube) {
            pV = Cube;
            for (c = 0; (c < MAX_VECTOR_OBJECTS) && (nErrorCode == 0); c++) {
                pV[c]->nXoffs = Rhombus[0]->nXoffs;
                pV[c]->nYoffs = Rhombus[0]->nYoffs;
                pV[c]->nSize = nSizeForAll;
            }
        } else {
            pV = Rhombus;
            for (c = 0; (c < MAX_VECTOR_OBJECTS) && (nErrorCode == 0); c++) {
                pV[c]->nXoffs= Cube[0]->nXoffs;
                pV[c]->nYoffs= Cube[0]->nYoffs;
                pV[c]->nSize = nSizeForAll;
            }
        }
    }
    for (c = 0; (c < MAX_VECTOR_OBJECTS) && (nErrorCode == 0); c++) {
        nErrorCode = DrawVectorObject(pV[c]);
        // Rotation
        pV[c]->fAngleX = pV[c]->fAngleX + 0.01;
        pV[c]->fAngleY = pV[c]->fAngleY + 0.01;
        pV[c]->fAngleZ = pV[c]->fAngleZ + 0.005;
        // Größe
        pV[c]->nSize = nSizeForAll;
        // Farben
        uRGB = GetRainbowColors((Video.uFrameCounter % 254) * 5 + 10 * c);
        pV[c]->nRed = (uint8_t)(uRGB >> 16);
        pV[c]->nGreen = (uint8_t)(uRGB >> 8);
        pV[c]->nBlue = (uint8_t)(uRGB);
        // Bewegung
        switch (pV[c]->uMovingState) {
            case (VECTOROBJECT_MOVE_RIGHT_DOWN):
                pV[c]->nXoffs = pV[c]->nXoffs + 2;
                pV[c]->nYoffs = pV[c]->nYoffs + 2;
                if (pV[c]->nXoffs > ((int32_t)(Config.uResX) - (pV[c]->nSize / 2))) { // Objekt stößt rechts an
                    pV[c]->uMovingState = VECTOROBJECT_MOVE_LEFT_DOWN;
                } else if (pV[c]->nYoffs > ((int32_t)(Config.uResY) - (pV[c]->nSize / 2)))  {    // Objekt stößt unten an
                    pV[c]->uMovingState = VECTOROBJECT_MOVE_RIGHT_UP;
                }
                break;
            case (VECTOROBJECT_MOVE_RIGHT_UP):
                pV[c]->nXoffs = pV[c]->nXoffs + 2;
                pV[c]->nYoffs = pV[c]->nYoffs - 2;
                if (pV[c]->nXoffs > ((int32_t)(Config.uResX) - (pV[c]->nSize / 2))) { // Objekt stößt rechts an
                    pV[c]->uMovingState = VECTOROBJECT_MOVE_LEFT_UP;
                } else if (pV[c]->nYoffs < -(pV[c]->nSize / 4)) {    // Objekt stößt oben an
                    pV[c]->uMovingState = VECTOROBJECT_MOVE_RIGHT_DOWN;
                }
                break;
            case (VECTOROBJECT_MOVE_LEFT_UP):
                pV[c]->nXoffs = pV[c]->nXoffs - 2;
                pV[c]->nYoffs = pV[c]->nYoffs - 2;
                if (pV[c]->nXoffs < -(pV[c]->nSize / 4)) { // Objekt stößt links an
                    pV[c]->uMovingState = VECTOROBJECT_MOVE_RIGHT_UP;
                } else if (pV[c]->nYoffs < -(pV[c]->nSize / 4)) {    // Objekt stößt oben an
                    pV[c]->uMovingState = VECTOROBJECT_MOVE_LEFT_DOWN;
                }
                break;
            case (VECTOROBJECT_MOVE_LEFT_DOWN):
                pV[c]->nXoffs = pV[c]->nXoffs - 2;
                pV[c]->nYoffs = pV[c]->nYoffs + 2;
                if (pV[c]->nXoffs < -(pV[c]->nSize / 4)) { // Objekt stößt links an
                    pV[c]->uMovingState = VECTOROBJECT_MOVE_RIGHT_DOWN;
                } else if (pV[c]->nYoffs > ((int32_t)(Config.uResY) - (pV[c]->nSize / 2)))  {    // Objekt stößt unten an
                    pV[c]->uMovingState = VECTOROBJECT_MOVE_LEFT_UP;
                }
                break;
            default:
                break;
        }
    }
    return 0;
}
