#ifndef ESP8266_H
#define	ESP8266_H

#define ESP8266_RECEIVE_BUFFER_SIZE 		1000
#define ESP8266_TIMEOUT						10000
#define ESP8266_CWJAP_TIMEOUT				20000
#define ESP8266_GETWEATHER_TIMEOUT			5000

#define SSID_NAME							"WiFi_SSID"
#define SSID_PASSWORD						"WiFi_Password"

#define SWITCH_CITY_ENABLE					2
#define CITY_NAME_1							"Istanbul"
#define CITY_NAME_2							"Izmir"
#define CITY_NAME_3							"London"

#ifndef RESET_IT
#define RESET_IT		3
#endif
#ifndef WAIT
#define WAIT  			2
#endif
#ifndef TRUE
#define TRUE  			1
#endif
#ifndef FALSE
#define FALSE 			0
#endif

typedef enum
{
		AT_IDLE,
    AT_SEND_DATA,
    AT_ANS_WAIT,
		AT_ANS_TIMEOUT,
}AT_STATE;

typedef enum
{
		WIFI_IDLE,
		AT_CWMODE,
		AT_CWMODE_ANS_WAIT,
    AT_CIPMODE,
		AT_CIPMODE_ANS_WAIT,
    AT_CIPMUX,
		AT_CIPMUX_ANS_WAIT,
		WIFI_ANS_TIMEOUT,
}WIFI_STATE;

typedef enum
{
		SSID_IDLE,
		AT_CWLAP,
		AT_CWLAP_ANS_WAIT,
		SSID_ANS_TIMEOUT,
}SSID_STATE;

typedef enum
{
		CON_SSID_IDLE,
		AT_CWJAP,
		AT_CWJAP_ANS_WAIT,
		CON_SSID_ANS_TIMEOUT,
		CON_SSID_FAIL,
}CON_SSID_STATE;

typedef enum
{
		RESET_IDLE,
		AT_RST,
		AT_RST_ANS_WAIT,
		RESET_ANS_TIMEOUT,
}RESET_STATE;

typedef enum
{
		CONNECTION_IDLE,
		CHECK_STILL_CONNECTED,
		CONNECTION_ANS_WAIT,
		CONNECTION_TIMEOUT,
}CONNECTION_STATE;

typedef enum
{
		AT_CIPSTA_IDLE,
		AT_CIPSTA,
		AT_CIPSTA_ANS_WAIT,
		AT_CIPSTA_TIMEOUT,
}GET_IP_NUMBER;

typedef enum
{
		OPENWEATHERMAP_IDLE,
		AT_CIPSTART,
		AT_CIPSTART_ANS_WAIT,
		AT_CIPSEND,
		AT_CIPSEND_ANS_WAIT,
		GET_WEATHER,
		GET_WEATHER_ANS_WAIT,
		WAIT_FOR_CLOSED,
		OPENWEATHERMAP_TIMEOUT,
}OPENWEATHERMAP;

typedef enum
{
	ESP8266_IDLE,
	ESP8266_SEND_AT,
	ESP8266_SEND_AT_GMR,
	ESP8266_SEND_WIFI_SETTING,
	ESP8266_FIND_SSID,
	ESP8266_CONNECT_SSID,
	ESP8266_RESET,
	ESP8266_CONNECT_OK,
	ESP8266_GET_IP,
	ESP8266_GET_WEATHER_CITY1,
	ESP8266_GET_WEATHER_CITY2,
	ESP8266_GET_WEATHER_CITY3,
}ESP8266_STATE;

struct StcCity
{
  char	City_Temp[10];
  char  City_Pressure[10];
  char 	City_Humidity[10];
};

extern unsigned char 		ESP8266_Version[10];
extern char					 		ESP8266_CWMODE[10];
extern char							ESP8266_CIPMODE[10];
extern char							ESP8266_CIPMUX[10];
extern char							ESP8266_IP[20];
extern struct StcCity		City;
extern unsigned int			GetWeatherCounter;

extern ESP8266_STATE ESP8266_State;

extern void InitESP8266(void);
extern void ClearBuffer(void);
extern unsigned char ESP8266_AT_Send(unsigned char AT_Send, const char Command[]);
extern unsigned char ESP8266_WifiSetting(void);
extern unsigned char Find_SSID(void);
extern unsigned char Connect_SSID(void);
extern unsigned char Reset_ESP8266(void);
extern unsigned char CheckConnection(void);
extern unsigned char GetIPNumber(void);
extern unsigned char OpenWeatherMap(unsigned char City_Number);
extern void FindTemperatureValues(void);
extern unsigned short Search_SubString(char Source[], char Target[]);
extern unsigned char ProcessESP8266(void);

#endif
