#ifndef SLIME_H_INCLUDED
#define SLIME_H_INCLUDED

// Prototypen zur Steuerung des Schleims
void ControlSlime(uint32_t I);
void SlimeDown(uint32_t I);
void SlimeUp(uint32_t I);
void SlimeRight(uint32_t I);
void SlimeLeft(uint32_t I);
bool IsSlimeEatElement(uint16_t uElement);
void HandleSlimeEat(uint32_t I, uint32_t uAnimation);
uint32_t GetShrinkByElement(uint16_t uElement);
void ControlSlimeExplosion(uint32_t I);
#endif // SLIME_H_INCLUDED
