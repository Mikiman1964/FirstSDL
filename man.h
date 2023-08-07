#ifndef MAN_H_INCLUDED
#define MAN_H_INCLUDED


#define     MANKEY_NONE                    0x00000000
#define     MANKEY_RIGHT                   0x00000001
#define     MANKEY_LEFT                    0x00000002
#define     MANKEY_UP                      0x00000003
#define     MANKEY_DOWN                    0x00000004


typedef struct {
    uint32_t uDirection;
    uint32_t uLastDirection;                // letzte Richtung
    bool bFire;
    bool bExit;                             // nur über GameController
    uint32_t uFireCount;                    // Für Zündung von Dynamit.
    uint32_t uLastActiveDirection;          // letzte aktive Richtung, die bestand
    uint32_t uLastDirectionFrameCount;      // Frame, an dem zuletzt eine neue Richtungsauslösung erfolgte
    uint32_t uLastNoDirectionFrameCount;    // Frame, an dem zuletzt eine Richtungsauslösung beendet wurde
    uint32_t uLastFireFrameCount;           // Frame, an dem zuletzt eine Firebutton-Auslösung erkannt wurde
    uint32_t uLastSwitchFrameCount;         // Letzte-Richtung-Frame, an dem Man einen Schalter umgelegt hat (Verhindert doppeltes Umschalten, wenn Schalter länger durchgehend gedrückt wird)
} MANKEY;

// Prototypen zur Steuerung des Mans und Rads
void ControlManDies(uint32_t I);
uint32_t ControlMan(uint32_t I, uint32_t uDirection);
void UpdateManKey(void);
uint32_t ManTouchElement(uint32_t uActPos, uint32_t uTouchPos, uint32_t uAnimation);
void ControlWheels(uint32_t I);
void CheckRunningWheel(void);
void ControlEnddoorReady(uint32_t I);
void ControlEnddoorReadySteel(uint32_t I);
void ManGoUp(uint32_t I, uint32_t uAdditionalAnimStatus, bool bDoubleSpeed);
void ManGoLeft(uint32_t I, uint32_t uAdditionalAnimStatus, bool bDoubleSpeed);
void ManGoDown(uint32_t I,uint32_t uAdditionalAnimStatus, bool bDoubleSpeed);
void ManGoRight(uint32_t,uint32_t uAdditionalAnimStatus, bool bDoubleSpeed);
void SetManArm(uint32_t I,uint32_t uAnimation);
void ManGoTeleporter(uint32_t uActCoordinate, uint32_t uDestTeleporterCoordinate, uint32_t uAnimation);
#endif // MAN_H_INCLUDED
