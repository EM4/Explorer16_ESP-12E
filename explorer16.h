/* 
 * File:   explorer16.h
 * Author: emontes
 *
 * Created on November 25, 2014, 2:23 PM
 */

#ifndef EXPLORER16_H
#define	EXPLORER16_H

#ifdef	__cplusplus
extern "C" {
#endif

#define BUFFER_SIZE 500

/* Buffer status */
typedef enum{
    Empty,
    Ready,
    Receiving    
} buffer_status;

/* Rx Buffer struct */
typedef struct{
    buffer_status status;
    unsigned int bytes_ok;
    unsigned int bytes_err;
    unsigned int bytes_total;
    unsigned char data[BUFFER_SIZE];
} rx_buffer;

/* Transmit buffer struct */
typedef struct{
    unsigned int bytes;
    unsigned char data[BUFFER_SIZE];
} tx_buffer;

//funciones
void delay(void);
void init_ports(void);
void init_peripherals(void);
void init_system_clock(void);
void init_vars(void);
int init_uart(int uart, unsigned int ubrg);
void putsUART(unsigned int uart, const unsigned int *pU16data, unsigned int len);
void putcUART(unsigned int uart, unsigned int data);
void HexToAscciString(unsigned char *pHex,unsigned char *pAscii, unsigned int size);
void BcdToAsciiString(unsigned char *pBcd,unsigned char *pAscii, unsigned int size);

//Interrupts defines
//#define _ISRFAST __attribute__((interrupt,shadow))
#define _ISRAPSV __attribute__((interrupt,auto_psv))
#define _ISRNPSV __attribute__((interrupt,no_auto_psv))

/* switchs */
#define S3 PORTDbits.RD6
#define S4 PORTDbits.RD13
#define S5 PORTAbits.RA7
#define S6 PORTDbits.RD7
/* leds */
#define D3 LATAbits.LATA0
#define D4 LATAbits.LATA1
#define D5 LATAbits.LATA2
#define D6 LATAbits.LATA3
#define D7 LATAbits.LATA4
#define D8 LATAbits.LATA5
#define D9 LATAbits.LATA6
#define D10 LATAbits.LATA7
#define _230400         16
#define _76800          51
#define _115200         34
#define _57600          68
#define _38400          103
#define _19200          207
#define _9600           416

#define putsUARTx(s)    putsUART1((unsigned int*)s)//Arreglo para que no presente warnings
#define putcUARTx       putcUART1
#define BusyUARTx       BusyUART1


/* Estructruas de almacenamiento de datos */
extern rx_buffer U1RxBuffer;
extern tx_buffer U1TxBuffer;
extern rx_buffer U2RxBuffer;
extern tx_buffer U2TxBuffer;
extern rx_buffer U3RxBuffer;

extern unsigned char *pUart1;
extern unsigned char *pUart2;
extern unsigned char *pUart3;

extern unsigned int vDec;
extern unsigned int vSeg;
extern unsigned int vDelay;
extern unsigned int vSystemLed;
extern unsigned int vDelay_x10ms;
extern unsigned int vDelay_x1ms;
extern unsigned int vDelay_x1s;
extern unsigned int vRFID;
extern unsigned int ESP8266_Counter;
extern unsigned int vTemp;
/* Interrupt Declarations */
void _ISRNPSV _U1RXInterrupt(void);      //interrupciones por el puerto de maquina
void _ISRNPSV _U2RXInterrupt(void);      //interrupciones por el puerto de sistema online
void _ISRNPSV _U3RXInterrupt(void);      //interrupciones por el puerto de debug
void _ISRNPSV _T1Interrupt(void);        //interrupcion por timer1
void _ISRNPSV _T2Interrupt(void);        //interrupcion por timer2
void _ISRNPSV _T3Interrupt(void);        //interrupcion por timer2
void _ISRNPSV _T5Interrupt(void);        //interrupcion por timer5
void _ISRNPSV _SPI1Interrupt(void);      //interrupcion por SPI

#ifdef	__cplusplus
}
#endif

#endif	/* EXPLORER16_H */

