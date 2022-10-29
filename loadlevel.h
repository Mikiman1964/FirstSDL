#ifndef LOADLEVEL_H_INCLUDED
#define LOADLEVEL_H_INCLUDED

void PrintPlayfieldValues();
int InitialisePlayfield(uint8_t *puLevel);
int GetManCoordinates(void);
int CheckLevelBorder(void);

#endif // LOADLEVEL_H_INCLUDED
