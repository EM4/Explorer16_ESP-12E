
#include <stdlib.h>
#include <ports.h>
#include "explorer16.h"

void delay(void) {
    long i = 65535;
    while(i--)
        ;
}
