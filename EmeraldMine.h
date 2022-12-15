#ifndef TESTSURFACE_H_INCLUDED
#define TESTSURFACE_H_INCLUDED

#include <SDL2/SDL.h>
#include <stdbool.h>

#define MIN_LEVEL_W     4       // Minumum 4 Elemente in der Breite
#define MIN_LEVEL_H     3       // Minumum 3 Elemente in der Höhe

#define MAX_LEVEL_W     500     // Maximum Elemente in der Breite
#define MAX_LEVEL_H     500     // Maximum Elemente in der Höhe

#define PANEL_H         32      // Spiel-Anzeige (Time, Score usw.)

// Level-Elemente
#define EMERALD_NONE                            0x00            // kein Element, auch kein Space
#define EMERALD_STONE                           0x01            // Stein
#define EMERALD_SAPPHIRE                        0x02            // Saphir
#define EMERALD_MAGIC_WALL_SWITCH               0x03            // Magische Mauer, Schalter
#define EMERALD_ALIEN                           0x04            // Alien
#define EMERALD_MINE_UP                         0x08            // Mine hoch
#define EMERALD_MINE_RIGHT                      0x09            // Mine rechts
#define EMERALD_MINE_DOWN                       0x0A            // Mine runter
#define EMERALD_MINE_LEFT                       0x0B            // Mine links
#define EMERALD_LIGHTBARRIER_RED_SWITCH         0x0C            // Lichtschranke, rot, Schalter
#define EMERALD_LIGHTBARRIER_GREEN_SWITCH       0x0D            // Lichtschranke, grün, Schalter
#define EMERALD_LIGHTBARRIER_BLUE_SWITCH        0x0E            // Lichtschranke, blue, Schalter
#define EMERALD_LIGHTBARRIER_YELLOW_SWITCH      0x0F            // Lichtschranke, gelb, Schalter
#define EMERALD_BOMB                            0x10            // Bombe
#define EMERALD_CRYSTAL                         0x11            // Kristall
#define EMERALD_EMERALD                         0x12            // Emerald
#define EMERALD_RUBY                            0x13            // Rubin
#define EMERALD_BEETLE_UP                       0x14            // Käfer hoch
#define EMERALD_BEETLE_RIGHT                    0x15            // Käfer rechts
#define EMERALD_BEETLE_DOWN                     0x16            // Käfer runter
#define EMERALD_BEETLE_LEFT                     0x17            // Käfer links
#define EMERALD_REPLICATOR_RED_TOP_LEFT         0x18        	// Replikator rot, oben links
#define EMERALD_REPLICATOR_RED_TOP_MID          0x19        	// Replikator rot, oben mitte
#define EMERALD_REPLICATOR_RED_TOP_RIGHT        0x1A        	// Replikator rot, oben rechts
#define EMERALD_REPLICATOR_RED_BUTTOM_LEFT      0x1B        	// Replikator rot, unten links
#define EMERALD_REPLICATOR_RED_BUTTOM_RIGHT     0x1C        	// Replikator rot, unten rechts
#define EMERALD_REPLICATOR_RED_SWITCH           0x1D        	// Schalter für roten Replikator
#define EMERALD_REPLICATOR_YELLOW_TOP_LEFT      0x1E        	// Replikator gelb, oben links
#define EMERALD_REPLICATOR_YELLOW_TOP_MID       0x1F        	// Replikator gelb, oben mitte
#define EMERALD_REPLICATOR_YELLOW_TOP_RIGHT     0x20        	// Replikator gelb, oben rechts
#define EMERALD_REPLICATOR_YELLOW_BUTTOM_LEFT   0x21        	// Replikator gelb, unten links
#define EMERALD_REPLICATOR_YELLOW_BUTTOM_RIGHT  0x22        	// Replikator gelb, unten rechts
#define EMERALD_REPLICATOR_YELLOW_SWITCH        0x23        	// Schalter für gelben Replikator
#define EMERALD_TIME_COIN                       0x24            // Zeitmünze
#define EMERALD_NUT                             0x25            // Nuss
#define EMERALD_YAM                             0x29            // Yam
#define EMERALD_REPLICATOR_GREEN_TOP_LEFT       0x2E        	// Replikator grün, oben links
#define EMERALD_REPLICATOR_GREEN_TOP_MID        0x2F        	// Replikator grün, oben mitte
#define EMERALD_REPLICATOR_GREEN_TOP_RIGHT      0x30        	// Replikator grün, oben rechts
#define EMERALD_REPLICATOR_GREEN_BUTTOM_LEFT    0x31        	// Replikator grün, unten links
#define EMERALD_REPLICATOR_GREEN_BUTTOM_RIGHT   0x32        	// Replikator grün, unten rechts
#define EMERALD_REPLICATOR_GREEN_SWITCH         0x33        	// Schalter für grünen Replikator
#define EMERALD_REPLICATOR_BLUE_TOP_LEFT        0x34        	// Replikator blau, oben links
#define EMERALD_REPLICATOR_BLUE_TOP_MID         0x35        	// Replikator blau, oben mitte
#define EMERALD_REPLICATOR_BLUE_TOP_RIGHT       0x36        	// Replikator blau, oben rechts
#define EMERALD_REPLICATOR_BLUE_BUTTOM_LEFT     0x37        	// Replikator blau, unten links
#define EMERALD_REPLICATOR_BLUE_BUTTOM_RIGHT    0x38        	// Replikator blau, unten rechts
#define EMERALD_REPLICATOR_BLUE_SWITCH          0x39        	// Schalter für blauen Replikator
#define EMERALD_DYNAMITE_OFF                    0x3A            // Dynamit aus
#define EMERALD_MAN                             0x40            // Man
#define EMERALD_MESSAGE_1                       0x41            // Briefumschlag 1
#define EMERALD_MESSAGE_2                       0x42            // Briefumschlag 2
#define EMERALD_MESSAGE_3                       0x43            // Briefumschlag 3
#define EMERALD_MESSAGE_4                       0x44            // Briefumschlag 4
#define EMERALD_MESSAGE_5                       0x45            // Briefumschlag 5
#define EMERALD_MESSAGE_6                       0x46            // Briefumschlag 6
#define EMERALD_MESSAGE_7                       0x47            // Briefumschlag 7
#define EMERALD_MESSAGE_8                       0x48            // Briefumschlag 8
#define EMERALD_LIGHTBARRIER_RED_UP             0x49            // Lichtschranke aus, rot, oben
#define EMERALD_LIGHTBARRIER_RED_DOWN           0x4A            // Lichtschranke aus, rot, unten
#define EMERALD_LIGHTBARRIER_RED_LEFT           0x4B            // Lichtschranke aus, rot, links
#define EMERALD_LIGHTBARRIER_RED_RIGHT          0x4C            // Lichtschranke aus, rot, rechts
#define EMERALD_LIGHTBARRIER_GREEN_UP           0x4D            // Lichtschranke aus, grün, oben
#define EMERALD_LIGHTBARRIER_GREEN_DOWN         0x4E            // Lichtschranke aus, grün, unten
#define EMERALD_LIGHTBARRIER_GREEN_LEFT         0x4F            // Lichtschranke aus, grün, links
#define EMERALD_LIGHTBARRIER_GREEN_RIGHT        0x50            // Lichtschranke aus, grün, rechts
#define EMERALD_LIGHTBARRIER_BLUE_UP            0x51            // Lichtschranke aus, blau, oben
#define EMERALD_LIGHTBARRIER_BLUE_DOWN          0x52            // Lichtschranke aus, blau, unten
#define EMERALD_LIGHTBARRIER_BLUE_LEFT          0x53            // Lichtschranke aus, blau, links
#define EMERALD_LIGHTBARRIER_BLUE_RIGHT         0x54            // Lichtschranke aus, blau, rechts
#define EMERALD_LIGHTBARRIER_YELLOW_UP          0x55            // Lichtschranke an, gelb, oben
#define EMERALD_LIGHTBARRIER_YELLOW_DOWN        0x56            // Lichtschranke an, gelb, unten
#define EMERALD_LIGHTBARRIER_YELLOW_LEFT        0x57            // Lichtschranke an, gelb, links
#define EMERALD_LIGHTBARRIER_YELLOW_RIGHT       0x58            // Lichtschranke an, gelb, rechts
#define EMERALD_BEAM_RED_VERTICAL               0x59            // Lichtstrahl, rot, vertikal
#define EMERALD_BEAM_RED_HORIZONTAL             0x5A            // Lichtstrahl, rot, horizontal
#define EMERALD_BEAM_GREEN_VERTICAL             0x5B            // Lichtstrahl, grün, vertikal
#define EMERALD_BEAM_GREEN_HORIZONTAL           0x5C            // Lichtstrahl, grün, horizontal
#define EMERALD_BEAM_BLUE_VERTICAL              0x5D            // Lichtstrahl, blau, vertikal
#define EMERALD_BEAM_BLUE_HORIZONTAL            0x5E            // Lichtstrahl, blau, horizontal
#define EMERALD_BEAM_YELLOW_VERTICAL            0x5F            // Lichtstrahl, gelb, vertikal
#define EMERALD_BEAM_YELLOW_HORIZONTAL          0x60            // Lichtstrahl, gelb, horizontal
#define EMERALD_BEAM_CROSS                      0x61            // Lichtstrahl, Kreuz
#define EMERALD_ACIDPOOL_DESTROY                0x79            // Objekt läuft ins Säurebecken -> Objekt löst sich selbst auf
#define EMERALD_ACIDPOOL_TOP_LEFT               0x7A            // Säurebecken, oben links
#define EMERALD_ACIDPOOL_TOP_MID                0x7B            // Säurebecken, oben mitte (aktives Feld)
#define EMERALD_ACIDPOOL                        0x7B            // Säurebecken, oben mitte (aktives Feld)
#define EMERALD_ACIDPOOL_TOP_RIGHT              0x7C            // Säurebecken, oben rechts
#define EMERALD_ACIDPOOL_BUTTOM_LEFT            0x7D            // Säurebecken, unten links
#define EMERALD_ACIDPOOL_BUTTOM_MID             0x7E            // Säurebecken, unten mitte
#define EMERALD_ACIDPOOL_BUTTOM_RIGHT           0x7F            // Säurebecken, unten rechts
#define EMERALD_SPACE                           0x80            // Space
#define EMERALD_WALL_SOFT_ROUND                 0x81            // weiche Mauer, rund
#define EMERALD_SAND                            0x82            // Sand
#define EMERALD_WALL_HARD                       0x83            // harte (Außen)-Mauer
#define EMERALD_WALL_SOFT_CORNERED              0x84            // weiche Mauer, eckig
#define EMERALD_KEY_RED                         0x85            // roter Schlüssel
#define EMERALD_KEY_YELLOW                      0x86            // gelber Schlüssel
#define EMERALD_KEY_BLUE                        0x87            // blauer Schlüssel
#define EMERALD_KEY_GREEN                       0x88            // grüner Schlüssel
#define EMERALD_DOOR_RED                        0x89            // rote Tür
#define EMERALD_DOOR_YELLOW                     0x8A            // gelbe Tür
#define EMERALD_DOOR_BLUE                       0x8B            // blaue Tür
#define EMERALD_DOOR_GREEN                      0x8C            // grüne Tür
#define EMERALD_HAMMER                          0x8D            // Hammer
#define EMERALD_DOOR_GREY_RED                   0x8E            // rote Geheimtür
#define EMERALD_DOOR_GREY_YELLOW                0x8F            // gelbe Geheimtür
#define EMERALD_DOOR_GREY_BLUE                  0x90            // blaue Geheimtür
#define EMERALD_DOOR_GREY_GREEN                 0x91            // grüne Geheimtür
#define EMERALD_MAGIC_WALL                      0x92            // magische Mauer
#define EMERALD_WHEEL                           0x93            // Rad
#define EMERALD_SWAMP                           0x94            // Sumpf ohne Stein
#define EMERALD_SWAMP_STONE                     0x95            // Sumpf mit Stein
#define EMERALD_STONE_SINK                      0x96            // Stein versinkt in Sumpf
#define EMERALD_STONE_SAG                       0x97            // Stein sackt aus Sumpf
#define EMERALD_DOOR_END_NOT_READY              0x98            // Endtür, nicht blinkend
#define EMERALD_GREEN_DROP_COMES                0x99            // grüner Tropfen, der zu Käse werden kann
#define EMERALD_GREEN_DROP                      0x9A            // grüner Tropfen, der zu Käse werden kann
#define EMERALD_GREEN_CHEESE                    0x9B            // grüner Käse, der sich über Tropfen ausbreiten kann
#define EMERALD_EXPLOSION_PHASE_1               0x9C            // Explosion, Phase 1
#define EMERALD_EXPLOSION_PHASE_2               0x9D            // Explosion, Phase 2
#define EMERALD_WALL_SOFT_ROUND_PIKE            0x9E            // weiche Mauer, rund, mit Spitze
#define EMERALD_DOOR_END_READY                  0x9F            // Endtür, blinkend
#define EMERALD_STEEL_ROUND_PIKE                0xA0	        // Stahl, rund, mit Spitze
#define EMERALD_STEEL_ROUND                     0xA1	        // Stahl, rund
#define EMERALD_STEEL_WARNING                   0xA2	        // Stahl, Achtung
#define EMERALD_STEEL_BIOHAZARD                 0xA3	        // Stahl, Biogefahr
#define EMERALD_STEEL_DEADEND                   0xA4	        // Stahl, Sackgasse
#define EMERALD_STEEL_STOP                      0xA5	        // Stahl, Stopp
#define EMERALD_STEEL_PARKING                   0xA6	        // Stahl, Parkplatz
#define EMERALD_STEEL_FORBIDDEN                 0xA7	        // Stahl, Einbahn
#define EMERALD_STEEL_EXIT                      0xA8	        // Stahl, Ausgang
#define EMERALD_STEEL_RADIOACTIVE               0xA9	        // Stahl, Radioaktivität
#define EMERALD_STEEL_EXPLOSION                 0xAA	        // Stahl, Explosion
#define EMERALD_STEEL_ACID                      0xAB	        // Stahl, Säure
#define EMERALD_STEEL_NOT_ROUND                 0xAC	        // Stahl, nicht rund
#define EMERALD_WALL_SOFT_NOT_ROUND             0xAD	        // Mauer, nicht rund
#define EMERALD_STEEL_MARKER_LEFT_UP            0xAE	        // Stahl, Markierung, links oben
#define EMERALD_STEEL_MARKER_UP                 0xAF	        // Stahl, Markierung, oben
#define EMERALD_STEEL_MARKER_RIGHT_UP           0xB0	        // Stahl, Markierung, rechts oben
#define EMERALD_STEEL_MARKER_LEFT               0xB1	        // Stahl, Markierung, links
#define EMERALD_STEEL_MARKER_RIGHT              0xB2	        // Stahl, Markierung, rechts
#define EMERALD_STEEL_MARKER_LEFT_BUTTOM        0xB3	        // Stahl, Markierung, links unten
#define EMERALD_STEEL_MARKER_BUTTOM             0xB4	        // Stahl, Markierung, unten
#define EMERALD_STEEL_MARKER_RIGHT_BUTTOM       0xB5	        // Stahl, Markierung, rechts unten

