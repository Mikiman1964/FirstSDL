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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "modplay.h"
#include "tinysid.h"

SIDSONG SidSong;
extern AUDIOPLAYER Audioplayer;

static inline int pfloat_ConvertFromInt(int i)
{
    return i<<16;
}
static inline int pfloat_ConvertFromFloat(float f)
{
    return (int)(f*(1<<16));
}
static inline int pfloat_Multiply(int a, int b)
{
    return (a>>8)*(b>>8);
}
static inline int pfloat_ConvertToInt(int i)
{
    return i>>16;
}

enum {
  adc, and, asl, bcc, bcs, beq, bit, bmi, bne, bpl, brk, bvc, bvs, clc,
  cld, cli, clv, cmp, cpx, cpy, dec, dex, dey, eor, inc, inx, iny, jmp,
  jsr, lda, ldx, ldy, lsr, nop, ora, pha, php, pla, plp, rol, ror, rti,
  rts, sbc, sec, sed, sei, sta, stx, sty, tax, tay, tsx, txa, txs, tya,
  xxx};


// SID register definition

struct s6581 {
                struct sidvoice {
                         uint16_t freq;
                         uint16_t pulse;
                         uint8_t wave;
                         uint8_t ad;
                         uint8_t sr;
                       } v[3];
                uint8_t ffreqlo;
                uint8_t ffreqhi;
                uint8_t res_ftv;
                uint8_t ftp_vol;
              };

// internal oscillator def

struct sidosc {
                uint32_t freq;
                uint32_t pulse;
                uint8_t wave;
                uint8_t filter;
                uint32_t attack;
                uint32_t decay;
                uint32_t sustain;
                uint32_t release;
                uint32_t       counter;  // Variablen
                signed int envval;
                uint8_t        envphase;
                uint32_t       noisepos;
                uint32_t       noiseval;
                uint8_t        noiseout;
              };

// internal filter def

struct sidflt {
                int freq;
                uint8_t  l_ena;
                uint8_t  b_ena;
                uint8_t  h_ena;
                uint8_t  v3ena;
                int vol;
                int rez;
                int h;
                int b;
                int l;
               };

// ------------------------------------------------------------- constants

static float attackTimes[16] =
{
  0.0022528606, 0.0080099577, 0.0157696042, 0.0237795619, 0.0372963655,
  0.0550684591,0.0668330845, 0.0783473987, 0.0981219818, 0.244554021,
  0.489108042, 0.782472742, 0.977715461, 2.93364701, 4.88907793, 7.82272493
};
static float decayReleaseTimes[16] =
{
  0.00891777693, 0.024594051, 0.0484185907, 0.0730116639, 0.114512475,
  0.169078356, 0.205199432, 0.240551975, 0.301266125, 0.750858245,
  1.50171551, 2.40243682, 3.00189298, 9.00721405, 15.010998, 24.0182111
};

// --------------------------- pseudo-constants (depending on mixing freq)

static int  mixing_frequency;
static int  freqmul;
static int  filtmul;
static int  attacks [16];
static int  releases[16];

// --------------------------------------------------------------- globals
static struct s6581 sid;
static struct sidosc osc[3];
static struct sidflt filter;

// --------------------------------------------------------- some aux stuff

static inline uint8_t get_bit(uint32_t val, uint8_t b)
{
  return (uint8_t) ((val >> b) & 1);
}

// ------------------------------------------------------------- synthesis

uint8_t c64memory[65536];  // The C64 memory

static int sample_active;
static int sample_position, sample_start, sample_end, sample_repeat_start;
static int fracPos = 0;		/* Fractal position of sample */
static int sample_period;
static int sample_repeats;
static int sample_order;
static int sample_nibble;

static inline int GenerateDigi(int sIn)
{
    // static int last_sample = 0;
    static int sample = 0;

    if (!sample_active) return(sIn);

    if ((sample_position < sample_end) && (sample_position >= sample_start))
    {
		//Interpolation routine
		//float a = (float)fracPos/(float)mixing_frequency;
		//float b = 1-a;
		//sIn += a*sample + b*last_sample;

        sIn += sample;

        fracPos += 985248/sample_period;

        if (fracPos > mixing_frequency)
        {
            fracPos%=mixing_frequency;

            // last_sample = sample;

			// N�hstes Samples holen
            if (sample_order == 0) {
                sample_nibble++;						// Nähstes Sample-Nibble
                if (sample_nibble==2) {
                    sample_nibble = 0;
                    sample_position++;
                }
            }
            else {
                sample_nibble--;
                if (sample_nibble < 0) {
                    sample_nibble=1;
                    sample_position++;
                }
            }
            if (sample_repeats)
            {
                if  (sample_position > sample_end)
                {
                    sample_repeats--;
                    sample_position = sample_repeat_start;
                }
                else sample_active = 0;
            }

            sample = c64memory[sample_position&0xffff];
            if (sample_nibble==1)	// Hi-Nibble holen?
                sample = (sample & 0xf0)>>4;
            else sample = sample & 0x0f;


            sample -= 7;
            sample <<= 10;
        }
    }

    /* Clipping */
    /*
    if (sIn < -32767) return -32767;
    else if (sIn > 32767) return 32767;
    */


    return (sIn);
}


// initialize SID and frequency dependant values

