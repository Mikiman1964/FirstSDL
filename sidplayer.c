/*
 *  sid.cpp - 6581 SID emulation
 *
 *  Frodo (C) Copyright 1994-2004 Christian Bauer
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "musicplayer.h"
#include "sidplayer.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

uint8_t dummy; // compiler warning

SIDINFO Sidinfo;

// Structure for one voice
typedef struct voice_t voice_t;
struct voice_t {
    int32_t wave;           // Selected waveform
    int32_t eg_state;       // Current state of EG
    voice_t *mod_by;        // Voice that modulates this one
    voice_t *mod_to;        // Voice that is modulated by this one

    uint32_t count;         // Counter for waveform generator, 8.16 fixed
    uint32_t add;           // Added to counter in every sample frame

    uint16_t freq;          // SID frequency value
    uint16_t pw;            // SID pulse-width value

    uint32_t a_add;         // EG parameters
    uint32_t d_sub;
    uint32_t s_level;
    uint32_t r_sub;
    uint32_t eg_level;      // Current EG level, 8.16 fixed

    uint32_t noise;         // Last noise generator output value

    uint16_t left_gain;     // Gain on left channel (12.4 fixed)
    uint16_t right_gain;    // Gain on right channel (12.4 fixed)

    bool gate;              // EG gate bit
    bool ring;              // Ring modulation bit
    bool test;              // Test bit
    bool filter;            // Flag: Voice filtered

                            // The following bit is set for the modulating
                            // voice, not for the modulated one (as the SID bits)
    bool sync;              // Sync modulation bit
    bool mute;              // Voice muted (voice 3 only)
};

// Data structures for both SIDs
typedef struct osid_t osid_t;
struct osid_t {
    int32_t sid_num;                        // SID number (0 or 1)

    voice_t voice[3];                       // Data for 3 voices

    uint8_t regs[128];                      // Copies of the 25 write-only SID registers (SIDPlayer uses 128 registers)
    uint8_t last_written_byte;              // Byte last written to SID (for emulation of read accesses to write-only registers)
    uint8_t volume;                         // Master volume (0..15)

    uint8_t f_type;                         // Filter type
    uint8_t f_freq;                         // SID filter frequency (upper 8 bits)
    uint8_t f_res;                          // Filter resonance (0..15)

    float f_ampl;                           // IIR filter input attenuation
    float d1, d2, g1, g2;                   // IIR filter coefficients
    float xn1_l, xn2_l, yn1_l, yn2_l;       // IIR filter previous input/output signal (left and right channel)
    float xn1_r, xn2_r, yn1_r, yn2_r;

    uint16_t v4_left_gain;                  // Gain of voice 4 on left channel (12.4 fixed)
    uint16_t v4_right_gain;                 // Gain of voice 4 on right channel (12.4 fixed)

    int32_t v4_state;                       // State of voice 4 (Galway noise/samples)
    uint32_t v4_count;                      // Counter for voice 4
    uint32_t v4_add;                        // Added to counter in every frame

    uint16_t gn_adr;                        // C64 address of tone list
    uint16_t gn_tone_length;                // Length of each tone in samples
    uint32_t gn_volume_add;                 // Added to SID volume reg. for every sample
    int32_t  gn_tone_counter;               // Number of tones in list
    uint16_t gn_base_cycles;                // Cycles before sample
    uint16_t gn_loop_cycles;                // Cycles between samples
    uint32_t gn_last_count;                 // Value of v4_count (lower 16 bits cleared) at end of tone

    uint32_t sm_adr;                        // C64 nybble address of sample
    uint32_t sm_end_adr;                    // C64 nybble address of end of sample
    uint32_t sm_rep_adr;                    // C64 nybble address of sample repeat point
    uint16_t sm_volume;                     // Sample volume (0..2, 0=loudest)
    uint8_t sm_rep_count;                   // Sample repeat counter (0xff=continous)
    bool sm_big_endian;                     // Flag: Sample is big-endian
};

osid_t *sid1, *sid2;
osid_t sid01,sid02;


// Global variables & constants
uint8_t c64_ram[C64_RAM_SIZE];  // C64-Memory area
uint32_t f_rand_seed;   // Fast pseudo-random number generator
uint32_t noise_rand_seed;
bool enable_filters;        // Flag: emulate SID filters
bool dual_sid;              // Flag: emulate 2 SID chips
bool emulate_8580;          // Flag: emulate new SID chip (8580)
int32_t audio_effect;       // Audio effect type (0 = none, 1 = reverb, 2 = spatial)
int32_t master_volume;      // Master volume (0..0x100)
int32_t dual_sep;           // Dual-SID stereo separation (0..0x100)

// Volumes (0..0x100) and panning (-0x100..0x100) of voices 1..4 (both SIDs)
int32_t v1_volume, v2_volume, v3_volume, v4_volume;
int32_t v1_panning, v2_panning, v3_panning, v4_panning;


// Number of SID clocks per sample frame
uint32_t sid_cycles;        // Integer
float sid_cycles_frac;      // With fractional part

// Phi2 clock frequency
uint32_t cycles_per_second;

// Replay counter variables
uint16_t cia_timer;         // CIA timer A latch
int32_t replay_count;       // Counter for timing replay routine
int32_t speed_adjust;       // Speed adjustment in percent

// Prototypes
void SIDClockFreqChanged(void);     // Clock frequency changed
void Sidplayer_CPUInit(void);
void CPUExecute(uint16_t startadr, uint8_t init_ra, uint8_t init_rx, uint8_t init_ry, uint32_t max_cycles);
void SIDReset(uint32_t now);
void Sidplayer_InitAll(void);
void SIDInit(); // Init SID emulation
// Set replay frequency and speed adjustment
void SIDSetReplayFreq(int32_t freq);
void SIDAdjustSpeed(int32_t percent);
// Write to CIA timer A (changes replay frequency)
void cia_tl_write(uint8_t byte);
void cia_th_write(uint8_t byte);
// Read from SID register
uint32_t sid_read(uint32_t adr, uint32_t now);
// Write to SID register
void sid_write(uint32_t adr, uint32_t byte, uint32_t now, bool rmw);
// Memory access function prototypes
uint32_t ram_read(uint32_t adr, uint32_t now);
void ram_write(uint32_t adr, uint32_t byte, uint32_t now, bool rmw);
void cia_write(uint32_t adr, uint32_t byte, uint32_t now, bool rmw);
// Sid / Filter prototypes
void osid_reset(osid_t *sid);
uint32_t osid_read(osid_t *sid, uint32_t adr, uint32_t now);
void osid_write(osid_t *sid, uint32_t adr, uint32_t byte, uint32_t now, bool rmw);
void osid_calc_gains(osid_t *sid, bool is_left_sid, bool is_right_sid);
void osid_calc_filter(osid_t *sid);
void osid_chunk_read(osid_t *sid, size_t size);
void osid_chunk_write(osid_t *sid);
void osid_calc_gain_voice(int32_t volume, int32_t panning, uint16_t *left_gain, uint16_t *right_gain);


// Zeigt Sidinfo-Struktur
void ShowSidSong() {
    if (Sidinfo.bOK) {
        printf("data len:              %u\r\n",Sidinfo.uDataLen);
        printf("module name:           %s\r\n",Sidinfo.szModuleName);
        printf("author:                %s\r\n",Sidinfo.szAuthor);
        printf("copyright:             %s\r\n",Sidinfo.szCopyright);
        printf("load address:          0x%x\r\n",Sidinfo.uLoadAddr);
        printf("init address:          0x%x\r\n",Sidinfo.uInitAddr);
        printf("play address:          0x%x\r\n",Sidinfo.uPlayAddr);
        printf("current song:          %u\r\n",Sidinfo.uCurrentSong);
        printf("number of songs:       %u\r\n",Sidinfo.uNumberOfSongs);
        printf("speed flags:           %u\r\n",Sidinfo.speed_flags);
        printf("play_adr_from_irq_vec: %u\r\n",Sidinfo.play_adr_from_irq_vec);
    } else {
        printf("%s: ERROR\r\n",__FUNCTION__);
    }
}


// Read 16-bit quantity from PSID header
uint16_t read_psid_16(const uint8_t *p, int32_t offset)
{
    return (p[offset] << 8) | p[offset + 1];
}


// Read 32-bit quantity from PSID header
uint32_t read_psid_32(const uint8_t *p, int32_t offset)
{
    return (p[offset] << 24) | (p[offset + 1] << 16) | (p[offset + 2] << 8) | p[offset + 3];
}


uint8_t f_rand()
{
    f_rand_seed = f_rand_seed * 1103515245 + 12345;
    return f_rand_seed >> 16;
}


void Sidplayer_MemoryInit()
{
    memset(c64_ram, 0, C64_RAM_SIZE - 0x2000);
    memset(c64_ram + 0xe000, 0x40, 0x2000);        // Fill kernal ROM area with RTI
    c64_ram[1] = 7;                                // 6510 I/O port
    c64_ram[0x0314] = 0x31;                        // IRQ vector
    c64_ram[0x0315] = 0xea;
}


void Sidplayer_InitAll(void)
{
    Sidplayer_MemoryInit();
    SIDInit();
    Sidplayer_CPUInit();
}


// Check for PSID header
bool IsPSIDHeader(const uint8_t *p)
{
    // Check signature and version
    uint32_t id = read_psid_32(p, PSID_ID);
    uint16_t version = read_psid_16(p, PSID_VERSION);
    return id == 0x50534944 && (version == 1 || version == 2);
}


// Load PSID file for playing
bool LoadPSIDFile(uint8_t *pMusicdata, int32_t len)
{
    uint8_t header[PSID_MAX_HEADER_LENGTH];
    uint8_t *pRead;

    if ((pMusicdata == NULL) || (len < PSID_MAX_HEADER_LENGTH)) {
        return false;
    }

    pRead = pMusicdata;
    // Clear C64 RAM
    Sidplayer_InitAll();

    // Load and check header
    memcpy(header,pMusicdata,PSID_MAX_HEADER_LENGTH);

    if (!IsPSIDHeader(header)) {
        return false;
    }

    Sidinfo.uDataLen = len;
    // Extract data from header
    Sidinfo.uNumberOfSongs = read_psid_16(header, PSID_NUMBER);
    if (Sidinfo.uNumberOfSongs == 0)
        Sidinfo.uNumberOfSongs = 1;
    Sidinfo.uCurrentSong = read_psid_16(header, PSID_DEFSONG);
    if (Sidinfo.uCurrentSong > 0)
        Sidinfo.uCurrentSong--;
    if (Sidinfo.uCurrentSong >= Sidinfo.uNumberOfSongs)
        Sidinfo.uCurrentSong = 0;

    Sidinfo.uInitAddr = read_psid_16(header, PSID_INIT);
    Sidinfo.uPlayAddr = read_psid_16(header, PSID_MAIN);
    Sidinfo.play_adr_from_irq_vec = (Sidinfo.uPlayAddr == 0);
    Sidinfo.speed_flags = read_psid_32(header, PSID_SPEED);

    memcpy(Sidinfo.szModuleName, (uint8_t *)(header + PSID_NAME), 32);
    memcpy(Sidinfo.szAuthor, (uint8_t *)(header + PSID_AUTHOR), 32);
    memcpy(Sidinfo.szCopyright, (uint8_t *)(header + PSID_COPYRIGHT), 32);

    // Seek to start of module data
    //fseek(f, read_psid_16(header, PSID_HEADERLEN), SEEK_SET);
    pRead = pRead + read_psid_16(header, PSID_HEADERLEN);

    // Find load address
    Sidinfo.uLoadAddr = read_psid_16(header, PSID_START);

    if (Sidinfo.uLoadAddr == 0) {    // Load address is at start of module data
        uint8_t lo = *pRead;
        pRead++;
        uint8_t hi = *pRead;
        pRead++;
        Sidinfo.uLoadAddr = (hi << 8) | lo;
    }

    if (Sidinfo.uInitAddr == 0)        // Init routine address is equal to load address
        Sidinfo.uInitAddr = Sidinfo.uLoadAddr;

    // Load module data to C64 RAM
    // fread(c64_ram + Sidinfo.uLoadAddr, 1, C64_RAM_SIZE - Sidinfo.uLoadAddr, f);
    memcpy(c64_ram + Sidinfo.uLoadAddr,pRead,len - (uint32_t)(pRead - pMusicdata));

    // Select default song
    SelectSong(Sidinfo.uCurrentSong);

    // Everything OK
    Sidinfo.bOK = true;
    return true;
}


// Select song for playing
void SelectSong(int32_t num)
{
    if (num >= Sidinfo.uNumberOfSongs)
        num = 0;
    Sidinfo.uCurrentSong = num;

    // Reset SID
    SIDReset(0);

    // Set replay frequency
    int32_t freq = 50;
    if (num < 32)
        freq = Sidinfo.speed_flags & (1 << num) ? 60 : 50;
    SIDSetReplayFreq(freq);

    // Execute init routine
    CPUExecute(Sidinfo.uInitAddr, Sidinfo.uCurrentSong, 0, 0, 1000000);
}


// Update play_adr from IRQ vector if necessary
void UpdatePlayAdr()
{
    if (Sidinfo.play_adr_from_irq_vec) {
        if (c64_ram[1] & 2)        // Kernal ROM switched in
            Sidinfo.uPlayAddr = (c64_ram[0x0315] << 8) | c64_ram[0x0314];
        else                // Kernal ROM switched out
            Sidinfo.uPlayAddr = (c64_ram[0xffff] << 8) | c64_ram[0xfffe];
    }
}


// CPU
// Memory access functions
typedef uint32_t (*mem_read_func)(uint32_t, uint32_t);
typedef void (*mem_write_func)(uint32_t, uint32_t, uint32_t, bool);

mem_read_func mem_read_table[256];       // Table of read/write functions for 256 pages
mem_write_func mem_write_table[256];




/*
 *  Init CPU emulation
 */

