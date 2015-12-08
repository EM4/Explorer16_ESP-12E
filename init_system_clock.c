
#include <timer.h>
#include <stdlib.h>

int
init_system_clock(void){
    ConfigIntTimer5(T5_INT_PRIOR_1 & T5_INT_ON);        //configura las interrupciones por timer1
    WriteTimer5(0);
    OpenTimer5( T5_ON & T5_IDLE_STOP & T5_GATE_OFF &
                T5_PS_1_256 & T5_SOURCE_INT,63);      //0.01s ó 10ms
    return(EXIT_SUCCESS);
}