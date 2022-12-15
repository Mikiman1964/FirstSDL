#ifndef EXTERNALPOINTER_H_INCLUDED
#define EXTERNALPOINTER_H_INCLUDED


// Bindet das Grafiken-Array ein
#include "Gfx/gfx.h"

// Das Array ist wie folgt aufgebaut:

/*       Offset      Länge
 uint32_t Gfx[] = {
        0x00000000,0x00000C36,     // 0000 - Space.bmp
        0x00000C36,0x00000C36,     // 0001 - Ausrufungszeichen.bmp
        0x0000186C,0x00000C36,     // 0002 - man.bmp
        0x000024A2,0x00000C36,     // 0003 - Stahl.bmp
        0x000030D8,0x00000C36,     // 0004 - PfeilRechts.bmp
        .          .
        .          .
        .          .
        .          .
        0xFFFFFFFF,0xFFFFFFFF      // Endekennung
 };
*/

#endif // EXTERNALPOINTER_H_INCLUDED