void synth_init   (uint32_t mixfrq)
{
  int i;
  mixing_frequency = mixfrq;
  freqmul = 15872000 / mixfrq;
  filtmul = pfloat_ConvertFromFloat(21.5332031f)/mixfrq;
  for (i=0;i<16;i++) {
    attacks [i]=(int) (0x1000000 / (attackTimes[i]*mixfrq));
    releases[i]=(int) (0x1000000 / (decayReleaseTimes[i]*mixfrq));
  }
  memset(&sid,0,sizeof(sid));
  memset(osc,0,sizeof(osc));
  memset(&filter,0,sizeof(filter));
  osc[0].noiseval = 0xffffff;
  osc[1].noiseval = 0xffffff;
  osc[2].noiseval = 0xffffff;
}

// render a buffer of n samples with the actual register contents
float filterl1=0,filterb1=0,freq=0.8,filterrez=0.1;
#include <math.h>
void synth_render (short *buffer, uint32_t len)
{
  uint8_t v, refosc, outv;
  uint32_t bp;
  uint8_t triout, sawout, plsout, nseout;
  signed short final_sample;
  // step 1: convert the not easily processable sid registers into some
  //         more convenient and fast values (makes the thing much faster
  //         if you process more than 1 sample value at once)
  for (v=0;v<3;v++) {
    osc[v].pulse   = (sid.v[v].pulse & 0xfff) << 16;
    osc[v].filter  = get_bit(sid.res_ftv,v);
    osc[v].attack  = attacks[sid.v[v].ad >> 4];
    osc[v].decay   = releases[sid.v[v].ad & 0xf];
    osc[v].sustain = sid.v[v].sr & 0xf0;
    osc[v].release = releases[sid.v[v].sr & 0xf];
    osc[v].wave    = sid.v[v].wave;
    osc[v].freq    = ((uint32_t)sid.v[v].freq)*freqmul;
  }

#ifdef USE_FILTER
  filter.freq  = (8 * sid.ffreqhi + (sid.ffreqlo&0x7)) * filtmul;

 if (filter.freq>pfloat_ConvertFromInt(1))
     filter.freq=pfloat_ConvertFromInt(1);
  // the above line isnt correct at all - the problem is that the filter
  // works only up to rmxfreq/4 - this is sufficient for 44KHz but isnt
  // for 32KHz and lower - well, but sound quality is bad enough then to
  // neglect the fact that the filter doesnt come that high ;)
  filter.l_ena = get_bit(sid.ftp_vol,4);
  filter.b_ena = get_bit(sid.ftp_vol,5);
  filter.h_ena = get_bit(sid.ftp_vol,6);
  filter.v3ena = !get_bit(sid.ftp_vol,7);
  filter.vol   = (sid.ftp_vol & 0xf);
  //filter.rez   = 1.0-0.04*(float)(sid.res_ftv >> 4);
  filter.rez   = pfloat_ConvertFromFloat(1.2f) -
          pfloat_ConvertFromFloat(0.04f)*(sid.res_ftv >> 4);
  /* We precalculate part of the quick float operation, saves time in loop later */
  filter.rez>>=8;
#endif

  // now render the buffer
  for (bp=0;bp<len;bp++) {
    int outo=0;
    int outf=0;
    // step 2 : generate the two output signals (for filtered and non-
    //          filtered) from the osc/eg sections
    for (v=0;v<3;v++) {
      // update wave counter
      osc[v].counter = (osc[v].counter+osc[v].freq) & 0xFFFFFFF;
      // reset counter / noise generator if reset get_bit set
      if (osc[v].wave & 0x08) {
        osc[v].counter  = 0;
        osc[v].noisepos = 0;
        osc[v].noiseval = 0xffffff;
      }
      refosc = v?v-1:2;  // reference oscillator for sync/ring
      // sync oscillator to refosc if sync bit set
      if (osc[v].wave & 0x02)
        if (osc[refosc].counter < osc[refosc].freq)
          osc[v].counter = osc[refosc].counter * osc[v].freq / osc[refosc].freq;
      // generate waveforms with really simple algorithms
      triout = (uint8_t) (osc[v].counter>>19);
      if (osc[v].counter>>27)
        triout^=0xff;
      sawout = (uint8_t) (osc[v].counter >> 20);
      plsout = (uint8_t) ((osc[v].counter > osc[v].pulse)-1);

      // generate noise waveform exactly as the SID does.
      if (osc[v].noisepos!=(osc[v].counter>>23))
      {
        osc[v].noisepos = osc[v].counter >> 23;
        osc[v].noiseval = (osc[v].noiseval << 1) |
                          (get_bit(osc[v].noiseval,22) ^ get_bit(osc[v].noiseval,17));
        osc[v].noiseout = (get_bit(osc[v].noiseval,22) << 7) |
                          (get_bit(osc[v].noiseval,20) << 6) |
                          (get_bit(osc[v].noiseval,16) << 5) |
                          (get_bit(osc[v].noiseval,13) << 4) |
                          (get_bit(osc[v].noiseval,11) << 3) |
                          (get_bit(osc[v].noiseval, 7) << 2) |
                          (get_bit(osc[v].noiseval, 4) << 1) |
                          (get_bit(osc[v].noiseval, 2) << 0);
      }
      nseout = osc[v].noiseout;

      // modulate triangle wave if ringmod bit set

      if (osc[v].wave & 0x04)
        if (osc[refosc].counter < 0x8000000)
          triout ^= 0xff;

      // now mix the oscillators with an AND operation as stated in
      // the SID's reference manual - even if this is completely wrong.
      // well, at least, the $30 and $70 waveform sounds correct and there's
      // no real solution to do $50 and $60, so who cares.

      outv=0xFF;
      if (osc[v].wave & 0x10) outv &= triout;
      if (osc[v].wave & 0x20) outv &= sawout;
      if (osc[v].wave & 0x40) outv &= plsout;
      if (osc[v].wave & 0x80) outv &= nseout;

      // now process the envelopes. the first thing about this is testing
      // the gate bit and put the EG into attack or release phase if desired
      if (!(osc[v].wave & 0x01)) osc[v].envphase=3;
      else if (osc[v].envphase==3) osc[v].envphase=0;
      // so now process the volume according to the phase and adsr values
      switch (osc[v].envphase) {
        case 0 : {                          // Phase 0 : Attack
                   osc[v].envval+=osc[v].attack;
                   if (osc[v].envval >= 0xFFFFFF)
                   {
                     osc[v].envval   = 0xFFFFFF;
                     osc[v].envphase = 1;
                   }
                   break;
                 }
        case 1 : {                          // Phase 1 : Decay
                   osc[v].envval-=osc[v].decay;
                   if ((signed int) osc[v].envval <= (signed int) (osc[v].sustain<<16))
                   {
                     osc[v].envval   = osc[v].sustain<<16;
                     osc[v].envphase = 2;
                   }
                   break;
                 }
        case 2 : {                          // Phase 2 : Sustain
                   if ((signed int) osc[v].envval != (signed int) (osc[v].sustain<<16))
                   {
                     osc[v].envphase = 1;
                   }
                     // :) yes, thats exactly how the SID works. and maybe
                     // a music routine out there supports this, so better
                     // let it in, thanks :)
                   break;
                 }
        case 3 : {                          // Phase 3 : Release
                   osc[v].envval-=osc[v].release;
                   if (osc[v].envval < 0x40000) osc[v].envval= 0x40000;
                     // the volume offset is because the SID does not
                     // completely silence the voices when it should. most
                     // emulators do so though and thats the main reason
                     // why the sound of emulators is too, err... emulated :)
                   break;
                 }
      }
      // now route the voice output to either the non-filtered or the
      // filtered channel and dont forget to blank out osc3 if desired
#ifdef USE_FILTER
      if (v<2 || filter.v3ena) {
        if (osc[v].filter)
          //outf+=((float)osc[v].envval*(float)outv-0x8000000)/0x30000000;
		  outf+=(((int)(outv-0x80))*osc[v].envval)>>22;

        else
          //outo+=((float)osc[v].envval*(float)outv-0x8000000)/0x30000000;
          outo+=(((int)(outv-0x80))*osc[v].envval)>>22;
      }
#endif
#ifndef USE_FILTER

    outf+=((signed short)(outv-0x80)) * (osc[v].envval>>8);
#endif
    }
    // step 3
    // so, now theres finally time to apply the multi-mode resonant filter
    // to the signal. The easiest thing ist just modelling a real electronic
    // filter circuit instead of fiddling around with complex IIRs or even
    // FIRs ...
    // it sounds as good as them or maybe better and needs only 3 MULs and
    // 4 ADDs for EVERYTHING. SIDPlay uses this kind of filter, too, but
    // Mage messed the whole thing completely up - as the rest of the
    // emulator.
    // This filter sounds a lot like the 8580, as the low-quality, dirty
    // sound of the 6581 is uuh too hard to achieve :)

#ifdef USE_FILTER

    //filter.h = outf - filter.b*filter.rez - filter.l;
	//filter.h = pfloat_ConvertFromInt(outf) - pfloat_Multiply(filter.b, filter.rez) - filter.l;
	filter.h = pfloat_ConvertFromInt(outf) - (filter.b>>8)*filter.rez - filter.l;
	//filter.b += filter.freq * filter.h;
	filter.b += pfloat_Multiply(filter.freq, filter.h);
	//filter.l += filter.freq * filter.b;
        filter.l += pfloat_Multiply(filter.freq, filter.b);

    outf = 0;

    if (filter.l_ena) outf+=pfloat_ConvertToInt(filter.l);
    if (filter.b_ena) outf+=pfloat_ConvertToInt(filter.b);
    if (filter.h_ena) outf+=pfloat_ConvertToInt(filter.h);

    final_sample = (signed short) (filter.vol*(outo+outf));
#endif
#ifndef USE_FILTER

    final_sample = outf>>10;
#endif


    *(buffer+bp)=(signed short) GenerateDigi(final_sample);
  }
}


