#ifndef LEVELCONVERTER_H_INCLUDED
#define LEVELCONVERTER_H_INCLUDED
#include <SDL2/SDL.h>
#include <stdio.h>

#include "editor.h"
#include "loadlevel.h"


#define IMPORT_LEVELTYPE_UNKNOWN                0
#define IMPORT_LEVELTYPE_DC3                    1
#define IMPORT_LEVELTYPE_EMERALDMINE_KINGSOFT   2
#define IMPORT_LEVELTYPE_ROCKSNDIAMONDS         3


#define IMPORT_BITMAP_MINSIZE                   4096

#define IMPORT_EMERALDMINE_KINGSOFT_LEVELSIZE   2106
#define IMPORT_EMERALDMINE_KINGSOFT_X_DIM       64
#define IMPORT_EMERALDMINE_KINGSOFT_Y_DIM       32

#define IMPORT_DIRECTORYNAME                    "import"                        // Import-Directory für z.B. Diamond Caves 3-Bitmap-Level
#define IMPORT_LEVELAUTHOR_FILENAME             "levelauthor.txt"               // Datei enthält Levelautor
#define IMPORT_MAX_FILES_IN_LIST                35                              // Maximale Anzahl, die angezeigt werden
#define IMPORT_MAX_SHOWFILENAME_LEN             71                              // Maximal 71 Zeichen des Filenamens (szShowFilename) anzeigen

#define EMERALD_BORDER_NOSTEEL_NONE             0x00                            // Kein Rand enthält Nicht-Stahl-Elemente, d.h. Level kann belassen werden
#define EMERALD_BORDER_NOSTEEL_LEFT             0x01                            // Linker Rand enthält Nicht-Stahl-Elemente
#define EMERALD_BORDER_NOSTEEL_RIGHT            0x02                            // Rechter Rand enthält Nicht-Stahl-Elemente
#define EMERALD_BORDER_NOSTEEL_TOP              0x04                            // Oberer Rand enthält Nicht-Stahl-Elemente
#define EMERALD_BORDER_NOSTEEL_BOTTOM           0x08                            // Unterer Rand enthält Nicht-Stahl-Elemente


typedef struct {
    char szFilename[EMERALD_MAX_FILENAME_LEN + 1];      // "Echter" Filename (zum Laden der Datei)
    char szShowFilename[EMERALD_MAX_FILENAME_LEN + 1];  // Anzeigename, Zeichensatz unterstützt nicht alle Zeichen, d.h. in der Regel kürzer als "echter" Name
    uint8_t uLevelType;                                 // Leveltyp, z.B. IMPORT_LEVELTYPE_DC3
    char szFileSize[11];                               // Größe in Bytes als String, maximal 10-stellig
} LEVELFILESLIST;



// Für Level-Import  ///////////////
typedef struct {
    uint32_t uFileCount;             // Anzahl der Import-Levelfiles
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
int32_t LevelConverterFromBitap(char *pszFilename);
int32_t LevelConverterFromEmeraldMineKingsoft(char *pszFilename);
int32_t LevelConverterFromRocksnDiamonds(char *pszFilename);

void EncryptEmeraldMineKingsoftLeveldata(uint8_t *pData);
uint16_t ConvertEmeraldMineKingsoft(uint8_t uEmeraldMineKingosftElement);
int32_t ReadBmHeader(FILE *pF,BITMAP *pBitmap);
void DumpMem(uint8_t *pcMem, int32_t nLen);
void PrintBitmapInfos(BITMAP *pBitmap);
void PrintLevelInfos(ED *pEd);
int32_t Read16x16Macroblock(FILE *pF, uint32_t X, uint32_t Y,uint8_t *puBlockbuf);
uint8_t GetLevelborderInfo(void);
int32_t AdjustLevelborder(void);
int32_t GetLevelAuthorFromFile(char *pszLevelAuthor);
int32_t GetLevelTitleFromFilename(char *pszFilename, char *pszLevelTitle);
int32_t Show16x16Macroblock(uint32_t uXpos,uint32_t uYpos,uint8_t *puBlockbuf);
uint16_t GetElementByMD5(char *szMd5);

int32_t CheckImportLevelFiles(void);
int32_t GetLevelFileList(char *pszDirectoryName, LEVELFILESLIST *pLevelFileList, uint32_t *puFileCount);

void CheckLevelmportDirectoryThread(void);
int32_t StartCheckLevelImportDirectoryThread(void);
int32_t CloseCheckLevelImportDirectoryThread(void);
uint32_t ConvertTime(uint32_t uFields);
uint32_t ConvertTimeRnd(uint32_t uRnDTime);


// Ab hier Rocks'n'Diamonds
#define RND_HEADER_NAME                     "RND1"
// Offsets innerhalb der Segmente
// "INFO"-Segment
#define RND_INFO_LEVEL_X_DIMENSION          0x00000003
#define RND_INFO_LEVEL_Y_DIMENSION          0x00000006
#define RND_INFO_TIMETOPLAY                 0x00000009
#define RND_INFO_EMERALDSTOCOLLECT          0x0000000C
#define RND_INFO_TIMESCOREFACTOR            0x0000000F


// Segmente, die vorhanden sein müssen
typedef struct {
    uint8_t* pInfo;
    uint32_t uInfoLen;

    uint8_t* pName;
    uint32_t uNameLen;

    uint8_t* pAuth;
    uint32_t uAuthLen;

    uint8_t* pBody;
    uint32_t uBodyLen;

    uint8_t* pElements;
    uint32_t uElementsLen;

    bool bAllSegmentsFound;
} RNDSEGMENTS;



int32_t GetRndSegments(uint8_t *pMemory, uint32_t uMemorySize);
int32_t GetSegmentLen(uint8_t *pMemory);
void GetRndLevelTitle(void);
void GetRnDLevelElements(void);
uint16_t ConvertRocksnDiamonds(uint16_t uRocksnDiamondsElement);
void CheckSegment(char *pszSegmentName, uint8_t* pStartPointer, uint32_t uSegmentLen);

#endif // LEVELCONVERTER_H_INCLUDED
