#include "config.h"
#include "loadlevel.h"
#include "mySDL.h"

CONFIG Config;
extern ACTUALPLAYER Actualplayer;
extern uint32_t ge_uXoffs;             // X-Offset für die Zentrierung von Elementen
extern uint32_t ge_uYoffs;             // X-Offset für die Zentrierung von Elementen

/*----------------------------------------------------------------------------
Name:           ShowConfigFile
------------------------------------------------------------------------------
Beschreibung: Zeigt die Konfigurations-Struktur

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  0 = Alles OK, sonst Fehler
Seiteneffekte: Config.x
------------------------------------------------------------------------------*/
void ShowConfigFile(void) {
    char szMd5String[32 + 1];
    char szString[64];

    GetMd5String(Config.uLevelgroupMd5Hash,szMd5String);
    SDL_Log("============ Config.xml =============");
    SDL_Log("Start dynamite with space:      %d",Config.bStartDynamiteWithSpace);
    SDL_Log("Full screen mode:               %d",Config.bFullScreen);
    SDL_Log("Editor zoom:                    %d",Config.bEditorZoom);
    SDL_Log("Game music:                     %d",Config.bGameMusic);
    SDL_Log("Show highscores:                %d",Config.bShowHighscores);
    SDL_Log("Display:                        %u",Config.uDisplay);
    SDL_Log("X-Resolution:                   %u",Config.uResX);
    SDL_Log("Y-Resolution:                   %u",Config.uResY);
    SDL_Log("Last levelgroup hash:           %s",szMd5String);
    SDL_Log("Last player:                    %s",Config.szPlayername);
    switch (Config.uInputdevice) {
        case (0):
            strcpy(szString,"none, only keyboard");
            break;
        case (1):
            strcpy(szString,"joystick");
            break;
        case (2):
            strcpy(szString,"game controller");
            break;
        default:
            strcpy(szString,"unknown");
            break;
    }
    SDL_Log("Input device:                   %d (%s)",Config.uInputdevice,szString);
    SDL_Log("Joystick/Controller Index:      %d",Config.uDeviceIndex);
    switch (Config.uControllerDirections) {
        case (0):
            strcpy(szString,"digital pad");
            break;
        case (1):
            strcpy(szString,"left axis");
            break;
        case (2):
            strcpy(szString,"right axis");
            break;
        default:
            strcpy(szString,"unknown");
            break;
    }
    SDL_Log("Gamecontroller Directions:      %d (%s)",Config.uControllerDirections,szString);
    SDL_Log("Gamecontroller Firebutton:      %c",Config.cControllerFireButton);
    SDL_Log("Gamecontroller Dynamitebutton:  %c",Config.cControllerStartDynamiteButton);
    SDL_Log("Gamecontroller Exitbutton:      %c",Config.cControllerExitButton);
    SDL_Log("Gamecontroller left threshold:  %d",Config.nAxisLeftThreshold);
    SDL_Log("Gamecontroller right threshold: %d",Config.nAxisRightThreshold);
    SDL_Log("Gamecontroller up threshold:    %d",Config.nAxisUpThreshold);
    SDL_Log("Gamecontroller down threshold:  %d",Config.nAxisDownThreshold);
    SDL_Log("Joystick Firebutton:            %c",Config.cJoystickFireButton);
    SDL_Log("Joystick Dynamitebutton:        %c",Config.cJoystickStartDynamiteButton);
    SDL_Log("Joystick Exitbutton:            %c",Config.cJoystickExitButton);
}


