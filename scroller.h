#ifndef SCROLLER_H_INCLUDED
#define SCROLLER_H_INCLUDED

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "mySDL.h"


typedef struct
{
  uint32_t uXStart;             // Horizontaler Start und
  uint32_t uXEnd;               // Ende für Scroller
  uint32_t uScrollerBufferLen;  // ((WINDOW_W / FONT_W) + 1)   // + 1, für den rechten unsichtbaren Rand
  bool bChangeScrollPixel;      // Anzahl Scrollpixel soll geändert werden
  uint32_t uScrolledPixel;      // Anzahl Pixel, die nach neuem Buchstaben gescrollt wurden (0 - FONT_W)
  uint32_t uScrollSpeedPixel;   // Anzahl Pixel / Scroll-Schritt
  uint8_t *pszScrolltext;       // Zeiger auf Scrolltext
  // Buffer sind die sichtbaren Buchstaben mit den enstsprechenden Winkeln (Sinus, Y-Auslenkung) des Scrollers
  uint8_t *puBuffer;            // uScrollerBufferLen Elemente
  float *pfAngles;              // uScrollerBufferLen Elemente
  uint32_t uScrolltextPointer;  // Zeiger auf Zeichen im Scrolltext
  int nYpos;                    // Y-Position
  float fXfreq;                 // X-Frequenz der Sinusfrequenz
  float fYfreq;                 // Y-Frequenz der Sinusfrequenz
  float fYamplitude;            // Y-Amplitude des Sinus
  float fScale;                 // Skalierung der Buchstaben
  bool bSinus;                  // Sinus-Scroller
  bool bSwellFont;              // Font "anschwellen" lassen
}SCROLLER;


int InitScroller(SCROLLER *pScroller, uint32_t uScrollSpeedPixel, uint32_t uXStart, uint32_t uXEnd, int nYpos, uint8_t *pszScrolltext, float fXfreq, float fYfreq, float fYamplitude, float fScale, bool bSinus, bool bSwellFont);
int DoScroller(SDL_Renderer *pRenderer, SCROLLER *pScroller);
uint8_t ConvertASCII(uint8_t uASCIICode);
void FreeScroller(SCROLLER *pScroller);

#endif // SCROLLER_H_INCLUDED
