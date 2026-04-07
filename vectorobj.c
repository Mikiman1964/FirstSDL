#include  <math.h>
#include <SDL2/SDL.h>
#include "config.h"
#include "vectorobj.h"
#include "mySDL.h"
#include "mystd.h"


CUBE cube[MAX_CUBES];
extern CONFIG Config;
extern VIDEO Video;
// Hauptsächlich von deepseek

// Würfel-Eckpunkte (8 Punkte, Seitenlänge 2, Zentrum bei 0,0,0)
Point3D cubeVertices[] = {
//Sechseck
    {1,   3, 0}, // 0
    {3,   1, 0}, // 1
    {3,  -1,  0}, // 2
    {1,  -3,  0}, // 3
    {-1, -3, 0}, // 4
    {-3, -1, 0}, // 5
    {-3,  1,  0}, // 6
    {-1,  3,  0},  // 7
// Spitze oben
    {0,  0,  5}, // 8
// Spitze unten
    {0,  0,  -5} // 9

   /*
    {-1, -1, -1}, // 0
    { 1, -1, -1}, // 1
    { 1, -1,  1}, // 2
    {-1, -1,  1}, // 3
    {-1,  1, -1}, // 4
    { 1,  1, -1}, // 5
    { 1,  1,  1}, // 6
    {-1,  1,  1}  // 7
    */
};


// Kanten des Würfels (jede Kante verbindet zwei Eckpunkte)
/*
int32_t edges[12][2] = {
    {0,1}, {1,2}, {2,3}, {3,0}, // untere Fläche
    {4,5}, {5,6}, {6,7}, {7,4}, // obere Fläche
    {0,4}, {1,5}, {2,6}, {3,7}  // vertikale Kanten
*/

int32_t edges[24][2] = {
    //Streben zur oberen Spitze
    {0,8}, {1,8}, {2,8}, {3,8},
    {4,8}, {5,8}, {6,8}, {7,8},
    // Kanten des SechsecksSechseck
    {0,1}, {1,2}, {2,3}, {3,4},
    {4,5}, {5,6}, {6,7}, {7,0},
    // Streben zur unteren Spitze


    //Streben zur oberen Spitze
    {0,9}, {1,9}, {2,9}, {3,9},
    {4,9}, {5,9}, {6,9}, {7,9},


    };


// Funktion: 3D-Punkt um X-Achse rotieren
Point3D rotateX(Point3D p, float angle) {
    Point3D result;
    result.x = p.x;
    result.y = p.y * cos(angle) - p.z * sin(angle);
    result.z = p.y * sin(angle) + p.z * cos(angle);
    return result;
}


// Funktion: 3D-Punkt um Y-Achse rotieren
Point3D rotateY(Point3D p, float angle) {
    Point3D result;
    result.x = p.x * cos(angle) + p.z * sin(angle);
    result.y = p.y;
    result.z = -p.x * sin(angle) + p.z * cos(angle);
    return result;
}

// Funktion: 3D-Punkt um Z-Achse rotieren
Point3D rotateZ(Point3D p, float angle) {
    Point3D result;
    result.x = p.x * cos(angle) - p.y * sin(angle);
    result.y = p.x * sin(angle) + p.y * cos(angle);
    result.z = p.z;
    return result;
}

// Funktion: 3D-Punkt auf 2D-Bildschirm projizieren (einfache perspektivische Projektion)
Point2D project(Point3D p, int32_t nWidth, int32_t nHeight) {
    Point2D result;
    result.x = (int32_t)(nWidth / 2 + p.x * nWidth / 4);
    result.y = (int32_t)(nHeight / 2 - p.y * nHeight / 4);
    return result;
}

// Funktion: Würfel zeichnen
int32_t DrawCube(CUBE Cube) {
    Point2D projected[10];
    int32_t i;
    int32_t nErrorCode;

    // Alle Eckpunkte rotieren und projizieren
    for (i = 0; i < 10; i++) {
        Point3D p = cubeVertices[i];
        p = rotateX(p, Cube.dAngleX);
        p = rotateY(p, Cube.dAngleY);
        p = rotateZ(p, Cube.dAngleZ);
        projected[i] = project(p,Cube.nSize,Cube.nSize);
    }
    // Alle Kanten zeichnen
    nErrorCode = 0;
    for (i = 0; (i < 24) && (nErrorCode == 0); i++) {
        int32_t v1 = edges[i][0];
        int32_t v2 = edges[i][1];

        // Farbe für Line setzen
        if (SDL_SetRenderDrawColor(Video.pRenderer,Cube.nRed,Cube.nGreen,Cube.nBlue,SDL_ALPHA_OPAQUE) == 0) {
            if (SDL_RenderDrawLine(Video.pRenderer,projected[v1].x + Cube.nXoffs, projected[v1].y + Cube.nYoffs, projected[v2].x + Cube.nXoffs, projected[v2].y + Cube.nYoffs) == 0) {
                nErrorCode = SDL_SetRenderDrawColor(Video.pRenderer,0,0,0, SDL_ALPHA_OPAQUE);  // Farbe für Line setzen
            } else {
                nErrorCode = -1;
            }
        } else {
            nErrorCode = -1;
        }
    }
    return nErrorCode;
}


