#include <SDL2/SDL.h>
#include "config.h"
#include "copper.h"
#include "loadlevel.h"
#include "mySDL.h"
#include "mystd.h"


uint32_t g_uPrecalculatedSegmentCount;                                       // Zählt die bereits durchgescrollten vorberechneten Segmente
uint32_t g_uColorPattern;
uint32_t uCopperVisibleSegments;
bool g_bReady;





COPPERSEGMENT *VisibleCopperSegments;                                    // Die sichtbaren Segmente;
COPPERSEGMENT PrecalculatedSegments[COPPER_PRECALCULATED_SEGMENTS];     // Vorberechnete Segmente
COLORFLOW ColorFlow[(COPPER_COLORLINES_PER_SEGMENT * 2) - 1];           // Vorberechneter Farbverlauf (dunkel -> hell -> dunkel) für aktuelles Color-Pattern

extern CONFIG Config;
extern VIDEO Video;

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
Rückgabewert:  int32_t, 0 = Alles OK, sonst Fehler
Seiteneffekte: g_uPrecalculatedSegmentCount, g_uColorPattern, g_bReady,
               VisibleCopperSegments, uCopperVisibleSegments, Config.x
------------------------------------------------------------------------------*/
int32_t InitVisibibleCopperSegments(void) {
    int32_t nErrorCode;

    nErrorCode = -1;
    uCopperVisibleSegments = ((Config.uResY + 20) / COPPER_COLORLINES_PER_SEGMENT);     // sichtbare Segmente
    g_uPrecalculatedSegmentCount = 0;   // Zählt die bereits durchgescrollten vorberechneten Segmente
    g_uColorPattern = 0;                // Aktuelle Scroll-Farbe (Maximalwerte)
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
      Eingang: nMaxRed, int32_t, maximaler Rotanteil
               nMaxGreen, int32_t, maximaler Grünanteil
               nMaxRBlue, int32_t, maximaler Blauanteil
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: ColorFlow[]
------------------------------------------------------------------------------*/
void CalculateColorFlow(int32_t nMaxRed, int32_t nMaxGreen, int32_t nMaxBlue) {
    int32_t nColor;
    int32_t nColorIndex;

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
      Eingang: nMaxRed, int32_t, maximaler Rotanteil
               nMaxGreen, int32_t, maximaler Grünanteil
               nMaxRBlue, int32_t, maximaler Blauanteil
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: ColorFlow[], PrecalculatedSegments[]
------------------------------------------------------------------------------*/
void PrecalculateSegments(int32_t nMaxRed, int32_t nMaxGreen, int32_t nMaxBlue) {
    uint32_t uSegment;
    int32_t nTakeColorsFromColorFlow;
    int32_t nLine;

    memset(&PrecalculatedSegments,0,sizeof(PrecalculatedSegments));
    CalculateColorFlow(nMaxRed,nMaxGreen,nMaxBlue);
    uSegment = 0;
    nTakeColorsFromColorFlow = 1;
    // Zunächst aufsteigene Segmente berechnen
    do {
        for (nLine = 0; nLine < nTakeColorsFromColorFlow; nLine++) {
            // SDL_Log("%s   segment: %d     Copperline: %d    ColorFlow: %d",__FUNCTION__,uSegment,COPPER_COLORLINES_PER_SEGMENT - nTakeColorsFromColorFlow + nLine,nLine);
            PrecalculatedSegments[uSegment].CopperLine[COPPER_COLORLINES_PER_SEGMENT - nTakeColorsFromColorFlow + nLine].nRed = ColorFlow[nLine].nRed;
            PrecalculatedSegments[uSegment].CopperLine[COPPER_COLORLINES_PER_SEGMENT - nTakeColorsFromColorFlow + nLine].nGreen = ColorFlow[nLine].nGreen;
            PrecalculatedSegments[uSegment].CopperLine[COPPER_COLORLINES_PER_SEGMENT - nTakeColorsFromColorFlow + nLine].nBlue = ColorFlow[nLine].nBlue;
        }
        nTakeColorsFromColorFlow++;
        uSegment++;
    } while (uSegment < COPPER_COLORLINES_PER_SEGMENT);

    // Ab hier absteigende Segmente  berechnen
    nTakeColorsFromColorFlow = 1;
    do {
        for (nLine = 0; nLine < COPPER_COLORLINES_PER_SEGMENT; nLine++) {

            if (nLine + nTakeColorsFromColorFlow < ((COPPER_COLORLINES_PER_SEGMENT * 2) - 1)) {
                // SDL_Log("%s   segment: %d      ColorFlow: %d",__FUNCTION__,uSegment,nLine + nTakeColorsFromColorFlow);
                PrecalculatedSegments[uSegment].CopperLine[nLine].nRed = ColorFlow[nLine + nTakeColorsFromColorFlow].nRed;
                PrecalculatedSegments[uSegment].CopperLine[nLine].nGreen = ColorFlow[nLine + nTakeColorsFromColorFlow].nGreen;
                PrecalculatedSegments[uSegment].CopperLine[nLine].nBlue = ColorFlow[nLine + nTakeColorsFromColorFlow].nBlue;
            }
        }
        nTakeColorsFromColorFlow++;
        uSegment++;
    } while (uSegment < COPPER_PRECALCULATED_SEGMENTS);
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
                g_uPrecalculatedSegmentCount, g_bReady, uCopperVisibleSegments
------------------------------------------------------------------------------*/
void ScrollVisibleSegments(void) {
    uint32_t uSegment;

    // Sichtbare Segmente scrollen
    for (uSegment = 0; uSegment < uCopperVisibleSegments - 1; uSegment++) {
        VisibleCopperSegments[uSegment] = VisibleCopperSegments[uSegment + 1];
    }
    // und neues vorberechnetes Segment unten einfügen
    VisibleCopperSegments[uSegment] = PrecalculatedSegments[g_uPrecalculatedSegmentCount];
    g_uPrecalculatedSegmentCount++;
    if (g_uPrecalculatedSegmentCount > COPPER_PRECALCULATED_SEGMENTS - 1) {
        g_uPrecalculatedSegmentCount = 0;
        g_bReady = true;              // Es wurden alle Segemente durchgescrollt.
    }
}


/*----------------------------------------------------------------------------
Name:           DoCopper
------------------------------------------------------------------------------
Beschreibung: Führt die Coppersimulation auf und ruft das Segmentscrolling auf.
Parameter
      Eingang: bScroll, bool, true = Scrollen, false = nicht scrollen
      Ausgang: -
Rückgabewert:  int32_t, 0 = OK, sonst Fehler
Seiteneffekte: VisibleCopperSegments[], g_ColorPatterns[]
               g_uColorPattern, g_bReady, uCopperVisibleSegments, Config.x, Video.x
------------------------------------------------------------------------------*/
int32_t DoCopper(bool bScroll) {
    uint32_t uSegment;
    int32_t nLine;
    int32_t nRed;
    int32_t nGreen;
    int32_t nBlue;
    int32_t nYpos;
    int32_t nRet;

    if (!bScroll) {
        return 0;
    }
    if (g_bReady) {
        nRed = g_ColorPatterns[0 + g_uColorPattern * 3];
        nGreen = g_ColorPatterns[1 + g_uColorPattern * 3];
        nBlue = g_ColorPatterns[2 + g_uColorPattern * 3];
        if ((nRed | nGreen | nBlue) == 0) {     // Endekennung ?
            g_uColorPattern = 0;
            nRed = g_ColorPatterns[0 + g_uColorPattern * 3];
            nGreen = g_ColorPatterns[1 + g_uColorPattern * 3];
            nBlue = g_ColorPatterns[2 + g_uColorPattern * 3];
        }
        g_uColorPattern++;
        PrecalculateSegments(nRed,nGreen,nBlue);
        g_bReady = false;
    }
    ScrollVisibleSegments();
    // Copperzeilen erzeugen/malen
    nRet = 0;
    for (uSegment = 0; (uSegment < uCopperVisibleSegments) && (nRet == 0); uSegment++) {
        for (nLine = 0; (nLine < COPPER_COLORLINES_PER_SEGMENT) && (nRet == 0); nLine++) {
            nRed = VisibleCopperSegments[uSegment].CopperLine[nLine].nRed;
            nGreen = VisibleCopperSegments[uSegment].CopperLine[nLine].nGreen;
            nBlue = VisibleCopperSegments[uSegment].CopperLine[nLine].nBlue;
            if (SDL_SetRenderDrawColor(Video.pRenderer,nRed,nGreen,nBlue, SDL_ALPHA_OPAQUE) == 0) {  // Farbe für Line setzen
                nYpos = uSegment * COPPER_COLORLINES_PER_SEGMENT + nLine;
                if (SDL_RenderDrawLine(Video.pRenderer, 0, nYpos, Config.uResX - 1, nYpos) != 0) {
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
        if (SDL_SetRenderDrawColor(Video.pRenderer,0,0,0, SDL_ALPHA_OPAQUE) != 0) {  // Muss am Ende stehen, damit Hintergrund wieder dunkel wird
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
