//-------| src/clcd.c |-------//
#include "clcd.h"
#include "string.h"


static short * clcd_cmd, * clcd_data;
static char* floors[14] = {"1","2","3","4","5","6","7","8","9","10","11","12","13","14"};

void init_clcd(short * address_cmd, short * address_data) {
	int D=1, C=0, B=0;
	int DL=1, N=1, F=0;
	
	clcd_cmd  = address_cmd;
	clcd_data = address_data;
	
	clcd_clear_display();
	clcd_return_home();
	clcd_display_control(D, C, B);
	clcd_function_set(DL, N, F);
	clcd_press_button();
}

void clcd_write_cmd(int cmd) {
	*clcd_cmd = (short)cmd; usleep(CLCD_CMD_US);
}
void clcd_write_data(int data) {
	*clcd_data = (short)data; usleep(CLCD_DATA_US);
}

void clcd_clear_display() {
	clcd_write_cmd(0x01); usleep(CLCD_RETURN_US);
}
void clcd_return_home() {
	clcd_write_cmd(0x02); usleep(CLCD_RETURN_US);
}
void clcd_entry_mode_set(int ID, int S) {
	int cmd = 1 << 2;
	if( ID != 0 ) { cmd |= (1 << 1); }
	if( S  != 0 ) { cmd |= (1 << 0); }
	clcd_write_cmd(cmd);
}
void clcd_display_control(int D, int C, int B) {
	int cmd = 1 << 3;
	if( D != 0 ) { cmd |= (1 << 2); }
	if( C != 0 ) { cmd |= (1 << 1); }
	if( B != 0 ) { cmd |= (1 << 0); }
	clcd_write_cmd(cmd);
}
void clcd_shift(int SC, int RL) {
	int cmd = 1 << 4;
	if( SC != 0 ) { cmd |= (1 << 3); }
	if( RL != 0 ) { cmd |= (1 << 2); }
	clcd_write_cmd(cmd);
}
void clcd_function_set(int DL, int N, int F) {
	int cmd = 1 << 5;
	if( DL != 0 ) { cmd |= (1 << 4); }
	if( N  != 0 ) { cmd |= (1 << 3); }
	if( F  != 0 ) { cmd |= (1 << 2); }
	clcd_write_cmd(cmd);
}
void clcd_set_DDRAM(int address) {
	int cmd = 1 << 7;
	cmd |= address;
	clcd_write_cmd(cmd);
}

void clcd_write_string(char str[]) {
	int i;
	for( i=0; (str[i] != 0); i++) {
		clcd_write_data(str[i]);
	}
}

void clcd_door_open(int elevator_floor) {
	clcd_clear_display();
	char display_str[12] = "Arrived at ";
	strcat(display_str,floors[elevator_floor]);
	strcat(display_str,"F");
	clcd_write_string("Door Opening");
	clcd_set_DDRAM( 0x40);
	clcd_write_string(display_str);
	usleep(0);
}

void clcd_door_opened() {
	clcd_clear_display();
	clcd_write_string("Door Opened");
	usleep(0);
}

void clcd_door_close() {
	clcd_clear_display();
	clcd_write_string("Door Closing");
	usleep(0);
}

void clcd_press_button() {
	clcd_clear_display();
	clcd_write_string("Press Button..");
	usleep(0);
}

void clcd_moving_up() {
	clcd_clear_display();
	clcd_write_string("Moving Up..");
	usleep(0);
}

void clcd_moving_down() {
	clcd_clear_display();
	clcd_write_string("Moving Down..");
	usleep(0);
}

void clcd_invalid_input() {
	clcd_clear_display();
	clcd_write_string("Invalid Input");
	usleep(0);
}
