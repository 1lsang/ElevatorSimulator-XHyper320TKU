//-------| src/main.c |-------//
#include "main.h"


static off_t IEB_DOT[MAX_DOT] = {
	IEB_DOT1,
	IEB_DOT2,
	IEB_DOT3,
	IEB_DOT4,
	IEB_DOT5
};
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

static int fd;
static int map_counter = 0;
static void * map_data[100];
static int exitProgram = 0; 
static int currentFloor = 0;

static int buttonPressed[10] = {0,0,0,0,0,0,0,0,0,0};
static int closeButton = 0;
static int openButton = 0;
static int running = 0;
static pthread_t thread2;
static int pid = -1;
static int dir = 2;
static int x=0;
static int maxFloor = 9;


// Test
static pthread_t moveThread;

int main(int argc, char* argv[]) {
	
	int i;
	short * led, * dot[MAX_DOT], * fnd[MAX_FND];
	short * clcd_cmd, * clcd_data, * keypad_out, * keypad_in;
	
	fd = open("/dev/mem", O_RDWR|O_SYNC);
	if (fd == -1) {
		fprintf(stderr, "Cannot open /dev/mem file");
		exit(EXIT_FAILURE);
	}
	
	led = mapper(IEB_LED, PROT_WRITE);
	for( i=0; i<MAX_DOT; i++ ) {
		dot[i] = mapper(IEB_DOT[i], PROT_WRITE);
	}
	for( i=0; i<MAX_FND; i++ ) {
		fnd[i] = mapper(IEB_FND[i], PROT_WRITE);
	}
	clcd_cmd  = mapper(IEB_CLCD_CMD, PROT_WRITE);
	clcd_data = mapper(IEB_CLCD_DATA, PROT_WRITE);

	if (argc==2) {
		currentFloor = atoi(argv[1]);
	} else if (argc==3) {
		currentFloor = atoi(argv[1]);
		maxFloor = atoi(argv[2]);
	}

	// keypad_out  = mapper(IEB_KEY_W, PROT_WRITE);
	// keypad_in = mapper(IEB_KEY_R, PROT_READ);

	init_led(led);
	init_dot(dot);
	init_fnd(fnd,currentFloor);
	init_clcd(clcd_cmd, clcd_data);
	// init_keypad(keypad_out, keypad_in);

	while( logic() == TRUE ) { 
	}
	
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

truth_t logic() {
	if( exitProgram == 1 ) { return FALSE; }
	else { input_mode(); }
	return TRUE;
}

void input_mode() {
		int key_count, key_value;
		key_count = keyboard_read(&key_value);
		// key_count = keypad_read(&key_value);

		if( key_count == 1 ) {
			printf("key_value = %d\n", key_value);
			if (key_value<=maxFloor) {
				buttonPressed[key_value] = 1;
			} else if (key_value==10) {//열림
				openButton=1;
				closeButton=0;
			} else if (key_value==11) {//닫힘
				closeButton=1;
				openButton=0;
			} else {
				clcd_clear_display();
				clcd_write_string("Invalid Input");
			}
		}
		else if( key_count > 1 ) {
			exitProgram = 0;
		}

}


void open_door() {
	int i = 0;

	led_all();//led 전체 점등

	for(i=0;i<3;i++) {//3초에 걸쳐서 문 열림
		usleep(1000000);
		dot_write(i);
		//문열림 표시
	}//도트 문 열리는 로직

	usleep(1000000);
	dot_write(3);//문 완전 열림
	clcd_door_opened();//문열림
}


void close_door() {
	int i = 0;
	clcd_door_close();//문 닫히는중 표시
	led_clear();//led 전체 꺼짐
	
	for(i=0;i<3;i++) {//3초에 걸쳐서 문 닫힘
		usleep(1000000);
		dot_write(3-i);
		//문닫힘 표시
	}//도트 문 닫는 로직
	
	usleep(1000000);
	dot_write(0);//문 완전 닫힘
	clcd_door_closed();//문닫힘
}


void move_up() {
	currentFloor++; // 층수 변경
	int i = 0;
	for (i = 0; i < 2; i++)
	{
		led_up_shift();	 // led shift
		usleep(5000);
	}
	
	fnd_number(currentFloor); // fnd 변경
	usleep(0);
}

void move_down() {
	int i = 0;
	for (i = 0; i < 2; i++)
	{
		led_down_shift();	 // led shift
		usleep(5000);
	}
	currentFloor--; // 층수 변경
	fnd_number(currentFloor); // fnd 변경
	usleep(0);
}


void *move_to() {
	while(check()) {
		running = 1;
		int target = get_target();
		if (target==10) {
			break;
		}
		if (target==currentFloor) {
				clcd_door_open(currentFloor);//현재 층 수 표시
				usleep(1000000);
				open_door();
				int j = 0;
				while (j!=10)
				{
					usleep(200000);
					j++;
					if (openButton) 
						j=0;
					if (closeButton)
						j=10;
				}
				openButton = 0;
				closeButton = 0;
				close_door();
				buttonPressed[currentFloor] = 0;
				running = 0;
				dir=2;
				continue;
		} else {
			if (target<currentFloor) {
				clcd_moving_down(); // clcd moving up
				move_down();
				dir = 0;
			} else if (target>currentFloor) {
				clcd_moving_up(); // clcd moving up
				move_up();
				dir = 1;
			}
		}
	}
}

int check() {
	int i = 0;
	int res = 0;
	for (i = 0; i < 10; i++)
		{
			if (buttonPressed[i] == 1) {
				res = 1;
			}
		}
	return res;
}

int get_target() {
	int i = 0;
	int res = 10;
	for (i = 9; i >= 0; i--)
		{
			if (buttonPressed[i] == 1) {
				if (dir==1&& i>=currentFloor || dir==0&&i<=currentFloor || dir==2) {
					int tmp = i-currentFloor;
					if (abs(tmp) < res)
						res = i;
				}
			}
		}
	return res;
}