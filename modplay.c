#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "config.h"
#include "modplay.h"

ModPlayerStatus_t mp;
AUDIOPLAYER Audioplayer;

extern CONFIG Config;

// Externe Pointer und Indexe
extern uint8_t _binary_echoing2_mod_start;extern uint8_t _binary_echoing2_mod_end;                      // 1. Mod von banana
extern uint8_t _binary_class01_mod_start;extern uint8_t _binary_class01_mod_end;                        // 3. class cracktro#15 Mod von maktone,1999
extern uint8_t _binary_gtrash3f_mod_start;extern uint8_t _binary_gtrash3f_mod_end;                      // 4. global trash 3 V2 von Jesper Kyd, 1991
extern uint8_t _binary_class11_mod_start;extern uint8_t _binary_class11_mod_end;                        // 5. class11.mod (class11.time flies) von Maktone
extern uint8_t _binary_2kad04_mod_start;extern uint8_t _binary_2kad04_mod_end;                          // 6. 2kad04.mod (2000AD:cracktro:IV) von Maktone
extern uint8_t _binary_2kad02_mod_start;extern uint8_t _binary_2kad02_mod_end;                          // 7. 2kad02.mod (2000AD cracktro02)  von Maktone
extern uint8_t _binary_brewery_mod_start;extern uint8_t _binary_brewery_mod_end;                        // 8. brewery.mod (the brewery)  von Maktone
extern uint8_t _binary_class05_1999_mod_start;extern uint8_t _binary_class05_1999_mod_end;              // 9. class05 von Maktone
extern uint8_t _binary_softworld_mod_start;extern uint8_t _binary_softworld_mod_end;                    // 10. softworld von Maktone
extern uint8_t _binary_circus_time_2_1993_mod_start;extern uint8_t _binary_circus_time_2_1993_mod_end;  // 11. circus time 2 von voyce/delight



uint8_t* g_pMusicPointer[(MAX_MUSICINDEX + 1) * 2];          // 2 Pointer / Musik + Pärchen NULL-Pointer

// Prototypen


/*----------------------------------------------------------------------------
Name:           InitMusicPointer
------------------------------------------------------------------------------
Beschreibung: Initialisiert das modulglobale MusicPointer-Array
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte:  g_pMusicPointer
                alle externen Pointer auf die MOD-Musikstücke
------------------------------------------------------------------------------*/
void InitMusicPointer(void) {
    g_pMusicPointer[0] = &_binary_echoing2_mod_start;g_pMusicPointer[1] = &_binary_echoing2_mod_end;
    g_pMusicPointer[2] = &_binary_circus_time_2_1993_mod_start;g_pMusicPointer[3] = &_binary_circus_time_2_1993_mod_end;
    g_pMusicPointer[4] = &_binary_class01_mod_start;g_pMusicPointer[5] = &_binary_class01_mod_end;
    g_pMusicPointer[6] = &_binary_gtrash3f_mod_start;g_pMusicPointer[7] = &_binary_gtrash3f_mod_end;
    g_pMusicPointer[8] = &_binary_class11_mod_start;g_pMusicPointer[9] = &_binary_class11_mod_end;
    g_pMusicPointer[10] = &_binary_2kad04_mod_start;g_pMusicPointer[11] = &_binary_2kad04_mod_end;
    g_pMusicPointer[12] = &_binary_2kad02_mod_start;g_pMusicPointer[13] = &_binary_2kad02_mod_end;
    g_pMusicPointer[14] = &_binary_brewery_mod_start;g_pMusicPointer[15] = &_binary_brewery_mod_end;
    g_pMusicPointer[16] = &_binary_class05_1999_mod_start;g_pMusicPointer[17] = &_binary_class05_1999_mod_end;
    g_pMusicPointer[18] = &_binary_softworld_mod_start;g_pMusicPointer[19] = &_binary_softworld_mod_end;
    g_pMusicPointer[20] = NULL;g_pMusicPointer[21] = NULL;// Ende
}

