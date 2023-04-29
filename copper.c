#include <SDL2/SDL.h>
#include "config.h"
#include "copper.h"
#include "loadlevel.h"
#include "mySDL.h"
#include "mystd.h"


int g_nPrecalculatedSegmentCount;                                       // Zählt die bereits durchgescrollten vorberechneten Segmente
int g_nColorPattern;
bool g_bReady;


uint32_t uCopperVisibleSegments;


COPPERSEGMENT *VisibleCopperSegments;                                    // Die sichtbaren Segmente;
COPPERSEGMENT PrecalculatedSegments[COPPER_PRECALCULATED_SEGMENTS];     // Vorberechnete Segmente
COLORFLOW ColorFlow[(COPPER_COLORLINES_PER_SEGMENT * 2) - 1];           // Vorberechneter Farbverlauf (dunkel -> hell -> dunkel) für aktuelles Color-Pattern

extern CONFIG Config;

uint8_t g_ColorPatterns[] = {
//                           R    G    B
                            0x00,0xFF,0xFF,         // türkis
                            0xFF,0xFF,0x00,         // gelb
                            0x00,0x00,0xFF,         // blau
                            0x00,0xFF,0x00,         // grün
                            0xFF,0x00,0x00,         // rot
                            0xFF,0x00,0xFF,         // violett
                            0xFF,0xFF,0xFF,         // weiß
                            0x00,0x00,0x00          // Ende-Kennung
                            };


