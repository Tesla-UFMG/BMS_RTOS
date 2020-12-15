#include "stm32f1xx_hal.h"
#include "string.h"
#include "stdio.h"

void sendCommand(const char*);
int recvRetCommandFinished(void);
int recvRetNumber(void);
int recvRetString(char*, int);
int nexInit(void);
int NexPageShow(int);
int NexTextSetText(int, const char*);
int NexTextGetText(int, char*, int);
int NexNumberSetValue(int, int);
int NexNumberGetValue(int);
int NexXfloatSetCollor(int, int);
int NexXfloatSetValue(int, int);
int NexWaveformAdd(int, int, int);
int NexButtonGetText(int, char*, int);
int NexButtonSetText(int, const char*);
int NexCheckboxGetValue(int);
int NexCheckboxSetValue(int, int);
int NexDSButtonGetValue(int);
int NexDSButtonSetValue(int, int);
int NexDSButtonGetText(int, char*, int);
int NexDSButtonSetText(int, const char*);
int NexGaugeGetValue(int);
int NexGaugeSetValue(int, int);
int NexProgressBarGetValue(int);
int NexProgressBarSetValue(int, int);
int NexRadioGetValue(int);
int NexRadioSetValue(int, int);
int NexVariableGetValue(int);
int NexVariableSetValue(int, int);
int NexVariableGetText(int, char*, uint32_t);
int NexVariableSetText(int, const char*);
int NexPictureSetPic(int, int);
int NexScrollingTextSetPic(int, int);
int NexScrollingTextSetText(int, const char*);
