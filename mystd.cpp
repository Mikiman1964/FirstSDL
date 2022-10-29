#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "mystd.h"


/*----------------------------------------------------------------------------
Name:           randf
------------------------------------------------------------------------------
Beschreibung: Erzeugt float-Zufallszahl zwischen low und high.
              Quelle: https://stackoverflow.com/questions/22115241/how-to-generate-random-numbers-between-1-and-1-in-c
              von SteveL
Parameter
      Eingang: low, float, unterer Bereich f�r Zufallszahl
      Ausgang: high, float, oberer Bereich f�r Zufallszahl
R�ckgabewert:  float, Zufallswert
Seiteneffekte:
------------------------------------------------------------------------------*/
float randf(float low,float high){
    return (rand()/(float)(RAND_MAX))*abs(low-high)+low;
}


/*----------------------------------------------------------------------------
Name:           DumpMem
------------------------------------------------------------------------------
Beschreibung: Macht Speicherdump auf stdout.
Parameter
      Eingang: pcMem, uint8_t *, Start-Zeiger auf Speicher
               nLen, int, Anzahl der auszugebenen Bytes ab Start-Zeiger
      Ausgang: -
R�ckgabewert:  -
Seiteneffekte:
------------------------------------------------------------------------------*/
void DumpMem(uint8_t *pcMem, int nLen)
{
    uint8_t *pcEnd;			// Letzte g�ltige Adresse
    int nX;
    int nY;

    if ( (nLen > 0) && (pcMem != NULL) ) {
        nY = 0;
        while (nLen > 0) {
            pcEnd = pcMem + nLen - 1;		    // Letzte Adresse, die ausgegeben werden soll
            for (nX = 0; nX < 16; nX++) {	    // 16er Bl�cke werden immer unabh�ngig von der Anzahl auszugebener Daten ausgegeben
                if (nX == 0) {
                    printf("%04X:   ",nY);	    // Adresse ausgeben
                }
                if ((pcMem + nX) <= pcEnd) {	// Soll Inhalt des Speichers noch ausgegeben werden ?
                    printf("%02X ",pcMem[nX]);	// Space nach jeder Zahl
                } else {
                    printf("** ");		        // Sterne f�r "ung�ltigen" Speicher anzeigen
                }
                if (nX == 7) {
                    printf("  ");			    // 2 Spaces nach 8 Zahlen
                }
            }
            printf("  ");			            // Ab hier Textblock ausgeben
            for (nX = 0; nX < 16; nX++) {	    // Text ausgeben
                if ((pcMem + nX) <= pcEnd) {	// Soll Inhalt des Speichers noch ausgegeben werden ?
                    if ((pcMem[nX] > 31) && (pcMem[nX] < 127)) {
                        printf("%c",pcMem[nX]);
                    } else {
                      printf(".");		        // nicht druckbares Zeichen
                    }
                } else {
                    printf(".");			    // "ung�ltiger" Speicher
                }
            }
            printf("\r\n");			            // neue Zeile
            nLen = nLen - 16;
            if (nLen > 0) {
                nY = nY + 16;
                pcMem = pcMem + 16;
            }
        }
    }
}


/*----------------------------------------------------------------------------
Name:           memmem
------------------------------------------------------------------------------
Beschreibung: Durchsucht einen Speicherbereich "haystack" nach "needle".
              Quelle: https://stackoverflow.com/questions/52988769/writing-own-memmem-for-windows
              von KamilCuk
Parameter
      Eingang: haystack, const char *, Zeiger auf zu durchsuchenden Speicherbereich
               haystack_len, size_t, L�nge in Bytes des zu durchsuchenden Speicherbereichs
               needle, const void * , Zeiger auf zu suchenden Teilbereich
               needle_len, const size_t, L�nge in Bytes des zu suchenden Teilbereichs
      Ausgang: -
R�ckgabewert:  const char *, Zeiger auf gefundenen Speicher, NULL = nichts gefunden
Seiteneffekte:
------------------------------------------------------------------------------*/
const char *memmem(const char *haystack, size_t haystack_len, const void *needle, const size_t needle_len)
{
    if (haystack == NULL) return NULL; // or assert(haystack != NULL);
    if (haystack_len == 0) return NULL;
    if (needle == NULL) return NULL; // or assert(needle != NULL);
    if (needle_len == 0) return NULL;

    for (const char *h = haystack;
            haystack_len >= needle_len;
            ++h, --haystack_len) {
        if (!memcmp(h, needle, needle_len)) {
            return h;
        }
    }
    return NULL;
}

