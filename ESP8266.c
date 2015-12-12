#include <string.h>
#include "ESP8266.h"
#include "explorer16.h"


AT_STATE                At_State;
WIFI_STATE              Wifi_State;
SSID_STATE              SSID_State;
CON_SSID_STATE          Con_SSID_State;
RESET_STATE             Reset_State;
CONNECTION_STATE        Connection_State;
GET_IP_NUMBER           Get_Ip_State;
CON_SERVER_STATE        ConServer_State;
SEND_DATA_STATE         SendData_State;
WAIT_DATA_STATE         WaitData_State;
DISCON_SERVER_STATE     DisconServer_State;
ESP8266_STATE           ESP8266_State;

unsigned int            ESP8266_Counter;

#define ESP8266ReceiveBuffer    U2RxBuffer.data

void ClearBuffer(void)
{
    U2RxBuffer.bytes_total=0;
    U2RxBuffer.status=Empty;
}

unsigned char ESP8266_AT_Send(const char Command[])
{
    unsigned char Return_Result=WAIT;
    if(strcmp(Command,"AT")==0){
        switch(At_State){
            case AT_IDLE:
                At_State=AT_SEND_DATA;
                Return_Result=WAIT;
                break;
            case AT_SEND_DATA:
                putsUART1("\nAT");
                ClearBuffer();
                putsUART2("AT\r\n");
                At_State=AT_ANS_WAIT;
                ESP8266_Counter=ESP8266_TIMEOUT;
                Return_Result=WAIT;
                break;
            case AT_ANS_WAIT:
                if(U2RxBuffer.status == Ready){
                    if(strstr(ESP8266ReceiveBuffer,"ERROR")){
                        At_State=AT_ANS_TIMEOUT;
                        putsUART1("..Error!\r\n");
                    }
                    if(strcmp(ESP8266ReceiveBuffer,"AT\r\r\n\r\nOK\r\n")==0){
                        At_State=AT_IDLE;
                        Return_Result=TRUE;
                        U2RxBuffer.status == Empty;
                        putsUART1("..OK!\r\n");
                    }
                    ClearBuffer();
                }else{
                    if(ESP8266_Counter==0){
                        At_State=AT_ANS_TIMEOUT;
                        putsUART1("..TimeOut!\r\n");
                    }
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
        switch(At_State){
            case AT_IDLE:
                At_State=AT_SEND_DATA;
                Return_Result=WAIT;
                break;
            case AT_SEND_DATA:
                ClearBuffer();
                putsUART1("\nAT+GMR");
                putsUART2("AT+GMR\r\n");
                At_State=AT_ANS_WAIT;
                ESP8266_Counter	=ESP8266_TIMEOUT;
                Return_Result=WAIT;
                break;

            case AT_ANS_WAIT:
                if(U2RxBuffer.status == Ready){
                    if(strstr(ESP8266ReceiveBuffer,"ERROR")){
                        At_State=AT_ANS_TIMEOUT;
                        putsUART1("..Error!\r\n");
                    }
                    if(strstr(ESP8266ReceiveBuffer,"OK")!=NULL){
                        At_State=AT_IDLE;
                        Return_Result=TRUE;
                        putsUART1("..OK!\r\n");
                    }
                    ClearBuffer();
                }else{
                    if(ESP8266_Counter==0){
                        At_State=AT_ANS_TIMEOUT;
                        putsUART1("..TimeOut!\r\n");
                    }
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

    return Return_Result;
}

unsigned char ESP8266_WifiSetting(void)
{
    unsigned char Return_Result=WAIT;

    switch(Wifi_State){
        case WIFI_IDLE:
            Wifi_State=AT_CWMODE;
            Return_Result=WAIT;
            break;
        case AT_CWMODE:
            putsUART1("\nAT+CWMODE=1");
            ClearBuffer();
            putsUART2("AT+CWMODE=1\r\n");
            Wifi_State=AT_CWMODE_ANS_WAIT;
            ESP8266_Counter=ESP8266_TIMEOUT;
            Return_Result=WAIT;
            break;
        case AT_CWMODE_ANS_WAIT:
            if(U2RxBuffer.status == Ready){
                if(strstr(ESP8266ReceiveBuffer,"ERROR")){
                    Wifi_State=WIFI_ANS_TIMEOUT;
                    putsUART1("..Error1!\r\n");
                }
                if(strstr(ESP8266ReceiveBuffer,"AT+CWMODE=1")!=NULL){
                    Wifi_State=AT_CIPMODE;
                    Return_Result=WAIT;
                    putsUART1("..OK1!\r\n");
                }
                ClearBuffer();
            }else{
                if(ESP8266_Counter==0){
                    Wifi_State=WIFI_ANS_TIMEOUT;
                    putsUART1("..TimeOut!\r\n");
                }
            }
            break;
        case AT_CIPMODE:
            putsUART1("\nAT+CIPMODE?");
            ClearBuffer();
            putsUART2("AT+CIPMODE?\r\n");
            Wifi_State=AT_CIPMODE_ANS_WAIT;
            ESP8266_Counter=ESP8266_TIMEOUT;
            Return_Result=WAIT;
            break;
        case AT_CIPMODE_ANS_WAIT:
            if(U2RxBuffer.status == Ready){
                if(strstr(ESP8266ReceiveBuffer,"ERROR")){
                    Wifi_State=WIFI_ANS_TIMEOUT;
                    putsUART1("..Error2!\r\n");
                }
                if(strstr(ESP8266ReceiveBuffer,"CIPMODE:0")!=NULL){
                    Wifi_State=AT_CIPMUX;
                    Return_Result=WAIT;
                    putsUART1("..OK2!\r\n");
                }
                ClearBuffer();
            }else{
                if(ESP8266_Counter==0){
                    Wifi_State=WIFI_ANS_TIMEOUT;
                    putsUART1("..TimeOut2!\r\n");
                }
            }
            break;

        case AT_CIPMUX:
            putsUART1("\nAT+CIPMUX=0");
            ClearBuffer();
            putsUART2("AT+CIPMUX=0\r\n");
            Wifi_State=AT_CIPMUX_ANS_WAIT;
            ESP8266_Counter=ESP8266_TIMEOUT;
            Return_Result=WAIT;
            break;
        case AT_CIPMUX_ANS_WAIT:
            if(U2RxBuffer.status == Ready){
                if(strstr(ESP8266ReceiveBuffer,"ERROR")){
                    Wifi_State=WIFI_ANS_TIMEOUT;
                    putsUART1("..Error3!\r\n");
                }
                if(strstr(ESP8266ReceiveBuffer,"AT+CIPMUX=0")!=NULL){
                    Wifi_State=WIFI_IDLE;
                    Return_Result=TRUE;
                    putsUART1("..OK3!\r\n");
                }
                ClearBuffer();
            }else{
                if(ESP8266_Counter==0){
                    Wifi_State=WIFI_ANS_TIMEOUT;
                    putsUART1("..TimeOut3!\r\n");
                }
            }
            break;
        case WIFI_ANS_TIMEOUT:
                Return_Result=FALSE;
                Wifi_State=WIFI_IDLE;
                break;
        default:
                Wifi_State=WIFI_IDLE;
                Return_Result=WAIT;
                break;
    }
    return Return_Result;
}

unsigned char Find_SSID(void)
{
    unsigned char Return_Result=WAIT;
    switch(SSID_State){
        case SSID_IDLE:
            SSID_State=AT_CWLAP;
            Return_Result=WAIT;
            break;
        case AT_CWLAP:
            putsUART1("\nAT+CWLAP");
            ClearBuffer();
            putsUART2("AT+CWLAP\r\n");
            SSID_State=AT_CWLAP_ANS_WAIT;
            ESP8266_Counter	=100000;
            Return_Result=WAIT;
            break;
        case AT_CWLAP_ANS_WAIT:
            if(U2RxBuffer.status == Ready){
                putsUART1(&U2RxBuffer.data);
                if(strstr(ESP8266ReceiveBuffer,"ERROR")){
                    SSID_State=SSID_ANS_TIMEOUT;
                    putsUART1("..TimeOut!\r\n");
                }
                if(strstr(ESP8266ReceiveBuffer,"EM4")!=NULL){
                    SSID_State=SSID_IDLE;
                    Return_Result=TRUE;
                    putsUART1("..Find OK!\r\n");
                }
                ClearBuffer();
            }else{
                if(ESP8266_Counter==0){
                    SSID_State=SSID_ANS_TIMEOUT;
                    putsUART1("..TimeOut!\r\n");
                }
            }
            break;
        case SSID_ANS_TIMEOUT:
            Return_Result=FALSE;
            SSID_State=SSID_IDLE;
            break;
        default:
            Return_Result=WAIT;
            SSID_State=SSID_IDLE;
            break;
    }

    return Return_Result;
}

unsigned char Connect_SSID(void)
{
    unsigned char Return_Result=WAIT;
    char Dummy_String[50]="";

    switch(Con_SSID_State){
        case CON_SSID_IDLE:
            Con_SSID_State=AT_CWJAP;
            Return_Result=WAIT;
            break;
        case AT_CWJAP:
            putsUART1("\nAT+CWJAP\r\n");
            ClearBuffer();
            strcat(Dummy_String,"AT+CWJAP=\"");
            strcat(Dummy_String,SSID_NAME);
            strcat(Dummy_String,"\",\"");
            strcat(Dummy_String,SSID_PASSWORD);
            strcat(Dummy_String,"\"\r\n");
            putsUART2(Dummy_String,strlen(Dummy_String));            
            Con_SSID_State	=AT_CWJAP_ANS_WAIT;
            ESP8266_Counter	=ESP8266_CWJAP_TIMEOUT;
            Return_Result=WAIT;
            break;
        case AT_CWJAP_ANS_WAIT:
            if(U2RxBuffer.status == Ready){
                putsUART1(&U2RxBuffer.data);
                if(strstr(ESP8266ReceiveBuffer,"ERROR")!=NULL){
                    Con_SSID_State=CON_SSID_ANS_TIMEOUT;
                    putsUART1("..TimeOut!\r\n");
                }
                if(strstr(ESP8266ReceiveBuffer,"OK")!=NULL){
                    Con_SSID_State=CON_SSID_IDLE;
                    Return_Result=TRUE;
                    putsUART1("..Connect OK!\r\n");
                }
                if(strstr(ESP8266ReceiveBuffer,"FAIL")!=NULL){
                    Con_SSID_State=CON_SSID_FAIL;
                    putsUART1("..FAIL!\r\n");
                }
                ClearBuffer();
            }else{
                if(ESP8266_Counter==0){
                    Con_SSID_State=CON_SSID_ANS_TIMEOUT;
                    putsUART1("..TimeOut!\r\n");
                }
            }
            break;
        case SSID_ANS_TIMEOUT:
            Return_Result=FALSE;
            Con_SSID_State=CON_SSID_IDLE;
            break;

        case CON_SSID_FAIL:
            Return_Result=RESET_IT;
            Con_SSID_State=CON_SSID_IDLE;
            break;

        default:
            Return_Result=WAIT;
            Con_SSID_State=CON_SSID_IDLE;
            break;
    }
    return Return_Result;
}
/**
 * Reset al modulo ESP8266
 * TRUE,FALSE,WAIT
 * @return
 */
unsigned char Reset_ESP8266(void){
    unsigned char Return_Result=WAIT;

    switch(Reset_State){
        case RESET_IDLE:
            Reset_State=AT_RST;
            Return_Result=WAIT;
            break;
        case AT_RST:
            putsUART1("\nAT+RST");
            ClearBuffer();
            putsUART2("AT+RST\r\n");
            Reset_State=AT_RST_ANS_WAIT;
            ESP8266_Counter=ESP8266_TIMEOUT;
            Return_Result=WAIT;
            break;

        case AT_RST_ANS_WAIT:
            if(U2RxBuffer.status == Ready){
                if(strstr(ESP8266ReceiveBuffer,"ERROR")){
                    Reset_State=RESET_ANS_TIMEOUT;
                    putsUART1("..ERROR!\r\n");
                }
                if(strstr(ESP8266ReceiveBuffer,"ready")!=NULL){
                    Reset_State=RESET_IDLE;
                    Return_Result=TRUE;
                    putsUART1("..Reset OK!\r\n");
                }
                ClearBuffer();
            }else{
                if(ESP8266_Counter==0){
                    Reset_State=RESET_ANS_TIMEOUT;
                    putsUART1("..TimeOut!\r\n");
                }
            }
            break;
        case RESET_ANS_TIMEOUT:
            Return_Result=FALSE;
            Reset_State=RESET_IDLE;
            break;
        default:
            Return_Result=WAIT;
            Reset_State=RESET_IDLE;
            break;
    }
    return Return_Result;
}

/**
 * Responde el estado de conexion con el SSID
 * FALSE,TRUE,WAIT
 * @return
 */
unsigned char CheckConnection(void)
{
    unsigned char Return_Result=WAIT;
    switch(Connection_State)
    {
        case CONNECTION_IDLE:
            Connection_State=CHECK_STILL_CONNECTED;
            Return_Result=WAIT;
            break;
        case CHECK_STILL_CONNECTED:
            putsUART1("\nAT+CWJAP?");
            ClearBuffer();
            putsUART2("AT+CWJAP?\r\n");
            Connection_State=CONNECTION_ANS_WAIT;
            ESP8266_Counter=100000;
            Return_Result=WAIT;
            break;
        case CONNECTION_ANS_WAIT:
            if(U2RxBuffer.status == Ready){
                if(strstr(ESP8266ReceiveBuffer,"ERROR")!=NULL){
                    Connection_State=CONNECTION_TIMEOUT;
                    putsUART1("..Error!\r\n");
                }
                if(strstr(ESP8266ReceiveBuffer,SSID_NAME)!=NULL){
                    Connection_State=CONNECTION_IDLE;
                    Return_Result=TRUE;
                    putsUART1("..Check OK!\r\n");
                }
                if(strstr(ESP8266ReceiveBuffer,"OK")!=NULL){
                    Connection_State=CONNECTION_IDLE;
                    Return_Result=TRUE;
                    putsUART1("..Check OK!\r\n");
                }
                if(strstr(ESP8266ReceiveBuffer,"WIFI CONNECTED")!=NULL){
                    Connection_State=CONNECTION_ANS_WAIT;
                    Return_Result=WAIT;
                    putsUART1("..WIFI CONNECTED!\r\n");
                }
                if(strstr(ESP8266ReceiveBuffer,"WIFI GOT IP")!=NULL){
                    Connection_State=CONNECTION_IDLE;
                    Return_Result=TRUE;
                    putsUART1("..WIFI GOT IP!\r\n");
                }
                ClearBuffer();
            }else{
                if(ESP8266_Counter==0){
                    Connection_State=CONNECTION_TIMEOUT;
                    putsUART1("..TimeOut!\r\n");
                }
            }
            break;
        case CONNECTION_TIMEOUT:
            Return_Result=FALSE;
            Connection_State=CONNECTION_IDLE;
            break;
        default:
            Return_Result=WAIT;
            Connection_State=CONNECTION_IDLE;
            break;
    }

    return Return_Result;
}

unsigned char CheckIP(void)
{
    unsigned char Return_Result=WAIT;

    switch(Get_Ip_State)
    {
        case AT_CIPSTA_IDLE:
            Get_Ip_State=AT_CIPSTA;
            Return_Result=WAIT;
            break;

        case AT_CIPSTA:
            putsUART1("\nAT+CIPSTA?");
            ClearBuffer();
            putsUART2("AT+CIPSTA?\r\n");
            Get_Ip_State=AT_CIPSTA_ANS_WAIT;
            ESP8266_Counter=ESP8266_TIMEOUT;
            Return_Result=WAIT;
            break;
        case AT_CIPSTA_ANS_WAIT:
            if(U2RxBuffer.status == Ready){
                if(strstr(ESP8266ReceiveBuffer,"ERROR")!=NULL){
                    Get_Ip_State=AT_CIPSTA_TIMEOUT;
                    putsUART1("..Error!\r\n");
                }
//                if(strstr(ESP8266ReceiveBuffer,"+CIPSTA:")!=NULL){
                if(strstr(ESP8266ReceiveBuffer,"192.168.")!=NULL){
                    Get_Ip_State=AT_CIPSTA_IDLE;
                    Return_Result=TRUE;
                    putsUART1("..Get IP OK!\r\n");
                    putsUART1(&U2RxBuffer.data);
                }
                ClearBuffer();
            }else{
                if(ESP8266_Counter==0){
                    Get_Ip_State=AT_CIPSTA_TIMEOUT;
                    putsUART1("..TimeOut!\r\n");
                }
            }
            break;

        case AT_CIPSTA_TIMEOUT:
            Return_Result=FALSE;
            Get_Ip_State=AT_CIPSTA_IDLE;
            break;

        default:
            Return_Result=WAIT;
            Get_Ip_State=AT_CIPSTA_IDLE;
            break;
    }

    return Return_Result;
}

unsigned char Connect_Server(void)
{
    unsigned char Return_Result=WAIT;
    char ServerAdd[50]="";
    switch(ConServer_State){
        case CON_SERVER_IDLE:
            ConServer_State=AT_CIPSTART;
            Return_Result=WAIT;
            break;
	case AT_CIPSTART:
            ClearBuffer();
            strcat(ServerAdd,"AT+CIPSTART=");
            strcat(ServerAdd,"\"");
            strcat(ServerAdd,"TCP");
            strcat(ServerAdd,"\"");
            strcat(ServerAdd,",");
            strcat(ServerAdd,"\"");
            strcat(ServerAdd,UBIDOTS);
            strcat(ServerAdd,"\"");
            strcat(ServerAdd,",");
            strcat(ServerAdd,UBIDOTS_PORT);            
            strcat(ServerAdd,"\r\n");
            putsUART2(ServerAdd);
            putsUART1(ServerAdd);
            ConServer_State=AT_CIPSTART_ANS_WAIT;
            ESP8266_Counter=ESP8266_TIMEOUT;
            Return_Result=WAIT;
            break;
        case AT_CIPSTART_ANS_WAIT:
            if(U2RxBuffer.status == Ready){
                putsUART1(&U2RxBuffer.data);
                if(strstr(ESP8266ReceiveBuffer,"ERROR")!=NULL){
                    ConServer_State=CON_SERVER_TIMEOUT;
                    putsUART1("..Error!\r\n");
                }
                if(strstr(ESP8266ReceiveBuffer,"CONNECT")!=NULL){
                    ConServer_State=CON_SERVER_IDLE;
                    Return_Result=TRUE;
                    putsUART1("..OK!\r\n");
                }
                ClearBuffer();
            }else{
                if(ESP8266_Counter==0){
                    ConServer_State=CON_SERVER_TIMEOUT;
                    putsUART1("..TimeOut!\r\n");
                }
            }
            break;
        case CON_SERVER_TIMEOUT:
            Return_Result=FALSE;
            ConServer_State=CON_SERVER_IDLE;
            break;
        default:
            Return_Result=WAIT;
            ConServer_State=CON_SERVER_IDLE;
            break;
        }
        return Return_Result;

}
unsigned char Send_data(void){
    unsigned char Header[20];
    unsigned char LongData[5];
    unsigned char Data[250];
    int iLongData;
    unsigned char Return_Result=WAIT;
    
    switch(SendData_State){
        case SEND_DATA_IDLE:
            SendData_State=AT_CIPSEND;
            Return_Result=WAIT;
            break;
        case AT_CIPSEND:
            ClearBuffer();
            putsUART1("AT+CIPSEND");
            strcat(Header,"AT+CIPSEND=205\r\n");
            //itoa(LongData,iLongData,10);
            //strcat(Header,LongData);
            //strcat(Header,"\r\n");
            putsUART2(Header);
            SendData_State=AT_CIPSEND_ANS_WAIT;
            ESP8266_Counter=ESP8266_TIMEOUT;
            Return_Result=WAIT;
            break;
        case AT_CIPSEND_ANS_WAIT:
            if(U2RxBuffer.status==Ready){
                if(strstr(ESP8266ReceiveBuffer,"ERROR")!=NULL){
                   SendData_State=SEND_DATA_TIMEOUT;
                    putsUART1("..Error!\r\n");
                }
                if(strstr(ESP8266ReceiveBuffer,">")!=NULL)
                {
                    SendData_State=SEND_DATA;
                    Return_Result=WAIT;
                    putsUART1("..OK!>\r\n");
                }
                ClearBuffer();
            }else{
                if(ESP8266_Counter==0){
                    SendData_State=SEND_DATA_TIMEOUT;
                    putsUART1("..TimeOut!\r\n");
                }
            }
            break;
        case SEND_DATA:
            ClearBuffer();
            strcat(Data,"POST /api/v1.6/variables/566bb44e76254208c0abeedd/values HTTP/1.1\r\n");
            strcat(Data,"Host: things.ubidots.com\r\n");
            strcat(Data,"Content-Type: application/json\r\n");
            strcat(Data,"X-Auth-Token: OQaKk5XmnBZiFxwensWsj1VdLqhx6m\r\n");
            strcat(Data,"Content-Length: 12\r\n\r\n");
            strcat(Data,"{\"value\":24}");
            //iLongData=strlen(Data);
            putsUART1("\nData");
            putsUART2(Data);
            SendData_State=SEND_DATA_ANS_WAIT;
            ESP8266_Counter=1000000000;
            Return_Result=WAIT;
            break;
        case SEND_DATA_ANS_WAIT:
            if(U2RxBuffer.status == Ready){
                putsUART1(&U2RxBuffer.data);
                if(strstr(ESP8266ReceiveBuffer,"ERROR")!=NULL){
                    SendData_State=SEND_DATA_TIMEOUT;
                    putsUART1("..Error\r\n");
                }
                if(strstr(ESP8266ReceiveBuffer,"SEND OK")!=NULL)
                {
                    SendData_State=SEND_DATA_IDLE;
                    Return_Result=TRUE;
                    putsUART1("..OK!\r\n");
                }
                ClearBuffer();
            }else{
                if(ESP8266_Counter==0){
                    SendData_State=SEND_DATA_TIMEOUT;
                    putsUART1("..TimeOut\r\n");
                }
            }
            break;
        case SEND_DATA_TIMEOUT:
            Return_Result=FALSE;
            SendData_State=SEND_DATA_IDLE;
            break;
        default:
            Return_Result=WAIT;
            SendData_State=SEND_DATA_IDLE;
            break;
    }
    return Return_Result;
}
unsigned char Disconnect_Server(void){
    unsigned char Return_Result=WAIT;
    switch(DisconServer_State){
        case DISCON_SERVER_IDLE:
            DisconServer_State=AT_CIPCLOSE;
            Return_Result=WAIT;
            break;
        case AT_CIPCLOSE:
            ClearBuffer();
            putsUART1("AT+CIPCLOSE");
            putsUART2("AT+CIPCLOSE\r\n");
            DisconServer_State=AT_CIPCLOSE_ANS_WAIT;
            ESP8266_Counter=ESP8266_TIMEOUT;
            Return_Result=WAIT;
            break;
        case AT_CIPCLOSE_ANS_WAIT:
            if(U2RxBuffer.status==Ready){
                if(strstr(ESP8266ReceiveBuffer,"ERROR")!=NULL){
                    DisconServer_State=DISCON_SERVER_TIMEOUT;
                    putsUART1("..Error!\r\n");
                }
                if(strstr(ESP8266ReceiveBuffer,"CLOSE")!=NULL)
                {
                    DisconServer_State=DISCON_SERVER_IDLE;
                    Return_Result=TRUE;
                    putsUART1("..OK!\r\n");
                }
                ClearBuffer();
            }else{
                if(ESP8266_Counter==0){
                    DisconServer_State=DISCON_SERVER_TIMEOUT;
                    putsUART1("..TimeOut!\r\n");
                }
            }
            break;
        case DISCON_SERVER_TIMEOUT:
            Return_Result=FALSE;
            DisconServer_State=DISCON_SERVER_IDLE;
            break;
        default:
            Return_Result=WAIT;
            DisconServer_State=DISCON_SERVER_IDLE;
            break;
	}
    return Return_Result;
}

unsigned char Wait_data(void){
    unsigned char Return_Result=WAIT;
    switch(WaitData_State){
        case WAIT_DATA_IDLE:
            WaitData_State=WAIT_DATA;
            Return_Result=WAIT;
            break;
        case WAIT_DATA:
            ClearBuffer();
            putsUART1("\nWAIT RESPONSE");
            WaitData_State=WAIT_DATA_ANS_WAIT;
            ESP8266_Counter=ESP8266_TIMEOUT;
            Return_Result=WAIT;
            break;
        case WAIT_DATA_ANS_WAIT:
            if(U2RxBuffer.status==Ready){
                if(strstr(ESP8266ReceiveBuffer,"ERROR")){
                    WaitData_State=WAIT_DATA_TIMEOUT;
                    putsUART1("..Error!\r\n");
                }
                if(strstr(ESP8266ReceiveBuffer,"+IPD,")!=NULL)
                {
                    putsUART1(&U2RxBuffer.data);
                    putsUART1("\r\n");
                    WaitData_State=WAIT_DATA_IDLE;
                    ESP8266_Counter=0;
                    Return_Result=TRUE;
                }
                ClearBuffer();
            }else{
                if(ESP8266_Counter==0){
                    WaitData_State=WAIT_DATA_TIMEOUT;
                    putsUART1("..TimeOut!\r\n");
                }
            }
            break;
        case WAIT_DATA_TIMEOUT:
            Return_Result=FALSE;
            WaitData_State=WAIT_DATA_IDLE;
            break;
        default:
            Return_Result=WAIT;
            WaitData_State=WAIT_DATA_IDLE;
            break;
	}
    return Return_Result;
}


unsigned char ESP8266_Error_Counter;
	
unsigned char ProcessESP8266(void){
    unsigned char ESP8266_Result;
    switch(ESP8266_State){
        case ESP8266_IDLE:
            ESP8266_State=ESP8266_SEND_AT;
            break;
        case ESP8266_RESET:
            ESP8266_Result=Reset_ESP8266();
            if(ESP8266_Result==TRUE) ESP8266_State=ESP8266_IDLE;
            if(ESP8266_Result==FALSE)ESP8266_State=ESP8266_RESET;
            break;
        case ESP8266_SEND_AT:
            ESP8266_Result=ESP8266_AT_Send("AT");
            if(ESP8266_Result==TRUE) ESP8266_State=ESP8266_SEND_AT_GMR;
            if(ESP8266_Result==FALSE) ESP8266_State=ESP8266_SEND_AT;
            break;
        case ESP8266_SEND_AT_GMR:
            ESP8266_Result=ESP8266_AT_Send("AT+GMR");
            if(ESP8266_Result==TRUE) ESP8266_State=ESP8266_SEND_WIFI_SETTING;
            if(ESP8266_Result==FALSE) ESP8266_State=ESP8266_SEND_AT;
            break;
        case ESP8266_SEND_WIFI_SETTING:
            ESP8266_Result=ESP8266_WifiSetting();
            if(ESP8266_Result==TRUE) ESP8266_State=ESP8266_FIND_SSID;
            if(ESP8266_Result==FALSE) ESP8266_State=ESP8266_SEND_AT;
            break;
        case ESP8266_FIND_SSID:
            ESP8266_Result=Find_SSID();
            switch(ESP8266_Result){
                case TRUE:
                    ESP8266_State=ESP8266_CONNECT_SSID;
                    break;
                case FALSE:
                    ESP8266_Error_Counter++;
                    if(ESP8266_Error_Counter>10){
                        ESP8266_State=ESP8266_SEND_AT;
                        ESP8266_Error_Counter=0;
                    }else{
                        ESP8266_State=ESP8266_RESET;
                    }
                    break;
                case WAIT:
                    break;
                default:
                    ESP8266_State=ESP8266_RESET;
                    break;
            }
            break;
        case ESP8266_CONNECT_SSID:
            ESP8266_Result=Connect_SSID();
            switch(ESP8266_Result){
                case TRUE:
                    ESP8266_State=ESP8266_CHECK_WIFI;
                    break;
                case FALSE:
                    ESP8266_Error_Counter++;
                    if(ESP8266_Error_Counter>10){
                        ESP8266_State=ESP8266_CONNECT_SSID;
                        ESP8266_Error_Counter=0;
                    }else{
                        ESP8266_State=ESP8266_RESET;
                    }
                    break;
                case WAIT:
                    break;
                default:
                    ESP8266_State=ESP8266_RESET;
                    break;
            }
            break;
        case ESP8266_CHECK_WIFI:
            ESP8266_Result=CheckConnection();
            switch(ESP8266_Result){
                case TRUE:
                    ESP8266_State=ESP8266_CHECK_IP;
                    break;
                case FALSE:
                    ESP8266_Error_Counter++;
                    if(ESP8266_Error_Counter>10){
                        ESP8266_State=ESP8266_RESET;
                        ESP8266_Error_Counter=0;
                    }else{
                        ESP8266_State=ESP8266_CHECK_WIFI;
                    }
                    break;
                case WAIT:
                    break;
                default:
                    ESP8266_State=ESP8266_RESET;
                    break;
            }
            break;
        case ESP8266_CHECK_IP:
            ESP8266_Result=CheckIP();
            switch(ESP8266_Result){
                case TRUE:
                    ESP8266_State=ESP8266_CONNECT_SERVER;
                    break;
                case FALSE:
                    ESP8266_State=ESP8266_RESET;
                    break;
                case WAIT:
                    break;
                default:
                    ESP8266_State=ESP8266_RESET;
                    break;
            }
            break;
        case ESP8266_CONNECT_SERVER:
            ESP8266_Result=Connect_Server();
            switch(ESP8266_Result){
                case TRUE:
                    ESP8266_State=ESP8266_SEND_DATA;
                    break;
                case FALSE:
                    ESP8266_Error_Counter++;
                    if(ESP8266_Error_Counter>10){
                        ESP8266_State=ESP8266_CHECK_WIFI;
                        ESP8266_Error_Counter=0;
                    }else{
                        ESP8266_State=ESP8266_CHECK_IP;
                    }
                    break;
                case WAIT:
                    break;
                default:
                    ESP8266_State=ESP8266_RESET;
                    break;
            }
            break;
        case ESP8266_SEND_DATA:
            ESP8266_Result=Send_data();
            switch(ESP8266_Result){
                case TRUE:
                    ESP8266_State=ESP8266_WAIT_DATA;
                    break;
                case FALSE:
                    ESP8266_Error_Counter++;
                    if(ESP8266_Error_Counter>3){
                        ESP8266_State=ESP8266_DISCONNECT_SERVER;
                        ESP8266_Error_Counter=0;
                    }else{
                        ESP8266_State=ESP8266_SEND_DATA;
                    }
                    break;
                case WAIT:
                    break;
                default:
                    ESP8266_State=ESP8266_RESET;
                    break;
            }
            break;
        case ESP8266_WAIT_DATA:
            ESP8266_Result=Wait_data();
            switch(ESP8266_Result){
                case TRUE:
                    ESP8266_State=ESP8266_SEND_DATA;
                    break;
                case FALSE:
                    ESP8266_State=ESP8266_DISCONNECT_SERVER;
                    break;
                case WAIT:
                    break;
                default:
                    ESP8266_State=ESP8266_RESET;
                    break;
            }
            break;
        case ESP8266_DISCONNECT_SERVER:
            ESP8266_Result=Disconnect_Server();
            switch(ESP8266_Result){
                case TRUE:
                    ESP8266_State=ESP8266_CONNECT_SERVER;
                    break;
                case FALSE:
                    ESP8266_State=ESP8266_RESET;
                    break;
                case WAIT:
                    break;
                default:
                    ESP8266_State=ESP8266_RESET;
                    break;
            }
            break;
        default:
            ESP8266_State=ESP8266_IDLE;
            break;
    }
}

void InitESP8266(void)
{	
	At_State=AT_IDLE;
}
