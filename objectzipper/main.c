#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mystd.h"
#include "miniz.h"

#define MINIZ_HEADER_FILE_ONLY
#define MINIZ_LITTLE_ENDIAN 1
#define MINIZ_HAS_64BIT_REGISTERS 1


int main(int argc, char *argv[]) {
    uint8_t *pInputfile = NULL;
    uint8_t *pCompressBuffer;
    uint32_t uInputLen;             // uncompressed Size
    uint32_t uMaxCompressedSize;
    uint8_t  uInputSize[4];

    if (argc != 3) {
        printf("bad parameters, use %s inputfile outputfile\n",argv[0]);
        return -1;
    }
    pInputfile = ReadFile(argv[1],&uInputLen);
    if (pInputfile == NULL) {
        printf("Error: %s\n",strerror(errno));
        return -1;
    }
    uMaxCompressedSize = uInputLen * 2;
    pCompressBuffer = malloc(uMaxCompressedSize);
    if (pCompressBuffer == NULL) {
        SAFE_FREE(pInputfile);
        printf("can not allocate memory for %u bytes!\n",uMaxCompressedSize);
        return -1;
    }
    // Inputfile komprimieren
    // uMaxCompressedSize ist hier noch Eingabeparameter, damit mz_compress() weiﬂ, wieviel Puffer zum Komprimieren bereit steht
    if (mz_compress2(pCompressBuffer,(mz_ulong*)&uMaxCompressedSize,pInputfile,uInputLen,MZ_BEST_COMPRESSION) != MZ_OK) {
        printf("can not compress input file, size: %u bytes!\n",uMaxCompressedSize);
        SAFE_FREE(pCompressBuffer);
        SAFE_FREE(pInputfile);
        return -1;
    } else {
        printf("input file:  %s\n",argv[1]);
        printf("input len:   %u bytes\n",uInputLen);
        printf("output file: %s\n",argv[2]);
        printf("output len:  %u bytes\n",uMaxCompressedSize);
        *(uint32_t*)(uInputSize) = uInputLen;
        if (WriteFile(argv[2],uInputSize,4,false) == 0) {       // Originall‰nge schreiben
            if (WriteFile(argv[2],pCompressBuffer,uMaxCompressedSize,true) == 0) {    // Komprimierte Daten anh‰ngen
                printf("OK\n");
            } else {
                printf("Error: %s\n",strerror(errno));
            }
        } else {
            printf("Error: %s\n",strerror(errno));
        }
    }
    SAFE_FREE(pCompressBuffer);
    SAFE_FREE(pInputfile);
    return 0;
}
