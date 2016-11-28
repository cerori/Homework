#ifndef __NETWORK__
#define __NETWORK__

#include <Windows.h>

BOOL NetworkProc(WPARAM wParam, LPARAM lParam);
BOOL RecvEvent(void);
BOOL SendEvent(void);
void PacketProc(BYTE packetType, char *packet);
void PacketProc_CreateMyCharacter(char *packet);
void PacketProc_CreateOtherCharacter(char *packet);
void packetProc_DeleteCharacter(char *packet);
void packetProc_MoveStart(char *packet);
void packetProc_MoveStop(char *packet);
void packetProc_Attack1(char *packet);
void packetProc_Attack2(char *packet);
void packetProc_Attack3(char *packet);
void packetProc_Damage(char *packet);

#endif