void set_memory_funcs(uint16_t from, uint16_t to, mem_read_func r, mem_write_func w)
{
    int32_t page;
    for (page = (from >> 8); page <= (to >> 8); page++) {
        mem_read_table[page] = r;
        mem_write_table[page] = w;
    }
}

void Sidplayer_CPUInit()
{
    // Set up memory access tables
    set_memory_funcs(0x0000, 0xffff, ram_read, ram_write);
    set_memory_funcs(0xd400, 0xd7ff, sid_read, sid_write);
    set_memory_funcs(0xdc00, 0xdcff, ram_read, cia_write);
}



/*
 *  Memory access functions
 */

uint32_t ram_read(uint32_t adr, uint32_t now)
{
    return c64_ram[adr];
}

void ram_write(uint32_t adr, uint32_t byte, uint32_t now, bool rmw)
{
    c64_ram[adr] = byte;
}

void cia_write(uint32_t adr, uint32_t byte, uint32_t now, bool rmw)
{
    if (adr == 0xdc04)
        cia_tl_write(byte);
    else if (adr == 0xdc05)
        cia_th_write(byte);
    else
        c64_ram[adr] = byte;
}


/*
 *  CPU emulation loop
 */

void CPUExecute(uint16_t startadr, uint8_t init_ra, uint8_t init_rx, uint8_t init_ry, uint32_t max_cycles)
{
    // 6510 registers
    register uint8_t a = init_ra, x = init_rx, y = init_ry;
    register uint8_t n_flag = 0, z_flag = 0;
    uint8_t sp = 0xff, pflags = 0;

    // Program counter
    register uint8_t *pc;

    // Temporary address storage
    uint16_t adr;

    // Phi 2 cycle counter
    register uint32_t current_cycle = 0;

#define RA a
#define RX x
#define RY y
#define RSP sp
#define RPC (pc - c64_ram)
#define N_FLAG n_flag
#define Z_FLAG z_flag
#define PFLAGS pflags
#define ADR adr

#define read_byte(adr) \
    mem_read_table[(adr) >> 8](adr, current_cycle)
#define read_zp(adr) \
    c64_ram[adr]

#define write_byte(adr, byte) \
    mem_write_table[(adr) >> 8](adr, byte, current_cycle, false)
#define write_byte_rmw(adr, byte) \
    mem_write_table[(adr) >> 8](adr, byte, current_cycle, true)
#define write_zp(adr, byte) \
    c64_ram[adr] = (byte)

#define read_idle(adr)
#define read_idle_zp(adr)
#define read_idle_stack(sp)

#define read_opcode \
    (*pc)
#define read_idle_opcode

#define push_byte(byte) \
{ \
    if (sp == 0) \
        quit = true; \
    (c64_ram + 0x100)[sp--] = (byte); \
}
#define pop_byte \
    (sp == 0xff ? (quit = true, 0) : (c64_ram + 0x100)[++sp])

#define jump(adr) \
    pc = c64_ram + (adr)
#define inc_pc \
    pc++

#define next_cycle \
    current_cycle++



    // Jump to specified start address
    jump(startadr);

    // Main loop: execute opcodes until stack under-/overflow, RTI, illegal opcode, or max_cycles reached
    bool quit = false;
    while (current_cycle < max_cycles && !quit) {

        // Fetch opcode
        uint8_t opcode = read_opcode; inc_pc; next_cycle;

        // Execute opcode
        switch (opcode) {
// Load group
case 0xa9:    // LDA #imm
    read_byte_imm(RA);
    set_nz(RA);
    break;

case 0xa5:    // LDA zero
    read_byte_zero(RA);
    set_nz(RA);
    break;

case 0xb5:    // LDA zero,X
    read_byte_zero_x(RA);
    set_nz(RA);
    break;

case 0xad:    // LDA abs
    read_byte_abs(RA);
    set_nz(RA);
    break;

case 0xbd:    // LDA abs,X
    read_byte_abs_x(RA);
    set_nz(RA);
    break;

case 0xb9:    // LDA abs,Y
    read_byte_abs_y(RA);
    set_nz(RA);
    break;

case 0xa1:    // LDA (ind,X)
    read_byte_ind_x(RA);
    set_nz(RA);
    break;

case 0xb1:    // LDA (ind),Y
    read_byte_ind_y(RA);
    set_nz(RA);
    break;

case 0xa2:    // LDX #imm
    read_byte_imm(RX);
    set_nz(RX);
    break;

case 0xa6:    // LDX zero
    read_byte_zero(RX);
    set_nz(RX);
    break;

case 0xb6:    // LDX zero,Y
    read_byte_zero_y(RX);
    set_nz(RX);
    break;

case 0xae:    // LDX abs
    read_byte_abs(RX);
    set_nz(RX);
    break;

case 0xbe:    // LDX abs,Y
    read_byte_abs_y(RX);
    set_nz(RX);
    break;

case 0xa0:    // LDY #imm
    read_byte_imm(RY);
    set_nz(RY);
    break;

case 0xa4:    // LDY zero
    read_byte_zero(RY);
    set_nz(RY);
    break;

case 0xb4:    // LDY zero,X
    read_byte_zero_x(RY);
    set_nz(RY);
    break;

case 0xac:    // LDY abs
    read_byte_abs(RY);
    set_nz(RY);
    break;

case 0xbc:    // LDY abs,X
    read_byte_abs_x(RY);
    set_nz(RY);
    break;


// Store group
case 0x85:    // STA zero
    read_adr_zero;
    write_zp(ADR, RA); next_cycle;
    break;

case 0x95:    // STA zero,X
    read_adr_zero_x;
    write_zp(ADR, RA); next_cycle;
    break;

case 0x8d:    // STA abs
    read_adr_abs;
    write_byte(ADR, RA); next_cycle;
    break;

case 0x9d:    // STA abs,X
    read_adr_abs_x;
    write_byte(ADR, RA); next_cycle;
    break;

case 0x99:    // STA abs,Y
    read_adr_abs_y;
    write_byte(ADR, RA); next_cycle;
    break;

case 0x81:    // STA (ind,X)
    read_adr_ind_x;
    write_byte(ADR, RA); next_cycle;
    break;

case 0x91:    // STA (ind),Y
    read_adr_ind_y;
    write_byte(ADR, RA); next_cycle;
    break;

case 0x86:    // STX zero
    read_adr_zero;
    write_zp(ADR, RX); next_cycle;
    break;

case 0x96:    // STX zero,Y
    read_adr_zero_y;
    write_zp(ADR, RX); next_cycle;
    break;

case 0x8e:    // STX abs
    read_adr_abs;
    write_byte(ADR, RX); next_cycle;
    break;

case 0x84:    // STY zero
    read_adr_zero;
    write_zp(ADR, RY); next_cycle;
    break;

case 0x94:    // STY zero,X
    read_adr_zero_x;
    write_zp(ADR, RY); next_cycle;
    break;

case 0x8c:    // STY abs
    read_adr_abs;
    write_byte(ADR, RY); next_cycle;
    break;


// Transfer group
case 0xaa:    // TAX
    set_nz(RX = RA);
    read_idle_opcode; next_cycle;
    break;

case 0x8a:    // TXA
    set_nz(RA = RX);
    read_idle_opcode; next_cycle;
    break;

case 0xa8:    // TAY
    set_nz(RY = RA);
    read_idle_opcode; next_cycle;
    break;

case 0x98:    // TYA
    set_nz(RA = RY);
    read_idle_opcode; next_cycle;
    break;

case 0xba:    // TSX
    set_nz(RX = RSP);
    read_idle_opcode; next_cycle;
    break;

case 0x9a:    // TXS
    RSP = RX;
    read_idle_opcode; next_cycle;
    break;


// Arithmetic group
case 0x69: {// ADC #imm
    uint8_t t;
    read_byte_imm(t);
    do_adc(t);
}
case 0x65: {// ADC zero
    uint8_t t;
    read_byte_zero(t);
    do_adc(t);
}
case 0x75: {// ADC zero,X
    uint8_t t;
    read_byte_zero_x(t);
    do_adc(t);
}
case 0x6d: {// ADC abs
    uint8_t t;
    read_byte_abs(t);
    do_adc(t);
}
case 0x7d: {// ADC abs,X
    uint8_t t;
    read_byte_abs_x(t);
    do_adc(t);
}
case 0x79: {// ADC abs,Y
    uint8_t t;
    read_byte_abs_y(t);
    do_adc(t);
}
case 0x61: {// ADC (ind,X)
    uint8_t t;
    read_byte_ind_x(t);
    do_adc(t);
}
case 0x71: {// ADC (ind),Y
    uint8_t t;
    read_byte_ind_y(t);
    do_adc(t);
}
case 0xe9:    // SBC #imm
case 0xeb: {// Undocumented opcode
    uint8_t t;
    read_byte_imm(t);
    do_sbc(t);
}
case 0xe5: {// SBC zero
    uint8_t t;
    read_byte_zero(t);
    do_sbc(t);
}
case 0xf5: {// SBC zero,X
    uint8_t t;
    read_byte_zero_x(t);
    do_sbc(t);
}
case 0xed: {// SBC abs
    uint8_t t;
    read_byte_abs(t);
    do_sbc(t);
}
case 0xfd: {// SBC abs,X
    uint8_t t;
    read_byte_abs_x(t);
    do_sbc(t);
}
case 0xf9: {// SBC abs,Y
    uint8_t t;
    read_byte_abs_y(t);
    do_sbc(t);
}
case 0xe1: {// SBC (ind,X)
    uint8_t t;
    read_byte_ind_x(t);
    do_sbc(t);
}
case 0xf1: {// SBC (ind),Y
    uint8_t t;
    read_byte_ind_y(t);
    do_sbc(t);
}


// Increment/decrement group
case 0xe8:    // INX
    set_nz(++RX);
    read_idle_opcode; next_cycle;
    break;

case 0xca:    // DEX
    set_nz(--RX);
    read_idle_opcode; next_cycle;
    break;

case 0xc8:    // INY
    set_nz(++RY);
    read_idle_opcode; next_cycle;
    break;

case 0x88:    // DEY
    set_nz(--RY);
    read_idle_opcode; next_cycle;
    break;

case 0xe6: {// INC zero
    uint32_t t;
    read_byte_zero(t);
    next_cycle; write_zp(ADR, set_nz(t + 1)); next_cycle;
    break;
}
case 0xf6: {// INC zero,X
    uint32_t t;
    read_byte_zero_x(t);
    next_cycle; write_zp(ADR, set_nz(t + 1)); next_cycle;
    break;
}
case 0xee: {// INC abs
    uint32_t t;
    read_byte_abs(t);
    next_cycle; write_byte_rmw(ADR, set_nz(t + 1)); next_cycle;
    break;
}
case 0xfe: {// INC abs,X
    read_adr_abs_x;
    uint32_t t = read_byte(ADR); next_cycle;
    next_cycle; write_byte_rmw(ADR, set_nz(t + 1)); next_cycle;
    break;
}
case 0xc6: {// DEC zero
    uint32_t t;
    read_byte_zero(t);
    next_cycle; write_zp(ADR, set_nz(t - 1)); next_cycle;
    break;
}
case 0xd6: {// DEC zero,X
    uint32_t t;
    read_byte_zero_x(t);
    next_cycle; write_zp(ADR, set_nz(t - 1)); next_cycle;
    break;
}
case 0xce: {// DEC abs
    uint32_t t;
    read_byte_abs(t);
    next_cycle; write_byte_rmw(ADR, set_nz(t - 1)); next_cycle;
    break;
}
case 0xde: {// DEC abs,X
    read_adr_abs_x;
    uint32_t t = read_byte(ADR); next_cycle;
    next_cycle; write_byte_rmw(ADR, set_nz(t - 1)); next_cycle;
    break;
}


// Logic group
case 0x29: {// AND #imm
    uint8_t t;
    read_byte_imm(t);
    set_nz(RA &= t);
    break;
}
case 0x25: {// AND zero
    uint8_t t;
    read_byte_zero(t);
    set_nz(RA &= t);
    break;
}
case 0x35: {// AND zero,X
    uint8_t t;
    read_byte_zero_x(t);
    set_nz(RA &= t);
    break;
}
case 0x2d: {// AND abs
    uint8_t t;
    read_byte_abs(t);
    set_nz(RA &= t);
    break;
}
case 0x3d: {// AND abs,X
    uint8_t t;
    read_byte_abs_x(t);
    set_nz(RA &= t);
    break;
}
case 0x39: {// AND abs,Y
    uint8_t t;
    read_byte_abs_y(t);
    set_nz(RA &= t);
    break;
}
case 0x21: {// AND (ind,X)
    uint8_t t;
    read_byte_ind_x(t);
    set_nz(RA &= t);
    break;
}
case 0x31: {// AND (ind),Y
    uint8_t t;
    read_byte_ind_y(t);
    set_nz(RA &= t);
    break;
}
case 0x09: {// ORA #imm
    uint8_t t;
    read_byte_imm(t);
    set_nz(RA |= t);
    break;
}
case 0x05: {// ORA zero
    uint8_t t;
    read_byte_zero(t);
    set_nz(RA |= t);
    break;
}
case 0x15: {// ORA zero,X
    uint8_t t;
    read_byte_zero_x(t);
    set_nz(RA |= t);
    break;
}
case 0x0d: {// ORA abs
    uint8_t t;
    read_byte_abs(t);
    set_nz(RA |= t);
    break;
}
case 0x1d: {// ORA abs,X
    uint8_t t;
    read_byte_abs_x(t);
    set_nz(RA |= t);
    break;
}
case 0x19: {// ORA abs,Y
    uint8_t t;
    read_byte_abs_y(t);
    set_nz(RA |= t);
    break;
}
case 0x01: {// ORA (ind,X)
    uint8_t t;
    read_byte_ind_x(t);
    set_nz(RA |= t);
    break;
}
case 0x11: {// ORA (ind),Y
    uint8_t t;
    read_byte_ind_y(t);
    set_nz(RA |= t);
    break;
}
case 0x49: {// EOR #imm
    uint8_t t;
    read_byte_imm(t);
    set_nz(RA ^= t);
    break;
}
case 0x45: {// EOR zero
    uint8_t t;
    read_byte_zero(t);
    set_nz(RA ^= t);
    break;
}
case 0x55: {// EOR zero,X
    uint8_t t;
    read_byte_zero_x(t);
    set_nz(RA ^= t);
    break;
}
case 0x4d: {// EOR abs
    uint8_t t;
    read_byte_abs(t);
    set_nz(RA ^= t);
    break;
}
case 0x5d: {// EOR abs,X
    uint8_t t;
    read_byte_abs_x(t);
    set_nz(RA ^= t);
    break;
}
case 0x59: {// EOR abs,Y
    uint8_t t;
    read_byte_abs_y(t);
    set_nz(RA ^= t);
    break;
}
case 0x41: {// EOR (ind,X)
    uint8_t t;
    read_byte_ind_x(t);
    set_nz(RA ^= t);
    break;
}
case 0x51: {// EOR (ind),Y
    uint8_t t;
    read_byte_ind_y(t);
    set_nz(RA ^= t);
    break;
}


// Compare group
case 0xc9: {// CMP #imm
    uint8_t t;
    read_byte_imm(t);
    do_cmp;
}
case 0xc5: {// CMP zero
    uint8_t t;
    read_byte_zero(t);
    do_cmp;
}
case 0xd5: {// CMP zero,X
    uint8_t t;
    read_byte_zero_x(t);
    do_cmp;
}
case 0xcd: {// CMP abs
    uint8_t t;
    read_byte_abs(t);
    do_cmp;
}
case 0xdd: {// CMP abs,X
    uint8_t t;
    read_byte_abs_x(t);
    do_cmp;
}
case 0xd9: {// CMP abs,Y
    uint8_t t;
    read_byte_abs_y(t);
    do_cmp;
}
case 0xc1: {// CMP (ind,X)
    uint8_t t;
    read_byte_ind_x(t);
    do_cmp;
}
case 0xd1: {// CMP (ind),Y
    uint8_t t;
    read_byte_ind_y(t);
    do_cmp;
}
case 0xe0: {// CPX #imm
    uint8_t t;
    read_byte_imm(t);
    do_cpx;
}
case 0xe4: {// CPX zero
    uint8_t t;
    read_byte_zero(t);
    do_cpx;
}
case 0xec: {// CPX abs
    uint8_t t;
    read_byte_abs(t);
    do_cpx;
}
case 0xc0: {// CPY #imm
    uint8_t t;
    read_byte_imm(t);
    do_cpy;
}
case 0xc4: {// CPY zero
    uint8_t t;
    read_byte_zero(t);
    do_cpy;
}
case 0xcc: {// CPY abs
    uint8_t t;
    read_byte_abs(t);
    do_cpy;
}


// Bit-test group
case 0x24: {// BIT zero
    uint8_t t;
    read_byte_zero(t);
    do_bit;
}
case 0x2c: {// BIT abs
    uint8_t t;
    read_byte_abs(t);
    do_bit;
}


// Shift/rotate group
case 0x0a:    // ASL A
    (RA & 0x80) ? (PFLAGS |= PFLAG_C) : (PFLAGS &= ~PFLAG_C);
    set_nz(RA <<= 1);
    read_idle_opcode; next_cycle;
    break;

case 0x06: {// ASL zero
    uint32_t t;
    read_byte_zero(t);
    do_asl(write_zp);
}
case 0x16: {// ASL zero,X
    uint32_t t;
    read_byte_zero_x(t);
    do_asl(write_zp);
}
case 0x0e: {// ASL abs
    uint32_t t;
    read_byte_abs(t);
    do_asl(write_byte_rmw);
}
case 0x1e: {// ASL abs,X
    read_adr_abs_x;
    uint32_t t = read_byte(ADR); next_cycle;
    do_asl(write_byte_rmw);
}
case 0x4a:    // LSR A
    (RA & 0x01) ? (PFLAGS |= PFLAG_C) : (PFLAGS &= ~PFLAG_C);
    set_nz(RA >>= 1);
    read_idle_opcode; next_cycle;
    break;

case 0x46: {// LSR zero
    uint32_t t;
    read_byte_zero(t);
    do_lsr(write_zp);
}
case 0x56: {// LSR zero,X
    uint32_t t;
    read_byte_zero_x(t);
    do_lsr(write_zp);
}
case 0x4e: {// LSR abs
    uint32_t t;
    read_byte_abs(t);
    do_lsr(write_byte_rmw);
}
case 0x5e: {// LSR abs,X
    read_adr_abs_x;
    uint32_t t = read_byte(ADR); next_cycle;
    do_lsr(write_byte_rmw);
}
case 0x2a: {// ROL A
    uint8_t t = RA;
    set_nz(RA = (RA << 1) | (PFLAGS & PFLAG_C));
    (t & 0x80) ? (PFLAGS |= PFLAG_C) : (PFLAGS &= ~PFLAG_C);
    read_idle_opcode; next_cycle;
    break;
}
case 0x26: {// ROL zero
    uint32_t t;
    read_byte_zero(t);
    do_rol(write_zp);
}
case 0x36: {// ROL zero,X
    uint32_t t;
    read_byte_zero_x(t);
    do_rol(write_zp);
}
case 0x2e: {// ROL abs
    uint32_t t;
    read_byte_abs(t);
    do_rol(write_byte_rmw);
}
case 0x3e: {// ROL abs,X
    read_adr_abs_x;
    uint32_t t = read_byte(ADR); next_cycle;
    do_rol(write_byte_rmw);
}
case 0x6a: {// ROR A
    uint8_t t = RA;
    set_nz(RA = ((PFLAGS & PFLAG_C) ? ((RA >> 1) | 0x80) : (RA >> 1)));
    (t & 0x01) ? (PFLAGS |= PFLAG_C) : (PFLAGS &= ~PFLAG_C);
    read_idle_opcode; next_cycle;
    break;
}
case 0x66: {// ROR zero
    uint32_t t;
    read_byte_zero(t);
    do_ror(write_zp);
}
case 0x76: {// ROR zero,X
    uint32_t t;
    read_byte_zero_x(t);
    do_ror(write_zp);
}
case 0x6e:    {// ROR abs
    uint32_t t;
    read_byte_abs(t);
    do_ror(write_byte_rmw);
}
case 0x7e:    {// ROR abs,X
    read_adr_abs_x;
    uint32_t t = read_byte(ADR); next_cycle;
    do_ror(write_byte_rmw);
}


// Stack group
case 0x48:    // PHA
    read_idle_opcode; next_cycle;
    push_byte(RA); next_cycle;
    break;

case 0x68:    // PLA
    read_idle_opcode; next_cycle;
    read_idle_stack(RSP); next_cycle;
    set_nz(RA = pop_byte); next_cycle;
    break;

case 0x08:    // PHP
    read_idle_opcode; next_cycle;
    push_flags(PFLAG_B);
    break;

case 0x28: {// PLP
    read_idle_opcode; next_cycle;
    read_idle_stack(RSP); next_cycle;
    uint8_t old_pflags = PFLAGS;
    pop_flags;
    if (!(old_pflags & PFLAG_I) && (PFLAGS & PFLAG_I))
        opcode |= OPFLAG_IRQ_DISABLED;
    else if ((old_pflags & PFLAG_I) && !(PFLAGS & PFLAG_I))
        opcode |= OPFLAG_IRQ_ENABLED;
    break;
}


// Jump/branch group
case 0x4c:    // JMP abs
    read_adr_abs;
    jump(ADR);
    break;

case 0x6c: {// JMP (ind)
    read_adr_abs;
    uint8_t t = read_byte(ADR); next_cycle;
    jump(t | (read_byte(((ADR + 1) & 0xff) | (ADR & 0xff00)) << 8)); next_cycle;
    break;
}
case 0x20: {// JSR abs
    uint8_t t = read_opcode; inc_pc; next_cycle;
    read_idle_stack(RSP); next_cycle;
    push_byte(RPC >> 8); next_cycle;
    push_byte(RPC); next_cycle;
    jump(t | (read_opcode << 8)); next_cycle;
    break;
}
case 0x60: {// RTS
    read_idle_opcode; next_cycle;
    read_idle_stack(RSP); next_cycle;
    uint8_t t = pop_byte; next_cycle;
    jump(t | (pop_byte << 8)); inc_pc; next_cycle;
    read_idle_opcode; next_cycle;
    break;
}
case 0x40: {// RTI
    quit = true;
    break;
}
case 0x00: {// BRK
    read_idle_opcode; inc_pc; next_cycle;
    push_byte(RPC >> 8); next_cycle;
    push_byte(RPC); next_cycle;
    push_flags(PFLAG_B);
    PFLAGS |= PFLAG_I;
    uint8_t t = read_byte(0xfffe); next_cycle;
    jump(t | (read_byte(0xffff) << 8 )); next_cycle;
    break;
}
case 0xb0:    // BCS rel
    branch(PFLAGS & PFLAG_C);

case 0x90:    // BCC rel
    branch(!(PFLAGS & PFLAG_C));

case 0xf0:    // BEQ rel
    branch(!Z_FLAG);

case 0xd0:    // BNE rel
    branch(Z_FLAG);

case 0x70:    // BVS rel
#ifdef DRIVE_CPU
    gcr_drive->Update(current_cycle);
    if (gcr_drive->ByteReady())
        branch(1)
    else
#endif
    branch(PFLAGS & PFLAG_V);

case 0x50:    // BVC rel
#ifdef DRIVE_CPU
    gcr_drive->Update(current_cycle);
    if (gcr_drive->ByteReady())
        branch(0)
    else
#endif
    branch(!(PFLAGS & PFLAG_V));

case 0x30:    // BMI rel
    branch(N_FLAG & 0x80);

case 0x10:    // BPL rel
    branch(!(N_FLAG & 0x80));


// Flags group
case 0x38:    // SEC
    PFLAGS |= PFLAG_C;
    read_idle_opcode; next_cycle;
    break;

case 0x18:    // CLC
    PFLAGS &= ~PFLAG_C;
    read_idle_opcode; next_cycle;
    break;

case 0xf8:    // SED
    PFLAGS |= PFLAG_D;
    read_idle_opcode; next_cycle;
    break;

case 0xd8:    // CLD
    PFLAGS &= ~PFLAG_D;
    read_idle_opcode; next_cycle;
    break;

case 0x78:    // SEI
    if (!(PFLAGS & PFLAG_I))
        opcode |= OPFLAG_IRQ_DISABLED;
    PFLAGS |= PFLAG_I;
    read_idle_opcode; next_cycle;
    break;

case 0x58:    // CLI
    if (PFLAGS & PFLAG_I)
        opcode |= OPFLAG_IRQ_ENABLED;
    PFLAGS &= ~PFLAG_I;
    read_idle_opcode; next_cycle;
    break;

case 0xb8:    // CLV
    PFLAGS &= ~PFLAG_V;
    read_idle_opcode; next_cycle;
    break;


// NOP group
case 0xea:    // NOP
    read_idle_opcode; next_cycle;
    break;


/*
 *  Routines for undocumented opcodes
 */

// Load A/X group
case 0xa7:    // LAX zero
    read_byte_zero(RA);
    set_nz(RX = RA);
    break;

case 0xb7:    // LAX zero,Y
    read_byte_zero_y(RA);
    set_nz(RX = RA);
    break;

case 0xaf:    // LAX abs
    read_byte_abs(RA);
    set_nz(RX = RA);
    break;

case 0xbf:    // LAX abs,Y
    read_byte_abs_y(RA);
    set_nz(RX = RA);
    break;

case 0xa3:    // LAX (ind,X)
    read_byte_ind_x(RA);
    set_nz(RX = RA);
    break;

case 0xb3:    // LAX (ind),Y
    read_byte_ind_y(RA);
    set_nz(RX = RA);
    break;


// Store A/X group
case 0x87:    // SAX zero
    read_adr_zero;
    write_zp(ADR, RA & RX); next_cycle;
    break;

case 0x97:    // SAX zero,Y
    read_adr_zero_y;
    write_zp(ADR, RA & RX); next_cycle;
    break;

case 0x8f:    // SAX abs
    read_adr_abs;
    write_byte(ADR, RA & RX); next_cycle;
    break;

case 0x83:    // SAX (ind,X)
    read_adr_ind_x;
    write_byte(ADR, RA & RX); next_cycle;
    break;


// ASL/ORA group
case 0x07: {// SLO zero
    uint32_t t;
    read_byte_zero(t);
    do_slo(write_zp);
}
case 0x17: {// SLO zero,X
    uint32_t t;
    read_byte_zero_x(t);
    do_slo(write_zp);
}
case 0x0f: {// SLO abs
    uint32_t t;
    read_byte_abs(t);
    do_slo(write_byte_rmw);
}
case 0x1f: {// SLO abs,X
    read_adr_abs_x;
    uint32_t t = read_byte(ADR); next_cycle;
    do_slo(write_byte_rmw);
}
case 0x1b: {// SLO abs,Y
    read_adr_abs_y;
    uint32_t t = read_byte(ADR); next_cycle;
    do_slo(write_byte_rmw);
}
case 0x03: {// SLO (ind,X)
    read_adr_ind_x;
    uint32_t t = read_byte(ADR); next_cycle;
    do_slo(write_byte_rmw);
}
case 0x13: {// SLO (ind),Y
    read_adr_ind_y;
    uint32_t t = read_byte(ADR); next_cycle;
    do_slo(write_byte_rmw);
}


// ROL/AND group
case 0x27: {// RLA zero
    uint32_t t;
    read_byte_zero(t);
    do_rla(write_zp);
}
case 0x37: {// RLA zero,X
    uint32_t t;
    read_byte_zero_x(t);
    do_rla(write_zp);
}
case 0x2f: {// RLA abs
    uint32_t t;
    read_byte_abs(t);
    do_rla(write_byte_rmw);
}
case 0x3f: {// RLA abs,X
    read_adr_abs_x;
    uint32_t t = read_byte(ADR); next_cycle;
    do_rla(write_byte_rmw);
}
case 0x3b: {// RLA abs,Y
    read_adr_abs_y;
    uint32_t t = read_byte(ADR); next_cycle;
    do_rla(write_byte_rmw);
}
case 0x23: {// RLA (ind,X)
    read_adr_ind_x;
    uint32_t t = read_byte(ADR); next_cycle;
    do_rla(write_byte_rmw);
}
case 0x33: {// RLA (ind),Y
    read_adr_ind_y;
    uint32_t t = read_byte(ADR); next_cycle;
    do_rla(write_byte_rmw);
}


// LSR/EOR group
case 0x47: {// SRE zero
    uint32_t t;
    read_byte_zero(t);
    do_sre(write_zp);
}
case 0x57: {// SRE zero,X
    uint32_t t;
    read_byte_zero_x(t);
    do_sre(write_zp);
}
case 0x4f: {// SRE abs
    uint32_t t;
    read_byte_abs(t);
    do_sre(write_byte_rmw);
}
case 0x5f: {// SRE abs,X
    read_adr_abs_x;
    uint32_t t = read_byte(ADR); next_cycle;
    do_sre(write_byte_rmw);
}
case 0x5b: {// SRE abs,Y
    read_adr_abs_y;
    uint32_t t = read_byte(ADR); next_cycle;
    do_sre(write_byte_rmw);
}
case 0x43: {// SRE (ind,X)
    read_adr_ind_x;
    uint32_t t = read_byte(ADR); next_cycle;
    do_sre(write_byte_rmw);
}
case 0x53: {// SRE (ind),Y
    read_adr_ind_y;
    uint32_t t = read_byte(ADR); next_cycle;
    do_sre(write_byte_rmw);
}


// ROR/ADC group
case 0x67: {// RRA zero
    uint32_t t;
    read_byte_zero(t);
    do_rra(write_zp);
}
case 0x77: {// RRA zero,X
    uint32_t t;
    read_byte_zero_x(t);
    do_rra(write_zp);
}
case 0x6f: {// RRA abs
    uint32_t t;
    read_byte_abs(t);
    do_rra(write_byte_rmw);
}
case 0x7f: {// RRA abs,X
    read_adr_abs_x;
    uint32_t t = read_byte(ADR); next_cycle;
    do_rra(write_byte_rmw);
}
case 0x7b: {// RRA abs,Y
    read_adr_abs_y;
    uint32_t t = read_byte(ADR); next_cycle;
    do_rra(write_byte_rmw);
}
case 0x63: {// RRA (ind,X)
    read_adr_ind_x;
    uint32_t t = read_byte(ADR); next_cycle;
    do_rra(write_byte_rmw);
}
case 0x73: {// RRA (ind),Y
    read_adr_ind_y;
    uint32_t t = read_byte(ADR); next_cycle;
    do_rra(write_byte_rmw);
}


// DEC/CMP group
case 0xc7: {// DCP zero
    uint32_t t;
    read_byte_zero(t);
    do_dcp(write_zp);
}
case 0xd7: {// DCP zero,X
    uint32_t t;
    read_byte_zero_x(t);
    do_dcp(write_zp);
}
case 0xcf: {// DCP abs
    uint32_t t;
    read_byte_abs(t);
    do_dcp(write_byte_rmw);
}
case 0xdf: {// DCP abs,X
    read_adr_abs_x;
    uint32_t t = read_byte(ADR); next_cycle;
    do_dcp(write_byte_rmw);
}
case 0xdb: {// DCP abs,Y
    read_adr_abs_y;
    uint32_t t = read_byte(ADR); next_cycle;
    do_dcp(write_byte_rmw);
}
case 0xc3: {// DCP (ind,X)
    read_adr_ind_x;
    uint32_t t = read_byte(ADR); next_cycle;
    do_dcp(write_byte_rmw);
}
case 0xd3: {// DCP (ind),Y
    read_adr_ind_y;
    uint32_t t = read_byte(ADR); next_cycle;
    do_dcp(write_byte_rmw);
}


// INC/SBC group
case 0xe7: {// ISB zero
    uint32_t t;
    read_byte_zero(t);
    do_isb(write_zp);
}
case 0xf7: {// ISB zero,X
    uint32_t t;
    read_byte_zero_x(t);
    do_isb(write_zp);
}
case 0xef: {// ISB abs
    uint32_t t;
    read_byte_abs(t);
    do_isb(write_byte_rmw);
}
case 0xff: {// ISB abs,X
    read_adr_abs_x;
    uint32_t t = read_byte(ADR); next_cycle;
    do_isb(write_byte_rmw);
}
case 0xfb: {// ISB abs,Y
    read_adr_abs_y;
    uint32_t t = read_byte(ADR); next_cycle;
    do_isb(write_byte_rmw);
}
case 0xe3: {// ISB (ind,X)
    read_adr_ind_x;
    uint32_t t = read_byte(ADR); next_cycle;
    do_isb(write_byte_rmw);
}
case 0xf3: {// ISB (ind),Y
    read_adr_ind_y;
    uint32_t t = read_byte(ADR); next_cycle;
    do_isb(write_byte_rmw);
}


// Complex functions
case 0x0b:    // ANC #imm
case 0x2b: {
    uint8_t t;
    read_byte_imm(t);
    set_nz(RA &= t);
    (N_FLAG & 0x80) ? (PFLAGS |= PFLAG_C) : (PFLAGS &= ~PFLAG_C);
    break;
}
case 0x4b: {// ASR #imm
    uint8_t t;
    read_byte_imm(t);
    RA &= t;
    (RA & 0x01) ? (PFLAGS |= PFLAG_C) : (PFLAGS &= ~PFLAG_C);
    set_nz(RA >>= 1);
    break;
}
case 0x6b: {// ARR #imm
    uint32_t t;
    read_byte_imm(t);
    t &= RA;
    RA = ((PFLAGS & PFLAG_C) ? (t >> 1) | 0x80 : (t >> 1));
    if (PFLAGS & PFLAG_D) {
        N_FLAG = (PFLAGS & PFLAG_C) << 7;
        Z_FLAG = RA;
        ((t ^ RA) & 0x40) ? (PFLAGS |= PFLAG_V) : (PFLAGS &= ~PFLAG_V);
        if ((t & 0x0f) + (t & 0x01) > 5)
            RA = (RA & 0xf0) | ((RA + 6) & 0x0f);
        (((t + (t & 0x10)) & 0x1f0) > 0x50) ? (PFLAGS |= PFLAG_C, RA += 0x60) : (PFLAGS &= ~PFLAG_C);
    } else {
        set_nz(RA);
        (RA & 0x40) ? (PFLAGS |= PFLAG_C) : (PFLAGS &= ~PFLAG_C);
        ((RA & 0x40) ^ ((RA & 0x20) << 1)) ? (PFLAGS |= PFLAG_V) : (PFLAGS &= ~PFLAG_V);
    }
    break;
}
case 0x8b: {// ANE #imm
    uint8_t t;
    read_byte_imm(t);
    set_nz(RA = (RA | 0xee) & RX & t);
    break;
}
case 0x93: {// SHA (ind),Y
    read_adr_ind_y;
    write_byte(ADR, RA & RX & (((ADR - RY) >> 8) + 1)); next_cycle;
    break;
}
case 0x9b: {// SHS abs,Y
    read_adr_abs_y;
    write_byte(ADR, (RSP = RA & RX) & (((ADR - RY) >> 8) + 1)); next_cycle;
    break;
}
case 0x9c: {// SHY abs,X
    read_adr_abs_x;
    write_byte(ADR, RY & (((ADR - RX) >> 8) + 1)); next_cycle;
    break;
}
case 0x9e: {// SHX abs,Y
    read_adr_abs_y;
    write_byte(ADR, RX & (((ADR - RY) >> 8) + 1)); next_cycle;
    break;
}
case 0x9f: {// SHA abs,Y
    read_adr_abs_y;
    write_byte(ADR, RA & RX & (((ADR - RY) >> 8) + 1)); next_cycle;
    break;
}
case 0xab: {// LXA #imm
    uint8_t t;
    read_byte_imm(t);
    set_nz(RA = RX = (RA | 0xee) & t);
    break;
}
case 0xbb: {// LAS abs,Y
    uint8_t t;
    read_byte_abs_y(t);
    set_nz(RA = RX = RSP = t & RSP);
    break;
}
case 0xcb: {// SBX #imm
    uint32_t t;
    read_byte_imm(t);
    set_nz(RX = t = (RA & RX) - t);
    (t < 0x100) ? (PFLAGS |= PFLAG_C) : (PFLAGS &= ~PFLAG_C);
    break;
}


// NOP group
case 0x1a:    // NOP
case 0x3a:
case 0x5a:
case 0x7a:
case 0xda:
case 0xfa:
    read_idle_opcode; next_cycle;
    break;

case 0x80:    // NOP #imm
case 0x82:
case 0x89:
case 0xc2:
case 0xe2:
    read_idle_opcode; inc_pc; next_cycle;
    break;

case 0x04:    // NOP zero
case 0x44:
case 0x64:
    read_adr_zero;
    read_idle_zp(ADR); next_cycle;
    break;

case 0x14:    // NOP zero,X
case 0x34:
case 0x54:
case 0x74:
case 0xd4:
case 0xf4:
    read_adr_zero_x;
    read_idle_zp(ADR); next_cycle;
    break;

case 0x0c:    // NOP abs
    read_adr_abs;
    read_idle(ADR); next_cycle;
    break;

case 0x1c:    // NOP abs,X
case 0x3c:
case 0x5c:
case 0x7c:
case 0xdc:
case 0xfc: {
    uint8_t t;
    read_byte_abs_x(t);
    dummy = t;  // compiler warning
    break;
}


// Jam group
case 0x02:
case 0x12:
case 0x22:
case 0x32:
case 0x42:
case 0x52:
case 0x62:
case 0x72:
case 0x92:
case 0xb2:
case 0xd2:
    goto illegal_op;

            case 0xf2:
            default:
illegal_op:        quit = true;
                break;
        }
    }
}
// CPU-Ende






