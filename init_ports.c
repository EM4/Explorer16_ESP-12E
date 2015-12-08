#include <stdlib.h>
#include <ports.h>
#include "explorer16.h"


void init_ports(void){


    mPORTACloseAll();
    AD1PCFG = 0xffff;
    mPORTAConfig(0x0080);  //RA[15..0] outputs
    mPORTAWrite(0);

    //Config PORTB
    mPORTBCloseAll();
    mPORTBConfig(0x00C0);   //RB6,RB7 as inputs
    mPORTBWrite(0);

    //Config PORTC
    mPORTCCloseAll();
    mPORTCConfig(0x9000);   //RC15,RC12 as inputs
    mPORTCWrite(0);


    //Config PORTD
    mPORTDCloseAll();
    mPORTDConfig(0x20C0);
    mPORTDWrite(0);

    //Config PORTE
    mPORTECloseAll();
    mPORTEConfig(0x0000);   //LCD Data Port
    mPORTEWrite(0);

    //Config PORTF
    mPORTFCloseAll();
    mPORTFConfig(0x0018);   //RF2 as input
    mPORTFWrite(0);

    //Config PORTG
    mPORTGCloseAll();
    mPORTGConfig(0x0000); //All portG unused
    mPORTGWrite(0);

}
