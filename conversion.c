#include <stdlib.h>
#include <string.h>
#include "explorer16.h"

void HexToAsciiString(unsigned char *pHex,unsigned char *pAscii, unsigned int size){
    unsigned int i;
    unsigned char vNible;
    for(i=0;i<size;i++){
        //Upper Nibble
        vNible = ((*pHex) & 0xF0)>>4;
        if(vNible>9){
            vNible = vNible-0x0A;
            vNible = vNible+0x41;   //'A'
        }else{
            vNible = vNible+0x30;
        }
        *pAscii++=vNible;
        //Lower Nibble
        vNible = ((*pHex)&0x0F);
        if(vNible>9){
            vNible = vNible-0x0A;
            vNible = vNible+0x41;   //'A'
        }else{
            vNible = vNible+0x30;
        }
        *pAscii++=vNible;
        pHex++;
    }
}
/**
 * @brief Rutina de conversión de una cadena en BCD a Ascii
 * @param pAscii Puntero de destino de la cadena a convertir
 * @param pBcd Puntero de origen de la cadena a convertir
 * @size Longitud de la cadena
 */
void BcdToAsciiString(unsigned char *pBcd,unsigned char *pAscii, unsigned int size){
    unsigned int i;
    unsigned char vNible;
    for(i=0;i<size;i++){
        vNible = (((*pBcd) & 0xF0)>>4)|0x30;
        *pAscii++=vNible;
        vNible = ((*pBcd)&0x0F)|0x30;
        *pAscii++=vNible;
        pBcd++;
    }
}