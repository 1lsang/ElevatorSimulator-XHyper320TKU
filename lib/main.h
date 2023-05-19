//-------| lib/main.h |-------//
#ifndef	MAIN_H
#define	MAIN_H 


#include <stdio.h>		// printf(), scanf(), fprintf()
#include <fcntl.h>		// open()
#include <unistd.h>		// close(), off_t , usleep()
#include <sys/mman.h>	// mmap(), munmap()
#include <stdlib.h> 	// exit()
// #include <string.h>		// strlen()
#include <pthread.h>

#include "ieb.h"
#include "led.h"
#include "dot.h"
#include "fnd.h"
#include "clcd.h"
#include "keypad.h"


#define MAX_FLOOR 14
// typedef struct {
//   int *data;
//   int size;
//   int capacity;
//   pthread_mutex_t mutex;
// } inputs_t;


typedef enum {
	FALSE = 0,
	TRUE  = 1
} truth_t;

typedef enum {
	ERROR	= -1,
	SUCCESS	= 0
} error_t;

typedef union {
	unsigned char all;
	struct {
		unsigned char exit : 1;
	};
} seclection_t;

int main();
short * mapper(off_t offset, int prot);
void unmapper();
void emergency_closer();

void *input();
void input_mode();
truth_t input_logic();


void *elevator();
void move_elevator();


void open_door();
void close_door();
void move_up();
void move_down();
void *move_to();
int check();
int get_target();
truth_t moving();

void close_door();
void open_door();
int get_target();

// void *move(void *arg);
// void inputs_init(inputs_t *inputs, int capacity);
// void inputs_push(inputs_t *inputs, int value);
// int inputs_pop(inputs_t *inputs);
// int inputs_front(inputs_t *inputs);
// truth_t inputs_empty(inputs_t *inputs);
// void inputs_free(inputs_t *inputs);

#endif
