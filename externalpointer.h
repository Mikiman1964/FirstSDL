#ifndef EXTERNALPOINTER_H_INCLUDED
#define EXTERNALPOINTER_H_INCLUDED

// Um z.B. Grafiken einzubinden, können Objektdateien mit hinzugelinkt werden.
// Mit objcopy (siehe Compiler-Verzeichnis mingw) kann aus einer Binärdatei (z.B. Bitmap) eine Objektdatei erzeugt werden.
// Beispiel: Aus der Bitmap "Sand.bmp" die Objektdatei "Sand.o" erzeugen.
// objcopy --input-target binary --output-target pe-x86-64 --binary-architecture i386 Sand.bmp Sand.o

// Für die extern eingebundenen Objektfiles gibt es hier die Pointer auf die Grafiken
extern uint8_t _binary_0_bmp_start;extern uint8_t _binary_0_bmp_end;
extern uint8_t _binary_1_bmp_start;extern uint8_t _binary_1_bmp_end;
extern uint8_t _binary_2_bmp_start;extern uint8_t _binary_2_bmp_end;
extern uint8_t _binary_3_bmp_start;extern uint8_t _binary_3_bmp_end;
extern uint8_t _binary_4_bmp_start;extern uint8_t _binary_4_bmp_end;
extern uint8_t _binary_5_bmp_start;extern uint8_t _binary_5_bmp_end;
extern uint8_t _binary_6_bmp_start;extern uint8_t _binary_6_bmp_end;
extern uint8_t _binary_7_bmp_start;extern uint8_t _binary_7_bmp_end;
extern uint8_t _binary_8_bmp_start;extern uint8_t _binary_8_bmp_end;
extern uint8_t _binary_9_bmp_start;extern uint8_t _binary_9_bmp_end;
extern uint8_t _binary_0_green_bmp_start;extern uint8_t _binary_0_green_bmp_end;
extern uint8_t _binary_1_green_bmp_start;extern uint8_t _binary_1_green_bmp_end;
extern uint8_t _binary_2_green_bmp_start;extern uint8_t _binary_2_green_bmp_end;
extern uint8_t _binary_3_green_bmp_start;extern uint8_t _binary_3_green_bmp_end;
extern uint8_t _binary_4_green_bmp_start;extern uint8_t _binary_4_green_bmp_end;
extern uint8_t _binary_5_green_bmp_start;extern uint8_t _binary_5_green_bmp_end;
extern uint8_t _binary_6_green_bmp_start;extern uint8_t _binary_6_green_bmp_end;
extern uint8_t _binary_7_green_bmp_start;extern uint8_t _binary_7_green_bmp_end;
extern uint8_t _binary_8_green_bmp_start;extern uint8_t _binary_8_green_bmp_end;
extern uint8_t _binary_9_green_bmp_start;extern uint8_t _binary_9_green_bmp_end;

extern uint8_t _binary_A_bmp_start;extern uint8_t _binary_A_bmp_end;
extern uint8_t _binary_B_bmp_start;extern uint8_t _binary_B_bmp_end;
extern uint8_t _binary_C_bmp_start;extern uint8_t _binary_C_bmp_end;
extern uint8_t _binary_D_bmp_start;extern uint8_t _binary_D_bmp_end;
extern uint8_t _binary_E_bmp_start;extern uint8_t _binary_E_bmp_end;
extern uint8_t _binary_F_bmp_start;extern uint8_t _binary_F_bmp_end;
extern uint8_t _binary_G_bmp_start;extern uint8_t _binary_G_bmp_end;
extern uint8_t _binary_H_bmp_start;extern uint8_t _binary_H_bmp_end;
extern uint8_t _binary_I_bmp_start;extern uint8_t _binary_I_bmp_end;
extern uint8_t _binary_J_bmp_start;extern uint8_t _binary_J_bmp_end;
extern uint8_t _binary_K_bmp_start;extern uint8_t _binary_K_bmp_end;
extern uint8_t _binary_L_bmp_start;extern uint8_t _binary_L_bmp_end;
extern uint8_t _binary_M_bmp_start;extern uint8_t _binary_M_bmp_end;
extern uint8_t _binary_N_bmp_start;extern uint8_t _binary_N_bmp_end;
extern uint8_t _binary_O_bmp_start;extern uint8_t _binary_O_bmp_end;
extern uint8_t _binary_P_bmp_start;extern uint8_t _binary_P_bmp_end;
extern uint8_t _binary_Q_bmp_start;extern uint8_t _binary_Q_bmp_end;
extern uint8_t _binary_R_bmp_start;extern uint8_t _binary_R_bmp_end;
extern uint8_t _binary_S_bmp_start;extern uint8_t _binary_S_bmp_end;
extern uint8_t _binary_T_bmp_start;extern uint8_t _binary_T_bmp_end;
extern uint8_t _binary_U_bmp_start;extern uint8_t _binary_U_bmp_end;
extern uint8_t _binary_V_bmp_start;extern uint8_t _binary_V_bmp_end;
extern uint8_t _binary_W_bmp_start;extern uint8_t _binary_W_bmp_end;
extern uint8_t _binary_X_bmp_start;extern uint8_t _binary_X_bmp_end;
extern uint8_t _binary_Y_bmp_start;extern uint8_t _binary_Y_bmp_end;
extern uint8_t _binary_Z_bmp_start;extern uint8_t _binary_Z_bmp_end;

