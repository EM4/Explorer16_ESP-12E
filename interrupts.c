#include "explorer16.h"
#include <timer.h>
#include <uart.h>

/* Estructruas de almacenamiento de datos */
rx_buffer U1RxBuffer;
tx_buffer U1TxBuffer;
rx_buffer U2RxBuffer;
tx_buffer U2TxBuffer;
rx_buffer U3RxBuffer;
unsigned char *pUart1;
unsigned char *pUart2;
unsigned char *pUart3;
unsigned int vDec;
unsigned int vSeg;

/* Rutina de interrupcion por puerto de Maquina */
void _ISRNPSV _U1RXInterrupt(void){
    unsigned char udata;

    udata= (unsigned char) ReadUART1();     //leo el dato
    /* Inicializa la recepcion si el buffer esta disponible */
    if(U1RxBuffer.status == Empty){
        U1RxBuffer.bytes_ok=0;               //reinicio contador
        U1RxBuffer.bytes_err=0;              //reinicio contador
        U1RxBuffer.bytes_total=0;            //inicio contador total
        pUart1=&U1RxBuffer.data[0];          //inicializo puntero de rx
        U1RxBuffer.status = Receiving;       //cambio el estado de rx
        WriteTimer1(0);
        T1CONbits.TON = 1;                  //inicio timer
    }
    /* Almacenar datos solo si está en estado de recepcion */
    if(U1RxBuffer.status==Receiving){
        if(!U1STAbits.OERR && !U1STAbits.FERR){
            //Llego bien
            *pUart1++=udata;
            U1RxBuffer.bytes_ok++;
        }else{
            //Llego mal
            *pUart1++=udata;
            U1RxBuffer.bytes_err++;
        }
        U1RxBuffer.bytes_total++;
    }
    /* Revision de buffer lleno */
    if(U1RxBuffer.bytes_total>=BUFFER_SIZE-1){
        T1CONbits.TON=0;
        U1RxBuffer.status = Ready;
    }
    WriteTimer1(0);                     //reinica el timer
    U1RX_Clear_Intr_Status_Bit;         //Limpio el flag de la interrupcion
}

/* timeout para recepcion de datos de maquina */
void _ISRNPSV _T1Interrupt(void)
{

    T1CONbits.TON=0;
    WriteTimer1(0);
    U1RxBuffer.status = Ready;
    T1_Clear_Intr_Status_Bit;
}

/* Rutina de interrupcion por puerto de Red */
void _ISRNPSV _U2RXInterrupt(void){
    unsigned char udata;
    udata= (unsigned char) ReadUART2();     //leo el dato
    if(U2RxBuffer.status == Empty){
        U2RxBuffer.bytes_ok=0;               //reinicio contador
        U2RxBuffer.bytes_err=0;              //reinicio contador
        U2RxBuffer.bytes_total=0;            //inicio contador total
        pUart2=&U2RxBuffer.data[0];          //inicializo puntero de rx
        U2RxBuffer.status = Receiving;       //cambio el estado de rx
        WriteTimer2(0);
        T2CONbits.TON = 1;                  //inicio timer
    }
    if(U2RxBuffer.status == Receiving){
        if(!U2STAbits.OERR && !U2STAbits.FERR){
        //Llego bien
        *pUart2++=udata;
            U2RxBuffer.bytes_ok++;
        }else{
            //Llego mal
            *pUart2++=udata;
            U2RxBuffer.bytes_err++;
        }
        U2RxBuffer.bytes_total++;
    }
    /* Revision de buffer lleno */
    if(U2RxBuffer.bytes_total>=BUFFER_SIZE-1){
        T2CONbits.TON=0;
        U2RxBuffer.data[U2RxBuffer.bytes_total]=0x00; //Caracter Null
        U2RxBuffer.status = Ready;
    }
    WriteTimer2(0);                     //reinica el timer
    U2RX_Clear_Intr_Status_Bit;         //Limpio el flag de la interrupcion
}

/* timeout para recepcion de datos de red */
void _ISRNPSV _T2Interrupt(void)        // control de temporizaciones cada 100ms
{
    T2CONbits.TON=0;                    //Apago el timer
    WriteTimer2(0);                     //Reset del timer
    U2RxBuffer.data[U2RxBuffer.bytes_total]=0x00; //Caracter Null
    U2RxBuffer.status = Ready;          //Indico que se termino de recibir datos
    T2_Clear_Intr_Status_Bit; 		//clear interrupt flag
}

/* Rutina de interrupcion por puerto de Debug */
void _ISRNPSV _U3RXInterrupt(void){
    unsigned char udata;
    udata= (unsigned char) ReadUART3();     //leo el dato
    if(U3RxBuffer.status == Empty){
        U3RxBuffer.bytes_ok=0;               //reinicio contador
        U3RxBuffer.bytes_err=0;              //reinicio contador
        U3RxBuffer.bytes_total=0;            //inicio contador total
        pUart3=&U3RxBuffer.data[0];          //inicializo puntero de rx
        U3RxBuffer.status = Receiving;       //cambio el estado de rx
        WriteTimer3(0);
        T3CONbits.TON = 1;                  //inicio timer
    }
    if(!U3STAbits.OERR && !U3STAbits.FERR){
        //Llego bien
        *pUart3++=udata;
        U3RxBuffer.bytes_ok++;
    }else{
        //Llego mal
        *pUart3++=udata;
        U3RxBuffer.bytes_err++;
    }
    U3RxBuffer.bytes_total++;
    WriteTimer3(0);                     //reinica el timer
    U3RX_Clear_Intr_Status_Bit;         //Limpio el flag de la interrupcion
}

/* timeout para recepcion de datos de red */
void _ISRNPSV _T3Interrupt(void)        // control de temporizaciones cada 100ms
{
    T3CONbits.TON=0;                    //Apago el timer
    WriteTimer3(0);                     //Reset del timer
    U3RxBuffer.status = Ready;         //Indico que se termino de recibir datos
    T3_Clear_Intr_Status_Bit; 		//clear interrupt flag
}

/* temporizador del sistema 1ms */
void _ISRNPSV _T5Interrupt(void)        // control de temporizaciones cada 1ms
{
    vDec--;
    if(vDec==0){
        vDec=10;
        /* Variables para temporizacion cada 10ms */
        if(vDelay_x10ms!=0) vDelay_x10ms--;
        vSeg--;                                         //Control de tiempo cada 1seg
        if(vSeg==0){
            vSeg=100;
            /* Variables para timeout por segundo */
            if(vDelay_x1s!=0) vDelay_x1s--;
        }
    };
    /* variables de temporizacion cada 1ms*/
    if(vDelay_x1ms!=0) vDelay_x1ms--;
    if(vSystemLed!=0) vSystemLed--;
    T5_Clear_Intr_Status_Bit; 		//clear interrupt flag
}
