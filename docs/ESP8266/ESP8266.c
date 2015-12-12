#include <string.h>
#include "ESP8266.h"
#include "ESP8266_Port.h"

AT_STATE 					At_State;
WIFI_STATE				Wifi_State;
SSID_STATE				SSID_State;
CON_SSID_STATE		Con_SSID_State;
RESET_STATE				Reset_State;
CONNECTION_STATE	Connection_State;
GET_IP_NUMBER			Get_Ip_State;
OPENWEATHERMAP		OpenWeatherMap_State;
ESP8266_STATE 		ESP8266_State;

unsigned char 		ESP8266_Version[10];
char							ESP8266_CWMODE[10];
char							ESP8266_CIPMODE[10];
char							ESP8266_CIPMUX[10];
char							ESP8266_IP[20];
unsigned short 		ESP266_Dummy_Counter;
struct StcCity		City;
unsigned int			GetWeatherCounter=0;

void ClearBuffer(void)
{
	for(ESP266_Dummy_Counter=0;ESP266_Dummy_Counter<ESP8266_RECEIVE_BUFFER_SIZE;ESP266_Dummy_Counter++)
		ESP8266ReceiveBuffer[ESP266_Dummy_Counter]=0;
	
	ESP8266ReceiveCounter=0;
}

unsigned char ESP8266_AT_Send(unsigned char AT_Send, const char Command[])
{
	unsigned char Return_Result=WAIT;
	
	if(strcmp(Command,"AT")==0)
	{
		switch(At_State)
		{
			case AT_IDLE:
				if(AT_Send==TRUE)
				{
					ESP8266ReceiveCounter	=0;
					At_State							=AT_SEND_DATA;
				}
				else
					At_State=AT_IDLE;
				
				Return_Result=WAIT;
				break;
				
			case AT_SEND_DATA:
				ClearBuffer();
			
				if(ESP8266_UART_String("AT\r\n",4)==TRUE)
				{
					At_State				=AT_ANS_WAIT;
					ESP8266_Counter	=ESP8266_TIMEOUT;
				}
				
				Return_Result=WAIT;
				break;
				
			case AT_ANS_WAIT:
				
				if(ESP8266_Counter==0 || strstr(ESP8266ReceiveBuffer,"ERROR"))
					At_State=AT_ANS_TIMEOUT;
			
				if(strcmp(ESP8266ReceiveBuffer,"AT\r\r\n\r\nOK\r\n")==0)
				{
					At_State=AT_IDLE;
					Return_Result=TRUE;
				}
				
				break;
				
			case AT_ANS_TIMEOUT:
				Return_Result=FALSE;
				At_State=AT_IDLE;
				break;
				
			default:
				At_State=AT_IDLE;
				Return_Result=WAIT;
				break;
		}
	}
	
	if(strcmp(Command,"AT+GMR")==0)
	{
		switch(At_State)
		{
			case AT_IDLE:
				if(AT_Send==TRUE)
				{
					ESP8266ReceiveCounter	=0;
					At_State							=AT_SEND_DATA;
				}
				else
					At_State=AT_IDLE;
				
				Return_Result=WAIT;
				break;
				
			case AT_SEND_DATA:
				ClearBuffer();
			
				if(ESP8266_UART_String("AT+GMR\r\n",8)==TRUE)
				{
					At_State				=AT_ANS_WAIT;
					ESP8266_Counter	=ESP8266_TIMEOUT;
				}
				
				Return_Result=WAIT;
				break;
				
			case AT_ANS_WAIT:
				
				if(ESP8266_Counter==0 || strstr(ESP8266ReceiveBuffer,"ERROR"))
					At_State=AT_ANS_TIMEOUT;
				
				if(strstr(ESP8266ReceiveBuffer,"OK")!=NULL)
				{
					for(ESP266_Dummy_Counter=0;ESP266_Dummy_Counter<10;ESP266_Dummy_Counter++)
						ESP8266_Version[ESP266_Dummy_Counter]=ESP8266ReceiveBuffer[ESP266_Dummy_Counter+9];
					
					At_State	=AT_IDLE;
					Return_Result		=TRUE;
				}
				
				break;
				
			case AT_ANS_TIMEOUT:
				Return_Result		=FALSE;
				At_State	=AT_IDLE;
				
				break;
				
			default:
				At_State	=AT_IDLE;
				Return_Result		=WAIT;
				break;
		}
	}
	
	return Return_Result;
}

