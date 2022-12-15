#ifndef MYSTD_H_INCLUDED
#define MYSTD_H_INCLUDED

#include <stdlib.h>


#define SAFE_FREE(x) { if (x != NULL) { free(x); x = NULL; } }


// https://stackoverflow.com/questions/22115241/how-to-generate-random-numbers-between-1-and-1-in-c
// von SteveL
float randf(float low,float high);
// https://stackoverflow.com/questions/52988769/writing-own-memmem-for-windows
const char *memmem(const char *haystack_start, size_t haystack_len, const void *needle_start, size_t needle_len);
void DumpMem(uint8_t *pcMem, int nLen);
uint8_t *ReadFile(const char *pszFilename,uint32_t *puLen);



#endif // MYSTD_H_INCLUDED
