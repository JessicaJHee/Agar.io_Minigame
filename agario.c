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

void clear_screen();
void drawBall(const playerBall *ball, short int color); 
void wait_for_vsync(); 
void plot_pixel(int x, int y, short int line_color); 

int main(void) {
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
	pixel_buffer_start = *pixel_ctrl_ptr;
	
	playerBall pBall = {160,120,10, 0,0};
	
	 /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();

    pixel_buffer_start = *pixel_ctrl_ptr;
	clear_screen();
	drawBall (&pBall,0xFFFF);
	
    *(pixel_ctrl_ptr + 1) = 0xC8000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
	
	drawBall (&pBall,0xFFFF); 
	 
    return 0;
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

//clears screen (same as part 1)
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

