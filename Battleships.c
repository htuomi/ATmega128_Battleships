#include <avr/io.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "ATmega128def.h"
#include <util/delay.h>

#define LCD_BLINK  0x01

void init(void) {

   		/* estetään kaikki keskeytykset */
		cli();

        /* kaiutin pinnit ulostuloksi */
        DDRE  |=  (1 << PE4) | (1 << PE5);
        /* pinni PE4 nollataan */
        PORTE &= ~(1 << PE4);
        /* pinni PE5 asetetaan */
        PORTE |=  (1 << PE5);   
        
        /* ajastin nollautuu, kun sen ja OCR1A rekisterin arvot ovat samat */
        TCCR1A &= ~( (1 << WGM11) | (1 << WGM10) );
        TCCR1B |=    (1 << WGM12);
        TCCR1B &=   ~(1 << WGM13);


		/* allow capture interrupt*/
		TIMSK |= (1 << TICIE1);


        /* salli keskeytys, jos ajastimen ja OCR1A rekisterin arvot ovat samat */
        TIMSK |= (1 << OCIE1A);

        /* asetetaan OCR1A rekisterin arvoksi 0x3e (~250hz) */
        OCR1AH = 0x00;
        OCR1AL = 0x3e;

        /* käynnistä ajastin ja käytä kellotaajuutena (16 000 000 / 1024) Hz */
        TCCR1B |= (1 << CS12) | (1 << CS10);

		/* näppäin pinnit sisääntuloksi */
		DDRA &= ~(1 << PA0);
		DDRA &= ~(1 << PA2);
		DDRA &= ~(1 << PA4);

		/* rele/led pinni ulostuloksi */
		DDRA |= (1 << PA6);

		/* lcd-näytön alustaminen */
		lcd_init();
		lcd_write_ctrl(LCD_ON | LCD_BLINK);
		lcd_write_ctrl(LCD_CLEAR);



}