unsigned char ESP8266_WifiSetting(void)
{
	unsigned char Return_Result=WAIT;
	
	switch(Wifi_State)
	{
		case WIFI_IDLE:
			Wifi_State				=AT_CWMODE;
			Return_Result			=WAIT;
			break;
		
		case AT_CWMODE:
				ClearBuffer();
			
				if(ESP8266_UART_String("AT+CWMODE=3\r\n",13)==TRUE)
				{
					Wifi_State			=AT_CWMODE_ANS_WAIT;
					ESP8266_Counter	=ESP8266_TIMEOUT;
				}
				
				Return_Result			=WAIT;
				
			break;
		case AT_CWMODE_ANS_WAIT:
			
				if(ESP8266_Counter==0 || strstr(ESP8266ReceiveBuffer,"ERROR"))
				{
					Wifi_State			=WIFI_ANS_TIMEOUT;
					strcpy(ESP8266_CWMODE,"Error!");
				}
				
				if(strstr(ESP8266ReceiveBuffer,"AT+CWMODE=3")!=NULL)
				{				
					Wifi_State				=AT_CIPMODE;
					Return_Result			=WAIT;
					strcpy(ESP8266_CWMODE,"Both  ");
				}
			break;
				
		case AT_CIPMODE:
				ClearBuffer();
			
				if(ESP8266_UART_String("AT+CIPMODE?\r\n",13)==TRUE)
				{
					Wifi_State			=AT_CIPMODE_ANS_WAIT;
					ESP8266_Counter	=ESP8266_TIMEOUT;
				}
				
				Return_Result			=WAIT;
			break;
				
		case AT_CIPMODE_ANS_WAIT:
				if(ESP8266_Counter==0 || strstr(ESP8266ReceiveBuffer,"ERROR"))
				{
					Wifi_State=WIFI_ANS_TIMEOUT;
					strcpy(ESP8266_CIPMODE,"Error!");
				}
				
				if(strstr(ESP8266ReceiveBuffer,"CIPMODE:0")!=NULL)
				{				
					Wifi_State				=AT_CIPMUX;
					Return_Result			=WAIT;
					strcpy(ESP8266_CIPMODE,"Normal");
				}
			break;
				
		case AT_CIPMUX:
				ClearBuffer();
			
				if(ESP8266_UART_String("AT+CIPMUX=1\r\n",13)==TRUE)
				{
					Wifi_State			=AT_CIPMUX_ANS_WAIT;
					ESP8266_Counter	=ESP8266_TIMEOUT;
				}
				
				Return_Result			=WAIT;
			break;
				
		case AT_CIPMUX_ANS_WAIT:
				if(ESP8266_Counter==0 || strstr(ESP8266ReceiveBuffer,"ERROR"))
				{
					Wifi_State=WIFI_ANS_TIMEOUT;
					strcpy(ESP8266_CIPMUX,"Error!  ");
				}
				
				if(strstr(ESP8266ReceiveBuffer,"AT+CIPMUX=1")!=NULL)
				{				
					Wifi_State				=WIFI_IDLE;
					Return_Result			=TRUE;
					strcpy(ESP8266_CIPMUX,"Multiple");
				}
				
			break;
				
		case WIFI_ANS_TIMEOUT:
				Return_Result			=FALSE;
				Wifi_State				=WIFI_IDLE;
			break;
		
		default:
			Wifi_State				=WIFI_IDLE;
			Return_Result			=WAIT;
			break;
	}
	
	return Return_Result;
}

