#ifndef TESTSURFACE_H_INCLUDED
#define TESTSURFACE_H_INCLUDED

#include <SDL2/SDL.h>
#include <stdbool.h>

#define MIN_LEVEL_W                             4       // Minumum 4 Elemente in der Breite
#define MIN_LEVEL_H                             3       // Minumum 3 Elemente in der H?he
#define MAX_LEVEL_W                             1000    // Maximum Elemente in der Breite
#define MAX_LEVEL_H                             1000    // Maximum Elemente in der H?he

#define PANEL_H                                 32      // Spiel-Anzeige (Time, Score usw.)

#define EMERALD_VERSION_LEN                     5       // Maximale Versionsl?nge, z.B "01.00"
#define EMERALD_TITLE_LEN                       32      // Maximale Titell?nge
#define EMERALD_AUTHOR_LEN                      32      // Maximale Autorl?nge
#define EMERALD_MD5_STRING_LEN                  32      // Maximale Autorl?nge

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
#define EMERALD_LIGHTBARRIER_GREEN_SWITCH       0x000D            // Lichtschranke, gr?n, Schalter
#define EMERALD_LIGHTBARRIER_BLUE_SWITCH        0x000E            // Lichtschranke, blue, Schalter
#define EMERALD_LIGHTBARRIER_YELLOW_SWITCH      0x000F            // Lichtschranke, gelb, Schalter
#define EMERALD_BOMB                            0x0010            // Bombe
#define EMERALD_CRYSTAL                         0x0011            // Kristall
#define EMERALD_EMERALD                         0x0012            // Emerald
#define EMERALD_RUBY                            0x0013            // Rubin
#define EMERALD_BEETLE_UP                       0x0014            // K?fer hoch
#define EMERALD_BEETLE_RIGHT                    0x0015            // K?fer rechts
#define EMERALD_BEETLE_DOWN                     0x0016            // K?fer runter
#define EMERALD_BEETLE_LEFT                     0x0017            // K?fer links
#define EMERALD_REPLICATOR_RED_TOP_LEFT         0x0018        	  // Replikator rot, oben links
#define EMERALD_REPLICATOR_RED_TOP_MID          0x0019        	  // Replikator rot, oben mitte
#define EMERALD_REPLICATOR_RED_TOP_RIGHT        0x001A        	  // Replikator rot, oben rechts
#define EMERALD_REPLICATOR_RED_BOTTOM_LEFT      0x001B        	  // Replikator rot, unten links
#define EMERALD_REPLICATOR_RED_BOTTOM_RIGHT     0x001C        	  // Replikator rot, unten rechts
#define EMERALD_REPLICATOR_RED_SWITCH           0x001D        	  // Schalter f?r roten Replikator
#define EMERALD_REPLICATOR_YELLOW_TOP_LEFT      0x001E        	  // Replikator gelb, oben links
#define EMERALD_REPLICATOR_YELLOW_TOP_MID       0x001F        	  // Replikator gelb, oben mitte
#define EMERALD_REPLICATOR_YELLOW_TOP_RIGHT     0x0020        	  // Replikator gelb, oben rechts
#define EMERALD_REPLICATOR_YELLOW_BOTTOM_LEFT   0x0021        	  // Replikator gelb, unten links
#define EMERALD_REPLICATOR_YELLOW_BOTTOM_RIGHT  0x0022        	  // Replikator gelb, unten rechts
#define EMERALD_REPLICATOR_YELLOW_SWITCH        0x0023        	  // Schalter f?r gelben Replikator
#define EMERALD_TIME_COIN                       0x0024            // Zeitm?nze
#define EMERALD_NUT                             0x0025            // Nuss
#define EMERALD_ALIEN                           0x0026            // Alien
#define EMERALD_YAM                             0x0027            // Yam
#define EMERALD_STANDMINE                       0x0028            // Stand-Mine
#define EMERALD_MAGIC_WALL_SWITCH               0x0029            // Magische Mauer, Schalter
#define EMERALD_MAGIC_WALL_STEEL                0x002A            // magische Mauer aus Stahl
#define EMERALD_SAND_MOLE                       0x002B            // Sand, der durch Maufwurf entsteht
#define EMERALD_GREEN_CHEESE_GOES               0x002C            // Gr?ner K?se l?st sich auf
#define EMERALD_LIGHT_SWITCH                    0x002D            // Lichtschalter
#define EMERALD_REPLICATOR_GREEN_TOP_LEFT       0x002E        	  // Replikator gr?n, oben links
#define EMERALD_REPLICATOR_GREEN_TOP_MID        0x002F        	  // Replikator gr?n, oben mitte
#define EMERALD_REPLICATOR_GREEN_TOP_RIGHT      0x0030        	  // Replikator gr?n, oben rechts
#define EMERALD_REPLICATOR_GREEN_BOTTOM_LEFT    0x0031        	  // Replikator gr?n, unten links
#define EMERALD_REPLICATOR_GREEN_BOTTOM_RIGHT   0x0032        	  // Replikator gr?n, unten rechts
#define EMERALD_REPLICATOR_GREEN_SWITCH         0x0033        	  // Schalter f?r gr?nen Replikator
#define EMERALD_REPLICATOR_BLUE_TOP_LEFT        0x0034        	  // Replikator blau, oben links
#define EMERALD_REPLICATOR_BLUE_TOP_MID         0x0035        	  // Replikator blau, oben mitte
#define EMERALD_REPLICATOR_BLUE_TOP_RIGHT       0x0036        	  // Replikator blau, oben rechts
#define EMERALD_REPLICATOR_BLUE_BOTTOM_LEFT     0x0037        	  // Replikator blau, unten links
#define EMERALD_REPLICATOR_BLUE_BOTTOM_RIGHT    0x0038        	  // Replikator blau, unten rechts
#define EMERALD_REPLICATOR_BLUE_SWITCH          0x0039        	  // Schalter f?r blauen Replikator
#define EMERALD_DYNAMITE_OFF                    0x003A            // Dynamit aus
#define EMERALD_DYNAMITE_ON                     0x003B            // Dynamit an
#define EMERALD_DOOR_RED_WOOD                   0x003C            // rote Holz-T?r
#define EMERALD_DOOR_YELLOW_WOOD                0x003D            // gelbe Holz-T?r
#define EMERALD_DOOR_BLUE_WOOD                  0x003E            // blaue Holz-T?r
#define EMERALD_DOOR_GREEN_WOOD                 0x003F            // gr?ne Holz-T?r
#define EMERALD_MAN                             0x0040            // Man
#define EMERALD_MESSAGE_1                       0x0041            // Briefumschlag 1, Die Message-Elemente
#define EMERALD_MESSAGE_2                       0x0042            // Briefumschlag 2, m?ssen aufsteigend und
#define EMERALD_MESSAGE_3                       0x0043            // Briefumschlag 3, ohne L?cken aufgez?hlt werden,
#define EMERALD_MESSAGE_4                       0x0044            // Briefumschlag 4, da im Quelltext dieser Bereich
#define EMERALD_MESSAGE_5                       0x0045            // Briefumschlag 5, gepr?ft wird.
#define EMERALD_MESSAGE_6                       0x0046            // Briefumschlag 6
#define EMERALD_MESSAGE_7                       0x0047            // Briefumschlag 7
#define EMERALD_MESSAGE_8                       0x0048            // Briefumschlag 8
#define EMERALD_LIGHTBARRIER_RED_UP             0x0049            // Lichtschranke aus, rot, oben
#define EMERALD_LIGHTBARRIER_RED_DOWN           0x004A            // Lichtschranke aus, rot, unten
#define EMERALD_LIGHTBARRIER_RED_LEFT           0x004B            // Lichtschranke aus, rot, links
#define EMERALD_LIGHTBARRIER_RED_RIGHT          0x004C            // Lichtschranke aus, rot, rechts
#define EMERALD_LIGHTBARRIER_GREEN_UP           0x004D            // Lichtschranke aus, gr?n, oben
#define EMERALD_LIGHTBARRIER_GREEN_DOWN         0x004E            // Lichtschranke aus, gr?n, unten
#define EMERALD_LIGHTBARRIER_GREEN_LEFT         0x004F            // Lichtschranke aus, gr?n, links
#define EMERALD_LIGHTBARRIER_GREEN_RIGHT        0x0050            // Lichtschranke aus, gr?n, rechts
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
#define EMERALD_BEAM_GREEN_VERTICAL             0x005B            // Lichtstrahl, gr?n, vertikal
#define EMERALD_BEAM_GREEN_HORIZONTAL           0x005C            // Lichtstrahl, gr?n, horizontal
#define EMERALD_BEAM_BLUE_VERTICAL              0x005D            // Lichtstrahl, blau, vertikal
#define EMERALD_BEAM_BLUE_HORIZONTAL            0x005E            // Lichtstrahl, blau, horizontal
#define EMERALD_BEAM_YELLOW_VERTICAL            0x005F            // Lichtstrahl, gelb, vertikal
#define EMERALD_BEAM_YELLOW_HORIZONTAL          0x0060            // Lichtstrahl, gelb, horizontal
#define EMERALD_BEAM_CROSS                      0x0061            // Lichtstrahl, Kreuz
#define EMERALD_DOOR_EMERALD                    0x0062            // Einen Emerald mehr zu sammeln beim Durchgang durch diese T?r
#define EMERALD_DOOR_MULTICOLOR                 0x0063            // Bunte T?r, ein Schl?ssel reicht
#define EMERALD_DOOR_ONLY_UP                    0x0064            // T?r, nur nach oben passierbar
#define EMERALD_DOOR_ONLY_DOWN                  0x0065            // T?r, nur nach unten passierbar
#define EMERALD_DOOR_ONLY_LEFT                  0x0066            // T?r, nur nach links passierbar
#define EMERALD_DOOR_ONLY_RIGHT                 0x0067            // T?r, nur nach rechts passierbar
#define EMERALD_WHEEL_TIMEDOOR                  0x0068            // Rad f?r Zeit-T?r
#define EMERALD_DOOR_TIME                       0x0069            // Zeit-T?r
#define EMERALD_SWITCHDOOR_OPEN                 0x006A            // Schaltt?r auf
#define EMERALD_SWITCHDOOR_CLOSED               0x006B            // Schaltt?r geschlossen
#define EMERALD_SWITCH_SWITCHDOOR               0x006C            // Schalter f?r Schaltt?r
#define EMERALD_DOOR_END_NOT_READY_STEEL        0x006D            // Endt?r aus Stahl, nicht blinkend
#define EMERALD_DOOR_END_READY_STEEL            0x006E            // Endt?r aus Stahl, blinkend
#define EMERALD_KEY_WHITE                       0x006F            // wei?er Schl?ssel
#define EMERALD_DOOR_WHITE                      0x0070            // wei?e T?r
#define EMERALD_DOOR_GREY_WHITE                 0x0071            // wei?e Geheimt?r
#define EMERALD_DOOR_WHITE_WOOD                 0x0072            // wei?e Holzt?r
#define EMERALD_KEY_GENERAL                     0x0073            // General-Schl?ssel
#define EMERALD_MEGABOMB                        0x0074            // Mega-Bombe
#define EMERALD_WALL_WITH_EMERALD               0x0075            // Mauer mit Emerald
#define EMERALD_WALL_WITH_RUBY                  0x0076            // Mauer mit Rubin
#define EMERALD_WALL_WITH_SAPPHIRE              0x0077            // Mauer mit Saphir
#define EMERALD_WALL_WITH_PERL                  0x0078            // Mauer mit Perle
#define EMERALD_ACIDPOOL_DESTROY                0x0079            // Objekt l?uft ins S?urebecken -> Objekt l?st sich selbst auf
#define EMERALD_ACIDPOOL_TOP_LEFT               0x007A            // S?urebecken, oben links
#define EMERALD_ACIDPOOL_TOP_MID                0x007B            // S?urebecken, oben mitte (aktives Feld)
#define EMERALD_ACIDPOOL                        0x007B            // S?urebecken, oben mitte (aktives Feld)
#define EMERALD_ACIDPOOL_TOP_RIGHT              0x007C            // S?urebecken, oben rechts
#define EMERALD_ACIDPOOL_BOTTOM_LEFT            0x007D            // S?urebecken, unten links
#define EMERALD_ACIDPOOL_BOTTOM_MID             0x007E            // S?urebecken, unten mitte
#define EMERALD_ACIDPOOL_BOTTOM_RIGHT           0x007F            // S?urebecken, unten rechts
#define EMERALD_SPACE                           0x0080            // Space
#define EMERALD_WALL_ROUND                      0x0081            // Mauer, rund
#define EMERALD_SAND                            0x0082            // Sand
#define EMERALD_STEEL                           0x0083            // Stahl
#define EMERALD_WALL_CORNERED                   0x0084            // Mauer, eckig
#define EMERALD_KEY_RED                         0x0085            // roter Schl?ssel
#define EMERALD_KEY_YELLOW                      0x0086            // gelber Schl?ssel
#define EMERALD_KEY_BLUE                        0x0087            // blauer Schl?ssel
#define EMERALD_KEY_GREEN                       0x0088            // gr?ner Schl?ssel
#define EMERALD_DOOR_RED                        0x0089            // rote T?r
#define EMERALD_DOOR_YELLOW                     0x008A            // gelbe T?r
#define EMERALD_DOOR_BLUE                       0x008B            // blaue T?r
#define EMERALD_DOOR_GREEN                      0x008C            // gr?ne T?r
#define EMERALD_HAMMER                          0x008D            // Hammer
#define EMERALD_DOOR_GREY_RED                   0x008E            // rote Geheimt?r
#define EMERALD_DOOR_GREY_YELLOW                0x008F            // gelbe Geheimt?r
#define EMERALD_DOOR_GREY_BLUE                  0x0090            // blaue Geheimt?r
#define EMERALD_DOOR_GREY_GREEN                 0x0091            // gr?ne Geheimt?r
#define EMERALD_MAGIC_WALL                      0x0092            // magische Mauer
#define EMERALD_WHEEL                           0x0093            // Rad
#define EMERALD_SWAMP                           0x0094            // Sumpf ohne Stein
#define EMERALD_SWAMP_STONE                     0x0095            // Sumpf mit Stein
#define EMERALD_STONE_SINK                      0x0096            // Stein versinkt in Sumpf
#define EMERALD_STONE_SAG                       0x0097            // Stein sackt aus Sumpf
#define EMERALD_DOOR_END_NOT_READY              0x0098            // Endt?r, nicht blinkend
#define EMERALD_GREEN_DROP_COMES                0x0099            // gr?ner Tropfen, der zu K?se werden kann
#define EMERALD_GREEN_DROP                      0x009A            // gr?ner Tropfen, der zu K?se werden kann
#define EMERALD_GREEN_CHEESE                    0x009B            // gr?ner K?se, der sich ?ber Tropfen ausbreiten kann
#define EMERALD_EXPLOSION_PHASE_1               0x009C            // Explosion, Phase 1
#define EMERALD_EXPLOSION_PHASE_2               0x009D            // Explosion, Phase 2
#define EMERALD_WALL_ROUND_PIKE                 0x009E            // Mauer, rund, mit Spitze
#define EMERALD_DOOR_END_READY                  0x009F            // Endt?r, blinkend
#define EMERALD_STEEL_ROUND_PIKE                0x00A0	          // Stahl, rund, mit Spitze
#define EMERALD_STEEL_ROUND                     0x00A1	          // Stahl, rund
#define EMERALD_STEEL_WARNING                   0x00A2	          // Stahl, Achtung
#define EMERALD_STEEL_BIOHAZARD                 0x00A3	          // Stahl, Biogefahr
#define EMERALD_STEEL_DEADEND                   0x00A4	          // Stahl, Sackgasse
#define EMERALD_STEEL_STOP                      0x00A5	          // Stahl, Stopp
#define EMERALD_STEEL_PARKING                   0x00A6	          // Stahl, Parkplatz
#define EMERALD_STEEL_FORBIDDEN                 0x00A7	          // Stahl, Einbahn
#define EMERALD_STEEL_EXIT                      0x00A8	          // Stahl, Ausgang
#define EMERALD_STEEL_RADIOACTIVE               0x00A9	          // Stahl, Radioaktivit?t
#define EMERALD_STEEL_EXPLOSION                 0x00AA	          // Stahl, Explosion
#define EMERALD_STEEL_ACID                      0x00AB	          // Stahl, S?ure
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
#define EMERALD_STEEL_GIVE_WAY                  0x00B9            // Stahl, Vorfahrt gew?hren
#define EMERALD_STEEL_YING                      0x00BA            // Stahl, Ying
#define EMERALD_STEEL_WHEELCHAIR                0x00BB            // Stahl, Rollstuhl
#define EMERALD_STEEL_ARROW_DOWN                0x00BC            // Stahl, Pfeil runter
#define EMERALD_STEEL_ARROW_UP                  0x00BD            // Stahl, Pfeil rauf
#define EMERALD_STEEL_ARROW_LEFT                0x00BE            // Stahl, Pfeil links
#define EMERALD_STEEL_ARROW_RIGHT               0x00BF            // Stahl, Pfeil rechts
#define EMERALD_STEEL_INVISIBLE                 0x00C0            // Stahl, unsichtbar / beleuchtet
#define EMERALD_WALL_INVISIBLE                  0x00C1            // Mauer, unsichtbar / beleuchtet
#define EMERALD_WALL_WITH_CRYSTAL               0x00C2            // Mauer mit Kristall
#define EMERALD_WALL_WITH_KEY_RED               0x00C3            // Mauer mit rotem Schl?ssel
#define EMERALD_WALL_WITH_KEY_GREEN             0x00C4            // Mauer mit gr?nem Schl?ssel
#define EMERALD_WALL_WITH_KEY_BLUE              0x00C5            // Mauer mit blauem Schl?ssel
#define EMERALD_WALL_WITH_KEY_YELLOW            0x00C6            // Mauer mit gelbem Schl?ssel
#define EMERALD_WALL_WITH_KEY_WHITE             0x00C7            // Mauer mit wei?em Schl?ssel
#define EMERALD_WALL_WITH_KEY_GENERAL           0x00C8            // Mauer mit Generalschl?ssel
#define EMERALD_WALL_WITH_BOMB                  0x00C9            // Mauer mit Bombe
#define EMERALD_WALL_WITH_MEGABOMB              0x00CA            // Mauer mit Mega-Bombe
#define EMERALD_WALL_WITH_STONE                 0x00CB            // Mauer mit Stein
#define EMERALD_WALL_WITH_NUT                   0x00CC            // Mauer mit Nuss
#define EMERALD_WALL_WITH_WHEEL                 0x00CD            // Mauer mit Wagenrad
#define EMERALD_WALL_WITH_DYNAMITE              0x00CE            // Mauer mit Dynamit
#define EMERALD_WALL_WITH_ENDDOOR               0x00CF            // Mauer mit Endt?r
#define EMERALD_WALL_WITH_ENDDOOR_READY         0x00D0            // Mauer mit Endt?r, blinkend
#define EMERALD_WALL_WITH_MINE_UP               0x00D1            // Mauer mit Mine hoch
#define EMERALD_WALL_WITH_MOLE                  0x00D2            // Mauer mit Maulwurf
#define EMERALD_WALL_WITH_GREEN_CHEESE          0x00D3            // Mauer mit gr?nem K?se
#define EMERALD_WALL_WITH_BEETLE_UP             0x00D4            // Mauer mit K?fer hoch
#define EMERALD_WALL_WITH_YAM                   0x00D5            // Mauer mit Yam
#define EMERALD_WALL_WITH_ALIEN                 0x00D6            // Mauer mit Alien
#define EMERALD_FONT_START                      0x0100            // Start des Zeichensatzes
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
#define EMERALD_FONT_AE                         0x0136            // ?
#define EMERALD_FONT_OE                         0x0137            // ?
#define EMERALD_FONT_UE                         0x0138            // ?
#define EMERALD_FONT_END                        0x0138            // Ende des Zeichensatzes