void InitCubes(void) {
    int32_t c;

    for (c = 0; c < MAX_CUBES; c++) {
        // Größe der Würfel
        cube[c].nSize = CUBE_SIZE;
        // Farben
        cube[c].nRed = 0;
        cube[c].nGreen = 0;
        cube[c].nBlue = 0;
        // Rotationswinkel
        cube[c].dAngleX = (float)c * 0.004;
        cube[c].dAngleY = (float)c * 0.004;
        cube[c].dAngleZ = (float)c * 0.004;
        // Position, Offset vom Mittelpunkt
        cube[c].nXoffs = -400;
        cube[c].nYoffs = -400;
        // Bewegungsstatus
        cube[c].uMovingState = CUBE_MOVE_RIGHT_DOWN;
    }
}


int32_t DoCubes(void) {
    int32_t c;
    int32_t nErrorCode = 0;
    int32_t nSizeForAll;
    uint32_t uRGB;

     nSizeForAll = CUBE_SIZE - cos(cube[0].dAngleX * 2) * (CUBE_SIZE - 10);

    //nSizeForAll = 200;

    for (c = 0; (c < MAX_CUBES) && (nErrorCode == 0); c++) {
        nErrorCode = DrawCube(cube[c]);
        // Rotation
        cube[c].dAngleX = cube[c].dAngleX + 0.01;
        cube[c].dAngleY = cube[c].dAngleY + 0.01;
        cube[c].dAngleZ = cube[c].dAngleZ + 0.005;
        // Größe
        cube[c].nSize = nSizeForAll;
        // Farben
        uRGB = GetRainbowColors((Video.uFrameCounter % 254) * 5 + 10 * c);
        cube[c].nRed = (uint8_t)(uRGB >> 16);
        cube[c].nGreen = (uint8_t)(uRGB >> 8);
        cube[c].nBlue = (uint8_t)(uRGB);
        // Bewegung
        switch (cube[c].uMovingState) {
            case (CUBE_MOVE_RIGHT_DOWN):
                cube[c].nXoffs = cube[c].nXoffs + 2;
                cube[c].nYoffs = cube[c].nYoffs + 2;
                if (cube[c].nXoffs > ((int32_t)(Config.uResX) - (cube[c].nSize / 2))) { // Würfel stößt rechts an
                    cube[c].uMovingState = CUBE_MOVE_LEFT_DOWN;
                } else if (cube[c].nYoffs > ((int32_t)(Config.uResY) - (cube[c].nSize / 2)))  {    // Würfel stößt unten an
                    cube[c].uMovingState = CUBE_MOVE_RIGHT_UP;
                }
                break;
            case (CUBE_MOVE_RIGHT_UP):
                cube[c].nXoffs = cube[c].nXoffs + 2;
                cube[c].nYoffs = cube[c].nYoffs - 2;
                if (cube[c].nXoffs > ((int32_t)(Config.uResX) - (cube[c].nSize / 2))) { // Würfel stößt rechts an
                    cube[c].uMovingState = CUBE_MOVE_LEFT_UP;
                } else if (cube[c].nYoffs < -(cube[c].nSize / 4)) {    // Würfel stößt oben an
                    cube[c].uMovingState = CUBE_MOVE_RIGHT_DOWN;
                }
                break;
            case (CUBE_MOVE_LEFT_UP):
                cube[c].nXoffs = cube[c].nXoffs - 2;
                cube[c].nYoffs = cube[c].nYoffs - 2;
                if (cube[c].nXoffs < -(cube[c].nSize / 4)) { // Würfel stößt links an
                    cube[c].uMovingState = CUBE_MOVE_RIGHT_UP;
                } else if (cube[c].nYoffs < -(cube[c].nSize / 4)) {    // Würfel stößt oben an
                    cube[c].uMovingState = CUBE_MOVE_LEFT_DOWN;
                }
                break;
            case (CUBE_MOVE_LEFT_DOWN):
                cube[c].nXoffs = cube[c].nXoffs - 2;
                cube[c].nYoffs = cube[c].nYoffs + 2;
                if (cube[c].nXoffs < -(cube[c].nSize / 4)) { // Würfel stößt links an
                    cube[c].uMovingState = CUBE_MOVE_RIGHT_DOWN;
                } else if (cube[c].nYoffs > ((int32_t)(Config.uResY) - (cube[c].nSize / 2)))  {    // Würfel stößt unten an
                    cube[c].uMovingState = CUBE_MOVE_LEFT_UP;
                }
                break;
            default:
                break;
        }
    }
    return nErrorCode;
}