void sidPoke(int reg, unsigned char val)
{
      int voice=0;

      if ((reg >= 0) && (reg <= 6)) voice=0;
      if ((reg >= 7) && (reg <=13)) {voice=1; reg-=7;}
      if ((reg >= 14) && (reg <=20)) {voice=2; reg-=14;}

      switch (reg) {
        case 0: { // Frequenz niederwertiges byte Stimme 1
				  sid.v[voice].freq = (sid.v[voice].freq&0xff00)+val;
				  //printf("Voice%d: %d\n", voice, sid.v[voice].freq);
                  break;
                }
        case 1: { // Frequenz h�erwertiges byte Stimme 1
			      sid.v[voice].freq = (sid.v[voice].freq&0xff)+(val<<8);
			      break;
		}
		case 2: { // Pulsbreite niederwertiges byte Stimme 1
				  sid.v[voice].pulse = (sid.v[voice].pulse&0xff00)+val;
				  break;
				}
		case 3: { // Pulsbreite h�erwertiges byte Stimme 1
				  sid.v[voice].pulse = (sid.v[voice].pulse&0xff)+(val<<8);
				  break;
				}
		case 4: { sid.v[voice].wave = val; break;}

		case 5: { sid.v[voice].ad = val; break;}
		case 6: { sid.v[voice].sr = val; break;}

		case 21: { sid.ffreqlo = val; break; }
		case 22: { sid.ffreqhi = val; break; }
		case 23: { sid.res_ftv = val; break; }
		case 24: { sid.ftp_vol = val; break;}
	}
  return;
}