/*----------------------------------------------------------------------------
Name:           WriteDefaultConfigFile
------------------------------------------------------------------------------
Beschreibung: Schreibt eine Werks-Konfiguration.

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  0 = Alles OK, sonst Fehler
Seiteneffekte: Config.x
------------------------------------------------------------------------------*/
int WriteDefaultConfigFile(void) {
    SDL_Log("Writing default config file ...");
    memset(&Config,0,sizeof(Config));               // löscht auch letzten Spieler
    Config.bStartDynamiteWithSpace = true;          // true = Dynamite wird mit Space gezündet
    Config.bFullScreen = false;                     // Spiel läuft als Fullscreen
    Config.bEditorZoom = false;                     // Editor läuft mit 16x16 Pixeln
    Config.bGameMusic = true;                       // true = Modfiles in den Gamemenüs abspielen
    Config.bShowHighscores = true;                  // true = Highscores zeigen, wenn genug Punkte
    Config.uDisplay = 0;                            // Ersten Montitor ansprechen
    Config.uResX = DEFAULT_WINDOW_W;                // Standard-Auflösung X bzw. Fensterbreite
    Config.uResY = DEFAULT_WINDOW_H;                // Auflösung Y bzw. Fensterhöhe
    memset(Config.uLevelgroupMd5Hash,0,16);         // MD5-Hash auf "00000000000000000000000000000000" setzen
    Config.uInputdevice = 0;                        // 0 = kein Input device (Keyboard), 1 = Joystick, 2 = Gamecontroller
    Config.uDeviceIndex = 0;                        // Standard ist 0, d.h. das zuerst Gefundene
    Config.uControllerDirections = 0;               // 0 = Digital Pad, 1 = Left Axis, 2 = Right Axis
    Config.cControllerFireButton = 'B';             // 'A', 'B', 'X' oder 'Y'
    Config.cControllerStartDynamiteButton = 'Y';    // 'A', 'B', 'X' oder 'Y'
    Config.cControllerExitButton = 'X';             // 'A', 'B', 'X', 'Y' oder 'N' = NONE
    Config.nAxisLeftThreshold = -30000;             // -32768 bis 0
    Config.nAxisRightThreshold = 30000;             // 0 bis 32767
    Config.nAxisUpThreshold = -30000;               // -32768 bis 0
    Config.nAxisDownThreshold = 30000;              // 0 bis 32767
    Config.cJoystickFireButton  = 'A';              // 'A', 'B', 'X' oder 'Y'               'A' = linker runder Nutton
    Config.cJoystickStartDynamiteButton = 'X';      // 'A', 'B', 'X' oder 'Y'               'X' = linker dreieckiger Button
    Config.cJoystickExitButton = 'Y';               // 'A', 'B', 'X', 'Y' oder 'N' = NONE   'Y' = rechter dreieckiger Button
    return WriteConfigFile();
}


