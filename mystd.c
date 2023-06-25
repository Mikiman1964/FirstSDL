#include <errno.h>
#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include "mystd.h"

XORSHIFT XorShift;

/*----------------------------------------------------------------------------
Name:           InitXorShift
------------------------------------------------------------------------------
Beschreibung: Initialisiert den XORSHIFT-Zufallszahlengenerator.
              https://de.wikipedia.org/wiki/Xorshift
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: XorShift.x
------------------------------------------------------------------------------*/
void InitXorShift(void) {
    int nErrorCode;
    uint32_t I;

    nErrorCode = -1;
    srand(time(0));
    XorShift.x = time(0) * 521288629;
    XorShift.y = rand() * 88675123;
    XorShift.z = (uint64_t)(&nErrorCode) * XorShift.y * XorShift.x;
    XorShift.w = (uint64_t)(&XorShift) * rand();
    printf("%s:\r\n x = %u\r\n y = %u\r\n z = %u\r\n w = %u\r\n i = %u\n",__FUNCTION__,XorShift.x,XorShift.y,XorShift.z,XorShift.w,XorShift.x & 0xFFFF);
    I = (XorShift.x & 0xFFFF);
    while (I > 0) {
        xorshift128();
        I--;
    }
}


/*----------------------------------------------------------------------------
Name:           DynStringInit
------------------------------------------------------------------------------
Beschreibung: Initialisiert ein neues String-Objekt. Der zurückgegebene Speicher
              muss mit DynStringFree() später freigegeben werden.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  DYNSTRING *, Zeiger auf String-Objekt, NULL = Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
DYNSTRING *DynStringInit(void) {
    DYNSTRING *DynString;

    DynString = (DYNSTRING*)malloc(sizeof(DYNSTRING));
    if (DynString != NULL) {
        DynString->nLen = 0;
        DynString->pszString = malloc(1);
        if (DynString->pszString != NULL) {
            DynString->pszString[0] = '\0';     // Stringende
        } else {
            SAFE_FREE(DynString);
        }
    }
    return DynString;
}


/*----------------------------------------------------------------------------
Name:           DynStringAdd
------------------------------------------------------------------------------
Beschreibung: Addiert einen String zu einem String-Objekt.

Parameter
      Eingang: DynString, DYNSTRING *, Zeiger auf String-Objekt
               pszString, char *, String, der dazu addiert wird.
      Ausgang: -
Rückgabewert:  0 = Alles OK, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int DynStringAdd(DYNSTRING *DynString, char *pszString) {
    int nErrorCode = -1;
    size_t uActualStringLen;
    size_t uAddStringLen;

    if ((DynString != NULL) && (pszString != NULL)) {
        if (DynString->pszString != NULL) {
            uActualStringLen = strlen(DynString->pszString);
            uAddStringLen = strlen(pszString);
            DynString->pszString = realloc(DynString->pszString,uActualStringLen + uAddStringLen + 1);
            strcat(DynString->pszString,pszString);
            DynString->nLen = strlen(DynString->pszString);
            nErrorCode = 0;
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           DynStringFree
------------------------------------------------------------------------------
Beschreibung: Gibt den Speicher für ein String-Objekt wieder frei.

Parameter
      Eingang: DynString, DYNSTRING *, Zeiger auf String-Objekt
      Ausgang: -
Rückgabewert:  0 = Alles OK, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int DynStringFree(DYNSTRING *DynString) {
    int nErrorCode = -1;

    if (DynString != NULL) {
        if (DynString->pszString != NULL) {
            SAFE_FREE(DynString->pszString);
            SAFE_FREE(DynString);
            nErrorCode = 0;
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetMd5String
------------------------------------------------------------------------------
Beschreibung: Gibt anhand eines MD5-Hashes (16 Bytes) den zugehörigen String (32 + 1 Bytes) zurück.
Parameter
      Eingang: puMd5Hash, uint8_t *, Zeiger auf MD5-Hash (mindestens 16 Bytes)
      Ausgang: pszMd5String, char *, Zeiger auf mindestens 32 + 1 = 33 Bytes für den MD5-String (Großbuchstaben)
Rückgabewert:  -
Seiteneffekte: -
------------------------------------------------------------------------------*/
void GetMd5String(uint8_t *puMd5Hash, char *pszMd5String) {
    uint32_t I;
    char szNum[8];

    if ((puMd5Hash != NULL) && (pszMd5String != NULL)) {
        pszMd5String[0] = 0;
        for (I = 0; I < 16; I++) {
            sprintf(szNum,"%02X",puMd5Hash[I]);
            strcat(pszMd5String,szNum);
        }
    }
}


