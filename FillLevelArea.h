#ifndef FILLLEVELAREA_H_INCLUDED
#define FILLLEVELAREA_H_INCLUDED


// Bei Levelgröße 1000 x 1000 werden bis zu 2.500.000 Elemente benötigt
#define FILLSTACK_SIZE      (MAX_LEVEL_W * MAX_LEVEL_H * 3) // aktuell 3.000.000 Elemente

// Für die iterative Füllroutine
typedef struct {
    uint32_t uX;
    uint32_t uY;
} KOORDS;


typedef struct {
    KOORDS Koords[FILLSTACK_SIZE];
    uint32_t uP;
    uint32_t uMaxP;
} FILLSTACK;

void InitFillStack(void);
int32_t PushFillstack(uint32_t uX, uint32_t uY);
int32_t PopFillstack(uint32_t *puX, uint32_t *puY);
int32_t FillLevelArea(uint32_t uX, uint32_t uY,uint16_t uFillElement,uint16_t uGroundElement);


#endif // FILLLEVELAREA_H_INCLUDED