extern uint8_t _binary_AE_bmp_start;extern uint8_t _binary_AE_bmp_end;
extern uint8_t _binary_Apostroph_bmp_start;extern uint8_t _binary_Apostroph_bmp_end;
extern uint8_t _binary_Ausrufungszeichen_bmp_start;extern uint8_t _binary_Ausrufungszeichen_bmp_end;
extern uint8_t _binary_Copyright_bmp_start;extern uint8_t _binary_Copyright_bmp_end;
extern uint8_t _binary_Doppelpunkt_bmp_start;extern uint8_t _binary_Doppelpunkt_bmp_end;
extern uint8_t _binary_Fragezeichen_bmp_start;extern uint8_t _binary_Fragezeichen_bmp_end;
extern uint8_t _binary_KlammerAuf_bmp_start;extern uint8_t _binary_KlammerAuf_bmp_end;
extern uint8_t _binary_KlammerZu_bmp_start;extern uint8_t _binary_KlammerZu_bmp_end;
extern uint8_t _binary_Minus_bmp_start;extern uint8_t _binary_Minus_bmp_end;
extern uint8_t _binary_PfeilLinks_bmp_start;extern uint8_t _binary_PfeilLinks_bmp_end;
extern uint8_t _binary_PfeilRechts_bmp_start;extern uint8_t _binary_PfeilRechts_bmp_end;
extern uint8_t _binary_PfeilHoch_bmp_start;extern uint8_t _binary_PfeilHoch_bmp_end;
extern uint8_t _binary_PfeilRunter_bmp_start;extern uint8_t _binary_PfeilRunter_bmp_end;
extern uint8_t _binary_Plus_bmp_start;extern uint8_t _binary_Plus_bmp_end;
extern uint8_t _binary_Punkt_bmp_start;extern uint8_t _binary_Punkt_bmp_end;
extern uint8_t _binary_Space_bmp_start;extern uint8_t _binary_Space_bmp_end;
extern uint8_t _binary_Teller_bmp_start;extern uint8_t _binary_Teller_bmp_end;
extern uint8_t _binary_UE_bmp_start;extern uint8_t _binary_UE_bmp_end;
extern uint8_t _binary_OE_bmp_start;extern uint8_t _binary_OE_bmp_end;
extern uint8_t _binary_Komma_bmp_start;extern uint8_t _binary_Komma_bmp_end;
extern uint8_t _binary_Schraegstrich_bmp_start;extern uint8_t _binary_Schraegstrich_bmp_end;