// Resonance frequency polynomials
float CALC_RESONANCE_LP(float f)
{
    return 227.755 - 1.7635 * f - 0.0176385 * f * f + 0.00333484 * f * f * f;
}

float CALC_RESONANCE_HP(float f)
{
    return 366.374 - 14.0052 * f + 0.603212 * f * f - 0.000880196 * f * f * f;
}

// Pseudo-random number generator for SID noise waveform (don't use f_rand()
// because the SID waveform calculation runs asynchronously and the output of
// f_rand() has to be predictable inside the main emulation)

uint8_t noise_rand()
{
    // This is not the original SID noise algorithm (which is unefficient to
    // implement in software) but this sounds close enough
    noise_rand_seed = noise_rand_seed * 1103515245 + 12345;
    return noise_rand_seed >> 16;
}

// SID waveforms
enum {
    WAVE_NONE,
    WAVE_TRI,
    WAVE_SAW,
    WAVE_TRISAW,
    WAVE_RECT,
    WAVE_TRIRECT,
    WAVE_SAWRECT,
    WAVE_TRISAWRECT,
    WAVE_NOISE
};

// EG states
enum {
    EG_IDLE,
    EG_ATTACK,
    EG_DECAY,
    EG_RELEASE
};

// Filter types
enum {
    FILT_NONE,
    FILT_LP,
    FILT_BP,
    FILT_LPBP,
    FILT_HP,
    FILT_NOTCH,
    FILT_HPBP,
    FILT_ALL
};