/*----------------------------------------------------------------------------
Name:           InitAudioplayerStruct
------------------------------------------------------------------------------
Beschreibung: Initialisiert die Struktur Audioplayer.x und öffnet das
              Audiodevice für MODPlay.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int, 0 = OK, sonst Fehler
Seiteneffekte: Audioplayer.x
------------------------------------------------------------------------------*/
int InitAudioplayerStruct(void) {
    int nErrorcode = -1;

    InitMusicPointer();
    memset(&Audioplayer,0,sizeof(AUDIOPLAYER));
    Audioplayer.sdl_audio.freq = SAMPLERATE;
    Audioplayer.sdl_audio.format = AUDIO_S16;
    Audioplayer.sdl_audio.channels = 2;
    Audioplayer.sdl_audio.samples = AUDIO_BUFFERSIZE;
    Audioplayer.sdl_audio.callback = NULL;
    Audioplayer.audio_device = SDL_OpenAudioDevice(NULL, 0, &Audioplayer.sdl_audio, NULL, 0);
    if (Audioplayer.audio_device > 0) {
        SDL_PauseAudioDevice(Audioplayer.audio_device, 0);
        nErrorcode = 0;
    } else {
        SDL_Log("%s: SDL_OpenAudioDevice() failed: %s",__FUNCTION__,SDL_GetError());
    }
    return nErrorcode;
}


