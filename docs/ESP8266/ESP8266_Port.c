#include "ESP8266_Port.h"

char 										ESP8266ReceiveBuffer[ESP8266_RECEIVE_BUFFER_SIZE];
unsigned short 					ESP8266ReceiveCounter=0;
volatile unsigned short ESP8266_Counter=0;

void ESP8266_UART_Init(void)
{
	General_USART_Init();
}

void ESP8266_Timer_Init(void)
{
	General_Timer_Init();
}

void ESP8266_UART_Putch(unsigned char c)
{
	//Wait for the uart to finish sending the byte.
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	
	USART_SendData(USART1,c);
}

unsigned char ESP8266_UART_String(char *s, unsigned int Length)
{
	while(Length)
	{
		ESP8266_UART_Putch(*s++);
		Length--;
	}

	return TRUE;
}

/**************************Interrupts For ESP8266********************************/
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)
	{
		ESP8266ReceiveBuffer[ESP8266ReceiveCounter]   =USART_ReceiveData(USART1);
    ESP8266ReceiveCounter++;

    if(ESP8266ReceiveCounter>ESP8266_RECEIVE_BUFFER_SIZE)
        ESP8266ReceiveCounter=0;
	}
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		
		if(ESP8266_Counter>0) 	ESP8266_Counter--;
		if(GetWeatherCounter>0)	GetWeatherCounter--;
	}
}
/******************************************************************************/