#define EMERALD_INVALID                         0xFF            // ungültiges Element


// Animationen
#define EMERALD_ANIM_STAND                      0x00000000      // keine Animation
#define EMERALD_ANIM_UP                         0x00000100      // hoch
#define EMERALD_ANIM_RIGHT                      0x00000200      // rechts
#define EMERALD_ANIM_DOWN                       0x00000300      // runter
#define EMERALD_ANIM_LEFT                       0x00000400      // links
#define EMERALD_ANIM_SPIN_UP_TO_RIGHT           0x00000500      // von oben nach rechts drehen
#define EMERALD_ANIM_SPIN_RIGHT_TO_DOWN         0x00000600      // von rechts nach unten drehen
#define EMERALD_ANIM_SPIN_DOWN_TO_LEFT          0x00000700      // von unten nach links drehen
#define EMERALD_ANIM_SPIN_LEFT_TO_UP            0x00000800      // von links nach oben drehen
#define EMERALD_ANIM_SPIN_UP_TO_LEFT            0x00000900      // von oben nach links drehen
#define EMERALD_ANIM_SPIN_LEFT_TO_DOWN          0x00000A00      // von links nach unten drehen
#define EMERALD_ANIM_SPIN_DOWN_TO_RIGHT         0x00000B00      // von unten nach rechts drehen
#define EMERALD_ANIM_SPIN_RIGHT_TO_UP           0x00000C00      // von rechts nach oben drehen
#define EMERALD_ANIM_UP_DOUBLESPEED             0x00000D00      // hoch, doppelte Geschwindigkeit
#define EMERALD_ANIM_RIGHT_DOUBLESPEED          0x00000E00      // rechts, doppelte Geschwindigkeit
#define EMERALD_ANIM_DOWN_DOUBLESPEED           0x00000F00      // runter, doppelte Geschwindigkeit
#define EMERALD_ANIM_LEFT_DOUBLESPEED           0x00001000      // links, doppelte Geschwindigkeit


