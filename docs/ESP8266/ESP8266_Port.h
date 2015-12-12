#ifndef ESP8266_PORT_H
#define	ESP8266_PORT_H

#include "ESP8266.h"

extern char 											ESP8266ReceiveBuffer[ESP8266_RECEIVE_BUFFER_SIZE];
extern unsigned short 						ESP8266ReceiveCounter;
extern volatile unsigned short 		ESP8266_Counter;

extern void ESP8266_UART_Init(void);
extern void ESP8266_Timer_Init(void);
extern unsigned char ESP8266_UART_String(char *s, unsigned int Length);

#endif