unsigned char Find_SSID(void)
{
	unsigned char Return_Result=WAIT;
	
	switch(SSID_State)
	{
		case SSID_IDLE:
			SSID_State				=AT_CWLAP;
			Return_Result			=WAIT;
			break;
		
		case AT_CWLAP:
			ClearBuffer();
		
			if(ESP8266_UART_String("AT+CWLAP\r\n",10)==TRUE)
			{
				SSID_State			=AT_CWLAP_ANS_WAIT;
				ESP8266_Counter	=ESP8266_TIMEOUT;
			}
			
			Return_Result			=WAIT;
				
			break;
				
		case AT_CWLAP_ANS_WAIT:
			if(ESP8266_Counter==0 || strstr(ESP8266ReceiveBuffer,"ERROR"))
					SSID_State			=SSID_ANS_TIMEOUT;
			
			if(strstr(ESP8266ReceiveBuffer,SSID_NAME)!=NULL)
			{				
				SSID_State				=SSID_IDLE;
				Return_Result			=TRUE;
			}
				
			break;
		case SSID_ANS_TIMEOUT:
			Return_Result	=FALSE;
			SSID_State		=SSID_IDLE;
			break;
		
		default:
			Return_Result	=WAIT;
			SSID_State		=SSID_IDLE;
			break;
	}
	
	return Return_Result;
}

unsigned char Connect_SSID(void)
{
	unsigned char Return_Result=WAIT;
	char 					Dummy_String[50]="";
	
	switch(Con_SSID_State)
	{
		case CON_SSID_IDLE:
			Con_SSID_State		=AT_CWJAP;
			Return_Result			=WAIT;
			break;
		
		case AT_CWJAP:
			ClearBuffer();
		
			strcat(Dummy_String,"AT+CWJAP=\"");
			strcat(Dummy_String,SSID_NAME);
			strcat(Dummy_String,"\"+\"");
			strcat(Dummy_String,SSID_PASSWORD);
			strcat(Dummy_String,"\"\r\n");
		
			if(ESP8266_UART_String(Dummy_String,strlen(Dummy_String))==TRUE)
			{
				Con_SSID_State	=AT_CWJAP_ANS_WAIT;
				ESP8266_Counter	=ESP8266_CWJAP_TIMEOUT;
			}
			
			Return_Result			=WAIT;
				
			break;
				
		case AT_CWJAP_ANS_WAIT:
			if(ESP8266_Counter==0 || strstr(ESP8266ReceiveBuffer,"ERROR"))
				Con_SSID_State	=CON_SSID_ANS_TIMEOUT;
			
			if(strstr(ESP8266ReceiveBuffer,"FAIL"))
				Con_SSID_State	=CON_SSID_FAIL;
			
			if(strstr(ESP8266ReceiveBuffer,"OK")!=NULL)
			{				
				Con_SSID_State		=CON_SSID_IDLE;
				Return_Result			=TRUE;
			}
				
			break;
		case SSID_ANS_TIMEOUT:
			Return_Result	=FALSE;
			Con_SSID_State=CON_SSID_IDLE;
			break;
		
		case CON_SSID_FAIL:
			Return_Result	=RESET_IT;
			Con_SSID_State=CON_SSID_IDLE;
			break;
		
		default:
			Return_Result	=WAIT;
			Con_SSID_State=CON_SSID_IDLE;
			break;
	}
	
	return Return_Result;
}

unsigned char Reset_ESP8266(void)
{
	unsigned char Return_Result=WAIT;
	
	switch(Reset_State)
	{
		case RESET_IDLE:
			Reset_State				=AT_RST;
			Return_Result			=WAIT;
			break;
		
		case AT_RST:
			ClearBuffer();
		
			if(ESP8266_UART_String("AT+RST\r\n",8)==TRUE)
			{
				Reset_State			=AT_RST_ANS_WAIT;
				ESP8266_Counter	=ESP8266_TIMEOUT;
			}
			
			Return_Result			=WAIT;
				
			break;
				
		case AT_RST_ANS_WAIT:
			if(ESP8266_Counter==0 || strstr(ESP8266ReceiveBuffer,"ERROR"))
				Reset_State			=RESET_ANS_TIMEOUT;
			
			if(strstr(ESP8266ReceiveBuffer,"ready")!=NULL)
			{				
				Reset_State				=RESET_IDLE;
				Return_Result			=TRUE;
			}
				
			break;
			
		case RESET_ANS_TIMEOUT:
			Return_Result	=FALSE;
			Reset_State		=RESET_IDLE;
			break;
		
		default:
			Return_Result	=WAIT;
			Reset_State		=RESET_IDLE;
			break;
	}
	
	return Return_Result;
}