extern uint8_t _binary_FreeTransparentMulticolor_bmp_start;extern uint8_t _binary_FreeTransparentMulticolor_bmp_end;
extern uint8_t _binary_Asteroid_bmp_start;extern uint8_t _binary_Asteroid_bmp_end;
extern uint8_t _binary_FD_bmp_start;extern uint8_t _binary_FD_bmp_end;
// extern uint8_t _binary_Sand_bmp_start;extern uint8_t _binary_Sand_bmp_end;   // kann ersetzt werden
extern uint8_t _binary_Stahl_bmp_start;extern uint8_t _binary_Stahl_bmp_end;
extern uint8_t _binary_MineLinks_bmp_start;extern uint8_t _binary_MineLinks_bmp_end;
extern uint8_t _binary_MineLinksAn_bmp_start;extern uint8_t _binary_MineLinksAn_bmp_end;
extern uint8_t _binary_BeetleLinks01_bmp_start;extern uint8_t _binary_BeetleLinks01_bmp_end;
extern uint8_t _binary_BeetleLinks02_bmp_start;extern uint8_t _binary_BeetleLinks02_bmp_end;
extern uint8_t _binary_BeetleLinks03_bmp_start;extern uint8_t _binary_BeetleLinks03_bmp_end;
extern uint8_t _binary_BeetleLinks04_bmp_start;extern uint8_t _binary_BeetleLinks04_bmp_end;
extern uint8_t _binary_BeetleLinks05_bmp_start;extern uint8_t _binary_BeetleLinks05_bmp_end;
extern uint8_t _binary_BeetleLinks06_bmp_start;extern uint8_t _binary_BeetleLinks06_bmp_end;
extern uint8_t _binary_BeetleLinks07_bmp_start;extern uint8_t _binary_BeetleLinks07_bmp_end;
extern uint8_t _binary_BeetleLinks08_bmp_start;extern uint8_t _binary_BeetleLinks08_bmp_end;
extern uint8_t _binary_Panel_bmp_start;extern uint8_t _binary_Panel_bmp_end;
extern uint8_t _binary_door_red_bmp_start;extern uint8_t _binary_door_red_bmp_end;
extern uint8_t _binary_door_green_bmp_start;extern uint8_t _binary_door_green_bmp_end;
extern uint8_t _binary_door_blue_bmp_start;extern uint8_t _binary_door_blue_bmp_end;
extern uint8_t _binary_door_yellow_bmp_start;extern uint8_t _binary_door_yellow_bmp_end;
extern uint8_t _binary_key_red_bmp_start;extern uint8_t _binary_key_red_bmp_end;
extern uint8_t _binary_key_green_bmp_start;extern uint8_t _binary_key_green_bmp_end;
extern uint8_t _binary_key_blue_bmp_start;extern uint8_t _binary_key_blue_bmp_end;
extern uint8_t _binary_key_yellow_bmp_start;extern uint8_t _binary_key_yellow_bmp_end;
extern uint8_t _binary_man_bmp_start;extern uint8_t _binary_man_bmp_end;
extern uint8_t _binary_ManLinks01_bmp_start;extern uint8_t _binary_ManLinks01_bmp_end;
extern uint8_t _binary_ManLinks02_bmp_start;extern uint8_t _binary_ManLinks02_bmp_end;
extern uint8_t _binary_ManLinks03_bmp_start;extern uint8_t _binary_ManLinks03_bmp_end;
extern uint8_t _binary_ManLinks04_bmp_start;extern uint8_t _binary_ManLinks04_bmp_end;
extern uint8_t _binary_ManLinks05_bmp_start;extern uint8_t _binary_ManLinks05_bmp_end;
extern uint8_t _binary_ManLinks06_bmp_start;extern uint8_t _binary_ManLinks06_bmp_end;
extern uint8_t _binary_ManLinks07_bmp_start;extern uint8_t _binary_ManLinks07_bmp_end;
extern uint8_t _binary_ManLinks08_bmp_start;extern uint8_t _binary_ManLinks08_bmp_end;
extern uint8_t _binary_ManHoch01_bmp_start;extern uint8_t _binary_ManHoch01_bmp_end;
extern uint8_t _binary_ManHoch02_bmp_start;extern uint8_t _binary_ManHoch02_bmp_end;
extern uint8_t _binary_ManHoch03_bmp_start;extern uint8_t _binary_ManHoch03_bmp_end;
extern uint8_t _binary_ManHoch04_bmp_start;extern uint8_t _binary_ManHoch04_bmp_end;
extern uint8_t _binary_ManHoch05_bmp_start;extern uint8_t _binary_ManHoch05_bmp_end;
extern uint8_t _binary_ManHoch06_bmp_start;extern uint8_t _binary_ManHoch06_bmp_end;
extern uint8_t _binary_ManHoch07_bmp_start;extern uint8_t _binary_ManHoch07_bmp_end;
extern uint8_t _binary_ManHoch08_bmp_start;extern uint8_t _binary_ManHoch08_bmp_end;
extern uint8_t _binary_ManRunter01_bmp_start;extern uint8_t _binary_ManRunter01_bmp_end;
extern uint8_t _binary_ManRunter02_bmp_start;extern uint8_t _binary_ManRunter02_bmp_end;
extern uint8_t _binary_ManRunter03_bmp_start;extern uint8_t _binary_ManRunter03_bmp_end;
extern uint8_t _binary_ManRunter04_bmp_start;extern uint8_t _binary_ManRunter04_bmp_end;
extern uint8_t _binary_ManRunter05_bmp_start;extern uint8_t _binary_ManRunter05_bmp_end;
extern uint8_t _binary_ManRunter06_bmp_start;extern uint8_t _binary_ManRunter06_bmp_end;
extern uint8_t _binary_ManRunter07_bmp_start;extern uint8_t _binary_ManRunter07_bmp_end;
extern uint8_t _binary_ManRunter08_bmp_start;extern uint8_t _binary_ManRunter08_bmp_end;
extern uint8_t _binary_AlienStand01_bmp_start;extern uint8_t _binary_AlienStand01_bmp_end;
extern uint8_t _binary_AlienStand02_bmp_start;extern uint8_t _binary_AlienStand02_bmp_end;
extern uint8_t _binary_AlienStand03_bmp_start;extern uint8_t _binary_AlienStand03_bmp_end;
extern uint8_t _binary_AlienStand04_bmp_start;extern uint8_t _binary_AlienStand04_bmp_end;
extern uint8_t _binary_AlienStand05_bmp_start;extern uint8_t _binary_AlienStand05_bmp_end;
extern uint8_t _binary_AlienStand06_bmp_start;extern uint8_t _binary_AlienStand06_bmp_end;
extern uint8_t _binary_AlienStand07_bmp_start;extern uint8_t _binary_AlienStand07_bmp_end;
extern uint8_t _binary_AlienStand08_bmp_start;extern uint8_t _binary_AlienStand08_bmp_end;
extern uint8_t _binary_AlienGo01_bmp_start;extern uint8_t _binary_AlienGo01_bmp_end;
extern uint8_t _binary_AlienGo02_bmp_start;extern uint8_t _binary_AlienGo02_bmp_end;
extern uint8_t _binary_Wheel_bmp_start;extern uint8_t _binary_Wheel_bmp_end;
extern uint8_t _binary_ReplikatorRotAus_ObenLinks_bmp_start;extern uint8_t _binary_ReplikatorRotAus_ObenLinks_bmp_end;
extern uint8_t _binary_ReplikatorRotAus_ObenMitte_bmp_start;extern uint8_t _binary_ReplikatorRotAus_ObenMitte_bmp_end;
extern uint8_t _binary_ReplikatorRotAus_ObenRechts_bmp_start;extern uint8_t _binary_ReplikatorRotAus_ObenRechts_bmp_end;
extern uint8_t _binary_ReplikatorRotAus_UntenLinks_bmp_start;extern uint8_t _binary_ReplikatorRotAus_UntenLinks_bmp_end;
extern uint8_t _binary_ReplikatorRotAus_UntenRechts_bmp_start;extern uint8_t _binary_ReplikatorRotAus_UntenRechts_bmp_end;
extern uint8_t _binary_ReplikatorRotAn01_ObenLinks_bmp_start;extern uint8_t _binary_ReplikatorRotAn01_ObenLinks_bmp_end;
extern uint8_t _binary_ReplikatorRotAn01_ObenMitte_bmp_start;extern uint8_t _binary_ReplikatorRotAn01_ObenMitte_bmp_end;
extern uint8_t _binary_ReplikatorRotAn01_ObenRechts_bmp_start;extern uint8_t _binary_ReplikatorRotAn01_ObenRechts_bmp_end;
extern uint8_t _binary_ReplikatorRotAn02_ObenLinks_bmp_start;extern uint8_t _binary_ReplikatorRotAn02_ObenLinks_bmp_end;
extern uint8_t _binary_ReplikatorRotAn02_ObenMitte_bmp_start;extern uint8_t _binary_ReplikatorRotAn02_ObenMitte_bmp_end;
extern uint8_t _binary_ReplikatorRotAn02_ObenRechts_bmp_start;extern uint8_t _binary_ReplikatorRotAn02_ObenRechts_bmp_end;
extern uint8_t _binary_ReplikatorRotAn03_ObenLinks_bmp_start;extern uint8_t _binary_ReplikatorRotAn03_ObenLinks_bmp_end;
extern uint8_t _binary_ReplikatorRotAn03_ObenMitte_bmp_start;extern uint8_t _binary_ReplikatorRotAn03_ObenMitte_bmp_end;
extern uint8_t _binary_ReplikatorRotAn03_ObenRechts_bmp_start;extern uint8_t _binary_ReplikatorRotAn03_ObenRechts_bmp_end;
extern uint8_t _binary_ReplikatorRotAn_UntenLinks_bmp_start;extern uint8_t _binary_ReplikatorRotAn_UntenLinks_bmp_end;
extern uint8_t _binary_ReplikatorRotAn_UntenRechts_bmp_start;extern uint8_t _binary_ReplikatorRotAn_UntenRechts_bmp_end;
extern uint8_t _binary_ReplikatorSchalterRotAus_bmp_start;extern uint8_t _binary_ReplikatorSchalterRotAus_bmp_end;
extern uint8_t _binary_ReplikatorSchalterRotAn_bmp_start;extern uint8_t _binary_ReplikatorSchalterRotAn_bmp_end;
extern uint8_t _binary_Sand_bmp_start;extern uint8_t _binary_Sand_bmp_end;                                                  // Sand (komplett eingeschlossen)
extern uint8_t _binary_Sand_links_bmp_start;extern uint8_t _binary_Sand_links_bmp_end;                                      // Sand (links frei)
extern uint8_t _binary_Sand_rechts_bmp_start;extern uint8_t _binary_Sand_rechts_bmp_end;                                    // Sand (rechts frei)
extern uint8_t _binary_Sand_oben_bmp_start;extern uint8_t _binary_Sand_oben_bmp_end;                                        // Sand (oben frei)
extern uint8_t _binary_Sand_unten_bmp_start;extern uint8_t _binary_Sand_unten_bmp_end;                                      // Sand (unten frei)
extern uint8_t _binary_Sand_links_oben_bmp_start;extern uint8_t _binary_Sand_links_oben_bmp_end;                            // Sand (links/oben frei)
extern uint8_t _binary_Sand_rechts_oben_bmp_start;extern uint8_t _binary_Sand_rechts_oben_bmp_end;                          // Sand (rechts/oben frei)
extern uint8_t _binary_Sand_rechts_unten_bmp_start;extern uint8_t _binary_Sand_rechts_unten_bmp_end;                        // Sand (rechts/unten frei)
extern uint8_t _binary_Sand_links_unten_bmp_start;extern uint8_t _binary_Sand_links_unten_bmp_end;                          // Sand (links/unten frei)
extern uint8_t _binary_Sand_links_rechts_bmp_start;extern uint8_t _binary_Sand_links_rechts_bmp_end;                        // Sand (links/rechts frei)
extern uint8_t _binary_Sand_oben_unten_bmp_start;extern uint8_t _binary_Sand_oben_unten_bmp_end;                            // Sand (oben/unten frei)
extern uint8_t _binary_Sand_links_rechts_oben_bmp_start;extern uint8_t _binary_Sand_links_rechts_oben_bmp_end;              // Sand (links/rechts/oben frei)
extern uint8_t _binary_Sand_rechts_oben_unten_bmp_start;extern uint8_t _binary_Sand_rechts_oben_unten_bmp_end;              // Sand (rechts/oben/unten frei)
extern uint8_t _binary_Sand_links_rechts_unten_bmp_start;extern uint8_t _binary_Sand_links_rechts_unten_bmp_end;            // Sand (links/rechts/unten frei)
extern uint8_t _binary_Sand_links_oben_unten_bmp_start;extern uint8_t _binary_Sand_links_oben_unten_bmp_end;                // Sand (links/oben/unten frei)
extern uint8_t _binary_Sand_links_rechts_oben_unten_bmp_start;extern uint8_t _binary_Sand_links_rechts_oben_unten_bmp_end;  // Sand (links/rechts/oben/unten frei)