#define EMERALD_ANIM_CLEAN_UP                   0x00010000      // über dem invaliden Feld löschen
#define EMERALD_ANIM_CLEAN_RIGHT                0x00020000      // rechts neben invaliden Feld löschen
#define EMERALD_ANIM_CLEAN_DOWN                 0x00030000      // unter dem invaliden Feld löschen
#define EMERALD_ANIM_CLEAN_LEFT                 0x00040000      // links neben invaliden Feld löschen
#define EMERALD_ANIM_CLEAN_UP_DOUBLESPEED       0x00050000      // 2 Felder über dem invaliden Feld löschen
#define EMERALD_ANIM_CLEAN_RIGHT_DOUBLESPEED    0x00060000      // 2 Felder rechts neben invaliden Feld löschen
#define EMERALD_ANIM_CLEAN_DOWN_DOUBLESPEED     0x00070000      // 2 Felder unter dem invaliden Feld löschen
#define EMERALD_ANIM_CLEAN_LEFT_DOUBLESPEED     0x00080000      // 2 Felder links neben invaliden Feld löschen
#define EMERALD_ANIM_CLEAN_NOTHING              0x00090000      // Gar nichts löschen/setzen, wird z.B. für grünen Tropfen in Phase 1 benötigt
#define EMERALD_ANIM_AVOID_DOUBLE_CONTROL       0x000A0000      // Element nicht doppelt steuern (Element wird z.B. I + X gesetzt und durch ControlGame(I) nachgesteuet)


