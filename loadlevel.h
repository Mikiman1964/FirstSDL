#ifndef LOADLEVEL_H_INCLUDED
#define LOADLEVEL_H_INCLUDED

#include <stdbool.h>

#include "EmeraldMine.h"
#include "ezxml.h"
#include "mystd.h"

#define EMERALD_LEVELGROUPS_DIRECTORYNAME       "levelgroups"                   // High-Scores-Directory
#define EMERALD_CONFIG_FILENAME                 "config.xml"                    // Konfigurationsfile
#define EMERALD_DEFAULT_LEVELGROUP_FILENAME     "default_levelgroup.xml"        // Diese Levelgruppe ist immer vorhanden
#define EMERALD_NAMES_FILENAME                  "names.dat"                     // Namen mit Handicaps, Games Played, Games Won und Totalscore für jede Levelgruppe

#define EMERALD_GROUPNAME_LEN                   25                              // Maximale Länge für Gruppennamen
#define EMERALD_GROUP_PASSWORD_LEN              25                              // Maximale Länge für Passwort einer Levelgruppe
#define EMERALD_MAX_LEVELGROUPFILES             100                             // Maximale Anzahl von Levelgruppen
#define EMERALD_MAX_FILENAME_LEN                255                             // Maximale Filenamenlänge ist bei den meisten Dateisystemen 255
#define EMERALD_PLAYERNAME_LEN                  29                              // Maximale Spielernamenlänge
#define EMERALD_MAX_LEVELCOUNT                  1000                            // maximale Anzahl Level in einer Levelgruppe
#define EMERALD_MAX_PLAYERNAMES                 100

#define BUTTONLABEL_CREATE_PLAYER               "CREATE_PLAYER"
#define BUTTONLABEL_DELETE_PLAYER               "DELETE_PLAYER"
#define BUTTONLABEL_LEVELEDITOR                 "LEVELEDITOR"
#define BUTTONLABEL_HIGHSCORES                  "SHOW_HIGHSCORES"
#define BUTTONLABEL_EXIT_HIGHSCORES             "EXIT_HIGHSCORES"


/////////////////////// Für names.dat ////////////////START//////////
typedef struct {
    bool     bDelete;   // wird nicht verwendet
    uint8_t  uHash[16];
    uint32_t uHandicap;
    uint32_t uGamesPlayed;
    uint32_t uGamesWon;
    uint32_t uTotalScore;
    uint32_t uPlayTimeS;
} GROUPHASH;


typedef struct {
    char szName[EMERALD_PLAYERNAME_LEN + 1];
    GROUPHASH GroupHash[EMERALD_MAX_LEVELGROUPFILES];
} NAME;


typedef struct {
    uint32_t uNameCount;
    NAME Name[EMERALD_MAX_PLAYERNAMES];
    uint8_t uSecurityHash[16];
} NAMES;
/////////////////////// Für names.dat ////////////////ENDE///////////

typedef struct {
    bool        bValid;                                                         // diese Struktur enthält einen validen Spieler
    char        szPlayername[EMERALD_PLAYERNAME_LEN + 1];                       // Aktueller Spieler
    uint8_t     uLevelgroupMd5Hash[16];                                         // aktuell gewählte Levelgruppe als MD5 Hash
    uint32_t    uLevel;                                                         // aktuell gewähltes Level (wird beim Init. auf Handicap gestellt)
    uint32_t    uHandicap;                                                      // aktuelles Handicap in der gewählten Levelgruppe
    uint32_t    uGamesPlayed;                                                   // Anzahl gespielter Level in der gewählten Levelgruppe
    uint32_t    uGamesWon;                                                      // Anzahl gewonnener Spiele in der gewählten Levelgruppe
    uint32_t    uTotalScore;                                                    // Gesamtpunktezahl in der gewählten Levelgruppe
    uint32_t    uPlayTimeS;                                                     // Gesamtspielzeit in Sekunden in der gewählten Levelgruppe
} ACTUALPLAYER;


typedef struct {
    char szFilename[EMERALD_MAX_FILENAME_LEN + 1];
    char szLevelgroupname[EMERALD_GROUPNAME_LEN + 1];
    char szPasswordHash[32 + 1];
    char szCreateTimestamp[15 + 1];                                             // 20230331_133530
    uint8_t uMd5Hash[16];
    uint32_t uLevelCount;                                                       // Anzahl Level in der Levelgruppe
} LEVELGROUPFILE;


