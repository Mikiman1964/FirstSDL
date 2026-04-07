#ifndef VECTOROBJ_H_INCLUDED
#define VECTOROBJ_H_INCLUDED

#include <stdint.h>

#define MAX_CUBES                   20
#define CUBE_SIZE                   600

#define CUBE_MOVE_RIGHT_DOWN        0
#define CUBE_MOVE_RIGHT_UP          1
#define CUBE_MOVE_LEFT_UP           2
#define CUBE_MOVE_LEFT_DOWN         3



// Hauptsächlich on deepseek

// 3D-Punkt Struktur
typedef struct {
    float x, y, z;
} Point3D;


// 2D-Punkt Struktur (für Bildschirmprojektion)
typedef struct {
    int32_t x, y;
} Point2D;


// Cube-Daten
typedef struct {
    // Größe des Würfels
    int32_t nSize;
    // Farbe des Würfels
    int32_t nRed;
    int32_t nGreen;
    int32_t nBlue;
    // Rotationswinkel
    float dAngleX;
    float dAngleY;
    float dAngleZ;
    // Position des Würfels, Offset zum Mittelpunkt
    int32_t nXoffs;
    int32_t nYoffs;
    // Bewegungsstatus
    uint8_t uMovingState;
} CUBE;


// Prototypen
Point3D rotateX(Point3D p, float angle);
Point3D rotateY(Point3D p, float angle);
Point3D rotateZ(Point3D p, float angle);
Point2D project(Point3D p, int32_t nWidth, int32_t nHeight);
int32_t DrawCube(CUBE Cube);
void InitCubes(void);
int32_t DoCubes(void);

#endif // VECTOROBJ_H_INCLUDED
