  
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
	
volatile int pixel_buffer_start; // global variable 

typedef struct playerBall{
	int x;
	int y; 
	int radius; 
	int dx;
	int dy; 
}playerBall; 

enum gameStatus{
	menu,
	game
}; 

enum gameStatus status; 

void clear_screen();
void drawBall(const playerBall *ball, short int color); 
void wait_for_vsync(); 
void plot_pixel(int x, int y, short int line_color); 
void readKeyboard(unsigned char *clickedKey);
void redrawPlayer(playerBall *ball, unsigned char var);
	void draw_string(int x, int y, char str[]);
void clear_text();
playerBall pBall = {160,120,15, 0,0};
	playerBall previousBall =  {160,120,15, 0,0};

int main(void) {
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
	pixel_buffer_start = *pixel_ctrl_ptr;
	

	 /* set front pixel buffer to start of FPGA On-chip memory */
   // *(pixel_ctrl_ptr + 1) = 0xC0000000;
    /* now, swap the front/back buffers, to set the front buffer location 
    wait_for_vsync();

    pixel_buffer_start = *pixel_ctrl_ptr;
	//clear_screen();
	//drawBall (&pBall,0x07E0);
	
    *(pixel_ctrl_ptr + 1) = 0xC8000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
	*/
	unsigned char clickedKey = 0; 
	clear_screen();
	clear_text();
	drawBall (&pBall,0x07E0); 
	status = menu;
	draw_string(30, 40, "press space to start");
	while(true){
		if (status == menu){
			readKeyboard(&clickedKey);
				if (clickedKey == 0x29) {
					clear_screen();
					clear_text();
					status = game;
				}
				
		}else if (status == game){
		wait_for_vsync();
		drawBall(&previousBall,0x0000);
		readKeyboard(&clickedKey);	
		redrawPlayer(&pBall, clickedKey);
		drawBall(&pBall,0x07E0);
		}
			
	}
}

void drawBall(const playerBall *ball, short int color) {
	for (int i = ball->x - ball->radius; i < ball->x + ball->radius; i++) {
		for (int j = ball->y - ball->radius; j < ball->y + ball->radius; j++) {
			plot_pixel(i, j, color);
		}
	}
}

void wait_for_vsync(){
	volatile int *pixel_ctrl_ptr = (int*)0xFF203020; // pixel controller
	register int status;
	
	*pixel_ctrl_ptr = 1; // start the synchronization process
	
	do {
		status = *(pixel_ctrl_ptr + 3);
	} while ((status & 0x01) != 0); // wait for s to become 0
}

void clear_screen(){
    for (int x = 0; x < 320; x++) {
        for (int y = 0; y < 240; y++) {
            plot_pixel(x, y, 0x0000);
        }
    }
}

void plot_pixel(int x, int y, short int line_color){
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

void readKeyboard(unsigned char *clickedKey){
	int dataVar; 
	volatile int * PS2_ptr = (int *) 0xFF200100;
	dataVar = *PS2_ptr;
	*clickedKey = dataVar & 0xFF;
	//loop 
	while (dataVar & 0x8000) {
		dataVar = *PS2_ptr;
	}
}
void redrawPlayer(playerBall *ball, unsigned char var){
	//for arrow key inputs 
	if (var == 0x72)
		ball->dy = 3; 
	else if (var == 0x75)
		ball->dy = -3;
	else if (var == 0x74)
		ball->dx = 3;
	else if (var == 0x6B)
		ball->dx = -3;
	else{
		ball->dx = 0;
		ball->dy = 0;
	}
	//add the delta 
	(ball->y) += (ball->dy); 
	(ball->x) += (ball->dx);
	//border cases for x 
	if (ball->x - ball->radius < 0 || ball->x + ball->radius > 320) {
		ball->x = ball->x - ball->dx;
		ball->dx = 0;}
	//border cases for y 
	if (ball->y - ball->radius < 0 || ball->y + ball->radius > 240) {
		ball->y = ball->y - ball->dy;
		ball->dy = 0;}
	previousBall = *ball;
}

void draw_string(int x, int y, char str[]) {
	for (int i = 0; i < strlen(str); i++) {
			volatile int charBuffer = 0xc9000000;
	*(char *)(charBuffer + (y << 7) + x+i) = str[i];
	}
}

void clear_text() {
	for (int x = 0; x < 80; x++) {
		for (int y = 0; y < 60; y++) {
			volatile int charBuffer = 0xc9000000;
	*(char *)(charBuffer + (y << 7) + x) = ' ';
		}
	}
}