/*----------------------------------------------------------------------------
Name:           WriteConfigFile
------------------------------------------------------------------------------
Beschreibung: Schreibt den Inhalt der Struktur Config.x als XML-File.

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  0 = Alles OK, sonst Fehler
Seiteneffekte: Config.x
------------------------------------------------------------------------------*/
int WriteConfigFile(void) {
    char szXML[16 * 1024];
    char szMd5String[32 + 1];
    char szString[256];

    SDL_Log("%s: T: %u",__FUNCTION__,SDL_GetTicks());
    memset(szXML,0,sizeof(szXML));
    strcpy(szXML,"<?xml version=\"1.0\"?>\n");
    strcat(szXML,"<configuration>\n");
    strcat(szXML,"<screen>\n");
    strcat(szXML,"  <!--primary display is num 0, second display 1-->\n");
    sprintf(szString,"  <display_num>%u</display_num>\n",Config.uDisplay);
    strcat(szXML,szString);
    strcat(szXML,"  <resolution>\n");
    sprintf(szString,"    <x>%d</x>\n",Config.uResX);
    strcat(szXML,szString);
    sprintf(szString,"    <y>%d</y>\n",Config.uResY);
    strcat(szXML,szString);
    strcat(szXML,"  </resolution>\n");
    strcat(szXML,"  <fullscreen>");
    if (Config.bFullScreen) {
        strcat(szXML,"1");
    } else {
        strcat(szXML,"0");
    }
    strcat(szXML,"</fullscreen>\n");
    strcat(szXML,"</screen>\n");
    strcat(szXML,"<start_dynamite_with_space>");
    if (Config.bStartDynamiteWithSpace) {
        strcat(szXML,"1");
    } else {
        strcat(szXML,"0");
    }
    strcat(szXML,"</start_dynamite_with_space>\n");
    strcat(szXML,"<last_played_levelgroup_md5_hash>");
    GetMd5String(Config.uLevelgroupMd5Hash,szMd5String);
    strcat(szXML,szMd5String);
    strcat(szXML,"</last_played_levelgroup_md5_hash>\n");
    strcat(szXML,"<last_player_name>");
    strcat(szXML,Config.szPlayername);
    strcat(szXML,"</last_player_name>\n");
    strcat(szXML,"<editor_zoom>");
    if (Config.bEditorZoom) {
        strcat(szXML,"1");
    } else {
        strcat(szXML,"0");
    }
    strcat(szXML,"</editor_zoom>\n");
    strcat(szXML,"<game_music>");
    if (Config.bGameMusic) {
        strcat(szXML,"1");
    } else {
        strcat(szXML,"0");
    }
    strcat(szXML,"</game_music>\n");
    strcat(szXML,"<show_highscores>");
    if (Config.bShowHighscores) {
        strcat(szXML,"1");
    } else {
        strcat(szXML,"0");
    }
    strcat(szXML,"</show_highscores>\n");
    strcat(szXML,"<input_device>\n");
    strcat(szXML,"  <!--device_name can be NONE, JOYSTICK or CONTROLLER-->\n");
    strcat(szXML,"  <device_name>");
    switch (Config.uInputdevice) {
        case (0):
            strcat(szXML,"NONE");
            break;
        case (1):
            strcat(szXML,"JOYSTICK");
            break;
        case (2):
            strcat(szXML,"CONTROLLER");
            break;
        default:
            strcat(szXML,"UNKNOWN"); // Sollte nie auftreten
            break;
    }
    strcat(szXML,"</device_name>\n");
    sprintf(szString,"  <device_index>%u</device_index>\n",Config.uDeviceIndex);
    strcat(szXML,szString);
    strcat(szXML,"  <controller_settings>\n");
    strcat(szXML,"    <!--button_fire can be X, Y, A, or B-->\n");
    sprintf(szString,"    <button_fire>%c</button_fire>\n",Config.cControllerFireButton);
    strcat(szXML,szString);
    strcat(szXML,"    <!--button_start_dynamite can be X, Y, A, or B-->\n");
    sprintf(szString,"    <button_start_dynamite>%c</button_start_dynamite>\n",Config.cControllerStartDynamiteButton);
    strcat(szXML,szString);
    strcat(szXML,"    <!--button_exit can be X, Y, A, B or N for NONE-->\n");
    sprintf(szString,"    <button_exit>%c</button_exit>\n",Config.cControllerExitButton);
    strcat(szXML,szString);
    strcat(szXML,"    <!--directions can be DIGITAL, LEFTAXIS or RIGHTAXIS-->\n");
    strcat(szXML,"    <directions>");
    switch (Config.uControllerDirections) {
        case (0):
            strcat(szXML,"DIGITAL");
            break;
        case (1):
            strcat(szXML,"LEFTAXIS");
            break;
        case (2):
            strcat(szXML,"RIGHTAXIS");
            break;
        default:
            strcat(szXML,"UNKNOWN"); // Sollte nie auftreten
            break;
    }
    strcat(szXML,"</directions>\n");
    strcat(szXML,"    <!--axis_left_threshold can range from -32768 to 0-->\n");
    sprintf(szString,"    <axis_left_threshold>%d</axis_left_threshold>\n",Config.nAxisLeftThreshold);
    strcat(szXML,szString);
    strcat(szXML,"    <!--axis_right_threshold can range from 0 to 32767-->\n");
    sprintf(szString,"    <axis_right_threshold>%d</axis_right_threshold>\n",Config.nAxisRightThreshold);
    strcat(szXML,szString);
    strcat(szXML,"    <!--axis_up_threshold can range from -32768 to 0-->\n");
    sprintf(szString,"    <axis_up_threshold>%d</axis_up_threshold>\n",Config.nAxisUpThreshold);
    strcat(szXML,szString);
    strcat(szXML,"    <!--axis_down_threshold can range from 0 to 32767-->\n");
    sprintf(szString,"    <axis_down_threshold>%d</axis_down_threshold>\n",Config.nAxisDownThreshold);
    strcat(szXML,szString);
    strcat(szXML,"  </controller_settings>\n");
    strcat(szXML,"  <joystick_settings>\n");
    strcat(szXML,"    <!--button_fire can be X, Y, A, or B-->\n");
    sprintf(szString,"    <button_fire>%c</button_fire>\n",Config.cJoystickFireButton);
    strcat(szXML,szString);
    strcat(szXML,"    <!--button_start_dynamite can be X, Y, A, or B-->\n");
    sprintf(szString,"    <button_start_dynamite>%c</button_start_dynamite>\n",Config.cJoystickStartDynamiteButton);
    strcat(szXML,szString);
    strcat(szXML,"    <!--button_exit can be X, Y, A, B or N for NONE-->\n");
    sprintf(szString,"    <button_exit>%c</button_exit>\n",Config.cJoystickExitButton);
    strcat(szXML,szString);
    strcat(szXML,"  </joystick_settings>\n");
    strcat(szXML,"</input_device>\n");
    strcat(szXML,"</configuration>\n");
    return WriteFile(EMERALD_CONFIG_FILENAME,(uint8_t *)szXML,(uint32_t)strlen(szXML),false);
}


