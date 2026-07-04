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
VECTOROBJ *Star[MAX_VECTOR_OBJECTS];
VECTOROBJ *Star2[MAX_VECTOR_OBJECTS];
uint32_t g_uObject;
uint32_t g_uObjectChangeTime;
float g_SizeSin = 0.0;

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

POINT3D StarPoints[20] = {
// Pentagramm, 5-Eck, oben
    {0, 1.5, 0.25},              // 0, Ecke 1
    {0.3368, 0.4635, 0.25},      // 1, Innenecke 1
    {1.4266, 0.4635, 0.25},      // 2, Ecke 2
    {0.5449, -0.1771, 0.25},     // 3, Innenecke 2
    {0.8817, -1.2135, 0.25},     // 4, Ecke 3
    {0, -0.5729, 0.25},          // 5, Innenecke 3
    {-0.8817, -1.2135, 0.25},    // 6, Ecke 4
    {-0.5449, -0.1771, 0.25},    // 7, Innenecke 4
    {-1.4266, 0.4635, 0.25},     // 8, Ecke 5
    {-0.3368, 0.4635, 0.25},     // 9, Innenecke 5
// Pentagramm, 5-Eck, unten
    {0, 1.5, -0.25},             // 10, Ecke 1
    {0.3368, 0.4635, -0.25},     // 11, Innenecke 1
    {1.4266, 0.4635, -0.25},     // 12, Ecke 2
    {0.5449, -0.1771, -0.25},    // 13, Innenecke 2
    {0.8817, -1.2135, -0.25},    // 14, Ecke 3
    {0, -0.5729, -0.25},         // 15, Innenecke 3
    {-0.8817, -1.2135, -0.25},   // 16, Ecke 4
    {-0.5449, -0.1771, -0.25},   // 17, Innenecke 4
    {-1.4266, 0.4635, -0.25},    // 18, Ecke 5
    {-0.3368, 0.4635, -0.25}     // 19, Innenecke 5
};

int32_t StarEdges[30][2] = {
    // Pentagramm-Verbindungen 2D, oben
    {0,1}, {1,2}, {2,3}, {3,4}, {4,5}, {5,6}, {6,7}, {7,8}, {8,9}, {9,0},
    // Pentagramm-Verbindungen 2D, unten
    {10,11}, {11,12}, {12,13}, {13,14}, {14,15}, {15,16}, {16,17}, {17,18}, {18,19}, {19,10},
    // Pentagramm-Streben zwischen oben und unten
    {0,10}, {1,11}, {2,12}, {3,13}, {4,14}, {5,15}, {6,16}, {7,17}, {8,18}, {9,19}
};


/////////////////////////////////////////////////////////////////////////////
// Stern von ChatGpt

POINT3D Star2Points[21] = {
// Basisring mit z = 0
    {12, 0, 0},                 // P0
    {7.482,  9.381, 0},         // P1
    {-2.670, 11.701, 0},        // P2
    {-10.812, 5.207, 0},        // P3
    {-10.812,-5.207, 0},        // P4
    {-2.670,-11.701, 0},        // P5
    { 7.482,-9.381, 0},         // P6
// Spitzen oben
    {30.000,  0.000, 12.5},     // P7
    {18.706, 23.454, 12.5},     // P8
    {-6.676, 29.247, 12.5},     // P9
    {-27.030, 13.017, 12.5},    // P10
    {-27.030,-13.017, 12.5},    // P11
    {-6.676,-29.247, 12.5},     // P12
    {18.706,-23.454, 12.5},     // P13
// Spitzen oben
    {30.000,  0.000, -12.5},
    {18.706, 23.454, -12.5},
    {-6.676, 29.247, -12.5},
    {-27.030, 13.017, -12.5},
    {-27.030,-13.017, -12.5},
    {-6.676,-29.247, -12.5},
    {18.706,-23.454, -12.5}
};

int32_t Star2Edges[1][2] = {
    {0,1}
};



