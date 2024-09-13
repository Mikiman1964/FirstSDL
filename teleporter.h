#ifndef TELEPORTER_H_INCLUDED
#define TELEPORTER_H_INCLUDED

#define EMERALD_INVALID_TELEPORTER_COORDINATE           0xFFFFFFFF

void PrintTeleporters(void);
void InitTeleporter(void);
int  SearchTeleporter(void);
int  AddTeleporterCoordinate(uint16_t uTeleporterElement,uint32_t uCoordinate);
void FreeTeleporterCoordinates(void);
uint32_t GetDestinationTeleporterCoordinate(uint32_t uSourceCoordinate, uint32_t uAnimation);

#endif // TELEPORTER_H_INCLUDED
