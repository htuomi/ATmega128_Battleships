/* definitions for bit shift operations in registers for I/O Buttons and LED */
  
  
#ifndef ATmega128definitions.h
#define ATmega128definitions.h
/* button up */
#define B_UP (!(PINA & (1<<PA0)))
/*button left*/
#define B_LEFT (!(PINA & (1<<PA1)))
/*button ok (middle) */
#define B_OK (!(PINA & (1<<PA2)))
/* button right */
#define B_RIGHT (!(PINA & (1<<PA3)))
/* button down */
#define B_DOWN (!(PINA & (1<<PA4)))
  
#define LED_ON (PORTA |= (1 << PA6))
#define LED_OFF (PORTA &= ~(1 << PA6))
  
#endif 
