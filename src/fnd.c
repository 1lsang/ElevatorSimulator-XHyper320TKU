//-------| src/fnd.c |-------//
#include "fnd.h"

static unsigned short fnd_decimal[10] = {
	0x38, // 0
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
		*fnd[i] = 0xFF;
	}
}

void fnd_write(int decimal, int fnd_num) {
	*fnd[fnd_num] = fnd_decimal[decimal];
}

void fnd_number(unsigned long number) {
	int i;
	for(i=0; i<MAX_FND; i++){
		fnd_write( (short)(number & 0xF), i);
		number = number >> 4;
	}
}