#define EMERALD_INVALID                         0xFFFF            // ung?ltiges Element


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
#define EMERALD_ANIM_MOLE_STAND                 0x00001100      // Mole "l?uft" auf der Stelle
#define EMERALD_STATUS_DOOR_OPEN                0x00001200      // Schalt- und Zeit-T?r ist offen
#define EMERALD_STATUS_DOOR_CLOSE               0x00001300      // Schalt- und Zeit-T?r ist geschlossen


#define EMERALD_ANIM_CLEAN_UP                   0x00010000      // ?ber dem invaliden Feld l?schen
#define EMERALD_ANIM_CLEAN_RIGHT                0x00020000      // rechts neben invaliden Feld l?schen
#define EMERALD_ANIM_CLEAN_DOWN                 0x00030000      // unter dem invaliden Feld l?schen
#define EMERALD_ANIM_CLEAN_LEFT                 0x00040000      // links neben invaliden Feld l?schen
#define EMERALD_ANIM_CLEAN_UP_DOUBLESPEED       0x00050000      // 2 Felder ?ber dem invaliden Feld l?schen
#define EMERALD_ANIM_CLEAN_RIGHT_DOUBLESPEED    0x00060000      // 2 Felder rechts neben invaliden Feld l?schen
#define EMERALD_ANIM_CLEAN_DOWN_DOUBLESPEED     0x00070000      // 2 Felder unter dem invaliden Feld l?schen
#define EMERALD_ANIM_CLEAN_LEFT_DOUBLESPEED     0x00080000      // 2 Felder links neben invaliden Feld l?schen
#define EMERALD_ANIM_CLEAN_NOTHING              0x00090000      // Gar nichts l?schen/setzen, wird z.B. f?r gr?nen Tropfen in Phase 1 ben?tigt

