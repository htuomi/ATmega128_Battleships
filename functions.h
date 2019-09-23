#ifndef func.h
#define func.h

int random_in_range (unsigned int min, unsigned int max);

void create_board(char ships_array[10][10]);

//int check_cordinates(int size, int x, int y, int direction, char ship_array[10][10]);

//void write_shiparray(int sixe, int x, int y, int direction, char *ship_array[10][10]);

void draw_board(int cursor_cord[2],int shots_left[], char shots_array[10][10]);

int shoot(int cursor_cord[], char ship_array[10][10]);

int get_screen_y( int x);

void buzzer(unsigned int frequency, unsigned int duration);

void initrand();
#endif 
