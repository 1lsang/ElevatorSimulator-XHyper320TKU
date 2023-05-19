//-------| src/main.c |-------//
#include "main.h"

// DOT MATRIX CODE
static off_t IEB_DOT[MAX_DOT] = {
	IEB_DOT1,
	IEB_DOT2,
	IEB_DOT3,
	IEB_DOT4,
	IEB_DOT5
};

// FND CODE
static off_t IEB_FND[MAX_FND] = {
	IEB_FND0,
	IEB_FND1,
	IEB_FND2,
	IEB_FND3,
	IEB_FND4,
	IEB_FND5,
	IEB_FND6,
	IEB_FND7
};

// Mapping variables
static int fd;
static int map_counter = 0;
static void * map_data[100];

// Mutex for pthread
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

int main(int argc, char* argv[]) {
	int i;
	short * led, * dot[MAX_DOT], * fnd[MAX_FND];
	short * clcd_cmd, * clcd_data, * keypad_out, * keypad_in;

	fd = open("/dev/mem", O_RDWR|O_SYNC);
	if (fd == -1) {
		fprintf(stderr, "Cannot open /dev/mem file");
		exit(EXIT_FAILURE);
	}

	// LED Mapping
	led = mapper(IEB_LED, PROT_WRITE);

	// DOT MATRIX Mapping
	for( i=0; i<MAX_DOT; i++ ) {
		dot[i] = mapper(IEB_DOT[i], PROT_WRITE);
	}

	// FND Mapping
	for( i=0; i<MAX_FND; i++ ) {
		fnd[i] = mapper(IEB_FND[i], PROT_WRITE);
	}

	// CLCD Mapping
	clcd_cmd  = mapper(IEB_CLCD_CMD, PROT_WRITE);
	clcd_data = mapper(IEB_CLCD_DATA, PROT_WRITE);

	// KEYPAD Mapping
	// keypad_out  = mapper(IEB_KEY_W, PROT_WRITE);
	// keypad_in = mapper(IEB_KEY_R, PROT_READ);

	// Init Hardware
	init_led(led);
	init_dot(dot);
	init_fnd(fnd,elevator_floor);
	init_clcd(clcd_cmd, clcd_data);
	// init_keypad(keypad_out, keypad_in);

	// Check arguments
	if (argc==2) {
		elevator_floor = atoi(argv[1]);
	} else if (argc==3) {
		elevator_floor = atoi(argv[1]);
		max_floor = atoi(argv[2]);
		if (max_floor > MAX_FLOOR) {
			printf("The maximum floor cannot be higher than %d.\n", MAX_FLOOR);
			exit(0);
		}
	} else if (argc>3) {
		printf("Too many arguments supplied.\n");
		exit(0);
	}
	if (elevator_floor < 0) {
			printf("The starting floor cannot be lower than 0.\n");
			exit(0);
		}
	if (elevator_floor > max_floor) {
		printf("The starting floor cannot be higher than the maximum floor.\n");
		exit(0);
	}

	// 눌린 버튼을 저장하는 배열 생성
	pressed_button = (int *)malloc(sizeof(int) * max_floor);

	// 입력 수신 스레드 생성
	pthread_t input_thread;
	pthread_create(&input_thread, NULL, input, NULL);

	// 엘리베이터 동작 스레드 생성
	pthread_t elevator_thread;
	pthread_create(&elevator_thread, NULL, elevator, NULL);

	// 스레드가 종료될 때까지 대기
	pthread_join(input_thread, NULL);
	pthread_join(elevator_thread, NULL);
	
	// 눌린 버튼을 저장하는 배열 해제
	free(pressed_button);

	unmapper();
	close(fd);
	return 0;
}


short * mapper(off_t offset, int prot) {
	map_data[map_counter] = mmap(NULL, sizeof(short), prot, MAP_SHARED, fd, offset);
	if ( map_data[map_counter] == MAP_FAILED ) {
		fprintf(stderr, "Cannot do mmap()");
		emergency_closer();
	}
	return (short *)map_data[map_counter++];
}

void unmapper() {
	int i;
	for( i=0; i<map_counter; i++) {
		munmap(map_data[i], sizeof(short));
	}
	map_counter = 0;
}

void emergency_closer() {
	unmapper();
	close(fd);
	exit(EXIT_FAILURE);
}

void *input() {
	while(input_logic()==TRUE) {
	}
	return NULL;
}

truth_t input_logic() {
	if( exitProgram == 1 ) { return FALSE; }
	else { input_mode(); }
	return TRUE;
}

void input_mode() {
		int key_count, key_value;
		key_count = keyboard_read(&key_value);
		// key_count = keypad_read(&key_value);

		if( key_count == 1 ) {
			// 층 버튼
			if (key_value < 14) {
				pthread_mutex_lock(&mutex);
				pressed_button[key_value] = 1;
				next_floor = key_value;
				pthread_mutex_unlock(&mutex);
			}
			
			// 열림 버튼
			if (key_value == 14) {
				pthread_mutex_lock(&mutex);
				time_cnt = 0;
				pthread_mutex_unlock(&mutex);
			}

			// 닫힘 버튼
			if (key_value == 15) {
				pthread_mutex_lock(&mutex);
				time_cnt = 10;
				pthread_mutex_unlock(&mutex);
			}
		}
		else if( key_count > 1 ) {
			exitProgram = 1;
		}

		usleep(0);
		printf("key_value = %d\t key_count = %d\n", key_value, key_count);
		int i;
		for(i=0; i<max_floor; i++) {
			printf("%d ", pressed_button[i]);
		}
		printf("\n");
}

void *elevator() {
	while(exitProgram != 1) {
		if (moving()==TRUE && elevator_floor==next_floor && elevator_direction!=0) {
			// 엘리베이터가 도착했을 때
			// 엘리베이터 문을 열고 닫는다.
			// 시간 초기화
			open_door();
			while (time_cnt<10)
				{
					usleep(200000);
					pthread_mutex_lock(&mutex);
					time_cnt++;
					pthread_mutex_unlock(&mutex);
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
	if (moving()==TRUE) {
		int i;
		next_floor = get_target();
		if (elevator_floor < next_floor) {
			// 엘리베이터가 올라갈 때
			elevator_direction = 1;
			for (i = 0; i < 2; i++)
			{
				led_up_shift();	 // led shift
				usleep(5000);
			}
			elevator_floor++;
		} else if (elevator_floor > next_floor) {
			// 엘리베이터가 내려갈 때
			elevator_direction = -1;
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

truth_t moving() {
	int i;
	for (i=0; i<max_floor; i++) {
		if (pressed_button[i]==1) {
			return TRUE;
		}
	}
	elevator_direction = 0;
	return FALSE;
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