#define EMERALD_ANIM_AVOID_DOUBLE_CONTROL       0x00800000      // Element nicht doppelt steuern (Element wird z.B. I + X gesetzt und durch ControlGame(I) nachgesteuet)




// "Selbststeuernde" Zust?nde                                   // Diese Zust?nde m?ssen vom Element selbst zur?ckgesetzt werden
#define EMERALD_NO_ADDITIONAL_ANIMSTATUS        0x00000000      // kein zus?tzlicher Status
#define EMERALD_ANIM_LOST_GUIDE                 0x01000000      // Mine oder K?fer haben F?hrung verloren
#define EMERALD_ANIM_ALIEN_MOVED                0x02000000      // Alien hat sich im letzten Schritt bewegt -> soll keine Doppelbewegungen machen
#define EMERALD_ANIM_WHEEL_RUN                  0x03000000      // Rad soll bewegt werden
#define EMERALD_ANIM_BORN1                      0x04000000      // Replikator erzeugt neues Element ?ber
#define EMERALD_ANIM_BORN2                      0x05000000      // 2 Steuerungsphasen -> halbe Geschwindigekit
#define EMERALD_ANIM_KEY_RED_SHRINK             0x06000000      // roter Schl?ssel wird keiner
#define EMERALD_ANIM_KEY_GREEN_SHRINK           0x07000000      // gr?ner Schl?ssel wird keiner
#define EMERALD_ANIM_KEY_BLUE_SHRINK            0x08000000      // blauer Schl?ssel wird keiner
#define EMERALD_ANIM_KEY_YELLOW_SHRINK          0x09000000      // gelber Schl?ssel wird keiner
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
#define EMERALD_ANIM_TIME_COIN_SHRINK           0x22000000      // Zeitm?nze wird kleiner
#define EMERALD_ANIM_HAMMER_SHRINK              0x23000000      // Hammer wird kleiner
#define EMERALD_ANIM_DYNAMITE_SHRINK            0x24000000      // Dynamit wird kleiner
#define EMERALD_ANIM_MESSAGE_SHRINK             0x25000000      // Briefumschlag wird kleiner
#define EMERALD_ANIM_GREEN_DROP_1               0x26000000      // gr?ner Tropfen, Phase 1
#define EMERALD_ANIM_GREEN_DROP_2               0x27000000      // gr?ner Tropfen, Phase 2
#define EMERALD_ANIM_PERL_BREAK                 0x28000000      // Perle zerbricht
#define EMERALD_ANIM_BLOCKED_MOLE_SPIN_RIGHT    0x29000000      // Blockierter Maulwurf drehte zuletzt nach rechts
#define EMERALD_ANIM_BLOCKED_MOLE_SPIN_LEFT     0x2A000000      // Blockierter Maulwurf drehte zuletzt nach links
#define EMERALD_ANIM_DOOR_OPEN                  0x2B000000      // Schalt- und Zeit-T?r wird ge?ffnet
#define EMERALD_ANIM_DOOR_CLOSE                 0x2C000000      // Schalt- und Zeit-T?r wird geschlossen
#define EMERALD_ANIM_KEY_WHITE_SHRINK           0x2D000000      // wei?er Schl?ssel wird keiner
#define EMERALD_ANIM_KEY_GENERAL_SHRINK         0x2E000000      // General-Schl?ssel wird keiner