typedef struct {
    bool bOK;
    char szFilename[EMERALD_MAX_FILENAME_LEN + 1];                              // Dateiname der ausgewählten Levelgruppe
    char szLevelgroupname[EMERALD_GROUPNAME_LEN + 1];                           // Levelgruppenname
    char szPasswordHash[32 + 1];
    uint8_t uMd5Hash[16];
    uint32_t uLevelCount;                                                       // Anzahl Level in der ausgewählten Levelgruppe
    char szLevelTitle[EMERALD_MAX_LEVELCOUNT][EMERALD_TITLE_LEN + 1];           // Levelname
    char szLevelAuthor[EMERALD_MAX_LEVELCOUNT][EMERALD_AUTHOR_LEN + 1];         // Levelname
} LEVELGROUP;


void PrintPlayfieldValues(void);
ezxml_t GetLevelTag(ezxml_t xml,uint32_t uLevelNumber);
int32_t GetLeveldimensionFromXml(ezxml_t xml,uint32_t *puX,uint32_t *puY);
int32_t GetTitleAuthorVersionHashFromXml(ezxml_t xml);
int32_t GetLevelScoresFromXml(ezxml_t xml);
int32_t GetOtherLevelValuesFromXml(ezxml_t xml);
int32_t GetLevelTimesFromXml(ezxml_t xml);
int32_t GetReplicatorLighbarrierSettingsFromXml(ezxml_t xml);
int32_t GetConveyorBeltSettingsFromXml(ezxml_t xml);
int32_t GetLetterMessagesFromXml(ezxml_t xml);
int32_t GetTreasureChestElementsFromXml(ezxml_t xml);
int32_t GetQuicksaveFromXml(ezxml_t xml);
int32_t GetYamExplosionsFromXml(ezxml_t xml);
int32_t GetLeveldataFromXml(ezxml_t xml);
int32_t GetMemoryForPlayfield(void);
int32_t InitialisePlayfield(uint32_t uLevelNumber);
void SetVisibleArea(void);
void InitYamExplosions(YAMEXPLOSION *pYamExplosions);
int32_t GetManCoordinates(uint16_t *pLevel, uint32_t uXdim, uint32_t uYdim, uint32_t *puManXpos, uint32_t *puManYpos);
void SetPipeLevel(void);
int32_t CheckLevelBorder(void);
int32_t CheckReplicators(uint16_t *pLevel, int32_t nXdim, int32_t nYdim);
int32_t CheckAcidPools(uint16_t *pLevel, int32_t uXdim, int32_t uYdim);
void CloseAllDoors(void);
void SetActiveDynamite(void);
void SetTreasureChests(void);
void SetCentralExplosionCoordinates(void);
void SetCentralMegaExplosionCoordinates(void);
void InitLevelgroups(void);
int32_t WriteDefaultLevelgroup(void); // Funktion befindet sich in default_levelgroup.c
int32_t GetLevelgroupFiles(void);
void SortLevelGroupsByGroupName(void);
int32_t CalculateLevelGroupMd5Hash(uint8_t *puLevelgroupXml,uint8_t *puMd5Hash);
uint32_t GetLevelgroupIndexByHash(uint8_t *puLevelgroupMd5Hash);
int32_t SelectLevelgroup(uint8_t *puLevelgroupMd5Hash, bool bReadWriteHighscores);
int32_t SelectAlternativeLevelgroup(uint8_t *puLevelgroupMd5Hash, bool bReadWriteHighscores);
void ShowSelectedLevelgroup(void);
void ShowAvailableLevelgroups(void);
int32_t DeleteName(char *pszName);
int32_t InsertNewName(char *pszName);
int32_t SelectName(char *pszname, uint8_t *puHash);
int32_t InsertGamesValuesIntoNamesFile(char *pszName, uint8_t *puHash);
int32_t InsertGroupHashForName(char *pszName, uint8_t *puHash);
int32_t CleanNameHashes(void);
void ShowActualPlayer(void);
void ShowNames(void);
int32_t WriteNamesFile(void);
int32_t WriteDefaultNamesFile(void);
int32_t ReadNamesFile(void);
#endif // LOADLEVEL_H_INCLUDED
