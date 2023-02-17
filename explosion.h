#ifndef EXPLOSION_H_INCLUDED
#define EXPLOSION_H_INCLUDED

#define EMERALD_EXPLOSION_NONE                          0x0000          // bei diesem Element kann keine Explosion stattfinden, z.B. Stahl
#define EMERALD_EXPLOSION_EMPTY                         0x0001          // an dieser Stelle wird eine Explosion durchgeführt und das bestehende Element entfernt
#define EMERALD_EXPLOSION_ELEMENT                       0x0002          // an dieser Stelle entsteht nach der Explosion ein neues Element
#define EMERALD_EXPLOSION_NEWCENTRAL                    0x0003          // an dieser Stelle entsteht eine neue zentrale 3x3-Explosion
#define EMERALD_EXPLOSION_NEWCENTRAL_MEGA               0x0004          // an dieser Stelle entsteht eine neue zentrale Mega-Explosion
#define EMERALD_EXPLOSION_NEWCENTRAL_BEETLE             0x0005          // an dieser Stelle entsteht eine neue zentrale Käferexploasion



void ControlExplosionToElement(uint32_t I);
void ControlCentralExplosion(uint32_t I);                               // eine "normale" 3x3-Explosion steuern
void ControlCentralBeetleExplosion(uint32_t I);                         // Käferexplosion 3x3
uint32_t CheckExplosionElement(uint16_t uElement);
#endif // EXPLOSION_H_INCLUDED