#define EMERALD_STANDARD_SPEED                  false
#define EMERALD_DOUBLE_SPEED                    true

#define EMERALD_MAX_HEADER_SIZE                 0x500           // memmem begrenzen
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


typedef struct {
    uint32_t        uTextureIndex;                              // Start-Texture zur Animation
    bool            bFlipVertical;                              // Texture vertikal spiegeln
    bool            bFlipHorizontal;                            // Texture horizontal spiegeln
    uint32_t        uX;                                         // X-Koordinate, wo Post-Animation durchgef?hrt werden soll.
    uint32_t        uY;                                         // Y-Koordinate, wo Post-Animation durchgef?hrt werden soll.
    uint32_t        uFormulaIndex;                              // Index auf Formel f?r Animation
} POSTANIMATION;


typedef struct {
    uint16_t uElement[9];                                       // 1 YAM erzeugt maximal 9 neue Elemente/Objekte
} YAMEXPLOSION;


typedef struct {
    uint16_t        *pLevel;
    uint32_t        *pStatusAnimation;
    POSTANIMATION   *pPostAnimation;
    uint32_t        uRollUnderground[65536];                    // Bit 0 = Emerald, Bit 1 = Saphir, Bit 2 = Stone, Bit 3 = Nut, Bit 4 = Bomb, Bit 5 = Rubin, Bit 6 = Kristall, Bit 7 = Perle, Bit 8 = Megabombe
    uint16_t        *pInvalidElement;
    bool            bInitOK;
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
    bool            bLightBarrierRedOn;
    bool            bLightBarrierGreenOn;
    bool            bLightBarrierBlueOn;
    bool            bLightBarrierYellowOn;
    bool            bTimeDoorOpen;                              // F?r Zeit-T?r
    bool            bSwitchDoorState;                           // F?r Schaltert?r, true = umgeschaltet
    bool            bSwitchDoorImpluse;                         // F?r Schaltert?r
    bool            bReplicatorRedOn;
    uint16_t        uReplicatorRedObject;
    bool            bReplicatorGreenOn;
    uint16_t        uReplicatorGreenObject;
    bool            bReplicatorBlueOn;
    uint16_t        uReplicatorBlueObject;
    bool            bReplicatorYellowOn;
    uint16_t        uReplicatorYellowObject;
    char            szVersion[EMERALD_VERSION_LEN + 1];         // z.B. "01.00"
    char            szLevelTitle[EMERALD_TITLE_LEN + 1];        // z.B. "Der Bunker"
    char            szLevelAuthor[EMERALD_AUTHOR_LEN + 1];      // z.B. "Mikiman"
    char            szMd5String[EMERALD_MD5_STRING_LEN + 1];    // MD5-Pr?fsumme als String f?r komprimierte Leveldaten, d.h. ohne Header
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
    uint32_t        uTimeToPlay;
    uint32_t        uAdditonalTimeCoinTime;                     // zus?tzliche Zeit durch Zeitm?nze
    uint32_t        uTimeWheelRotation;
    uint32_t        uTimeWheelRotationLeft;                     // Verbleibende Zeit f?r Wheel-Rotation
    uint32_t        uTimeDoorTime;
    uint32_t        uTimeDoorTimeLeft;
    uint32_t        uWheelRunningXpos;                          // nur g?ltig, wenn bWheelRunning = true
    uint32_t        uWheelRunningYpos;                          // nur g?ltig, wenn bWheelRunning = true
    uint32_t        uTimeMagicWall;
    uint32_t        uTimeMagicWallLeft;                         // Verbleibende Zeit f?r Magic-Wall
    uint32_t        uTimeLight;
    uint32_t        uTimeLightLeft;                             // Verbleibende Zeit f?r Licht
    uint32_t        uDynamiteCount;                             // Anzahl Dynamits, die der Man hat
    uint32_t        uHammerCount;                               // Anzahl Hammer, die der Man hat
    uint32_t        uWhiteKeyCount;                             // Anzahl der wei?en Schl?ssel, die der Man hat
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
    uint32_t        uShiftLevelYpix;                            // Anzahl Pixel, die das Level nach unten geshiftet wird, da es kleiner bzw. gleich der Anzeigeh?he ist
    int             nMaxXpos;                                   // F?r maximale X-Positionierung
    int             nMaxYpos;                                   // F?r maximale Y-Positionierung
    uint32_t        uManXpos;                                   // X-Element-Position des Man
    uint32_t        uManYpos;                                   // Y-Element-Position des Man
    uint32_t        uFrameCounter;                              // Bildz?hler
    uint32_t        uShowMessageNo;                             // Nachricht Nr. x zeigen, 0 = keine Nachricht
    uint32_t        uMaxYamExplosionIndex;                      // Max. Yam-Explosions-Index in diesem Level
    uint32_t        uYamExplosion;                              // Aktuelle YAM-Explosion
    YAMEXPLOSION    YamExplosions[EMERALD_MAX_YAM_EXPLOSIONS];
} PLAYFIELD;


uint32_t ControlGame(uint32_t uDirection);
int RenderLevel(SDL_Renderer *pRenderer, int *pnXpos, int *pnYpos, int nAnimationCount);
uint32_t GetTextureIndexByElement(uint16_t uElement,int nAnimationCount,float *pfAngle);
uint32_t GetTextureIndexByElementForAcidPool(uint16_t uElement,int nAnimationCount, float *pfAngle);
void ScrollAndCenterLevel(uint32_t uManDirection);
int RunGame(SDL_Window *pWindow, SDL_Renderer *pRenderer);
void InitRollUnderground(void);
uint8_t GetFreeRollDirections(uint32_t I);
uint32_t GetTextureIndexByShrink(uint32_t uShrinkAnimation);
void ControlPreElements(void);
void ControlTimeDoor(uint32_t I);
void PostControlSwitchDoors(void);
bool IsSteel(uint16_t uElement);
#endif // TESTSURFACE_H_INCLUDED