uint8_t getmem(uint16_t addr)
{
  if (addr == 0xdd0d) c64memory[addr]=0;
  return c64memory[addr];
}

int internal_period, internal_order, internal_start, internal_end,
internal_add, internal_repeat_times, internal_repeat_start;

void setmem(uint16_t addr, uint8_t value)
{

  c64memory[addr]=value;

  if ((addr&0xfc00)==0xd400)
  {
    //addr&=0x1f;
    sidPoke(addr&0x1f,value);
    // Neue SID-Register
    if ((addr > 0xd418) && (addr < 0xd500))
    {
        // Start-Hi
        if (addr == 0xd41f) internal_start = (internal_start&0x00ff) | (value<<8);
	  // Start-Lo
        if (addr == 0xd41e) internal_start = (internal_start&0xff00) | (value);
	  // Repeat-Hi
        if (addr == 0xd47f) internal_repeat_start = (internal_repeat_start&0x00ff) | (value<<8);
	  // Repeat-Lo
        if (addr == 0xd47e) internal_repeat_start = (internal_repeat_start&0xff00) | (value);

	  // End-Hi
        if (addr == 0xd43e) {
            internal_end = (internal_end&0x00ff) | (value<<8);
        }
	  // End-Lo
        if (addr == 0xd43d) {
            internal_end = (internal_end&0xff00) | (value);
        }
	  // Loop-Size
        if (addr == 0xd43f) internal_repeat_times = value;
	  // Period-Hi
        if (addr == 0xd45e) internal_period = (internal_period&0x00ff) | (value<<8);
	  // Period-Lo
        if (addr == 0xd45d) {
            internal_period = (internal_period&0xff00) | (value);
        }
	  // Sample Order
        if (addr == 0xd47d) internal_order = value;
	  // Sample Add
        if (addr == 0xd45f) internal_add = value;
	  // Start-Sampling
        if (addr == 0xd41d)
        {
            sample_repeats = internal_repeat_times;
            sample_position = internal_start;
            sample_start = internal_start;
            sample_end = internal_end;
            sample_repeat_start = internal_repeat_start;
            sample_period = internal_period;
            sample_order = internal_order;
            switch (value)
            {
                case 0xfd: sample_active = 0; break;
                case 0xfe:
                case 0xff: sample_active = 1; break;

                default: return;
            }

        }
    }

  }

}

//enum { imp, imm, abs, absx, absy, zp, zpx, zpy, ind, indx, indy, acc, rel};
#define imp 0
#define imm 1
#define abs 2
#define absx 3
#define absy 4
#define zp 6
#define zpx 7
#define zpy 8
#define ind 9
#define indx 10
#define indy 11
#define acc 12
#define rel 13

static int opcodes[256]= {
  brk,ora,xxx,xxx,xxx,ora,asl,xxx,php,ora,asl,xxx,xxx,ora,asl,xxx,
  bpl,ora,xxx,xxx,xxx,ora,asl,xxx,clc,ora,xxx,xxx,xxx,ora,asl,xxx,
  jsr,and,xxx,xxx,bit,and,rol,xxx,plp,and,rol,xxx,bit,and,rol,xxx,
  bmi,and,xxx,xxx,xxx,and,rol,xxx,sec,and,xxx,xxx,xxx,and,rol,xxx,
  rti,eor,xxx,xxx,xxx,eor,lsr,xxx,pha,eor,lsr,xxx,jmp,eor,lsr,xxx,
  bvc,eor,xxx,xxx,xxx,eor,lsr,xxx,cli,eor,xxx,xxx,xxx,eor,lsr,xxx,
  rts,adc,xxx,xxx,xxx,adc,ror,xxx,pla,adc,ror,xxx,jmp,adc,ror,xxx,
  bvs,adc,xxx,xxx,xxx,adc,ror,xxx,sei,adc,xxx,xxx,xxx,adc,ror,xxx,
  xxx,sta,xxx,xxx,sty,sta,stx,xxx,dey,xxx,txa,xxx,sty,sta,stx,xxx,
  bcc,sta,xxx,xxx,sty,sta,stx,xxx,tya,sta,txs,xxx,xxx,sta,xxx,xxx,
  ldy,lda,ldx,xxx,ldy,lda,ldx,xxx,tay,lda,tax,xxx,ldy,lda,ldx,xxx,
  bcs,lda,xxx,xxx,ldy,lda,ldx,xxx,clv,lda,tsx,xxx,ldy,lda,ldx,xxx,
  cpy,cmp,xxx,xxx,cpy,cmp,dec,xxx,iny,cmp,dex,xxx,cpy,cmp,dec,xxx,
  bne,cmp,xxx,xxx,xxx,cmp,dec,xxx,cld,cmp,xxx,xxx,xxx,cmp,dec,xxx,
  cpx,sbc,xxx,xxx,cpx,sbc,inc,xxx,inx,sbc,nop,xxx,cpx,sbc,inc,xxx,
  beq,sbc,xxx,xxx,xxx,sbc,inc,xxx,sed,sbc,xxx,xxx,xxx,sbc,inc,xxx
};

