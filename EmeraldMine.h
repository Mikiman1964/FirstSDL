#ifndef TESTSURFACE_H_INCLUDED
#define TESTSURFACE_H_INCLUDED

#include <SDL2/SDL.h>
#include <stdbool.h>

#define MIN_LEVEL_W                             4       // Minumum 4 Elemente in der Breite
#define MIN_LEVEL_H                             3       // Minumum 3 Elemente in der Höhe
#define MAX_LEVEL_W                             1000    // Maximum Elemente in der Breite
#define MAX_LEVEL_H                             1000    // Maximum Elemente in der Höhe

#define PANEL_H                                 32      // Spiel-Anzeige (Time, Score usw.)

#define EMERALD_VERSION                         "01.00" // Version

#define EMERALD_VERSION_LEN                     5       // Maximale Versionslänge, z.B "01.00"
#define EMERALD_TITLE_LEN                       32      // Maximale Titellänge
#define EMERALD_AUTHOR_LEN                      32      // Maximale Autorlänge
#define EMERALD_MD5_STRING_LEN                  32      // Maximale Autorlänge
#define EMERALD_MAX_YAM_EXPLOSIONS              100     // Maximale Anzahl an verschiedenen YAM-Explosionen

// Level-Elemente
#define EMERALD_NONE                            0x0000            // kein Element, auch kein Space
#define EMERALD_STONE                           0x0001            // Stein
#define EMERALD_SAPPHIRE                        0x0002            // Saphir
#define EMERALD_PERL                            0x0003            // Perle
#define EMERALD_MOLE_UP                         0x0004            // Maulwurf hoch
#define EMERALD_MOLE_RIGHT                      0x0005            // Maulwurf rechts
#define EMERALD_MOLE_DOWN                       0x0006            // Maulwurf runter
#define EMERALD_MOLE_LEFT                       0x0007            // Maulwurf links
#define EMERALD_MINE_UP                         0x0008            // Mine hoch
#define EMERALD_MINE_RIGHT                      0x0009            // Mine rechts
#define EMERALD_MINE_DOWN                       0x000A            // Mine runter
#define EMERALD_MINE_LEFT                       0x000B            // Mine links
#define EMERALD_LIGHTBARRIER_RED_SWITCH         0x000C            // Lichtschranke, rot, Schalter
#define EMERALD_LIGHTBARRIER_GREEN_SWITCH       0x000D            // Lichtschranke, grün, Schalter
#define EMERALD_LIGHTBARRIER_BLUE_SWITCH        0x000E            // Lichtschranke, blue, Schalter
#define EMERALD_LIGHTBARRIER_YELLOW_SWITCH      0x000F            // Lichtschranke, gelb, Schalter
#define EMERALD_BOMB                            0x0010            // Bombe
#define EMERALD_CRYSTAL                         0x0011            // Kristall
#define EMERALD_EMERALD                         0x0012            // Emerald
#define EMERALD_RUBY                            0x0013            // Rubin
#define EMERALD_BEETLE_UP                       0x0014            // Käfer hoch
#define EMERALD_BEETLE_RIGHT                    0x0015            // Käfer rechts
#define EMERALD_BEETLE_DOWN                     0x0016            // Käfer runter
#define EMERALD_BEETLE_LEFT                     0x0017            // Käfer links
#define EMERALD_REPLICATOR_RED_TOP_LEFT         0x0018        	  // Replikator rot, oben links
#define EMERALD_REPLICATOR_RED_TOP_MID          0x0019        	  // Replikator rot, oben mitte
#define EMERALD_REPLICATOR_RED_TOP_RIGHT        0x001A        	  // Replikator rot, oben rechts
#define EMERALD_REPLICATOR_RED_BOTTOM_LEFT      0x001B        	  // Replikator rot, unten links
#define EMERALD_REPLICATOR_RED_BOTTOM_RIGHT     0x001C        	  // Replikator rot, unten rechts
#define EMERALD_REPLICATOR_RED_SWITCH           0x001D        	  // Schalter für roten Replikator
#define EMERALD_REPLICATOR_YELLOW_TOP_LEFT      0x001E        	  // Replikator gelb, oben links
#define EMERALD_REPLICATOR_YELLOW_TOP_MID       0x001F        	  // Replikator gelb, oben mitte
#define EMERALD_REPLICATOR_YELLOW_TOP_RIGHT     0x0020        	  // Replikator gelb, oben rechts
#define EMERALD_REPLICATOR_YELLOW_BOTTOM_LEFT   0x0021        	  // Replikator gelb, unten links
#define EMERALD_REPLICATOR_YELLOW_BOTTOM_RIGHT  0x0022        	  // Replikator gelb, unten rechts
#define EMERALD_REPLICATOR_YELLOW_SWITCH        0x0023        	  // Schalter für gelben Replikator
#define EMERALD_TIME_COIN                       0x0024            // Zeitmünze
#define EMERALD_NUT                             0x0025            // Nuss
#define EMERALD_ALIEN                           0x0026            // Alien
#define EMERALD_YAM                             0x0027            // Yam
#define EMERALD_STANDMINE                       0x0028            // Stand-Mine
#define EMERALD_MAGIC_WALL_SWITCH               0x0029            // Magische Mauer, Schalter
#define EMERALD_MAGIC_WALL_STEEL                0x002A            // magische Mauer aus Stahl
#define EMERALD_SAND_MOLE                       0x002B            // Sand, der durch Maufwurf entsteht
#define EMERALD_GREEN_CHEESE_GOES               0x002C            // Grüner Käse löst sich auf
#define EMERALD_LIGHT_SWITCH                    0x002D            // Lichtschalter
#define EMERALD_REPLICATOR_GREEN_TOP_LEFT       0x002E        	  // Replikator grün, oben links
#define EMERALD_REPLICATOR_GREEN_TOP_MID        0x002F        	  // Replikator grün, oben mitte
#define EMERALD_REPLICATOR_GREEN_TOP_RIGHT      0x0030        	  // Replikator grün, oben rechts
#define EMERALD_REPLICATOR_GREEN_BOTTOM_LEFT    0x0031        	  // Replikator grün, unten links
#define EMERALD_REPLICATOR_GREEN_BOTTOM_RIGHT   0x0032        	  // Replikator grün, unten rechts
#define EMERALD_REPLICATOR_GREEN_SWITCH         0x0033        	  // Schalter für grünen Replikator
#define EMERALD_REPLICATOR_BLUE_TOP_LEFT        0x0034        	  // Replikator blau, oben links
#define EMERALD_REPLICATOR_BLUE_TOP_MID         0x0035        	  // Replikator blau, oben mitte
#define EMERALD_REPLICATOR_BLUE_TOP_RIGHT       0x0036        	  // Replikator blau, oben rechts
#define EMERALD_REPLICATOR_BLUE_BOTTOM_LEFT     0x0037        	  // Replikator blau, unten links
#define EMERALD_REPLICATOR_BLUE_BOTTOM_RIGHT    0x0038        	  // Replikator blau, unten rechts
#define EMERALD_REPLICATOR_BLUE_SWITCH          0x0039        	  // Schalter für blauen Replikator
#define EMERALD_DYNAMITE_OFF                    0x003A            // Dynamit aus
#define EMERALD_DYNAMITE_ON                     0x003B            // Dynamit an
#define EMERALD_DOOR_RED_WOOD                   0x003C            // rote Holz-Tür
#define EMERALD_DOOR_YELLOW_WOOD                0x003D            // gelbe Holz-Tür
#define EMERALD_DOOR_BLUE_WOOD                  0x003E            // blaue Holz-Tür
#define EMERALD_DOOR_GREEN_WOOD                 0x003F            // grüne Holz-Tür
#define EMERALD_MAN                             0x0040            // Man
#define EMERALD_MESSAGE_1                       0x0041            // Briefumschlag 1, Die Message-Elemente
#define EMERALD_MESSAGE_2                       0x0042            // Briefumschlag 2, müssen aufsteigend und
#define EMERALD_MESSAGE_3                       0x0043            // Briefumschlag 3, ohne Lücken aufgezählt werden,
#define EMERALD_MESSAGE_4                       0x0044            // Briefumschlag 4, da im Quelltext dieser Bereich
#define EMERALD_MESSAGE_5                       0x0045            // Briefumschlag 5, geprüft wird.
#define EMERALD_MESSAGE_6                       0x0046            // Briefumschlag 6
#define EMERALD_MESSAGE_7                       0x0047            // Briefumschlag 7
#define EMERALD_MESSAGE_8                       0x0048            // Briefumschlag 8
#define EMERALD_LIGHTBARRIER_RED_UP             0x0049            // Lichtschranke aus, rot, oben
#define EMERALD_LIGHTBARRIER_RED_DOWN           0x004A            // Lichtschranke aus, rot, unten
#define EMERALD_LIGHTBARRIER_RED_LEFT           0x004B            // Lichtschranke aus, rot, links
#define EMERALD_LIGHTBARRIER_RED_RIGHT          0x004C            // Lichtschranke aus, rot, rechts
#define EMERALD_LIGHTBARRIER_GREEN_UP           0x004D            // Lichtschranke aus, grün, oben
#define EMERALD_LIGHTBARRIER_GREEN_DOWN         0x004E            // Lichtschranke aus, grün, unten
#define EMERALD_LIGHTBARRIER_GREEN_LEFT         0x004F            // Lichtschranke aus, grün, links
#define EMERALD_LIGHTBARRIER_GREEN_RIGHT        0x0050            // Lichtschranke aus, grün, rechts
#define EMERALD_LIGHTBARRIER_BLUE_UP            0x0051            // Lichtschranke aus, blau, oben
#define EMERALD_LIGHTBARRIER_BLUE_DOWN          0x0052            // Lichtschranke aus, blau, unten
#define EMERALD_LIGHTBARRIER_BLUE_LEFT          0x0053            // Lichtschranke aus, blau, links
#define EMERALD_LIGHTBARRIER_BLUE_RIGHT         0x0054            // Lichtschranke aus, blau, rechts
#define EMERALD_LIGHTBARRIER_YELLOW_UP          0x0055            // Lichtschranke an, gelb, oben
#define EMERALD_LIGHTBARRIER_YELLOW_DOWN        0x0056            // Lichtschranke an, gelb, unten
#define EMERALD_LIGHTBARRIER_YELLOW_LEFT        0x0057            // Lichtschranke an, gelb, links
#define EMERALD_LIGHTBARRIER_YELLOW_RIGHT       0x0058            // Lichtschranke an, gelb, rechts
#define EMERALD_BEAM_RED_VERTICAL               0x0059            // Lichtstrahl, rot, vertikal
#define EMERALD_BEAM_RED_HORIZONTAL             0x005A            // Lichtstrahl, rot, horizontal
#define EMERALD_BEAM_GREEN_VERTICAL             0x005B            // Lichtstrahl, grün, vertikal
#define EMERALD_BEAM_GREEN_HORIZONTAL           0x005C            // Lichtstrahl, grün, horizontal
#define EMERALD_BEAM_BLUE_VERTICAL              0x005D            // Lichtstrahl, blau, vertikal
#define EMERALD_BEAM_BLUE_HORIZONTAL            0x005E            // Lichtstrahl, blau, horizontal
#define EMERALD_BEAM_YELLOW_VERTICAL            0x005F            // Lichtstrahl, gelb, vertikal
#define EMERALD_BEAM_YELLOW_HORIZONTAL          0x0060            // Lichtstrahl, gelb, horizontal
#define EMERALD_BEAM_CROSS                      0x0061            // Lichtstrahl, Kreuz
#define EMERALD_DOOR_EMERALD                    0x0062            // Einen Emerald mehr zu sammeln beim Durchgang durch diese Tür
#define EMERALD_DOOR_MULTICOLOR                 0x0063            // Bunte Tür, ein Schlüssel reicht
#define EMERALD_DOOR_ONLY_UP_STEEL              0x0064            // Tür, nur nach oben passierbar
#define EMERALD_DOOR_ONLY_DOWN_STEEL            0x0065            // Tür, nur nach unten passierbar
#define EMERALD_DOOR_ONLY_LEFT_STEEL            0x0066            // Tür, nur nach links passierbar
#define EMERALD_DOOR_ONLY_RIGHT_STEEL           0x0067            // Tür, nur nach rechts passierbar
#define EMERALD_WHEEL_TIMEDOOR                  0x0068            // Rad für Zeit-Tür
#define EMERALD_DOOR_TIME                       0x0069            // Zeit-Tür
#define EMERALD_SWITCHDOOR_OPEN                 0x006A            // Schalttür auf
#define EMERALD_SWITCHDOOR_CLOSED               0x006B            // Schalttür geschlossen
#define EMERALD_SWITCH_SWITCHDOOR               0x006C            // Schalter für Schalttür
#define EMERALD_DOOR_END_NOT_READY_STEEL        0x006D            // Endtür aus Stahl, nicht blinkend
#define EMERALD_DOOR_END_READY_STEEL            0x006E            // Endtür aus Stahl, blinkend
#define EMERALD_KEY_WHITE                       0x006F            // weißer Schlüssel
#define EMERALD_DOOR_WHITE                      0x0070            // weiße Tür
#define EMERALD_DOOR_GREY_WHITE                 0x0071            // weiße Geheimtür
#define EMERALD_DOOR_WHITE_WOOD                 0x0072            // weiße Holztür
#define EMERALD_KEY_GENERAL                     0x0073            // General-Schlüssel
#define EMERALD_MEGABOMB                        0x0074            // Mega-Bombe
#define EMERALD_WALL_WITH_EMERALD               0x0075            // Mauer mit Emerald
#define EMERALD_WALL_WITH_RUBY                  0x0076            // Mauer mit Rubin
#define EMERALD_WALL_WITH_SAPPHIRE              0x0077            // Mauer mit Saphir
#define EMERALD_WALL_WITH_PERL                  0x0078            // Mauer mit Perle
#define EMERALD_ACIDPOOL_DESTROY                0x0079            // Objekt läuft ins Säurebecken -> Objekt löst sich selbst auf
#define EMERALD_ACIDPOOL_TOP_LEFT               0x007A            // Säurebecken, oben links
#define EMERALD_ACIDPOOL_TOP_MID                0x007B            // Säurebecken, oben mitte (aktives Feld)
#define EMERALD_ACIDPOOL                        0x007B            // Säurebecken, oben mitte (aktives Feld)
#define EMERALD_ACIDPOOL_TOP_RIGHT              0x007C            // Säurebecken, oben rechts
#define EMERALD_ACIDPOOL_BOTTOM_LEFT            0x007D            // Säurebecken, unten links
#define EMERALD_ACIDPOOL_BOTTOM_MID             0x007E            // Säurebecken, unten mitte
#define EMERALD_ACIDPOOL_BOTTOM_RIGHT           0x007F            // Säurebecken, unten rechts
#define EMERALD_SPACE                           0x0080            // Space
#define EMERALD_WALL_ROUND                      0x0081            // Mauer, rund
#define EMERALD_SAND                            0x0082            // Sand
#define EMERALD_STEEL                           0x0083            // Stahl
#define EMERALD_WALL_CORNERED                   0x0084            // Mauer, eckig
#define EMERALD_KEY_RED                         0x0085            // roter Schlüssel
#define EMERALD_KEY_YELLOW                      0x0086            // gelber Schlüssel
#define EMERALD_KEY_BLUE                        0x0087            // blauer Schlüssel
#define EMERALD_KEY_GREEN                       0x0088            // grüner Schlüssel
#define EMERALD_DOOR_RED                        0x0089            // rote Tür
#define EMERALD_DOOR_YELLOW                     0x008A            // gelbe Tür
#define EMERALD_DOOR_BLUE                       0x008B            // blaue Tür
#define EMERALD_DOOR_GREEN                      0x008C            // grüne Tür
#define EMERALD_HAMMER                          0x008D            // Hammer
#define EMERALD_DOOR_GREY_RED                   0x008E            // rote Geheimtür
#define EMERALD_DOOR_GREY_YELLOW                0x008F            // gelbe Geheimtür
#define EMERALD_DOOR_GREY_BLUE                  0x0090            // blaue Geheimtür
#define EMERALD_DOOR_GREY_GREEN                 0x0091            // grüne Geheimtür
#define EMERALD_MAGIC_WALL                      0x0092            // magische Mauer
#define EMERALD_WHEEL                           0x0093            // Rad
#define EMERALD_SWAMP                           0x0094            // Sumpf ohne Stein
#define EMERALD_SWAMP_STONE                     0x0095            // Sumpf mit Stein
#define EMERALD_STONE_SINK                      0x0096            // Stein versinkt in Sumpf
#define EMERALD_STONE_SAG                       0x0097            // Stein sackt aus Sumpf
#define EMERALD_DOOR_END_NOT_READY              0x0098            // Endtür, nicht blinkend
#define EMERALD_GREEN_DROP_COMES                0x0099            // grüner Tropfen, der zu Käse werden kann
#define EMERALD_GREEN_DROP                      0x009A            // grüner Tropfen, der zu Käse werden kann
#define EMERALD_GREEN_CHEESE                    0x009B            // grüner Käse, der sich über Tropfen ausbreiten kann
#define EMERALD_EXPLOSION_TO_ELEMENT_1          0x009C            // Explosion wandelt sich in Element, Phase 1
#define EMERALD_EXPLOSION_TO_ELEMENT_2          0x009D            // Explosion wandelt sich in Element, Phase 2
#define EMERALD_WALL_ROUND_PIKE                 0x009E            // Mauer, rund, mit Spitze
#define EMERALD_DOOR_END_READY                  0x009F            // Endtür, blinkend
#define EMERALD_STEEL_ROUND_PIKE                0x00A0	          // Stahl, rund, mit Spitze
#define EMERALD_STEEL_ROUND                     0x00A1	          // Stahl, rund
#define EMERALD_STEEL_WARNING                   0x00A2	          // Stahl, Achtung
#define EMERALD_STEEL_BIOHAZARD                 0x00A3	          // Stahl, Biogefahr
#define EMERALD_STEEL_DEADEND                   0x00A4	          // Stahl, Sackgasse
#define EMERALD_STEEL_STOP                      0x00A5	          // Stahl, Stopp
#define EMERALD_STEEL_PARKING                   0x00A6	          // Stahl, Parkplatz
#define EMERALD_STEEL_FORBIDDEN                 0x00A7	          // Stahl, Einbahn
#define EMERALD_STEEL_EXIT                      0x00A8	          // Stahl, Ausgang
#define EMERALD_STEEL_RADIOACTIVE               0x00A9	          // Stahl, Radioaktivität
#define EMERALD_STEEL_EXPLOSION                 0x00AA	          // Stahl, Explosion
#define EMERALD_STEEL_ACID                      0x00AB	          // Stahl, Säure
#define EMERALD_STEEL_NOT_ROUND                 0x00AC	          // Stahl, nicht rund
#define EMERALD_WALL_NOT_ROUND                  0x00AD	          // Mauer, nicht rund
#define EMERALD_STEEL_MARKER_LEFT_UP            0x00AE	          // Stahl, Markierung, links oben
#define EMERALD_STEEL_MARKER_UP                 0x00AF	          // Stahl, Markierung, oben
#define EMERALD_STEEL_MARKER_RIGHT_UP           0x00B0	          // Stahl, Markierung, rechts oben
#define EMERALD_STEEL_MARKER_LEFT               0x00B1	          // Stahl, Markierung, links
#define EMERALD_STEEL_MARKER_RIGHT              0x00B2	          // Stahl, Markierung, rechts
#define EMERALD_STEEL_MARKER_LEFT_BOTTOM        0x00B3	          // Stahl, Markierung, links unten
#define EMERALD_STEEL_MARKER_BOTTOM             0x00B4	          // Stahl, Markierung, unten
#define EMERALD_STEEL_MARKER_RIGHT_BOTTOM       0x00B5	          // Stahl, Markierung, rechts unten
#define EMERALD_STEEL_HEART                     0x00B6            // Stahl, Herz
#define EMERALD_STEEL_PLAYERHEAD                0x00B7            // Stahl, Spielerkopf
#define EMERALD_STEEL_NO_ENTRY                  0x00B8            // Stahl, Einfahrt verboten
#define EMERALD_STEEL_GIVE_WAY                  0x00B9            // Stahl, Vorfahrt gewähren
#define EMERALD_STEEL_YING                      0x00BA            // Stahl, Ying
#define EMERALD_STEEL_WHEELCHAIR                0x00BB            // Stahl, Rollstuhl
#define EMERALD_STEEL_ARROW_DOWN                0x00BC            // Stahl, Pfeil runter
#define EMERALD_STEEL_ARROW_UP                  0x00BD            // Stahl, Pfeil rauf
#define EMERALD_STEEL_ARROW_LEFT                0x00BE            // Stahl, Pfeil links
#define EMERALD_STEEL_ARROW_RIGHT               0x00BF            // Stahl, Pfeil rechts
#define EMERALD_STEEL_INVISIBLE                 0x00C0            // Stahl, unsichtbar / beleuchtet
#define EMERALD_WALL_INVISIBLE                  0x00C1            // Mauer, unsichtbar / beleuchtet
#define EMERALD_WALL_WITH_CRYSTAL               0x00C2            // Mauer mit Kristall
#define EMERALD_WALL_WITH_KEY_RED               0x00C3            // Mauer mit rotem Schlüssel
#define EMERALD_WALL_WITH_KEY_GREEN             0x00C4            // Mauer mit grünem Schlüssel
#define EMERALD_WALL_WITH_KEY_BLUE              0x00C5            // Mauer mit blauem Schlüssel
#define EMERALD_WALL_WITH_KEY_YELLOW            0x00C6            // Mauer mit gelbem Schlüssel
#define EMERALD_WALL_WITH_KEY_WHITE             0x00C7            // Mauer mit weißem Schlüssel
#define EMERALD_WALL_WITH_KEY_GENERAL           0x00C8            // Mauer mit Generalschlüssel
#define EMERALD_WALL_WITH_BOMB                  0x00C9            // Mauer mit Bombe
#define EMERALD_WALL_WITH_MEGABOMB              0x00CA            // Mauer mit Mega-Bombe
#define EMERALD_WALL_WITH_STONE                 0x00CB            // Mauer mit Stein
#define EMERALD_WALL_WITH_NUT                   0x00CC            // Mauer mit Nuss
#define EMERALD_WALL_WITH_WHEEL                 0x00CD            // Mauer mit Wagenrad
#define EMERALD_WALL_WITH_DYNAMITE              0x00CE            // Mauer mit Dynamit
#define EMERALD_WALL_WITH_ENDDOOR               0x00CF            // Mauer mit Endtür
#define EMERALD_WALL_WITH_ENDDOOR_READY         0x00D0            // Mauer mit Endtür, blinkend
#define EMERALD_WALL_WITH_MINE_UP               0x00D1            // Mauer mit Mine hoch
#define EMERALD_WALL_WITH_MOLE_UP               0x00D2            // Mauer mit Maulwurf hoch
#define EMERALD_WALL_WITH_GREEN_CHEESE          0x00D3            // Mauer mit grünem Käse
#define EMERALD_WALL_WITH_BEETLE_UP             0x00D4            // Mauer mit Käfer hoch
#define EMERALD_WALL_WITH_YAM                   0x00D5            // Mauer mit Yam
#define EMERALD_WALL_WITH_ALIEN                 0x00D6            // Mauer mit Alien
#define EMERALD_FONT_START                      0x0100            // Start des blauen Zeichensatzes
#define EMERALD_FONT_EXCLAMATION                0x0100            // !
#define EMERALD_FONT_ARROW_RIGHT                0x0101            // Pfeil rechts
#define EMERALD_FONT_ARROW_UP                   0x0102            // Pfeil hoch
#define EMERALD_FONT_ARROW_DOWN                 0x0103            // Pfeil runter
#define EMERALD_FONT_APOSTROPHE                 0x0104            // '
#define EMERALD_FONT_BRACE_OPEN                 0x0105            // (
#define EMERALD_FONT_BRACE_CLOSE                0x0106            // )
#define EMERALD_FONT_COPYRIGHT                  0x0107            // Copyright
#define EMERALD_FONT_PLUS                       0x0108            // +
#define EMERALD_FONT_COMMA                      0x0109            // ,
#define EMERALD_FONT_MINUS                      0x010A            // -
#define EMERALD_FONT_POINT                      0x010B            // .
#define EMERALD_FONT_SLASH                      0x010C            // /
#define EMERALD_FONT_0                          0x010D            // 0
#define EMERALD_FONT_1                          0x010E            // 1
#define EMERALD_FONT_2                          0x010F            // 2
#define EMERALD_FONT_3                          0x0110            // 3
#define EMERALD_FONT_4                          0x0111            // 4
#define EMERALD_FONT_5                          0x0112            // 5
#define EMERALD_FONT_6                          0x0113            // 6
#define EMERALD_FONT_7                          0x0114            // 7
#define EMERALD_FONT_8                          0x0115            // 8
#define EMERALD_FONT_9                          0x0116            // 9
#define EMERALD_FONT_DOUBLE_POINT               0x0117            // :
#define EMERALD_FONT_PLATE                      0x0118            // Plate
#define EMERALD_FONT_ARROW_LEFT                 0x0119            // Pfeil links
#define EMERALD_FONT_PAFF                       0x011A            // Paff, der Stofftieraffe
#define EMERALD_FONT_QUESTION_MARK              0x011B            // ?
#define EMERALD_FONT_A                          0x011C            // A
#define EMERALD_FONT_B                          0x011D            // B
#define EMERALD_FONT_C                          0x011E            // C
#define EMERALD_FONT_D                          0x011F            // D
#define EMERALD_FONT_E                          0x0120            // E
#define EMERALD_FONT_F                          0x0121            // F
#define EMERALD_FONT_G                          0x0122            // G
#define EMERALD_FONT_H                          0x0123            // H
#define EMERALD_FONT_I                          0x0124            // I
#define EMERALD_FONT_J                          0x0125            // J
#define EMERALD_FONT_K                          0x0126            // K
#define EMERALD_FONT_L                          0x0127            // L
#define EMERALD_FONT_M                          0x0128            // M
#define EMERALD_FONT_N                          0x0129            // N
#define EMERALD_FONT_O                          0x012A            // O
#define EMERALD_FONT_P                          0x012B            // P
#define EMERALD_FONT_Q                          0x012C            // Q
#define EMERALD_FONT_R                          0x012D            // R
#define EMERALD_FONT_S                          0x012E            // S
#define EMERALD_FONT_T                          0x012F            // T
#define EMERALD_FONT_U                          0x0130            // U
#define EMERALD_FONT_V                          0x0131            // V
#define EMERALD_FONT_W                          0x0132            // W
#define EMERALD_FONT_X                          0x0133            // X
#define EMERALD_FONT_Y                          0x0134            // Y
#define EMERALD_FONT_Z                          0x0135            // Z
#define EMERALD_FONT_AE                         0x0136            // Ä
#define EMERALD_FONT_OE                         0x0137            // Ö
#define EMERALD_FONT_UE                         0x0138            // Ü
#define EMERALD_FONT_STEEL_EXCLAMATION          0x0139            // !, Ab hier blauer Zeichensatz in Stahl
#define EMERALD_FONT_STEEL_ARROW_RIGHT          0x013A            // Pfeil rechts
#define EMERALD_FONT_STEEL_ARROW_UP             0x013B            // Pfeil hoch
#define EMERALD_FONT_STEEL_ARROW_DOWN           0x013C            // Pfeil runter
#define EMERALD_FONT_STEEL_APOSTROPHE           0x013D            // '
#define EMERALD_FONT_STEEL_BRACE_OPEN           0x013E            // (
#define EMERALD_FONT_STEEL_BRACE_CLOSE          0x013F            // )
#define EMERALD_FONT_STEEL_COPYRIGHT            0x0140            // Copyright
#define EMERALD_FONT_STEEL_PLUS                 0x0141            // +
#define EMERALD_FONT_STEEL_COMMA                0x0142            // ,
#define EMERALD_FONT_STEEL_MINUS                0x0143            // -
#define EMERALD_FONT_STEEL_POINT                0x0144            // .
#define EMERALD_FONT_STEEL_SLASH                0x0145            // /
#define EMERALD_FONT_STEEL_0                    0x0146            // 0
#define EMERALD_FONT_STEEL_1                    0x0147            // 1
#define EMERALD_FONT_STEEL_2                    0x0148            // 2
#define EMERALD_FONT_STEEL_3                    0x0149            // 3
#define EMERALD_FONT_STEEL_4                    0x014A            // 4
#define EMERALD_FONT_STEEL_5                    0x014B            // 5
#define EMERALD_FONT_STEEL_6                    0x014C            // 6
#define EMERALD_FONT_STEEL_7                    0x014D            // 7
#define EMERALD_FONT_STEEL_8                    0x014E            // 8
#define EMERALD_FONT_STEEL_9                    0x014F            // 9
#define EMERALD_FONT_STEEL_DOUBLE_POINT         0x0150            // :
#define EMERALD_FONT_STEEL_PLATE                0x0151            // Plate
#define EMERALD_FONT_STEEL_ARROW_LEFT           0x0152            // Pfeil links
#define EMERALD_FONT_STEEL_QUESTION_MARK        0x0153            // ?
#define EMERALD_FONT_STEEL_A                    0x0154            // A
#define EMERALD_FONT_STEEL_B                    0x0155            // B
#define EMERALD_FONT_STEEL_C                    0x0156            // C
#define EMERALD_FONT_STEEL_D                    0x0157            // D
#define EMERALD_FONT_STEEL_E                    0x0158            // E
#define EMERALD_FONT_STEEL_F                    0x0159            // F
#define EMERALD_FONT_STEEL_G                    0x015A            // G
#define EMERALD_FONT_STEEL_H                    0x015B            // H
#define EMERALD_FONT_STEEL_I                    0x015C            // I
#define EMERALD_FONT_STEEL_J                    0x015D            // J
#define EMERALD_FONT_STEEL_K                    0x015E            // K
#define EMERALD_FONT_STEEL_L                    0x015F            // L
#define EMERALD_FONT_STEEL_M                    0x0160            // M
#define EMERALD_FONT_STEEL_N                    0x0161            // N
#define EMERALD_FONT_STEEL_O                    0x0162            // O
#define EMERALD_FONT_STEEL_P                    0x0163            // P
#define EMERALD_FONT_STEEL_Q                    0x0164            // Q
#define EMERALD_FONT_STEEL_R                    0x0165            // R
#define EMERALD_FONT_STEEL_S                    0x0166            // S
#define EMERALD_FONT_STEEL_T                    0x0167            // T
#define EMERALD_FONT_STEEL_U                    0x0168            // U
#define EMERALD_FONT_STEEL_V                    0x0169            // V
#define EMERALD_FONT_STEEL_W                    0x016A            // W
#define EMERALD_FONT_STEEL_X                    0x016B            // X
#define EMERALD_FONT_STEEL_Y                    0x016C            // Y
#define EMERALD_FONT_STEEL_Z                    0x016D            // Z
#define EMERALD_FONT_STEEL_AE                   0x016E            // Ä
#define EMERALD_FONT_STEEL_OE                   0x016F            // Ö
#define EMERALD_FONT_STEEL_UE                   0x0170            // Ü
#define EMERALD_FONT_GREEN_EXCLAMATION          0x0171            // !, Ab hier grüner Zeichensatz
#define EMERALD_FONT_GREEN_ARROW_RIGHT          0x0172            // Pfeil rechts
#define EMERALD_FONT_GREEN_ARROW_UP             0x0173            // Pfeil hoch
#define EMERALD_FONT_GREEN_ARROW_DOWN           0x0174            // Pfeil runter
#define EMERALD_FONT_GREEN_APOSTROPHE           0x0175            // '
#define EMERALD_FONT_GREEN_BRACE_OPEN           0x0176            // (
#define EMERALD_FONT_GREEN_BRACE_CLOSE          0x0177            // )
#define EMERALD_FONT_GREEN_COPYRIGHT            0x0178            // Copyright
#define EMERALD_FONT_GREEN_PLUS                 0x0179            // +
#define EMERALD_FONT_GREEN_COMMA                0x017A            // ,
#define EMERALD_FONT_GREEN_MINUS                0x017B            // -
#define EMERALD_FONT_GREEN_POINT                0x017C            // .
#define EMERALD_FONT_GREEN_SLASH                0x017D            // /
#define EMERALD_FONT_GREEN_0                    0x017E            // 0
#define EMERALD_FONT_GREEN_1                    0x017F            // 1
#define EMERALD_FONT_GREEN_2                    0x0180            // 2
#define EMERALD_FONT_GREEN_3                    0x0181            // 3
#define EMERALD_FONT_GREEN_4                    0x0182            // 4
#define EMERALD_FONT_GREEN_5                    0x0183            // 5
#define EMERALD_FONT_GREEN_6                    0x0184            // 6
#define EMERALD_FONT_GREEN_7                    0x0185            // 7
#define EMERALD_FONT_GREEN_8                    0x0186            // 8
#define EMERALD_FONT_GREEN_9                    0x0187            // 9
#define EMERALD_FONT_GREEN_DOUBLE_POINT         0x0188            // :
#define EMERALD_FONT_GREEN_PLATE                0x0189            // Plate
#define EMERALD_FONT_GREEN_ARROW_LEFT           0x018A            // Pfeil links
#define EMERALD_FONT_GREEN_QUESTION_MARK        0x018B            // ?
#define EMERALD_FONT_GREEN_A                    0x018C            // A
#define EMERALD_FONT_GREEN_B                    0x018D            // B
#define EMERALD_FONT_GREEN_C                    0x018E            // C
#define EMERALD_FONT_GREEN_D                    0x018F            // D
#define EMERALD_FONT_GREEN_E                    0x0190            // E
#define EMERALD_FONT_GREEN_F                    0x0191            // F
#define EMERALD_FONT_GREEN_G                    0x0192            // G
#define EMERALD_FONT_GREEN_H                    0x0193            // H
#define EMERALD_FONT_GREEN_I                    0x0194            // I
#define EMERALD_FONT_GREEN_J                    0x0195            // J
#define EMERALD_FONT_GREEN_K                    0x0196            // K
#define EMERALD_FONT_GREEN_L                    0x0197            // L
#define EMERALD_FONT_GREEN_M                    0x0198            // M
#define EMERALD_FONT_GREEN_N                    0x0199            // N
#define EMERALD_FONT_GREEN_O                    0x019A            // O
#define EMERALD_FONT_GREEN_P                    0x019B            // P
#define EMERALD_FONT_GREEN_Q                    0x019C            // Q
#define EMERALD_FONT_GREEN_R                    0x019D            // R
#define EMERALD_FONT_GREEN_S                    0x019E            // S
#define EMERALD_FONT_GREEN_T                    0x019F            // T
#define EMERALD_FONT_GREEN_U                    0x01A0            // U
#define EMERALD_FONT_GREEN_V                    0x01A1            // V
#define EMERALD_FONT_GREEN_W                    0x01A2            // W
#define EMERALD_FONT_GREEN_X                    0x01A3            // X
#define EMERALD_FONT_GREEN_Y                    0x01A4            // Y
#define EMERALD_FONT_GREEN_Z                    0x01A5            // Z
#define EMERALD_FONT_GREEN_AE                   0x01A6            // Ä
#define EMERALD_FONT_GREEN_OE                   0x01A7            // Ö
#define EMERALD_FONT_GREEN_UE                   0x01A8            // Ü
#define EMERALD_FONT_STEEL_GREEN_EXCLAMATION    0x01A9            // !, Ab hier grüner Zeichensatz in Stahl
#define EMERALD_FONT_STEEL_GREEN_ARROW_RIGHT    0x01AA            // Pfeil rechts
#define EMERALD_FONT_STEEL_GREEN_ARROW_UP       0x01AB            // Pfeil hoch
#define EMERALD_FONT_STEEL_GREEN_ARROW_DOWN     0x01AC            // Pfeil runter
#define EMERALD_FONT_STEEL_GREEN_APOSTROPHE     0x01AD            // '
#define EMERALD_FONT_STEEL_GREEN_BRACE_OPEN     0x01AE            // (
#define EMERALD_FONT_STEEL_GREEN_BRACE_CLOSE    0x01AF            // )
#define EMERALD_FONT_STEEL_GREEN_COPYRIGHT      0x01B0            // Copyright
#define EMERALD_FONT_STEEL_GREEN_PLUS           0x01B1            // +
#define EMERALD_FONT_STEEL_GREEN_COMMA          0x01B2            // ,
#define EMERALD_FONT_STEEL_GREEN_MINUS          0x01B3            // -
#define EMERALD_FONT_STEEL_GREEN_POINT          0x01B4            // .
#define EMERALD_FONT_STEEL_GREEN_SLASH          0x01B5            // /
#define EMERALD_FONT_STEEL_GREEN_0              0x01B6            // 0
#define EMERALD_FONT_STEEL_GREEN_1              0x01B7            // 1
#define EMERALD_FONT_STEEL_GREEN_2              0x01B8            // 2
#define EMERALD_FONT_STEEL_GREEN_3              0x01B9            // 3
#define EMERALD_FONT_STEEL_GREEN_4              0x01BA            // 4
#define EMERALD_FONT_STEEL_GREEN_5              0x01BB            // 5
#define EMERALD_FONT_STEEL_GREEN_6              0x01BC            // 6
#define EMERALD_FONT_STEEL_GREEN_7              0x01BD            // 7
#define EMERALD_FONT_STEEL_GREEN_8              0x01BE            // 8
#define EMERALD_FONT_STEEL_GREEN_9              0x01BF            // 9
#define EMERALD_FONT_STEEL_GREEN_DOUBLE_POINT   0x01C0            // :
#define EMERALD_FONT_STEEL_GREEN_PLATE          0x01C1            // Plate
#define EMERALD_FONT_STEEL_GREEN_ARROW_LEFT     0x01C2            // Pfeil links
#define EMERALD_FONT_STEEL_GREEN_QUESTION_MARK  0x01C3            // ?
#define EMERALD_FONT_STEEL_GREEN_A              0x01C4            // A
#define EMERALD_FONT_STEEL_GREEN_B              0x01C5            // B
#define EMERALD_FONT_STEEL_GREEN_C              0x01C6            // C
#define EMERALD_FONT_STEEL_GREEN_D              0x01C7            // D
#define EMERALD_FONT_STEEL_GREEN_E              0x01C8            // E
#define EMERALD_FONT_STEEL_GREEN_F              0x01C9            // F
#define EMERALD_FONT_STEEL_GREEN_G              0x01CA            // G
#define EMERALD_FONT_STEEL_GREEN_H              0x01CB            // H
#define EMERALD_FONT_STEEL_GREEN_I              0x01CC            // I
#define EMERALD_FONT_STEEL_GREEN_J              0x01CD            // J
#define EMERALD_FONT_STEEL_GREEN_K              0x01CE            // K
#define EMERALD_FONT_STEEL_GREEN_L              0x01CF            // L
#define EMERALD_FONT_STEEL_GREEN_M              0x01D0            // M
#define EMERALD_FONT_STEEL_GREEN_N              0x01D1            // N
#define EMERALD_FONT_STEEL_GREEN_O              0x01D2            // O
#define EMERALD_FONT_STEEL_GREEN_P              0x01D3            // P
#define EMERALD_FONT_STEEL_GREEN_Q              0x01D4            // Q
#define EMERALD_FONT_STEEL_GREEN_R              0x01D5            // R
#define EMERALD_FONT_STEEL_GREEN_S              0x01D6            // S
#define EMERALD_FONT_STEEL_GREEN_T              0x01D7            // T
#define EMERALD_FONT_STEEL_GREEN_U              0x01D8            // U
#define EMERALD_FONT_STEEL_GREEN_V              0x01D9            // V
#define EMERALD_FONT_STEEL_GREEN_W              0x01DA            // W
#define EMERALD_FONT_STEEL_GREEN_X              0x01DB            // X
#define EMERALD_FONT_STEEL_GREEN_Y              0x01DC            // Y
#define EMERALD_FONT_STEEL_GREEN_Z              0x01DD            // Z
#define EMERALD_FONT_STEEL_GREEN_AE             0x01DE            // Ä
#define EMERALD_FONT_STEEL_GREEN_OE             0x01DF            // Ö
#define EMERALD_FONT_STEEL_GREEN_UE             0x01E0            // Ü
#define EMERALD_CENTRAL_EXPLOSION               0x01E1            // Mittelpunkt einer (Bomben)-Explosion
#define EMERALD_CENTRAL_EXPLOSION_MEGA          0x01E2            // Mittelpunkt einer Mega-Bomben-Explosion
#define EMERALD_CENTRAL_EXPLOSION_BEETLE        0x01E3            // Mittelpunkt einer Käferexplosion
#define EMERALD_MAN_DIES                        0x01E4            // Man stirbt
#define EMERALD_ALIEN_KILLS_MAN                 0x01E5            // Alien tötet Man
#define EMERALD_YAM_KILLS_MAN                   0x01E6            // Yam tötet Man
#define EMERALD_WALL_WITH_TIME_COIN             0x01E7            // Mauer mit Zeitmünze
#define EMERALD_STEEL_TRASHCAN                  0x01E8            // Stahl Mülleimer
#define EMERALD_STEEL_JOYSTICK                  0x01E9            // Stahl Joystick
#define EMERALD_STEEL_EDIT_LEVEL                0x01EA            // Stahl Level editieren
#define EMERALD_STEEL_MOVE_LEVEL                0x01EB            // Stahl Level verschieben
#define EMERALD_STEEL_ADD_LEVELGROUP            0x01EC            // Stahl erzeuge neue Levelgruppe
#define EMERALD_STEEL_COPY_LEVEL                0x01ED            // Stahl Level kopieren
#define EMERALD_STEEL_CLIPBOARD_LEVEL           0x01EE            // Stahl Clipboard-Level
#define EMERALD_STEEL_DC3_IMPORT                0x01EF            // Stahl DC3 (Diamond Caves 3)-Level-Import
#define EMERALD_STEEL_RENAME_LEVELGROUP         0x01F0            // Stahl Levelgruppe umbenennen
#define EMERALD_STEEL_PASSWORD                  0x01F1            // Stahl Passwort
#define EMERALD_GRASS                           0x01F2            // Gras, kann sich ähnlich wie grüner Käse ausbreiten
#define EMERALD_GRASS_COMES                     0x01F3            // Gras, welches gerade entsteht
#define EMERALD_SANDMINE                        0x01F4            // Sandmine
#define EMERALD_SAND_INVISIBLE                  0x01F5            // unsichtbarer Sand
#define EMERALD_CONVEYORBELT_RED                0x01F6            // Laufband, rot
#define EMERALD_CONVEYORBELT_SWITCH_RED         0x01F7            // Laufband-Schalter, rot
#define EMERALD_CONVEYORBELT_GREEN              0x01F8            // Laufband, grün
#define EMERALD_CONVEYORBELT_SWITCH_GREEN       0x01F9            // Laufband-Schalter, grün
#define EMERALD_CONVEYORBELT_BLUE               0x01FA            // Laufband, blau
#define EMERALD_CONVEYORBELT_SWITCH_BLUE        0x01FB            // Laufband-Schalter, blau
#define EMERALD_CONVEYORBELT_YELLOW             0x01FC            // Laufband, gelb
#define EMERALD_CONVEYORBELT_SWITCH_YELLOW      0x01FD            // Laufband-Schalter, gelb
#define EMERALD_LEVELEDITOR_MESSAGE_1_4         0x01FE            // Leveleditor-Message 1/4, wenn fenster zu klein
#define EMERALD_LEVELEDITOR_MESSAGE_2_4         0x01FF            // Leveleditor-Message 2/4, wenn fenster zu klein
#define EMERALD_LEVELEDITOR_MESSAGE_3_4         0x0200            // Leveleditor-Message 3/4, wenn fenster zu klein
#define EMERALD_LEVELEDITOR_MESSAGE_4_4         0x0201            // Leveleditor-Message 4/4, wenn fenster zu klein
#define EMERALD_STEEL_GROW_LEFT                 0x0202            // Stahl, wächst links
#define EMERALD_STEEL_GROW_RIGHT                0x0203            // Stahl, wächst rechts
#define EMERALD_STEEL_GROW_UP                   0x0204            // Stahl, wächst hoch
#define EMERALD_STEEL_GROW_DOWN                 0x0205            // Stahl, wächst runter
#define EMERALD_STEEL_GROW_LEFT_RIGHT           0x0206            // Stahl, wächst links und rechts
#define EMERALD_STEEL_GROW_UP_DOWN              0x0207            // Stahl, wächst hoch und runter
#define EMERALD_STEEL_GROW_ALL                  0x0208            // Stahl, wächst in alle Richtungen
#define EMERALD_STEEL_GROWING_LEFT              0x0209            // Neuer links wachsender Stahl
#define EMERALD_STEEL_GROWING_RIGHT             0x020A            // Neuer rechts wachsender Stahl
#define EMERALD_STEEL_GROWING_UP                0x020B            // Neuer hoch wachsender Stahl
#define EMERALD_STEEL_GROWING_DOWN              0x020C            // Neuer runter wachsender Stahl
#define EMERALD_WALL_GROW_LEFT                  0x020D            // Mauer, wächst links
#define EMERALD_WALL_GROW_RIGHT                 0x020E            // Mauer, wächst rechts
#define EMERALD_WALL_GROW_UP                    0x020F            // Mauer, wächst hoch
#define EMERALD_WALL_GROW_DOWN                  0x0210            // Mauer, wächst runter
#define EMERALD_WALL_GROW_LEFT_RIGHT            0x0211            // Mauer, wächst links und rechts
#define EMERALD_WALL_GROW_UP_DOWN               0x0212            // Mauer, wächst hoch und runter
#define EMERALD_WALL_GROW_ALL                   0x0213            // Mauer, wächst in alle Richtungen
#define EMERALD_WALL_GROWING_LEFT               0x0214            // Neue links wachsende Mauer
#define EMERALD_WALL_GROWING_RIGHT              0x0215            // Neue rechts wachsende Mauer
#define EMERALD_WALL_GROWING_UP                 0x0216            // Neue hoch wachsende Mauer
#define EMERALD_WALL_GROWING_DOWN               0x0217            // Neue runter wachsende Mauer
#define EMERALD_DOOR_ONLY_UP_WALL               0x0218            // Tür, nur nach oben passierbar, sprengbar
#define EMERALD_DOOR_ONLY_DOWN_WALL             0x0219            // Tür, nur nach unten passierbar, sprengbar
#define EMERALD_DOOR_ONLY_LEFT_WALL             0x021A            // Tür, nur nach links passierbar, sprengbar
#define EMERALD_DOOR_ONLY_RIGHT_WALL            0x021B            // Tür, nur nach rechts passierbar, sprengbar
#define EMERALD_TELEPORTER_RED                  0x021C            // Teleporter, rot
#define EMERALD_TELEPORTER_YELLOW               0x021D            // Teleporter, gelb
#define EMERALD_TELEPORTER_GREEN                0x021E            // Teleporter, grün
#define EMERALD_TELEPORTER_BLUE                 0x021F            // Teleporter, blau
#define EMERALD_SWITCH_REMOTEBOMB_UP            0x0220            // ferngesteuerte Bombe, Schalter hoch
#define EMERALD_SWITCH_REMOTEBOMB_DOWN          0x0221            // ferngesteuerte Bombe, Schalter runter
#define EMERALD_SWITCH_REMOTEBOMB_LEFT          0x0222            // ferngesteuerte Bombe, Schalter links
#define EMERALD_SWITCH_REMOTEBOMB_RIGHT         0x0223            // ferngesteuerte Bombe, Schalter rechts
#define EMERALD_SWITCH_REMOTEBOMB_IGNITION      0x0224            // ferngesteuerte Bombe, Schalter Zündung
#define EMERALD_REMOTEBOMB                      0x0225            // ferngesteuerte Bombe
#define EMERALD_STEEL_MODERN_LEFT_END           0x0226            // Stahl, modern, linkes Ende
#define EMERALD_STEEL_MODERN_LEFT_RIGHT         0x0227            // Stahl, modern, links/rechts
#define EMERALD_STEEL_MODERN_RIGHT_END          0x0228            // Stahl, modern, rechtes Ende
#define EMERALD_STEEL_MODERN_UP_END             0x0229            // Stahl, modern, oberes Ende
#define EMERALD_STEEL_MODERN_UP_DOWN            0x022A            // Stahl, modern, oben/unten
#define EMERALD_STEEL_MODERN_DOWN_END           0x022B            // Stahl, modern, unteres Ende
#define EMERALD_STEEL_MODERN_MIDDLE             0x022C            // Stahl, modern, Mittelteil