// Voice 4 states (SIDPlayer only)
enum {
    V4_OFF,
    V4_GALWAY_NOISE,
    V4_SAMPLE
};


// Waveform tables
uint16_t tri_table[0x1000*2];
const uint16_t *tri_saw_table;
const uint16_t *tri_rect_table;
const uint16_t *saw_rect_table;
const uint16_t *tri_saw_rect_table;

// Sampled from a 6581R4
const uint16_t tri_saw_table_6581[0x100] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x0808,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x1010, 0x3C3C,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x0808,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x1010, 0x3C3C
};

const uint16_t tri_rect_table_6581[0x100] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x8080,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x8080,
    0, 0, 0, 0, 0, 0, 0x8080, 0xC0C0, 0, 0x8080, 0x8080, 0xE0E0, 0x8080, 0xE0E0, 0xF0F0, 0xFCFC,
    0xFFFF, 0xFCFC, 0xFAFA, 0xF0F0, 0xF6F6, 0xE0E0, 0xE0E0, 0x8080, 0xEEEE, 0xE0E0, 0xE0E0, 0x8080, 0xC0C0, 0, 0, 0,
    0xDEDE, 0xC0C0, 0xC0C0, 0, 0x8080, 0, 0, 0, 0x8080, 0, 0, 0, 0, 0, 0, 0,
    0xBEBE, 0x8080, 0x8080, 0, 0x8080, 0, 0, 0, 0x8080, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0x7E7E, 0x4040, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const uint16_t saw_rect_table_6581[0x100] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x7878,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x7878
};

