#ifndef LOADLEVEL_H_INCLUDED
#define LOADLEVEL_H_INCLUDED

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


void PrintPlayfieldValues();
ezxml_t GetLevelTag(ezxml_t xml,uint32_t uLevelNumber);
int GetLeveldimensionFromXml(ezxml_t xml,uint32_t *puX,uint32_t *puY);
int GetTitleAuthorVersionHashFromXml(ezxml_t xml);
int GetLevelScoresFromXml(ezxml_t xml);
int GetOtherLevelValuesFromXml(ezxml_t xml);
int GetLevelTimesFromXml(ezxml_t xml);
int GetReplicatorLighbarrierSettingsFromXml(ezxml_t xml);
int GetConveyorBeltSettingsFromXml(ezxml_t xml);
int GetLetterMessagesFromXml(ezxml_t xml);
int GetYamExplosionsFromXml(ezxml_t xml);
int GetLeveldataFromXml(ezxml_t xml);
int GetMemoryForPlayfield(void);
int InitialisePlayfield(uint32_t uLevelNumber);
void InitYamExplosions(YAMEXPLOSION *pYamExplosions);
int GetManCoordinates(uint16_t *pLevel, uint32_t uXdim, uint32_t uYdim, uint32_t *puManXpos, uint32_t *puManYpos);
int CheckLevelBorder(void);
int CheckReplicators(uint16_t *pLevel, uint32_t uXdim, uint32_t uYdim);
int CheckAcidPools(uint16_t *pLevel, uint32_t uXdim, uint32_t uYdim);
void CloseAllDoors(void);
void SetActiveDynamiteP1(void);
void SetCentralExplosionCoordinates(void);
void SetCentralMegaExplosionCoordinates(void);
void InitLevelgroups(void);
int WriteDefaultLevelgroup(void); // Funktion befindet sich in default_levelgroup.c
int GetLevelgroupFiles(void);
int CalculateLevelGroupMd5Hash(uint8_t *puLevelgroupXml,uint8_t *puMd5Hash);
uint32_t GetLevelgroupIndexByHash(uint8_t *puLevelgroupMd5Hash);
int SelectLevelgroup(uint8_t *puLevelgroupMd5Hash, bool bReadWriteHighscores);
int SelectAlternativeLevelgroup(uint8_t *puLevelgroupMd5Hash, bool bReadWriteHighscores);
void ShowSelectedLevelgroup(void);
void ShowAvailableLevelgroups(void);
int DeleteName(char *pszName);
int InsertNewName(char *pszName);
int SelectName(char *pszname, uint8_t *puHash);
int InsertGamesValuesIntoNamesFile(char *pszName, uint8_t *puHash);
int InsertGroupHashForName(char *pszName, uint8_t *puHash);
int CleanNameHashes(void);
void ShowActualPlayer(void);
void ShowNames(void);
int WriteNamesFile(void);
int WriteDefaultNamesFile(void);
int ReadNamesFile(void);
#endif // LOADLEVEL_H_INCLUDED
