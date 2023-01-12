#ifndef LIGHTBARRIER_H_INCLUDED
#define LIGHTBARRIER_H_INCLUDED


#define EMERALD_BARRIER_COLOR_RED           0
#define EMERALD_BARRIER_COLOR_GREEN         1
#define EMERALD_BARRIER_COLOR_BLUE          2
#define EMERALD_BARRIER_COLOR_YELLOW        3


void CleanLightBarriers(void);
void ControlLightBarriers(void);
void ControlBarrierUp(uint32_t I, uint32_t uColor);
void ControlBarrierDown(uint32_t I, uint32_t uColor);
void ControlBarrierLeft(uint32_t I, uint32_t uColor);
void ControlBarrierRight(uint32_t I, uint32_t uColor);
uint32_t HandleBeam(uint32_t I, uint16_t uOwnElement, uint32_t uOwnColor);

#endif // LIGHTBARRIER_H_INCLUDED