const uint16_t tri_saw_rect_table_6581[0x100] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Sampled from an 8580R5
const uint16_t tri_saw_table_8580[0x100] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x0808,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x1818, 0x3C3C,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x1C1C,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x8080, 0, 0x8080, 0x8080,
    0xC0C0, 0xC0C0, 0xC0C0, 0xC0C0, 0xC0C0, 0xC0C0, 0xC0C0, 0xE0E0, 0xF0F0, 0xF0F0, 0xF0F0, 0xF0F0, 0xF8F8, 0xF8F8, 0xFCFC, 0xFEFE
};

const uint16_t tri_rect_table_8580[0x100] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0xFFFF, 0xFCFC, 0xF8F8, 0xF0F0, 0xF4F4, 0xF0F0, 0xF0F0, 0xE0E0, 0xECEC, 0xE0E0, 0xE0E0, 0xC0C0, 0xE0E0, 0xC0C0, 0xC0C0, 0xC0C0,
    0xDCDC, 0xC0C0, 0xC0C0, 0xC0C0, 0xC0C0, 0xC0C0, 0x8080, 0x8080, 0xC0C0, 0x8080, 0x8080, 0x8080, 0x8080, 0x8080, 0, 0,
    0xBEBE, 0xA0A0, 0x8080, 0x8080, 0x8080, 0x8080, 0x8080, 0, 0x8080, 0x8080, 0, 0, 0, 0, 0, 0,
    0x8080, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0x7E7E, 0x7070, 0x6060, 0, 0x4040, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const uint16_t saw_rect_table_8580[0x100] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x8080,
    0, 0, 0, 0, 0, 0, 0x8080, 0x8080, 0, 0x8080, 0x8080, 0x8080, 0x8080, 0x8080, 0xB0B0, 0xBEBE,
    0, 0, 0, 0, 0, 0, 0, 0x8080, 0, 0, 0, 0x8080, 0x8080, 0x8080, 0x8080, 0xC0C0,
    0, 0x8080, 0x8080, 0x8080, 0x8080, 0x8080, 0x8080, 0xC0C0, 0x8080, 0x8080, 0xC0C0, 0xC0C0, 0xC0C0, 0xC0C0, 0xC0C0, 0xDCDC,
    0x8080, 0x8080, 0x8080, 0xC0C0, 0xC0C0, 0xC0C0, 0xC0C0, 0xC0C0, 0xC0C0, 0xC0C0, 0xC0C0, 0xE0E0, 0xE0E0, 0xE0E0, 0xE0E0, 0xECEC,
    0xC0C0, 0xE0E0, 0xE0E0, 0xE0E0, 0xE0E0, 0xF0F0, 0xF0F0, 0xF4F4, 0xF0F0, 0xF0F0, 0xF8F8, 0xF8F8, 0xF8F8, 0xFCFC, 0xFEFE, 0xFFFF
};

