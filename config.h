#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include "loadlevel.h"


typedef struct {
    bool        bStartDynamiteWithSpace;                                        // true = Dynamite wird mit Space gezündet
    bool        bFullScreen;                                                    // true = Spiel läuft als Fullscreen
    bool        bEditorZoom;                                                    // true = 32x32 Pixel, sonst 16x16 Pixel
    bool        bGameMusic;                                                     // true = Modfiles in den Gamemenüs abspielen
    bool        bShowHighscores;                                                // true = Highscores zeigen, wenn genug Punkte
    uint32_t    uResX;                                                          // Auflösung X bzw. Fensterbreite
    uint32_t    uResY;                                                          // Auflösung Y bzw. Fensterhöhe
    uint8_t     uLevelgroupMd5Hash[16];                                         // zuletzt gewählte Levelgruppe als MD5 Hash
    char        szPlayername[EMERALD_PLAYERNAME_LEN + 1];                       // letzter Spieler
    uint32_t    uInputdevice;                                                   // 0 = kein Input device (Keyboard), 1 = Joystick, 2 = Gamecontroller
    uint32_t    uDeviceIndex;                                                   // Standard ist 0, d.h. das zuerst Gefundene
    uint32_t    uControllerDirections;                                          // 0 = Digital Pad, 1 = Left Axis, 2 = Right Axis
    char        cControllerFireButton;                                          // 'A', 'B', 'X' oder 'Y'
    char        cControllerStartDynamiteButton;                                 // 'A', 'B', 'X' oder 'Y'
    char        cControllerExitButton;                                          // 'A', 'B', 'X' oder 'Y'
    int16_t     nAxisLeftThreshold;                                             // 0 bis 32767
    int16_t     nAxisRightThreshold;                                            // 0 bis 32767
    int16_t     nAxisUpThreshold;                                               // 0 bis 32767
    int16_t     nAxisDownThreshold;                                             // 0 bis 32767
    char        cJoystickFireButton;                                            // 'A', 'B', 'X' oder 'Y'
    char        cJoystickStartDynamiteButton;                                   // 'A', 'B', 'X' oder 'Y'
    char        cJoystickExitButton;                                            // 'A', 'B', 'X' oder 'Y'
    uint32_t    uDisplay;                                                       // Ab 0 gezählt (eingestellt)
    uint32_t    uDisplayUse;                                                    // Ab 0 gezählt (Dieses wird vervendet)
} CONFIG;


void ShowConfigFile(void);
int WriteDefaultConfigFile(void);
int WriteConfigFile(void);
int ReadConfigFile(void);

#endif // CONFIG_H_INCLUDED
