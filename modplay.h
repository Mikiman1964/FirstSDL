#ifndef MODPLAY_H_INCLUDED
#define MODPLAY_H_INCLUDED

#include <stdint.h>
#include "xm.h"

#define SAMPLERATE                  44100
#define AUDIO_BUFFERSIZE            8192                // Puffer reicht für ((8192 / 44100) * 1000) / 2 = ca. 90 ms, durch 2, da Puffer für beide Kanäle

#define MODULE_TYPE_UNKNOWN         0                   // unbekannter Modultyp
#define MODULE_TYPE_MOD             1                   // MOD
#define MODULE_TYPE_XM              2                   // XM (Extended Module)



typedef struct {
    int nAvailableSongs;                    // Anzahl der verfügbaren Songs (mod/xm)
    SDL_AudioDeviceID audio_device;
    SDL_AudioSpec sdl_audio;
    short audiobuffer[AUDIO_BUFFERSIZE];    // Puffer für beide Kanäle
    float xm_audiobuffer[AUDIO_BUFFERSIZE]; // Zwischenpuffer für Extended Module
    uint8_t *pMusicStart;
    int nMusicSize;
    int nMusicIndex;
    int nNextMusicIndex;                    // wird nur in main() zum Umschalten der Musik verwendet
    uint8_t *pMusicAll;                     // Alle Songs in einem Speicherblock
    uint8_t *pTheMusic;                     // Zeiger auf Kopie eines Songs, da durch das Abspielen die Daten verändert werden
    xm_context_t *pCtxXm;                   // Context für XM-Player
    int nModulType;                         // Modul-Typ (MOD, XM) oder unbekannt
}AUDIOPLAYER;

typedef struct {
	const int8_t *sample;
	uint32_t age;
	uint32_t currentptr;
	uint16_t length;
	uint16_t looplength;
	int16_t period;
	int8_t volume;
	int8_t muted;
} PaulaChannel_t;

typedef struct {
	const int8_t *data;
	uint16_t length;
	uint16_t actuallength;
	uint16_t looppoint;
	uint16_t looplength;
	uint8_t volume;
	int8_t finetune;
} Sample;

typedef struct {
	int val;
	uint8_t waveform;
	uint8_t phase;
	uint8_t speed;
	uint8_t depth;
} Oscillator;

typedef struct {
	int orders, maxpattern, order, row, tick, maxtick, speed, arp,
		skiporderrequest, skiporderdestrow,
		patlooprow, patloopcycle,
		samplerate, paularate, audiospeed, audiotick, random;

	int note[4], sample[4], eff[4], effval[4];
	int slideamount[4], slidenote[4], sampleoffset[4];

	Oscillator vibrato[4], tremolo[4];

	uint8_t *patterndata, *ordertable;
	Sample samples[31];

	PaulaChannel_t paula[4];
	uint8_t uVolumePercent;
} ModPlayerStatus_t;


void InitMusicPointer(void);
int InitAudioplayerStruct(void);
int SetModMusic(int nMusicIndex);
int PlayMusic(bool bIgnoreConfig);
void CheckMusicSwitch(const Uint8 *pKeyboardArray);
void SetModVolume(uint8_t uVolumePercent);

ModPlayerStatus_t *InitMOD(uint8_t *mod, int samplerate);
ModPlayerStatus_t *RenderMOD(short *buf, int len);
ModPlayerStatus_t *ProcessMOD();

#endif // MODPLAY_H_INCLUDED
