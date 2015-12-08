/* 
 * File:   main.c
 * Author: emontes
 *
 * Created on November 25, 2014, 1:01 PM
 */

#include <p24FJ256GA110.h>
#include <stdio.h>
#include <stdlib.h>
#include <ports.h>
#include <PwrMgnt.h>
#include <wdt.h>
#include <uart.h>
#include <string.h>
#include <Rtcc.h>
#include "explorer16.h"
#include "xlcd.h"

/* Configuraciones globales para el microcontrolador */
_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & BKBUG_OFF & ICS_PGx2 & FWDTEN_OFF & WINDIS_OFF & FWPSA_PR128 & WDTPS_PS256)
_CONFIG2(IESO_OFF & FNOSC_PRIPLL & FCKSM_CSECME & OSCIOFNC_ON & IOL1WAY_OFF & POSCMOD_HS)
_CONFIG3( WPCFG_WPCFGDIS & WPDIS_WPDIS & WPEND_WPSTARTMEM & WPFP_WPFP511 )

/* Estructuras globales utilizadas en la libreria PwrMgnt */
RESET_SOURCE reset_source;
WAKEUP_SOURCE wakeup_source;

/* Variables Globales */
unsigned int vSystemLed;
unsigned int vDelay_x10ms;
unsigned int vDelay_x1ms;
unsigned int vDelay_x1s;
unsigned int vDelay;
unsigned int vLCD;
unsigned char vChecksum;
unsigned char *pData;
unsigned char *pData2;
unsigned int i;
unsigned char string[50];
unsigned char *pString;
unsigned int vTemp;
rtccTime RtccTime;
rtccTimeDate RtccTimeDate;
rtccTimeDate RtccTimeDateVal;
BOOL update_time;
BOOL Time_Input_out_of_range;


