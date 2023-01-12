#ifndef SCROLLER_H_INCLUDED
#define SCROLLER_H_INCLUDED

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "mySDL.h"

#define SCROLLER_BUFFER_LEN     ((WINDOW_W / FONT_W) + 1)   // + 1, für den rechten unsichtbaren Rand



typedef struct
{
  bool bChangeScrollPixel;      // Anzahl Scrollpixel soll geändert werden
  uint32_t uScrolledPixel;      // Anzahl Pixel, die nach neuem Buchstaben gescrollt wurden (0 - FONT_W)
  uint32_t uScrollSpeedPixel;   // Anzahl Pixel / Scroll-Schritt
  uint8_t *pszScrolltext;       // Zeiger auf Scrolltext
  // Buffer sind die sichtbaren Buchstaben mit den enstsprechenden Winkeln (Sinus, Y-Auslenkung) des Scrollers
  uint8_t Buffer[SCROLLER_BUFFER_LEN];
  float fAngles[SCROLLER_BUFFER_LEN];
  uint32_t uScrolltextPointer;  // zeiger auf Zeichen im Scrolltext
  int nYpos;                    // Y-Position
  float fXfreq;                 // X-Frequenz der Sinusfrequenz
  float fYfreq;                 // Y-Frequenz der Sinusfrequenz
  float fYamplitude;            // Y-Amplitude des Sinus
  float fScale;                 // Skalierung der Buchstaben
  bool bSinus;                  // Sinus-Scroller
  bool bSwellFont;              // Font "anschwellen" lassen
}SCROLLER;


int InitScroller(SCROLLER *pScroller, uint32_t uScrollSpeedPixel, int nYpos, uint8_t *pszScrolltext, float fXfreq, float fYfreq, float fYamplitude, float fScale, bool bSinus, bool bSwellFont);
int DoScroller(SDL_Renderer *pRenderer, SCROLLER *pScroller);
uint8_t ConvertASCII(uint8_t uASCIICode);


#endif // SCROLLER_H_INCLUDED