/*----------------------------------------------------------------------------
Name:           InitVectorObjects
------------------------------------------------------------------------------
Beschreibung: Erzeugt und initialisiert verschiedene Vektorobjekte (Würfel, Rhombus).

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int32_t, 0 = Alles OK, sonst Fehler
Seiteneffekte: Cube[].x, Rhombus[].x, Star[].x, Star2[].x,
               CubePoints[], CubeEdges[],
               RhombusPoints[], RhombusEdges[],
               StarPoints[], StarEdges[],
               Star2Points[], Star2Edges[],
               Config.x, g_bObjectCube, g_uObjectChangeTime
------------------------------------------------------------------------------*/
int32_t InitVectorObjects(void) {
    uint32_t I;
    int32_t nErrorCode;

    for (I = 0; I < MAX_VECTOR_OBJECTS; I++) {
        Cube[I] = NULL;
        Rhombus[I] = NULL;
        Star[I] = NULL;
    }
    nErrorCode = 0;
    for (I = 0; (I < MAX_VECTOR_OBJECTS) && (nErrorCode == 0); I++) {
        Cube[I] = CreateVectorObject(8,12);
        Rhombus[I] = CreateVectorObject(10,24);
        Star[I] = CreateVectorObject(20,30);
        Star2[I] = CreateVectorObject(21,1);
        if ((Cube[I] != NULL) && (Rhombus[I] != NULL) && (Star[I] != NULL) && (Star2[I] != NULL)) {
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

            // Größe des Objekts
            Star[I]->nSize = VECTOR_OBJECT_SIZE;
            // Farben
            Star[I]->nRed = 0;
            Star[I]->nGreen = 0;
            Star[I]->nBlue = 0;
            // Rotationswinkel
            Star[I]->fAngleX = (float)I * 0.004;
            Star[I]->fAngleY = (float)I * 0.004;
            Star[I]->fAngleZ = (float)I * 0.004;
            // Position, Offset vom Mittelpunkt
            Star[I]->nXoffs = Config.uResX + 400;
            Star[I]->nYoffs = -400;
            // Bewegungsstatus
            Star[I]->uMovingState = VECTOROBJECT_MOVE_LEFT_DOWN;
            // Punkte und Kanten übergeben
            nErrorCode = SetVectorObjectPointsEdges(Star[I],StarPoints,StarEdges);

            // Größe des Objekts
            Star2[I]->nSize = VECTOR_OBJECT_SIZE;
            // Farben
            Star2[I]->nRed = 0;
            Star2[I]->nGreen = 0;
            Star2[I]->nBlue = 0;
            // Rotationswinkel
            Star2[I]->fAngleX = (float)I * 0.004;
            Star2[I]->fAngleY = (float)I * 0.004;
            Star2[I]->fAngleZ = (float)I * 0.004;
            // Position, Offset vom Mittelpunkt
            Star2[I]->nXoffs = Config.uResX + 400;
            Star2[I]->nYoffs = -400;
            // Bewegungsstatus
            Star2[I]->uMovingState = VECTOROBJECT_MOVE_LEFT_DOWN;
            // Punkte und Kanten übergeben
            nErrorCode = SetVectorObjectPointsEdges(Star2[I],Star2Points,Star2Edges);
        } else {
            nErrorCode = -1;
        }
    }
    g_uObject = 0; // 0 = Cube
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
Seiteneffekte: Cube[].x, Rhombus[].x, Star[].x
------------------------------------------------------------------------------*/
void FreeAllVectorObjects(void) {
    uint32_t I;

    for (I = 0; I < MAX_VECTOR_OBJECTS; I++) {
        FreeVectorObject(Cube[I]);
        FreeVectorObject(Rhombus[I]);
        FreeVectorObject(Star[I]);
        FreeVectorObject(Star2[I]);
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
    int32_t nLastXoffs;
    int32_t nLastYoffs;

    if (g_uObject == 0) {
        pV = Cube;
    } else if (g_uObject == 1) {
        pV = Rhombus;
    } else if (g_uObject == 2) {
        pV = Star;
    } else {
        pV = Star2;
    }

    nSizeForAll = VECTOR_OBJECT_SIZE - cos(g_SizeSin) * (VECTOR_OBJECT_SIZE - 10);
    g_SizeSin = g_SizeSin + 0.01;
    // Prüfen, ob Vektor-Objekt gewechselt werden soll
    if ((SDL_GetTicks() - g_uObjectChangeTime > 10000) && (nSizeForAll <= 10)) {
        g_uObject++;
        if (g_uObject > VECTOROBJECT_STAR) {
            g_uObject = VECTOROBJECT_CUBE;
        }
        g_uObjectChangeTime = SDL_GetTicks();
        // Position vom vorigen Objekt merken
        nLastXoffs = pV[0]->nXoffs;
        nLastYoffs = pV[0]->nYoffs;
        switch (g_uObject) {
            case (VECTOROBJECT_CUBE):
                pV = Cube;
                break;
            case (VECTOROBJECT_RHOMBUS):
                pV = Rhombus;
                break;
            case (VECTOROBJECT_STAR):
                pV = Star;
                break;
            case (VECTOROBJECT_STAR2):
                pV = Star2;
                break;
        }
        // Position des letzten Objekts dem Neuen zuweisen
        for (c = 0; (c < MAX_VECTOR_OBJECTS) && (nErrorCode == 0); c++) {
            pV[c]->nXoffs = nLastXoffs;
            pV[c]->nYoffs = nLastYoffs;
            pV[c]->nSize = nSizeForAll;
        }
    }
    for (c = 0; (c < MAX_VECTOR_OBJECTS) && (nErrorCode == 0); c++) {
        nErrorCode = DrawVectorObject(pV[c]);
        // Rotation
        if ((g_uObject == VECTOROBJECT_STAR) || (g_uObject == VECTOROBJECT_STAR2)) {
            pV[c]->fAngleX = pV[c]->fAngleX + 0.03;
            pV[c]->fAngleZ = pV[c]->fAngleZ + 0.005;
        } else {
            pV[c]->fAngleX = pV[c]->fAngleX + 0.01;
            pV[c]->fAngleY = pV[c]->fAngleY + 0.01;
            pV[c]->fAngleZ = pV[c]->fAngleZ + 0.005;
        }
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
