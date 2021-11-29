/*
 * HexToAsc.c
 *
 *  Created on: 11 Sep 2020
 *      Author: Admin
 */
#include "HexToAsc.h"

/*
 * Hex to int and hex to ascii routines to convert hex variables to ascii if required, eg. 0x35 to 5 in int or "5" ascii
 */
int hex_to_int(char c){
        int first = c / 16 - 3;
        int second = c % 16;
        int result = first*10 + second;
        if(result > 9) result--;
        return result;
}

int hex_to_ascii(char *data){
		char c = 0xF0 & *data;
		char d = 0x0F & *data;
        int high = hex_to_int(c) * 16;
        int low = hex_to_int(d);
        return high+low;
}
