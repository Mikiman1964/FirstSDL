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
      Eingang: nX, int32_t, X-Position
               nY, int32_t, Y-Position
      Ausgang: -
Rückgabewert:  int32_t, 0 = OK, sonst Fehler
Seiteneffekte: Fillstack.x
------------------------------------------------------------------------------*/
int32_t PushFillstack(int32_t nX, int32_t nY) {
    if (Fillstack.uP < FILLSTACK_SIZE) {
        Fillstack.Koords[Fillstack.uP].nX = nX;
        Fillstack.Koords[Fillstack.uP].nY = nY;
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
      Eingang: pnX, int32_t *, X-Position
               pnY, int32_t *, Y-Position
      Ausgang: -
Rückgabewert:  int32_t, 0 = OK, sonst Fehler
Seiteneffekte: Fillstack.x
------------------------------------------------------------------------------*/
int32_t PopFillstack(int32_t *pnX, int32_t *pnY) {
    if ((pnX != NULL) && (pnY != NULL) && (Fillstack.uP > 0)) {
        Fillstack.uP--;
        *pnX = Fillstack.Koords[Fillstack.uP].nX;
        *pnY = Fillstack.Koords[Fillstack.uP].nY;
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
      Eingang: nX, int32_t, X-Position im Level
               nY, int32_t, Y-Position im Level
               uFillElement, uint16_t, Element, mit dem gefüllt werden soll
               uGroundElement, uint16_t, Grund-Element, dass ersetzt werden soll
Rückgabewert:  int32_t, 0 = alles OK, sonst Fehler
Seiteneffekte: Ed.x
------------------------------------------------------------------------------*/
int32_t FillLevelArea(int32_t nX,int32_t nY,uint16_t uFillElement,uint16_t uGroundElement) {
    int32_t nErrorCode;

    InitFillStack();
    nErrorCode = PushFillstack(nX,nY);
    while ((Fillstack.uP > 0) && (nErrorCode == 0)) {
        nErrorCode = PopFillstack(&nX,&nY);
        if (nErrorCode != 0) {
            return -1;
        }
        if (!((nX > 0) && (nX < (Ed.uLevel_X_Dimension - 1)) && (nY > 0) && (nY < (Ed.uLevel_Y_Dimension - 1)))) {
            continue;
        }
        if (Ed.pLevel[Ed.uLevel_X_Dimension * nY + nX] == uFillElement) {
            continue;
        }
        if (Ed.pLevel[Ed.uLevel_X_Dimension * nY + nX] == uGroundElement) {
            Ed.pLevel[Ed.uLevel_X_Dimension * nY + nX] = uFillElement;
        } else {
            continue;
        }
        nErrorCode = -1;
        if (PushFillstack(nX + 1,nY) == 0) {
            if (PushFillstack(nX - 1,nY) == 0) {
                if (PushFillstack(nX,nY + 1) == 0) {
                    if (PushFillstack(nX,nY - 1) == 0) {
                        nErrorCode = 0;
                    }
                }
            }
        }
    }
    // SDL_Log("MaxP = %u",Fillstack.uMaxP);
    return nErrorCode;
}
