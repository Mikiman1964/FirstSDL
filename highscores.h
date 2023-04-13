#ifndef HIGHSCORES_H_INCLUDED
#define HIGHSCORES_H_INCLUDED

#include "loadlevel.h"

#define EMERALD_HIGHSCORES_DIRECTORYNAME            "highscores"                       // High-Scores-Directory
#define EMERALD_HIGHSCORES_FILENAME                 "high_"                             // High-Scores-Filename mit Gruppen-Hash
#define EMERALD_HIGHSCORES_FILENAME_EXTENSION       ".dat"                              // High-Scores-Extension
#define EMERALD_HIGHSCORE_LISTLEN                   20                                  // 20 Einträge pro Level


/*
1000 Level / Gruppe
32 Zeichen / Playername
4 Bytes / Punktzahl
20 Einträge / Level

Max. Platzbedarf pro Levelgruppe:
1000 * 20 * (32 + 4) = 720.000 Bytes
*/


typedef struct {
    char szTopTwenty[EMERALD_HIGHSCORE_LISTLEN][EMERALD_PLAYERNAME_LEN + 1];
    uint32_t uHighScore[EMERALD_HIGHSCORE_LISTLEN];
} TOPTWENTY;


// Diese Struktur gilt für eine Levelgruppe
typedef struct {
    TOPTWENTY TopTwenty[EMERALD_MAX_LEVELCOUNT];
} HIGHSCOREFILE;


int CheckHighScoresDirectory(void);
int WriteHighScoreFile(uint8_t *puLevelgroupHash);
int ReadHighScoreFile(uint8_t *puLevelgroupHash);
int InsertScore(char *szName, uint32_t uLevel, uint32_t uScore);

#endif // HIGHSCORES_H_INCLUDED