unsigned char CheckConnection(void)
{
	unsigned char Return_Result=WAIT;
	
	switch(Connection_State)
	{
		case CONNECTION_IDLE:
			Connection_State	=CHECK_STILL_CONNECTED;
			Return_Result			=WAIT;
			break;
		
		case CHECK_STILL_CONNECTED:
			ClearBuffer();
		
			if(ESP8266_UART_String("AT+CWJAP?\r\n",11)==TRUE)
			{
				Connection_State	=CONNECTION_ANS_WAIT;
				ESP8266_Counter	  =ESP8266_TIMEOUT;
			}
			
			Return_Result			=WAIT;
				
			break;
				
		case CONNECTION_ANS_WAIT:
			if(ESP8266_Counter==0 || strstr(ESP8266ReceiveBuffer,"ERROR"))
				Connection_State			=CONNECTION_TIMEOUT;
			
			if(strstr(ESP8266ReceiveBuffer,SSID_NAME)!=NULL)
			{				
				Connection_State	=CONNECTION_IDLE;
				Return_Result			=TRUE;
			}
				
			break;
			
		case CONNECTION_TIMEOUT:
			Return_Result			=FALSE;
			Connection_State	=CONNECTION_IDLE;
			break;
		
		default:
			Return_Result			=WAIT;
			Connection_State	=CONNECTION_IDLE;
			break;
	}
	
	return Return_Result;
}

unsigned char GetIPNumber(void)
{
	unsigned char Return_Result=WAIT;
	char 					Start_Address;
	char 					Str_Lenght;
	
	switch(Get_Ip_State)
	{
		case AT_CIPSTA_IDLE:
			Get_Ip_State	=AT_CIPSTA;
			Return_Result	=WAIT;
			break;
		
		case AT_CIPSTA:
			ClearBuffer();
		
			if(ESP8266_UART_String("AT+CIPSTA?\r\n",12)==TRUE)
			{
				Get_Ip_State			=AT_CIPSTA_ANS_WAIT;
				ESP8266_Counter	  =ESP8266_TIMEOUT;
			}
			
			Return_Result				=WAIT;
				
			break;
				
		case AT_CIPSTA_ANS_WAIT:
			if(ESP8266_Counter==0 || strstr(ESP8266ReceiveBuffer,"ERROR"))
				Get_Ip_State			=AT_CIPSTA_TIMEOUT;
			
			if(strstr(ESP8266ReceiveBuffer,"+CIPSTA:")!=NULL)
			{
				Start_Address	=strcspn(ESP8266ReceiveBuffer,"192");
				Str_Lenght		=strlen(ESP8266ReceiveBuffer);

				for(ESP266_Dummy_Counter=0;ESP266_Dummy_Counter<(Str_Lenght-Start_Address-9);ESP266_Dummy_Counter++)
					ESP8266_IP[ESP266_Dummy_Counter]=ESP8266ReceiveBuffer[Start_Address+ESP266_Dummy_Counter];
				
				Get_Ip_State	=AT_CIPSTA_IDLE;
				Return_Result	=TRUE;
			}
				
			break;
			
		case AT_CIPSTA_TIMEOUT:
			Return_Result	=FALSE;
			Get_Ip_State	=AT_CIPSTA_IDLE;
			break;
		
		default:
			Return_Result	=WAIT;
			Get_Ip_State	=AT_CIPSTA_IDLE;
			break;
	}
	
	return Return_Result;
}

