#ifndef EXPLOSION_H_INCLUDED
#define EXPLOSION_H_INCLUDED

#define EMERALD_EXPLOSION_NONE                          0x0000          // bei diesem Element kann keine Explosion stattfinden, z.B. Stahl
#define EMERALD_EXPLOSION_EMPTY                         0x0001          // an dieser Stelle wird eine Explosion durchgeführt und das bestehende Element entfernt
#define EMERALD_EXPLOSION_EMPTY_MAN                     0x0002          // an dieser Stelle wird eine Explosion durchgeführt und der Man entfernt/geklillt
#define EMERALD_EXPLOSION_ELEMENT                       0x0003          // an dieser Stelle entsteht nach der Explosion ein neues Element
#define EMERALD_EXPLOSION_NEWCENTRAL                    0x0004          // an dieser Stelle entsteht eine neue zentrale 3x3-Explosion
#define EMERALD_EXPLOSION_NEWCENTRAL_MEGA               0x0005          // an dieser Stelle entsteht eine neue zentrale Mega-Explosion
#define EMERALD_EXPLOSION_NEWCENTRAL_BEETLE             0x0006          // an dieser Stelle entsteht eine neue zentrale Käferexploasion


#define NO_MEGA_EXPLOSION                               false
#define MEGA_EXPLOSION                                  true


void ControlExplosionToElement(uint32_t I);
void ControlCentralExplosion(uint32_t I);                               // eine "normale" 3x3-Explosion steuern
void ControlCentralMegaExplosion(int nI);                               // eine Mega-Explosion steuern
void ControlCentralBeetleExplosion(uint32_t I);                         // Käferexplosion 3x3
void ControlCentralYamExplosion(uint32_t I);                            // Yamexplosion 3x3
uint32_t CheckExplosionElement(uint16_t uElement,uint32_t uCoordinate);
int YamHasReplicatorAcidPool(uint16_t *YamExplosion);
void CheckYamContents(uint32_t I,uint16_t *YamElements);
bool IsDangerousEnemyAround(uint32_t I);
void CleanInvalidFieldsForCentralExplosion(int I, bool bMega);

#endif // EXPLOSION_H_INCLUDED


