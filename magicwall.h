#ifndef MAGICWALL_H_INCLUDED
#define MAGICWALL_H_INCLUDED

void CheckRunningMagicWall(void);
void CheckLight(void);
void CheckTimeDoorOpen(void);
void ElementGoesMagicWall(uint32_t I, uint32_t uDestElement);
void ControlMagicWall(uint32_t I);

#endif // MAGICWALL_H_INCLUDED
