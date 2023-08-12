#ifndef HIGHSCORES_H_INCLUDED
#define HIGHSCORES_H_INCLUDED

#include "loadlevel.h"

#define EMERALD_HIGHSCORES_DIRECTORYNAME            "highscores"                        // High-Scores-Directory
#define EMERALD_HIGHSCORES_FILENAME                 "high_"                             // High-Scores-Filename mit Gruppen-Hash
#define EMERALD_HIGHSCORES_FILENAME_EXTENSION       ".dat"                              // High-Scores-Extension
#define EMERALD_HIGHSCORE_LISTLEN                   20                                  // 20 Einträge pro Level


/*
1000 Level / Gruppe
29 Zeichen / Playername + 1 (String-Terminierung)
4 Bytes / Punktzahl
20 Einträge / Level

Max. Platzbedarf pro Levelgruppe:
1000 * 20 * (29 + 1 + 4) = 680.000 Bytes
*/


typedef struct {
    char szTopTwenty[EMERALD_HIGHSCORE_LISTLEN][EMERALD_PLAYERNAME_LEN + 1];
    uint32_t uHighScore[EMERALD_HIGHSCORE_LISTLEN];
} TOPTWENTY;


// Diese Struktur gilt für eine Levelgruppe
typedef struct {
    TOPTWENTY TopTwenty[EMERALD_MAX_LEVELCOUNT];
} HIGHSCOREFILE;


int CheckHighScoresDir(void);
int WriteHighScoreFile(uint8_t *puLevelgroupHash);
int ReadHighScoreFile(uint8_t *puLevelgroupHash);
int InsertScore(char *szName, uint32_t uLevel, uint32_t uScore, bool bWellDone);
int CleanUpHighScoreDir(void);
#endif // HIGHSCORES_H_INCLUDED
