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
// 프로그램 종료 여부
static int exit_input = 0; 


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
	keypad_out  = mapper(IEB_KEY_W, PROT_WRITE);
	keypad_in = mapper(IEB_KEY_R, PROT_READ);

	// Check arguments
	if (argc==2) {
		elevator_floor = atoi(argv[1])-1;
	} else if (argc==3) {
		elevator_floor = atoi(argv[1])-1;
		max_floor = atoi(argv[2]);
		if (max_floor > MAX_FLOOR) {
			fprintf(stderr, "The maximum floor cannot be higher than %d.\n", MAX_FLOOR);
			exit(0);
		}
	} else if (argc>3) {
		printf("Too many arguments supplied.\n");
		exit(0);
	}
	if (elevator_floor < 0) {
			printf("The starting floor cannot be lower than 1.\n");
			exit(0);
		}
	if (elevator_floor >= max_floor) {
		printf("The starting floor cannot be higher than the maximum floor.\n");
		exit(0);
	}

	// Init Hardware
	init_led(led);
	init_dot(dot);
	init_fnd(fnd,elevator_floor);
	init_clcd(clcd_cmd, clcd_data);
	init_keypad(keypad_out, keypad_in);

	init_elevator(&mutex);
	set_elevator_floor(elevator_floor);
	set_max_floor(max_floor);

	// 입력 수신 스레드 생성
	pthread_t input_thread;
	pthread_create(&input_thread, NULL, input, NULL);

	// 엘리베이터 동작 스레드 생성
	pthread_t elevator_thread;
	pthread_create(&elevator_thread, NULL, elevator, NULL);

	// 스레드가 종료될 때까지 대기
	pthread_join(input_thread, NULL);
	pthread_join(elevator_thread, NULL);
	
	exit_program();

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
	if( exit_input == 1 ) { return FALSE; }
	else { input_mode(); }
	return TRUE;
}

void input_mode() {
		int key_count, key_value;
		
		// Ximulator 입력
		// key_count = keyboard_read(&key_value);

		// 키패드 입력
		key_count = keypad_read(&key_value);

		if( key_count == 1 ) {
			// 층 버튼
			if (key_value < max_floor && key_value != get_elevator_floor()) {
				press_button(key_value);
			} 
			else if ((key_value>=max_floor && key_value<14) || key_value==get_elevator_floor()) {
				clcd_invalid_input();
			}
			
			// 열림 버튼
			if (key_value == 14) {
				set_time_cnt(0);
			}

			// 닫힘 버튼
			if (key_value == 15) {
				set_time_cnt(8);
			}

		}
		else if( key_count > 1 ) {
			exit_input = 1;
			exit_program();
		}

		usleep(0);
}


