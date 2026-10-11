#include <stdio.h>
#include "editor.h"
#include "EmeraldMine.h"
#include "FillLevelArea.h"


// Quelle: https://dev.to/codeguppy/flood-fill-recursion-or-no-recursion-3fop
// Wir benutzen hier die nicht-rekursive Variante
FILLSTACK Fillstack;
extern ED Ed;


/*----------------------------------------------------------------------------
Name:           InitFillStack
------------------------------------------------------------------------------
Beschreibung: Initialisiert den Fillstack für die Level-Füllroutine.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Fillstack.x
------------------------------------------------------------------------------*/
void InitFillStack(void) {
    Fillstack.uP = 0;
    Fillstack.uMaxP = 0;
}


/*----------------------------------------------------------------------------
Name:           PushFillstack
------------------------------------------------------------------------------
Beschreibung: Legt eine Koordinate auf dem Fillstack ab.
Parameter
      Eingang: uX, uint32_t, X-Position
               uY, uint32_t, Y-Position
      Ausgang: -
Rückgabewert:  int32_t, 0 = OK, sonst Fehler
Seiteneffekte: Fillstack.x
------------------------------------------------------------------------------*/
int32_t PushFillstack(uint32_t uX, uint32_t uY) {
    if (Fillstack.uP < FILLSTACK_SIZE) {
        Fillstack.Koords[Fillstack.uP].uX = uX;
        Fillstack.Koords[Fillstack.uP].uY = uY;
        Fillstack.uP++;
        if (Fillstack.uP > Fillstack.uMaxP) {
            Fillstack.uMaxP = Fillstack.uP;
        }
        return 0;
    } else {
        SDL_Log("%s: Heap Overflow",__FUNCTION__);
        return -1;
    }
}


/*----------------------------------------------------------------------------
Name:           PopFillstack
------------------------------------------------------------------------------
Beschreibung: Holt eine Koordinate aus dem Fillstack ab.
Parameter
      Eingang: puX, uint32_t *, X-Position
               puY, uint32_t *, Y-Position
      Ausgang: -
Rückgabewert:  int32_t, 0 = OK, sonst Fehler
Seiteneffekte: Fillstack.x
------------------------------------------------------------------------------*/
int32_t PopFillstack(uint32_t *puX, uint32_t *puY) {
    if ((puX != NULL) && (puY != NULL) && (Fillstack.uP > 0)) {
        Fillstack.uP--;
        *puX = Fillstack.Koords[Fillstack.uP].uX;
        *puY = Fillstack.Koords[Fillstack.uP].uY;
        return 0;
    } else {
        SDL_Log("%s: Heap Underflow",__FUNCTION__);
        return -1;
    }
}


/*----------------------------------------------------------------------------
Name:           FillLevelArea
------------------------------------------------------------------------------
Beschreibung: Füllt einen (begrenzten) Levelbereich mit einem bestimmten Element.
              Die Routine bzw. Idee stammt von: https://dev.to/codeguppy/flood-fill-recursion-or-no-recursion-3fop
                    // Flood fill algorithm implemented with a stack on the heap
                    // This algorithm will also work with big size matrixes
                    var fillStack = [];
                    function fillMatrix2(matrix, row, col)
                    {
                        fillStack.push([row, col]);

                        while(fillStack.length > 0)
                        {
                            var [row, col] = fillStack.pop();

                            if (!validCoordinates(matrix, row, col))
                                continue;

                            if (matrix[row][col] == 1)
                                continue;

                            matrix[row][col] = 1;

                            fillStack.push([row + 1, col]);
                            fillStack.push([row - 1, col]);
                            fillStack.push([row, col + 1]);
                            fillStack.push([row, col - 1]);
                        }
                    }
Parameter
      Eingang: uX, uint32_t, X-Position im Level
               uY, uint32_t, Y-Position im Level
               uFillElement, uint16_t, Element, mit dem gefüllt werden soll
               uGroundElement, uint16_t, Grund-Element, dass ersetzt werden soll
Rückgabewert:  int32_t, 0 = alles OK, sonst Fehler
Seiteneffekte: Ed.x
------------------------------------------------------------------------------*/
int32_t FillLevelArea(uint32_t uX,uint32_t uY,uint16_t uFillElement,uint16_t uGroundElement) {
    int32_t nErrorCode;

    InitFillStack();
    nErrorCode = PushFillstack(uX,uY);
    while ((Fillstack.uP > 0) && (nErrorCode == 0)) {
        nErrorCode = PopFillstack(&uX,&uY);
        if (nErrorCode != 0) {
            return -1;
        }
        if (!((uX > 0) && (uX < (Ed.uLevel_X_Dimension - 1)) && (uY > 0) && (uY < (Ed.uLevel_Y_Dimension - 1)))) {
            continue;
        }
        if (Ed.pLevel[Ed.uLevel_X_Dimension * uY + uX] == uFillElement) {
            continue;
        }
        if (Ed.pLevel[Ed.uLevel_X_Dimension * uY + uX] == uGroundElement) {
            Ed.pLevel[Ed.uLevel_X_Dimension * uY + uX] = uFillElement;
        } else {
            continue;
        }
        nErrorCode = -1;
        if (PushFillstack(uX + 1,uY) == 0) {
            if (PushFillstack(uX - 1,uY) == 0) {
                if (PushFillstack(uX,uY + 1) == 0) {
                    if (PushFillstack(uX,uY - 1) == 0) {
                        nErrorCode = 0;
                    }
                }
            }
        }
    }
    // SDL_Log("MaxP = %u",Fillstack.uMaxP);
    return nErrorCode;
}