// "Selbststeuernde" Zustände                                   // Diese Zustände müssen vom Element selbst zurückgesetzt werden
#define EMERALD_NO_ADDITIONAL_ANIMSTATUS        0x00000000      // kein zusätzlicher Status
#define EMERALD_ANIM_LOST_GUIDE                 0x01000000      // Mine oder Käfer haben Führung verloren
#define EMERALD_ANIM_ALIEN_MOVED                0x02000000      // Alien hat sich im letzten Schritt bewegt -> soll keine Doppelbewegungen machen
#define EMERALD_ANIM_WHEEL_RUN                  0x03000000      // Rad soll bewegt werden
#define EMERALD_ANIM_BORN1                      0x04000000      // Replikator erzeugt neues Element über
#define EMERALD_ANIM_BORN2                      0x05000000      // 2 Steuerungsphasen -> halbe Geschwindigekit
#define EMERALD_ANIM_KEY_RED_SHRINK             0x06000000      // roter Schlüssel wird keiner
#define EMERALD_ANIM_KEY_GREEN_SHRINK           0x07000000      // grüner Schlüssel wird keiner
#define EMERALD_ANIM_KEY_BLUE_SHRINK            0x08000000      // blauer Schlüssel wird keiner
#define EMERALD_ANIM_KEY_YELLOW_SHRINK          0x09000000      // gelber Schlüssel wird keiner
#define EMERALD_ANIM_SAND_SHRINK                0x0A000000      // Sand verschwindet
#define EMERALD_ANIM_DOWN_SELF                  0x0B000000      // Bewegung war nach unten (selbststeuernd)
#define EMERALD_ANIM_EMERALD_SHRINK             0x0C000000      // Emerald wird kleiner
#define EMERALD_ANIM_NUT_CRACK1                 0x0D000000      // Nuss wird von Stein geknackt, muss in 2 Stufen erfolgen (Nuss unter Stein)
#define EMERALD_ANIM_NUT_CRACK2                 0x0E000000      // Nuss wird von Stein geknackt, 2 Stufe
#define EMERALD_ANIM_MAN_LEFT_ARM               0x0F000000      // Man hebt linken Arm
#define EMERALD_ANIM_MAN_RIGHT_ARM              0x10000000      // Man hebt rechten Arm
#define EMERALD_ANIM_MAN_UP_ARM                 0x11000000      // Man hebt Arm hoch
#define EMERALD_ANIM_MAN_DOWN_ARM               0x12000000      // Man bewegt Arm runter
#define EMERALD_ANIM_MAN_PUSH_RIGHT             0x13000000      // Man schiebt rechts
#define EMERALD_ANIM_MAN_PUSH_RIGHT2            0x14000000      // Man schiebt rechts, Phase 2
#define EMERALD_ANIM_MAN_PUSH_LEFT              0x15000000      // Man schiebt links
#define EMERALD_ANIM_MAN_PUSH_LEFT2             0x16000000      // Man schiebt links, Phase 2
#define EMERALD_ANIM_SAPPHIRE_SHRINK            0x17000000      // Saphir wird kleiner
#define EMERALD_ANIM_SAPPHIRE_SQUEAK            0x18000000      // Saphir wird gequetscht
#define EMERALD_ANIM_STONE_SWAMP1               0x19000000      // Stein versinkt/durchsackt, Phase 1
#define EMERALD_ANIM_STONE_SWAMP2               0x1A000000      // Stein versinkt/durchsackt, Phase 2
#define EMERALD_ANIM_STONE_SWAMP3               0x1B000000      // Stein durchsackt, Phase 3 (Beim Durchsacken eine Phase mehr)
#define EMERALD_ANIM_BLITZ                      0x1C000000      // Emerald oder Saphir soll blitzen
#define EMERALD_ANIM_SINK_IN_MAGIC_WALL         0x1D000000      // Ein Element (Stein, Emerald, Saphir, Rubin) sinkt in Magic Wall
#define EMERALD_ANIM_SAG_OUT_MAGIC_WALL         0x1E000000      // Ein Element (Stein, Emerald, Saphir, Rubin) kommt aus Magic Wall
#define EMERALD_ANIM_RUBY_SHRINK                0x1F000000      // Rubin wird kleiner
#define EMERALD_ANIM_CRYSTAL_SHRINK             0x20000000      // Kristall wird kleiner
#define EMERALD_ANIM_TIME_COIN_SHRINK           0x21000000      // Zeitmünze wird kleiner
#define EMERALD_ANIM_HAMMER_SHRINK              0x22000000      // Hammer wird kleiner
#define EMERALD_ANIM_DYNAMITE_SHRINK            0x23000000      // Dynamit wird kleiner
#define EMERALD_ANIM_MESSAGE_SHRINK             0x24000000      // Briefumschlag wird kleiner
#define EMERALD_ANIM_GREEN_DROP_1               0x25000000      // grüner Tropfen, Phase 1
#define EMERALD_ANIM_GREEN_DROP_2               0x26000000      // grüner Tropfen, Phase 2
#define EMERALD_STANDARD_SPEED                  false
#define EMERALD_DOUBLE_SPEED                    true