int main(int argc, char** argv) {

    init_ports();
    init_peripherals();
    init_vars();                        /* Inicializacion de variables */
    init_system_clock();                /* Inicializacion del reloj del sistema */
    XLCDInit();                         /* Inicializacion de LCD */
    init_uart(1,_115200);               /* Inicializacion UART1 - P1 */
//  init_fram();
//  init_analog();
    RtccInitClock();                    //turn on clock source
    RtccWrOn();                         // unlock write to RTCC control register
    mRtccOn();                          //enable RTCC pripheral

    //UART Demo Messages
    putsUARTx("\nMicrochip Explorer 16\n");
    putsUARTx("System Entertainment Solutions SAC\n");
    putsUARTx("Enrique Montes\n");
    
    //Power UP bit test
    switch(reset_source){
        case POWER_ON_Reset:
            mPWRMGNT_Clear_PORbit();
            putsUARTx("Msg: Power On Reset\n");
            break;
        case BURN_OUT_Reset:
            mPWRMGNT_Clear_BORbit();
            putsUARTx("Msg: Burn Out Reset\n");
            break;
        case WATCHDOG_Reset:
            mPWRMGNT_Clear_WDTObit();
            putsUARTx("Msg: Watch Dog Reset\n");
            break;
        case SOFTWARE_Reset:
            mPWRMGNT_Clear_SWRbit();
            putsUARTx("Msg: Software Reset\n");
            break;
        case EXTERNAL_Reset:
            mPWRMGNT_Clear_EXTRbit();
            putsUARTx("Msg: External Reset\n");
            break;
        case CFG_WORD_MISMATCH_Reset:
            mPWRMGNT_Clear_CMbit();
            putsUARTx("Msg: Cfg word mismatch Reset\n");
            break;
        case ILLEGAL_INSTR_Reset:
            mPWRMGNT_Clear_IOPUWRbit();
            putsUARTx("Msg: Illegal Instr Reset\n");
            break;
        case TRAP_Reset:
            mPWRMGNT_Clear_TRAPRbit();
            putsUARTx("Msg: Trap Reset\n");
            break;
        default:
            putsUARTx("Msg: Unkwon Reset\n");
            break;
    }

    EnableWDT(WDT_ENABLE);               /* Enable Watchdog timer */

    XLCDClear();
    XLCDL1home();
    XLCDPutRamString("SES Proyectos");
    XLCDL2home();

    //Inicializacion de ESP-12E
    init_uart(2,_76800);                /* Inicializacion UART2 - J9 Pins 2,4 */
    LATGbits.LATG2=0;                   //Enable pin ESP-12E
    LATDbits.LATD0=0;                   //GPIO15
    LATDbits.LATD2=1;                   //GPIO0
    LATDbits.LATD1=1;                   //GPIO2
    LATGbits.LATG2=1;                   //Enable pin ESP-12E

    for(;;){                            /* Main Bucle */
        if(vSystemLed==0){
            D3=~D3;
            vSystemLed=500;
        }
        
        ClrWdt();                         //WatchDog Timer
        
        if(U1RxBuffer.status == Ready){
            /* Retransmitir comando AT a UART2 */
            switch(U1RxBuffer.data[0]){
                case 'E':
                    if(U1RxBuffer.data[1]=='0'){
                        LATGbits.LATG2=0;                   //Enable pin ESP-12E
                        D4=0;
                    }
                    if(U1RxBuffer.data[1]=='1'){
                        LATGbits.LATG2=1;                   //Enable pin ESP-12E
                        D4=1;
                    }
                    if(U1RxBuffer.data[1]=='2'){
                        LATDbits.LATD0=0;                   //GPIO15
                        LATDbits.LATD2=0;                   //GPIO0
                        LATDbits.LATD1=1;                   //GPIO2

                    }
                    if(U1RxBuffer.data[1]=='3'){
                        LATDbits.LATD0=0;                   //GPIO15
                        LATDbits.LATD2=1;                   //GPIO0
                        LATDbits.LATD1=1;                   //GPIO2

                    }
                    if(U1RxBuffer.data[1]=='4'){
                        init_uart(2,_76800);                /* Inicializacion UART2 - J9 Pins 2,4 */

                    }
                    if(U1RxBuffer.data[1]=='5'){
                        init_uart(2,_115200);                /* Inicializacion UART2 - J9 Pins 2,4 */
                    }
                    break;
                case 'C':
                    putsUART2("AT\r\n");
                    break;
                default:
                    putsUART(2,&U1RxBuffer.data,U1RxBuffer.bytes_total);
                    break;
            }
            U1RxBuffer.status=Empty;
        }
        /* Buffer de Recepcion UART2 ESP-12E */
        if(U2RxBuffer.status == Ready){
            
            //putsUART(1,&U2RxBuffer.data,U2RxBuffer.bytes_total);
            putsUART1(&U2RxBuffer.data);
            /* Utilizar librerias de parseo de cadenas */
            /* Iniciar el modo AT */
            /* Espera por un mensaje al inicio */
            /* AT, esperar el OK */
            /* AT+RST */
            /* AT+CWMODE=1*/
            /* AT+CWJAP="SES","WirelessSES#0123"*/
            /* Esperar el OK*/

            switch (U2RxBuffer.bytes_total){
                case 7:
                    if(strncmp(&U2RxBuffer.data[0],"\r\nSDK",5)==0){
                       init_uart(2,_115200);                /* Inicializacion UART2 - J9 Pins 2,4 */
                    }
                    break;
                default:
                    break;
            }
//            memset(&string,0,10);
//            vTemp=U2RxBuffer.bytes_total;
//            itoa(&string,vTemp,10);
//            putsUARTx("\nRx: ");
//            putsUART1(&string);
//            putsUARTx(" Bytes\n");
            U2RxBuffer.status = Empty;
        }

        ClrWdt();                         //WatchDog Timer
        if(S3==0){
            vDelay_x10ms=15;
            while(vDelay_x10ms!=0){
                ClrWdt();
            }
            putsUART2("AT\r\n");
        }
        if(S6==0){
            vDelay_x10ms=15;
            while(vDelay_x10ms!=0){
                ClrWdt();
            }
            putsUART2("AT+RST\r\n");
        }
        if(S5==0){
            vDelay_x10ms=15;
            while(vDelay_x10ms!=0){
                ClrWdt();
            }
            putsUART2("AT+GMR\r\n");
        }
        if(S4==0){
            vDelay_x10ms=15;
            while(vDelay_x10ms!=0){
                ClrWdt();
            }
            putsUART2("AT+CIFSR\r\n");
            while(U2RxBuffer.status != Ready){
                ClrWdt();
            }

            //pData = strpbrk(&U2RxBuffer.data,"+CIFSR:STAIP");
            //pData = strstr(&U2RxBuffer.data,"+CIFSR:");
            pData = strchr(&U2RxBuffer.data,'"');
            pData2 = strchr(pData+1,'"');
            if(pData!=0 && pData2!=0){
                putsUART1("IP: ");
                putsUART(1,pData+1,pData2-pData-1);
                putsUART1("\r\n");

                XLCDL1home();
                memset(&string,0,20);
                memcpy(&string,pData+1,pData2-pData-1);
                XLCDPutRomString(&string);

            }else{
                putsUART1("Puntero Nulo\r\n");
            }
            pData = strchr(pData2+1,'"');
            pData2 = strchr(pData+1,'"');
            if(pData!=0 && pData2!=0){
                putsUART1("MAC: ");
                putsUART(1,pData+1,pData2-pData-1);
                putsUART1("\r\n");
                XLCDL2home();
                memset(&string,0,20);
                memcpy(&string,pData+1,pData2-pData-1);
                XLCDPutRomString(&string);
            }else{
                putsUART1("Puntero Nulo\r\n");
            }
            U2RxBuffer.status = Empty;
        }
        ClrWdt();                         //WatchDog Timer
    }
    asm("RESET");
}

//{cantidad:1,trama1:T00123456789111112350000000000011512042123460000000000}
//{cantidad:1,trama1:T00123456789111112370000000000011512042123460000000000}
//{cantidad:1,trama1:T00123456789111112370000000000011512042123481000000000}