unsigned char OpenWeatherMap(unsigned char City_Number)
{
	unsigned char Return_Result				=WAIT;
	char At_ChipStart[50]							="AT+CIPSTART=0,\"TCP\",\"api.openweathermap.org\",80\r\n";
	char At_Chipsend[50]							="AT+CIPSEND=0,";
	char Get_Weather_1[60]						="GET /data/2.5/weather?q=";
	char Get_Weather_2[60]						="&units=metric\r\nHTTP/1.0Host: api.openweathermap.org\r\n\r\n\r\n";
	char Get_Weather[120]							="";
	char Get_Weather_String_Number[2];
	char AnswerCompare[50]						="";
	
	if(City_Number==0)
	{
		strcat(Get_Weather,Get_Weather_1);
		strcat(Get_Weather,CITY_NAME_1);
		strcat(Get_Weather,Get_Weather_2);
		
		strcat(AnswerCompare,"\"name\":\"");
		strcat(AnswerCompare,CITY_NAME_1);
		strcat(AnswerCompare,"\"");
	}
	else if(City_Number==1)
	{
		strcat(Get_Weather,Get_Weather_1);
		strcat(Get_Weather,CITY_NAME_2);
		strcat(Get_Weather,Get_Weather_2);
		
		strcat(AnswerCompare,"\"name\":\"");
		strcat(AnswerCompare,CITY_NAME_2);
		strcat(AnswerCompare,"\"");
	}
	else if(City_Number==2)
	{
		strcat(Get_Weather,Get_Weather_1);
		strcat(Get_Weather,CITY_NAME_3);
		strcat(Get_Weather,Get_Weather_2);
		
		strcat(AnswerCompare,"\"name\":\"");
		strcat(AnswerCompare,CITY_NAME_3);
		strcat(AnswerCompare,"\"");
	}
	
	Get_Weather_String_Number[0]=(strlen(Get_Weather)/10)+48;
	Get_Weather_String_Number[1]=(strlen(Get_Weather)%10)+48;
	
	strcat(At_Chipsend,Get_Weather_String_Number);
	strcat(At_Chipsend,"\r\n");
	
	switch(OpenWeatherMap_State)
	{
		case OPENWEATHERMAP_IDLE:
			OpenWeatherMap_State	=AT_CIPSTART;
			Return_Result					=WAIT;
			break;
		
		case AT_CIPSTART:
			ClearBuffer();
		
			if(GetWeatherCounter==0)
			{
				if(ESP8266_UART_String(At_ChipStart,strlen(At_ChipStart))==TRUE)
				{
					OpenWeatherMap_State			=AT_CIPSTART_ANS_WAIT;
					ESP8266_Counter	  				=ESP8266_TIMEOUT;
				}
			}
			
			Return_Result				=WAIT;
			
		case AT_CIPSTART_ANS_WAIT:
				
			if(ESP8266_Counter==0 || strstr(ESP8266ReceiveBuffer,"ERROR"))
				OpenWeatherMap_State			=OPENWEATHERMAP_TIMEOUT;
			
			if(strstr(ESP8266ReceiveBuffer,"CONNECT")!=NULL)
			{
				OpenWeatherMap_State	=AT_CIPSEND;
				Return_Result					=WAIT;
			}
			break;
			
		case AT_CIPSEND:
			ClearBuffer();
		
			if(ESP8266_UART_String(At_Chipsend,strlen(At_Chipsend))==TRUE)
			{
				OpenWeatherMap_State			=AT_CIPSEND_ANS_WAIT;
				ESP8266_Counter	  				=ESP8266_TIMEOUT;
			}
			
			Return_Result				=WAIT;
			break;
			
		case AT_CIPSEND_ANS_WAIT:
			if(ESP8266_Counter==0 || strstr(ESP8266ReceiveBuffer,"ERROR"))
				OpenWeatherMap_State			=OPENWEATHERMAP_TIMEOUT;
			
			if(strstr(ESP8266ReceiveBuffer,">")!=NULL)
			{
				OpenWeatherMap_State	=GET_WEATHER;
				Return_Result					=WAIT;
			}
			break;
			
		case GET_WEATHER:
			ClearBuffer();
		
			if(ESP8266_UART_String(Get_Weather,strlen(Get_Weather))==TRUE)
			{
				OpenWeatherMap_State			=GET_WEATHER_ANS_WAIT;
				ESP8266_Counter	  				=ESP8266_TIMEOUT;
			}
			
			Return_Result				=WAIT;
			
		case GET_WEATHER_ANS_WAIT:
			if(ESP8266_Counter==0 || strstr(ESP8266ReceiveBuffer,"ERROR"))
				OpenWeatherMap_State			=OPENWEATHERMAP_TIMEOUT;
			
			if(strstr(ESP8266ReceiveBuffer,AnswerCompare)!=NULL)
			{
				OpenWeatherMap_State	=WAIT_FOR_CLOSED;
				ESP8266_Counter	  		=ESP8266_TIMEOUT;
				Return_Result					=WAIT;
			}
			break;
			
		case WAIT_FOR_CLOSED:
			if(ESP8266_Counter==0 || strstr(ESP8266ReceiveBuffer,"ERROR"))
				OpenWeatherMap_State			=OPENWEATHERMAP_TIMEOUT;
			
			if(strstr(ESP8266ReceiveBuffer,"CLOSED")!=NULL)
			{
				OpenWeatherMap_State	=OPENWEATHERMAP_IDLE;
				GetWeatherCounter			=ESP8266_GETWEATHER_TIMEOUT;
				Return_Result					=TRUE;
			}
		break;
			
		case OPENWEATHERMAP_TIMEOUT:
			Return_Result					=FALSE;
			OpenWeatherMap_State	=OPENWEATHERMAP_IDLE;
			break;
		
		default:
			Return_Result					=WAIT;
			OpenWeatherMap_State	=OPENWEATHERMAP_IDLE;
			break;
	}
	
	return Return_Result;
}

