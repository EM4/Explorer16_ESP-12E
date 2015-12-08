#include <uart.h>
#include <timer.h>
#include <stdlib.h>
#include "explorer16.h"

int
init_uart(int uart, unsigned int ubrg) {
    switch (uart) {
        case 1:
            //Configuracion del UART para comunicacion con máquina
            ConfigIntUART1(UART_RX_INT_EN & UART_RX_INT_PR6 & //Configura Interrupcion RX UART1
                    UART_TX_INT_DIS & UART_TX_INT_PR0); //Rx prioridad 6
            OpenUART1(//UxMODE Register
                    UART_EN & UART_IDLE_STOP & UART_IrDA_DISABLE & UART_MODE_FLOW &
                    UART_UEN_00 & UART_DIS_WAKE & UART_DIS_LOOPBACK & UART_DIS_ABAUD &
                    UART_UXRX_IDLE_ONE & UART_NO_PAR_8BIT & UART_BRGH_FOUR &
                    UART_1STOPBIT,
                    //UxSTA Register
                    UART_INT_TX_BUF_EMPTY & UART_IrDA_POL_INV_ZERO &
                    UART_SYNC_BREAK_DISABLED & UART_TX_ENABLE &
                    UART_INT_RX_CHAR & UART_ADR_DETECT_DIS,
                    //UxBRG Register
                    ubrg);
            U1RX_Clear_Intr_Status_Bit; //Limpio el flag de la interrupcion
            U1TX_Clear_Intr_Status_Bit; //Limpio el flag de la interrupcion
            /* Configura el delay interbyte para recepción de tramas */
            // T(seg)=PR*PRESC*Tcy -->Tcy=1/Fosc, con pll es Tcy=1/(Fosc/2), Fosc con pll es 32Mhz
            ConfigIntTimer1(T1_INT_PRIOR_1 & T1_INT_ON); //configura las interrupciones por timer1
            WriteTimer1(0);
            OpenTimer1( T1_ON & T1_IDLE_CON & T1_GATE_OFF &
                        T1_PS_1_256 & T1_SOURCE_INT,69); //1.104ms-->16 bytes delay
            T1CONbits.TON = 0; //Timer 1 pagado
            T1_Clear_Intr_Status_Bit;
            break;
        case 2:
            ConfigIntUART2(UART_RX_INT_EN & UART_RX_INT_PR4 & //Configura Interrupcion RX UART2
                    UART_TX_INT_DIS & UART_TX_INT_PR0); //Rx prioridad 7
            OpenUART2(//UxMODE Register
                    UART_EN & UART_IDLE_CON & UART_IrDA_DISABLE & UART_MODE_FLOW &
                    UART_UEN_00 & UART_DIS_WAKE & UART_DIS_LOOPBACK & UART_DIS_ABAUD &
                    UART_UXRX_IDLE_ONE & UART_NO_PAR_8BIT & UART_BRGH_FOUR & UART_1STOPBIT,
                    //UxSTA Register
                    UART_INT_TX_BUF_EMPTY & UART_IrDA_POL_INV_ZERO &
                    UART_SYNC_BREAK_DISABLED & UART_TX_ENABLE &
                    UART_INT_RX_CHAR & UART_ADR_DETECT_DIS,
                    //UxBRG Register
                    ubrg);
            U2RX_Clear_Intr_Status_Bit; //Limpio el flag de la interrupcion
            U2TX_Clear_Intr_Status_Bit; //Limpio el flag de la interrupcion
            ConfigIntTimer2(T2_INT_PRIOR_1 & T2_INT_ON); //configura las interrupciones por timer2
            WriteTimer2(0);
            OpenTimer2( T2_ON & T2_IDLE_CON & T2_GATE_OFF &
                        T2_PS_1_256 & T2_32BIT_MODE_OFF & T2_SOURCE_INT,313); //313<=>5.008ms->sas interbyte delay
            T2CONbits.TON = 0; //Timer 2 pagado
            T2_Clear_Intr_Status_Bit;
            break;
        case 3:
            ConfigIntUART3( UART_RX_INT_EN & UART_RX_INT_PR2 & //Configura Interrupcion RX UART2
                            UART_TX_INT_DIS & UART_TX_INT_PR0); //Rx prioridad 7
            OpenUART3(//UxMODE Register
                    UART_EN & UART_IDLE_CON & UART_IrDA_DISABLE & UART_MODE_FLOW &
                    UART_UEN_00 & UART_DIS_WAKE & UART_DIS_LOOPBACK & UART_DIS_ABAUD &
                    UART_UXRX_IDLE_ONE & UART_NO_PAR_8BIT & UART_BRGH_FOUR & UART_1STOPBIT,
                    //UxSTA Register
                    UART_INT_TX_BUF_EMPTY & UART_IrDA_POL_INV_ZERO &
                    UART_SYNC_BREAK_DISABLED & UART_TX_ENABLE &
                    UART_INT_RX_CHAR & UART_ADR_DETECT_DIS,
                    //UxBRG Register
                    ubrg);
            U3RX_Clear_Intr_Status_Bit; //Limpio el flag de la interrupcion
            U3TX_Clear_Intr_Status_Bit; //Limpio el flag de la interrupcion
            ConfigIntTimer3(T3_INT_PRIOR_1 & T3_INT_ON); //configura las interrupciones por timer3
            WriteTimer3(0);
            OpenTimer3( T3_ON & T3_IDLE_CON & T3_GATE_OFF &
                        T3_PS_1_256 & T3_SOURCE_INT,69); //1.104ms->16 bytes delay
            T3CONbits.TON = 0; //Timer 3 pagado
            T3_Clear_Intr_Status_Bit;
            break;
        case 4:
            OpenUART4(//UxMODE Register
                    UART_EN & UART_IDLE_STOP & UART_IrDA_DISABLE & UART_MODE_FLOW &
                    UART_UEN_00 & UART_DIS_WAKE & UART_DIS_LOOPBACK & UART_DIS_ABAUD &
                    UART_UXRX_IDLE_ONE & UART_NO_PAR_8BIT & UART_BRGH_FOUR &
                    UART_1STOPBIT,
                    //UxSTA Register
                    UART_INT_TX_BUF_EMPTY & UART_IrDA_POL_INV_ZERO &
                    UART_SYNC_BREAK_DISABLED & UART_TX_ENABLE &
                    UART_INT_RX_CHAR & UART_ADR_DETECT_DIS,
                    //UxBRG Register
                    ubrg);
            break;
        default:
            return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
}

//rutina para envio de cadenas sin caracter null

//putsUART(unsigned int uart, unsigned char *ptr, unsigned int length)
void putsUART(unsigned int uart, const unsigned int *pU16data, unsigned int len){
    unsigned int i;
    const unsigned char* pU8data = (unsigned char*)pU16data;
    switch (uart) {
        case 1:
            for(i=0; i<len; i++) {
                while (U1STAbits.UTXBF);    /* wait if the buffer is full */
                U1TXREG = pU8data[i];       /* transfer data byte to TX reg for 9bits */
            }
            break;
            break;
        case 2:
            for(i=0; i<len; i++) {
                while (U2STAbits.UTXBF);    /* wait if the buffer is full */
                U2TXREG = pU8data[i];       /* transfer data byte to TX reg for 8bits */
            }
            break;
        case 3:
            for(i=0; i<len; i++) {
                while (U3STAbits.UTXBF);    /* wait if the buffer is full */
                U3TXREG = pU8data[i];       /* transfer data byte to TX reg for 8bits */
            }
            break;
        case 4:
            for(i=0; i<len; i++) {
                while (U4STAbits.UTXBF);    /* wait if the buffer is full */
                U4TXREG = pU8data[i];       /* transfer data byte to TX reg for 8bits */
            }
            break;
        default:
            break;
    }
}

//rutina para envio de datos por un UART

void putcUART(unsigned int uart, unsigned int data)
{
    switch (uart) {
        case 1:
            while (U1STAbits.UTXBF);    /* wait if the buffer is full */
            WriteUART1(data);           /* write data byte for 8 or 9bits */
            break;
        case 2:
            while (U2STAbits.UTXBF);    /* wait if the buffer is full */
            WriteUART2(data);           /* write data byte for 8 or 9bits */
            break;
        case 3:
            while (U3STAbits.UTXBF);    /* wait if the buffer is full */
            WriteUART3(data);           /* write data byte for 8 or 9bits */
            break;
        case 4:
            while (U4STAbits.UTXBF);    /* wait if the buffer is full */
            WriteUART4(data);           /* write data byte for 8 or 9bits */
            break;
        default:
            asm("RESET");
            break;
    }
}