const uint16_t tri_saw_rect_table_8580[0x100] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x8080, 0x8080,
    0x8080, 0x8080, 0x8080, 0x8080, 0x8080, 0x8080, 0xC0C0, 0xC0C0, 0xC0C0, 0xC0C0, 0xE0E0, 0xE0E0, 0xE0E0, 0xF0F0, 0xF8F8, 0xFCFC
};

// Envelope tables
uint32_t eg_table[16];

const uint8_t eg_dr_shift[256] = {
    5,5,5,5,5,5,5,5,4,4,4,4,4,4,4,4,
    3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

// Filter tables
float ffreq_lp[256];    // Low-pass resonance frequency table
float ffreq_hp[256];    // High-pass resonance frequency table

// Table for sampled voices
const int16_t sample_tab[16 * 3] = {
    0x8000, 0x9111, 0xa222, 0xb333, 0xc444, 0xd555, 0xe666, 0xf777,
    0x0888, 0x1999, 0x2aaa, 0x3bbb, 0x4ccc, 0x5ddd, 0x6eee, 0x7fff,

    0xc444, 0xc444, 0xd555, 0xd555, 0xe666, 0xe666, 0xf777, 0xf777,
    0x0888, 0x0888, 0x1999, 0x1999, 0x2aaa, 0x2aaa, 0x3bbb, 0x3bbb,

    0xe666, 0xe666, 0xe666, 0xe666, 0xf777, 0xf777, 0xf777, 0xf777,
    0x0888, 0x0888, 0x0888, 0x0888, 0x1999, 0x1999, 0x1999, 0x1999
};

int16_t galway_tab[16 * 64];

// Work buffer and variables for audio effects
#define WORK_BUFFER_SIZE 0x10000
int16_t work_buffer[WORK_BUFFER_SIZE];
int32_t wb_read_offset = 0, wb_write_offset = 0;
int32_t rev_feedback = 0;


/*
 *  Init SID emulation
 */

void osid_init(osid_t *sid, int32_t n)
{
    sid->sid_num = n;

    // Link voices
    sid->voice[0].mod_by = &sid->voice[2];
    sid->voice[1].mod_by = &sid->voice[0];
    sid->voice[2].mod_by = &sid->voice[1];
    sid->voice[0].mod_to = &sid->voice[1];
    sid->voice[1].mod_to = &sid->voice[2];
    sid->voice[2].mod_to = &sid->voice[0];

    osid_reset(sid);
}



void calc_gains()
{
    if (dual_sid) {
        osid_calc_gains(sid1, true, false);
        osid_calc_gains(sid2, false, true);
    } else
        osid_calc_gains(sid1, false, false);
}

void set_sid_data()
{
    if (emulate_8580) {
        tri_saw_table = tri_saw_table_8580;
        tri_rect_table = tri_rect_table_8580;
        saw_rect_table = saw_rect_table_8580;
        tri_saw_rect_table = tri_saw_rect_table_8580;
    } else {
        tri_saw_table = tri_saw_table_6581;
        tri_rect_table = tri_rect_table_6581;
        saw_rect_table = saw_rect_table_6581;
        tri_saw_rect_table = tri_saw_rect_table_6581;
    }
}


/*
PrefsAddString("victype", "6569");
    PrefsAddString("sidtype", "6581");
    PrefsAddInt32("samplerate", 44100);
    PrefsAddBool("audio16bit", true);
    PrefsAddBool("stereo", true);
    PrefsAddBool("filters", true);
    PrefsAddBool("dualsid", false);
    PrefsAddInt32("audioeffect", 2);
    PrefsAddInt32("revdelay", 125);
    PrefsAddInt32("revfeedback", 0x50);
    PrefsAddInt32("volume", 0x100);
    PrefsAddInt32("v1volume", 0x100);
    PrefsAddInt32("v1pan", -0x40);
    PrefsAddInt32("v2volume", 0x100);
    PrefsAddInt32("v2pan", 0);
    PrefsAddInt32("v3volume", 0x100);
    PrefsAddInt32("v3pan", 0x40);
    PrefsAddInt32("v4volume", 0x100);
    PrefsAddInt32("v4pan", 0);
    PrefsAddInt32("dualsep", 0x80);
    PrefsAddInt32("speed", 100);
*/


void SIDInit()
{
    int32_t i, j;

    emulate_8580 = false;
    enable_filters = true;
    dual_sid = false;
    f_rand_seed = 1;
    noise_rand_seed = 1;
    cycles_per_second = NTSC_CLOCK;
    speed_adjust = 100;
    audio_effect = 2;
    rev_feedback = 0x50;
    master_volume = 0x100;
    v1_volume = 0x100;
    v2_volume = 0x100;
    v3_volume = 0x100;
    v4_volume = 0x100;
    v1_panning = -0x40;
    v2_panning = 0;
    v3_panning = 0x40;
    v4_panning = 0;
    dual_sep = 0x80;


    sid1 = &sid01;
    sid2 = &sid02;
    osid_init(sid1, 0);
    osid_init(sid2, 1);
    set_sid_data();

    calc_gains();

    // Compute number of cycles per sample frame and envelope table
    SIDClockFreqChanged();

    // Compute triangle table
    for (i=0; i<0x1000; i++) {
        tri_table[i] = (i << 4) | (i >> 8);
        tri_table[0x1fff-i] = (i << 4) | (i >> 8);
    }

    // Compute filter tables
    for (i=0; i<256; i++) {
        ffreq_lp[i] = CALC_RESONANCE_LP(i);
        ffreq_hp[i] = CALC_RESONANCE_HP(i);
    }

    // Compute galway noise table
    for (i=0; i<16; i++)
        for (j=0; j<64; j++)
            galway_tab[i * 64 + j] = sample_tab[(i * j) & 0x0f];
}


/*
 *  Reset SID emulation
 */

void osid_reset(osid_t *sid)
{
    memset(sid->regs, 0, sizeof(sid->regs));
    sid->last_written_byte = 0;

    sid->volume = 15;
    sid->regs[24] = 0x0f;

    int32_t v;
    for (v=0; v<3; v++) {
        sid->voice[v].wave = WAVE_NONE;
        sid->voice[v].eg_state = EG_IDLE;
        sid->voice[v].count = sid->voice[v].add = 0;
        sid->voice[v].freq = sid->voice[v].pw = 0;
        sid->voice[v].eg_level = sid->voice[v].s_level = 0;
        sid->voice[v].a_add = sid->voice[v].d_sub = sid->voice[v].r_sub = eg_table[0];
        sid->voice[v].gate = sid->voice[v].ring = sid->voice[v].test = false;
        sid->voice[v].filter = sid->voice[v].sync = sid->voice[v].mute = false;
    }

    sid->f_type = FILT_NONE;
    sid->f_freq = sid->f_res = 0;
    sid->f_ampl = 1.0;
    sid->d1 = sid->d2 = sid->g1 = sid->g2 = 0.0;
    sid->xn1_l = sid->xn2_l = sid->yn1_l = sid->yn2_l = 0.0;
    sid->xn1_r = sid->xn2_r = sid->yn1_r = sid->yn2_r = 0.0;

    sid->v4_state = V4_OFF;
    sid->v4_count = sid->v4_add = 0;

    sid->gn_adr = sid->gn_tone_length = 0;
    sid->gn_volume_add = 0;
    sid->gn_tone_counter = 0;
    sid->gn_base_cycles = sid->gn_loop_cycles = 0;
    sid->gn_last_count = 0;

    sid->sm_adr = sid->sm_end_adr = sid->sm_rep_adr = 0;
    sid->sm_volume = 0;
    sid->sm_rep_count = 0;
    sid->sm_big_endian = false;
}

void SIDReset(uint32_t now)
{
    //SDL_LockAudio();
    osid_reset(sid1);
    osid_reset(sid2);

    memset(work_buffer, 0, sizeof(work_buffer));

    //SDL_UnlockAudio();
}




/*
 *  Clock frequency changed (result of VIC type change)
 */

void SIDClockFreqChanged()
{
    // Compute number of cycles per sample frame
    sid_cycles = cycles_per_second / SAMPLERATE;
    sid_cycles_frac = (float)cycles_per_second / SAMPLERATE;

    // Compute envelope table
    const uint32_t div[16] = {
        9, 32, 63, 95, 149, 220, 267, 313,
        392, 977, 1954, 3126, 3906, 11720, 19531, 31251
    };
    int32_t i;
    for (i=0; i<16; i++)
        eg_table[i] = (sid_cycles << 16) / div[i];

    // Recompute voice_t::add values
    osid_write(sid1, 0, sid1->regs[0], 0, false);
    osid_write(sid1, 7, sid1->regs[7], 0, false);
    osid_write(sid1, 14, sid1->regs[14], 0, false);
    osid_write(sid2, 0, sid2->regs[0], 0, false);
    osid_write(sid2, 7, sid2->regs[7], 0, false);
    osid_write(sid2, 14, sid2->regs[14], 0, false);
}


/*
 *  Set replay frequency
 */

void SIDSetReplayFreq(int32_t freq)
{
    cia_timer = cycles_per_second / freq - 1;
}



/*
 *  Write to CIA timer A (changes replay frequency)
 */

void cia_tl_write(uint8_t byte)
{
    cia_timer = (cia_timer & 0xff00) | byte;
}

void cia_th_write(uint8_t byte)
{
    cia_timer = (cia_timer & 0x00ff) | (byte << 8);
}


/*
 *  Fill audio buffer with SID sound
 */
void calc_sid(osid_t *sid, int32_t *sum_output_left, int32_t *sum_output_right)
{
    // Sampled voice (!! todo: gain/panning)
#if 0    //!!
    uint8_t master_volume = sid->sample_buf[(sample_count >> 16) % SAMPLE_BUF_SIZE];
    sample_count += ((0x138 * 50) << 16) / obtained.freq;
#else
    uint8_t master_volume = sid->volume;
#endif

    int32_t sum_output_filter_left = 0, sum_output_filter_right = 0;

    // Loop for all three voices
    int32_t j;
    for (j=0; j<3; j++) {
        voice_t *v = sid->voice + j;

        // Envelope generator
        uint16_t envelope;

        switch (v->eg_state) {
            case EG_ATTACK:
                v->eg_level += v->a_add;
                if (v->eg_level > 0xffffff) {
                    v->eg_level = 0xffffff;
                    v->eg_state = EG_DECAY;
                }
                break;
            case EG_DECAY:
                if (v->eg_level <= v->s_level || v->eg_level > 0xffffff)
                    v->eg_level = v->s_level;
                else {
                    v->eg_level -= v->d_sub >> eg_dr_shift[v->eg_level >> 16];
                    if (v->eg_level <= v->s_level || v->eg_level > 0xffffff)
                        v->eg_level = v->s_level;
                }
                break;
            case EG_RELEASE:
                v->eg_level -= v->r_sub >> eg_dr_shift[v->eg_level >> 16];
                if (v->eg_level > 0xffffff) {
                    v->eg_level = 0;
                    v->eg_state = EG_IDLE;
                }
                break;
            case EG_IDLE:
                v->eg_level = 0;
                break;
        }
        envelope = (v->eg_level * master_volume) >> 20;

        // Waveform generator
        uint16_t output;

        if (!v->test)
            v->count += v->add;

        if (v->sync && (v->count >= 0x1000000))
            v->mod_to->count = 0;

        v->count &= 0xffffff;

        switch (v->wave) {
            case WAVE_TRI:
                if (v->ring)
                    output = tri_table[(v->count ^ (v->mod_by->count & 0x800000)) >> 11];
                else
                    output = tri_table[v->count >> 11];
                break;
            case WAVE_SAW:
                output = v->count >> 8;
                break;
            case WAVE_RECT:
                if (v->count > (uint32_t)(v->pw << 12))
                    output = 0xffff;
                else
                    output = 0;
                break;
            case WAVE_TRISAW:
                output = tri_saw_table[v->count >> 16];
                break;
            case WAVE_TRIRECT:
                if (v->count > (uint32_t)(v->pw << 12))
                    output = tri_rect_table[v->count >> 16];
                else
                    output = 0;
                break;
            case WAVE_SAWRECT:
                if (v->count > (uint32_t)(v->pw << 12))
                    output = saw_rect_table[v->count >> 16];
                else
                    output = 0;
                break;
            case WAVE_TRISAWRECT:
                if (v->count > (uint32_t)(v->pw << 12))
                    output = tri_saw_rect_table[v->count >> 16];
                else
                    output = 0;
                break;
            case WAVE_NOISE:
                if (v->count >= 0x100000) {
                    output = v->noise = noise_rand() << 8;
                    v->count &= 0xfffff;
                } else
                    output = v->noise;
                break;
            default:
                output = 0x8000;
                break;
        }
        int32_t x = (int16_t)(output ^ 0x8000) * envelope;
        if (v->filter) {
            sum_output_filter_left += (x * v->left_gain) >> 4;
            sum_output_filter_right += (x * v->right_gain) >> 4;
        } else if (!(v->mute)) {
            *sum_output_left += (x * v->left_gain) >> 4;
            *sum_output_right += (x * v->right_gain) >> 4;
        }
    }

    // Galway noise/samples
    int32_t v4_output = 0;
    switch (sid->v4_state) {

        case V4_GALWAY_NOISE:
            v4_output = galway_tab[(sid->gn_volume_add << 6) + (((sid->gn_last_count + sid->v4_count) >> 16) & 0x3f)] << 8;
            sid->v4_count += sid->v4_add;
            if ((sid->v4_count >> 16) >= sid->gn_tone_length) {
                if (sid->gn_tone_counter) {
                    sid->gn_tone_counter--;
                    sid->gn_last_count = sid->v4_count & 0xffff0000;
                    sid->v4_count &= 0xffff;
                    int32_t div = c64_ram[sid->gn_adr + sid->gn_tone_counter] * sid->gn_loop_cycles + sid->gn_base_cycles;
                    if (div == 0)
                        sid->v4_add = 0;
                    else
                        sid->v4_add = sid_cycles * 0x10000 / div;
                } else
                    sid->v4_state = V4_OFF;
            }
            break;

        case V4_SAMPLE: {
            uint8_t sample = c64_ram[sid->sm_adr >> 1];
            if (sid->sm_big_endian)
                if (sid->sm_adr & 1)
                    sample = sample & 0xf;
                else
                    sample = sample >> 4;
            else
                if (sid->sm_adr & 1)
                    sample = sample >> 4;
                else
                    sample = sample & 0xf;
            v4_output = sample_tab[sid->sm_volume * 16 + sample] << 8;
            sid->v4_count += sid->v4_add;
            sid->sm_adr += sid->v4_count >> 16;
            sid->v4_count &= 0xffff;
            if (sid->sm_adr >= sid->sm_end_adr) {
                if (sid->sm_rep_count) {
                    if (sid->sm_rep_count != 0xff)
                        sid->sm_rep_count--;
                    sid->sm_adr = sid->sm_rep_adr;
                } else
                    sid->v4_state = V4_OFF;
            }
            break;
        }
    }
    *sum_output_left += (v4_output * sid->v4_left_gain) >> 4;
    *sum_output_right += (v4_output * sid->v4_right_gain) >> 4;

    if (enable_filters) {
        float xn = (float)sum_output_filter_left * sid->f_ampl;
        float yn = xn + sid->d1 * sid->xn1_l + sid->d2 * sid->xn2_l - sid->g1 * sid->yn1_l - sid->g2 * sid->yn2_l;
        sum_output_filter_left = (int32_t)yn;
        sid->yn2_l = sid->yn1_l; sid->yn1_l = yn; sid->xn2_l = sid->xn1_l; sid->xn1_l = xn;
        xn = (float)sum_output_filter_right * sid->f_ampl;
        yn = xn + sid->d1 * sid->xn1_r + sid->d2 * sid->xn2_r - sid->g1 * sid->yn1_r - sid->g2 * sid->yn2_r;
        sum_output_filter_right = (int32_t)yn;
        sid->yn2_r = sid->yn1_r; sid->yn1_r = yn; sid->xn2_r = sid->xn1_r; sid->xn1_r = xn;
    }

    // Add filtered and non-filtered output
    *sum_output_left += sum_output_filter_left;
    *sum_output_right += sum_output_filter_right;
}



// count ist Anzahl samples für einen Kanal, d.h. pro count werden 4 Bytes in den Puffer geschrieben
void calc_buffer(int16_t *buf, int32_t count)
{
    int16_t * buf16;
    int32_t replay_limit = (SAMPLERATE * 100) / (cycles_per_second / (cia_timer + 1) * speed_adjust); // 882
    int32_t s;

    buf16 = buf;

    // Main calculation loop
    for (s = 0; s < count; s++) {
        int32_t sum_output_left = 0, sum_output_right = 0;


        // Execute 6510 play routine if due
        if (++replay_count >= replay_limit) {
            replay_count = 0;
            UpdatePlayAdr();
            CPUExecute(Sidinfo.uPlayAddr, 0, 0, 0, 1000000);
        }

        // Calculate output of voices from both SIDs
        calc_sid(sid1, &sum_output_left, &sum_output_right);

        // Apply audio effects (post-processing)
        if (audio_effect) {
            sum_output_left >>= 11;
            sum_output_right >>= 11;
            if (audio_effect == 1) {    // Reverb
                sum_output_left += (rev_feedback * work_buffer[wb_read_offset++]) >> 8;
                work_buffer[wb_write_offset++] = sum_output_left;
                sum_output_right += (rev_feedback * work_buffer[wb_read_offset]) >> 8;
                work_buffer[wb_write_offset] = sum_output_right;
            } else {                    // Spatial
                sum_output_left += (rev_feedback * work_buffer[wb_read_offset++]) >> 8;
                work_buffer[wb_write_offset++] = sum_output_left;
                sum_output_right -= (rev_feedback * work_buffer[wb_read_offset]) >> 8;
                work_buffer[wb_write_offset] = sum_output_right;
            }
            wb_read_offset = (wb_read_offset + 1) & (WORK_BUFFER_SIZE - 1);
            wb_write_offset = (wb_write_offset + 1) & (WORK_BUFFER_SIZE - 1);
        } else {
            sum_output_left >>= 10;
            sum_output_right >>= 10;
        }

        // Clip to 16 bits
        if (sum_output_left > 32767)
            sum_output_left = 32767;
        else if (sum_output_left < -32768)
            sum_output_left = -32768;
        if (sum_output_right > 32767)
            sum_output_right = 32767;
        else if (sum_output_right < -32768)
            sum_output_right = -32768;

        // Write to output buffer
        buf16[s * 2] = sum_output_left;
        buf16[s * 2 + 1] = sum_output_right;
    }

}


/*
 *  Calculate IIR filter coefficients
 */
void osid_calc_filter(osid_t *sid)
{
    // Filter off? Then reset all coefficients
    if (sid->f_type == FILT_NONE) {
        sid->f_ampl = 0.0;
        sid->d1 = sid->d2 = sid->g1 = sid->g2 = 0.0;
        return;
    }

    // Calculate resonance frequency
    float fr;
    if (sid->f_type == FILT_LP || sid->f_type == FILT_LPBP)
        fr = ffreq_lp[sid->f_freq];
    else
        fr = ffreq_hp[sid->f_freq];

    // Limit to <1/2 sample frequency, avoid div by 0 in case FILT_NOTCH below
    //filt_t arg = fr / ((float) (obtained.freq >> 1));
    float arg = fr / (float)(SAMPLERATE >> 1);
    if (arg > 0.99)
        arg = 0.99;
    if (arg < 0.01)
        arg = 0.01;

    // Calculate poles (resonance frequency and resonance)
    // The (complex) poles are at
    //   zp_1/2 = (-g1 +/- sqrt(g1^2 - 4*g2)) / 2
    sid->g2 = 0.55 + 1.2 * arg * arg - 1.2 * arg + (float)(sid->f_res) * 0.0133333333;
    sid->g1 = -2.0 * sqrt(sid->g2) * cos(M_PI * arg);

    // Increase resonance if LP/HP combined with BP
    if (sid->f_type == FILT_LPBP || sid->f_type == FILT_HPBP)
        sid->g2 += 0.1;

    // Stabilize filter
    if (fabs(sid->g1) >= sid->g2 + 1.0) {
        if (sid->g1 > 0.0)
            sid->g1 = sid->g2 + 0.99;
        else
            sid->g1 = -(sid->g2 + 0.99);
    }

    // Calculate roots (filter characteristic) and input attenuation
    // The (complex) roots are at
    //   z0_1/2 = (-d1 +/- sqrt(d1^2 - 4*d2)) / 2
    switch (sid->f_type) {

        case FILT_LPBP:
        case FILT_LP:        // Both roots at -1, H(1)=1
            sid->d1 = 2.0; sid->d2 = 1.0;
            sid->f_ampl = 0.25 * (1.0 + sid->g1 + sid->g2);
            break;

        case FILT_HPBP:
        case FILT_HP:        // Both roots at 1, H(-1)=1
            sid->d1 = -2.0; sid->d2 = 1.0;
            sid->f_ampl = 0.25 * (1.0 - sid->g1 + sid->g2);
            break;

        case FILT_BP: {        // Roots at +1 and -1, H_max=1
            sid->d1 = 0.0; sid->d2 = -1.0;
            float c = sqrt(sid->g2*sid->g2 + 2.0*sid->g2 - sid->g1*sid->g1 + 1.0);
            sid->f_ampl = 0.25 * (-2.0*sid->g2*sid->g2 - (4.0+2.0*c)*sid->g2 - 2.0*c + (c+2.0)*sid->g1*sid->g1 - 2.0) / (-sid->g2*sid->g2 - (c+2.0)*sid->g2 - c + sid->g1*sid->g1 - 1.0);
            break;
        }

        case FILT_NOTCH:    // Roots at exp(i*pi*arg) and exp(-i*pi*arg), H(1)=1 (arg>=0.5) or H(-1)=1 (arg<0.5)
            sid->d1 = -2.0 * cos(M_PI * arg); sid->d2 = 1.0;
            if (arg >= 0.5)
                sid->f_ampl = 0.5 * (1.0 + sid->g1 + sid->g2) / (1.0 - cos(M_PI * arg));
            else
                sid->f_ampl = 0.5 * (1.0 - sid->g1 + sid->g2) / (1.0 + cos(M_PI * arg));
            break;

        // The following is pure guesswork...
        case FILT_ALL:        // Roots at 2*exp(i*pi*arg) and 2*exp(-i*pi*arg), H(-1)=1 (arg>=0.5) or H(1)=1 (arg<0.5)
            sid->d1 = -4.0 * cos(M_PI * arg); sid->d2 = 4.0;

            if (arg >= 0.5)
                sid->f_ampl = (1.0 - sid->g1 + sid->g2) / (5.0 + 4.0 * cos(M_PI * arg));
            else
                sid->f_ampl = (1.0 + sid->g1 + sid->g2) / (5.0 - 4.0 * cos(M_PI * arg));
            break;
        default:
            break;
    }
}


// Calculate gain values for all voices
void osid_calc_gain_voice(int32_t volume, int32_t panning, uint16_t *left_gain, uint16_t *right_gain)
{
    int32_t gain;
    if (panning < -0x100)
        panning = -0x100;
    if (panning > 0x100)
        panning = 0x100;
    gain = (volume * (-panning + 0x100) * master_volume) >> 20;
    if (gain > 0x200)
        gain = 0x200;
    if (gain < 0)
        gain = 0;
    *left_gain = gain;
    gain = (volume * (panning + 0x100) * master_volume) >> 20;
    if (gain > 0x200)
        gain = 0x200;
    if (gain < 0)
        gain = 0;
    *right_gain = gain;
}

void osid_calc_gains(osid_t *sid, bool is_left_sid, bool is_right_sid)
{
    int32_t pan_offset = 0;
    if (is_left_sid)
        pan_offset = -dual_sep;
    else if (is_right_sid)
        pan_offset = dual_sep;
    osid_calc_gain_voice(v1_volume, v1_panning + pan_offset, &sid->voice[0].left_gain, &sid->voice[0].right_gain);
    osid_calc_gain_voice(v2_volume, v2_panning + pan_offset, &sid->voice[1].left_gain, &sid->voice[1].right_gain);
    osid_calc_gain_voice(v3_volume, v3_panning + pan_offset, &sid->voice[2].left_gain, &sid->voice[2].right_gain);
    osid_calc_gain_voice(v4_volume, v4_panning + pan_offset, &sid->v4_left_gain, &sid->v4_right_gain);
}


/*
 *  Read from SID register
 */

uint32_t osid_read(osid_t *sid, uint32_t adr, uint32_t now)
{
    switch (adr) {
        case 0x19:    // A/D converters
        case 0x1a:
            sid->last_written_byte = 0;
            return 0xff;
        case 0x1b:    // Voice 3 oscillator/EG readout
        case 0x1c:
            sid->last_written_byte = 0;
            return f_rand();
        default: {    // Write-only register: return last value written to SID
            uint8_t ret = sid->last_written_byte;
            sid->last_written_byte = 0;
            return ret;
        }
    }
}

uint32_t sid_read(uint32_t adr, uint32_t now)
{
    return osid_read(sid1, adr & 0x7f, now);
}


// Write to SID register
void osid_write(osid_t *sid, uint32_t adr, uint32_t byte, uint32_t now, bool rmw)
{
    // Writing to standard SID mirrored registers
    if ((adr & 0x1f) < 0x1d)
        adr &= 0x1f;

    sid->last_written_byte = sid->regs[adr] = byte;
    int32_t v = adr/7;    // Voice number

    switch (adr) {
        case 0:
        case 7:
        case 14:
            sid->voice[v].freq = (sid->voice[v].freq & 0xff00) | byte;
            sid->voice[v].add = (uint32_t) ((float) sid->voice[v].freq) * sid_cycles_frac;
            break;

        case 1:
        case 8:
        case 15:
            sid->voice[v].freq = (sid->voice[v].freq & 0xff) | (byte << 8);
            sid->voice[v].add = (uint32_t) ((float) sid->voice[v].freq) * sid_cycles_frac;
            break;

        case 2:
        case 9:
        case 16:
            sid->voice[v].pw = (sid->voice[v].pw & 0x0f00) | byte;
            break;

        case 3:
        case 10:
        case 17:
            sid->voice[v].pw = (sid->voice[v].pw & 0xff) | ((byte & 0xf) << 8);
            break;

        case 4:
        case 11:
        case 18:
            sid->voice[v].wave = (byte >> 4) & 0xf;
            if ((byte & 1) != sid->voice[v].gate) {
                if (byte & 1)    // Gate turned on
                    sid->voice[v].eg_state = EG_ATTACK;
                else            // Gate turned off
                    if (sid->voice[v].eg_state != EG_IDLE)
                        sid->voice[v].eg_state = EG_RELEASE;
            }
            sid->voice[v].gate = byte & 1;
            sid->voice[v].mod_by->sync = byte & 2;
            sid->voice[v].ring = byte & 4;
            if ((sid->voice[v].test = byte & 8))
                sid->voice[v].count = 0;
            break;

        case 5:
        case 12:
        case 19:
            sid->voice[v].a_add = eg_table[byte >> 4];
            sid->voice[v].d_sub = eg_table[byte & 0xf];
            break;

        case 6:
        case 13:
        case 20:
            sid->voice[v].s_level = (byte >> 4) * 0x111111;
            sid->voice[v].r_sub = eg_table[byte & 0xf];
            break;

        case 22:
            if (byte != sid->f_freq) {
                sid->f_freq = byte;
                if (enable_filters)
                    osid_calc_filter(sid);
            }
            break;

        case 23:
            sid->voice[0].filter = byte & 1;
            sid->voice[1].filter = byte & 2;
            sid->voice[2].filter = byte & 4;
            if ((byte >> 4) != sid->f_res) {
                sid->f_res = byte >> 4;
                if (enable_filters)
                    osid_calc_filter(sid);
            }
            break;

        case 24:
            sid->volume = byte & 0xf;
            sid->voice[2].mute = byte & 0x80;
            if (((byte >> 4) & 7) != sid->f_type) {
                sid->f_type = (byte >> 4) & 7;
                sid->xn1_l = sid->xn2_l = sid->yn1_l = sid->yn2_l = 0.0;
                sid->xn1_r = sid->xn2_r = sid->yn1_r = sid->yn2_r = 0.0;
                if (enable_filters)
                    osid_calc_filter(sid);
            }
            break;

        case 29:
            if (byte) {
                if (byte < 0xfc) {            // Galway noise
                    sid->gn_adr = (sid->regs[0x1f] << 8) | sid->regs[0x1e];
                    sid->gn_tone_length = sid->regs[0x3d];
                    sid->gn_volume_add = sid->regs[0x3e] & 15;
                    sid->gn_tone_counter = byte;
                    sid->gn_base_cycles = sid->regs[0x5d];
                    sid->gn_loop_cycles = sid->regs[0x3f];
                    sid->gn_last_count = 0;
                    sid->v4_count = 0;
                    int32_t div = c64_ram[sid->gn_adr + sid->gn_tone_counter] * sid->gn_loop_cycles + sid->gn_base_cycles;
                    if (div == 0)
                        sid->v4_add = 0;
                    else
                        sid->v4_add = sid_cycles * 0x10000 / div;
                    sid->v4_state = V4_GALWAY_NOISE;

                } else if (byte == 0xfd) {    // Sample off
                    sid->v4_state = V4_OFF;

                } else {                    // Sample on
                    sid->sm_adr = ((sid->regs[0x1f] << 8) | sid->regs[0x1e]) << 1;
                    sid->sm_end_adr = ((sid->regs[0x3e] << 8) | sid->regs[0x3d]) << 1;
                    sid->sm_rep_adr = ((sid->regs[0x7f] << 8) | sid->regs[0x7e]) << 1;
                    sid->sm_rep_count = sid->regs[0x3f];
                    sid->sm_big_endian = sid->regs[0x7d];
                    switch (byte) {
                        case 0xfc:
                            sid->sm_volume = 2;
                            break;
                        case 0xfe:
                            sid->sm_volume = 1;
                            break;
                        case 0xff:
                            sid->sm_volume = 0;
                            break;
                    };
                    int32_t div = (sid->regs[0x5e] << 8) | sid->regs[0x5d];
                    if (sid->regs[0x5f])
                        div >>= sid->regs[0x5f];
                    if (div == 0) {
                        sid->v4_state = V4_OFF;
                    } else {
                        sid->v4_count = 0;
                        sid->v4_add = sid_cycles * 0x10000 / div;
                        sid->v4_state = V4_SAMPLE;
                    }
                }
            }
            break;
    }
}

void sid_write(uint32_t adr, uint32_t byte, uint32_t now, bool rmw)
{
    osid_write(sid1, adr & 0x7f, byte, now, rmw);
}
