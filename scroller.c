#include <SDL2/SDL.h>
#include "config.h"
#include "loadlevel.h"
#include <math.h>
#include "mySDL.h"
#include "mystd.h"
#include "scroller.h"

extern CONFIG Config;


/*----------------------------------------------------------------------------
Name:           InitScroller
------------------------------------------------------------------------------
Beschreibung: Initialisiert die Sroller-Struktur
Parameter
      Eingang: pScroller, SCROLLER *, Zeiger auf Scroller-Struktur
               uScrollSpeedPixel, uint32_t, Anzahl Pixel, die pro Aufruf gescrollt werden sollen (Scroll-Speed)
                         sollte ein Vielfaches von FONT_W sein, da der Scroller sonst ruckelt
               uXStart, uint32_t, Horizontale Position wo Scroller ended (linke Seite)
               uXEnd, uint32_t, Horizontale Position wo Scroller startet (rechte Seite)
               nYpos, int, Y-Startposition, hat scheinbar nur auf Sinusscroller Einfluss
               pszScrolltext, uint8_t *, Zeiger auf Scrolltext, null-terminiert
               fXfreq, float, X-Frequenz der Sinusfrequenz
               fYfreq, float, Y-Frequenz der Sinusfrequenz
      Ausgang: -
Rückgabewert:  int, 0 = OK, sonst Fehler
Seiteneffekte: Config.x
------------------------------------------------------------------------------*/
int InitScroller(SCROLLER *pScroller, uint32_t uScrollSpeedPixel, uint32_t uXStart, uint32_t uXEnd, int nYpos, uint8_t *pszScrolltext, float fXfreq, float fYfreq, float fYamplitude, float fScale, bool bSinus, bool bSwellFont) {
    int nErrorcode = -1;
    uint32_t I;

    if ( (pScroller != NULL) && (pszScrolltext != NULL) && ((uXEnd - uXStart) > FONT_W) ) {
        memset(pScroller,0,sizeof(SCROLLER));
        pScroller->uScrollerBufferLen = (((uXEnd - uXStart) / FONT_W) + 1); // + 1, für den rechten unsichtbaren Rand
        pScroller->uXStart = uXStart;
        pScroller->uXEnd = uXEnd;
        //pScroller->uScrollerBufferLen = ((Config.uResX / FONT_W) + 1); // + 1, für den rechten unsichtbaren Rand
        pScroller->puBuffer = malloc(pScroller->uScrollerBufferLen * sizeof(uint8_t));  // uScrollerBufferLen Elemente
        pScroller->pfAngles = malloc(pScroller->uScrollerBufferLen * sizeof(float));    // uScrollerBufferLen Elemente
        if ((pScroller->puBuffer != NULL) && (pScroller->pfAngles != NULL)) {
            pScroller->bChangeScrollPixel = false;
            pScroller->uScrolledPixel = 0;
            if (uScrollSpeedPixel > FONT_W) {
                uScrollSpeedPixel = FONT_W;
            }
            // Falls gewünschte Scroll-Geschwindigkeit kein Vielfaches von FONT_W ist, dann an nächst schneller Geschwindigkeit anpassen
            while (FONT_W % uScrollSpeedPixel != 0) {
                uScrollSpeedPixel++;
            }
            pScroller->uScrollSpeedPixel = uScrollSpeedPixel;
            pScroller->nYpos = nYpos;
            pScroller->uScrolltextPointer = 0;      // Auf Zeichenkettenanfang setzen
            pScroller->pszScrolltext = pszScrolltext;
            for (I = 0; I < pScroller->uScrollerBufferLen; I++) {
                pScroller->pfAngles[I] = 0;         // Alle Winkel auf 0°
                pScroller->puBuffer[I] = ConvertASCII(0x20);       // Alle Pufferzeichen auf Space
            }
            pScroller->fXfreq = fXfreq;
            pScroller->fYfreq = fYfreq;
            pScroller->fYamplitude = fYamplitude;
            pScroller->fScale = fScale;
            pScroller->bSinus = bSinus;
            pScroller->bSwellFont = bSwellFont;
            nErrorcode = 0;
        } else {
            SDL_Log("%s: malloc() failed",__FUNCTION__);
            SAFE_FREE(pScroller->puBuffer);
            SAFE_FREE(pScroller->pfAngles);
        }
    } else {
        SDL_Log("%s: check parameters, null pointer or bad positions",__FUNCTION__);
    }
    return nErrorcode;
}


/*----------------------------------------------------------------------------
Name:           FreeScroller
------------------------------------------------------------------------------
Beschreibung: Gibt allozierten Speicher für den Scroller wieder frei.
Parameter
      Eingang: pScroller, SCROLLER *, Zeiger auf Scroller-Struktur
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: -
------------------------------------------------------------------------------*/
void FreeScroller(SCROLLER *pScroller) {
    SAFE_FREE(pScroller->puBuffer);
    SAFE_FREE(pScroller->pfAngles);
}