#define EMERALD_MAX_ELEMENT                     0x022C            // hier immer das letzte Element eintragen (für ControlExplosionToElement())
#define EMERALD_INVALID                         0xFFFF            // ungültiges Element

#define EMERALD_FONT_BLUE                       0x00              // Bit 0 = 1 = Stahl, Bit 1 = Farbe (0 = blau, 1 = grün)
#define EMERALD_FONT_STEEL_BLUE                 0x01
#define EMERALD_FONT_GREEN                      0x10
#define EMERALD_FONT_STEEL_GREEN                0x11

// Status der Laufbänder
#define EMERALD_CONVEYBELT_OFF                  0
#define EMERALD_CONVEYBELT_LEFT                 1
#define EMERALD_CONVEYBELT_RIGHT                2
// Schalt-Richtungen für Laufband-Umschalter
#define EMERALD_CONVEYBELT_TO_LEFT              0
#define EMERALD_CONVEYBELT_TO_RIGHT             1


// Animationen (für Explosionen wird das niederwertigste uint16_t verwendet)
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
#define EMERALD_ANIM_MOLE_STAND                 0x00001100      // Mole "läuft" auf der Stelle
#define EMERALD_STATUS_DOOR_OPEN                0x00001200      // Schalt- und Zeit-Tür ist offen
#define EMERALD_STATUS_DOOR_CLOSE               0x00001300      // Schalt- und Zeit-Tür ist geschlossen
#define EMERALD_ANIM_SAND_0                     0x00001400      // Sand-Ränder 0 / F
// Elemente zwischen 0x00001400 und 0x00002300 sind entsprechend besetzt
#define EMERALD_ANIM_SAND_F                     0x00002300      // Sand-Ränder F / F


