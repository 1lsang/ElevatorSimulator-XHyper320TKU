//-------| src/dot.c |-------//
#include "dot.h"


#include <unistd.h> // usleep()

#define MAX_DOT 5

static unsigned short dot_door[4][MAX_DOT] = {
    {0x7F, 0x7F, 0x7F, 0x7F, 0x7F}, // 문 닫힘
    {0x7F, 0x7F, 0x00, 0x7F, 0x7F}, // 문 열리는 중 1
    {0x7F, 0x00, 0x00, 0x00, 0x7F}, // 문 열리는 중 2
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 문 열림
};


static short * dot[MAX_DOT];

void init_dot(short * address[]) {
	int i;
	for( i=0; i<MAX_DOT; i++ ) {
		dot[i] = address[i];
	}
}

void dot_clear() {
	int i;
	for(i=0; i<MAX_DOT; i++){
		*dot[i] = 0;
	}
	usleep(0); // for Ximulator
}

void dot_write(int number) {
	int i;
	for(i=0; i<MAX_DOT; i++){
		*dot[i] = dot_door[number][i];
	}
	usleep(0); // for Ximulator
}

void dot_open_door() {
	int i;
	for(i=0;i<4;i++) {//3초에 걸쳐서 문 열림
		usleep(1000000);
		dot_write(i);
		//문열림 표시
	}//도트 문 열리는 로직
}

void dot_close_door() {
	int i;
	for(i=3;i>=0;i--) {//3초에 걸쳐서 문 닫힘
		usleep(1000000);
		dot_write(i);
		//문닫힘 표시
	}//도트 문 닫히는 로직
}