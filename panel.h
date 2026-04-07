#ifndef PANEL_H_INCLUDED
#define PANEL_H_INCLUDED

#define WARN_LOW_TIME           20      // Unter 20 Sekunden ertönt Warngong und roter Balken

void CheckPlayTime(void);
int32_t ShowPanel(void);
int32_t WritePanelText(const char *szText, int32_t nXpos, int32_t nYpos, int32_t nFontSize, uint32_t uFont);
int32_t WritePanelDynamitHammerKeys(void);
int32_t WriteShieldValue(void);
int32_t ConfirmMessage(void);

#endif // PANEL_H_INCLUDED
