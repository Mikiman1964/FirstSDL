/*
Dieses Programm erzeugt aus einer beliebigen (binären) Datei
eine C-include-Datei mit den Daten als uint8_t-Array.
*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#define MAX_ARRAYNAME_LEN                   50
#define MAX_DATA_LINE_CHARACTERS            32      // 32 Zeichen pro Zeile
#define SAFE_FREE(x) { if (x != NULL) { free(x); x = NULL; } }

uint8_t *ReadFile(char *pszFilename,uint32_t *puLen);
int CreateTextureLine(char *szHead,char *szFilename,int32_t uIndex);


int main(int argc, char *argv[]) {
    uint8_t *pInputfile = NULL;
    uint32_t uInputLen;
    uint32_t I;
    uint32_t uCharacters;
    FILE *Writefile = NULL;
    char szString[MAX_ARRAYNAME_LEN * 10];
    char szStringLine[5 * MAX_DATA_LINE_CHARACTERS + 100];        // 0xYY,       Maximal 32 Zeichen pro Zeile mit 5 Zeichen
    char szByte[16];
    bool bOK;

    bOK = true;
    if (argc != 4) {
        printf("bad parameters, use %s inputfile outputfile arrayname\n",argv[0]);
        return -1;
    }
    if (strlen(argv[3]) == 0) {
        printf("arrayname is empty!\n");
        return -1;
    }
    if (strlen(argv[3]) > MAX_ARRAYNAME_LEN) {
        printf("arrayname is too long, max len = %d\n",MAX_ARRAYNAME_LEN);
        return -1;
    }
    pInputfile = ReadFile(argv[1],&uInputLen);
    if (pInputfile == NULL) {
        printf("Error: %s\n",strerror(errno));
        return -1;
    }
    if (uInputLen > 0) {
        Writefile = fopen(argv[2],"wb");
        if (Writefile != NULL) {
            sprintf(szString,"uint8_t %s[%d] = {\n",argv[3],uInputLen);
            // Kopfzeile schreiben
            bOK = (fwrite(szString,1,strlen(szString),Writefile) == strlen(szString));
            uCharacters = 0;
            strcpy(szStringLine,"   ");     // 3 Spaces zum Einrücken
            for (I = 0; (bOK) && (I < uInputLen); I++) {
                sprintf(szByte,"0x%02X,",pInputfile[I]);
                strcat(szStringLine,szByte);
                uCharacters++;
                if (uCharacters == MAX_DATA_LINE_CHARACTERS) {
                    strcat(szStringLine,"\n");
                    bOK = (fwrite(szStringLine,1,strlen(szStringLine),Writefile) == strlen(szStringLine));
                    uCharacters = 0;
                    strcpy(szStringLine,"   ");     // 3 Spaces zum Einrücken
                }
            }
            if ((uCharacters > 0) && (bOK)) {
                // Restliche Zeile schreiben
                strcat(szStringLine,"\n");
                bOK = (fwrite(szStringLine,1,strlen(szStringLine),Writefile) == strlen(szStringLine));
                // Ende schreiben
                if (bOK) {
                    strcpy(szString,"};\n");
                    bOK = (fwrite(szString,1,strlen(szString),Writefile) == strlen(szString));
                }
            }
            if (!bOK) {
                printf("Error: %s,  can not write into file %s for writing!\n",strerror(errno),argv[2]);
            }
            fclose(Writefile);
        } else {
            printf("Error: %s,  can not open file %s for writing!\n",strerror(errno),argv[2]);
        }
    } else {
        printf("input file %s is empty!\r",argv[1]);
    }
    SAFE_FREE(pInputfile);
    if (!bOK) {
        return -1;
    } else {
        printf("OK, %d bytes converted!\n",uInputLen);
        return 0;
    }

}


/*----------------------------------------------------------------------------
Name:           ReadFile
------------------------------------------------------------------------------
Beschreibung: Liest Daten aus einer Datei und alloziert hierfür Speicher, der
              außerhalb dieser Funktion wieder freigegeben werden muss.
Parameter
      Eingang: pszFilename, char *, Zeiger auf Dateinamen (komplette Pfadangabe)
      Ausgang: puLen, uint32_t *, Anzahl Bytes, die eingelesen bzw. alloziert wurden.
Rückgabewert:  uint8_t *, NULL = Fehler, sonst Zeiger auf allozierten Speicher mit Daten.
Seiteneffekte:
------------------------------------------------------------------------------*/
uint8_t *ReadFile(char *pszFilename,uint32_t *puLen) {
    uint8_t *pRet = NULL;
    FILE *Readfile = NULL;
    struct stat Fileinfo;

    if ( (pszFilename != NULL) && (puLen != NULL) ) {
        if (stat(pszFilename,&Fileinfo) == 0) {
            if (Fileinfo.st_size > 0) {
                Readfile = fopen(pszFilename,"rb");
                if (Readfile != NULL) {
                    pRet = (uint8_t*)malloc(Fileinfo.st_size);
                    if (pRet != NULL) {
                        if (fread(pRet,1,Fileinfo.st_size,Readfile) == (size_t)Fileinfo.st_size) { // Datei lesen
                            *puLen = Fileinfo.st_size;
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
            printf("%s: stat() failed, filename: %s\r\n",__FUNCTION__,pszFilename);
        }
    } else {
        printf("%s: bad parameter\r\n",__FUNCTION__);
    }
    if (Readfile != NULL) {
        fclose(Readfile);
    }
    return pRet;
}