static int modes[256]= {
 imp,indx,xxx,xxx,zp,zp,zp,xxx,imp,imm,acc,xxx,abs,abs,abs,xxx,
 rel,indy,xxx,xxx,xxx,zpx,zpx,xxx,imp,absy,xxx,xxx,xxx,absx,absx,xxx,
 abs,indx,xxx,xxx,zp,zp,zp,xxx,imp,imm,acc,xxx,abs,abs,abs,xxx,
 rel,indy,xxx,xxx,xxx,zpx,zpx,xxx,imp,absy,xxx,xxx,xxx,absx,absx,xxx,
 imp,indx,xxx,xxx,zp,zp,zp,xxx,imp,imm,acc,xxx,abs,abs,abs,xxx,
 rel,indy,xxx,xxx,xxx,zpx,zpx,xxx,imp,absy,xxx,xxx,xxx,absx,absx,xxx,
 imp,indx,xxx,xxx,zp,zp,zp,xxx,imp,imm,acc,xxx,ind,abs,abs,xxx,
 rel,indy,xxx,xxx,xxx,zpx,zpx,xxx,imp,absy,xxx,xxx,xxx,absx,absx,xxx,
 imm,indx,xxx,xxx,zp,zp,zp,xxx,imp,imm,acc,xxx,abs,abs,abs,xxx,
 rel,indy,xxx,xxx,zpx,zpx,zpy,xxx,imp,absy,acc,xxx,xxx,absx,absx,xxx,
 imm,indx,imm,xxx,zp,zp,zp,xxx,imp,imm,acc,xxx,abs,abs,abs,xxx,
 rel,indy,xxx,xxx,zpx,zpx,zpy,xxx,imp,absy,acc,xxx,absx,absx,absy,xxx,
 imm,indx,xxx,xxx,zp,zp,zp,xxx,imp,imm,acc,xxx,abs,abs,abs,xxx,
 rel,indy,xxx,xxx,zpx,zpx,zpx,xxx,imp,absy,acc,xxx,xxx,absx,absx,xxx,
 imm,indx,xxx,xxx,zp,zp,zp,xxx,imp,imm,acc,xxx,abs,abs,abs,xxx,
 rel,indy,xxx,xxx,zpx,zpx,zpx,xxx,imp,absy,acc,xxx,xxx,absx,absx,xxx
};

// ----------------------------------------------- globale Faulheitsvariablen

static int cycles;
static uint8_t bval;
static uint16_t wval;

// ----------------------------------------------------------------- Register

static uint8_t a,x,y,s,p;
static uint16_t pc;

// ----------------------------------------------------------- DER HARTE KERN

static uint8_t getaddr(int mode)
{
  uint16_t ad,ad2;
  switch(mode)
  {
    case imp:
      cycles+=2;
      return 0;
    case imm:
      cycles+=2;
      return getmem(pc++);
    case abs:
      cycles+=4;
      ad=getmem(pc++);
      ad|=getmem(pc++)<<8;
      return getmem(ad);
    case absx:
      cycles+=4;
      ad=getmem(pc++);
      ad|=256*getmem(pc++);
      ad2=ad+x;
      if ((ad2&0xff00)!=(ad&0xff00))
        cycles++;
      return getmem(ad2);
    case absy:
      cycles+=4;
      ad=getmem(pc++);
      ad|=256*getmem(pc++);
      ad2=ad+y;
      if ((ad2&0xff00)!=(ad&0xff00))
        cycles++;
      return getmem(ad2);
    case zp:
      cycles+=3;
      ad=getmem(pc++);
      return getmem(ad);
    case zpx:
      cycles+=4;
      ad=getmem(pc++);
      ad+=x;
      return getmem(ad&0xff);
    case zpy:
      cycles+=4;
      ad=getmem(pc++);
      ad+=y;
      return getmem(ad&0xff);
    case indx:
      cycles+=6;
      ad=getmem(pc++);
      ad+=x;
      ad2=getmem(ad&0xff);
      ad++;
      ad2|=getmem(ad&0xff)<<8;
      return getmem(ad2);
    case indy:
      cycles+=5;
      ad=getmem(pc++);
      ad2=getmem(ad);
      ad2|=getmem((ad+1)&0xff)<<8;
      ad=ad2+y;
      if ((ad2&0xff00)!=(ad&0xff00))
        cycles++;
      return getmem(ad);
    case acc:
      cycles+=2;
      return a;
  }
  return 0;
}


static void setaddr(int mode, uint8_t val)
{
  uint16_t ad,ad2;
  switch(mode)
  {
    case abs:
      cycles+=2;
      ad=getmem(pc-2);
      ad|=256*getmem(pc-1);
      setmem(ad,val);
      return;
    case absx:
      cycles+=3;
      ad=getmem(pc-2);
      ad|=256*getmem(pc-1);
      ad2=ad+x;
      if ((ad2&0xff00)!=(ad&0xff00))
        cycles--;
      setmem(ad2,val);
      return;
    case zp:
      cycles+=2;
      ad=getmem(pc-1);
      setmem(ad,val);
      return;
    case zpx:
      cycles+=2;
      ad=getmem(pc-1);
      ad+=x;
      setmem(ad&0xff,val);
      return;
    case acc:
      a=val;
      return;
  }
}


