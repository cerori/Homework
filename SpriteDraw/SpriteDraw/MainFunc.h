#ifndef __MAIN_FUNCTION__
#define __MAIN_FUNCTION__

#include "BaseObject.h"

void InitialGame(void); 
void Update(void);
void ContentLoad(void);
void Draw(void);
void KeyProcess(void);
void Action(void);
bool compare_axle_y(BaseObject *first, BaseObject *second);

// ERROR
void err_quit(WCHAR *msg);
void err_display(WCHAR *msg);

// SOCKET
void InitWSA(void);

#endif;