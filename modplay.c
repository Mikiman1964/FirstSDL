#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "config.h"
#include "miniz.h"
#include "modplay.h"
#include "sfx/music.h"
#include "sfx/music_compressed.h"
#include "tinysid.h"

// ModPlayerStatus_t mp;
AUDIOPLAYER Audioplayer;

extern CONFIG Config;

/*----------------------------------------------------------------------------
Name:           InitAudioplayerStruct
------------------------------------------------------------------------------
Beschreibung: Initialisiert die Struktur Audioplayer.x und öffnet das
              Audiodevice für MODPlay und XM-Player und SID-Player.
              Darf nur einmal zum Programmstart aufgerufen werden.
              Die Speicher pMusicAll und pTheMusic müssen zum Programmende
              freigegeben werden.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int32_t, 0 = OK, sonst Fehler
Seiteneffekte: Audioplayer.x, music[].x, music_compressed[]
------------------------------------------------------------------------------*/
int32_t InitAudioplayerStruct(void) {
    int32_t nErrorCode = -1;
    uint8_t *pCompressedMusicStart;
    uint32_t uUnCompressedMusicSize;
    uint32_t uCompressedMusicSize;
    uint32_t I;
    int32_t nMiniz;

    memset(&Audioplayer,0,sizeof(AUDIOPLAYER));
    // Alle Songs entpacken und im Speicher Audioplayer.pMusicAll ablegen
    pCompressedMusicStart = music_compressed;
    uCompressedMusicSize = sizeof(music_compressed);
    uUnCompressedMusicSize = *(uint32_t*)pCompressedMusicStart;
    Audioplayer.pMusicAll = malloc(uUnCompressedMusicSize);
    if (Audioplayer.pMusicAll == NULL) {
        SDL_Log("%s: can not allocate memory for decompressed music (%u bytes)",__FUNCTION__,uUnCompressedMusicSize);
        return -1;
    }
    nMiniz = mz_uncompress(Audioplayer.pMusicAll,(mz_ulong*)&uUnCompressedMusicSize,pCompressedMusicStart + 4,(mz_ulong)uCompressedMusicSize);
    if (nMiniz != MZ_OK) {
        SDL_Log("%s: can not decompress music, error: %d",__FUNCTION__,nMiniz);
        SAFE_FREE(Audioplayer.pMusicAll);
        return -1;
    }
    // Anzahl der Songs feststellen und in Audioplayer.nAvailableSongs ablegen
    I = 0;
    do {
        if (music[I] != 0xFFFFFFFF) {
            Audioplayer.nAvailableSongs++;
        }
        I = I + 2;
    } while (music[I] != 0xFFFFFFFF);
    // Audio device einstellen und öffnen
    Audioplayer.sdl_audio.freq = SAMPLERATE;
    Audioplayer.sdl_audio.format = AUDIO_S16;
    Audioplayer.sdl_audio.channels = 2;
    Audioplayer.sdl_audio.samples = AUDIO_BUFFERSIZE;
    Audioplayer.sdl_audio.callback = NULL;
    Audioplayer.audio_device = SDL_OpenAudioDevice(NULL, 0, &Audioplayer.sdl_audio, NULL, 0);
    Audioplayer.uMusicVolumePercent = 100;
    if (Audioplayer.audio_device > 0) {
        SDL_PauseAudioDevice(Audioplayer.audio_device, 0);
        nErrorCode = 0;
    } else {
        SDL_Log("%s: SDL_OpenAudioDevice() failed: %s",__FUNCTION__,SDL_GetError());
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           SetMusic
------------------------------------------------------------------------------
Beschreibung: Stellt ein MOD-/XM-/SID-File zum Abspielen bereit.

Parameter
      Eingang: nMusicIndex, int32_t, Index auf Song, siehe oben "Externe Pointer und Indexe"
      Ausgang: -
Rückgabewert:  int32_t, 0 = OK, sonst Fehler
Seiteneffekte: Audioplayer.x, music[].x
------------------------------------------------------------------------------*/
int32_t SetMusic(int32_t nMusicIndex) {
    int32_t nErrorCode;

    Audioplayer.nModulType = MODULE_TYPE_UNKNOWN;
    if (Audioplayer.pCtxXm != NULL) {
        xm_free_context(Audioplayer.pCtxXm);    // ggf. voriges XM freigeben
        Audioplayer.pCtxXm = NULL;
    }
    nErrorCode = -1;
    if ( (nMusicIndex < 1) || (nMusicIndex > Audioplayer.nAvailableSongs) ) {
        nMusicIndex = 1;
    }
    nMusicIndex--; // Pointer-Array ab 0
    Audioplayer.nMusicIndex = nMusicIndex;
    Audioplayer.pMusicStart = Audioplayer.pMusicAll + music[nMusicIndex * 2 + 0];
    Audioplayer.nMusicSize = music[nMusicIndex * 2 + 1];
    Audioplayer.nNextMusicIndex = 0;
    if ((Audioplayer.pMusicStart != NULL) && (Audioplayer.nMusicSize > 0)) {
        Audioplayer.pTheMusic = (uint8_t*)realloc(Audioplayer.pTheMusic,Audioplayer.nMusicSize + 10); // + 10 wegen valgrind (siehe RenderMOD)
        if (Audioplayer.pTheMusic != NULL) {
            memcpy(Audioplayer.pTheMusic,Audioplayer.pMusicStart,Audioplayer.nMusicSize);
            // Prüfen, ob MOD oder XM
            if (memcmp(Audioplayer.pTheMusic,"Extended Module:",16) == 0) {
                if (xm_create_context_safe(&Audioplayer.pCtxXm,(char*) Audioplayer.pTheMusic, Audioplayer.nMusicSize, 44100) == 0) {
                    nErrorCode = 0;
                    Audioplayer.nModulType = MODULE_TYPE_XM;
                } else {
                    SDL_Log("%s: invalid xm file, data size: %d",__FUNCTION__,Audioplayer.nMusicSize);
                    Audioplayer.pCtxXm = NULL;
                }

            } else if (memcmp(Audioplayer.pTheMusic,"PSID",4) == 0) {
                c64Init();
                InitSidSong(Audioplayer.pTheMusic,Audioplayer.nMusicSize);
                ShowSidSong();
                nErrorCode = 0;
                Audioplayer.nModulType = MODULE_TYPE_SID;
            } else {
                if (InitMOD(Audioplayer.pTheMusic, Audioplayer.sdl_audio.freq) != NULL) {
                    nErrorCode = 0;
                    Audioplayer.nModulType = MODULE_TYPE_MOD;
                } else {
                    SDL_Log("%s: invalid mod file, data size: %d",__FUNCTION__,Audioplayer.nMusicSize);
                }
            }
        } else {
            SDL_Log("%s: realloc() failed: memory size: %d",__FUNCTION__,Audioplayer.nMusicSize);
        }
    } else {
        SDL_Log("%s: bad file size or bad start pointer: %p, MusicIndex: %d",__FUNCTION__,Audioplayer.pMusicStart,nMusicIndex + 1);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           PlayMusic
------------------------------------------------------------------------------
Beschreibung: Spielt ein MOD-File ab. Diese Funktion muss zyklisch aufgerufen werden,
              da z.Z. keine Callback-Funktion verwendet wird.
              Vor Aufruf dieser Funktion muss das Audiodevice geöffnet
              * InitAudioplayerStruct()   und
              ein Modfile gesetzt
              * SetModMusic
              worden sein

Parameter
      Eingang: bIgnoreConfig, bool, true = Config.bGameMusic wird ignoriert
      Ausgang: -
Rückgabewert:  int32_t, 0 = OK, sonst Fehler
Seiteneffekte: Audioplayer.x, Config.x
------------------------------------------------------------------------------*/
int32_t PlayMusic(bool bIgnoreConfig) {
    int32_t nErrorCode = 0;
    uint32_t k;
    uint32_t uSamples;

    uSamples = Audioplayer.sdl_audio.samples;   // für mod und xm
    if ((Config.bGameMusic) || (bIgnoreConfig)) {
        if (SDL_GetQueuedAudioSize(Audioplayer.audio_device) < Audioplayer.sdl_audio.samples * sizeof(short) * 2) {
            if (Audioplayer.nModulType == MODULE_TYPE_MOD) {
                RenderMOD(Audioplayer.audiobuffer, Audioplayer.sdl_audio.samples / 2); // durch 2, da 2 Kanäle
            } else if (Audioplayer.nModulType == MODULE_TYPE_XM) {
                xm_generate_samples(Audioplayer.pCtxXm,Audioplayer.xm_audiobuffer,Audioplayer.sdl_audio.samples / 2);   // durch 2, da 2 Kanäle
                for (k = 0; k < AUDIO_BUFFERSIZE; ++k) {
                    Audioplayer.audiobuffer[k] = Audioplayer.xm_audiobuffer[k] * 32767; // floats in short umrechnen
                }
            } else if (Audioplayer.nModulType == MODULE_TYPE_SID) {
                uSamples = RenderSid(Audioplayer.audiobuffer,4);    // 4 Blöcke -> 882 * 4 * 2 = 7056 Samples
            } else {
                SDL_Log("%s: unknown module type: %d",__FUNCTION__,Audioplayer.nModulType);
                nErrorCode = -1; // unbekanntes Modul
            }
            if (nErrorCode == 0) {
                nErrorCode = SDL_QueueAudio(Audioplayer.audio_device,Audioplayer.audiobuffer,uSamples * sizeof(short)); // 2 channels, 2 bytes/sample
            }
            if (nErrorCode != 0) {
                SDL_Log("%s: SDL_QueueAudio() failed: %s",__FUNCTION__,SDL_GetError());
            }
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           CheckMusicSwitch
------------------------------------------------------------------------------
Beschreibung: Prüft anhand eines Tastendrucks, ob auf eine andere Musik umgeschaltet werden soll
Parameter
      Eingang: pKeyboardArray, const Uint8 *, Zeiger auf Tastatur-Array
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Audioplayer.x
------------------------------------------------------------------------------*/
void CheckMusicSwitch(const Uint8 *pKeyboardArray) {

    if (pKeyboardArray[SDL_SCANCODE_1]) {
        if (Audioplayer.nNextMusicIndex == 0) {
            Audioplayer.nNextMusicIndex = 1;
        }
    }
    if (pKeyboardArray[SDL_SCANCODE_2]) {
        if (Audioplayer.nNextMusicIndex == 0) {
            Audioplayer.nNextMusicIndex = 2;
        }
    }
    if (pKeyboardArray[SDL_SCANCODE_3]) {
        if (Audioplayer.nNextMusicIndex == 0) {
            Audioplayer.nNextMusicIndex = 3;
        }
    }
    if (pKeyboardArray[SDL_SCANCODE_4]) {
        if (Audioplayer.nNextMusicIndex == 0) {
            Audioplayer.nNextMusicIndex = 4;
        }
    }
    if (pKeyboardArray[SDL_SCANCODE_5]) {
        if (Audioplayer.nNextMusicIndex == 0) {
            Audioplayer.nNextMusicIndex = 5;
        }
    }
    if (pKeyboardArray[SDL_SCANCODE_6]) {
        if (Audioplayer.nNextMusicIndex == 0) {
            Audioplayer.nNextMusicIndex = 6;
        }
    }
    if (pKeyboardArray[SDL_SCANCODE_7]) {
        if (Audioplayer.nNextMusicIndex == 0) {
            Audioplayer.nNextMusicIndex = 7;
        }
    }
    if (pKeyboardArray[SDL_SCANCODE_8]) {
        if (Audioplayer.nNextMusicIndex == 0) {
            Audioplayer.nNextMusicIndex = 8;
        }
    }
    if (pKeyboardArray[SDL_SCANCODE_9]) {
        if (Audioplayer.nNextMusicIndex == 0) {
            Audioplayer.nNextMusicIndex = 9;
        }
    }
    if (pKeyboardArray[SDL_SCANCODE_0]) {
        if (Audioplayer.nNextMusicIndex == 0) {
            Audioplayer.nNextMusicIndex = 10;
        }
    }
}




// Ab hier MODPlay

ModPlayerStatus_t mp;

static const int32_t finetune_table[16] = {
	65536, 65065, 64596, 64132,
	63670, 63212, 62757, 62306,
	69433, 68933, 68438, 67945,
	67456, 66971, 66489, 66011
};

static const uint8_t sine_table[32] = {
	0, 24, 49, 74, 97, 120, 141, 161,
	180, 197, 212, 224, 235, 244, 250, 253,
	255, 253, 250, 244, 235, 224, 212, 197,
	180, 161, 141, 120, 97, 74, 49, 24
};

static const int32_t arpeggio_table[16] = {
	65536, 61858, 58386, 55109,
	52016, 49096, 46341, 43740,
	41285, 38968, 36781, 34716,
	32768, 30929, 29193, 27554
};

void _RecalculateWaveform(Oscillator_t *oscillator) {
	int32_t result = 0;

	// The following generators _might_ have been inspired by micromod's code:
	// https://github.com/martincameron/micromod/blob/master/micromod-c/micromod.c

	switch(oscillator->waveform) {
		case 0:
			// Sine
			result = sine_table[oscillator->phase & 0x1F];
			if((oscillator->phase & 0x20) > 0) result *= (-1);
			break;

		case 1:
			// Sawtooth
			result = 255 - (((oscillator->phase + 0x20) & 0x3F) << 3);
			break;

		case 2:
			// Square
			result = 255 - ((oscillator->phase & 0x20) << 4);
			break;

		case 3:
			// Random
			result = (mp.random >> 20) - 255;
			mp.random = (mp.random * 65 + 17) & 0x1FFFFFFF;
			break;
	}

	oscillator->val = result * oscillator->depth;
}


/*
 * ModPlayerStatus_t *ProcessMOD(void);
 *
 * Advances to the next tick of the MOD file.
 *
 * Handles the decoding of all pattern data (notes, samples, effects)
 * and generates pitch/volume/sample offset commands for an external sampler
 * (this info is accessible in the returned object->paula[0..3]).
 *
 * NOTE: THIS FUNCTION IS CALLED INTERNALLY BY RenderMOD()!
 * Please do not call this function if you are building a desktop
 * application, use RenderMOD() instead, which will give you
 * raw sample data that you can pass to your audio subsystem.
 *
 * This function is only intended to be called by your application
 * if you are not interested in MODPlay's sample renderer
 * and want to create your own, or you want to use this library
 * on system with actual hardware samplers (such as the Commodore Amiga
 * or the Sony PlayStation).
 */
ModPlayerStatus_t *ProcessMOD(void) {
	if(mp.tick == 0) {
		mp.skiporderrequest = -1;

		for(int i = 0; i < mp.channels; i++) {
			mp.ch[i].vibrato.val = mp.ch[i].tremolo.val = 0;

			const uint8_t *cell = mp.patterndata + 4 * (i + mp.channels * (mp.row + 64 * mp.ordertable[mp.order]));

			int note_tmp = ((cell[0] << 8) | cell[1]) & 0xFFF;
			int sample_tmp = (cell[0] & 0xF0) | (cell[2] >> 4);
			int eff_tmp = cell[2] & 0x0F;
			int effval_tmp = cell[3];

			if(mp.ch[i].eff == 0 && mp.ch[i].effval != 0) {
				mp.ch[i].period = mp.ch[i].note;
			}

			if(sample_tmp) {
				if(sample_tmp > 31) sample_tmp = 1;

				mp.ch[i].sample = sample_tmp - 1;

				mp.ch[i].samplegen.length = mp.samples[sample_tmp - 1].actuallength << 1;
				mp.ch[i].samplegen.looplength = mp.samples[sample_tmp - 1].looplength << 1;
				mp.ch[i].volume = mp.sampleheaders[sample_tmp - 1].volume;
				mp.ch[i].samplegen.sample = mp.samples[sample_tmp - 1].data;
			}

			if(note_tmp) {
				int finetune;

				if(eff_tmp == 0xE && (effval_tmp & 0xF0) == 0x50)
					finetune = effval_tmp & 0xF;
				else
					finetune = mp.sampleheaders[mp.ch[i].sample].finetune;

				note_tmp = note_tmp * finetune_table[finetune & 0xF] >> 16;

				mp.ch[i].note = note_tmp;

				if(eff_tmp != 0x3 && eff_tmp != 0x5 && (eff_tmp != 0xE || (effval_tmp & 0xF0) != 0xD0)) {
					mp.ch[i].samplegen.age = mp.ch[i].samplegen.currentptr = 0;
					mp.ch[i].period = mp.ch[i].note;

					if(mp.ch[i].vibrato.waveform < 4) mp.ch[i].vibrato.phase = 0;
					if(mp.ch[i].tremolo.waveform < 4) mp.ch[i].tremolo.phase = 0;
				}
			}

			if(eff_tmp || effval_tmp) switch(eff_tmp) {
				case 0x3:
					if(effval_tmp) mp.ch[i].slideamount = effval_tmp;

				case 0x5:
					mp.ch[i].slidenote = mp.ch[i].note;
					break;

				case 0x4:
					if(effval_tmp & 0xF0) mp.ch[i].vibrato.speed = effval_tmp >> 4;
					if(effval_tmp & 0x0F) mp.ch[i].vibrato.depth = effval_tmp & 0x0F;

					// break intentionally left out here

				case 0x6:
					_RecalculateWaveform(&mp.ch[i].vibrato);
					break;

				case 0x7:
					if(effval_tmp & 0xF0) mp.ch[i].tremolo.speed = effval_tmp >> 4;
					if(effval_tmp & 0x0F) mp.ch[i].tremolo.depth = effval_tmp & 0x0F;
					_RecalculateWaveform(&mp.ch[i].tremolo);
					break;

				case 0xC:
					mp.ch[i].volume = (effval_tmp > 0x40) ? 0x40 : effval_tmp;
					break;

				case 0x9:
					if(effval_tmp) {
						mp.ch[i].samplegen.currentptr = effval_tmp << 8;
						mp.ch[i].sampleoffset = effval_tmp;
					} else {
						mp.ch[i].samplegen.currentptr = mp.ch[i].sampleoffset << 8;
					}

					mp.ch[i].samplegen.age = 0;
					break;

				case 0xB:
					if(effval_tmp >= mp.orders) effval_tmp = 0;

					mp.skiporderrequest = effval_tmp;
					break;

				case 0xD:
					if(mp.skiporderrequest < 0) {
						if(mp.order + 1 < mp.orders)
							mp.skiporderrequest = mp.order + 1;
						else
							mp.skiporderrequest = 0;
					}

					if(effval_tmp > 0x63) effval_tmp = 0;

					mp.skiporderdestrow = (effval_tmp >> 4) * 10 + (effval_tmp & 0xF); // What were the ProTracker guys smoking?!
					break;

				case 0xE:
					switch(effval_tmp >> 4) {
						case 0x1:
							mp.ch[i].period -= effval_tmp & 0xF;
							break;

						case 0x2:
							mp.ch[i].period += effval_tmp & 0xF;
							break;

						case 0x4:
							mp.ch[i].vibrato.waveform = effval_tmp & 0x7;
							break;

						case 0x6:
							if(effval_tmp & 0xF) {
								if(!mp.patloopcycle)
									mp.patloopcycle = (effval_tmp & 0xF) + 1;

								if(mp.patloopcycle > 1) {
									mp.skiporderrequest = mp.order;
									mp.skiporderdestrow = mp.patlooprow;
								}

								mp.patloopcycle--;
							} else {
								mp.patlooprow = mp.row;
							}

						case 0x7:
							mp.ch[i].tremolo.waveform = effval_tmp & 0x7;
							break;

						case 0xA:
							mp.ch[i].volume += effval_tmp & 0xF;
							if(mp.ch[i].volume > 0x40) mp.ch[i].volume = 0x40;
							break;

						case 0xB:
							mp.ch[i].volume -= effval_tmp & 0xF;
							if(mp.ch[i].volume < 0x00) mp.ch[i].volume = 0x00;
							break;

						case 0xE:
							mp.maxtick *= ((effval_tmp & 0xF) + 1);
							break;
					}
					break;

				case 0xF:
					if(effval_tmp) {
						if(effval_tmp < 0x20) {
							mp.maxtick = (mp.maxtick / mp.speed) * effval_tmp;
							mp.speed = effval_tmp;
						} else {
							mp.audiospeed = mp.samplerate * 125 / effval_tmp / 50;
						}
					}

					break;
			}

			mp.ch[i].eff = eff_tmp;
			mp.ch[i].effval = effval_tmp;
		}
	}

	for(int i = 0; i < mp.channels; i++) {
		int eff_tmp = mp.ch[i].eff;
		int effval_tmp = mp.ch[i].effval;

		if(eff_tmp || effval_tmp) switch(eff_tmp) {
			case 0x0:
				switch(mp.tick % 3) {
					case 0:
						mp.ch[i].period = mp.ch[i].note;
						break;

					case 1:
						mp.ch[i].period = (mp.ch[i].note * arpeggio_table[effval_tmp >> 4]) >> 16;
						break;

					case 2:
						mp.ch[i].period = (mp.ch[i].note * arpeggio_table[effval_tmp & 0xF]) >> 16;
						break;
				}
				break;

			case 0x1:
				if(mp.tick) mp.ch[i].period -= effval_tmp;
				break;

			case 0x2:
				if(mp.tick) mp.ch[i].period += effval_tmp;
				break;

			case 0x5:
				if(mp.tick) {
					if(effval_tmp > 0xF) {
						mp.ch[i].volume += (effval_tmp >> 4);
						if(mp.ch[i].volume > 0x40) mp.ch[i].volume = 0x40;
					} else {
						mp.ch[i].volume -= (effval_tmp & 0xF);
						if(mp.ch[i].volume < 0x00) mp.ch[i].volume = 0x00;
					}
				}

				effval_tmp = 0;
				// break intentionally left out here

			case 0x3:
				if(mp.tick) {
					if(!effval_tmp) effval_tmp = mp.ch[i].slideamount;

					if(mp.ch[i].slidenote > mp.ch[i].period) {
						mp.ch[i].period += effval_tmp;

						if(mp.ch[i].slidenote < mp.ch[i].period)
							mp.ch[i].period = mp.ch[i].slidenote;
					} else if(mp.ch[i].slidenote < mp.ch[i].period) {
						mp.ch[i].period -= effval_tmp;

						if(mp.ch[i].slidenote > mp.ch[i].period)
							mp.ch[i].period = mp.ch[i].slidenote;
					}
				}

				break;

			case 0x4:
				if(mp.tick) {
					mp.ch[i].vibrato.phase += mp.ch[i].vibrato.speed;
					_RecalculateWaveform(&mp.ch[i].vibrato);
				}
				break;

			case 0x6:
				if(mp.tick) {
					mp.ch[i].vibrato.phase += mp.ch[i].vibrato.speed;
					_RecalculateWaveform(&mp.ch[i].vibrato);
				}
				// break intentionally left out here

			case 0xA:
				if(mp.tick) {
					if(effval_tmp > 0xF) {
						mp.ch[i].volume += (effval_tmp >> 4);
						if(mp.ch[i].volume > 0x40) mp.ch[i].volume = 0x40;
					} else {
						mp.ch[i].volume -= (effval_tmp & 0xF);
						if(mp.ch[i].volume < 0x00) mp.ch[i].volume = 0x00;
					}
				}

				break;

			case 0x7:
				if(mp.tick) {
					mp.ch[i].tremolo.phase += mp.ch[i].tremolo.speed;
					_RecalculateWaveform(&mp.ch[i].tremolo);
				}
				break;

			case 0xE:
				switch(effval_tmp >> 4) {
					case 0x9:
						if(mp.tick && !(mp.tick % (effval_tmp & 0xF)))
							mp.ch[i].samplegen.age = mp.ch[i].samplegen.currentptr = mp.ch[i].samplegen.currentsubptr = 0;
						break;

					case 0xC:
						if(mp.tick >= (effval_tmp & 0xF)) mp.ch[i].volume = 0;
						break;

					case 0xD:
						if(mp.tick == (effval_tmp & 0xF)) {
							mp.ch[i].samplegen.age = mp.ch[i].samplegen.currentptr = mp.ch[i].samplegen.currentsubptr = 0;
							mp.ch[i].period = mp.ch[i].note;
						}
						break;
				}

				break;
		}

		if(mp.ch[i].period < 0 && mp.ch[i].period != 0) {
			mp.ch[i].period = 0;
		}

		// Pre-calculate sampler period & volume

		if(mp.ch[i].period)
			mp.ch[i].samplegen.period = mp.paularate / (mp.ch[i].period + (mp.ch[i].vibrato.val >> 7));
		else
			mp.ch[i].samplegen.period = 0;

		int32_t vol = mp.ch[i].volume + (mp.ch[i].tremolo.val >> 6);

		if(vol < 0) vol = 0;
		if(vol > 64) vol = 64;

		mp.ch[i].samplegen.volume = vol;
	}

	mp.tick++;
	if(mp.tick >= mp.maxtick) {
		mp.tick = 0;
		mp.maxtick = mp.speed;

		if(mp.skiporderrequest >= 0) {
			mp.row = mp.skiporderdestrow;
			mp.order = mp.skiporderrequest;

			mp.skiporderdestrow = 0;
			mp.skiporderrequest = -1;
		} else {
			mp.row++;
			if(mp.row >= 0x40) {
				mp.row = 0;
				mp.order++;

				if(mp.order >= mp.orders) mp.order = 0;
			}
		}
	}

	return &mp;
}


/*
 * ModPlayerStatus_t *RenderMOD(short *buf, int len);
 *
 * Renders a buffer from the MOD given to InitMOD() to `*buf`.
 *
 * NOTE: `len` specifies the number of samples, NOT BYTES.
 * This MOD player renders in 16-bit stereo, so the `*buf` array
 * is expected to have `len` * 4 allocated bytes of memory.
 *
 * The samples are interleaved as follows:
 *
 * Index | Value
 * ------+------
 * 0     | Sample 0, left channel
 * 1     | Sample 0, right channel
 * 2     | Sample 1, left channel
 * 3     | Sample 1, right channel
 * ...   | ...
 *
 * This makes it compatible with many popular audio output
 * libraries without any extra conversion (SDL, PortAudio etc).
 */
 ModPlayerStatus_t *RenderMOD(short *buf, int len) {
	memset(buf, 0, len * 4);

	int32_t majorchmul = 131072 / (mp.channels / 2);
	int32_t minorchmul = 131072 / 3 / (mp.channels / 2);

	for(int s = 0; s < len; s++) {
		// Process the tick, if necessary

		if(!mp.audiotick) {
			ProcessMOD();
			mp.audiotick = mp.audiospeed;
		}

		mp.audiotick--;

		// Render the audio

		int32_t l = 0, r = 0;

		for(int ch = 0; ch < mp.channels; ch++) {
			PaulaChannel_t *pch = &mp.ch[ch].samplegen;

			if(pch->sample) {
				// If the single-shot sample has finished playing, skip this channel

				if((pch->looplength == 0) && (pch->currentptr >= pch->length))
					continue;

				// If it is a looping sample, wrap around to the loop point

				while(pch->currentptr >= pch->length)
					pch->currentptr -= pch->looplength;

				// Render the current sample

				if(!pch->muted) {

					uint32_t nextptr = pch->currentptr + 1;

					while(nextptr >= pch->length) {
						if(pch->looplength != 0)
							nextptr -= pch->looplength;
						else
							nextptr = pch->currentptr;
					}

					//assert(pch->currentptr < pch->length, "channel: %d, test %u < %u", ch, pch->currentptr, pch->length);
					//assert(nextptr < pch->length, "channel: %d, test %u < %u", ch, nextptr, pch->length);

					int32_t sample1 = pch->sample[pch->currentptr];
					int32_t sample2 = pch->sample[nextptr];

					//assert(pch->currentsubptr < 0x10000, "channel: %d, test %u < 0x10000", ch, pch->currentsubptr);

					int32_t sample = (sample1 * (0x10000 - pch->currentsubptr) +
						sample2 * pch->currentsubptr) * pch->volume / 65536;

                    // MIK: externe Lautstärkeregelung
                    if (Audioplayer.uMusicVolumePercent < 100) {
                        sample = (short)(((int32_t)sample * Audioplayer.uMusicVolumePercent) / 100);
                    }

					// Distribute the rendered sample across both output channels

					if((ch & 3) == 1 || (ch & 3) == 2) {
						l += sample * minorchmul;
						r += sample * majorchmul;
					} else {
						l += sample * majorchmul;
						r += sample * minorchmul;
					}
				}

				// Advance to the next required sample

				pch->currentsubptr += pch->period;

				if(pch->currentsubptr >= 0x10000) {
					pch->currentptr += pch->currentsubptr >> 16;
					pch->currentsubptr &= 0xFFFF;
				}

				if(pch->age < INT32_MAX)
					pch->age++;
			}
		}

		buf[s * 2] = l / 65536;
		buf[s * 2 + 1] = r / 65536;
	}

	return &mp;
}


/*
 * ModPlayerStatus_t *InitMOD(const uint8_t *mod, uint32_t samplerate);
 *
 * Initializes the MOD player with the given mod file and samplerate.
 */
ModPlayerStatus_t *InitMOD(const uint8_t *mod, uint32_t samplerate) {
	uint32_t signature = mod[1083] | (mod[1082] << 8) | (mod[1081] << 16) | (mod[1080] << 24);

	int channels = 0;

	// M.K. and M!K! = 4 channels

	if(signature == 0x4D2E4B2E || signature == 0x4D214B21) {
		channels = 4;
	}

	// FLTx

	if((signature & 0xFFFFFF00) == 0x464C5400) {
		channels = (signature & 0xFF) - '0';
	}

	// xCHN

	if((signature & 0x00FFFFFF) == 0x0043484E) {
		channels = (signature >> 24) - '0';
	}

	// xxCH

	if((signature & 0x0000FFFF) == 0x00004348) {
		channels = ((signature >> 24) & 0xF) * 10 + ((signature >> 16) & 0xF);
	}

	if(channels == 0 || channels >= CHANNELS) {
		return NULL;
	}

	memset(&mp, 0, sizeof(mp));

	mp.channels = channels;

	mp.samplerate = samplerate;
	mp.paularate = (3546895 / samplerate) << 16;

	mp.orders = mod[950];
	mp.ordertable = mod + 952;

	mp.maxpattern = 0;

	for(int i = 0; i < 128; i++) {
		if(mp.ordertable[i] >= mp.maxpattern) mp.maxpattern = mp.ordertable[i];
	}
	mp.maxpattern++;

	const int8_t *samplemem = ((const int8_t *) mod) + 1084 + 64 * 4 * mp.channels * mp.maxpattern;
	mp.patterndata = mod + 1084;

	mp.sampleheaders = (SampleHeader_t *) (mod + 20);

	for(int i = 0; i < 31; i++) {
		const SampleHeader_t *sample = mp.sampleheaders + i;

		uint16_t length = (sample->lengthhi << 8) | sample->lengthlo;
		uint16_t looppoint = (sample->looppointhi << 8) | sample->looppointlo;
		mp.samples[i].actuallength = (sample->looplengthhi << 8) | sample->looplengthlo;

		mp.samples[i].data = samplemem;
		samplemem += length * 2;

		mp.samples[i].actuallength += looppoint;

		if(mp.samples[i].actuallength < 0x2) {
			mp.samples[i].actuallength = length;
			looppoint = 0xFFFF;
			mp.samples[i].looplength = 0;
		} else if(mp.samples[i].actuallength > length) {
			looppoint /= 2;
			mp.samples[i].actuallength -= looppoint;
			mp.samples[i].looplength = mp.samples[i].actuallength - looppoint;
		} else {
			mp.samples[i].looplength = mp.samples[i].actuallength - looppoint;
		}
	}

	mp.maxtick = mp.speed = 6; mp.audiospeed = mp.samplerate / 50;

	for(int i = 0; i < mp.channels; i++) {
		mp.ch[i].samplegen.age = INT32_MAX;
	}
    SetMusicVolume(100); // MIK
	return &mp;
}


/*----------------------------------------------------------------------------
Name:           SetMusicVolume
------------------------------------------------------------------------------
Beschreibung: Setzt die Lautstärke der abzuspielenden Musik in Prozent.
Parameter
      Eingang: uVolumePercent, uint8_t, Lautstärke in Prozent (0 = leise, 100 maximale Lautstärke)
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Audioplayer.x
------------------------------------------------------------------------------*/
void SetMusicVolume(uint8_t uVolumePercent) {
    if (uVolumePercent <= 100) {
        Audioplayer.uMusicVolumePercent = uVolumePercent;
    }
}
