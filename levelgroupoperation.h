#ifndef LEVELGROUPOPERATION_H_INCLUDED
#define LEVELGROUPOPERATION_H_INCLUDED

#include "mySDL.h"
#include "mystd.h"

uint8_t *GetLevelFromLevelgroup(uint8_t *pszLevelgroup, uint32_t uLevelNumber);
int32_t GetLevelTitleFromLevel(uint8_t *pszLevel, uint8_t *pszLevelTitle);
int32_t LevelgroupOperaton_Edit(uint32_t uLevelNumber, DYNSTRING *NewLevel);
int32_t LevelgroupOperaton_Copy(uint32_t uLevelNumber);
int32_t LevelgroupOperaton_CopyClipboard(uint32_t uLevelNumber);
int32_t LevelgroupOperaton_Delete(uint32_t uLevelNumber);
int32_t LevelgroupOperaton_Move(uint32_t uSrcLevelNumber,uint32_t uDestLevelNumber);
int32_t LevelgroupOperaton_NewGroup(void);
int32_t LevelgroupOperaton_RenameGroupname(SDL_Renderer *pRenderer);
int32_t LevelgroupOperaton_Password(SDL_Renderer *pRenderer);
int32_t LevelgroupOperaton_AskPassword(SDL_Renderer *pRenderer);
int32_t LevelgroupOperaton_ImportDC3(SDL_Renderer *pRenderer);
#endif // LEVELGROUPOPERATION_H_INCLUDED