/*----------------------------------------------------------------------------
Name:           DoScroller
------------------------------------------------------------------------------
Beschreibung: Führt den entsprechenden Scroller aus, der in der Struktur pScroller hinterlegt wurde.
Parameter
      Eingang: pRenderer, SDL_Renderer *, Zeiger auf Renderer
               pScroller, SCROLLER *, Zeiger auf Scroller-Struktur
      Ausgang: -
Rückgabewert:  int, 0 = OK, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int DoScroller(SDL_Renderer *pRenderer, SCROLLER *pScroller) {
    uint8_t newchar;
    uint32_t I;
    int nErrorCode;
    float fScaleW;
    float fScaleH;
    float fRotation;

    nErrorCode = -1;
    if (pScroller->uScrolledPixel == 0) {
        newchar = pScroller->pszScrolltext[pScroller->uScrolltextPointer];
        if (newchar == 0) {     // Textende erkannt?
            pScroller->uScrolltextPointer = 0;
            newchar = pScroller->pszScrolltext[0];
        }
        newchar = ConvertASCII(newchar);
        pScroller->uScrolltextPointer++;
        // Scroll-Puffer mit entsprechenden Winkeln nach links kopieren und neues Zeichen ganz rechts einfügen
        for (I = 0; I < pScroller->uScrollerBufferLen - 1; I++) {
            pScroller->puBuffer[I] = pScroller->puBuffer[I + 1];
            pScroller->pfAngles[I] = pScroller->pfAngles[I + 1];
        }
        pScroller->puBuffer[I] = newchar;
        pScroller->pfAngles[I] = pScroller->pfAngles[I - 1] + pScroller->fXfreq;
    }
    // Buffer-Zeichen rendern
    for (I = 0; I < pScroller->uScrollerBufferLen; I++) {
       if (pScroller->bSinus) {
            if (pScroller->bSwellFont) {
                fScaleW = pScroller->fScale * abs(sin(pScroller->pfAngles[I]));
                fScaleH = fScaleW;
            } else {
                fScaleW = pScroller->fScale;
                fScaleH = fScaleW;
            }
            fRotation = cos(pScroller->pfAngles[I]) * 45;
            //fScaleW = fScaleW + abs(cos(pScroller->pfAngles[I])) * 0.2;
        } else {
            fScaleW = pScroller->fScale;
            fScaleH = fScaleW;
            fRotation = 0;
        }
        nErrorCode = CopyTexture(pRenderer,
                                 pScroller->puBuffer[I],        // TextureIndex
                                 pScroller->uXStart + I * FONT_W - pScroller->uScrolledPixel,        // X-pos
                                 pScroller->nYpos + sin(pScroller->pfAngles[I]) * pScroller->fYamplitude,   // Y-pos
                                 FONT_W,                                        // Texture Width
                                 FONT_H,                                        // Texture Height
                                 fScaleW,                                       // Skalierung W
                                 fScaleH,                                       // Skalierung H
                                 fRotation);                                    // Rotation
    }
    for (I = 0; I < pScroller->uScrollerBufferLen; I++) {
        pScroller->pfAngles[I] = pScroller->pfAngles[I] + pScroller->fYfreq;
    }
    pScroller->uScrolledPixel += pScroller->uScrollSpeedPixel;
    if (pScroller->uScrolledPixel >= FONT_W) {
         pScroller->uScrolledPixel = 0;
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           ConvertASCII
------------------------------------------------------------------------------
Beschreibung: Konvertiert für die Auswahl eines Font-Zeichens den gegebenen ASCII-Code
                in die eigene Texture-Kodierung
Parameter
      Eingang: uASCIICode, uint8_t, ASCII-Zeichen, das konvertiert werden soll
      Ausgang: -
Rückgabewert:  uint8_t, konvertierter Code
Seiteneffekte: -
------------------------------------------------------------------------------*/
uint8_t ConvertASCII(uint8_t uASCIICode) {
    uint8_t uConvCode;

    if ((uASCIICode >= 32) && (uASCIICode <= 96)) {
        uConvCode = uASCIICode - 32;
    } else if ((uASCIICode >= 97) && (uASCIICode <= 122)) {   // Kleinbuchstaben wandeln
        uConvCode = uASCIICode - 64;
    } else if (uASCIICode == 123) {                            // ASCII 123 "{" in Ä wandeln
        uConvCode = 65;
    } else if (uASCIICode == 124) {                            // ASCII 124 "|" in Ö wandeln
        uConvCode = 66;
    } else if (uASCIICode == 125) {                            // ASCII 125 "}" in Ü wandeln
        uConvCode = 67;
    } else {                                                   // Space, wenn keine Konvertierung möglich
        uConvCode = 0;
        SDL_Log("%s unknown character found, ASCII-Value = %d",__FUNCTION__,uASCIICode);
    }
    return uConvCode;
}