/*----------------------------------------------------------------------------
Name:           InitVisibibleCopperSegments
------------------------------------------------------------------------------
Beschreibung: Initialisiert alle sichtbaren Farbsegmente auf schwarz und die
              Hilfsvariablen für den Farbscroller.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte: g_nPrecalculatedSegmentCount, g_nColorPattern, g_bReady,
               VisibleCopperSegments, uCopperVisibleSegments, Config.x
------------------------------------------------------------------------------*/
int InitVisibibleCopperSegments() {
    int nErrorCode;

    nErrorCode = -1;
    uCopperVisibleSegments = ((Config.uResY + 20) / COPPER_COLORLINES_PER_SEGMENT);     // sichtbare Segmente
    g_nPrecalculatedSegmentCount = 0;   // Zählt die bereits durchgescrollten vorberechneten Segmente
    g_nColorPattern = 0;                // Aktuelle Scroll-Farbe (Maximalwerte)
    g_bReady = true;                    // Es wurden alle vorberechneten Segmente durchgescrollt bzw. Neustart
    VisibleCopperSegments = malloc(sizeof(COPPERSEGMENT) * uCopperVisibleSegments);
    if (VisibleCopperSegments != NULL) {
        memset(VisibleCopperSegments,0,sizeof(COPPERSEGMENT) * uCopperVisibleSegments); // Alle sichtbaren Segmente auf schwarz setzen
        nErrorCode = 0;
    } else {
        SDL_Log("%s: malloc() failed",__FUNCTION__);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           CalculateColorFlow
------------------------------------------------------------------------------
Beschreibung: Berechnet einen ansteigenden und abfallenden Farbverlauf.
                ( Beispiel 6 Farben, 0 = dunkel, 5 = maximale Helligkeit
                    0
                    1
                    2
                    3
                    4
                    5
                    4
                    3
                    2
                    1
                    0
Parameter
      Eingang: nMaxRed, int, maximaler Rotanteil
               nMaxGreen, int, maximaler Grünanteil
               nMaxRBlue, int, maximaler Blauanteil
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: ColorFlow[]
------------------------------------------------------------------------------*/
void CalculateColorFlow(int nMaxRed, int nMaxGreen, int nMaxBlue) {
    int nColor;
    int nColorIndex;

    nColorIndex = 0;
    nColor = 0;
    // Zunächst bis Farb-Maximum berechnen
    do {
        ColorFlow[nColorIndex].nRed = nColor * nMaxRed / COPPER_COLORLINES_PER_SEGMENT;
        ColorFlow[nColorIndex].nGreen = nColor * nMaxGreen / COPPER_COLORLINES_PER_SEGMENT;
        ColorFlow[nColorIndex].nBlue = nColor * nMaxBlue / COPPER_COLORLINES_PER_SEGMENT;
        //SDL_Log("%s   write Index: %03d    R:%03d   G: %03d   B:%03d",__FUNCTION__,nColorIndex,ColorFlow[nColorIndex].nRed,ColorFlow[nColorIndex].nGreen,ColorFlow[nColorIndex].nBlue);
        nColorIndex++;
        nColor++;
    } while (nColorIndex < COPPER_COLORLINES_PER_SEGMENT);
    nColor--;
    nColor--;
    // Ab hier abfallender Farbverlauf
    do {
        ColorFlow[nColorIndex].nRed = nColor * nMaxRed / COPPER_COLORLINES_PER_SEGMENT;
        ColorFlow[nColorIndex].nGreen = nColor * nMaxGreen / COPPER_COLORLINES_PER_SEGMENT;
        ColorFlow[nColorIndex].nBlue = nColor * nMaxBlue / COPPER_COLORLINES_PER_SEGMENT;
        //SDL_Log("%s   write Index: %03d    R:%03d   G: %03d   B:%03d",__FUNCTION__,nColorIndex,ColorFlow[nColorIndex].nRed,ColorFlow[nColorIndex].nGreen,ColorFlow[nColorIndex].nBlue);
        nColorIndex++;
        nColor--;
    } while (nColorIndex < (COPPER_COLORLINES_PER_SEGMENT * 2) - 1);
}


/*----------------------------------------------------------------------------
Name:           PrecalculateSegments
------------------------------------------------------------------------------
Beschreibung: Berechnet für den Segmentscroller die anzuzeigenden Segmente vor.
Parameter
      Eingang: nMaxRed, int, maximaler Rotanteil
               nMaxGreen, int, maximaler Grünanteil
               nMaxRBlue, int, maximaler Blauanteil
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: ColorFlow[], PrecalculatedSegments[]
------------------------------------------------------------------------------*/
void PrecalculateSegments(int nMaxRed, int nMaxGreen, int nMaxBlue) {
    int nSegment;
    int nTakeColorsFromColorFlow;
    int nLine;

    memset(&PrecalculatedSegments,0,sizeof(PrecalculatedSegments));
    CalculateColorFlow(nMaxRed,nMaxGreen,nMaxBlue);
    nSegment = 0;
    nTakeColorsFromColorFlow = 1;
    // Zunächst aufsteigene Segmente berechnen
    do {
        for (nLine = 0; nLine < nTakeColorsFromColorFlow; nLine++) {
            // SDL_Log("%s   segment: %d     Copperline: %d    ColorFlow: %d",__FUNCTION__,nSegment,COPPER_COLORLINES_PER_SEGMENT - nTakeColorsFromColorFlow + nLine,nLine);
            PrecalculatedSegments[nSegment].CopperLine[COPPER_COLORLINES_PER_SEGMENT - nTakeColorsFromColorFlow + nLine].nRed = ColorFlow[nLine].nRed;
            PrecalculatedSegments[nSegment].CopperLine[COPPER_COLORLINES_PER_SEGMENT - nTakeColorsFromColorFlow + nLine].nGreen = ColorFlow[nLine].nGreen;
            PrecalculatedSegments[nSegment].CopperLine[COPPER_COLORLINES_PER_SEGMENT - nTakeColorsFromColorFlow + nLine].nBlue = ColorFlow[nLine].nBlue;
        }
        nTakeColorsFromColorFlow++;
        nSegment++;
    } while (nSegment < COPPER_COLORLINES_PER_SEGMENT);

    // Ab hier absteigende Segmente  berechnen
    nTakeColorsFromColorFlow = 1;
    do {
        for (nLine = 0; nLine < COPPER_COLORLINES_PER_SEGMENT; nLine++) {

            if (nLine + nTakeColorsFromColorFlow < ((COPPER_COLORLINES_PER_SEGMENT * 2) - 1)) {
                // SDL_Log("%s   segment: %d      ColorFlow: %d",__FUNCTION__,nSegment,nLine + nTakeColorsFromColorFlow);
                PrecalculatedSegments[nSegment].CopperLine[nLine].nRed = ColorFlow[nLine + nTakeColorsFromColorFlow].nRed;
                PrecalculatedSegments[nSegment].CopperLine[nLine].nGreen = ColorFlow[nLine + nTakeColorsFromColorFlow].nGreen;
                PrecalculatedSegments[nSegment].CopperLine[nLine].nBlue = ColorFlow[nLine + nTakeColorsFromColorFlow].nBlue;
            }
        }
        nTakeColorsFromColorFlow++;
        nSegment++;
    } while (nSegment < COPPER_PRECALCULATED_SEGMENTS);
}


/*----------------------------------------------------------------------------
Name:           ScrollVisibleSegments
------------------------------------------------------------------------------
Beschreibung: Scrollt die sichtbaren Segmente nach oben und fügt unten ein
              vorberechnetes Segment neu ein.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: VisibleCopperSegments[], PrecalculatedSegments[],
                g_nPrecalculatedSegmentCount, g_bReady, uCopperVisibleSegments
------------------------------------------------------------------------------*/
void ScrollVisibleSegments(void) {
    int nSegment;

    // Sichtbare Segmente scrollen
    for (nSegment = 0; nSegment < uCopperVisibleSegments - 1; nSegment++) {
        VisibleCopperSegments[nSegment] = VisibleCopperSegments[nSegment + 1];
    }
    // und neues vorberechnetes Segment unten einfügen
    VisibleCopperSegments[nSegment] = PrecalculatedSegments[g_nPrecalculatedSegmentCount];
    g_nPrecalculatedSegmentCount++;
    if (g_nPrecalculatedSegmentCount > COPPER_PRECALCULATED_SEGMENTS - 1) {
        g_nPrecalculatedSegmentCount = 0;
        g_bReady = true;              // Es wurden alle Segemente durchgescrollt.
    }
}


/*----------------------------------------------------------------------------
Name:           DoCopper
------------------------------------------------------------------------------
Beschreibung: Führt die Coppersimulation auf und ruft das Segmentscrolling auf.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
               bScroll, bool, true = Scrollen, false = nicht scrollen
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: VisibleCopperSegments[], g_ColorPatterns[]
               g_nColorPattern, g_bReady, uCopperVisibleSegments, Config.x
------------------------------------------------------------------------------*/
int DoCopper(SDL_Renderer *pRenderer,bool bScroll) {
    int nSegment;
    int nLine;
    int nRed;
    int nGreen;
    int nBlue;
    int nYpos;
    int nRet;

    if (!bScroll) {
        return 0;
    }
    if (g_bReady) {
        nRed = g_ColorPatterns[0 + g_nColorPattern * 3];
        nGreen = g_ColorPatterns[1 + g_nColorPattern * 3];
        nBlue = g_ColorPatterns[2 + g_nColorPattern * 3];
        if ((nRed | nGreen | nBlue) == 0) {     // Endekennung ?
            g_nColorPattern = 0;
            nRed = g_ColorPatterns[0 + g_nColorPattern * 3];
            nGreen = g_ColorPatterns[1 + g_nColorPattern * 3];
            nBlue = g_ColorPatterns[2 + g_nColorPattern * 3];
        }
        g_nColorPattern++;
        PrecalculateSegments(nRed,nGreen,nBlue);
        g_bReady = false;
    }
    ScrollVisibleSegments();
    // Copperzeilen erzeugen/malen
    nRet = 0;
    for (nSegment = 0; (nSegment < uCopperVisibleSegments) && (nRet == 0); nSegment++) {
        for (nLine = 0; (nLine < COPPER_COLORLINES_PER_SEGMENT) && (nRet == 0); nLine++) {
            nRed = VisibleCopperSegments[nSegment].CopperLine[nLine].nRed;
            nGreen = VisibleCopperSegments[nSegment].CopperLine[nLine].nGreen;
            nBlue = VisibleCopperSegments[nSegment].CopperLine[nLine].nBlue;
            if (SDL_SetRenderDrawColor(pRenderer,nRed,nGreen,nBlue, SDL_ALPHA_OPAQUE) == 0) {  // Farbe für Line setzen
                nYpos = nSegment * COPPER_COLORLINES_PER_SEGMENT + nLine;
                if (SDL_RenderDrawLine(pRenderer, 0, nYpos, Config.uResX - 1, nYpos) != 0) {
                    nRet = -1;
                    SDL_Log("%s: SDL_RenderDrawLine() failed: %sd",__FUNCTION__,SDL_GetError());
                }
            } else {
                nRet = -1;
                SDL_Log("%s: SDL_SetRenderDrawColor() failed: %s",__FUNCTION__,SDL_GetError());
            }
        }
    }
    if (nRet == 0) {
        if (SDL_SetRenderDrawColor(pRenderer,0,0,0, SDL_ALPHA_OPAQUE) != 0) {  // Muss am Ende stehen, damit Hintergrund wieder dunkel wird
            nRet = -1;
            SDL_Log("%s: SDL_SetRenderDrawColor() failed: %s",__FUNCTION__,SDL_GetError());
        }
    }
    return nRet;
}


/*----------------------------------------------------------------------------
Name:           FreeCopper
------------------------------------------------------------------------------
Beschreibung: Gibt den Speicher für die sichtbaren Segmente wieder frei.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: VisibleCopperSegments
------------------------------------------------------------------------------*/
void FreeCopper(void) {
    SAFE_FREE(VisibleCopperSegments);
}
