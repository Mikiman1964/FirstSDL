#include <SDL2/SDL.h>
#include <stdio.h>
#include "EmeraldMine.h"
#include "ezxml.h"
#include "loadlevel.h"
#include "md5.h"
#include "mySDL.h"
#include "mystd.h"
#include "zlib.h"

extern PLAYFIELD Playfield;
extern SDL_DisplayMode ge_DisplayMode;

// Dieses interne Level wird verwendet, wenn die Datei Level.xml nicht gefunden wird
uint8_t g_InternalLevel[] = {"<?xml version=\"1.0\"?>\
<level>\
  <title>DER BUNKER</title>\
  <author>MIKIMAN</author>\
  <version>01.00</version>\
  <values>\
    <emeralds_to_collect>20</emeralds_to_collect>\
    <score_time_factor>2</score_time_factor>\
    <speed_cheese_spread>0</speed_cheese_spread>\
  </values>\
  <leveldimension>\
    <x>64</x>\
    <y>32</y>\
  </leveldimension>\
  <scores>\
    <emerald>100</emerald>\
    <ruby>48</ruby>\
    <sapphire>200</sapphire>\
    <perl>5</perl>\
    <crystal>8</crystal>\
    <letter>100</letter>\
    <key>2</key>\
    <dynamite>3</dynamite>\
    <hammer>3</hammer>\
    <nutcracking>4</nutcracking>\
    <stoning_beetle>5</stoning_beetle>\
    <stoning_mine>6</stoning_mine>\
    <stoning_alien>7</stoning_alien>\
    <stoning_yam>8</stoning_yam>\
    <timecoin>3</timecoin>\
  </scores>\
  <times>\
    <to_play>500</to_play>\
    <wheel_rotation>20</wheel_rotation>\
    <magic_wall>30</magic_wall>\
    <light>10</light>\
    <timedoor>3</timedoor>\
    <timecoin>50</timecoin>\
  </times>\
  <inventory>\
    <dynamite>572</dynamite>\
    <hammer>514</hammer>\
    <white_key>4</white_key>\
  </inventory>\
  <replicators>\
    <red>\
      <active>0</active>\
      <element>18</element>\
    </red>\
    <green>\
      <active>0</active>\
      <element>2</element>\
    </green>\
    <blue>\
      <active>0</active>\
      <element>154</element>\
    </blue>\
    <yellow>\
      <active>0</active>\
      <element>10</element>\
    </yellow>\
  </replicators>\
  <lightbarriers>\
    <red>\
      <active>0</active>\
    </red>\
    <green>\
      <active>1</active>\
    </green>\
    <blue>\
      <active>1</active>\
    </blue>\
    <yellow>\
      <active>0</active>\
    </yellow>\
  </lightbarriers>\
  <messages>\
    <m1>V2lsbGtvbW1lbiBpbSBlcnN0ZW4gVGVzdGxldmVsIQpEaWUgTW9uc3RlciBzaW5kIHp1ciBaZWl0IG5vY2ggaGFybWxvcywKYWJlciBlcyBmdW5rdGlvbmllcmVuIGJlcmVpdHMgdmllbGUgRWxlbWVudGUuCgpQcmVzcyBGaXJlIG9yIFNwYWNlIHRvIGNvbmZpcm0h</m1>\
    <m2>RGllcyBpc3QgZGllIHp3ZWl0ZSBUZXh0Ym94IQoKUHJlc3MgRmlyZSBvciBTcGFjZSB0byBjb25maXJtIQ==</m2>\
    <m3></m3>\
    <m4></m4>\
    <m5></m5>\
    <m6></m6>\
    <m7></m7>\
    <m8></m8>\
  </messages>\
  <max_yam_explosions>2</max_yam_explosions>\
  <yam_explosions>\
    <explosion01>\
      <element01>18</element01>\
      <element02>18</element02>\
      <element03>18</element03>\
      <element04>19</element04>\
      <element05>19</element05>\
      <element06>19</element06>\
      <element07>2</element07>\
      <element08>2</element08>\
      <element09>2</element09>\
    </explosion01>\
    <explosion02>\
      <element01>133</element01>\
      <element02>136</element02>\
      <element03>135</element03>\
      <element04>133</element04>\
      <element05>136</element05>\
      <element06>135</element06>\
      <element07>133</element07>\
      <element08>136</element08>\
      <element09>135</element09>\
    </explosion02>\
  </yam_explosions>\
  <leveldata>eJzFV0dvFDEU/rwEKYS6RKGGzSYhbEiUTQgdLfWIxAUhcaeGHsqJas0kwFJygAtKJHpRgDO9/gT+EW9sz8yzx7PACT155vm5fK96PNMI/yNNU5PoF32iJJrRSe8BIVEWvaJMXLuoiA7q94qVYjVJJIaJGxTdxFVppCLa1HpNPs6ldKRVkdYiUCRN+xfyo3LOJXuGlkRtSFTJwgAlMUSWV40nVpFsHfUkBkkSWV0ieR9ZXqFewewTNrC4sR/CZIeFRlLAfYwj8nGEMyS6iBOokB5rqFWIF5j0+Nm1c5tlc5iZZeNrWsD4XwnX7OjuQ8+PvI3MR4qW/2WSB5wukC+uo0eN97C1Mxzbs6jb/uD/KHNHGX5rsk+gnv3k/zJFvqqi3k+1UFIV4LOK+znLZ/3MbU5n8n3jSuwl3Kj2In3aSTKoNHiR4+P6XzRXk8jr2vO2BjGdoXbe8IGKQoCKxx69ezYi9rw6UtzY/zF+PHcd1mMDqhjEEJZgKZahhA6UzekQUoUUHO/64rvdo0kIO+NS+7l0I7VNWEPPYSynTGtHJ/FdNK8FjxU6pzTirv2+M6fOdBpl+LMc/Y9lLOpROd+D54RZpzZBsom/jDlvo6xxHd9ZmTuJEar7SIuiiv5Ksv1JEkM9b7eT/bH9+zNRCamGXXuCZF2b6r8nzEmMKYr63TSyWXE76Aw4maw7Tu0CTuTEPaXtjn11hJaFbSz+EX1Q+AWjQRG7yN/6VJDYiTdm14fGi/tMfx5DTHNbYqviWhNP6PjOdnSyM3iMkcRaGlmBw+jDPaxWuuidexWn/TGHJHOTPdOdt2Z2t/ElbhppbL/ERwdf4q4hiSPMMoka0vqZz/g600Sa3Glsf2BmN6unRk4jdBdnsUWtOGpWfU98lf+9HTHeDwm9yLDuZPwd6/IZX/AV32h3jW9jnENcK7HmE5kI7mH5734ZXJmtfST5ZCQFxGfQMzbjABrXc83heeOnXYhbDF3rdtujUVSZp3EKrxR/MJE99VaS7wYS92uWJ3mz18T8W9NvwgM8SqSvSfISMzFlKDRvaZ4+8n2d7BM5zbL0BPDdLuvmyxgkuCHZleri94TGq1m+dcdvWPju+S+x2NxD9LdP0k19qiFlo6FvVPym4bc/vvG4pO3vpHh0qZmHPPFP4zFF1e3G3295tiLy8aV3NHQsTzm9496GnndzNv+fQ8ffXVlnueDGgO9bw4Di3udYnuLn13eTgx8w++OcDD34P00E+Dngz5C8k7RbaX8RlxRdTmo3SDJrkZNNGr/I8F8mEfRHWOOP4wfN/2HWcDqJK7iq6JqZ3WJlRb7/f6rReWZmmW7QHer23qX+odpFLNF/wP+PpvEbTBnCsg==</leveldata>\
  <leveldata_md5_hash>5AC3012E387DAC41CAEDA23B21B128B5</leveldata_md5_hash>\
</level>"};


