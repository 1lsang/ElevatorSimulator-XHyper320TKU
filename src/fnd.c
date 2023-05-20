//-------| src/fnd.c |-------//
#include "fnd.h"

static unsigned short fnd_decimal[10] = {
	0x3F, // 0
	0x06, // 1
	0x5B, // 2
	0x4F, // 3
	0x66, // 4
	0x6D, // 5
	0x7D, // 6
	0x07, // 7
	0x7F, // 8
	0x67 // 9
};


static short * fnd[MAX_FND];

void init_fnd(short * address[],int startFloor) {
	int i;
	for( i=0; i<MAX_FND; i++ ) {
		fnd[i] = address[i];
	}
	fnd_clear();
	fnd_number(startFloor);
}

void fnd_clear() {
	int i;
	for(i=0; i<MAX_FND; i++){
		*fnd[i] = 0;
	}
}

void fnd_all() {
	int i;
	for(i=0; i<MAX_FND; i++){
		*fnd[i] = 0x01;
	}
}

void fnd_write(int decimal, int fnd_num) {
	*fnd[fnd_num] = fnd_decimal[decimal];
}

void fnd_number(unsigned long number) {
	number++;
	int number1, number2;
	number1 = number / 10;
	number2 = number % 10;
	fnd_write((int)number2, 3);
	fnd_write((int)number1, 4);
	usleep(0);
}