#define EMERALD_ANIM_CLEAN_UP                   0x00010000      // über dem invaliden Feld löschen
#define EMERALD_ANIM_CLEAN_RIGHT                0x00020000      // rechts neben invaliden Feld löschen
#define EMERALD_ANIM_CLEAN_DOWN                 0x00030000      // unter dem invaliden Feld löschen
#define EMERALD_ANIM_CLEAN_LEFT                 0x00040000      // links neben invaliden Feld löschen
#define EMERALD_ANIM_CLEAN_UP_DOUBLESPEED       0x00050000      // 2 Felder über dem invaliden Feld löschen
#define EMERALD_ANIM_CLEAN_RIGHT_DOUBLESPEED    0x00060000      // 2 Felder rechts neben invaliden Feld löschen
#define EMERALD_ANIM_CLEAN_DOWN_DOUBLESPEED     0x00070000      // 2 Felder unter dem invaliden Feld löschen
#define EMERALD_ANIM_CLEAN_LEFT_DOUBLESPEED     0x00080000      // 2 Felder links neben invaliden Feld löschen
#define EMERALD_ANIM_CLEAN_NOTHING              0x00090000      // Gar nichts löschen/setzen, wird z.B. für grünen Tropfen in Phase 1 benötigt

#define EMERALD_ANIM_AVOID_DOUBLE_CONTROL       0x00800000      // Element nicht doppelt steuern (Element wird z.B. I + X gesetzt und durch ControlGame(I) nachgesteuet)


