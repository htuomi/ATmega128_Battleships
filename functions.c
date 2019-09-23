/*functions for the battleships game */
/* Antti Tauriainen */
/* Henri Tuomivaara */


#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include "ATmega128def.h"
#include "lcd.h"
#include "functions.h"
#include <util/delay.h>

#define F_CPU 16000000UL

//#include <avr/eeprom.h>

/* create seed for random generation, source: avrfreks.net forum
void initrand()
{
	uint32_t state;
	static uint32_t EEMEM sstate;

	state = eeprom_read_dword(&sstate);

	if (state == 0xffffffUL){
		state = 0xAAAAAAUL;
	}
	srandom(state);
	eeprom_write_dword(&sstate, random());
}*%

/*function for creating the ship array, 
placing ships & making sure they fit and don't overlap */
void create_board(char ships_array[10][10]) {
   
int s,x,y,direction,size, COLS = 10, ROWS = 10;
    int sizes[]={5,4,3,3,2,1};
for(s=0;s<6;s++){
    size=sizes[s];
Randomize:
do {
x = random_in_range(0,9);
y = random_in_range(0,9);
direction = random_in_range(0,2);
} 
while (ships_array[x][y] != '0');
/*Horizontal Orientation*/
	if (direction == 0) {
/*Check if placement is on the ships_array*/
	while (y + size >= COLS){
		y = random_in_range(0,9);
}
/*Check if other ships are on the way*/
		for (int j = y; j < y + size; j++) {
			if (ships_array[x][j] == '1')
				goto Randomize;
			if (ships_array[x+1][j+1] == '1')
				goto Randomize;
			if (ships_array[x+1][j-1] == '1')
				goto Randomize;
			if (ships_array[x-1][j+1] == '1')
				goto Randomize;
			if (ships_array[x-1][j-1] == '1')
goto Randomize;
}
/*Place the ship on the ships_array*/
for (int j = y; j < y + size; j++) {
ships_array[x][j] = '1';
}
}
/*Vertical Orientation*/
	if (direction == 1) {
	while (x + size >= ROWS) {
		x = random_in_range(0,9);
		}
	for (int i = x; i < x + size; i++) {
		if (ships_array[i][y] == '1')
			goto Randomize;
		if (ships_array[i+1][y+1] == '1')
			goto Randomize;
		if (ships_array[i+1][y-1] == '1')
			goto Randomize;
		if (ships_array[i-1][y+1] == '1')
			goto Randomize;
		if (ships_array[i-1][y-1] == '1')
			goto Randomize;
	}
for (int i = x; i < x + size; i++) {
	ships_array[i][y] = '1';
}
}
}
return;
}

/* This random value generating function is from
http://stackoverflow.com/questions/2509679/how-to-generate-a-random-number-from-within-a-range-c by Ryan Reich */

int random_in_range (unsigned int min, unsigned int max)
{
 int base_random = rand(); /* in [0, RAND_MAX] */
 if (RAND_MAX == base_random) return random_in_range(min, max);
 /* now guaranteed to be in [0, RAND_MAX) */
 int range       = max - min,
     remainder   = RAND_MAX % range,
     bucket      = RAND_MAX / range;
 /* There are range buckets, plus one smaller interval
    within remainder of RAND_MAX */
 if (base_random < RAND_MAX - remainder) {
   return min + base_random/bucket;
 } else {
   return random_in_range (min, max);
 }
}


int get_screen_y( int x) {
	if ( x % 2 == 0) {
		return 0;
	}
	else return 1;
}



/* function for draw board on screen */
void draw_board(int cursor_cord[2],int shots_left[], char shots_array[10][10]){
	int i,b, row, a=0;
	char c_row0[2], c_row1[2], c_cord0[2], c_cord1[2], c_shots_left[3];
	lcd_gotoxy(0,0);
	/* find out the row where to begin printing on screen*/
	if (cursor_cord[1]%2 == 0){
		row = cursor_cord[1];
	}
	else if (cursor_cord[1]%2 != 0){
		row = (cursor_cord[1] -1);
	}
	/* change type int to char */
	sprintf(c_row0,"%d",row);
	/* draw the y coordinates */
	lcd_write_data(c_row0[0]);
	lcd_gotoxy(0,1);
	sprintf(c_row1,"%d",(row+1));
	lcd_write_data(c_row1[0]);

	//draw borders
	lcd_gotoxy(1,0);
	lcd_write_data('I');
	lcd_gotoxy(1,1);
	lcd_write_data('I');
	lcd_gotoxy(12,0);
	lcd_write_data('I');
	lcd_gotoxy(12,1);
	lcd_write_data('I');
	/* draw two rows of gameboard in the middle of the screen*/
	for(b=row;b<=(row+1);b++){
		lcd_gotoxy(2,a);
		a=1;
		for(i=0;i<=9;i++){
			lcd_write_data(shots_array[b][i]);
		}
	}

	/* draw cursor coordinates and shot count) */
	lcd_gotoxy(14,0);
	sprintf(c_cord0,"%d",cursor_cord[0]);
	lcd_write_data(c_cord0[0]);
	sprintf(c_cord1,"%d",cursor_cord[1]);
	lcd_write_data(c_cord1[0]);
	lcd_gotoxy(13,1);
	lcd_write_data('S');
	sprintf(c_shots_left, "%d", shots_left);
	if (shots_left >= 10) {
		lcd_write_data(c_shots_left[0]);
		lcd_write_data(c_shots_left[1]);
		}
	else {
		lcd_write_data('0');
		lcd_write_data(c_shots_left[0]);
		}
}



/*function for marking hit or miss */
int shoot (int cursor_cord[], char ships_array[10][10]) { 
  /*check if shot is a hit*/
  if (ships_array[cursor_cord[1]][cursor_cord[0]]=='1')
{
return 1;
}
//miss
else
{
return 0;
}
}


//buzzer wihtout interrupts, sjames_remingon at yahoo dot com

void buzzer(unsigned int frequency, unsigned int duration){
		unsigned int wait_units, loops, i;
		DDRE  |=  (1 << PE4) | (1 << PE5);
		//DDRE |= 1;										//make PORTE output
		wait_units = F_CPU/(8UL * (long int) frequency); //half of the tone period, in delay loop units
		loops = duration*F_CPU/(8000UL*wait_units);		//number of loops to execute
		for (i=1;i<=loops; i++)		{
				PORTE ^= (1 << PE4) | (1 << PE5); 		//sound the buzzer
				_delay_loop_2(wait_units);
				PORTE ^= (1 << PE4) | (1 << PE5);		//
				_delay_loop_2(wait_units);
		}
}
