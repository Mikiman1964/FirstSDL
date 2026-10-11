#ifndef VECTOROBJ_H_INCLUDED
#define VECTOROBJ_H_INCLUDED

#include <stdint.h>

#define MAX_VECTOR_OBJECTS                  15
#define VECTOR_OBJECT_SIZE                  500

#define VECTOROBJECT_MOVE_RIGHT_DOWN        0
#define VECTOROBJECT_MOVE_RIGHT_UP          1
#define VECTOROBJECT_MOVE_LEFT_UP           2
#define VECTOROBJECT_MOVE_LEFT_DOWN         3


#define VECTOROBJECT_CUBE                   0
#define VECTOROBJECT_RHOMBUS                1
#define VECTOROBJECT_STAR                   2
#define VECTOROBJECT_STAR2                  3


// 3D-Punkt Struktur
typedef struct {
    float fX, fY, fZ;
} POINT3D;


// 2D-Punkt Struktur (für Bildschirmprojektion)
typedef struct {
    int32_t nX, nY;
} POINT2D;


typedef struct {
    // Anzahl Punkte
    uint32_t uPointCount;
    // Anzahl Kanten
    uint32_t uEdgeCount;
    // Punkte
    POINT3D *pPoints3D;
    POINT2D *pPoints2D; // Für Bildschirmprojektion
    // Kanten
    int32_t *pEdges;

    // Größe des Objekts
    int32_t nSize;
    // Farbe des Würfels
    int32_t nRed;
    int32_t nGreen;
    int32_t nBlue;
    // Rotationswinkel
    float fAngleX;
    float fAngleY;
    float fAngleZ;
    // Position des Würfels, Offset zum Mittelpunkt
    int32_t nXoffs;
    int32_t nYoffs;
    // Bewegungsstatus
    uint8_t uMovingState;
} VECTOROBJ;



int32_t InitVectorObjects(void);
VECTOROBJ *CreateVectorObject(uint32_t uPointCount, uint32_t uEdgeCount);
void FreeVectorObject(VECTOROBJ *pV);
int32_t SetVectorObjectPointsEdges(VECTOROBJ *pV, POINT3D *pPoints3D, int32_t Edges[][2]);
int32_t DrawVectorObject(VECTOROBJ *pV);
int32_t DoVectorObject(void);
void FreeAllVectorObjects(void);


// Prototypen
POINT3D RotateX(POINT3D p, float fAngle);
POINT3D RotateY(POINT3D p, float fAngle);
POINT3D RotateZ(POINT3D p, float fAngle);
POINT2D Project(POINT3D p, int32_t nWidth, int32_t nHeight);

#endif // VECTOROBJ_H_INCLUDED
