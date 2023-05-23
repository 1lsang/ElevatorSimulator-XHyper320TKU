//-------| src/elevator.c |-------//
#include "elevator.h"


static pthread_mutex_t mutex;
// 엘리베이터의 현재 층수
static int elevator_floor = 0;
// 엘리베이터의 최대 층수
static int max_floor = MAX_FLOOR;
// 엘리베이터의 현재 방향
static int elevator_direction = 1; // 1 = 위, -1 = 아래, 0 = 정지
// 엘리베이터의 다음 목적지
static int next_floor = 0;
// 프로그램 종료 여부
static int exitProgram = 0; 
// 엘리베이터 문 여닫이 시간 카운트
static int time_cnt = 0;
// 엘리베이터 버튼 눌림 여부
static int *pressed_button;


void init_elevator(pthread_mutex_t *e_mutex) {
    mutex = *e_mutex;
    // 눌린 버튼을 저장하는 배열 생성
	pressed_button = (int *)malloc(sizeof(int) * max_floor);
}

void set_elevator_floor(int floor) {
    elevator_floor = floor;
}

int get_elevator_floor() {
	return elevator_floor;
}

void set_max_floor(int floor) {
    max_floor = floor;
}

void press_button(int key_value) {
	pthread_mutex_lock(&mutex);
	pressed_button[key_value] = 1;
	pthread_mutex_unlock(&mutex);
}

void exit_elevator() {
	exitProgram = 1;
}

void exit_program() {
	// 눌린 버튼을 저장하는 배열 해제
	free(pressed_button);
}

void set_time_cnt(int t) {
	pthread_mutex_lock(&mutex);
	time_cnt=t;
	pthread_mutex_unlock(&mutex);
}

void *elevator() {
	while(exitProgram != 1) {
		if (moving()==true && elevator_floor==next_floor && elevator_direction!=0) {
			// 엘리베이터가 도착했을 때
			// 엘리베이터 문을 열고 닫는다.
			// 시간 초기화
			open_door();
			pthread_mutex_lock(&mutex);
			time_cnt=0;
			pthread_mutex_unlock(&mutex);
			while (time_cnt<=8)
				{
					usleep(200000);
					pthread_mutex_lock(&mutex);
					time_cnt++;
					pthread_mutex_unlock(&mutex);
					led_time(time_cnt);
				}
			close_door();
			
			pthread_mutex_lock(&mutex);
			pressed_button[elevator_floor] = 0;
			pthread_mutex_unlock(&mutex);
			
		}
		move_elevator();
	
	}
	return NULL;
}

void move_elevator() {
	if (moving()==true) {
		int i;
		next_floor = get_target();
		if (elevator_floor < next_floor) {
			// 엘리베이터가 올라갈 때
			elevator_direction = 1;
			clcd_moving_up();
			for (i = 0; i < 2; i++)
			{
				led_up_shift();	 // led shift
				usleep(5000);
			}
			elevator_floor++;
		} else if (elevator_floor > next_floor) {
			// 엘리베이터가 내려갈 때
			elevator_direction = -1;
			clcd_moving_down();
			for (i = 0; i < 2; i++)
			{
				led_down_shift();	 // led shift
				usleep(5000);
			}
			elevator_floor--;
		}
		fnd_number(elevator_floor);
		usleep(0);
	}
}

void open_door() {
	// 문이 열립니다.
	clcd_door_open(elevator_floor);
	led_all();
	dot_open_door();
	
}

void close_door() {
	// 문이 닫힙니다.
	clcd_door_close();
	led_clear();
	dot_close_door();
	clcd_press_button();
}

bool_t moving() {
	int i;
	for (i=0; i<max_floor; i++) {
		if (pressed_button[i]==1) {
			return true;
		}
	}
	elevator_direction = 0;
	return false;
}

int get_target() {
	int i;
	if (elevator_direction==1) {
		for(i=elevator_floor; i<max_floor; i++) {
			if (pressed_button[i]==1) {
				return i;
			}
		}
	}
	else if (elevator_direction==-1) {
		for (i=elevator_floor; i>=0; i--) {
			if (pressed_button[i]==1) {
				return i;
			}
		}
	}
	else {
		for (i=0; i<max_floor; i++) {
			if (pressed_button[i]==1) {
				return i;
			}
		}
	}
	elevator_direction = 0;
	return elevator_floor;
}