// Letzte YAM-Richtung
#define EMERALD_LAST_YAM_DIR_BLOCKED            0               // blockiert
#define EMERALD_LAST_YAM_DIR_UP                 1               // hoch
#define EMERALD_LAST_YAM_DIR_RIGHT              2               // rechts
#define EMERALD_LAST_YAM_DIR_DOWN               3               // runter
#define EMERALD_LAST_YAM_DIR_LEFT               4               // links

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
#define EMERALD_ANIM_PERL_SHRINK                0x20000000      // Perle wird kleiner
#define EMERALD_ANIM_CRYSTAL_SHRINK             0x21000000      // Kristall wird kleiner
#define EMERALD_ANIM_TIME_COIN_SHRINK           0x22000000      // Zeitmünze wird kleiner
#define EMERALD_ANIM_HAMMER_SHRINK              0x23000000      // Hammer wird kleiner
#define EMERALD_ANIM_DYNAMITE_SHRINK            0x24000000      // Dynamit wird kleiner
#define EMERALD_ANIM_MESSAGE_SHRINK             0x25000000      // Briefumschlag wird kleiner
#define EMERALD_ANIM_GREEN_DROP_1               0x26000000      // grüner Tropfen, Phase 1
#define EMERALD_ANIM_GREEN_DROP_2               0x27000000      // grüner Tropfen, Phase 2
#define EMERALD_ANIM_PERL_BREAK                 0x28000000      // Perle zerbricht
#define EMERALD_ANIM_BLOCKED_MOLE_SPIN_RIGHT    0x29000000      // Blockierter Maulwurf drehte zuletzt nach rechts
#define EMERALD_ANIM_BLOCKED_MOLE_SPIN_LEFT     0x2A000000      // Blockierter Maulwurf drehte zuletzt nach links
#define EMERALD_ANIM_DOOR_OPEN                  0x2B000000      // Schalt- und Zeit-Tür wird geöffnet
#define EMERALD_ANIM_DOOR_CLOSE                 0x2C000000      // Schalt- und Zeit-Tür wird geschlossen
#define EMERALD_ANIM_KEY_WHITE_SHRINK           0x2D000000      // weißer Schlüssel wird keiner
#define EMERALD_ANIM_KEY_GENERAL_SHRINK         0x2E000000      // General-Schlüssel wird keiner
#define EMERALD_ANIM_BLOCK_MAN                  0x2F000000      // Diese Animation blockiert Man, wird i.d.R. dem Element EMERALD_SPACE gegeben
#define EMERALD_ANIM_MOLE_GOES_ACID             0x30000000      // Mole fällt ins Säurebecken und muss beim Sterben noch Sand über dem Becken erzeugen
#define EMERALD_ANIM_MAN_GOES_ENDDOOR           0x31000000      // Man geht in eine Endtür
#define EMERALD_ANIM_DOOR_READY_SHRINK          0x32000000      // Endtür verschwindet
#define EMERALD_ANIM_DYNAMITE_START             0x33000000      // Dynamit wird aktiv durch manuelle Zündung (Man)
#define EMERALD_ANIM_DYNAMITE_ON_P1             0x34000000      // Dynamit aktiv, Phase 1
#define EMERALD_ANIM_DYNAMITE_ON_P2             0x35000000      // Dynamit aktiv, Phase 2
#define EMERALD_ANIM_DYNAMITE_ON_P3             0x36000000      // Dynamit aktiv, Phase 3
#define EMERALD_ANIM_DYNAMITE_ON_P4             0x37000000      // Dynamit aktiv, Phase 4
#define EMERALD_ANIM_MAN_DIES_P1                0x38000000      // Man stirbt, Phase 1
#define EMERALD_ANIM_MAN_DIES_P2                0x39000000      // Man stirbt, Phase 1
#define EMERALD_ANIM_MONSTER_KILLS_UP           0x3A000000      // Monster (Alien oder Yam) tötet nach oben
#define EMERALD_ANIM_MONSTER_KILLS_LEFT         0x3B000000      // Monster (Alien oder Yam) tötet nach links
#define EMERALD_ANIM_MONSTER_KILLS_RIGHT        0x3C000000      // Monster (Alien oder Yam) tötet nach rechts
#define EMERALD_ANIM_MONSTER_KILLS_DOWN         0x3D000000      // Monster (Alien oder Yam) tötet nach unten
#define EMERALD_ANIM_YAM_WAS_BLOCKED            0x3E000000      // Yam war in der letzten Control-Phase blockiert
#define EMERALD_ANIM_MINE_WILL_EXPLODE          0x3F000000      // Mine wird nächste Kontrollrunde sprengen
#define EMERALD_ANIM_BEETLE_WILL_EXPLODE        0x40000000      // Käfer wird nächste Kontrollrunde sprengen
#define EMERALD_ANIM_GRASS_SHRINK               0x41000000      // Gras verschwindet
#define EMERALD_ANIM_SAND_INVISIBLE_SHRINK      0x42000000      // unsichtbarer Sand verschwindet
#define EMERALD_ANIM_MAN_BLOCKED_LEFT           0x43000000      // Man will nach links, ist aber blockiert (Türen, Teleporter)
#define EMERALD_ANIM_MAN_BLOCKED_UP             0x44000000      // Man will nach oben, ist aber blockiert (Türen, Teleporter)
#define EMERALD_ANIM_MAN_BLOCKED_RIGHT          0x45000000      // Man will nach rechts, ist aber blockiert (Türen, Teleporter)
#define EMERALD_ANIM_MAN_BLOCKED_DOWN           0x46000000      // Man will nach unten, ist aber blockiert (Türen, Teleporter)