// Grafiken
// Für jede Grafik gibt es einen Start- und End-Pointer, um die Größe zu berechnen.
uint8_t* g_pGfxPointer[] = {
// Index
/*0000*/    &_binary_Space_bmp_start,&_binary_Space_bmp_end,// ASCII-032, Space
/*0001*/    &_binary_Ausrufungszeichen_bmp_start,&_binary_Ausrufungszeichen_bmp_end,// ASCII-033, !
/*0002*/    &_binary_man_bmp_start,&_binary_man_bmp_end,// ASCII-034, " -> Space
/*0003*/    &_binary_Stahl_bmp_start,&_binary_Stahl_bmp_end,// ASCII-035, # -> Stahl
/*0004*/    &_binary_PfeilRechts_bmp_start,&_binary_PfeilRechts_bmp_end,// ASCII-036, $ -> Pfeil rechts
/*0005*/    &_binary_PfeilHoch_bmp_start,&_binary_PfeilHoch_bmp_end,// ASCII-037, % -> Pfeil hoch
/*0006*/    &_binary_PfeilRunter_bmp_start,&_binary_PfeilRunter_bmp_end,// ASCII-038, & -> Pfeil runter
/*0007*/    &_binary_Apostroph_bmp_start,&_binary_Apostroph_bmp_end,// ASCII-039, '
/*0008*/    &_binary_KlammerAuf_bmp_start,&_binary_KlammerAuf_bmp_end,// ASCII-040, (
/*0009*/    &_binary_KlammerZu_bmp_start,&_binary_KlammerZu_bmp_end,// ASCII-041, )
/*0010*/    &_binary_Copyright_bmp_start,&_binary_Copyright_bmp_end,// ASCII-042, * -> Copyright
/*0011*/    &_binary_Plus_bmp_start,&_binary_Plus_bmp_end,// ASCII-043, +
/*0012*/    &_binary_Komma_bmp_start,&_binary_Komma_bmp_end,// ASCII-044, ,
/*0013*/    &_binary_Minus_bmp_start,&_binary_Minus_bmp_end,// ASCII-045, -
/*0014*/    &_binary_Punkt_bmp_start,&_binary_Punkt_bmp_end,// ASCII-046, .
/*0015*/    &_binary_Schraegstrich_bmp_start,&_binary_Schraegstrich_bmp_end,// ASCII-047, /
/*0016*/    &_binary_0_bmp_start,&_binary_0_bmp_end,// ASCII-048, 0
/*0017*/    &_binary_1_bmp_start,&_binary_1_bmp_end,// ASCII-049, 1
/*0018*/    &_binary_2_bmp_start,&_binary_2_bmp_end,// ASCII-050, 2
/*0019*/    &_binary_3_bmp_start,&_binary_3_bmp_end,// ASCII-051, 3
/*0020*/    &_binary_4_bmp_start,&_binary_4_bmp_end,// ASCII-052, 4
/*0021*/    &_binary_5_bmp_start,&_binary_5_bmp_end,// ASCII-053, 5
/*0022*/    &_binary_6_bmp_start,&_binary_6_bmp_end,// ASCII-054, 6
/*0023*/    &_binary_7_bmp_start,&_binary_7_bmp_end,// ASCII-055, 7
/*0024*/    &_binary_8_bmp_start,&_binary_8_bmp_end,// ASCII-056, 8
/*0025*/    &_binary_9_bmp_start,&_binary_9_bmp_end,// ASCII-057, 9
/*0026*/    &_binary_Doppelpunkt_bmp_start,&_binary_Doppelpunkt_bmp_end,// ASCII-058, :
/*0027*/    &_binary_Teller_bmp_start,&_binary_Teller_bmp_end,// ASCII-059, ; -> Teller
/*0028*/    &_binary_PfeilLinks_bmp_start,&_binary_PfeilLinks_bmp_end,// ASCII-060, < -> Pfeil links
/*0029*/    &_binary_Space_bmp_start,&_binary_Space_bmp_end,// ASCII-061, = -> Space
/*0030*/    &_binary_PfeilRechts_bmp_start,&_binary_PfeilRechts_bmp_end,// ASCII-062, > -> Pfeil rechts
/*0031*/    &_binary_Fragezeichen_bmp_start,&_binary_Fragezeichen_bmp_end,// ASCII-063, ?
/*0032*/    &_binary_Copyright_bmp_start,&_binary_Copyright_bmp_end,// ASCII-064, @ -> Copyright
/*0033*/    &_binary_A_bmp_start,&_binary_A_bmp_end,// ASCII-065, A
/*0034*/    &_binary_B_bmp_start,&_binary_B_bmp_end,// ASCII-066, B
/*0035*/    &_binary_C_bmp_start,&_binary_C_bmp_end,// ASCII-067, C
/*0036*/    &_binary_D_bmp_start,&_binary_D_bmp_end,// ASCII-068, D
/*0037*/    &_binary_E_bmp_start,&_binary_E_bmp_end,// ASCII-069, E
/*0038*/    &_binary_F_bmp_start,&_binary_F_bmp_end,// ASCII-070, F
/*0039*/    &_binary_G_bmp_start,&_binary_G_bmp_end,// ASCII-071, G
/*0040*/    &_binary_H_bmp_start,&_binary_H_bmp_end,// ASCII-072, H
/*0041*/    &_binary_I_bmp_start,&_binary_I_bmp_end,// ASCII-073, I
/*0042*/    &_binary_J_bmp_start,&_binary_J_bmp_end,// ASCII-074, J
/*0043*/    &_binary_K_bmp_start,&_binary_K_bmp_end,// ASCII-075, K
/*0044*/    &_binary_L_bmp_start,&_binary_L_bmp_end,// ASCII-076, L
/*0045*/    &_binary_M_bmp_start,&_binary_M_bmp_end,// ASCII-077, M
/*0046*/    &_binary_N_bmp_start,&_binary_N_bmp_end,// ASCII-078, N
/*0047*/    &_binary_O_bmp_start,&_binary_O_bmp_end,// ASCII-079, O
/*0048*/    &_binary_P_bmp_start,&_binary_P_bmp_end,// ASCII-080, P
/*0049*/    &_binary_Q_bmp_start,&_binary_Q_bmp_end,// ASCII-081, Q
/*0050*/    &_binary_R_bmp_start,&_binary_R_bmp_end,// ASCII-082, R
/*0051*/    &_binary_S_bmp_start,&_binary_S_bmp_end,// ASCII-083, S
/*0052*/    &_binary_T_bmp_start,&_binary_T_bmp_end,// ASCII-084, T
/*0053*/    &_binary_U_bmp_start,&_binary_U_bmp_end,// ASCII-085, U
/*0054*/    &_binary_V_bmp_start,&_binary_V_bmp_end,// ASCII-086, V
/*0055*/    &_binary_W_bmp_start,&_binary_W_bmp_end,// ASCII-087, W
/*0056*/    &_binary_X_bmp_start,&_binary_X_bmp_end,// ASCII-088, X
/*0057*/    &_binary_Y_bmp_start,&_binary_Y_bmp_end,// ASCII-089, Y
/*0058*/    &_binary_Z_bmp_start,&_binary_Z_bmp_end,// ASCII-090, Z
/*0059*/    &_binary_KlammerAuf_bmp_start,&_binary_KlammerAuf_bmp_end,      // ASCII-091, [ -> (
/*0060*/    &_binary_Schraegstrich_bmp_start,&_binary_Schraegstrich_bmp_end,// ASCII-092, \ -> /
/*0061*/    &_binary_KlammerZu_bmp_start,&_binary_KlammerZu_bmp_end,        // ASCII-093, ] -> )
/*0062*/    &_binary_PfeilHoch_bmp_start,&_binary_PfeilHoch_bmp_end,        // ASCII-094, Dach -> Pfeil hoch
/*0063*/    &_binary_Minus_bmp_start,&_binary_Minus_bmp_end,                // ASCII-095, _ -> -
/*0064*/    &_binary_Apostroph_bmp_start,&_binary_Apostroph_bmp_end,        // ASCII-096, '
/*0065*/    &_binary_AE_bmp_start,&_binary_AE_bmp_end,                      // ASCII-123, { -> Ä
/*0066*/    &_binary_OE_bmp_start,&_binary_OE_bmp_end,                      // ASCII-124, | -> Ö
/*0067*/    &_binary_UE_bmp_start,&_binary_UE_bmp_end,                      // ASCII-125, } -> Ü
/*0068*/    &_binary_FreeTransparentMulticolor_bmp_start,&_binary_FreeTransparentMulticolor_bmp_end,// ASCII-126, ~ -> Ballons
/*0069*/    &_binary_Asteroid_bmp_start,&_binary_Asteroid_bmp_end,          // Asteroid
/*0070*/    &_binary_FD_bmp_start,&_binary_FD_bmp_end,                      // FD-Firmenlogo
/*0071*/    &_binary_Sand_bmp_start,&_binary_Sand_bmp_end,                  // Sand (alt, kann ersetzt werden)
/*0072*/    &_binary_Stahl_bmp_start,&_binary_Stahl_bmp_end,                // Stahl
/*0073*/    &_binary_MineLinks_bmp_start,&_binary_MineLinks_bmp_end,        // MinLinks
/*0074*/    &_binary_MineLinksAn_bmp_start,&_binary_MineLinksAn_bmp_end,    // MineLinksAn
/*0075*/    &_binary_BeetleLinks01_bmp_start,&_binary_BeetleLinks01_bmp_end,// BeetleLinks, Animationsschritt 1,9
/*0076*/    &_binary_BeetleLinks02_bmp_start,&_binary_BeetleLinks02_bmp_end,// BeetleLinks, Animationsschritt 2,10
/*0077*/    &_binary_BeetleLinks03_bmp_start,&_binary_BeetleLinks03_bmp_end,// BeetleLinks, Animationsschritt 3,11
/*0078*/    &_binary_BeetleLinks04_bmp_start,&_binary_BeetleLinks04_bmp_end,// BeetleLinks, Animationsschritt 4,12
/*0079*/    &_binary_BeetleLinks05_bmp_start,&_binary_BeetleLinks05_bmp_end,// BeetleLinks, Animationsschritt 5,13
/*0080*/    &_binary_BeetleLinks06_bmp_start,&_binary_BeetleLinks06_bmp_end,// BeetleLinks, Animationsschritt 6,14
/*0081*/    &_binary_BeetleLinks07_bmp_start,&_binary_BeetleLinks07_bmp_end,// BeetleLinks, Animationsschritt 7,15
/*0082*/    &_binary_BeetleLinks08_bmp_start,&_binary_BeetleLinks08_bmp_end,// BeetleLinks, Animationsschritt 8,16
/*0083*/    &_binary_Panel_bmp_start,&_binary_Panel_bmp_end,                // Spiele-Panel
/*0084*/    &_binary_0_green_bmp_start,&_binary_0_green_bmp_end,            // 0 (grün)
/*0085*/    &_binary_1_green_bmp_start,&_binary_1_green_bmp_end,            // 1 (grün)
/*0086*/    &_binary_2_green_bmp_start,&_binary_2_green_bmp_end,            // 2 (grün)
/*0087*/    &_binary_3_green_bmp_start,&_binary_3_green_bmp_end,            // 3 (grün)
/*0088*/    &_binary_4_green_bmp_start,&_binary_4_green_bmp_end,            // 4 (grün)
/*0089*/    &_binary_5_green_bmp_start,&_binary_5_green_bmp_end,            // 5 (grün)
/*0090*/    &_binary_6_green_bmp_start,&_binary_6_green_bmp_end,            // 6 (grün)
/*0091*/    &_binary_7_green_bmp_start,&_binary_7_green_bmp_end,            // 7 (grün)
/*0092*/    &_binary_8_green_bmp_start,&_binary_8_green_bmp_end,            // 8 (grün)
/*0093*/    &_binary_9_green_bmp_start,&_binary_9_green_bmp_end,            // 9 (grün)
/*0094*/    &_binary_door_red_bmp_start,&_binary_door_red_bmp_end,          // rote Tür
/*0095*/    &_binary_door_green_bmp_start,&_binary_door_green_bmp_end,      // grüne Tür
/*0096*/    &_binary_door_blue_bmp_start,&_binary_door_blue_bmp_end,        // blaue Tür
/*0097*/    &_binary_door_yellow_bmp_start,&_binary_door_yellow_bmp_end,    // gelbe Tür
/*0098*/    &_binary_key_red_bmp_start,&_binary_key_red_bmp_end,            // roter Schlüssel
/*0099*/    &_binary_key_green_bmp_start,&_binary_key_green_bmp_end,        // grüner Schlüssel
/*0100*/    &_binary_key_blue_bmp_start,&_binary_key_blue_bmp_end,          // blauer Schlüssel
/*0101*/    &_binary_key_yellow_bmp_start,&_binary_key_yellow_bmp_end,      // gelber Schlüssel
/*0102*/    &_binary_man_bmp_start,&_binary_man_bmp_end,                    // Man, stehend
/*0103*/    &_binary_ManLinks01_bmp_start,&_binary_ManLinks01_bmp_end,      // ManLinks, Animationsschritt 1,9
/*0104*/    &_binary_ManLinks02_bmp_start,&_binary_ManLinks02_bmp_end,      // ManLinks, Animationsschritt 2,10
/*0105*/    &_binary_ManLinks03_bmp_start,&_binary_ManLinks03_bmp_end,      // ManLinks, Animationsschritt 3,11
/*0106*/    &_binary_ManLinks04_bmp_start,&_binary_ManLinks04_bmp_end,      // ManLinks, Animationsschritt 4,12
/*0107*/    &_binary_ManLinks05_bmp_start,&_binary_ManLinks05_bmp_end,      // ManLinks, Animationsschritt 5,13
/*0108*/    &_binary_ManLinks06_bmp_start,&_binary_ManLinks06_bmp_end,      // ManLinks, Animationsschritt 6,14
/*0109*/    &_binary_ManLinks07_bmp_start,&_binary_ManLinks07_bmp_end,      // ManLinks, Animationsschritt 7,15
/*0110*/    &_binary_ManLinks08_bmp_start,&_binary_ManLinks08_bmp_end,      // ManLinks, Animationsschritt 8,16
/*0111*/    &_binary_ManHoch01_bmp_start,&_binary_ManHoch01_bmp_end,        // ManHoch, Animationsschritt 1,9
/*0112*/    &_binary_ManHoch02_bmp_start,&_binary_ManHoch02_bmp_end,        // ManHoch, Animationsschritt 2,10
/*0113*/    &_binary_ManHoch03_bmp_start,&_binary_ManHoch03_bmp_end,        // ManHoch, Animationsschritt 3,11
/*0114*/    &_binary_ManHoch04_bmp_start,&_binary_ManHoch04_bmp_end,        // ManHoch, Animationsschritt 4,12
/*0115*/    &_binary_ManHoch05_bmp_start,&_binary_ManHoch05_bmp_end,        // ManHoch, Animationsschritt 5,13
/*0116*/    &_binary_ManHoch06_bmp_start,&_binary_ManHoch06_bmp_end,        // ManHoch, Animationsschritt 6,14
/*0117*/    &_binary_ManHoch07_bmp_start,&_binary_ManHoch07_bmp_end,        // ManHoch, Animationsschritt 7,15
/*0118*/    &_binary_ManHoch08_bmp_start,&_binary_ManHoch08_bmp_end,        // ManHoch, Animationsschritt 8,16
/*0119*/    &_binary_ManRunter01_bmp_start,&_binary_ManRunter01_bmp_end,    // ManRunter, Animationsschritt 1,9
/*0120*/    &_binary_ManRunter02_bmp_start,&_binary_ManRunter02_bmp_end,    // ManRunter, Animationsschritt 2,10
/*0121*/    &_binary_ManRunter03_bmp_start,&_binary_ManRunter03_bmp_end,    // ManRunter, Animationsschritt 3,11
/*0122*/    &_binary_ManRunter04_bmp_start,&_binary_ManRunter04_bmp_end,    // ManRunter, Animationsschritt 4,12
/*0123*/    &_binary_ManRunter05_bmp_start,&_binary_ManRunter05_bmp_end,    // ManRunter, Animationsschritt 5,13
/*0124*/    &_binary_ManRunter06_bmp_start,&_binary_ManRunter06_bmp_end,    // ManRunter, Animationsschritt 6,14
/*0125*/    &_binary_ManRunter07_bmp_start,&_binary_ManRunter07_bmp_end,    // ManRunter, Animationsschritt 7,15
/*0126*/    &_binary_ManRunter08_bmp_start,&_binary_ManRunter08_bmp_end,    // ManRunter, Animationsschritt 8,16
/*0127*/    &_binary_AlienStand01_bmp_start,&_binary_AlienStand01_bmp_end,  // Alien stehend , Animationsschritt 1,2
/*0128*/    &_binary_AlienStand02_bmp_start,&_binary_AlienStand02_bmp_end,  // Alien stehend , Animationsschritt 3,4
/*0129*/    &_binary_AlienStand03_bmp_start,&_binary_AlienStand03_bmp_end,  // Alien stehend , Animationsschritt 5,6
/*0130*/    &_binary_AlienStand04_bmp_start,&_binary_AlienStand04_bmp_end,  // Alien stehend , Animationsschritt 7,8
/*0131*/    &_binary_AlienStand05_bmp_start,&_binary_AlienStand05_bmp_end,  // Alien stehend , Animationsschritt 8,10
/*0132*/    &_binary_AlienStand06_bmp_start,&_binary_AlienStand06_bmp_end,  // Alien stehend , Animationsschritt 11,12
/*0133*/    &_binary_AlienStand07_bmp_start,&_binary_AlienStand07_bmp_end,  // Alien stehend , Animationsschritt 13,14
/*0134*/    &_binary_AlienStand08_bmp_start,&_binary_AlienStand08_bmp_end,  // Alien stehend , Animationsschritt 15,16
/*0135*/    &_binary_AlienGo01_bmp_start,&_binary_AlienGo01_bmp_end,        // Alien geht, Animationsschritt 1,2,3,4,13,14,15,16
/*0136*/    &_binary_AlienGo02_bmp_start,&_binary_AlienGo02_bmp_end,        // Alien geht , Animationsschritt 5,6,7,8,9,10,11,12
/*0137*/    &_binary_Wheel_bmp_start,&_binary_Wheel_bmp_end,                // Alien-Rad, nicht drehend
/*0138*/    &_binary_ReplikatorRotAus_ObenLinks_bmp_start,&_binary_ReplikatorRotAus_ObenLinks_bmp_end,
/*0139*/    &_binary_ReplikatorRotAus_ObenMitte_bmp_start,&_binary_ReplikatorRotAus_ObenMitte_bmp_end,
/*0140*/    &_binary_ReplikatorRotAus_ObenRechts_bmp_start,&_binary_ReplikatorRotAus_ObenRechts_bmp_end,
/*0141*/    &_binary_ReplikatorRotAus_UntenLinks_bmp_start,&_binary_ReplikatorRotAus_UntenLinks_bmp_end,
/*0142*/    &_binary_ReplikatorRotAus_UntenRechts_bmp_start,&_binary_ReplikatorRotAus_UntenRechts_bmp_end,
/*0143*/    &_binary_ReplikatorRotAn01_ObenLinks_bmp_start,&_binary_ReplikatorRotAn01_ObenLinks_bmp_end,    // Replikator rot aktiv, Animationsschritt 1,2,3,4
/*0144*/    &_binary_ReplikatorRotAn01_ObenMitte_bmp_start,&_binary_ReplikatorRotAn01_ObenMitte_bmp_end,
/*0145*/    &_binary_ReplikatorRotAn01_ObenRechts_bmp_start,&_binary_ReplikatorRotAn01_ObenRechts_bmp_end,
/*0146*/    &_binary_ReplikatorRotAn02_ObenLinks_bmp_start,&_binary_ReplikatorRotAn02_ObenLinks_bmp_end,   // Replikator rot aktiv, Animationsschritt 5,6,7,8
/*0147*/    &_binary_ReplikatorRotAn02_ObenMitte_bmp_start,&_binary_ReplikatorRotAn02_ObenMitte_bmp_end,
/*0148*/    &_binary_ReplikatorRotAn02_ObenRechts_bmp_start,&_binary_ReplikatorRotAn02_ObenRechts_bmp_end,
/*0149*/    &_binary_ReplikatorRotAn03_ObenLinks_bmp_start,&_binary_ReplikatorRotAn03_ObenLinks_bmp_end,  // Replikator rot aktiv, Animationsschritt 9,10,11,12
/*0150*/    &_binary_ReplikatorRotAn03_ObenMitte_bmp_start,&_binary_ReplikatorRotAn03_ObenMitte_bmp_end,
/*0151*/    &_binary_ReplikatorRotAn03_ObenRechts_bmp_start,&_binary_ReplikatorRotAn03_ObenRechts_bmp_end,
/*0152*/    &_binary_ReplikatorRotAn_UntenLinks_bmp_start,&_binary_ReplikatorRotAn_UntenLinks_bmp_end,
/*0153*/    &_binary_ReplikatorRotAn_UntenRechts_bmp_start,&_binary_ReplikatorRotAn_UntenRechts_bmp_end,
/*0154*/    &_binary_ReplikatorSchalterRotAus_bmp_start,&_binary_ReplikatorSchalterRotAus_bmp_end,
/*0155*/    &_binary_ReplikatorSchalterRotAn_bmp_start,&_binary_ReplikatorSchalterRotAn_bmp_end,
/*0156*/    &_binary_Sand_links_rechts_oben_unten_bmp_start,&_binary_Sand_links_rechts_oben_unten_bmp_end,  // Sand (links/rechts/oben/unten frei 0000)
/*0157*/    &_binary_Sand_links_rechts_oben_bmp_start,&_binary_Sand_links_rechts_oben_bmp_end,              // Sand (links/rechts/oben frei 0001)
/*0158*/    &_binary_Sand_links_rechts_unten_bmp_start,&_binary_Sand_links_rechts_unten_bmp_end,            // Sand (links/rechts/unten frei 0010)
/*0159*/    &_binary_Sand_links_rechts_bmp_start,&_binary_Sand_links_rechts_bmp_end,                        // Sand (links/rechts frei 0011)
/*0160*/    &_binary_Sand_links_oben_unten_bmp_start,&_binary_Sand_links_oben_unten_bmp_end,                // Sand (links/oben/unten frei 0100)
/*0161*/    &_binary_Sand_links_oben_bmp_start,&_binary_Sand_links_oben_bmp_end,                            // Sand (links/oben frei 0101)
/*0162*/    &_binary_Sand_links_unten_bmp_start,&_binary_Sand_links_unten_bmp_end,                          // Sand (links/unten frei 0110)
/*0163*/    &_binary_Sand_links_bmp_start,&_binary_Sand_links_bmp_end,                                      // Sand (links frei 0111)
/*0164*/    &_binary_Sand_rechts_oben_unten_bmp_start,&_binary_Sand_rechts_oben_unten_bmp_end,              // Sand (rechts/oben/unten frei 1000)
/*0165*/    &_binary_Sand_rechts_oben_bmp_start,&_binary_Sand_rechts_oben_bmp_end,                          // Sand (rechts/oben frei 1001)
/*0166*/    &_binary_Sand_rechts_unten_bmp_start,&_binary_Sand_rechts_unten_bmp_end,                        // Sand (rechts/unten frei 1010)
/*0167*/    &_binary_Sand_rechts_bmp_start,&_binary_Sand_rechts_bmp_end,                                    // Sand (rechts frei 1011)
/*0168*/    &_binary_Sand_oben_unten_bmp_start,&_binary_Sand_oben_unten_bmp_end,                            // Sand (oben/unten frei 1100)
/*0169*/    &_binary_Sand_oben_bmp_start,&_binary_Sand_oben_bmp_end,                                        // Sand (oben frei 1101)
/*0170*/    &_binary_Sand_unten_bmp_start,&_binary_Sand_unten_bmp_end,                                      // Sand (unten frei 1110)
/*0171*/    &_binary_Sand_bmp_start,&_binary_Sand_bmp_end,                                                  // Sand (komplett eingeschlossen 1111)

            // Hier können weitere Grafikpointer (jeweils mit Start und Ende) eingetragen werden
            NULL,NULL// Ende, diese Zeile unbedingt hier belassen !
};

#endif // EXTERNALPOINTER_H_INCLUDED