static void putaddr(int mode, uint8_t val)
{
  uint16_t ad,ad2;
  switch(mode)
  {
    case abs:
      cycles+=4;
      ad=getmem(pc++);
      ad|=getmem(pc++)<<8;
      setmem(ad,val);
      return;
    case absx:
      cycles+=4;
      ad=getmem(pc++);
      ad|=getmem(pc++)<<8;
      ad2=ad+x;
      setmem(ad2,val);
      return;
    case absy:
      cycles+=4;
      ad=getmem(pc++);
      ad|=getmem(pc++)<<8;
      ad2=ad+y;
      if ((ad2&0xff00)!=(ad&0xff00))
        cycles++;
      setmem(ad2,val);
      return;
    case zp:
      cycles+=3;
      ad=getmem(pc++);
      setmem(ad,val);
      return;
    case zpx:
      cycles+=4;
      ad=getmem(pc++);
      ad+=x;
      setmem(ad&0xff,val);
      return;
    case zpy:
      cycles+=4;
      ad=getmem(pc++);
      ad+=y;
      setmem(ad&0xff,val);
      return;
    case indx:
      cycles+=6;
      ad=getmem(pc++);
      ad+=x;
      ad2=getmem(ad&0xff);
      ad++;
      ad2|=getmem(ad&0xff)<<8;
      setmem(ad2,val);
      return;
    case indy:
      cycles+=5;
      ad=getmem(pc++);
      ad2=getmem(ad);
      ad2|=getmem((ad+1)&0xff)<<8;
      ad=ad2+y;
      setmem(ad,val);
      return;
    case acc:
      cycles+=2;
      a=val;
      return;
  }
}


static inline void setflags(int flag, int cond)
{
  // cond?p|=flag:p&=~flag;
  if (cond) p|=flag;
  else p&=~flag;
}


static inline void push(uint8_t val)
{
  setmem(0x100+s,val);
  if (s) s--;
}

static inline uint8_t pop()
{
  if (s<0xff) s++;
  return getmem(0x100+s);
}

static void branch(int flag)
{
  signed char dist;
  dist=(signed char)getaddr(imm);
  wval=pc+dist;
  if (flag)
  {
    cycles+=((pc&0x100)!=(wval&0x100))?2:1;
    pc=wval;
  }
}

// ----------------------------------------------------- öffentliche Routinen

void cpuReset()
{
  a=x=y=0;
  p=0;
  s=255;
  pc=getaddr(0xfffc);
}

