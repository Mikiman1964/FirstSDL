#ifndef MINE_H_INCLUDED
#define MINE_H_INCLUDED

// Prototypen zur Steuerung der Mine
void ControlMineUp(uint32_t I);
void ControlMineRight(uint32_t I);
void ControlMineDown(uint32_t I);
void ControlMineLeft(uint32_t I);
void ControlStandMine(uint32_t I);
bool IsStandMineExplode(uint16_t uElement);
void ControlDynamiteOn(uint32_t I);
void ControlManWithDynamiteOn(uint32_t I);

#endif // MINE_H_INCLUDED
