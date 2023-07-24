#ifndef LEVELCONVERTER_H_INCLUDED
#define LEVELCONVERTER_H_INCLUDED
#include <SDL2/SDL.h>
#include <stdio.h>

#include "editor.h"
#include "loadlevel.h"

#define EMERALD_LEVELTYPE_DC3                   0

#define BITMAP_MINSIZE                          4096
#define EMERALD_IMPORTDC3_DIRECTORYNAME         "importdc3"                     // Import-Directory für Diamond Caves 3-Bitmap-Level

#define EMERALD_MAX_IMPORTFILES                 200
#define EMERALD_MAX_SHOWFILENAME_LEN            120                             // Maximal 120 Zeichen des Filenamens (szShowFilename) anzeigen
#define EMERALD_MAX_MAXIMPORTFILES_IN_LIST      35

typedef struct {
    char szFilename[EMERALD_MAX_FILENAME_LEN + 1];      // "Echter" Filename (zum Laden der Datei)
    char szShowFilename[EMERALD_MAX_FILENAME_LEN + 1];  // Anzeigename, Zeichensatz unterstützt nicht alle Zeichen, d.h. in der Regel kürzer als "echter" Name
} LEVELFILESLIST;



// Für IMPORT (DC3) ///////////////
typedef struct {
    uint32_t uDc3FileCount;             // Anzahl der DC3-Bimap-Levelfiles
} IMPORTLEVEL;


typedef struct {
    uint32_t        uFilesize;
    char            szFilename[EMERALD_MAX_FILENAME_LEN + 1];
    uint8_t         cHeader[14];
    uint8_t         cInfoHeader[40];            // Es gibt zwar noch andere Info-Header-Größen, hier wird aber nur diese Größe verwendet
    uint8_t         uBytesPerPixel;
    uint32_t        uBytesPerLine;
    int32_t         nW;
    int32_t         nH;
    uint32_t        uDataOffset;
    bool            bBottomUp;                  // Bild ist vertikal gedreht (Normalfall)
} BITMAP;


// Prototypen
int LevelConverterFromBitap(char *pszFilename);
int ReadBmHeader(FILE *pF,BITMAP *pBitmap);
void DumpMem(uint8_t *pcMem, int nLen);
void PrintBitmapInfos(BITMAP *pBitmap);
void PrintLevelInfos(ED *pEd);
int Read16x16Macroblock(FILE *pF, uint32_t X, uint32_t Y,uint8_t *puBlockbuf);
int Show16x16Macroblock(SDL_Renderer *pRenderer,uint32_t uXpos,uint32_t uYpos,uint8_t *puBlockbuf);
uint16_t GetElementByMD5(char *szMd5);

int CheckImportLevelFiles(void);
int GetLevelFileList(char *pszDirectoryName, char *pszFileExtension, LEVELFILESLIST *pLevelFileList, uint32_t *puFileCount);
#endif // LEVELCONVERTER_H_INCLUDED