#define EMERALD_STANDARD_SPEED                  false
#define EMERALD_DOUBLE_SPEED                    true

#define EMERALD_MAX_MESSAGES                    8

 // Bit 0 = Emerald, Bit 1 = Saphir, Bit 2 = Stone, Bit 3 = Nut, Bit 4 = Bomb, Bit 5 = Rubin, Bit 6 = Kristall, Bit 7 =  Perle, Bit 8 = Megabombe
#define EMERALD_CHECKROLL_EMERALD               0x01
#define EMERALD_CHECKROLL_SAPPHIRE              0x02
#define EMERALD_CHECKROLL_STONE                 0x04
#define EMERALD_CHECKROLL_NUT                   0x08
#define EMERALD_CHECKROLL_BOMB                  0x10
#define EMERALD_CHECKROLL_RUBY                  0x20
#define EMERALD_CHECKROLL_CRYSTAL               0x40
#define EMERALD_CHECKROLL_PERL                  0x80
#define EMERALD_CHECKROLL_MEGABOMB              0x100

#define EMERALD_INVALID_COORDINATE              0xFFFFFFFF


typedef struct {
    // Datentypen sind hier etwas geiziger ausgelegt, sollten aber immer noch locker ausreichen
    uint16_t        uTextureIndex;                              // Start-Texture zur Animation
    uint16_t        uX;                                         // X-Koordinate, wo Post-Animation durchgeführt werden soll.
    uint16_t        uY;                                         // Y-Koordinate, wo Post-Animation durchgeführt werden soll.
    bool            bFlipVertical;                              // Texture vertikal spiegeln
    bool            bFlipHorizontal;                            // Texture horizontal spiegeln
} POSTANIMATION;