/*----------------------------------------------------------------------------
Name:           GetLeveldimensionFromXml
------------------------------------------------------------------------------
Beschreibung: Ermittelt die X- und Y-Dimension aus der XML-Leveldatei.
Parameter
      Eingang: xml, ezxml_t, gültiges XML-Handle
      Ausgang: puX, uint32_t *, X-Level-Dimension
               puY, uint32_t *, Y-Level-Dimension
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int GetLeveldimensionFromXml(ezxml_t xml,uint32_t *puX,uint32_t *puY) {
    int nErrorCode;
    long int nX;
    long int nY;
    ezxml_t leveldimension,x,y;

    nErrorCode = -1;
    if ((xml != NULL) && (puX != NULL) && (puY != NULL)) {
        leveldimension = ezxml_child(xml,"leveldimension");
        if (leveldimension != NULL) {
            x = ezxml_child(leveldimension,"x");
            y = ezxml_child(leveldimension,"y");
            if ((x != NULL) && (y != NULL)) {
                nX = strtol(x->txt,NULL,10);
                nY = strtol(y->txt,NULL,10);
                if ((nX >= MIN_LEVEL_W) && (nX <= MAX_LEVEL_W) && (nY >= MIN_LEVEL_H) && (nY <= MAX_LEVEL_H)) {
                    *puX = (uint32_t)nX;
                    *puY = (uint32_t)nY;
                    nErrorCode = 0;
                } else {
                    SDL_Log("%s: bad level dimension, x = %d, y = %d",__FUNCTION__,(int)nX,(int)nY);
                }
            } else {
                SDL_Log("%s: error in xml file, 'x' or 'y' not found",__FUNCTION__);
            }
        } else {
            SDL_Log("%s: error in xml file, 'leveldimension' not found",__FUNCTION__);
        }
    } else {
        SDL_Log("%s: bad xml handle, null pointer",__FUNCTION__);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetMemoryForPlayfield
------------------------------------------------------------------------------
Beschreibung: Erzeugt die Speicherbereiche für das Spielfeld.
              Hinweis: Die Level-Dimension (X/Y) muss bereits ermittelt worden sein.
              Speicherbedarf S = X * Y * 2 * 2 * 4 * 8 = X * Y * 64
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int GetMemoryForPlayfield(void) {
    int nErrorCode;

    Playfield.pLevel = (uint16_t*)malloc(Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(uint16_t));
    Playfield.pInvalidElement = (uint16_t*)malloc(Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(uint16_t));
    Playfield.pStatusAnimation = (uint32_t*)malloc(Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(uint32_t));
    Playfield.pPostAnimation = (POSTANIMATION*)malloc(Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(POSTANIMATION));
    if ((Playfield.pLevel != NULL) && (Playfield.pStatusAnimation != NULL) && (Playfield.pPostAnimation != NULL) && (Playfield.pInvalidElement != NULL))  {
        memset(Playfield.pLevel,0,Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(uint16_t));
        memset(Playfield.pInvalidElement,0,Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(uint16_t));
        memset(Playfield.pStatusAnimation,0,Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(uint32_t));
        memset(Playfield.pPostAnimation,0,Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(POSTANIMATION));
        nErrorCode = 0;
    } else {
        nErrorCode = -1;
        SDL_Log("%s: can not allocate memory for playfield.",__FUNCTION__);
        SAFE_FREE(Playfield.pLevel);
        SAFE_FREE(Playfield.pInvalidElement);
        SAFE_FREE(Playfield.pStatusAnimation);
        SAFE_FREE(Playfield.pPostAnimation);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetTitleAuthorVersionHashFromXml
------------------------------------------------------------------------------
Beschreibung: Ermittelt den Level-Titel, Level-Autor, die Level-Version und den
              MD5-Hash und legt diese Informationen in der Struktur Playfield.x ab.
Parameter
      Eingang: xml, ezxml_t, gültiges XML-Handle
      Ausgang: -
               -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int GetTitleAuthorVersionHashFromXml(ezxml_t xml) {
    int nErrorCode;
    uint32_t uLen;
    ezxml_t node;
    char *pAttr;

    nErrorCode = -1;
    if (xml != NULL) {
        node = ezxml_child(xml,"title");
        if (node != NULL) {
            pAttr = node->txt;
            uLen = (uint32_t)strlen(pAttr);
            if (uLen == 0) {
                strcpy(Playfield.szLevelTitle,"unknown title");
            } else if (uLen > EMERALD_TITLE_LEN) {
                memcpy(Playfield.szLevelTitle,pAttr,EMERALD_TITLE_LEN);
            } else {
                strcpy(Playfield.szLevelTitle,pAttr);
            }
            node = ezxml_child(xml,"author");
            if (node != NULL) {
                pAttr = node->txt;
                uLen = (uint32_t)strlen(pAttr);
                if (uLen == 0) {
                    strcpy(Playfield.szLevelTitle,"unknown author");
                } else if (uLen > EMERALD_AUTHOR_LEN) {
                    memcpy(Playfield.szLevelAuthor,pAttr,EMERALD_AUTHOR_LEN);
                } else {
                    strcpy(Playfield.szLevelAuthor,pAttr);
                }
                node = ezxml_child(xml,"version");
                if (node != NULL) {
                    pAttr = node->txt;
                    uLen = (uint32_t)strlen(pAttr);
                    if ((uLen == 0) || (uLen > EMERALD_VERSION_LEN)) {
                        strcpy(Playfield.szVersion,"00.00");
                    } else {
                        strcpy(Playfield.szVersion,pAttr);
                    }
                    node = ezxml_child(xml,"leveldata_md5_hash");
                    if (node != NULL) {
                        pAttr = node->txt;
                        uLen = (uint32_t)strlen(pAttr);
                        if (uLen == 32) {   // MD5-Hash muss immer 32 Zeichen lang sein
                            strcpy(Playfield.szMd5String,pAttr);
                            nErrorCode = 0;
                        } else {
                            SDL_Log("%s: error in xml file, invalid md5 length: %d",__FUNCTION__,uLen);
                        }
                    } else {
                        SDL_Log("%s: error in xml file, 'leveldata_md5_hash' not found",__FUNCTION__);
                    }
                } else {
                    SDL_Log("%s: error in xml file, 'version' not found",__FUNCTION__);
                }
            } else {
                SDL_Log("%s: error in xml file, 'author' not found",__FUNCTION__);
            }
        } else {
            SDL_Log("%s: error in xml file, 'title' not found",__FUNCTION__);
        }
    } else {
        SDL_Log("%s: bad xml handle, null pointer",__FUNCTION__);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetLevelScoresFromXml
------------------------------------------------------------------------------
Beschreibung: Ermittelt alle Level-Scores und trägt diese in die Struktur Playfield.x ein.
Parameter
      Eingang: xml, ezxml_t, gültiges XML-Handle
      Ausgang: -
               -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int GetLevelScoresFromXml(ezxml_t xml) {
    int nErrorCode;
    ezxml_t scores,node;
    long int nNum;
    char *pAttr;

    nErrorCode = -1;
    if (xml != NULL) {
        scores = ezxml_child(xml,"scores");
        if (scores != NULL) {
            node = ezxml_child(scores,"emerald");
            if (node != NULL) {
                pAttr = node->txt;
                nNum = strtol(pAttr,NULL,10);
                Playfield.uScoreEmerald = (uint32_t)nNum;
                node = ezxml_child(scores,"ruby");
                if (node != NULL) {
                    pAttr = node->txt;
                    nNum = strtol(pAttr,NULL,10);
                    Playfield.uScoreRuby = (uint32_t)nNum;
                    node = ezxml_child(scores,"sapphire");
                    if (node != NULL) {
                        pAttr = node->txt;
                        nNum = strtol(pAttr,NULL,10);
                        Playfield.uScoreSaphir = (uint32_t)nNum;
                        node = ezxml_child(scores,"perl");
                        if (node != NULL) {
                            pAttr = node->txt;
                            nNum = strtol(pAttr,NULL,10);
                            Playfield.uScorePerl = (uint32_t)nNum;
                            node = ezxml_child(scores,"crystal");
                            if (node != NULL) {
                                pAttr = node->txt;
                                nNum = strtol(pAttr,NULL,10);
                                Playfield.uScoreCrystal = (uint32_t)nNum;
                                node = ezxml_child(scores,"letter");
                                if (node != NULL) {
                                    pAttr = node->txt;
                                    nNum = strtol(pAttr,NULL,10);
                                    Playfield.uScoreMessage = (uint32_t)nNum;
                                    node = ezxml_child(scores,"key");
                                    if (node != NULL) {
                                        pAttr = node->txt;
                                        nNum = strtol(pAttr,NULL,10);
                                        Playfield.uScoreKey = (uint32_t)nNum;
                                        node = ezxml_child(scores,"dynamite");
                                        if (node != NULL) {
                                            pAttr = node->txt;
                                            nNum = strtol(pAttr,NULL,10);
                                            Playfield.uScoreDynamite = (uint32_t)nNum;
                                            node = ezxml_child(scores,"hammer");
                                            if (node != NULL) {
                                                pAttr = node->txt;
                                                nNum = strtol(pAttr,NULL,10);
                                                Playfield.uScoreHammer = (uint32_t)nNum;
                                                node = ezxml_child(scores,"nutcracking");
                                                if (node != NULL) {
                                                    pAttr = node->txt;
                                                    nNum = strtol(pAttr,NULL,10);
                                                    Playfield.uScoreNutCracking = (uint32_t)nNum;
                                                    node = ezxml_child(scores,"stoning_beetle");
                                                    if (node != NULL) {
                                                        pAttr = node->txt;
                                                        nNum = strtol(pAttr,NULL,10);
                                                        Playfield.uScoreStoningBeetle = (uint32_t)nNum;
                                                        node = ezxml_child(scores,"stoning_mine");
                                                        if (node != NULL) {
                                                            pAttr = node->txt;
                                                            nNum = strtol(pAttr,NULL,10);
                                                            Playfield.uScoreStoningMine = (uint32_t)nNum;
                                                            node = ezxml_child(scores,"stoning_alien");
                                                            if (node != NULL) {
                                                                pAttr = node->txt;
                                                                nNum = strtol(pAttr,NULL,10);
                                                                Playfield.uScoreStoningAlien = (uint32_t)nNum;
                                                                node = ezxml_child(scores,"stoning_yam");
                                                                if (node != NULL) {
                                                                    pAttr = node->txt;
                                                                    nNum = strtol(pAttr,NULL,10);
                                                                    Playfield.uScoreStoningYam = (uint32_t)nNum;
                                                                    node = ezxml_child(scores,"timecoin");
                                                                    if (node != NULL) {
                                                                        pAttr = node->txt;
                                                                        nNum = strtol(pAttr,NULL,10);
                                                                        Playfield.uScoreTimeCoin = (uint32_t)nNum;
                                                                        nErrorCode = 0;
                                                                    } else {
                                                                        SDL_Log("%s: error in xml file, 'scores->timecoin' not found",__FUNCTION__);
                                                                    }
                                                                } else {
                                                                    SDL_Log("%s: error in xml file, 'scores->stoning_yam' not found",__FUNCTION__);
                                                                }
                                                            } else {
                                                                SDL_Log("%s: error in xml file, 'scores->stoning_alien' not found",__FUNCTION__);
                                                            }
                                                        } else {
                                                            SDL_Log("%s: error in xml file, 'scores->stoning_mine' not found",__FUNCTION__);
                                                        }
                                                    } else {
                                                        SDL_Log("%s: error in xml file, 'scores->stoning_beetle' not found",__FUNCTION__);
                                                    }
                                                } else {
                                                    SDL_Log("%s: error in xml file, 'scores->nutcracking' not found",__FUNCTION__);
                                                }
                                            } else {
                                                SDL_Log("%s: error in xml file, 'scores->hammer' not found",__FUNCTION__);
                                            }
                                        } else {
                                            SDL_Log("%s: error in xml file, 'scores->dynamite' not found",__FUNCTION__);
                                        }
                                    } else {
                                        SDL_Log("%s: error in xml file, 'scores->key' not found",__FUNCTION__);
                                    }
                                } else {
                                    SDL_Log("%s: error in xml file, 'scores->letter' not found",__FUNCTION__);
                                }
                            } else {
                                SDL_Log("%s: error in xml file, 'scores->crystal' not found",__FUNCTION__);
                            }
                        } else {
                            SDL_Log("%s: error in xml file, 'scores->perl' not found",__FUNCTION__);
                        }
                    } else {
                        SDL_Log("%s: error in xml file, 'scores->sapphire' not found",__FUNCTION__);
                    }
                } else {
                    SDL_Log("%s: error in xml file, 'scores->ruby' not found",__FUNCTION__);
                }
            } else {
                SDL_Log("%s: error in xml file, 'scores->emerald' not found",__FUNCTION__);
            }
        } else {
            SDL_Log("%s: error in xml file, 'scores' not found",__FUNCTION__);
        }
    } else {
        SDL_Log("%s: bad xml handle, null pointer",__FUNCTION__);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetOtherLevelValuesFromXml
------------------------------------------------------------------------------
Beschreibung: Ermittelt folgende 'Values' aus der Level-XML-Datei  und trägt
              diese in die Struktur Playfield.x ein.
              * Emerald, die gesammelt werden müssen, um Spiel zu lösen
              * Time / Score - Faktor
              * Ausbreitungsgeschwindigkeit für grünen Käse
Parameter
      Eingang: xml, ezxml_t, gültiges XML-Handle
      Ausgang: -
               -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int GetOtherLevelValuesFromXml(ezxml_t xml) {
    int nErrorCode;
    ezxml_t values,node;
    long int nNum;
    char *pAttr;

    nErrorCode = -1;
    if (xml != NULL) {
        values = ezxml_child(xml,"values");
        if (values != NULL) {
            node = ezxml_child(values,"emeralds_to_collect");
            if (node != NULL) {
                pAttr = node->txt;
                nNum = strtol(pAttr,NULL,10);
                Playfield.uEmeraldsToCollect = (uint32_t)nNum;
                node = ezxml_child(values,"score_time_factor");
                if (node != NULL) {
                    pAttr = node->txt;
                    nNum = strtol(pAttr,NULL,10);
                    Playfield.uTimeScoreFactor = (uint32_t)nNum;
                    node = ezxml_child(values,"speed_cheese_spread");
                    if (node != NULL) {
                        pAttr = node->txt;
                        nNum = strtol(pAttr,NULL,10);
                        Playfield.uCheeseSpreadSpeed = (uint32_t)nNum;
                        nErrorCode = 0;
                    } else {
                        SDL_Log("%s: error in xml file, 'values->speed_cheese_spread' not found",__FUNCTION__);
                    }
                } else {
                    SDL_Log("%s: error in xml file, 'values->score_time_factor' not found",__FUNCTION__);
                }
            } else {
                SDL_Log("%s: error in xml file, 'values->emeralds_to_collect' not found",__FUNCTION__);
            }
        } else {
            SDL_Log("%s: error in xml file, 'values' not found",__FUNCTION__);
        }
    } else {
        SDL_Log("%s: bad xml handle, null pointer",__FUNCTION__);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetLevelTimesFromXml
------------------------------------------------------------------------------
Beschreibung: Ermittelt alle Level-Zeiten aus der Level-XML-Datei und trägt
              diese in die Struktur Playfield.x ein.
Parameter
      Eingang: xml, ezxml_t, gültiges XML-Handle
      Ausgang: -
               -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int GetLevelTimesFromXml(ezxml_t xml) {
    int nErrorCode;
    ezxml_t times,node;
    long int nNum;
    char *pAttr;

    nErrorCode = -1;
    if (xml != NULL) {
        times = ezxml_child(xml,"times");
        if (times != NULL) {
            node = ezxml_child(times,"to_play");
            if (node != NULL) {
                pAttr = node->txt;
                nNum = strtol(pAttr,NULL,10);
                Playfield.uTimeToPlay = (uint32_t)nNum * ge_DisplayMode.refresh_rate;
                node = ezxml_child(times,"wheel_rotation");
                if (node != NULL) {
                    pAttr = node->txt;
                    nNum = strtol(pAttr,NULL,10);
                    Playfield.uTimeWheelRotation = (uint32_t)nNum * ge_DisplayMode.refresh_rate;
                    node = ezxml_child(times,"magic_wall");
                    if (node != NULL) {
                        pAttr = node->txt;
                        nNum = strtol(pAttr,NULL,10);
                        Playfield.uTimeMagicWall = (uint32_t)nNum * ge_DisplayMode.refresh_rate;
                        node = ezxml_child(times,"light");
                        if (node != NULL) {
                            pAttr = node->txt;
                            nNum = strtol(pAttr,NULL,10);
                            Playfield.uTimeLight = (uint32_t)nNum * ge_DisplayMode.refresh_rate;
                            node = ezxml_child(times,"timedoor");
                            if (node != NULL) {
                                pAttr = node->txt;
                                nNum = strtol(pAttr,NULL,10);
                                Playfield.uTimeDoorTime = (uint32_t)nNum * ge_DisplayMode.refresh_rate;
                                node = ezxml_child(times,"timecoin");
                                if (node != NULL) {
                                    pAttr = node->txt;
                                    nNum = strtol(pAttr,NULL,10);
                                    Playfield.uAdditonalTimeCoinTime = (uint32_t)nNum * ge_DisplayMode.refresh_rate;
                                    nErrorCode = 0;
                                } else {
                                    SDL_Log("%s: error in xml file, 'times->timecoin' not found",__FUNCTION__);
                                }
                            } else {
                                SDL_Log("%s: error in xml file, 'times->timedoor' not found",__FUNCTION__);
                            }
                        } else {
                            SDL_Log("%s: error in xml file, 'times->light' not found",__FUNCTION__);
                        }
                    } else {
                        SDL_Log("%s: error in xml file, 'times->magic_wall' not found",__FUNCTION__);
                    }
                } else {
                    SDL_Log("%s: error in xml file, 'times->wheel_rotation' not found",__FUNCTION__);
                }
            } else {
                SDL_Log("%s: error in xml file, 'times->to_play' not found",__FUNCTION__);
            }
        } else {
            SDL_Log("%s: error in xml file, 'times' not found",__FUNCTION__);
        }
    } else {
        SDL_Log("%s: bad xml handle, null pointer",__FUNCTION__);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetLevelInventoryFromXml
------------------------------------------------------------------------------
Beschreibung: Ermittelt alle Gegenstände, die die Spielfigur bei Levelstart
              erhält und trägt diese in die Struktur Playfield.x ein.
Parameter
      Eingang: xml, ezxml_t, gültiges XML-Handle
      Ausgang: -
               -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int GetLevelInventoryFromXml(ezxml_t xml) {
    int nErrorCode;
    ezxml_t inventory,node;
    long int nNum;
    char *pAttr;

    nErrorCode = -1;
    if (xml != NULL) {
        inventory = ezxml_child(xml,"inventory");
        if (inventory != NULL) {
            node = ezxml_child(inventory,"dynamite");
            if (node != NULL) {
                pAttr = node->txt;
                nNum = strtol(pAttr,NULL,10);
                Playfield.uDynamiteCount = (uint32_t)nNum;
                node = ezxml_child(inventory,"hammer");
                if (node != NULL) {
                    pAttr = node->txt;
                    nNum = strtol(pAttr,NULL,10);
                    Playfield.uHammerCount = (uint32_t)nNum;
                    node = ezxml_child(inventory,"white_key");
                    if (node != NULL) {
                        pAttr = node->txt;
                        nNum = strtol(pAttr,NULL,10);
                        Playfield.uWhiteKeyCount = (uint32_t)nNum;
                        nErrorCode = 0;
                    } else {
                        SDL_Log("%s: error in xml file, 'inventory->white_key' not found",__FUNCTION__);
                    }
                } else {
                    SDL_Log("%s: error in xml file, 'inventory->hammer' not found",__FUNCTION__);
                }
            } else {
                SDL_Log("%s: error in xml file, 'inventory->dynamite' not found",__FUNCTION__);
            }
        } else {
            SDL_Log("%s: error in xml file, 'inventory' not found",__FUNCTION__);
        }
    } else {
        SDL_Log("%s: bad xml handle, null pointer",__FUNCTION__);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetReplicatorLighbarrierSettingsFromXml
------------------------------------------------------------------------------
Beschreibung: Ermittelt für alle Replikatoren und Lichtschranken die Einstellungen
                und trägt diese in die Struktur Playfield.x ein.
Parameter
      Eingang: xml, ezxml_t, gültiges XML-Handle
      Ausgang: -
               -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int GetReplicatorLighbarrierSettingsFromXml(ezxml_t xml) {
    int nErrorCode;
    ezxml_t lightbarrier,replicator,color,node;
    long int nNum;
    char *pAttr;

    nErrorCode = -1;
    if (xml != NULL) {
        replicator = ezxml_child(xml,"replicators");
        if (replicator != NULL) {
            color = ezxml_child(replicator,"red");
            if (color != NULL) {
                node = ezxml_child(color,"active");
                if (node != NULL) {
                    pAttr = node->txt;
                    nNum = strtol(pAttr,NULL,10);
                    Playfield.bReplicatorRedOn = (nNum == 1);
                    node = ezxml_child(color,"element");
                    if (node != NULL) {
                        pAttr = node->txt;
                        nNum = strtol(pAttr,NULL,10);
                        Playfield.uReplicatorRedObject = (uint32_t)nNum;
                        nErrorCode = 0;
                        color = ezxml_child(replicator,"green");
                        if (color != NULL) {
                            node = ezxml_child(color,"active");
                            if (node != NULL) {
                                pAttr = node->txt;
                                nNum = strtol(pAttr,NULL,10);
                                Playfield.bReplicatorGreenOn = (nNum == 1);
                                node = ezxml_child(color,"element");
                                if (node != NULL) {
                                    pAttr = node->txt;
                                    nNum = strtol(pAttr,NULL,10);
                                    Playfield.uReplicatorGreenObject = (uint32_t)nNum;
                                    color = ezxml_child(replicator,"blue");
                                    if (color != NULL) {
                                        node = ezxml_child(color,"active");
                                        if (node != NULL) {
                                            pAttr = node->txt;
                                            nNum = strtol(pAttr,NULL,10);
                                            Playfield.bReplicatorBlueOn = (nNum == 1);
                                            node = ezxml_child(color,"element");
                                            if (node != NULL) {
                                                pAttr = node->txt;
                                                nNum = strtol(pAttr,NULL,10);
                                                Playfield.uReplicatorBlueObject = (uint32_t)nNum;
                                                color = ezxml_child(replicator,"yellow");
                                                if (color != NULL) {
                                                    node = ezxml_child(color,"active");
                                                    if (node != NULL) {
                                                        pAttr = node->txt;
                                                        nNum = strtol(pAttr,NULL,10);
                                                        Playfield.bReplicatorYellowOn = (nNum == 1);
                                                        node = ezxml_child(color,"element");
                                                        if (node != NULL) {
                                                            pAttr = node->txt;
                                                            nNum = strtol(pAttr,NULL,10);
                                                            Playfield.uReplicatorYellowObject = (uint32_t)nNum;
                                                            nErrorCode = 0;
                                                        } else {
                                                            SDL_Log("%s: error in xml file, 'replicators->yellow->element' not found",__FUNCTION__);
                                                        }
                                                    } else {
                                                        SDL_Log("%s: error in xml file, 'replicators->yellow->active' not found",__FUNCTION__);
                                                    }
                                                } else {
                                                    SDL_Log("%s: error in xml file, 'replicators->yellow' not found",__FUNCTION__);
                                                }
                                            } else {
                                                SDL_Log("%s: error in xml file, 'replicators->blue->element' not found",__FUNCTION__);
                                            }
                                        } else {
                                            SDL_Log("%s: error in xml file, 'replicators->blue->active' not found",__FUNCTION__);
                                        }
                                    } else {
                                        SDL_Log("%s: error in xml file, 'replicators->blue' not found",__FUNCTION__);
                                    }
                                } else {
                                    SDL_Log("%s: error in xml file, 'replicators->green->element' not found",__FUNCTION__);
                                }
                            } else {
                                SDL_Log("%s: error in xml file, 'replicators->green->active' not found",__FUNCTION__);
                            }
                        } else {
                            SDL_Log("%s: error in xml file, 'replicators->green' not found",__FUNCTION__);
                        }
                    } else {
                        SDL_Log("%s: error in xml file, 'replicators->red->element' not found",__FUNCTION__);
                    }
                } else {
                    SDL_Log("%s: error in xml file, 'replicators->red->active' not found",__FUNCTION__);
                }
            } else {
                SDL_Log("%s: error in xml file, 'replicators->red' not found",__FUNCTION__);
            }
        } else {
            SDL_Log("%s: error in xml file, 'replicators' not found",__FUNCTION__);
        }
    } else {
        SDL_Log("%s: bad xml handle, null pointer",__FUNCTION__);
    }
    if (nErrorCode == 0) {
        nErrorCode = -1;
        lightbarrier = ezxml_child(xml,"lightbarriers");
        if (lightbarrier != NULL) {
            color = ezxml_child(lightbarrier,"red");
            if (color != NULL) {
                node = ezxml_child(color,"active");
                if (node != NULL) {
                    pAttr = node->txt;
                    nNum = strtol(pAttr,NULL,10);
                    Playfield.bLightBarrierRedOn = (nNum == 1);
                    color = ezxml_child(lightbarrier,"green");
                    if (color != NULL) {
                        node = ezxml_child(color,"active");
                        if (node != NULL) {
                            pAttr = node->txt;
                            nNum = strtol(pAttr,NULL,10);
                            Playfield.bLightBarrierGreenOn = (nNum == 1);
                            color = ezxml_child(lightbarrier,"blue");
                            if (color != NULL) {
                                node = ezxml_child(color,"active");
                                if (node != NULL) {
                                    pAttr = node->txt;
                                    nNum = strtol(pAttr,NULL,10);
                                    Playfield.bLightBarrierBlueOn = (nNum == 1);
                                    color = ezxml_child(lightbarrier,"yellow");
                                    if (color != NULL) {
                                        node = ezxml_child(color,"active");
                                        if (node != NULL) {
                                            pAttr = node->txt;
                                            nNum = strtol(pAttr,NULL,10);
                                            Playfield.bLightBarrierYellowOn = (nNum == 1);
                                            nErrorCode = 0;
                                        } else {
                                            SDL_Log("%s: error in xml file, 'lightbarriers->yellow->active' not found",__FUNCTION__);
                                        }
                                    } else {
                                        SDL_Log("%s: error in xml file, 'lightbarriers->yellow' not found",__FUNCTION__);
                                    }
                                } else {
                                    SDL_Log("%s: error in xml file, 'lightbarriers->blue->active' not found",__FUNCTION__);
                                }
                            } else {
                                SDL_Log("%s: error in xml file, 'lightbarriers->blue' not found",__FUNCTION__);
                            }
                        } else {
                            SDL_Log("%s: error in xml file, 'lightbarriers->green->active' not found",__FUNCTION__);
                        }
                    } else {
                        SDL_Log("%s: error in xml file, 'lightbarriers->green' not found",__FUNCTION__);
                    }
                } else {
                    SDL_Log("%s: error in xml file, 'lightbarriers->red->active' not found",__FUNCTION__);
                }
            } else {
                SDL_Log("%s: error in xml file, 'lightbarriers->red' not found",__FUNCTION__);
            }
        } else {
            SDL_Log("%s: error in xml file, 'lightbarriers' not found",__FUNCTION__);
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetLetterMessagesFromXml
------------------------------------------------------------------------------
Beschreibung: Ermittelt alle Nachrichten für das Element 'Briefumschlag', EMERALD_MESSAGE_1 bis EMERALD_MESSAGE_8
               und trägt diese in die Struktur Playfield.x ein.
Parameter
      Eingang: xml, ezxml_t, gültiges XML-Handle
      Ausgang: -
               -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int GetLetterMessagesFromXml(ezxml_t xml) {
    int nErrorCode;
    uint32_t I;
    ezxml_t messages,messagenr;
    char *pAttr;
    char szM[4];    // 'm1' bis 'm8'
    uint32_t uBinaryLen;

    nErrorCode = -1;
    szM[0] = 'm';
    szM[1] = 'x';
    szM[2] = 0;
    if (xml != NULL) {
        messages = ezxml_child(xml,"messages");
        if (messages != NULL) {
            for (I = 1; (I <= EMERALD_MAX_MESSAGES) && (nErrorCode == -1); I++) {
               szM[1] = I + 0x30;
               messagenr = ezxml_child(messages,szM);
               if (messagenr != NULL) {
                    pAttr = messagenr->txt;
                    if (strlen(pAttr) > 0) {
                        Playfield.pMessage[I - 1] = malloc(strlen(pAttr) + 1);  // Zielspeicher wird auf jeden Fall kleiner
                        if (Playfield.pMessage[I - 1] != NULL) {
                            memset(Playfield.pMessage[I - 1],0,strlen(pAttr) + 1); // Für Stringende sorgen
                            if (Base64ToBin((uint8_t*)Playfield.pMessage[I - 1],(uint8_t*)pAttr,strlen(pAttr),&uBinaryLen) != 0) {
                                SDL_Log("%s: Base64ToBin() failed for Message Nr. %d.",__FUNCTION__,I + 1);
                                nErrorCode = -2;
                            }
                        } else {
                            SDL_Log("%s: can not allocate memory for Message Nr. %d.",__FUNCTION__,I + 1);
                            nErrorCode = -2;
                        }
                    }
               } else {
                    SDL_Log("%s: error in xml file, 'messages->%s' not found",__FUNCTION__,szM);
                    nErrorCode = -2;
               }
            }
            nErrorCode = 0;
        } else {
            SDL_Log("%s: error in xml file, 'messages' not found",__FUNCTION__);
        }
    } else {
        SDL_Log("%s: bad xml handle, null pointer",__FUNCTION__);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetYamExplosionsFromXml
------------------------------------------------------------------------------
Beschreibung: Ermittelt YAM-Explosionen und trägt diese in die Struktur Playfield.x ein.
Parameter
      Eingang: xml, ezxml_t, gültiges XML-Handle
      Ausgang: -
               -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int GetYamExplosionsFromXml(ezxml_t xml) {
    int nErrorCode;
    long int nNum;
    uint32_t I;
    uint32_t E;
    ezxml_t yams,explosion,element;
    char *pAttr;
    char szTag[32];

    nErrorCode = -1;
    yams = ezxml_child(xml,"max_yam_explosions");
    if (yams != NULL) {
        pAttr = yams->txt;
        nNum = strtol(pAttr,NULL,10);
        if ((nNum > 0) && (nNum <= EMERALD_MAX_YAM_EXPLOSIONS)) {
            Playfield.uMaxYamExplosionIndex = (uint32_t)nNum - 1;
            yams = ezxml_child(xml,"yam_explosions");
            if (yams != NULL) {
                nErrorCode = 0;
                for (I = 0; (I <= Playfield.uMaxYamExplosionIndex) && (nErrorCode == 0); I++) {
                    sprintf(szTag,"explosion%02d",I + 1);
                    explosion = ezxml_child(yams,szTag);
                    if (explosion != NULL) {
                        for (E = 0; (E < 9) && (nErrorCode == 0); E++) {
                            sprintf(szTag,"element%02d",E + 1);
                            element = ezxml_child(explosion,szTag);
                            if (element != NULL) {
                                pAttr = element->txt;
                                nNum = strtol(pAttr,NULL,10);
                                Playfield.YamExplosions[I].uElement[E] = (uint16_t)nNum;
                            } else {
                                SDL_Log("%s: error in xml file, tag <%s> not found",__FUNCTION__,szTag);
                                nErrorCode = -1;
                            }
                        }
                    } else {
                        SDL_Log("%s: error in xml file, tag <%s> not found",__FUNCTION__,szTag);
                        nErrorCode = -1;
                    }
                }
            } else {
                SDL_Log("%s: error in xml file, 'yam_explosions' not found",__FUNCTION__);
            }
        } else {
            SDL_Log("%s: error in xml file, bad yam explosions: %u",__FUNCTION__,(uint32_t)nNum);
        }
    } else {
        SDL_Log("%s: error in xml file, 'max_yam_explosions' not found",__FUNCTION__);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           GetLeveldataFromXml
------------------------------------------------------------------------------
Beschreibung:  Holt die eigentlichen Labyrinth-Leveldaten aus der Level-XML-Datei
               und kopiert diese in die Struktur Playfield.pLevel. Der Speicher
               muss dort bereits alloziert worden sein.
Parameter
      Eingang: xml, ezxml_t, gültiges XML-Handle
      Ausgang: -
               -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int GetLeveldataFromXml(ezxml_t xml) {
    int nErrorCode;
    ezxml_t leveldata;
    char *pAttr;
    uint32_t uBase64Len;
    uint32_t uBinLen;
    uint32_t I;
    char szMD5String[32 + 1];   // + 1 für Stringende
    char szNum[16];
    MD5Context MD5Leveldata;
    uint8_t *pCompressed;
    uint32_t uUnCompressLen;

    pCompressed = NULL;
    nErrorCode = -1;
    if (xml != NULL) {
        leveldata = ezxml_child(xml,"leveldata");
        if (leveldata != NULL) {
            pAttr = leveldata->txt;
            uBase64Len = strlen(pAttr);
            if (uBase64Len > 0) {
                md5Init(&MD5Leveldata);
                md5Update(&MD5Leveldata,(uint8_t*)pAttr,strlen(pAttr));
                md5Finalize(&MD5Leveldata);
                szMD5String[0] = 0;
                for (I = 0; I < 16; I++) {
                    sprintf(szNum,"%02X",MD5Leveldata.digest[I]);
                    strcat(szMD5String,szNum);
                }
                if (strcasecmp(Playfield.szMd5String,szMD5String) == 0) {
                    // Genaue Länge der Binärdaten ermitteln und Probelauf mit Base64-Daten
                    if (Base64ToBin(NULL,(uint8_t*)pAttr, strlen(pAttr),&uBinLen) != 0) {
                        uBinLen = -1;
                    }
                    if (uBinLen > 0) {
                        pCompressed = malloc(uBinLen);
                        if (pCompressed != NULL) {
                            Base64ToBin(pCompressed,(uint8_t*)pAttr,strlen(pAttr),&uBinLen);
                            uUnCompressLen = Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(uint16_t);
                            if (uncompress((uint8_t*)Playfield.pLevel,(uLong*)&uUnCompressLen,pCompressed,(uLong)uBinLen) == Z_OK) {
                                if (uUnCompressLen == (Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(uint16_t))) {
                                    nErrorCode = 0;
                                } else {
                                    SDL_Log("%s: decompress unexpected len: %u     expected: %u",__FUNCTION__,(uint32_t)uUnCompressLen,(uint32_t)(Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension * sizeof(uint16_t)));
                                }
                            } else {
                                SDL_Log("%s: decompress ERROR",__FUNCTION__);
                            }
                        } else {
                            SDL_Log("%s: malloc() failed for compressed leveldata",__FUNCTION__);
                        }
                    } else {
                        SDL_Log("%s: Base64ToBin() failed",__FUNCTION__);
                    }
                } else {
                    SDL_Log("%s: md5 hash not valid",__FUNCTION__);
                }
            } else {
                SDL_Log("%s: error in xml file, empty level",__FUNCTION__);
            }
        } else {
            SDL_Log("%s: error in xml file, 'leveldata' not found",__FUNCTION__);
        }
    } else {
        SDL_Log("%s: bad xml handle, null pointer",__FUNCTION__);
    }
    SAFE_FREE(pCompressed);
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           InitialisePlayfield
------------------------------------------------------------------------------
Beschreibung: Lädt die Leveldaten und initialisiert das Spielfeld. Das Ergebnis
              wird in der Struktur Playfield.x abgelegt.
              Diese Funktion alloziert Speicher, der später wieder freigegebem
              werden muss: Playfield.pLevel und Playfield.pStatusAnimation
Parameter
      Eingang: pszLevelFilename, char *, Zeiger auf Level-Dateinamen
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x, ge_DisplayMode, g_InternalLevel[]
------------------------------------------------------------------------------*/
int InitialisePlayfield(char *pszLevelFilename) {
    ezxml_t xml;
    bool bFreeXml;      // Zeigt an, dass der Speicher auf Pointer pXml freigegeben werden muss
    int nErrorCode;
    uint32_t I;
    uint32_t uXmlLen;
    uint8_t *pXml;
    uint8_t *pCopyofInternalXml = NULL;

    nErrorCode = -1;
    memset(&Playfield,0,sizeof(Playfield));
    InitYamExplosions(Playfield.YamExplosions);
    Playfield.bInitOK = false;
    for (I = 0; I < EMERALD_MAX_MESSAGES; I++) {
        Playfield.pMessage[I] = NULL;
    }
    pXml = ReadFile(pszLevelFilename,&uXmlLen);
    if (pXml != NULL) {
        bFreeXml = true;
        SDL_Log("%s: using xml from file %s, filesize: %u",__FUNCTION__,pszLevelFilename,uXmlLen);
    } else {
        bFreeXml = false;
        SDL_Log("%s: using internal level, size: %u",__FUNCTION__,(uint32_t)strlen((char*)g_InternalLevel));
        pCopyofInternalXml = malloc(strlen((char*)g_InternalLevel) + 1);    // ezxml macht originale-XML-datei kaputt?
        if (pCopyofInternalXml != NULL) {
            strcpy((char*)pCopyofInternalXml,(char*)g_InternalLevel);
            pXml = pCopyofInternalXml;
        } else {
            return -1;
        }
    }
    xml = ezxml_parse_str((char*)pXml,strlen((char*)pXml));
    if (xml != NULL) {
        // Zunächst X- und Y-Dimension ermitteln. Diese Werte werden benötigt, um die Größe des Levelspeichers zu bestimmen
        if (GetLeveldimensionFromXml(xml,&Playfield.uLevel_X_Dimension,&Playfield.uLevel_Y_Dimension) == 0) {
            if (GetMemoryForPlayfield() == 0) {
                if (GetTitleAuthorVersionHashFromXml(xml) == 0) {
                    if (GetLevelScoresFromXml(xml) == 0) {
                        if (GetOtherLevelValuesFromXml(xml) == 0) {
                            if (GetLevelTimesFromXml(xml) == 0) {
                                if (GetLevelInventoryFromXml(xml) == 0) {
                                    if (GetReplicatorLighbarrierSettingsFromXml(xml) == 0) {
                                        if (GetLetterMessagesFromXml(xml) == 0) {
                                            if (GetYamExplosionsFromXml(xml) == 0) {
                                                if (GetLeveldataFromXml(xml) == 0) {
                                                    if (GetManCoordinates(Playfield.pLevel,Playfield.uLevel_X_Dimension,Playfield.uLevel_Y_Dimension,&Playfield.uManXpos,&Playfield.uManYpos) == 0) {
                                                        if (CheckReplicators(Playfield.pLevel,Playfield.uLevel_X_Dimension,Playfield.uLevel_Y_Dimension) == 0) {
                                                            if (CheckAcidPools(Playfield.pLevel,Playfield.uLevel_X_Dimension,Playfield.uLevel_Y_Dimension) == 0) {
                                                                if (CheckLevelBorder() == 0) {
                                                                    CloseAllDoors();
                                                                    nErrorCode = 0;
                                                                    Playfield.bInitOK = true;
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    } else {
        SDL_Log("%s: ezxml_parse_str() failed",__FUNCTION__);
    }
    ezxml_free(xml);    // Prüft selbst, ob pointer NULL ist
    if (bFreeXml) {
        SAFE_FREE(pXml);
    }
    SAFE_FREE(pCopyofInternalXml);
    if (Playfield.bInitOK) {
        // Sichtbaren Bereich berechnen
        Playfield.uVisibleY = ((WINDOW_H - PANEL_H) / FONT_H);
        Playfield.uVisibleX = WINDOW_W / FONT_W;
        Playfield.uVisibleCenterY = Playfield.uVisibleY / 2;
        Playfield.uVisibleCenterX = Playfield.uVisibleX / 2;
        // Y-Zentrierung des Levels im Anzeigebereich
        if (Playfield.uVisibleY >= Playfield.uLevel_Y_Dimension) {    // Level ist in Y-Richtung kleiner bzw. gleich der Anzeigehöhe (in Y-Richtung wird nicht gescrollt)
            Playfield.uShiftLevelYpix = ((Playfield.uVisibleY - Playfield.uLevel_Y_Dimension) / 2) * FONT_H;    // Level im Anzeigebereich zentrieren
        } else {
            // Level ist in Y-Richtung größer als Anzeigebereich, daher oben anfangen (es muss ggf. in Y-Richtung gescrollt werden)
            Playfield.uShiftLevelYpix = 0;
        }
        // X-Zentrierung des Levels im Anzeigebereich
        if (Playfield.uVisibleX >= Playfield.uLevel_X_Dimension) {    // Level ist in X-Richtung kleiner bzw. gleich der Anzeigebreite (in X-Richtung wird nicht gescrollt)
            Playfield.uShiftLevelXpix = ((Playfield.uVisibleX - Playfield.uLevel_X_Dimension) / 2) * FONT_H;    // Level im Anzeigebereich zentrieren
        } else {
            // Level ist in X-Richtung größer als Anzeigebereich, daher oben anfangen (es muss ggf. in Y-Richtung gescrollt werden)
            Playfield.uShiftLevelXpix = 0;
        }
        // Positionsüberläufe abfangen
        Playfield.nMaxXpos = (Playfield.uLevel_X_Dimension * FONT_W) - WINDOW_W;
        if (Playfield.nMaxXpos < 0) {
            Playfield.nMaxXpos = 0;
        }
        Playfield.nMaxYpos = (Playfield.uLevel_Y_Dimension * FONT_H) - WINDOW_H + PANEL_H;
        if (Playfield.nMaxYpos < 0) {
            Playfield.nMaxYpos = 0;
        }
        // Startposition im Level
        if (Playfield.uManXpos >= Playfield.uVisibleCenterX) {
            Playfield.nTopLeftXpos = (Playfield.uManXpos - Playfield.uVisibleCenterX) * FONT_W;
        } else {
            Playfield.nTopLeftXpos = 0;
        }
        if (Playfield.uManYpos >= Playfield.uVisibleCenterY) {
            Playfield.nTopLeftYpos = (Playfield.uManYpos - Playfield.uVisibleCenterY) * FONT_H;
        } else {
            Playfield.nTopLeftYpos = 0;
        }
        Playfield.uTotalScore = 0;
        Playfield.uFrameCounter = 0;
        Playfield.bMagicWallRunning = false;
        Playfield.bMagicWallWasOn = false;
        Playfield.uTimeMagicWallLeft = 0;
        Playfield.uTimeLightLeft = 0;
        Playfield.bWheelRunning = false;
        Playfield.uTimeWheelRotationLeft = 0;
        Playfield.uWheelRunningXpos = 0;      // nur gültig, wenn bWheelRunning = true
        Playfield.uWheelRunningYpos = 0;      // nur gültig, wenn bWheelRunning = true
        Playfield.bHasRedKey = false;
        Playfield.bHasGreenKey = false;
        Playfield.bHasBlueKey = false;
        Playfield.bHasYellowKey = false;
        Playfield.bHasGeneralKey = false;
        Playfield.bManDead = false;
        Playfield.bWellDone = false;
        Playfield.bLightOn = false;
        Playfield.uTimeDoorTimeLeft = 0;
        Playfield.bSwitchDoorState = false;
        Playfield.bSwitchDoorImpluse = false;
        Playfield.uShowMessageNo = 0;
        Playfield.uYamExplosion = 0;          // Aktuelle YAM-Explosion
        InitRollUnderground();
        PrintPlayfieldValues();
        SetCentralExplosionCoordinates();
        SetCentralMegaExplosionCoordinates();
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           SetCentralExplosionCoordinates
------------------------------------------------------------------------------
Beschreibung: Hinterlegt die 3x3-Koordinaten für eine zentrale Explosion ab Mittelpunkt.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void SetCentralExplosionCoordinates() {
    Playfield.nCentralExplosionCoordinates[0] = -(int)Playfield.uLevel_X_Dimension - 1;     // oben links
    Playfield.nCentralExplosionCoordinates[1] = -(int)Playfield.uLevel_X_Dimension;         // oben mitte
    Playfield.nCentralExplosionCoordinates[2] = -(int)Playfield.uLevel_X_Dimension + 1;     // oben rechts
    Playfield.nCentralExplosionCoordinates[3] = -1;                                         // mitte links
    Playfield.nCentralExplosionCoordinates[4] = 1;                                          // mitte rechts
    Playfield.nCentralExplosionCoordinates[5] = (int)Playfield.uLevel_X_Dimension - 1;     // unten links
    Playfield.nCentralExplosionCoordinates[6] = (int)Playfield.uLevel_X_Dimension;         // unten mitte
    Playfield.nCentralExplosionCoordinates[7] = (int)Playfield.uLevel_X_Dimension + 1;     // unten rechts
}


/*----------------------------------------------------------------------------
Name:           SetCentralMegaExplosionCoordinates
------------------------------------------------------------------------------
Beschreibung: Hinterlegt die  Koordinaten für eine zentrale Mega-Explosion ab Mittelpunkt.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void SetCentralMegaExplosionCoordinates(void) {
// TODO

}



/*----------------------------------------------------------------------------
Name:           InitYamExplosions
------------------------------------------------------------------------------
Beschreibung: Stellt für alle Yam-Explosionen das Element auf EMERALD_SPACE.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: -
------------------------------------------------------------------------------*/
void InitYamExplosions(YAMEXPLOSION *pYamExplosions) {
    uint32_t I;     // Yam-Explosion
    uint32_t E;     // Element einer Yam-Explosion

    for (I = 0; I < EMERALD_MAX_YAM_EXPLOSIONS; I++) {
        for (E = 0; E < 9; E++) {
            pYamExplosions[I].uElement[E] = EMERALD_SPACE;
        }
    }
}


/*----------------------------------------------------------------------------
Name:           CheckLevelBorder
------------------------------------------------------------------------------
Beschreibung: Prüft, ob die Level-Umrandung mit "Stahl" an jeder Stelle abgrenzt.
Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
int CheckLevelBorder(void) {
    uint32_t I;
    int nErrorCode;

    nErrorCode = 0;
    // Obere Zeile prüfen
    for (I = 0; (I < Playfield.uLevel_X_Dimension) && (nErrorCode == 0); I++) {
        if (!IsSteel(Playfield.pLevel[I])) {
            SDL_Log("%s[top line]  invalid element (%x) found at %d",__FUNCTION__,Playfield.pLevel[I],I);
            nErrorCode = -1;
        }
    }
    // Untere Zeile prüfen
    for (I = Playfield.uLevel_X_Dimension * (Playfield.uLevel_Y_Dimension - 1); (I < Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension) && (nErrorCode == 0); I++) {
        if (!IsSteel(Playfield.pLevel[I])) {
            SDL_Log("%s[buttom line]  invalid element (%x) found at %d",__FUNCTION__,Playfield.pLevel[I],I);
            nErrorCode = -1;
        }
    }
    // Linke Spalte prüfen
    for (I = 0; (I < Playfield.uLevel_Y_Dimension) && (nErrorCode == 0); I++) {

        if (!IsSteel(Playfield.pLevel[I * Playfield.uLevel_X_Dimension])) {
            SDL_Log("%s[left column]  invalid element (%x) found at %d",__FUNCTION__,Playfield.pLevel[I],I);
            nErrorCode = -1;
        }
    }
    // Rechte Spalte prüfen
    for (I = 1; (I <= Playfield.uLevel_Y_Dimension) && (nErrorCode == 0); I++) {
        if (!IsSteel(Playfield.pLevel[I * Playfield.uLevel_X_Dimension - 1])) {
            SDL_Log("%s[right column]  invalid element (%x) found at %d",__FUNCTION__,Playfield.pLevel[I],I);
            nErrorCode = -1;
        }
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           CheckReplicators
------------------------------------------------------------------------------
Beschreibung: Prüft, ob im Level die Replikatoren korrekt zusammengebaut sind.
Parameter
      Eingang: pLevel, uint16_t *, Zeiger auf Leveldaten
               uXdim, uint32_t, X-Dimension des Levels
               uYdim, uint32_t, Y-Dimension des Levels
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int CheckReplicators(uint16_t *pLevel, uint32_t uXdim, uint32_t uYdim) {
    uint32_t I;
    uint32_t uMaxIndex;
    int nErrorCode;
    uint16_t uElement;

    if (pLevel == NULL) {
        return -1;
    }
    nErrorCode = 0;
    uMaxIndex = (uXdim * uYdim) - 1;
    for (I = 0; (I <= uMaxIndex) && (nErrorCode == 0); I++) {
        uElement = pLevel[I];
        switch (uElement) {
            // Roter Replikator
            case (EMERALD_REPLICATOR_RED_TOP_LEFT):
                if ((I + 1) <= uMaxIndex) {
                    if (pLevel[I + 1] == EMERALD_REPLICATOR_RED_TOP_MID) {
                        if ((I + uXdim) <= uMaxIndex) {
                            if (pLevel[I + uXdim] != EMERALD_REPLICATOR_RED_BOTTOM_LEFT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_RED_TOP_MID):
                if ((I - 1) >= 0) {
                    if (pLevel[I - 1] == EMERALD_REPLICATOR_RED_TOP_LEFT) {
                        if ((I + 1) <= uMaxIndex) {
                            if (pLevel[I + 1] != EMERALD_REPLICATOR_RED_TOP_RIGHT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_RED_TOP_RIGHT):
                if ((I - 1) >= 0) {
                    if (pLevel[I - 1] == EMERALD_REPLICATOR_RED_TOP_MID) {
                        if ((I + uXdim) <= uMaxIndex) {
                            if (pLevel[I + uXdim] != EMERALD_REPLICATOR_RED_BOTTOM_RIGHT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_RED_BOTTOM_LEFT):
                if ((I - uXdim) >= 0) {
                    if (pLevel[I - uXdim] != EMERALD_REPLICATOR_RED_TOP_LEFT) {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_RED_BOTTOM_RIGHT):
                if ((I - uXdim) >= 0) {
                    if (pLevel[I - uXdim] != EMERALD_REPLICATOR_RED_TOP_RIGHT) {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            // Grüner Replikator
            case (EMERALD_REPLICATOR_GREEN_TOP_LEFT):
                if ((I + 1) <= uMaxIndex) {
                    if (pLevel[I + 1] == EMERALD_REPLICATOR_GREEN_TOP_MID) {
                        if ((I + uXdim) <= uMaxIndex) {
                            if (pLevel[I + uXdim] != EMERALD_REPLICATOR_GREEN_BOTTOM_LEFT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_GREEN_TOP_MID):
                if ((I - 1) >= 0) {
                    if (pLevel[I - 1] == EMERALD_REPLICATOR_GREEN_TOP_LEFT) {
                        if ((I + 1) <= uMaxIndex) {
                            if (pLevel[I + 1] != EMERALD_REPLICATOR_GREEN_TOP_RIGHT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_GREEN_TOP_RIGHT):
                if ((I - 1) >= 0) {
                    if (pLevel[I - 1] == EMERALD_REPLICATOR_GREEN_TOP_MID) {
                        if ((I + uXdim) <= uMaxIndex) {
                            if (pLevel[I + uXdim] != EMERALD_REPLICATOR_GREEN_BOTTOM_RIGHT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_GREEN_BOTTOM_LEFT):
                if ((I - uXdim) >= 0) {
                    if (pLevel[I - uXdim] != EMERALD_REPLICATOR_GREEN_TOP_LEFT) {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_GREEN_BOTTOM_RIGHT):
                if ((I - uXdim) >= 0) {
                    if (pLevel[I - uXdim] != EMERALD_REPLICATOR_GREEN_TOP_RIGHT) {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            // Blauer Replikator
            case (EMERALD_REPLICATOR_BLUE_TOP_LEFT):
                if ((I + 1) <= uMaxIndex) {
                    if (pLevel[I + 1] == EMERALD_REPLICATOR_BLUE_TOP_MID) {
                        if ((I + uXdim) <= uMaxIndex) {
                            if (pLevel[I + uXdim] != EMERALD_REPLICATOR_BLUE_BOTTOM_LEFT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_BLUE_TOP_MID):
                if ((I - 1) >= 0) {
                    if (pLevel[I - 1] == EMERALD_REPLICATOR_BLUE_TOP_LEFT) {
                        if ((I + 1) <= uMaxIndex) {
                            if (pLevel[I + 1] != EMERALD_REPLICATOR_BLUE_TOP_RIGHT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_BLUE_TOP_RIGHT):
                if ((I - 1) >= 0) {
                    if (pLevel[I - 1] == EMERALD_REPLICATOR_BLUE_TOP_MID) {
                        if ((I + uXdim) <= uMaxIndex) {
                            if (pLevel[I + uXdim] != EMERALD_REPLICATOR_BLUE_BOTTOM_RIGHT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_BLUE_BOTTOM_LEFT):
                if ((I - uXdim) >= 0) {
                    if (pLevel[I - uXdim] != EMERALD_REPLICATOR_BLUE_TOP_LEFT) {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_BLUE_BOTTOM_RIGHT):
                if ((I - uXdim) >= 0) {
                    if (pLevel[I - uXdim] != EMERALD_REPLICATOR_BLUE_TOP_RIGHT) {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            // Gelber Replikator
            case (EMERALD_REPLICATOR_YELLOW_TOP_LEFT):
                if ((I + 1) <= uMaxIndex) {
                    if (pLevel[I + 1] == EMERALD_REPLICATOR_YELLOW_TOP_MID) {
                        if ((I + uXdim) <= uMaxIndex) {
                            if (pLevel[I + uXdim] != EMERALD_REPLICATOR_YELLOW_BOTTOM_LEFT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_YELLOW_TOP_MID):
                if ((I - 1) >= 0) {
                    if (pLevel[I - 1] == EMERALD_REPLICATOR_YELLOW_TOP_LEFT) {
                        if ((I + 1) <= uMaxIndex) {
                            if (pLevel[I + 1] != EMERALD_REPLICATOR_YELLOW_TOP_RIGHT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_YELLOW_TOP_RIGHT):
                if ((I - 1) >= 0) {
                    if (pLevel[I - 1] == EMERALD_REPLICATOR_YELLOW_TOP_MID) {
                        if ((I + uXdim) <= uMaxIndex) {
                            if (pLevel[I + uXdim] != EMERALD_REPLICATOR_YELLOW_BOTTOM_RIGHT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_YELLOW_BOTTOM_LEFT):
                if ((I - uXdim) >= 0) {
                    if (pLevel[I - uXdim] != EMERALD_REPLICATOR_YELLOW_TOP_LEFT) {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_REPLICATOR_YELLOW_BOTTOM_RIGHT):
                if ((I - uXdim) >= 0) {
                    if (pLevel[I - uXdim] != EMERALD_REPLICATOR_YELLOW_TOP_RIGHT) {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            default:
                // kein Replikator-Element
                break;
        }
    }
    if (nErrorCode != 0) {
        SDL_Log("%s: bad replicator found",__FUNCTION__);
    }
    return nErrorCode;
}


/*----------------------------------------------------------------------------
Name:           CheckAcidPools
------------------------------------------------------------------------------
Beschreibung: Prüft, ob im Level die Säurebecken korrekt zusammengebaut sind.
Parameter
      Eingang: pLevel, uint16_t *, Zeiger auf Leveldaten
               uXdim, uint32_t, X-Dimension des Levels
               uYdim, uint32_t, Y-Dimension des Levels
      Ausgang: -
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int CheckAcidPools(uint16_t *pLevel, uint32_t uXdim, uint32_t uYdim) {
    uint32_t I;
    uint32_t uMaxIndex;
    int nErrorCode;
    uint16_t uElement;

    if (pLevel == NULL) {
        return -1;
    }
    nErrorCode = 0;
    uMaxIndex = (uXdim * uYdim) - 1;
    for (I = 0; (I <= uMaxIndex) && (nErrorCode == 0); I++) {
        uElement = pLevel[I];
        switch (uElement) {
            // Roter Replikator
            case (EMERALD_ACIDPOOL_TOP_LEFT):
                if ((I + uXdim) <= uMaxIndex) {     // Schließt folgende Prüfung (I + 1) mit ein
                    if (pLevel[I + uXdim] == EMERALD_ACIDPOOL_BOTTOM_LEFT) {
                        if (pLevel[I + 1] != EMERALD_ACIDPOOL_TOP_MID) {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_ACIDPOOL_TOP_MID):
                if ((I - 1) >= 0) {
                    if ((pLevel[I - 1] == EMERALD_ACIDPOOL_TOP_MID) || (pLevel[I - 1] == EMERALD_ACIDPOOL_TOP_LEFT)) {
                        if ((I + uXdim) <= uMaxIndex) {     // Schließt folgende Prüfung (I + 1) mit ein
                            if (pLevel[I + uXdim] == EMERALD_ACIDPOOL_BOTTOM_MID) {
                                if ((pLevel[I + 1] != EMERALD_ACIDPOOL_TOP_MID) && (pLevel[I + 1] != EMERALD_ACIDPOOL_TOP_RIGHT)) {
                                    nErrorCode = -1;
                                }
                            } else {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_ACIDPOOL_TOP_RIGHT):
                if ((I - 1) >= 0) {
                    if (pLevel[I - 1] == EMERALD_ACIDPOOL_TOP_MID) {
                        if ((I + uXdim) <= uMaxIndex) {     // Schließt folgende Prüfung (I + 1) mit ein
                            if (pLevel[I + uXdim] != EMERALD_ACIDPOOL_BOTTOM_RIGHT) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_ACIDPOOL_BOTTOM_LEFT):
                if ((I - uXdim) >= 0) {
                    if (pLevel[I - uXdim] == EMERALD_ACIDPOOL_TOP_LEFT) {
                        if ((I + 1) <= uMaxIndex) {
                            if (pLevel[I + 1] != EMERALD_ACIDPOOL_BOTTOM_MID) {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_ACIDPOOL_BOTTOM_MID):
                if ((I - uXdim) >= 0) { // Schließt folgende Prüfung (I - 1) mit ein
                    if (pLevel[I - uXdim] == EMERALD_ACIDPOOL_TOP_MID) {
                        if ((pLevel[I - 1] == EMERALD_ACIDPOOL_BOTTOM_MID) || (pLevel[I - 1] == EMERALD_ACIDPOOL_BOTTOM_LEFT)) {
                            if ((I + 1) <= uMaxIndex) {
                                if ((pLevel[I + 1] != EMERALD_ACIDPOOL_BOTTOM_MID) && (pLevel[I + 1] != EMERALD_ACIDPOOL_BOTTOM_RIGHT)) {
                                    nErrorCode = -1;
                                }
                            } else {
                                nErrorCode = -1;
                            }
                        } else {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
            case (EMERALD_ACIDPOOL_BOTTOM_RIGHT):
                if ((I - uXdim) >= 0) { // Schließt folgende Prüfung (I - 1) mit ein
                    if (pLevel[I - uXdim] == EMERALD_ACIDPOOL_TOP_RIGHT) {
                        if (pLevel[I - 1] != EMERALD_ACIDPOOL_BOTTOM_MID) {
                            nErrorCode = -1;
                        }
                    } else {
                        nErrorCode = -1;
                    }
                } else {
                    nErrorCode = -1;
                }
                break;
        }
    }
    if (nErrorCode != 0) {
        SDL_Log("%s: bad acid pool found",__FUNCTION__);
    }
    return nErrorCode;
}

/*----------------------------------------------------------------------------
Name:           GetManCoordinates
------------------------------------------------------------------------------
Beschreibung: Ermittelt die X- und Y-Koordinate des Man im Level.

Parameter
      Eingang: pLevel, uint16_t *, Zeiger auf Leveldaten
               uXdim, uint32_t, X-Dimension des Levels
               uYdim, uint32_t, Y-Dimension des Levels
      Ausgang: puManXpos, uint32_t *, X-Koordinate des Man, darf NULL sein
               puManYpos, uint32_t *, Y-Koordinate des Man, darf NULL sein
Rückgabewert:  int , 0 = OK, sonst Fehler
Seiteneffekte: -
------------------------------------------------------------------------------*/
int GetManCoordinates(uint16_t *pLevel, uint32_t uXdim, uint32_t uYdim, uint32_t *puManXpos, uint32_t *puManYpos) {
    int nErrorcode;
    int nManCount;
    uint32_t I;                 // Index im Level
    uint32_t uLinCoordinate;    // Lineare Koordinate

    nManCount = 0;
    nErrorcode = -1;
    // Das Gröbste wurde bereits in InitialisePlayfield() geprüft
    if (pLevel != NULL) {
        // Prüfen, ob sich genau ein Man im Level befindet
        for (I = 0; I < (uXdim * uYdim); I++) {
            if (pLevel[I] == EMERALD_MAN) {
                uLinCoordinate = I;
                nManCount++;
            }
        }
        if (nManCount == 1) {
            // Lineare Koordinate in X- und Y-Koordinate zerlegen
            if (puManXpos != NULL) {
                *puManXpos = uLinCoordinate % uXdim;
            }
            if (puManYpos != NULL) {
                *puManYpos = uLinCoordinate / uXdim;
            }
            nErrorcode = 0;
        } else {
            SDL_Log("%s: invalid man count: %d",__FUNCTION__,nManCount);
        }

    }
    return nErrorcode;
}


/*----------------------------------------------------------------------------
Name:           CloseAllDoors
------------------------------------------------------------------------------
Beschreibung: Schließt alle Türen (Timer+Schalter) in einem Level.

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void CloseAllDoors(void) {
    uint32_t I;                 // Index im Level

    for (I = 0; I < (Playfield.uLevel_X_Dimension * Playfield.uLevel_Y_Dimension); I++) {
        if (Playfield.pLevel[I] == EMERALD_DOOR_TIME) {
            Playfield.pStatusAnimation[I] = EMERALD_STATUS_DOOR_CLOSE;
        }
    }
}


/*----------------------------------------------------------------------------
Name:           PrintPlayfieldValues
------------------------------------------------------------------------------
Beschreibung: Zeigt die Werte der Struktur Playfield.x an.

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  -
Seiteneffekte: Playfield.x
------------------------------------------------------------------------------*/
void PrintPlayfieldValues() {
    if (Playfield.bInitOK) {
        printf("Playfield-Values\r\n");
        printf("================\r\n");
        printf("InitOK:                      %d\r\n",Playfield.bInitOK);
        printf("Version:                     %s\r\n",Playfield.szVersion);       // z.B. "01.00"
        printf("Level title:                 %s\r\n",Playfield.szLevelTitle);
        printf("Level author:                %s\r\n",Playfield.szLevelAuthor);
        printf("Score Emerald:               %d\r\n",Playfield.uScoreEmerald);
        printf("Score Ruby:                  %d\r\n",Playfield.uScoreRuby);
        printf("Score Saphire:               %d\r\n",Playfield.uScoreSaphir);
        printf("Score Perl:                  %d\r\n",Playfield.uScorePerl);
        printf("Score crystal:               %d\r\n",Playfield.uScoreCrystal);
        printf("Score Letter:                %d\r\n",Playfield.uScoreMessage);
        printf("Score Key:                   %d\r\n",Playfield.uScoreKey);
        printf("Score Dynamite:              %d\r\n",Playfield.uScoreDynamite);
        printf("Score Hammer:                %d\r\n",Playfield.uScoreHammer);
        printf("Score Nut cracking:          %d\r\n",Playfield.uScoreNutCracking);
        printf("Score stoning beetle:        %d\r\n",Playfield.uScoreStoningBeetle);
        printf("Score stoning mine:          %d\r\n",Playfield.uScoreStoningMine);
        printf("Score stoning alien:         %d\r\n",Playfield.uScoreStoningAlien);
        printf("Score stoning yam:           %d\r\n",Playfield.uScoreStoningYam);
        printf("Score time coin:             %d\r\n",Playfield.uScoreTimeCoin);
        printf("Additional time for coin:    %d\r\n",Playfield.uAdditonalTimeCoinTime);
        printf("Emerald to collect:          %d\r\n",Playfield.uEmeraldsToCollect);
        printf("Dynamite Count:              %d\r\n",Playfield.uDynamiteCount);
        printf("Hammer Count:                %d\r\n",Playfield.uHammerCount);
        printf("White key Count:             %d\r\n",Playfield.uWhiteKeyCount);
        printf("Time Score Factor:           %d\r\n",Playfield.uTimeScoreFactor);
        printf("Cheese spread speed:         %d\r\n",Playfield.uCheeseSpreadSpeed);
        printf("Time to play:                %d\r\n",Playfield.uTimeToPlay);
        printf("Time Wheel Rotation:         %d\r\n",Playfield.uTimeWheelRotation);
        printf("Time magic wall:             %d\r\n",Playfield.uTimeMagicWall);
        printf("Time light:                  %d\r\n",Playfield.uTimeLight);
        printf("Time TimeDoor:               %d\r\n",Playfield.uTimeDoorTime);
        printf("Replicator red, element:     %02X\r\n",Playfield.uReplicatorRedObject);
        printf("Replicator red, on:          %d\r\n",Playfield.bReplicatorRedOn);
        printf("Replicator green, element:   %02X\r\n",Playfield.uReplicatorGreenObject);
        printf("Replicator green, on:        %d\r\n",Playfield.bReplicatorGreenOn);
        printf("Replicator blue, element:    %02X\r\n",Playfield.uReplicatorBlueObject);
        printf("Replicator blue, on:         %d\r\n",Playfield.bReplicatorBlueOn);
        printf("Replicator yellow, element:  %02X\r\n",Playfield.uReplicatorYellowObject);
        printf("Replicator yellow, on:       %d\r\n",Playfield.bReplicatorYellowOn);
        printf("Lighbarrier red, on:         %d\r\n",Playfield.bLightBarrierRedOn);
        printf("Lighbarrier green, on:       %d\r\n",Playfield.bLightBarrierGreenOn);
        printf("Lighbarrier blue, on:        %d\r\n",Playfield.bLightBarrierBlueOn);
        printf("Lighbarrier yellow, on:      %d\r\n",Playfield.bLightBarrierYellowOn);
        printf("MaxYamExplosionIndex:        %d\r\n",Playfield.uMaxYamExplosionIndex);
        printf("Total Score:                 %d\r\n",Playfield.uTotalScore);            // Total-Score
        printf("Level Dimension:             X(%d) x Y(%d)\r\n",Playfield.uLevel_X_Dimension,Playfield.uLevel_Y_Dimension);
        printf("visible Dimension:           X(%d) x Y(%d)\r\n",Playfield.uVisibleX,Playfield.uVisibleY);   // Sichtbarer Bereich (nur von Fenstergröße abhängig)
        printf("visible Center:              X(%d) / Y(%d)\r\n",Playfield.uVisibleCenterX,Playfield.uVisibleCenterY);   // Zentrum des sichtbaren Bereichs (nur von Fenstergröße abhängig)
        printf("top left Pixel Position:     X(%d) / Y(%d)\r\n",Playfield.nTopLeftXpos,Playfield.nTopLeftYpos);           // aktuelle X/Y-Pixelposition, abhängig von Man position
        printf("Man Position:                X(%d) / Y(%d)\r\n",Playfield.uManXpos,Playfield.uManYpos);   // Man-X/Y-Element-Koordinate
        printf("FrameCounter:                %u\r\n",Playfield.uFrameCounter);
        printf("MD5 hash for leveldata:      %s\r\n",Playfield.szMd5String);
    } else {
        printf("Error in level data, can't show playfield values\r\n");
    }
}
