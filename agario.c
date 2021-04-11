  
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
	
volatile int pixel_buffer_start; // global variable 

typedef struct Ball{
	int x;
	int y; 
	int radius; 
	int dx;
	int dy; 
	short color;
}Ball; 

enum gameStatus{
	menu,
	game
}; 

enum gameStatus status; 

void clear_screen();
void drawBall(const Ball *ball, short int color); 
void wait_for_vsync(); 
void plot_pixel(int x, int y, short int line_color); 
void readKeyboard(unsigned char *clickedKey);
void redrawPlayer(Ball *ball, unsigned char var);
void draw_string(int x, int y, char str[]);
void clear_text();
int generateRandomNum(int lower, int upper);
void redrawRandomBall(Ball *ball,Ball *previousRandomBall);

Ball playerBall = {160,120,15, 0,0};
Ball previousBall =  {160,120,15, 0,0};
Ball randomBall = {0,0,0,0,0};
Ball previousRandomBall = {0,0,0,0,0};
	
int main(void) {
	
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
	pixel_buffer_start = *pixel_ctrl_ptr;
	
	int count = 0;
	
	 /* set front pixel buffer to start of FPGA On-chip memory */
   // *(pixel_ctrl_ptr + 1) = 0xC0000000;
    /* now, swap the front/back buffers, to set the front buffer location 
    wait_for_vsync();
    pixel_buffer_start = *pixel_ctrl_ptr;
	//clear_screen();
	//drawBall (&pBall,0x07E0);
	
    *(pixel_ctrl_ptr + 1) = 0xC8000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer*/
	
	unsigned char clickedKey = 0; 
	clear_screen();
	clear_text();
	
	drawBall (&playerBall,0x07E0); 
	status = menu;
	draw_string(30, 40, "press space to start");
	Ball randomBallArray[10];
	Ball previousBallArray[10];
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
		
		//draw previous balls to refresh screen
		drawBall(&previousBall,0x0000);
			
		
		//create random balls
		
		if (count ==0){
			for (int i=0; i<10; i++){			
				randomBallArray[i].x = generateRandomNum(10,310);
				randomBallArray[i].y = generateRandomNum(10,230);
				randomBallArray[i].radius = generateRandomNum(5,10);
				randomBallArray[i].dx = generateRandomNum(-3,3);
				randomBallArray[i].dy = generateRandomNum(-3,3);
				//randomize colour
				short color_array [10] = {0xf800,0x001f,0xffe0};
				randomBallArray[i].color = color_array [rand()%3];
				drawBall(&randomBallArray[i],randomBallArray[i].color);				
			}		
			
		}
		
		//draw previous ball
		if (count !=0){
			for (int i=0; i<10; i++){
			drawBall(&previousBallArray[i],0x0000);	
			}
		}else if (count ==0){
			for (int i=0; i<10; i++){
			drawBall(&randomBallArray[i],0x0000);	
				count++;
			}
		}
			
		//redraw player ball
		readKeyboard(&clickedKey);	
		redrawPlayer(&playerBall, clickedKey);
		drawBall(&playerBall,0x07E0);
			
		//redraw random ball
		for (int i=0; i<10; i++){
			redrawRandomBall(&randomBallArray[i], &previousBallArray[i]);
			drawBall(&randomBallArray[i],randomBallArray[i].color);
		}
		//drawRandomBall(&rBall,0x001F);
		}
			
	}
}

void redrawPlayer(Ball *ball, unsigned char var){
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

void redrawRandomBall(Ball *ball, Ball *previousRandomBall){
	(ball->y) += (ball->dx);
	(ball->x) += (ball->dy);
	//border cases for x
	if (ball->x - ball->radius < 0 || ball->x + ball->radius > 320) {
		ball->x = ball->x - 2*ball->dx;
		ball->dx = -1*ball->dx;}
	//border cases for y 
	if (ball->y - ball->radius < 0 || ball->y + ball->radius > 240) {
		ball->y = ball->y - 2*ball->dy;
		ball->dy = -1*ball->dy;}
	
	*previousRandomBall = *ball;
}

void drawBall(const Ball *ball, short int color){
	for (int i = ball->x - ball->radius; i < ball->x + ball->radius; i++) {
		for (int j = ball->y - ball->radius; j < ball->y + ball->radius; j++) {
			plot_pixel(i, j, color);
		}
	}
}
//------------------------------helper functions-------------------------------------------------
int generateRandomNum(int lower, int upper){
	int num = (rand() % (upper - lower + 1)) + lower;
	return num; 
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
