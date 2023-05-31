//-------| lib/elevator.h |-------//
#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <stdlib.h>
#include <pthread.h>
#include "led.h"
#include "dot.h"
#include "fnd.h"
#include "clcd.h"

#define MAX_FLOOR 14

typedef enum {
	false = 0,
	true  = 1
} bool_t;


void init_elevator(pthread_mutex_t *e_mutex);
void set_elevator_floor(int floor);
int get_elevator_floor();
void set_max_floor(int floor);
void set_time_cnt(int t);
void press_button(int key_value);
void exit_elevator();
void exit_program();
void *elevator();
void move_elevator();
bool_t moving();
void close_door();
void open_door();
int get_target();

#endif