int main(void) 
{

		/* alusta laitteen komponentit */
		init();
		int  shots_left=0,  cursor_cord[2] = {0,0}, ship_health=0, hitsInRow=0, hitCount=0,
			game_over = 1, state_draw=1, menuScreen=0,
			state_aim = 0, intro = 1 , screen_cord_y= 0, victory=0;

		char shots_array[10][10], ships_array[10][10];
        /* ikuinen silmukka */
        while (1) {

			if (intro == 1){  				
				lcd_gotoxy(0,0);
				write_lcd(1, " **Battleships**");
				lcd_gotoxy(1,5);
				write_lcd(0, "~~Press UP~~");
				intro = 0;
				//start timer for random seed
				TCCR0 |= ((1 << CS0) | (1 << CS2));
			}
			else if B_UP { 
				buzzer(350, 100);
				/*get random seed from timer */
				srand(TCNT1);
				//stop timer
				TCCR0 &= 0B11111000;
				//reset timer value
				TCNT0=0;
				//initrand();
				/* initialize variables */
				//shots_left = 20;
				ship_health = 18;
				victory = 0;
				cursor_cord[0] = 0;
				cursor_cord[1] = 0;
				hitsInRow = 0;
				hitCount = 0;
				/* fill arrays with zeroes*/
				memset(shots_array,'0',sizeof(shots_array));
				memset(ships_array,'0',sizeof(ships_array));
				/*set up the board */
				create_board(*ships_array);
				//Menu
				menuScreen = 1;
				while (menuScreen) {
					lcd_gotoxy(0,0);
					write_lcd(1, "Help,pressUP");
					lcd_gotoxy(0,1);
					write_lcd(0, "Easy,pressDOWN");
					/* delay prevent multiple button presses */
					_delay_ms(300);
					/*_delay_ms(1000);
					lcd_gotoxy(0,0);
					write_lcd(0, "Medium,pressRIGHT");
					_delay_ms(1000);
					lcd_gotoxy(0,1);
					write_lcd(0, "Hard,pressLEFT");
					_delay_ms(1000);*/
					//game_over = 0;
					if (B_UP) {
						// write instructions 
						buzzer(350, 100);
						lcd_gotoxy(0,0);
						write_lcd(1,"Hits marked '#'");
						lcd_gotoxy(0,1);
						write_lcd(0, "and misses '/'");
						_delay_ms(1000);
						lcd_gotoxy(0,0);
						write_lcd(1, "30 shots");
						_delay_ms(1000);
						lcd_gotoxy(0,0);
						write_lcd(1, "3 hit combo ->");
						lcd_gotoxy(0,1);
						write_lcd(0, "+ 1 shot");
						int help_screen = 1;
						_delay_ms(300);
						while (help_screen == 1) {
					
							if B_UP {
								buzzer(350, 100);
								help_screen = 0;
							}
						}
					}
					else if (B_DOWN) {
						buzzer(350, 100);
						game_over = 0;
						state_draw = 1;
						menuScreen = 0;
						shots_left = 30;
						lcd_gotoxy(0,0);
						write_lcd(1, "You have");
						lcd_gotoxy(0,1);
						write_lcd(0, "30 shots");
						_delay_ms(1000);

					}
					/*else if (B_RIGHT) {
						game_over = 0;
						state_draw= 1;
						menuScreen = 0;
						shots_left = 20;
						lcd_gotoxy(0,0);
						write_lcd(1, "You have");
						lcd_gotoxy(0,1);
						write_lcd(0, "20 shots");
						_delay_ms(1000);
					}
					else if (B_LEFT) {
						game_over = 0;
						state_draw= 1;
						menuScreen = 0;
						shots_left = 10;
						lcd_gotoxy(0,0);
						write_lcd(1, "You have");
						lcd_gotoxy(0,1);
						write_lcd(0, "10 shots");
						_delay_ms(1000);
					}*/
				}
			}
			//Game loop
			while (game_over == 0){				
						if (state_draw == 1){						
							lcd_write_ctrl(LCD_CLEAR);
							draw_board(cursor_cord, shots_left, shots_array );
						/* check what row blinking cursor should be on screen */
						}
						screen_cord_y = get_screen_y(cursor_cord[1]);
						/* blink the cursor only on board elements*/
						lcd_gotoxy((cursor_cord[0] + 2), screen_cord_y);
						state_draw = 0;
						/* if shots left = 0*/
						state_aim = 1;
						if (shots_left== 0){
							game_over = 1;
							state_aim = 0;
						}
						
						/* ship_health = 0 */
						else if (ship_health == 0) {
							victory = 1;
							game_over = 1;
							state_aim = 0;
						}
						
						/*delay to prevent multiple button presses*/
						_delay_ms(300);
						//make sure LED is off
						LED_OFF;
						while (state_aim==1){
							if (B_UP) {
								buzzer(350, 100);
								if (cursor_cord[1] > 0){
									cursor_cord[1] = cursor_cord[1]-1;
									state_draw = 1;
									state_aim =0;
								}
							}

							else if (B_RIGHT ){
								buzzer(350, 100);
								if (9>cursor_cord[0]){
									cursor_cord[0] = cursor_cord[0]+1;
									state_draw = 1;
									state_aim =0;
								}
							}

							else if (B_DOWN){
								buzzer(350, 100);
								if (cursor_cord[1]<9){
									cursor_cord[1] = cursor_cord[1]+1;
									state_draw = 1;
									state_aim =0;
								}
							}

							else if (B_LEFT){
								buzzer(350, 100);
								if (cursor_cord[0]>0){
									cursor_cord[0]=cursor_cord[0]-1;
									state_draw = 1;
									state_aim =0;
								}
							}

							else if (B_OK){
								state_aim = 0;
								/*check if cordinates have been shot before */
								if (shots_array[cursor_cord[1]][cursor_cord[0]] == '0')
									{
									if (shoot(cursor_cord, ships_array) == 1)
										{
										//Turn LED on if hit
										LED_ON;
										ship_health = (ship_health - 1);
										shots_array[cursor_cord[1]][cursor_cord[0]] = '#';
										hitsInRow = hitsInRow + 1;
										//count hits for score
										hitCount = hitCount + 1;
										buzzer(200,300);
										buzzer(300,200);
										//if 3 hits in row, add one shot
										if (hitsInRow == 3)
											{
											shots_left = shots_left +1;
											hitsInRow = 0;
											buzzer(200,300);
											buzzer(300,100);
											buzzer(400,200);
											}
										}
									else if (shoot(cursor_cord, ships_array) == 0)
										{
										shots_left = (shots_left- 1);
										shots_array[cursor_cord[1]][cursor_cord[0]] = '/';
										hitsInRow = 0;
										//if miss, play sound
										buzzer(250,500);
										}
								}
								state_draw = 1;
								
							}

							

							
						
							
						


					/*while state_aim*/
					}
				
	    		

				if ((game_over == 1) && (victory == 0)){
					lcd_gotoxy(0,0);
					write_lcd(1, "You lost...");
					lcd_gotoxy(0,1);
					int gameOver_screen = 1;
					// count and write score
					int score = 0;
					char c_score[5];
					score = (hitCount * 25);
					//cange type int to char
					sprintf(c_score,"%d", score);
					write_lcd(0, "Your score:");
					write_lcd(0, c_score);
					//Show 'You lost...' -text for one second, then replace with 'Press Down'
					_delay_ms(1000);
					lcd_gotoxy(0,0);
					write_lcd(0,"Press DOWN");
					while (gameOver_screen == 1) {
							if B_DOWN {
								lcd_gotoxy(0,0);
								write_lcd(1, "Restarting");
								int k = 0;
								for (k=0;k<5;k++) {
									write_lcd(0, ".");
									_delay_ms(600);
								}
								intro = 1;
								//game_over = 0;
								gameOver_screen = 0;
								break;
							}
					}
				}
				else if (victory == 1){
					lcd_gotoxy(0,0);
					write_lcd(1,"You won!!!");
					lcd_gotoxy(0,1);
					int victory_screen = 1;
					//count and write score
					int score = 0;
					char c_score[5];
					score = (shots_left * 150) + (hitCount * 25);
					sprintf(c_score,"%d", score);
					write_lcd(0, "Your score:");
					write_lcd(0, c_score);
					//Show 'You won!!!' -text for one second, then replace with 'Press Down'
					_delay_ms(1000);
					lcd_gotoxy(0,0);
					write_lcd(0,"Press DOWN");
					while (victory_screen == 1) {
						if B_DOWN {
							lcd_gotoxy(0,0);
							write_lcd(1, "Restarting");
							int g;
							for (g=0;g<5;g++) {
								write_lcd(0, ".");
								_delay_ms(600);
							}
							intro = 1;
							victory = 0;
							victory_screen = 0;
							break;
						}
					}
				}
					
			/* while (game_over == 0)*/
			}
		/*while(1)*/
		}
/*main*/
}
		


ISR(TIMER1_COMPA_vect) {
	/* vaihdetaan kaiutin pinnien tilat XOR operaatiolla */
 	PORTE ^= (1 << PE4) | (1 << PE5); 
}