void FindTemperatureValues(void)
{
	unsigned char  CityDummyCounter;
	unsigned short CityDummyStartAddress;
	
	CityDummyStartAddress=Search_SubString(ESP8266ReceiveBuffer,"pressure");
	
	for(CityDummyCounter=0;CityDummyCounter<4;CityDummyCounter++)
		City.City_Pressure[CityDummyCounter]=ESP8266ReceiveBuffer[CityDummyStartAddress+CityDummyCounter+10];
	
	CityDummyStartAddress=Search_SubString(ESP8266ReceiveBuffer,"\"temp\"");
	
	for(CityDummyCounter=0;CityDummyCounter<2;CityDummyCounter++)
		City.City_Temp[CityDummyCounter]=ESP8266ReceiveBuffer[CityDummyStartAddress+CityDummyCounter+7];
	
	CityDummyStartAddress=Search_SubString(ESP8266ReceiveBuffer,"humidity");
	
	for(CityDummyCounter=0;CityDummyCounter<2;CityDummyCounter++)
		City.City_Humidity[CityDummyCounter]=ESP8266ReceiveBuffer[CityDummyStartAddress+CityDummyCounter+10];
}

unsigned short Search_SubString(char Source[], char Target[]) 
	{
   unsigned short   ReturnValues=NULL;
   unsigned short   Search_i=0,
                    Search_j=0,
                    FirstOcc;

   while (Source[Search_i] != '\0')
    {

      while (Source[Search_i] != Target[0] && Source[Search_i] != '\0')
         Search_i++;

      if (Source[Search_i] == '\0')
      {
         ReturnValues=NULL;
         break;
      }


      FirstOcc = Search_i;

      while (Source[Search_i] == Target[Search_j] && Source[Search_i] != '\0' && Target[Search_j] != '\0')
      {
         Search_i++;
         Search_j++;
      }

      if (Target[Search_j] == '\0')
      {
         ReturnValues=FirstOcc;
         break;
      }
      if (Source[Search_i] == '\0')
      {
         ReturnValues=NULL;
         break;
      }


      Search_i = FirstOcc + 1;
      Search_j = 0;
   }

   return ReturnValues;
}
	
unsigned char State_Number=0;
unsigned char ESP8266_Error_Counter;
	