#define EMERALD_MAX_HEADER_SIZE                 0x500           // memmem begrenzen
#define EMERALD_MAX_MESSAGES                    8
 // Bit 0 = Emerald, Bit 1 = Saphir, Bit 2 = Stone, Bit 3 = Nut, Bit 4 = Bomb, Bit 5 = Rubin, Bit 6 = Kristall
#define EMERALD_CHECKROLL_EMERALD               0x01
#define EMERALD_CHECKROLL_SAPPHIRE              0x02
#define EMERALD_CHECKROLL_STONE                 0x04
#define EMERALD_CHECKROLL_NUT                   0x08
#define EMERALD_CHECKROLL_BOMB                  0x10
#define EMERALD_CHECKROLL_RUBY                  0x20
#define EMERALD_CHECKROLL_CRYSTAL               0x40


typedef struct {
    uint32_t        uTextureIndex;          // Start-Texture zur Animation
    bool            bFlipVertical;          // Texture vertikal spiegeln
    bool            bFlipHorizontal;        // Texture horizontal spiegeln
    uint32_t        uX;                     // X-Koordinate, wo Post-Animation durchgeführt werden soll.
    uint32_t        uY;                     // Y-Koordinate, wo Post-Animation durchgeführt werden soll.
    uint32_t        uFormulaIndex;          // Index auf Formel für Animation
} POSTANIMATION;


