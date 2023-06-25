#ifndef MYSTD_H_INCLUDED
#define MYSTD_H_INCLUDED

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define KB  1024

#define SAFE_FREE(x) { if (x != NULL) { free(x); x = NULL; } }


typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t z;
    uint32_t w;
} XORSHIFT;



typedef struct {
    char *pszString;
    int  nLen;
} DYNSTRING;


// Dynamische Strings
DYNSTRING *DynStringInit(void);
int DynStringAdd(DYNSTRING *DynString, char *pszString);
int DynStringFree(DYNSTRING *DynString);

// https://stackoverflow.com/questions/22115241/how-to-generate-random-numbers-between-1-and-1-in-c
// von SteveL
float randf(float low,float high);
void GetMd5String(uint8_t *puMd5Hash, char *pszMd5String);
char *GetMd5String2(uint8_t *puMd5Hash);
void GetMd5HashFromString(char *pszMd5String,uint8_t *puMd5Hash);
void GetActualTimestamp(char *pszTimestamp);
int randn(int low,int high);
void InitXorShift(void);
uint32_t xorshift128(void); // https://de.wikipedia.org/wiki/Xorshift
void DumpMem(uint8_t *pcMem, int nLen);
uint8_t *ReadFile(const char *pszFilename,uint32_t *puLen);
int WriteFile(const char *pszFilename,uint8_t *pcData, uint32_t uLen,bool bAppend);
int CheckAndCreateDir(char *pszDirectoryName);
uint8_t *BinToBase64(uint8_t *pcBinary, uint32_t uBinaryLen, uint32_t *puBase64Len, bool bNewLine);
int GetLineFeedCount(char *pszText);
int GetLineLen(char *pszText, int nCursorPos);
float myrandom(void);
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

int decode_char(uint8_t in);
int Base64ToBin(uint8_t *pcBinary, uint8_t *pcBase64, uint32_t uBase64Len, uint32_t *puBinaryLen);
#endif // MYSTD_H_INCLUDED
