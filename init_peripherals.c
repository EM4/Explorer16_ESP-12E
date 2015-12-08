#include <stdlib.h>
#include <p24Fxxxx.h>
#include <PPS.h>
#include "explorer16.h"

//rutina de inicio de perifericos
void
init_peripherals(void){

    /* SPI pins */
//    iPPSInput(IN_FN_PPS_SDI1,IN_PIN_PPS_RPI44);
//    iPPSOutput(OUT_PIN_PPS_RP15,OUT_FN_PPS_SDO1);
//    iPPSOutput(OUT_PIN_PPS_RP31,OUT_FN_PPS_SCK1OUT);

    /* Uart 1 pins*/
    iPPSOutput(OUT_PIN_PPS_RP17,OUT_FN_PPS_U1TX);
    iPPSInput(IN_FN_PPS_U1RX,IN_PIN_PPS_RP10);

    /* Uart 2 pins */
    LATBbits.LATB14=0;
    iPPSOutput(OUT_PIN_PPS_RP30,OUT_FN_PPS_U2TX);
    iPPSInput(IN_FN_PPS_U2RX,IN_PIN_PPS_RP16);

//    /* Uart 3 pins */
//    iPPSOutput(OUT_PIN_PPS_RP16,OUT_FN_PPS_U3TX);
//    iPPSInput(IN_FN_PPS_U3RX,IN_PIN_PPS_RP30);

    /* Apagamos modulos que no se usan */
    PMD1bits.ADC1MD=1;

    PMD1bits.I2C1MD=1;
    PMD3bits.I2C2MD=1;
    PMD3bits.I2C3MD=1;

    PMD1bits.SPI2MD=1;
    PMD6bits.SPI3MD=1;

    PMD2bits.OC1MD=1;
    PMD2bits.OC2MD=1;
    PMD2bits.OC3MD=1;
    PMD2bits.OC4MD=1;
    PMD2bits.OC5MD=1;
    PMD2bits.OC6MD=1;
    PMD2bits.OC7MD=1;
    PMD2bits.OC8MD=1;
    PMD5bits.OC9MD=1;

    PMD3bits.PMPMD=1;
    PMD3bits.RTCCMD=0;
    PMD3bits.CMPMD=1;
    PMD3bits.CRCPMD=1;
    PMD3bits.CRCMD=1;
    PMD4bits.CTMUMD=1;
    PMD4bits.REFOMD=1;
    PMD4bits.LVDMD=1;
    PMD5bits.IC9MD=1;

}