/*----------------------------------------------------------------------------
Name:           ReadConfigFile
------------------------------------------------------------------------------
Beschreibung: Liest die Konfigurationsdatei und befüllt die Struktur Config.x

Parameter
      Eingang: -
      Ausgang: -
Rückgabewert:  0 = Alles OK, sonst Fehler
Seiteneffekte: Config.x, Actualplayer.x, ge_uXoffs, ge_uYoffs
------------------------------------------------------------------------------*/
int ReadConfigFile(void) {
    ezxml_t xml = NULL;
    ezxml_t screen,fullscreen,resolution,display,x,y,dynamite,gamemusic,highscores,levelgrouphash,playername,editorzoom;
    ezxml_t inputdevice,devicename,deviceindex,controllersettings,joysticksettings,button,directions,threshold;
    int nErrorCode;
    uint8_t *pXml;
    uint32_t uXmlLen;
    char szErrorMessage[256];

    nErrorCode = -1;
    memset(&Config,0,sizeof(Config));       // löscht auch letzten Spieler
    memset(&Actualplayer,0,sizeof(Actualplayer));
    pXml = ReadFile(EMERALD_CONFIG_FILENAME,&uXmlLen);     // Levelgruppen-Datei einlesen
    if (pXml != NULL) {
        // Prüfen, ob Schreibrechte bestehen und versuchen gelesene Datei zurück zu schreiben
        if (WriteFile(EMERALD_CONFIG_FILENAME,pXml,uXmlLen,false) == 0) {
            if ((strstr((char*)pXml,"<configuration>") != NULL) && (strstr((char*)pXml,"</configuration>") != NULL)) {  // configuration tags gefunden?
                xml = ezxml_parse_str((char*)pXml,strlen((char*)pXml));
                if (xml != NULL) {
                    screen = ezxml_child(xml,"screen");
                    if (screen != NULL) {
                        fullscreen = ezxml_child(screen,"fullscreen");
                        if (fullscreen != NULL) {
                            Config.bFullScreen = (strtol(fullscreen->txt,NULL,10) == 1);
                            resolution = ezxml_child(screen,"resolution");
                            if (resolution != NULL) {
                                x = ezxml_child(resolution,"x");
                                if (x != NULL) {
                                    Config.uResX = strtol(x->txt,NULL,10);
                                    y = ezxml_child(resolution,"y");
                                    if (y != NULL) {
                                        Config.uResY = strtol(y->txt,NULL,10);
                                        display = ezxml_child(screen,"display_num");
                                        if (display != NULL) {
                                            Config.uDisplay = strtol(display->txt,NULL,10);
                                            if (Config.uDisplay > 1) {
                                                Config.uDisplay = 1;
                                            }
                                            Config.uDisplayUse = Config.uDisplay; // Wird später ggf. noch angepasst
                                            highscores = ezxml_child(xml,"show_highscores");
                                            if (highscores != NULL) {
                                                Config.bShowHighscores = (strtol(highscores->txt,NULL,10) == 1);
                                                gamemusic = ezxml_child(xml,"game_music");
                                                if (gamemusic != NULL) {
                                                    Config.bGameMusic = (strtol(gamemusic->txt,NULL,10) == 1);
                                                    dynamite = ezxml_child(xml,"start_dynamite_with_space");
                                                    if (dynamite != NULL) {
                                                        Config.bStartDynamiteWithSpace = (strtol(dynamite->txt,NULL,10) == 1);
                                                        editorzoom = ezxml_child(xml,"editor_zoom");
                                                        if (editorzoom != NULL) {
                                                            Config.bEditorZoom = (strtol(editorzoom->txt,NULL,10) == 1);
                                                            levelgrouphash = ezxml_child(xml,"last_played_levelgroup_md5_hash");
                                                            if (levelgrouphash != NULL) {
                                                                GetMd5HashFromString(levelgrouphash->txt,Config.uLevelgroupMd5Hash);
                                                                playername = levelgrouphash = ezxml_child(xml,"last_player_name");
                                                                if (playername != NULL) {
                                                                    if (strlen(playername->txt) <= EMERALD_PLAYERNAME_LEN) {
                                                                        strcpy(Config.szPlayername,playername->txt);
                                                                    }
                                                                    // Auflösung checken
                                                                    if ((Config.uResX >= DEFAULT_WINDOW_W) && (Config.uResY >= DEFAULT_WINDOW_H)) {
                                                                        // ggf. die abgerundeten Werte in die Konfiguration übernehmen
                                                                        ge_uXoffs = (Config.uResX - DEFAULT_WINDOW_W) / 2;
                                                                        ge_uYoffs = (Config.uResY - DEFAULT_WINDOW_H) / 2;
                                                                        inputdevice = ezxml_child(xml,"input_device");
                                                                        if (inputdevice != NULL) {
                                                                            devicename = ezxml_child(inputdevice,"device_name");
                                                                            if (devicename != NULL) {
                                                                                if (strcmp(devicename->txt,"JOYSTICK") == 0) {
                                                                                    Config.uInputdevice = 1;
                                                                                } else if (strcmp(devicename->txt,"CONTROLLER") == 0) {
                                                                                    Config.uInputdevice = 2;
                                                                                } else {
                                                                                    Config.uInputdevice = 0; // NONE oder UNKNOWN
                                                                                }
                                                                                deviceindex = ezxml_child(inputdevice,"device_index");
                                                                                if (deviceindex != NULL) {
                                                                                    Config.uDeviceIndex = strtol(deviceindex->txt,NULL,10);
                                                                                    controllersettings = ezxml_child(inputdevice,"controller_settings");
                                                                                    if (controllersettings != NULL) {
                                                                                        button = ezxml_child(controllersettings,"button_fire");
                                                                                        if (button != NULL) {   // Fire-Button
                                                                                            Config.cControllerFireButton = button->txt[0];
                                                                                            button = ezxml_child(controllersettings,"button_start_dynamite");
                                                                                            if (button != NULL) {
                                                                                                Config.cControllerStartDynamiteButton = button->txt[0];
                                                                                                button = ezxml_child(controllersettings,"button_exit");
                                                                                                if (button != NULL) {
                                                                                                    Config.cControllerExitButton = button->txt[0];
                                                                                                    directions = ezxml_child(controllersettings,"directions");
                                                                                                    if (directions != NULL) {
                                                                                                        if (strcmp(directions->txt,"LEFTAXIS") == 0) {
                                                                                                            Config.uControllerDirections = 1;
                                                                                                        } else if (strcmp(directions->txt,"RIGHTAXIS") == 0) {
                                                                                                            Config.uControllerDirections = 2;
                                                                                                        } else {
                                                                                                            Config.uControllerDirections = 0; // DIGITAL oder UNKNOWN
                                                                                                        }
                                                                                                        threshold = ezxml_child(controllersettings,"axis_left_threshold");
                                                                                                        if (threshold != NULL) {
                                                                                                            Config.nAxisLeftThreshold = strtol(threshold->txt,NULL,10);
                                                                                                            threshold = ezxml_child(controllersettings,"axis_right_threshold");
                                                                                                            if (threshold != NULL) {
                                                                                                                Config.nAxisRightThreshold = strtol(threshold->txt,NULL,10);
                                                                                                                threshold = ezxml_child(controllersettings,"axis_up_threshold");
                                                                                                                if (threshold != NULL) {
                                                                                                                    Config.nAxisUpThreshold = strtol(threshold->txt,NULL,10);
                                                                                                                    threshold = ezxml_child(controllersettings,"axis_down_threshold");
                                                                                                                    if (threshold != NULL) {
                                                                                                                        Config.nAxisDownThreshold = strtol(threshold->txt,NULL,10);
                                                                                                                        joysticksettings = ezxml_child(inputdevice,"joystick_settings");
                                                                                                                        if (joysticksettings != NULL) {
                                                                                                                            button = ezxml_child(joysticksettings,"button_fire");
                                                                                                                            if (button != NULL) {   // Fire-Button
                                                                                                                                Config.cJoystickFireButton = button->txt[0];
                                                                                                                                button = ezxml_child(joysticksettings,"button_start_dynamite");
                                                                                                                                if (button != NULL) {
                                                                                                                                    Config.cJoystickStartDynamiteButton = button->txt[0];
                                                                                                                                    button = ezxml_child(joysticksettings,"button_exit");
                                                                                                                                    if (button != NULL) {
                                                                                                                                        Config.cJoystickExitButton = button->txt[0];
                                                                                                                                        nErrorCode = 0;
                                                                                                                                    }
                                                                                                                                }
                                                                                                                            }
                                                                                                                        }
                                                                                                                    }
                                                                                                                }
                                                                                                            }
                                                                                                        }
                                                                                                    }
                                                                                                }
                                                                                            }
                                                                                        }
                                                                                    }
                                                                                }
                                                                            }
                                                                        }
                                                                    } else {
                                                                        sprintf(szErrorMessage,"%s:\nbad resolution X(%u)/Y(%u), minimum required: X(%u)/Y(%u)\nPlease adjust your config.xml",__FUNCTION__,Config.uResX,Config.uResY,DEFAULT_WINDOW_W,DEFAULT_WINDOW_H);
                                                                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Resolution problem",szErrorMessage,NULL);
                                                                        nErrorCode = -2;    // Programmende
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Write problem","Can not write config.xml file!\nPlease check write permissions.",NULL);
            nErrorCode = -2;    // Programmende
        }
    }
    SAFE_FREE(xml);
    SAFE_FREE(pXml);
    if (nErrorCode == 0) {
        nErrorCode = WriteConfigFile(); // ggf. angepasste Auflösung schreiben
    } else if (nErrorCode == -1) {
        nErrorCode = WriteDefaultConfigFile();
    }
    return nErrorCode;
}