char g_szMD5String[32 + 1];
/*----------------------------------------------------------------------------
Name:           GetMd5String2
------------------------------------------------------------------------------
Beschreibung: Gibt einen char-Pointer auf einen MD5-Hash-String zurück.
              Diese Funktion verwendet statischen Speicher, d.h. er
              darf nicht freigegeben werden.
              Funktion ist somit nicht threadsicher!

Parameter
      Eingang: puMd5Hash, uint8_t *, Zeiger auf MD5-Hash (mindestens 16 Bytes)
Rückgabewert:  char *, Zeiger auf MD5-String, Bei Fehler wird "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
                       zurückgegeben.
Seiteneffekte: g_szMD5String;
------------------------------------------------------------------------------*/
char *GetMd5String2(uint8_t *puMd5Hash) {
    strcpy(g_szMD5String,"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
    if (puMd5Hash != NULL) {
          GetMd5String(puMd5Hash,g_szMD5String);
    }
    return g_szMD5String;
}


/*----------------------------------------------------------------------------
Name:           GetMd5HashFromString
------------------------------------------------------------------------------
Beschreibung: Gibt anhand eines MD5-Strings (32 Bytes + 1) den zugehörigen Hash (16 Bytes) zurück.
Parameter
      Eingang: pszMd5String, char *, Zeiger auf mindestens 32 + 1 = 33 Bytes für den MD5-String (Groß - oder Kleinbuchstaben)
      Ausgang: puMd5Hash, uint8_t *, Zeiger auf MD5-Hash (mindestens 16 Bytes)
Rückgabewert:  -
Seiteneffekte: -
------------------------------------------------------------------------------*/
void GetMd5HashFromString(char *pszMd5String,uint8_t *puMd5Hash) {
    uint32_t I;
    char szHex[8];

    szHex[2] = 0;   // szHex[0] und szHex[1] ist die 2-stellige Hexziffer
    if ((pszMd5String != NULL) && (puMd5Hash != NULL)) {
        if (strlen(pszMd5String) == 32) {
            for (I = 0; I < 16; I++) {
                szHex[0] = pszMd5String[I * 2 + 0];
                szHex[1] = pszMd5String[I * 2 + 1];
                puMd5Hash[I] = (strtol(szHex,NULL,16) & 0xFF);
            }
        } else {
            printf("%s: string must contain 32 characters\n",__FUNCTION__);
        }
    }
}


/*----------------------------------------------------------------------------
Name:           GetActualTimestamp
------------------------------------------------------------------------------
Beschreibung: Gibt die aktuelle Zeit als Zeitstempel-String zurück. Der String
              hat das folgendes Format: YYYYMMDD_HHMMSS
              Es werden 15 + 1 Bytes Speicher benötigt
Parameter
      Eingang: -
      Ausgang: pszTimestamp, char *, Zeiger auf Speicher für Zeitstempelstring (mindestens 16 Bytes)
Rückgabewert:  -
Seiteneffekte: -
------------------------------------------------------------------------------*/
void GetActualTimestamp(char *pszTimestamp) {
    time_t clock;
    struct tm *result;

    if (pszTimestamp != NULL) {
        time(&clock);
        result = localtime(&clock);
        sprintf(pszTimestamp,"%04d%02d%02d_%02d%02d%02d",
                result->tm_year + 1900,
                result->tm_mon + 1,
                result->tm_mday,
                result->tm_hour,
                result->tm_min,
                result->tm_sec
                );
    }
}


/*----------------------------------------------------------------------------
Name:           randn
------------------------------------------------------------------------------
Beschreibung: Erzeugt Integer-Zufallszahl zwischen low und high.
              Vorsicht: Der Bereich sollte nicht um den Nullpunkt gelegt werden, da es ansonsten falsche Ergebnisse gibt.
              Quelle: https://mixable.blog/java-zufallszahlen-in-einem-bestimmten-bereich-erstellen/
Parameter
      Eingang: low, int, unterer Bereich für Zufallszahl (einschließlich low)
      Ausgang: high, int, oberer Bereich für Zufallszahl (einschließlich high)
Rückgabewert:  int, Zufallswert
Seiteneffekte: -
------------------------------------------------------------------------------*/
int randn(int low, int high) {
    return (int)((myrandom() * ((high - low) + 1)) + low);
}


/*----------------------------------------------------------------------------
Name:           randf
------------------------------------------------------------------------------
Beschreibung: Erzeugt float-Zufallszahl zwischen low und high.
              Quelle: https://stackoverflow.com/questions/22115241/how-to-generate-random-numbers-between-1-and-1-in-c
              von SteveL
              Ausführlicher Test am 23.6.2023 mit low = -1, high = +1
              Vorsicht: Falls der Bereich um den Nullpunkt gelegt werden soll, muss dieser symetrisch (z.B. -2,+2) sein.

Parameter
      Eingang: low, float, unterer Bereich für Zufallszahl (nicht einschließlich low)
      Ausgang: high, float, oberer Bereich für Zufallszahl (nicht einschließlich high)
Rückgabewert:  float, Zufallswert
Seiteneffekte: -
------------------------------------------------------------------------------*/
float randf(float low, float high) {
    return  myrandom() * abs(low - high) + low;
}


/*----------------------------------------------------------------------------
Name:           myrandom
------------------------------------------------------------------------------
Beschreibung: Erzeugt float-Zufallszahl zwischen 0 und < 1

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  float, Zufallswert zwischen 0 und < 1
Seiteneffekte: -
------------------------------------------------------------------------------*/
float myrandom(void) {
    uint32_t uR;

    uR = xorshift128();
    return (float)uR / (float)0xFFFFFFFF;
}


/*----------------------------------------------------------------------------
Name:           xorshift128
------------------------------------------------------------------------------
Beschreibung: Erzeugt eine Zufallszahl nach dem XORSHIFT-Verfahren.
              https://de.wikipedia.org/wiki/Xorshift
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  uint32_t, Zufallszahl zwischen 0 und 0xFFFFFFFF
Seiteneffekte: XorShift.x
------------------------------------------------------------------------------*/
uint32_t xorshift128(void) {
  uint32_t t = XorShift.x ^ (XorShift.x << 11);
  XorShift.x = XorShift.y;
  XorShift.y = XorShift.z;
  XorShift.z = XorShift.w;
  XorShift.w ^= (XorShift.w >> 19) ^ t ^ (t >> 8);//https://de.wikipedia.org/wiki/Xorshift
  return XorShift.w;
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
Name:           ReadFile
------------------------------------------------------------------------------
Beschreibung: Liest Daten aus einer Datei und alloziert hierfür Speicher, der
              außerhalb dieser Funktion wieder freigegeben werden muss.
              Es wird >immer< sichergestellt, dass sich nach den Daten ein
              Nullbyte (Stringterminierung) befindet, d.h. ggf. eingelesene Textdateien
              können als String behandelt werden!
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
                            printf("%s: read error, freeing memory for file: %s\r\n",__FUNCTION__,pszFilename);
                            SAFE_FREE(pRet);
                        }
                    } else {
                        printf("%s: can not allocate memory for filesize: %ld, file: %s\r\n",__FUNCTION__,Fileinfo.st_size,pszFilename);
                    }
                } else {
                    printf("%s: can not open file: %s\r\n",__FUNCTION__,pszFilename);
                }
            } else {
                printf("%s: file %s is empty\r\n",__FUNCTION__,pszFilename);
            }
        } else {
            printf("%s: stat() for file %s failed\r\n",__FUNCTION__,pszFilename);
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
Beschreibung: Schreibt Daten in eine Datei. Eine ggf. vorhandene Datei wird überschrieben.

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


/*----------------------------------------------------------------------------
Name:           GetLineFeedCount
------------------------------------------------------------------------------
Beschreibung: Zählt die Zeilenumbrüche (0x0A) in einem Text.

Parameter
      Eingang: pszText, char *, Zeiger auf Text
      Ausgang: -
Rückgabewert:  int, Anzahl LineFeeds
Seiteneffekte:
------------------------------------------------------------------------------*/
int GetLineFeedCount(char *pszText) {
    int nLineFeeds;
    uint32_t I;

    nLineFeeds = 0;
    if (pszText != NULL) {
        for (I = 0; I < strlen(pszText); I++) {
            if (pszText[I] == 0x0A) {
                nLineFeeds++;
            }
        }
    }
    return nLineFeeds;
}


/*----------------------------------------------------------------------------
Name:           GetLineLen
------------------------------------------------------------------------------
Beschreibung: Ermittelt die aktuelle Zeilenlänge, in der der Cursor steht.

Parameter
      Eingang: pszText, char *, Zeiger auf Text
               nCursorPos, int, Cursorposition im Text
      Ausgang: -
Rückgabewert:  int, Anzahl LineFeeds
Seiteneffekte:
------------------------------------------------------------------------------*/
int GetLineLen(char *pszText, int nCursorPos) {
    int nLineLen;
    int nTextLen;
    int nI;
    bool bEnd;

    nLineLen = 0;
    if (pszText != NULL) {
        nTextLen = (int)strlen(pszText);
        // Liegt Cursorposition innerhalb des Textes?
        if ((nTextLen > 0) && (nCursorPos >= 0) && (nCursorPos < nTextLen)) {
            // Falls Cursor auf Linefeed steht, dann werden nur die Zeichen >vor< dem Cursor gezählt
            if (pszText[nCursorPos] != 0x0A) {
                // Zunächst ab Cursorposition + 1 bis Stringende oder Linefeed Zeichen zählen
                nI = nCursorPos + 1;
                bEnd = false;
                do {
                    if ((pszText[nI] != 0x0A) && (pszText[nI] != 0) && (nI < nTextLen)) {
                        nLineLen++;
                        nI++;
                    } else {
                        bEnd = true;
                    }
                } while (!bEnd);
            }
            // Dann ab Cursorposition bis Zeilenanfang/Linefeed bzw. Textanfang Zeichen zählen
            nI = nCursorPos;
            bEnd = false;
            if (pszText[nCursorPos] == 0x0A) {
                nI--;
            }
            if (nI > 0) {
                do {
                    if ((pszText[nI] != 0x0A) && (pszText[nI] != 0)) {
                        nLineLen++;
                        nI--;
                        if (nI < 0) {
                            bEnd = true;
                        }
                    } else {
                        bEnd = true;
                    }
                } while (!bEnd);
            }
        }
    }
    return nLineLen;
}


/*----------------------------------------------------------------------------
Name:           CheckAndCreateDir
------------------------------------------------------------------------------
Beschreibung: Prüft ein Directory und legt dieses an, falls es nicht existiert.
Parameter
      Eingang: pszDirectoryName, char *, Zeiger auf Directory-namen
      Ausgang: -

Rückgabewert:  int, 0 = kein Fehler, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int CheckAndCreateDir(char *pszDirectoryName) {
    int nErrorCode;
    DIR *dir;
    bool bTryToCreateDir;

    bTryToCreateDir = false;
    nErrorCode = -1;
    if (pszDirectoryName != NULL) {
        dir = opendir(pszDirectoryName);
        if (dir != NULL) {
            nErrorCode = closedir(dir);     // Directory besteht und kann nun wieder geschlossen werden
        } else {
            printf("can not open dir, error: %s\n",strerror(errno));
            if (errno == ENOTDIR) {         // File existiert, ist aber kein Directory
                bTryToCreateDir = (remove(pszDirectoryName) == 0);  // remove kann beides (Dir/File) löschen
            } else if (errno == ENOENT) {   // Directory/File gibt es nicht
                bTryToCreateDir = true;
            }
            if (bTryToCreateDir) {
                #ifdef __linux__
                    nErrorCode = mkdir(pszDirectoryName, 0777);
                #else
                    nErrorCode = mkdir(pszDirectoryName);
                #endif
                if (nErrorCode != 0) {
                    printf("can not create dir, error: %s\n",strerror(errno));
                }
            }
        }
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