void cpuResetTo(uint16_t npc)
{
  a=0;
  x=0;
  y=0;
  p=0;
  s=255;
  pc=npc;
}
int cpuParse()
{
  uint8_t opc;
  int cmd, addr, c;
  cycles=0;

  opc=getmem(pc++);
  cmd=opcodes[opc];
  addr=modes[opc];

  switch (cmd)
  {
    case adc:
      wval=(uint16_t)a+getaddr(addr)+((p&FLAG_C)?1:0);
      setflags(FLAG_C, wval&0x100);
      a=(uint8_t)wval;
      setflags(FLAG_Z, !a);
      setflags(FLAG_N, a&0x80);
      setflags(FLAG_V, (!!(p&FLAG_C)) ^ (!!(p&FLAG_N)));
      break;
    case and:
      bval=getaddr(addr);
      a&=bval;
      setflags(FLAG_Z, !a);
      setflags(FLAG_N, a&0x80);
      break;
    case asl:
      wval=getaddr(addr);
      wval<<=1;
      setaddr(addr,(uint8_t)wval);
      setflags(FLAG_Z,!wval);
      setflags(FLAG_N,wval&0x80);
      setflags(FLAG_C,wval&0x100);
      break;
    case bcc:
      branch(!(p&FLAG_C));
      break;
    case bcs:
      branch(p&FLAG_C);
      break;
    case bne:
      branch(!(p&FLAG_Z));
      break;
    case beq:
      branch(p&FLAG_Z);
      break;
    case bpl:
      branch(!(p&FLAG_N));
      break;
    case bmi:
      branch(p&FLAG_N);
      break;
    case bvc:
      branch(!(p&FLAG_V));
      break;
    case bvs:
      branch(p&FLAG_V);
      break;
    case bit:
      bval=getaddr(addr);
      setflags(FLAG_Z,!(a&bval));
      setflags(FLAG_N,bval&0x80);
      setflags(FLAG_V,bval&0x40);
      break;
    case brk:
      push(pc&0xff);
      push(pc>>8);
      push(p);
      setflags(FLAG_B,1);
      pc=getmem(0xfffe);
      cycles+=7;
      break;
    case clc:
      cycles+=2;
      setflags(FLAG_C,0);
      break;
    case cld:
      cycles+=2;
      setflags(FLAG_D,0);
      break;
    case cli:
      cycles+=2;
      setflags(FLAG_I,0);
      break;
    case clv:
      cycles+=2;
      setflags(FLAG_V,0);
      break;
    case cmp:
      bval=getaddr(addr);
      wval=(uint16_t)a-bval;
      setflags(FLAG_Z,!wval);
      setflags(FLAG_N,wval&0x80);
      setflags(FLAG_C,a>=bval);
      break;
    case cpx:
      bval=getaddr(addr);
      wval=(uint16_t)x-bval;
      setflags(FLAG_Z,!wval);
      setflags(FLAG_N,wval&0x80);
      setflags(FLAG_C,a>=bval);
      break;
    case cpy:
      bval=getaddr(addr);
      wval=(uint16_t)y-bval;
      setflags(FLAG_Z,!wval);
      setflags(FLAG_N,wval&0x80);
      setflags(FLAG_C,a>=bval);
      break;
    case dec:
      bval=getaddr(addr);
      bval--;
      setaddr(addr,bval);
      setflags(FLAG_Z,!bval);
      setflags(FLAG_N,bval&0x80);
      break;
    case dex:
      cycles+=2;
      x--;
      setflags(FLAG_Z,!x);
      setflags(FLAG_N,x&0x80);
      break;
    case dey:
      cycles+=2;
      y--;
      setflags(FLAG_Z,!y);
      setflags(FLAG_N,y&0x80);
      break;
    case eor:
      bval=getaddr(addr);
      a^=bval;
      setflags(FLAG_Z,!a);
      setflags(FLAG_N,a&0x80);
      break;
    case inc:
      bval=getaddr(addr);
      bval++;
      setaddr(addr,bval);
      setflags(FLAG_Z,!bval);
      setflags(FLAG_N,bval&0x80);
      break;
    case inx:
      cycles+=2;
      x++;
      setflags(FLAG_Z,!x);
      setflags(FLAG_N,x&0x80);
      break;
    case iny:
      cycles+=2;
      y++;
      setflags(FLAG_Z,!y);
      setflags(FLAG_N,y&0x80);
      break;
    case jmp:
      cycles+=3;
      wval=getmem(pc++);
      wval|=256*getmem(pc++);
      switch (addr)
      {
        case abs:
          pc=wval;
          break;
        case ind:
          pc=getmem(wval);
          pc|=256*getmem(wval+1);
          cycles+=2;
          break;
      }
      break;
    case jsr:
      cycles+=6;
      push((pc+2));
      push((pc+2)>>8);
      wval=getmem(pc++);
      wval|=256*getmem(pc++);
      pc=wval;
      break;
    case lda:
      a=getaddr(addr);
      setflags(FLAG_Z,!a);
      setflags(FLAG_N,a&0x80);
      break;
    case ldx:
      x=getaddr(addr);
      setflags(FLAG_Z,!x);
      setflags(FLAG_N,x&0x80);
      break;
    case ldy:
      y=getaddr(addr);
      setflags(FLAG_Z,!y);
      setflags(FLAG_N,y&0x80);
      break;
    case lsr:
      //bval=wval=getaddr(addr);
	  bval=getaddr(addr); wval=(uint8_t)bval;
      wval>>=1;
      setaddr(addr,(uint8_t)wval);
      setflags(FLAG_Z,!wval);
      setflags(FLAG_N,wval&0x80);
      setflags(FLAG_C,bval&1);
      break;
    case nop:
      cycles+=2;
      break;
    case ora:
      bval=getaddr(addr);
      a|=bval;
      setflags(FLAG_Z,!a);
      setflags(FLAG_N,a&0x80);
      break;
    case pha:
      push(a);
      cycles+=3;
      break;
    case php:
      push(p);
      cycles+=3;
      break;
    case pla:
      a=pop();
      setflags(FLAG_Z,!a);
      setflags(FLAG_N,a&0x80);
      cycles+=4;
      break;
    case plp:
      p=pop();
      cycles+=4;
      break;
    case rol:
      bval=getaddr(addr);
      c=!!(p&FLAG_C);
      setflags(FLAG_C,bval&0x80);
      bval<<=1;
      bval|=c;
      setaddr(addr,bval);
      setflags(FLAG_N,bval&0x80);
      setflags(FLAG_Z,!bval);
      break;
    case ror:
      bval=getaddr(addr);
      c=!!(p&FLAG_C);
      setflags(FLAG_C,bval&1);
      bval>>=1;
      bval|=128*c;
      setaddr(addr,bval);
      setflags(FLAG_N,bval&0x80);
      setflags(FLAG_Z,!bval);
      break;
    case rti:
		// NEU, rti wie rts, au�r das alle register wieder vom stack kommen
      //p=pop();
		p=pop();
		y=pop();
		x=pop();
		a=pop();
		// in_nmi = false;
		//write_console("NMI EXIT!");
    case rts:
      wval=256*pop();
      wval|=pop();
      pc=wval;
      cycles+=6;
      break;
    case sbc:
      bval=getaddr(addr)^0xff;
      wval=(uint16_t)a+bval+((p&FLAG_C)?1:0);
      setflags(FLAG_C, wval&0x100);
      a=(uint8_t)wval;
      setflags(FLAG_Z, !a);
      setflags(FLAG_N, a>127);
      setflags(FLAG_V, (!!(p&FLAG_C)) ^ (!!(p&FLAG_N)));
      break;
    case sec:
      cycles+=2;
      setflags(FLAG_C,1);
      break;
    case sed:
      cycles+=2;
      setflags(FLAG_D,1);
      break;
    case sei:
      cycles+=2;
      setflags(FLAG_I,1);
      break;
    case sta:
      putaddr(addr,a);
      break;
    case stx:
      putaddr(addr,x);
      break;
    case sty:
      putaddr(addr,y);
      break;
    case tax:
      cycles+=2;
      x=a;
      setflags(FLAG_Z, !x);
      setflags(FLAG_N, x&0x80);
      break;
    case tay:
      cycles+=2;
      y=a;
      setflags(FLAG_Z, !y);
      setflags(FLAG_N, y&0x80);
      break;
    case tsx:
      cycles+=2;
      x=s;
      setflags(FLAG_Z, !x);
      setflags(FLAG_N, x&0x80);
      break;
    case txa:
      cycles+=2;
      a=x;
      setflags(FLAG_Z, !a);
      setflags(FLAG_N, a&0x80);
      break;
    case txs:
      cycles+=2;
      s=x;
      break;
    case tya:
      cycles+=2;
      a=y;
      setflags(FLAG_Z, !a);
      setflags(FLAG_N, a&0x80);
      break;
  }

  return cycles;
}

int cpuJSR(uint16_t npc, uint8_t na)
{
  int ccl;

  a=na;
  x=0;
  y=0;
  p=0;
  s=255;
  pc=npc;
  push(0);
  push(0);
  ccl=0;
  while (pc)
  {
    ccl+=cpuParse();
  }
  return ccl;
}


void c64Init(void) {
    synth_init(44100);
    memset(c64memory,0,sizeof(c64memory));
    cpuReset();
}