unsigned char ProcessESP8266(void)
{
	unsigned char ESP8266_Result;
	
	switch(ESP8266_State)
	{
		case ESP8266_IDLE:
			ESP8266_State=ESP8266_SEND_AT;
			break;
		
		case ESP8266_RESET:
			ESP8266_Result=Reset_ESP8266();
			State_Number	=0;
		
			if(ESP8266_Result==TRUE)
				ESP8266_State=ESP8266_IDLE;
			else if(ESP8266_Result==FALSE)
				ESP8266_State=ESP8266_RESET;
			
			break;
		
		case ESP8266_SEND_AT:
			
			ESP8266_Result=ESP8266_AT_Send(TRUE,"AT");
			State_Number	=1;
			
			if(ESP8266_Result==TRUE)
				ESP8266_State=ESP8266_SEND_AT_GMR;
			else if(ESP8266_Result==FALSE)
				ESP8266_State=ESP8266_SEND_AT;
		
			break;
		
		case ESP8266_SEND_AT_GMR:
			ESP8266_Result=ESP8266_AT_Send(TRUE,"AT+GMR");
			State_Number	=2;
		
			if(ESP8266_Result==TRUE)
				ESP8266_State=ESP8266_SEND_WIFI_SETTING;
			else if(ESP8266_Result==FALSE)
				ESP8266_State=ESP8266_SEND_AT;
			
			break;
			
		case ESP8266_SEND_WIFI_SETTING:
			ESP8266_Result=ESP8266_WifiSetting();
			State_Number	=3;
		
			if(ESP8266_Result==TRUE)
				ESP8266_State=ESP8266_FIND_SSID;
			else if(ESP8266_Result==FALSE)
				ESP8266_State=ESP8266_SEND_AT;
			break;
			
		case ESP8266_FIND_SSID:
			ESP8266_Result=Find_SSID();
			State_Number	=4;
		
			if(ESP8266_Result==TRUE)
				ESP8266_State=ESP8266_CONNECT_SSID;
			else if(ESP8266_Result==FALSE)
				ESP8266_State=ESP8266_SEND_AT;
		
		break;
			
		case ESP8266_CONNECT_SSID:
			ESP8266_Result=Connect_SSID();
			State_Number	=5;
		
			if(ESP8266_Result==TRUE)
				ESP8266_State=ESP8266_CONNECT_OK;
			else if(ESP8266_Result==FALSE)
				ESP8266_State=ESP8266_CONNECT_SSID;
			else if(ESP8266_Result==RESET_IT)
				ESP8266_State=ESP8266_RESET;
		break;
			
		case ESP8266_CONNECT_OK:
			ESP8266_Result=CheckConnection();
			State_Number	=6;
			
			if(ESP8266_Result==TRUE)
				ESP8266_State=ESP8266_GET_IP;
			else if(ESP8266_Result==FALSE)
				ESP8266_State=ESP8266_RESET;
			
		break;
			
		case ESP8266_GET_IP:
			ESP8266_Result=GetIPNumber();
			State_Number	=7;
			
			if(ESP8266_Result==TRUE)
			{
				ESP8266_State					=ESP8266_GET_WEATHER_CITY1;
				ESP8266_Error_Counter	=0;
			}
			else if(ESP8266_Result==FALSE)
				ESP8266_State=ESP8266_RESET;
		break;
			
		case ESP8266_GET_WEATHER_CITY1:
			ESP8266_Result	=OpenWeatherMap(0);
			
			if(ESP8266_Result==TRUE)
			{
				State_Number		=8;
				FindTemperatureValues();
				if(SWITCH_CITY_ENABLE>0)
					ESP8266_State	=ESP8266_GET_WEATHER_CITY2;
				else
					ESP8266_State	=ESP8266_GET_WEATHER_CITY1;
				
				ESP8266_Error_Counter=0;
			}
			else if(ESP8266_Result==FALSE)
				ESP8266_Error_Counter++;
			
			if(ESP8266_Error_Counter>10)
				ESP8266_State=ESP8266_RESET;
		break;
			
		case ESP8266_GET_WEATHER_CITY2:
			ESP8266_Result	=OpenWeatherMap(1);
			
			if(ESP8266_Result==TRUE)
			{
				State_Number		=9;
				FindTemperatureValues();
				if(SWITCH_CITY_ENABLE>1)
					ESP8266_State	=ESP8266_GET_WEATHER_CITY3;
				else
					ESP8266_State	=ESP8266_GET_WEATHER_CITY1;
				
				ESP8266_Error_Counter=0;
			}
			else if(ESP8266_Result==FALSE)
				ESP8266_Error_Counter++;
			
			if(ESP8266_Error_Counter>10)
				ESP8266_State=ESP8266_RESET;
		break;
			
		case ESP8266_GET_WEATHER_CITY3:
			ESP8266_Result	=OpenWeatherMap(2);
			
			if(ESP8266_Result==TRUE)
			{
				State_Number		=10;
				FindTemperatureValues();
				ESP8266_State	=ESP8266_GET_WEATHER_CITY1;
				ESP8266_Error_Counter=0;
			}
			else if(ESP8266_Result==FALSE)
				ESP8266_Error_Counter++;
			
			if(ESP8266_Error_Counter>10)
				ESP8266_State=ESP8266_RESET;
		break;
			
		default:
			ESP8266_State=ESP8266_IDLE;
			
		break;
	}
	
	return State_Number;
}

void InitESP8266(void)
{	
	ESP8266_UART_Init();
	ESP8266_Timer_Init();
	
	At_State=AT_IDLE;
}
