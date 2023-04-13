#ifndef YAM_H_INCLUDED
#define YAM_H_INCLUDED

void ControlYam(uint32_t I);
void CheckYamGoLeft(uint32_t I);
void CheckYamGoRight(uint32_t I);
void CheckYamGoUp(uint32_t I);
void CheckYamGoDown(uint32_t I);
void ControlYamKillsMan(uint32_t I);
bool IsYamCompleteBlocked(uint32_t I);
#endif // YAM_H_INCLUDED