typedef struct {
    uint8_t         *pLevel;
    uint32_t        *pStatusAnimation;
    uint16_t        uRollUnderground[256];  // Bit 0 = Emerald, Bit 1 = Saphir, Bit 2 = Stone, Bit 3 = Nut, Bit 4 = Bomb, Bit 5 = Rubin, Bit 6 = Kristall
    POSTANIMATION   *pPostAnimation;
    bool            bInitOK;
    bool            bHasRedKey;
    bool            bHasGreenKey;
    bool            bHasBlueKey;
    bool            bHasYellowKey;
    bool            bReplicatorRedOn;
    bool            bWheelRunning;
    bool            bMagicWallRunning;
    bool            bMagicWallWasOn;        // Magic Wall war mal an
    bool            bPushStone;             // wenn true, kann Man sofort (Stone, Nut, Bomb) schieben
    bool            bManDead;
    bool            bLightBarrierRedOn;
    bool            bLightBarrierGreenOn;
    bool            bLightBarrierBlueOn;
    bool            bLightBarrierYellowOn;
    uint8_t         uReplicatorRedObject;
    bool            bReplicatorGreenOn;
    uint8_t         uReplicatorGreenObject;
    bool            bReplicatorBlueOn;
    uint8_t         uReplicatorBlueObject;
    bool            bReplicatorYellowOn;
    uint8_t         uReplicatorYellowObject;
    char            szVersion[6];           // z.B. "01.00"
    char            szLevelTitle[32 + 1];   // z.B. "Der Bunker"
    char            szLevelAuthor[32 + 1];  // z.B. "Mikiman"
    char            *pMessage[8];           // 8 Nachrichtentexte
    uint32_t        uScoreEmerald;
    uint32_t        uScoreRuby;
    uint32_t        uScoreSaphir;
    uint32_t        uScoreCrystal;
    uint32_t        uScoreKey;
    uint32_t        uScoreDynamite;
    uint32_t        uScoreHammer;
    uint32_t        uScoreNutCracking;
    uint32_t        uScoreStoningBeetle;
    uint32_t        uScoreStoningMine;
    uint32_t        uScoreStoningAlien;
    uint32_t        uScoreStoningYam;
    uint32_t        uScoreTimeCoin;
    uint32_t        uScoreMessage;
    uint32_t        uEmeraldsToCollect;
    uint32_t        uTimeScoreFactor;
    uint32_t        uCheeseSpreadSpeed;
    uint32_t        uTimeToPlay;
    uint32_t        uAdditonalTimeCoinTime; // zusätzliche Zeit durch Zeitmünze
    uint32_t        uTimeWheelRotation;
    uint32_t        uTimeWheelRotationLeft; // Verbleibende Zeit für Wheel-Rotation
    uint32_t        uWheelRunningXpos;      // nur gültig, wenn bWheelRunning = true
    uint32_t        uWheelRunningYpos;      // nur gültig, wenn bWheelRunning = true
    uint32_t        uTimeMagicWall;
    uint32_t        uTimeMagicWallLeft;     // Verbleibende Zeit für Magic-Wall
    uint32_t        uDynamiteCount;         // Anzahl Dynamits, die der Man hat
    uint32_t        uHammerCount;           // Anzahl Hammer, die der Man hat
    uint32_t        uLevel_X_Dimension;
    uint32_t        uLevel_Y_Dimension;
    int             nTopLeftXpos;           // aktuelle X-Pixel-Position (obere linke sichtbare Ecke)
    int             nTopLeftYpos;           // aktuelle Y-Pixel-Position (obere linke sichtbare Ecke)
    uint32_t        uTotalScore;
    uint32_t        uVisibleX;              // Anzahl der sichtbaren Elemente-Spalten
    uint32_t        uVisibleY;              // Anzahl der sichtbaren Elemente-Zeilen (ohne unteres Panel)
    uint32_t        uVisibleCenterX;        // Zentrum X des sichtbaren Bereichs (ohne unteres Panel)
    uint32_t        uVisibleCenterY;        // Zentrum X des sichtbaren Bereichs (ohne unteres Panel)
    uint32_t        uManXpos;               // X-Element-Position des Man
    uint32_t        uManYpos;               // Y-Element-Position des Man
    uint32_t        uFrameCounter;          // Bildzähler
    uint32_t        uShowMessageNo;         // Nachricht Nr. x zeigen, 0 = keine Nachricht
} PLAYFIELD;


uint32_t ControlGame(uint32_t uDirection);
int RenderLevel(SDL_Renderer *pRenderer, int *pnXpos, int *pnYpos, int nAnimationCount);
uint32_t GetTextureIndexByElement(uint32_t uElement,int nAnimationCount,float *pfAngle);
uint32_t GetTextureIndexByElementForAcidPool(uint32_t uElement,int nAnimationCount, float *pfAngle);
void ScrollAndCenterLevel(uint32_t uManDirection);
int RunGame(SDL_Window *pWindow, SDL_Renderer *pRenderer);
void InitRollUnderground(void);
uint8_t GetFreeRollDirections(uint32_t I);
uint32_t GetTextureIndexByShrink(uint32_t uShrinkAnimation);

#endif // TESTSURFACE_H_INCLUDED
