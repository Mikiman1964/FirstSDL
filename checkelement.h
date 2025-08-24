#ifndef CHECKELEMENT_H_INCLUDED
#define CHECKELEMENT_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>

#define         REPLICATOR_UNKNOWN              0
#define         REPLICATOR_TOP_LEFT             1
#define         REPLICATOR_TOP_MID              2
#define         REPLICATOR_TOP_RIGHT            3
#define         REPLICATOR_BOTTOM_LEFT          4
#define         REPLICATOR_BOTTOM_RIGHT         5


bool IsElementAlive(uint16_t uElement);
bool IsSteel(uint16_t uElement);
bool IsPipeElement(uint16_t uElement);
bool IsAcidPoolOrReplicatorElement(uint16_t uElement);
bool IsReplicatorElement(uint16_t uElement);
uint32_t GetReplicatorPosition(uint16_t uElement);
bool IsReplicatorElementRed(uint16_t uElement);
bool IsReplicatorElementYellow(uint16_t uElement);
bool IsReplicatorElementGreen(uint16_t uElement);
bool IsReplicatorElementBlue(uint16_t uElement);
bool IsAcidPoolElement(uint16_t uElement);
bool IsContactMineExplode(uint16_t uElement);
#endif // CHECKELEMENT_H_INCLUDED
