#ifndef LEVELCONVERTER_H_INCLUDED
#define LEVELCONVERTER_H_INCLUDED
#include <SDL2/SDL.h>
#include <stdio.h>

#define BITMAP_FILENAME                         "C:\\users\\Mike\\Desktop\\Level.bmp"
#define BITMAP_MINSIZE                          4096

#define LVCV_OUTPUTFILENAME                     "C:\\users\\Mike\\Desktop\\md5.txt"

typedef struct {
    uint32_t        uFilesize;
    char            szFilename[255];
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
int LevelConverter(uint32_t uConvert);
int LevelConverterFromBitap(void);
int LevelConverterFromDosgame(void);
int ReadBmHeader(FILE *pF,BITMAP *pBitmap);
void DumpMem(uint8_t *pcMem, int nLen);
void PrintBitmapInfos(BITMAP *pBitmap);
void PrintLevelInfos(ED *pEd);
int Read16x16Macroblock(FILE *pF, uint32_t X, uint32_t Y,uint8_t *puBlockbuf);
int Show16x16Macroblock(SDL_Renderer *pRenderer,uint32_t uXpos,uint32_t uYpos,uint8_t *puBlockbuf);
uint16_t GetElementByMD5(char *szMd5);

#endif // LEVELCONVERTER_H_INCLUDED