/*----------------------------------------------------------------------------
Name:           SetModMusic
------------------------------------------------------------------------------
Beschreibung: Stellt ein MOD-File für MODPlay zum Abspielen bereit.

Parameter
      Eingang: nMusicIndex, int, Index auf MOD-File, siehe oben "Externe Pointer und Indexe"
      Ausgang: -
Rückgabewert:  int, 0 = OK, sonst Fehler
Seiteneffekte: Audioplayer.x,
               g_pMusicPointer[] (alle externen Pointer auf die MOD-Musikstücke)
------------------------------------------------------------------------------*/
int SetModMusic(int nMusicIndex) {
    int nErrorCode;

    nErrorCode = -1;
    if ( (nMusicIndex < 1) || (nMusicIndex > MAX_MUSICINDEX) ) {
        nMusicIndex = 1;
    }
    nMusicIndex--; // Pointer-Array ab 0
    Audioplayer.nMusicIndex = nMusicIndex;
    Audioplayer.pMusicStart = g_pMusicPointer[nMusicIndex * 2 + 0];
    Audioplayer.pMusicEnd = g_pMusicPointer[nMusicIndex * 2 + 1];
    Audioplayer.nNextMusicIndex = 0;

    if ( (Audioplayer.pMusicStart != NULL) && (Audioplayer.pMusicEnd != NULL) ) {
        Audioplayer.nMusicSize = Audioplayer.pMusicEnd - Audioplayer.pMusicStart;
    } else {
        Audioplayer.nMusicSize = 0;
    }
    if (Audioplayer.nMusicSize > 0) {
        Audioplayer.pTheMusic = (uint8_t*)realloc(Audioplayer.pTheMusic,Audioplayer.nMusicSize);
        if (Audioplayer.pTheMusic != NULL) {
            memcpy(Audioplayer.pTheMusic,Audioplayer.pMusicStart,Audioplayer.nMusicSize);
            if (InitMOD(Audioplayer.pTheMusic, Audioplayer.sdl_audio.freq) != NULL) {
                nErrorCode = 0;
            } else {
                SDL_Log("%s: invalid mod file, data size: %d",__FUNCTION__,Audioplayer.nMusicSize);
            }
        }
    } else {
        SDL_Log("%s: bad mod file size, MusicIndex: %d",__FUNCTION__,nMusicIndex + 1);
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
Rückgabewert:  int, 0 = OK, sonst Fehler
Seiteneffekte: Audioplayer.x, Config.x
------------------------------------------------------------------------------*/
int PlayMusic(bool bIgnoreConfig) {
    int nErrorCode;
    // ModPlayerStatus_t *m;

    // m = &mp;
    // printf("\rRow %02d, order %02d/%02d (pattern %02d) @ speed %d", m->row, m->order, m->orders - 1, m->ordertable[m->order], m->speed);
    if ((Config.bGameMusic) || (bIgnoreConfig)) {
        if (SDL_GetQueuedAudioSize(Audioplayer.audio_device) < (Audioplayer.sdl_audio.samples * 4)) {
            RenderMOD(Audioplayer.audiobuffer, Audioplayer.sdl_audio.samples);
            nErrorCode = SDL_QueueAudio(Audioplayer.audio_device,Audioplayer. audiobuffer, Audioplayer.sdl_audio.samples * 4); // 2 channels, 2 bytes/sample
            if (nErrorCode != 0) {
                SDL_Log("%s: SDL_QueueAudio() failed: %s",__FUNCTION__,SDL_GetError());
            }
        } else {
            nErrorCode = 0; // Queue ist noch voll
        }
    } else {
        nErrorCode = 0; // Es soll nicht gespielt werden
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


short period_tables[16][3*12] = {
	{
		856,808,762,720,678,640,604,570,538,508,480,453,
		428,404,381,360,339,320,302,285,269,254,240,226,
		214,202,190,180,170,160,151,143,135,127,120,113
	}, {
		850,802,757,715,674,637,601,567,535,505,477,450,
		425,401,379,357,337,318,300,284,268,253,239,225,
		213,201,189,179,169,159,150,142,134,126,119,113
	}, {
		844,796,752,709,670,632,597,563,532,502,474,447,
		422,398,376,355,335,316,298,282,266,251,237,224,
		211,199,188,177,167,158,149,141,133,125,118,112
	}, {
		838,791,746,704,665,628,592,559,528,498,470,444,
		419,395,373,352,332,314,296,280,264,249,235,222,
		209,198,187,176,166,157,148,140,132,125,118,111
	}, {
		832,785,741,699,660,623,588,555,524,495,467,441,
		416,392,370,350,330,312,294,278,262,247,233,220,
		208,196,185,175,165,156,147,139,131,124,117,110
	}, {
		826,779,736,694,655,619,584,551,520,491,463,437,
		413,390,368,347,328,309,292,276,260,245,232,219,
		206,195,184,174,164,155,146,138,130,123,116,109
	}, {
		820,774,730,689,651,614,580,547,516,487,460,434,
		410,387,365,345,325,307,290,274,258,244,230,217,
		205,193,183,172,163,154,145,137,129,122,115,109
	}, {
		814,768,725,684,646,610,575,543,513,484,457,431,
		407,384,363,342,323,305,288,272,256,242,228,216,
		204,192,181,171,161,152,144,136,128,121,114,108
	}, {
		907,856,808,762,720,678,640,604,570,538,508,480,
		453,428,404,381,360,339,320,302,285,269,254,240,
		226,214,202,190,180,170,160,151,143,135,127,120
	}, {
		900,850,802,757,715,675,636,601,567,535,505,477,
		450,425,401,379,357,337,318,300,284,268,253,238,
		225,212,200,189,179,169,159,150,142,134,126,119
	}, {
		894,844,796,752,709,670,632,597,563,532,502,474,
		447,422,398,376,355,335,316,298,282,266,251,237,
		223,211,199,188,177,167,158,149,141,133,125,118
	}, {
		887,838,791,746,704,665,628,592,559,528,498,470,
		444,419,395,373,352,332,314,296,280,264,249,235,
		222,209,198,187,176,166,157,148,140,132,125,118
	}, {
		881,832,785,741,699,660,623,588,555,524,494,467,
		441,416,392,370,350,330,312,294,278,262,247,233,
		220,208,196,185,175,165,156,147,139,131,123,117
	}, {
		875,826,779,736,694,655,619,584,551,520,491,463,
		437,413,390,368,347,328,309,292,276,260,245,232,
		219,206,195,184,174,164,155,146,138,130,123,116
	}, {
		868,820,774,730,689,651,614,580,547,516,487,460,
		434,410,387,365,345,325,307,290,274,258,244,230,
		217,205,193,183,172,163,154,145,137,129,122,115
	}, {
		862,814,768,725,684,646,610,575,543,513,484,457,
		431,407,384,363,342,323,305,288,272,256,242,228,
		216,203,192,181,171,161,152,144,136,128,121,114
	}
};

const unsigned char sine_table[32] = {
	  0, 24, 49, 74, 97,120,141,161,
	180,197,212,224,235,244,250,253,
	255,253,250,244,235,224,212,197,
	180,161,141,120, 97, 74, 49, 24
};

const int arpeggio_table[16] = {
	65536, 61858, 58386, 55109,
	52016, 49096, 46341, 43740,
	41285, 38968, 36781, 34716,
	32768, 30929, 29193, 27554
};



void _RecalculateWaveform(Oscillator *oscillator) {
	int result = 0;

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

ModPlayerStatus_t *ProcessMOD() {
	int i;

	if(mp.tick == 0) {
		mp.skiporderrequest = -1;

		for(i = 0; i < 4; i++) {
			mp.vibrato[i].val = mp.tremolo[i].val = 0;

			uint8_t *cell = mp.patterndata + mp.ordertable[mp.order] * (64 * 16) + mp.row * 16 + i * 4;

			int note_tmp = cell[0];
			int sample_tmp = cell[1];
			int eff_tmp = cell[2];
			int effval_tmp = cell[3];

			if(mp.eff[i] == 0 && mp.effval[i] != 0) {
				mp.paula[i].period = mp.note[i];
			}

			if(sample_tmp) {
				mp.sample[i] = sample_tmp - 1;

				mp.paula[i].length = mp.samples[sample_tmp - 1].actuallength;
				mp.paula[i].looplength = mp.samples[sample_tmp - 1].looplength;
				mp.paula[i].volume = mp.samples[sample_tmp - 1].volume;
				mp.paula[i].sample = mp.samples[sample_tmp - 1].data;
			}

			if(note_tmp) {
				char finetune;

				if(eff_tmp == 0xE && (effval_tmp & 0xF0) == 0x50)
					finetune = effval_tmp & 0xF;
				else
					finetune = mp.samples[mp.sample[i]].finetune;

				note_tmp = period_tables[(int)finetune][note_tmp - 1];

				mp.note[i] = note_tmp;

				if(eff_tmp != 0x3 && eff_tmp != 0x5 && (eff_tmp != 0xE || (effval_tmp & 0xF0) != 0xD0)) {
					mp.paula[i].age = mp.paula[i].currentptr = 0;
					mp.paula[i].period = mp.note[i];

					if(mp.vibrato[i].waveform < 4) mp.vibrato[i].phase = 0;
					if(mp.tremolo[i].waveform < 4) mp.tremolo[i].phase = 0;
				}
			}

			if(eff_tmp || effval_tmp) switch(eff_tmp) {
				case 0x3:
					if(effval_tmp) mp.slideamount[i] = effval_tmp;

				case 0x5:
					mp.slidenote[i] = mp.note[i];
					break;

				case 0x4:
					if(effval_tmp & 0xF0) mp.vibrato[i].speed = effval_tmp >> 4;
					if(effval_tmp & 0x0F) mp.vibrato[i].depth = effval_tmp & 0x0F;

					// break intentionally left out here

				case 0x6:
					_RecalculateWaveform(&mp.vibrato[i]);
					break;

				case 0x7:
					if(effval_tmp & 0xF0) mp.tremolo[i].speed = effval_tmp >> 4;
					if(effval_tmp & 0x0F) mp.tremolo[i].depth = effval_tmp & 0x0F;
					_RecalculateWaveform(&mp.tremolo[i]);
					break;

				case 0xC:
					mp.paula[i].volume = (effval_tmp > 0x40) ? 0x40 : effval_tmp;
					break;

				case 0x9:
					if(effval_tmp) {
						mp.paula[i].currentptr = effval_tmp << 24;
						mp.sampleoffset[i] = effval_tmp;
					} else {
						mp.paula[i].currentptr = mp.sampleoffset[i] << 24;
					}

					mp.paula[i].age = 0;
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

					//if(effval_tmp > 0x3F) effval_tmp = 0;
					if(effval_tmp > 0x63) effval_tmp = 0;

					mp.skiporderdestrow = (effval_tmp >> 4) * 10 + (effval_tmp & 0xF); // What were the ProTracker guys smoking?!
					break;

				case 0xE:
					switch(effval_tmp >> 4) {
						case 0x1:
							mp.paula[i].period -= effval_tmp & 0xF;
							break;

						case 0x2:
							mp.paula[i].period += effval_tmp & 0xF;
							break;

						case 0x4:
							mp.vibrato[i].waveform = effval_tmp & 0x7;
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
							mp.tremolo[i].waveform = effval_tmp & 0x7;
							break;

						case 0xA:
							mp.paula[i].volume += effval_tmp & 0xF;
							if(mp.paula[i].volume > 0x40) mp.paula[i].volume = 0x40;
							break;

						case 0xB:
							mp.paula[i].volume -= effval_tmp & 0xF;
							if(mp.paula[i].volume < 0x00) mp.paula[i].volume = 0x00;
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

			mp.eff[i] = eff_tmp;
			mp.effval[i] = effval_tmp;
		}
	}

	for(i = 0; i < 4; i++) {
		int eff_tmp = mp.eff[i];
		int effval_tmp = mp.effval[i];

		if(eff_tmp || effval_tmp) switch(eff_tmp) {
			case 0x0:
				switch(mp.tick % 3) {
					case 0:
						mp.arp = mp.note[i];
						break;

					case 1:
						mp.arp = (mp.note[i] * arpeggio_table[effval_tmp >> 4]) >> 16;
						break;

					case 2:
						mp.arp = (mp.note[i] * arpeggio_table[effval_tmp & 0xF]) >> 16;
						break;
				}

				mp.paula[i].period = mp.arp;
				break;

			case 0x1:
				if(mp.tick) mp.paula[i].period -= effval_tmp;
				break;

			case 0x2:
				if(mp.tick) mp.paula[i].period += effval_tmp;
				break;

			case 0x5:
				if(mp.tick) {
					if(effval_tmp > 0xF) {
						mp.paula[i].volume += (effval_tmp >> 4);
						if(mp.paula[i].volume > 0x40) mp.paula[i].volume = 0x40;
					} else {
						mp.paula[i].volume -= (effval_tmp & 0xF);
						if(mp.paula[i].volume < 0x00) mp.paula[i].volume = 0x00;
					}
				}

				effval_tmp = 0;
				// break intentionally left out here

			case 0x3:
				if(mp.tick) {
					if(!effval_tmp) effval_tmp = mp.slideamount[i];

					if(mp.slidenote[i] > mp.paula[i].period) {
						mp.paula[i].period += effval_tmp;

						if(mp.slidenote[i] < mp.paula[i].period)
							mp.paula[i].period = mp.slidenote[i];
					} else if(mp.slidenote[i] < mp.paula[i].period) {
						mp.paula[i].period -= effval_tmp;

						if(mp.slidenote[i] > mp.paula[i].period)
							mp.paula[i].period = mp.slidenote[i];
					}
				}

				break;

			case 0x4:
				if(mp.tick) {
					mp.vibrato[i].phase += mp.vibrato[i].speed;
					_RecalculateWaveform(&mp.vibrato[i]);
				}
				break;

			case 0x6:
				if(mp.tick) {
					mp.vibrato[i].phase += mp.vibrato[i].speed;
					_RecalculateWaveform(&mp.vibrato[i]);
				}
				// break intentionally left out here

			case 0xA:
				if(mp.tick) {
					if(effval_tmp > 0xF) {
						mp.paula[i].volume += (effval_tmp >> 4);
						if(mp.paula[i].volume > 0x40) mp.paula[i].volume = 0x40;
					} else {
						mp.paula[i].volume -= (effval_tmp & 0xF);
						if(mp.paula[i].volume < 0x00) mp.paula[i].volume = 0x00;
					}
				}

				break;

			case 0x7:
				if(mp.tick) {
					mp.tremolo[i].phase += mp.tremolo[i].speed;
					_RecalculateWaveform(&mp.tremolo[i]);
				}
				break;

			case 0xE:
				switch(effval_tmp >> 4) {
					case 0x9:
						if(mp.tick && !(mp.tick % (effval_tmp & 0xF))) mp.paula[i].age = mp.paula[i].currentptr = 0;
						break;

					case 0xC:
						if(mp.tick >= (effval_tmp & 0xF)) mp.paula[i].volume = 0;
						break;

					case 0xD:
						if(mp.tick == (effval_tmp & 0xF)) {
							mp.paula[i].age = mp.paula[i].currentptr = 0;
							mp.paula[i].period = mp.note[i];
						}
						break;
				}

				break;
		}

		if(mp.paula[i].period < period_tables[0][3*12-1] && mp.paula[i].period != 0) {
			mp.paula[i].period = period_tables[0][3*12-1];
		}
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

ModPlayerStatus_t *RenderMOD(short *buf, int len) {
	memset(buf, 0, len * 4);

	for(int s = 0; s < len; s++) {
		// Process the tick, if necessary

		if(!mp.audiotick) {
			ProcessMOD();
			mp.audiotick = mp.audiospeed;
		}

		mp.audiotick--;

		// Render the audio

		for(int ch = 0; ch < 4; ch++) {
			if(mp.paula[ch].sample) {
				// Perform linear interpolation on the sample (otherwise it will sound like crap)

				if(!mp.paula[ch].muted) {
					uint32_t nextptr = mp.paula[ch].currentptr + 0x10000;

					if((nextptr >> 17) >= mp.paula[ch].length &&
						mp.paula[ch].looplength != 0)

						nextptr -= mp.paula[ch].looplength << 17;

					int vol = mp.paula[ch].volume + (mp.tremolo[ch].val >> 6);

					if(vol < 0) vol = 0;
					if(vol > 64) vol = 64;

					int sample1 = mp.paula[ch].sample[mp.paula[ch].currentptr >> 16] * vol;
					int sample2 = mp.paula[ch].sample[nextptr >> 16] * vol;

					short sample = (sample1 * (0x10000 - (nextptr & 0xFFFF)) +
						  sample2 * (nextptr & 0xFFFF)) / 0x10000;


                    if (mp.uVolumePercent < 100) {
                        sample = (short)(((int)sample * mp.uVolumePercent) / 100);
                    }

					// short sample = mp.paula[ch].sample[mp.paula[ch].currentptr >> 16] * vol;

					// Distribute the rendered sample across both output channels

					if((ch & 3) == 1 || (ch & 3) == 2) {
						buf[s * 2] += sample / 3;
						buf[s * 2 + 1] += sample;
					} else {
						buf[s * 2] += sample;
						buf[s * 2 + 1] += sample / 3;
					}
				}

				// Advance to the next required sample

				if(mp.paula[ch].period)
					mp.paula[ch].currentptr += (mp.paularate << 16) / (((uint32_t) mp.paula[ch].period) + (mp.vibrato[ch].val >> 7));

				// Stop this channel if we have reached the end or loop it, if desired

				if((mp.paula[ch].currentptr >> 17) >= mp.paula[ch].length) {
					if(mp.paula[ch].looplength == 0) {
						mp.paula[ch].period = 0;
					} else {
						mp.paula[ch].currentptr -= mp.paula[ch].looplength << 17;
					}
				} else {
					if(mp.paula[ch].age < INT32_MAX)
					mp.paula[ch].age++;
				}
			}
		}
	}

	return &mp;
}

ModPlayerStatus_t *InitMOD(uint8_t *mod, int samplerate) {
	if(memcmp(mod + 1080, "M.K.", 4)) {
		return NULL;
	}
	memset(&mp, 0, sizeof(mp));

	mp.samplerate = samplerate;

	mp.paularate = 3546895 / samplerate;

	mp.orders = mod[950];
	mp.ordertable = mod + 952;

	mp.maxpattern = 0;

	for(int i = 0; i < 128; i++) {
		if(mp.ordertable[i] >= mp.maxpattern) mp.maxpattern = mp.ordertable[i];
	}
	mp.maxpattern++;


	// MIK int8_t *samplemem = mod + 1084 + 1024 * mp.maxpattern;
	int8_t *samplemem = (int8_t *)(mod + 1084 + 1024 * mp.maxpattern);



	mp.patterndata = mod + 1084;

	for(int i = 0; i < 31; i++) {
		uint8_t *sample = mod + 20 + i * 30;

		mp.samples[i].length = (sample[22] << 8) | sample[23];
		mp.samples[i].finetune = sample[24];
		mp.samples[i].volume = sample[25];
		mp.samples[i].looppoint = (sample[26] << 8) | sample[27];
		mp.samples[i].actuallength = (sample[28] << 8) | sample[29];

		mp.samples[i].data = samplemem;
		samplemem += mp.samples[i].length * 2;

		mp.samples[i].actuallength += mp.samples[i].looppoint;

		if(mp.samples[i].actuallength < 0x2) {
			mp.samples[i].actuallength = mp.samples[i].length;
			mp.samples[i].looppoint = 0xFFFF;
			mp.samples[i].looplength = 0;
		} else if(mp.samples[i].actuallength > mp.samples[i].length) {
			mp.samples[i].looppoint /= 2;
			mp.samples[i].actuallength -= mp.samples[i].looppoint;
			mp.samples[i].looplength = mp.samples[i].actuallength - mp.samples[i].looppoint;
		} else {
			mp.samples[i].looplength = mp.samples[i].actuallength - mp.samples[i].looppoint;
		}
	}

	for(int pat = 0; pat < mp.maxpattern; pat++) {
		for(int row = 0; row < 64; row++) {
			for(int col = 0; col < 4; col++) {
				uint8_t *cell = mp.patterndata + pat * (64 * 16) + row * 16 + col * 4;

				int period = ((cell[0] & 0x0F) << 8) | cell[1];
				int sample = (cell[0] & 0xF0) | ((cell[2] & 0xF0) >> 4);
				int eff = cell[2] & 0x0F;
				int effval = cell[3];

				if(period == 0) {
					cell[0] = 0;
				} else {
					int note = 0;

					for(int i = 1; i < 36; i++) {
						if(period_tables[0][i] <= period) {
							int low = period - period_tables[0][i];
							int high = period_tables[0][i - 1] - period;

							if(low < high)
								note = i;
							else
								note = i - 1;

							break;
						}
					}

					cell[0] = note + 1;
				}

				cell[1] = sample;
				cell[2] = eff;
				cell[3] = effval;
			}
		}
	}

	mp.maxtick = mp.speed = 6; mp.audiospeed = mp.samplerate / 50;
    mp.uVolumePercent = 100;
	for(int i = 0; i < 4; i++) {
		mp.paula[i].age = INT32_MAX;
	}

	return &mp;
}



void SetModVolume(uint8_t uVolumePercent) {
    if (uVolumePercent <= 100) {
        mp.uVolumePercent = uVolumePercent;
    }

}
