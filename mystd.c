#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "mystd.h"


/*----------------------------------------------------------------------------
Name:           randf
------------------------------------------------------------------------------
Beschreibung: Erzeugt float-Zufallszahl zwischen low und high.
              Quelle: https://stackoverflow.com/questions/22115241/how-to-generate-random-numbers-between-1-and-1-in-c
              von SteveL
Parameter
      Eingang: low, float, unterer Bereich für Zufallszahl
      Ausgang: high, float, oberer Bereich für Zufallszahl
Rückgabewert:  float, Zufallswert
Seiteneffekte: -
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
Rückgabewert:  -
Seiteneffekte: -
------------------------------------------------------------------------------*/
void DumpMem(uint8_t *pcMem, int nLen)
{
    uint8_t *pcEnd;			// Letzte gültige Adresse
    int nX;
    int nY;

    if ( (nLen > 0) && (pcMem != NULL) ) {
        nY = 0;
        while (nLen > 0) {
            pcEnd = pcMem + nLen - 1;		    // Letzte Adresse, die ausgegeben werden soll
            for (nX = 0; nX < 16; nX++) {	    // 16er Blöcke werden immer unabhängig von der Anzahl auszugebener Daten ausgegeben
                if (nX == 0) {
                    printf("%04X:   ",nY);	    // Adresse ausgeben
                }
                if ((pcMem + nX) <= pcEnd) {	// Soll Inhalt des Speichers noch ausgegeben werden ?
                    printf("%02X ",pcMem[nX]);	// Space nach jeder Zahl
                } else {
                    printf("** ");		        // Sterne für "ungültigen" Speicher anzeigen
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
                    printf(".");			    // "ungültiger" Speicher
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
               haystack_len, size_t, Länge in Bytes des zu durchsuchenden Speicherbereichs
               needle, const void * , Zeiger auf zu suchenden Teilbereich
               needle_len, const size_t, Länge in Bytes des zu suchenden Teilbereichs
      Ausgang: -
Rückgabewert:  const char *, Zeiger auf gefundenen Speicher, NULL = nichts gefunden
Seiteneffekte: -
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


/*----------------------------------------------------------------------------
Name:           ReadFile
------------------------------------------------------------------------------
Beschreibung: Liest Daten aus einer Datei und alloziert hierfür Speicher, der
              außerhalb dieser Funktion wieder freigegeben werden muss.
Parameter
      Eingang: pszFilename, const char *, Zeiger auf Dateinamen (komplette Pfadangabe)
      Ausgang: puLen, uint32_t *, Anzahl Bytes, die eingelesen bzw. alloziert wurden.
Rückgabewert:  uint8_t *, NULL = Fehler, sonst Zeiger auf allozierten Speicher mit Daten.
Seiteneffekte:
------------------------------------------------------------------------------*/
uint8_t *ReadFile(const char *pszFilename,uint32_t *puLen) {
    uint8_t *pRet = NULL;
    FILE *Readfile = NULL;
    struct stat Fileinfo;

    if ( (pszFilename != NULL) && (puLen != NULL) ) {
        if (stat(pszFilename,&Fileinfo) == 0) {
            if (Fileinfo.st_size > 0) {
                Readfile = fopen(pszFilename,"rb");
                if (Readfile != NULL) {
                    pRet = (uint8_t*)malloc(Fileinfo.st_size + 1);  // + 1 für abschließendes Null-Byte
                    if (pRet != NULL) {
                        if (fread(pRet,1,Fileinfo.st_size,Readfile) == (size_t)Fileinfo.st_size) { // Datei lesen
                            *puLen = Fileinfo.st_size;
                             pRet[Fileinfo.st_size] = 0; // abschließendes Null-Byte
                        } else {
                            printf("%s: read error, freeing memory\r\n",__FUNCTION__);
                            SAFE_FREE(pRet);
                        }
                    } else {
                        printf("%s: can not allocate memory for filesize: %ld\r\n",__FUNCTION__,Fileinfo.st_size);
                    }
                } else {
                    printf("%s: can not open file: %s\r\n",__FUNCTION__,pszFilename);
                }
            } else {
                printf("%s: file is empty\r\n",__FUNCTION__);
            }
        } else {
            printf("%s: stat() failed\r\n",__FUNCTION__);
        }
    } else {
        printf("%s: bad parameter\r\n",__FUNCTION__);
    }
    if (Readfile != NULL) {
        fclose(Readfile);
    }
    return pRet;
}


/*----------------------------------------------------------------------------
Name:           WriteFile
------------------------------------------------------------------------------
Beschreibung: Schreibt daten in eine Datei. Eine ggf. vorhandene Datei wird überschrieben.

Parameter
      Eingang: pszFilename, const char *, Zeiger auf Dateinamen (komplette Pfadangabe)
               pcData, uint8_t *, Zeiger auf Daten
               uLen, uint32_t, Anzahl Bytes, die geschrieben werde
               bAppend, bool, TRUE = Daten werden an bestehende Daten angehängt
      Ausgang: -
Rückgabewert:  int, 0 = Alles OK, sonst Fehler
Seiteneffekte:
------------------------------------------------------------------------------*/
int WriteFile(const char *pszFilename,uint8_t *pcData, uint32_t uLen,bool bAppend)
{
    char szMode[8];
    int nErrorCode;
    FILE *Writefile;
    nErrorCode = -1;

    if ((pszFilename != NULL) && (pcData != NULL) && (uLen > 0)) {
        if (bAppend) {
            strcpy(szMode,"ab");
        } else {
            strcpy(szMode,"wb");
        }
        Writefile = fopen(pszFilename,szMode);
        if (Writefile != NULL) {
            if (fwrite(pcData,1,uLen,Writefile) == uLen) {
                nErrorCode = 0;
            } else {
                printf("%s: write error\r\n",__FUNCTION__);
            }
            fclose(Writefile);
        } else {
            printf("%s: can not open file: %s\r\n",__FUNCTION__,pszFilename);
        }
    } else {
        printf("%s: bad parameter\r\n",__FUNCTION__);
    }
    return nErrorCode;
}


// Für Base64
uint8_t g_cBase64digits[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
/*----------------------------------------------------------------------------
Name:           BinToBase64
------------------------------------------------------------------------------
Beschreibung:  Konvertiert binäre Daten in das base64-Format (druckbare
                Zeichen). Aus 3 binären Bytes werden 4 druckbare erzeugt.
                Ist der Parameter bNewline = true, so werden
                nach 76 erzeugten Zeichen automatisch "\r\n" angehängt.
                Die Funktion alloziert Speicher, der außerhalb dieser
                Funktion wieder freigegeben werden muss.
Parameter
      Eingang: pcBinary, uint8_t *, Zeiger auf binäre Daten
               uBinaryLen, uint32_t, Anzahl Bytes für binäre Daten
               bNewLine, bool, true = nach 19 Zeichen neue Zeile erzeugen
      Ausgang: puBase64Len, uint32_t *, Länge der base64-Daten in Bytes
Rückgabewert:  uint8_t *, Zeiger auf base64-kodierte Daten, NULL = Fehler
Seiteneffekte: g_cBase64digits[]
------------------------------------------------------------------------------*/
uint8_t *BinToBase64(uint8_t *pcBinary, uint32_t uBinaryLen, uint32_t *puBase64Len, bool bNewLine) {
    // Hinweis zum Speicherbedarf für Ausgangspuffer: Aus 3 Originalbytes werden 4 druckbare Bytes erzeugt
    uint8_t cFragment;
    uint8_t *pcOut;
    uint8_t *pcBase64;
    uint8_t *pcIn;
    uint32_t uCount;
    uint32_t uBase64Count;
    uint32_t uPass1Len;

    // Das Schlimmste verhindern
    if ((pcBinary == NULL) || (uBinaryLen == 0) || (puBase64Len == NULL)) {
        return NULL;
    }
    // PASS 1, um die genaue Ausgangslänge zu ermitteln
    uPass1Len = uBinaryLen;
    uBase64Count = 0;
    pcIn = pcBinary;
    uCount = 0;
    for (; uPass1Len >= 3; uPass1Len -= 3) {
        uBase64Count = uBase64Count + 4;
        pcIn += 3;
        uCount++;
        if (uCount == 19) {	// 19 * 4 = 76
            uCount = 0;
            if (bNewLine) {
                uBase64Count = uBase64Count + 2;
            }
        }
    }
    if (uPass1Len > 0) {
        uBase64Count = uBase64Count + 4;
        uCount++;
        if (uCount == 19) {	// 19 * 4 = 76
            uCount = 0;
            if (bNewLine) {
                uBase64Count = uBase64Count + 2;
            }
        }
    }
    pcBase64 = (uint8_t*)malloc(uBase64Count + 1);  // +1 für Stringende
    if (pcBase64 != NULL) {
        // PASS 2, die eigentliche Kodierung
        pcOut = pcBase64;
        pcIn = pcBinary;
        uCount = 0;
        for (; uBinaryLen >= 3; uBinaryLen -= 3) {
            *pcOut++ = g_cBase64digits[pcIn[0] >> 2];
            *pcOut++ = g_cBase64digits[((pcIn[0] << 4) & 0x30) | (pcIn[1] >> 4)];
            *pcOut++ = g_cBase64digits[((pcIn[1] << 2) & 0x3c) | (pcIn[2] >> 6)];
            *pcOut++ = g_cBase64digits[pcIn[2] & 0x3f];
            pcIn += 3;
            uCount++;
            if (uCount == 19) {	// 19 * 4 = 76
              uCount = 0;
              if (bNewLine) {
                *pcOut++ = '\r';
                *pcOut++ = '\n';
              }
            }
        }
        if (uBinaryLen > 0) {
            *pcOut++ = g_cBase64digits[pcIn[0] >> 2];
            cFragment = (pcIn[0] << 4) & 0x30;
            if (uBinaryLen > 1) {
                cFragment |= pcIn[1] >> 4;
            }
            *pcOut++ = g_cBase64digits[cFragment];
            *pcOut++ = (uBinaryLen < 2) ? '=' : g_cBase64digits[(pcIn[1] << 2) & 0x3c];
            *pcOut++ = '=';
            uCount++;
            if (uCount == 19) {	// 19 * 4 = 76
                uCount = 0;
                if (bNewLine) {
                    *pcOut++ = '\r';
                    *pcOut++ = '\n';
                }
            }
        }
        *puBase64Len = uBase64Count;    // Länge der base64-kodierten Daten zurückgeben
        *pcOut = 0;                     // Stringende
    }
    return pcBase64;
}



/*
Copyright (c) 2012, Cameron Harper
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define PADCHAR '='
#define CHAR62  '+'
#define CHAR63  '/'
#define CHAR0D  0x0D
#define CHAR0A  0x0A
#define CHAR20  0x20

int decode_char(uint8_t in)
{
    if((in >= 'A') && (in <= 'Z'))
        return (in - 'A');
    else if((in >= 'a') && (in <= 'z'))
        return (in - 'a' + 26);
    else if((in >= '0') && (in <= '9'))
        return (in - '0' + 52);
    else{
        switch(in){
            case CHAR62:
                return 62;
            case CHAR63:
                return 63;
            case PADCHAR:
            case CHAR0A:
            case CHAR0D:
            case CHAR20:
                return 64;
            default:
                return 255;
            }
    }
}

int Base64ToBin(uint8_t *pcBinary, uint8_t *pcBase64, uint32_t uBase64Len, uint32_t *puBinaryLen)
{
    uint8_t c, acc = 0;
    uint32_t i;
    uint32_t uCharCount;
    *puBinaryLen = 0;

    uCharCount = 0;
    for (i = 0; i < uBase64Len; i++) {
        switch (c = decode_char(pcBase64[i])) {
            case 255:
                return -1;
            case 64:
                // Nichts machen bei Zeilenumbrüchen, Space und  '='
                break;
            default:
                switch(uCharCount % 4) {
                    case 0:
                        acc = (c << 2);
                        break;
                    case 1:
                        if (pcBinary != NULL) {
                            pcBinary[(*puBinaryLen)++] = acc | (c >> 4);
                        } else {
                            (*puBinaryLen)++;
                        }
                        acc = (c << 4);
                        break;
                    case 2:
                        if (pcBinary != NULL) {
                            pcBinary[(*puBinaryLen)++] = acc | (c >> 2);
                        } else {
                            (*puBinaryLen)++;
                        }
                        acc = (c << 6);
                        break;
                    case 3:
                        if (pcBinary != NULL) {
                            pcBinary[(*puBinaryLen)++] = acc | c;
                        } else {
                            (*puBinaryLen)++;
                        }
                }
                uCharCount++;
                break;
        }
    }
    return 0;
}
