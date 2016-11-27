#ifndef __NETWORK__
#define __NETWORK__

#include <Windows.h>

BOOL NetworkProc(WPARAM wParam, LPARAM lParam);
BOOL RecvEvent(void);
BOOL SendEvent(void);
void PacketProc(BYTE packetType, char *packet);
void PacketProc_CreateMyCharacter(char *packet);


#endif
