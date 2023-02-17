#include <SDL2/SDL.h>
#include <math.h>
#include "mySDL.h"
#include "scroller.h"

/*----------------------------------------------------------------------------
Name:           InitScroller
------------------------------------------------------------------------------
Beschreibung: Initialisiert die Sroller-Struktur
Parameter
      Eingang: pScroller, SCROLLER *, Zeiger auf Scroller-Struktur
               uScrollSpeedPixel, uint32_t, Anzahl Pixel, die pro Aufruf gescrollt werden sollen (Scroll-Speed)
                         sollte ein Vielfaches von FONT_W sein, da der Scroller sonst ruckelt
               nYpos, int, Y-Startposition, hat scheinbar nur auf Sinusscroller Einfluss
               pszScrolltext, uint8_t *, Zeiger auf Scrolltext, null-terminiert
               fXfreq, float, X-Frequenz der Sinusfrequenz
               fYfreq, float, Y-Frequenz der Sinusfrequenz
      Ausgang: -
Rückgabewert:  int, 0 = OK, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int InitScroller(SCROLLER *pScroller, uint32_t uScrollSpeedPixel, int nYpos, uint8_t *pszScrolltext, float fXfreq, float fYfreq, float fYamplitude, float fScale, bool bSinus, bool bSwellFont) {
    int nErrorcode = -1;
    uint32_t I;

    if ( (pScroller != NULL) && (pszScrolltext != NULL) ) {
        memset(pScroller,0,sizeof(SCROLLER));
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
        for (I = 0; I < SCROLLER_BUFFER_LEN; I++) {
            pScroller->fAngles[I] = 0;         // Alle Winkel auf 0°
            pScroller->Buffer[I] = ConvertASCII(0x20);       // Alle Pufferzeichen auf Space
        }
        pScroller->fXfreq = fXfreq;
        pScroller->fYfreq = fYfreq;
        pScroller->fYamplitude = fYamplitude;
        pScroller->fScale = fScale;
        pScroller->bSinus = bSinus;
        pScroller->bSwellFont = bSwellFont;
        nErrorcode = 0;
    } else {
        SDL_Log("%s: check parameters, null pointer",__FUNCTION__);
    }
    return nErrorcode;
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
        for (I = 0; I < SCROLLER_BUFFER_LEN - 1; I++) {
            pScroller->Buffer[I] = pScroller->Buffer[I + 1];
            pScroller->fAngles[I] = pScroller->fAngles[I + 1];
        }
        pScroller->Buffer[I] = newchar;
        pScroller->fAngles[I] = pScroller->fAngles[I - 1] + pScroller->fXfreq;
    }
    // Buffer-Zeichen rendern
    for (I = 0; I < SCROLLER_BUFFER_LEN; I++) {
       if (pScroller->bSinus) {
            if (pScroller->bSwellFont) {
                fScaleW = pScroller->fScale * abs(sin(pScroller->fAngles[I]));
                fScaleH = fScaleW;
            } else {
                fScaleW = pScroller->fScale;
                fScaleH = fScaleW;
            }
            fRotation = cos(pScroller->fAngles[I]) * 45;
            //fScaleW = fScaleW + abs(cos(pScroller->fAngles[I])) * 0.2;
        } else {
            fScaleW = pScroller->fScale;
            fScaleH = fScaleW;
            fRotation = 0;
        }
        nErrorCode = CopyTexture(pRenderer,
                                 pScroller->Buffer[I],
                                 I * FONT_W - pScroller->uScrolledPixel,
                                 pScroller->nYpos + sin(pScroller->fAngles[I]) * pScroller->fYamplitude,
                                 FONT_W,
                                 FONT_H,
                                 fScaleW,
                                 fScaleH,
                                 fRotation);
    }
    for (I = 0; I < SCROLLER_BUFFER_LEN; I++) {
        pScroller->fAngles[I] = pScroller->fAngles[I] + pScroller->fYfreq;
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
