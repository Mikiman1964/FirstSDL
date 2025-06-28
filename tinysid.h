/*
TinySID
Welcome to the world of C64 music

TinySID is the smallest SID player available.
Its most advantage lies in its unique HardSID support, that is:

    * exact C64 <-> PC cycle syncronizing
    * multispeed tunes will use the same timing a real C64 does
    * improved HardSID sample output (includes mixer)
    * support for multiprocessor systems
    * Current windows 32 bit version requires just 23kbyte.

Current version features:

    * Multiplatform
          o Native 32/64 bit windows support (both with hardsid
		    support)
          o Runs on Unix/linux platform (hardsid version
		    available, but not online yet)
          o Apple Macinthosh OS X 10.4 universal binary
		    (beta version)
          o Playstation Portable (PSP) version available
          o Also incorporated in rockbox for portable mp3 players
    * Accurate playback on both hardsid and software emulation,
	  includes multispeed tunes
    * Digitized sample playback on both hardsid and software
	  emulation
    * 99% 6510 cpu emulation (still some bugs to fix)
    * Fat 6581 SID emulation, including filters. Sounds
	  different than other players!

Please note that TinySID has nothing to do with other projects
such as SIDPlay. The 6510/6581 emulation is based on routines
by Tammo Hinrichs (kb) and has been further developed.
*/
#ifndef TINYSID_H_INCLUDED
#define TINYSID_H_INCLUDED

#include <stdint.h>

#define USE_FILTER

#define FLAG_N 128
#define FLAG_V 64
#define FLAG_B 16
#define FLAG_D 8
#define FLAG_I 4
#define FLAG_Z 2
#define FLAG_C 1


typedef struct {
    char        szName[32];
    char        szAuthor[32];
    char        szCopyright[32];
    uint16_t    uLoadAddr;
    uint16_t    uInitAddr;
    uint16_t    uPlayAddr;
    uint8_t     uActualSubsong;
    uint8_t     uMaxSubsong;
    uint8_t     uPlaySpeed;
    uint8_t     uData[65536];
    uint16_t    uLen;
} SIDSONG;


int InitSidSong(uint8_t *pSidSong, uint16_t uLen);
void ShowSidSong();
void c64Init(void);
uint32_t RenderSid(short *pAudiobuf,uint32_t uSidBlocks);


#endif