typedef struct {
    uint16_t uElement[9];                                       // 1 YAM erzeugt maximal 9 neue Elemente/Objekte
} YAMEXPLOSION;


typedef struct {
    uint16_t        *pLevel;
    uint32_t        *pStatusAnimation;
    uint32_t        *pLastStatusAnimation;
    POSTANIMATION   *pPostAnimation;
    uint32_t        uRollUnderground[65536];                    // Bit 0 = Emerald, Bit 1 = Saphir, Bit 2 = Stone, Bit 3 = Nut, Bit 4 = Bomb, Bit 5 = Rubin, Bit 6 = Kristall, Bit 7 = Perle, Bit 8 = Megabombe
    uint16_t        *pInvalidElement;
    uint8_t         *pLastYamDirection;
    int             nCentralExplosionCoordinates[8];            // Koordinaten um Zentrum einer 3x3-Explosion
    int             nCentralMegaExplosionCoordinates[20];       // Koordinaten um Zentrum einer Mega-Explosion
    int             nCheckReplicatorForYamExplosionTop[5];      // Für Yam-Explosion mit Replikator (obere Hälfte)
    int             nCheckReplicatorForYamExplosionButtom[5];   // Für Yam-Explosion mit Replikator (untere Hälfte)
    int             nCheckAcidPoolForYamExplosionTop[6];        // Für Yam-Explosion mit Säurebecken (obere Hälfte)
    int             nCheckAcidPoolForYamExplosionButtom[6];     // Für Yam-Explosion mit Säurebecken (untere Hälfte)
    bool            bInitOK;
    bool            bReadyToGo;                                 // Man hat genug Diamanten eingesammelt
    bool            bHasRedKey;
    bool            bHasGreenKey;
    bool            bHasBlueKey;
    bool            bHasYellowKey;
    bool            bHasGeneralKey;
    bool            bWheelRunning;
    bool            bMagicWallRunning;
    bool            bMagicWallWasOn;                            // Magic Wall war mal an
    bool            bLightOn;                                   // Licht ist an -> unsichtbare Mauern/Stahl sind sichtbar
    bool            bPushStone;                                 // wenn true, kann Man sofort (Stone, Nut, Bomb) schieben
    bool            bManDead;
    bool            bWellDone;                                  // Level wurde geschafft
    bool            bLightBarrierRedOn;
    bool            bLightBarrierGreenOn;
    bool            bLightBarrierBlueOn;
    bool            bLightBarrierYellowOn;
    bool            bTimeDoorOpen;                              // Für Zeit-Tür
    bool            bSwitchDoorState;                           // Für Schaltertür, true = umgeschaltet
    bool            bSwitchDoorImpluse;                         // Für Schaltertür
    bool            bSwitchRemoteBombLeft;
    bool            bSwitchRemoteBombRight;
    bool            bSwitchRemoteBombDown;
    bool            bSwitchRemoteBombUp;
    bool            bSwitchRemoteBombIgnition;
    bool            bRemoteBombMoved;                           // Mindestens 1 ferngesteuerte Bombe wurde bewegt
    bool            bReplicatorRedOn;
    uint16_t        uReplicatorRedObject;
    bool            bReplicatorGreenOn;
    uint16_t        uReplicatorGreenObject;
    bool            bReplicatorBlueOn;
    uint16_t        uReplicatorBlueObject;
    bool            bReplicatorYellowOn;
    uint16_t        uReplicatorYellowObject;
    uint8_t         uConveybeltRedState;                        // CONVEYBELT_OFF, CONVEYBELT_LEFT, CONVEYBELT_RIGHT
    uint8_t         uConveybeltRedDirection;                    // EMERALD_CONVEYBELT_TO_LEFT, EMERALD_CONVEYBELT_TO_RIGHT
    uint8_t         uConveybeltGreenState;
    uint8_t         uConveybeltGreenDirection;
    uint8_t         uConveybeltBlueState;
    uint8_t         uConveybeltBlueDirection;
    uint8_t         uConveybeltYellowState;
    uint8_t         uConveybeltYellowDirection;
    char            szVersion[EMERALD_VERSION_LEN + 1];         // z.B. "01.00"
    char            szLevelTitle[EMERALD_TITLE_LEN + 1];        // z.B. "DER BUNKER"
    char            szLevelAuthor[EMERALD_AUTHOR_LEN + 1];      // z.B. "MIKIMAN"
    char            szMd5String[EMERALD_MD5_STRING_LEN + 1];    // MD5-Prüfsumme als String für komprimierte Leveldaten, d.h. ohne Header
    char            *pMessage[EMERALD_MAX_MESSAGES];            // 8 Nachrichtentexte
    uint32_t        uScoreEmerald;
    uint32_t        uScoreRuby;
    uint32_t        uScoreSaphir;
    uint32_t        uScorePerl;
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
    uint32_t        uGrassSpreadSpeed;
    uint32_t        uTimeToPlay;
    uint32_t        uAdditonalTimeCoinTime;                     // zusätzliche Zeit durch Zeitmünze
    uint32_t        uTimeWheelRotation;
    uint32_t        uTimeWheelRotationLeft;                     // Verbleibende Zeit für Wheel-Rotation
    uint32_t        uTimeDoorTime;
    uint32_t        uTimeDoorTimeLeft;
    uint32_t        uWheelRunningXpos;                          // nur gültig, wenn bWheelRunning = true
    uint32_t        uWheelRunningYpos;                          // nur gültig, wenn bWheelRunning = true
    uint32_t        uTimeMagicWall;
    uint32_t        uTimeMagicWallLeft;                         // Verbleibende Zeit für Magic-Wall
    uint32_t        uTimeLight;
    uint32_t        uTimeLightLeft;                             // Verbleibende Zeit für Licht
    uint32_t        uDynamiteCount;                             // Anzahl Dynamits, die der Man hat
    uint32_t        uHammerCount;                               // Anzahl Hammer, die der Man hat
    uint32_t        uWhiteKeyCount;                             // Anzahl der weißen Schlüssel, die der Man hat
    uint32_t        uLevel_X_Dimension;
    uint32_t        uLevel_Y_Dimension;
    int             nTopLeftXpos;                               // aktuelle X-Pixel-Position (obere linke sichtbare Ecke)
    int             nTopLeftYpos;                               // aktuelle Y-Pixel-Position (obere linke sichtbare Ecke)
    uint32_t        uTotalScore;
    uint32_t        uVisibleX;                                  // Anzahl der sichtbaren Elemente-Spalten
    uint32_t        uVisibleY;                                  // Anzahl der sichtbaren Elemente-Zeilen (ohne unteres Panel)
    uint32_t        uVisibleCenterX;                            // Zentrum X des sichtbaren Bereichs (ohne unteres Panel)
    uint32_t        uVisibleCenterY;                            // Zentrum X des sichtbaren Bereichs (ohne unteres Panel)
    uint32_t        uShiftLevelXpix;                            // Anzahl Pixel, die das Level nach rechts geshiftet wird, da es kleiner bzw. gleich der Anzeigebreite ist
    uint32_t        uShiftLevelYpix;                            // Anzahl Pixel, die das Level nach unten geshiftet wird, da es kleiner bzw. gleich der Anzeigehöhe ist
    int             nMaxXpos;                                   // Für maximale X-Positionierung
    int             nMaxYpos;                                   // Für maximale Y-Positionierung
    uint32_t        uManXpos;                                   // X-Element-Position des Man
    uint32_t        uManYpos;                                   // Y-Element-Position des Man
    uint32_t        uFrameCounter;                              // Bildzähler
    uint32_t        uShowMessageNo;                             // Nachricht Nr. x zeigen, 0 = keine Nachricht
    uint32_t        uMaxYamExplosionIndex;                      // Max. Yam-Explosions-Index in diesem Level
    uint32_t        uYamExplosion;                              // Aktuelle YAM-Explosion
    uint32_t        uDynamitePos;                               // lineare Koordinate des manuell gezündeten Dynamits durch den Man, 0xFFFFFFFF = keine Zündung
    uint32_t        uDynamiteStatusAnim;                        // Status/Animation für manuell gezündetes Dynamit
    uint32_t        uPlayTimeStart;                             // Zeitpunkt, wann Level gestartet wurde
    uint32_t        uPlayTimeEnd;                               // Zeitpunkt, wann Level beendet wurde
    YAMEXPLOSION    YamExplosions[EMERALD_MAX_YAM_EXPLOSIONS];
    uint32_t        uTeleporterRedCounter;                      // Anzahl roter Teleporter
    uint32_t        *puTeleporterRedCoordinates;                // lineare Koordinaten, rote Teleporter
    uint32_t        uTeleporterYellowCounter;                   // Anzahl gelber Teleporter
    uint32_t        *puTeleporterYellowCoordinates;             // lineare Koordinaten, gelbe Teleporter
    uint32_t        uTeleporterGreenCounter;                    // Anzahl grüner Teleporter
    uint32_t        *puTeleporterGreenCoordinates;              // lineare Koordinaten, grüne Teleporter
    uint32_t        uTeleporterBlueCounter;                     // Anzahl blauer Teleporter
    uint32_t        *puTeleporterBlueCoordinates;               // lineare Koordinaten, blaue Teleporter
} PLAYFIELD;


uint32_t ControlGame(uint32_t uDirection);
uint32_t GetTextureIndexByElementForAcidPool(uint16_t uElement,int nAnimationCount, float *pfAngle);
void ScrollAndCenterLevel(uint32_t uManDirection);
int Menu(SDL_Renderer *pRenderer);
int RunGame(SDL_Renderer *pRenderer, uint32_t uLevel);
void InitRollUnderground(void);
uint8_t GetFreeRollDirections(uint32_t I);
uint32_t GetTextureIndexByShrink(uint32_t uShrinkAnimation);
void ControlPreElements(void);
void ControlTimeDoor(uint32_t I);
void PostControlSwitchDoors(void);
void PostControlBombExplosions(void);
bool IsSteel(uint16_t uElement);
int CheckGameDirectorys(void);
#endif // TESTSURFACE_H_INCLUDED