/*----------------------------------------------------------------------------
Name:           InitSidSong
------------------------------------------------------------------------------
Beschreibung: Initialisiert die Struktur SidSong.x durch ein angegebenen
              Sid-Song.
Parameter
      Eingang: pSidSong, uint8_t *, Zeiger auf Sid-Song
               uLen, uint16_t, Länge des Sid-Songs in Bytes
Rückgabewert:  int, 0 = OK, sonst Fehler
Seiteneffekte: SidSong.x
------------------------------------------------------------------------------*/
int InitSidSong(uint8_t *pSidSong, uint16_t uLen) {
    int nErrorCode;
    uint8_t data_file_offset;

    if ((pSidSong != NULL) && (uLen > 0)) {
        memset(&SidSong,0,sizeof(SidSong));
        memcpy(SidSong.uData,pSidSong,uLen);
        memcpy(SidSong.szName,SidSong.uData + 0x16,32);
        memcpy(SidSong.szAuthor,SidSong.uData + 0x36,32);
        memcpy(SidSong.szCopyright,SidSong.uData + 0x56,32);
        SidSong.uLen = uLen;
        data_file_offset = SidSong.uData[7];
        SidSong.uLoadAddr = (SidSong.uData[8] << 8) | SidSong.uData[9];
        SidSong.uInitAddr = (SidSong.uData[10] << 8) | SidSong.uData[11];
        SidSong.uPlayAddr = (SidSong.uData[12] << 8) | SidSong.uData[13];
        SidSong.uLoadAddr = (SidSong.uData[data_file_offset + 1] << 8) | SidSong.uData[data_file_offset];
        SidSong.uMaxSubsong = SidSong.uData[0x0F] - 1;  // subsongs
        SidSong.uActualSubsong = SidSong.uData[0x11] - 1;  // startsong
        SidSong.uPlaySpeed = SidSong.uData[0x15];
        memset(c64memory,0,sizeof(c64memory));
        memcpy(&c64memory[SidSong.uLoadAddr],&SidSong.uData[data_file_offset + 2],SidSong.uLen - (data_file_offset + 2));
        if (SidSong.uPlayAddr == 0) {
            cpuJSR(SidSong.uInitAddr,0);
            SidSong.uPlayAddr = (c64memory[0x0315] << 8) + c64memory[0x0314];
        }
        nErrorCode = 0;
    } else {
        nErrorCode = -1;
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           ShowSidSong
------------------------------------------------------------------------------
Beschreibung: Zeigt Werte der Struktur SidSong.x an.

Parameter:     -
Rückgabewert:  -
Seiteneffekte: SidSong.x
------------------------------------------------------------------------------*/
void ShowSidSong() {
    if (SidSong.uLen > 0) {
        printf("title    :      %s\n",SidSong.szName);
        printf("author   :      %s\n",SidSong.szAuthor);
        printf("copyright:      %s\n",SidSong.szCopyright);
        printf("load addr:      %x\n",SidSong.uLoadAddr);
        printf("init addr:      %x\n",SidSong.uInitAddr);
        printf("play addr:      %x\n",SidSong.uPlayAddr);
        printf("actual subsong: %d\n",SidSong.uActualSubsong);
        printf("max subsong:    %d\n",SidSong.uMaxSubsong);
        printf("play speed:     %d\n",SidSong.uPlaySpeed);
        printf("sid song len:   %d\n",SidSong.uLen);
    } else {
        printf("ERROR\n");
    }
}


/*----------------------------------------------------------------------------
Name:           RenderSid
------------------------------------------------------------------------------
Beschreibung: Erzeugt Audiodaten für ein Sid-Song und legt diese im short-Puffer
              Audiobuf ab.
              Es werden minimal 882 * 2 short-Werte im Puffer abgelegt.

Eingang:      uSidBlocks, uint32_t, Anzahl von SID-Blöcken a 882 * 2 shorts
                1000 Blöcke ca. 2:40 min Spielzeit
Ausgang:      pAudiobuf, short *, Zeiger auf Audiopuffer, mindest 882 * 2 shorts

Rückgabewert: uint32_t, Anzahl Samples im Audiopuffer
Seiteneffekte: SidSong.x, Audioplayer.x
------------------------------------------------------------------------------*/
uint32_t RenderSid(short *pAudiobuf,uint32_t uSidBlocks) {
    uint32_t uBlock;
    uint32_t k;
    short localAudiobuf[882]; // Monoblock
    short sample;

    for (uBlock = 0; uBlock < uSidBlocks; uBlock++) {
        if (SidSong.uPlaySpeed == 0) { // Single Speed (50Hz)
            cpuJSR(SidSong.uPlayAddr,0);
            synth_render(localAudiobuf,882);
        } else if (SidSong.uPlaySpeed == 1) {// Double Speed (100Hz)
			cpuJSR(SidSong.uPlayAddr,0);
			synth_render(localAudiobuf,441);
			cpuJSR(SidSong.uPlayAddr,0);
			synth_render(&localAudiobuf[441],441);
        }
        // Monoblock als Stereoblock in Audiobuf kopieren
        for (k = 0; k < 882; k++) {
            if (Audioplayer.uMusicVolumePercent < 100) {
                sample = (short)(((int)localAudiobuf[k] * Audioplayer.uMusicVolumePercent) / 100);
            } else {
                sample = localAudiobuf[k];
            }
            pAudiobuf[uBlock * 882 * 2 + k * 2] = sample;
            pAudiobuf[uBlock * 882 * 2 + k * 2 + 1] = sample;
        }
    }
	return uSidBlocks * 882 * 2;    // * 2 wegen "Stereo"
}


