//-------| lib/elevator.h |-------//
#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <pthread.h>
// #include "led.h"
// #include "dot.h"
// #include "fnd.h"
// #include "clcd.h"
#include "main.h"

#define MAX_FLOOR 14

void init_elevator();
void set_elevator_floor(int floor);
void set_max_floor(int floor);
void *elevator();
void move_elevator();
truth_t moving();
void close_door();
void open_door();
int get_target();

#endif ELEVATOR_H