// Omar Martinez
// Rotating LED Project
// Comp Org I
// 03/31/2018

#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "systick.h"

#define red		0
#define yellow	1
#define green	2
#define blue	3

#define RED0		0x01			// Pin - PB0
#define RED1		0x02			// Pin - PB1
#define BUTTONPRESSED 0x00// Pin - PA7
#define YELLOW0	0x04			// Pin - PB2
#define YELLOW1	0x08			// Pin - PB3
#define GREEN0	0x10			// Pin - PB4
#define GREEN1	0x20			// Pin - PB5
#define BLUE0		0x40			// Pin - PB6
#define BLUE1		0x80			// Pin - PB7

struct State 
{
	uint8_t LEDAddresses[2]; // contains the offset of each led to its respective pin address offset
	uint8_t nextState;	// index of the next state, while not particularly useful for this assignment, if we wanted to jump around states, we could
};

typedef const struct State STyp;

STyp FSM[4] =
{
	{{RED0, RED1}, yellow},
	{{YELLOW0, YELLOW1}, green},
	{{GREEN0, GREEN1}, blue},
	{{BLUE0, BLUE1}, red}
};

void GPIOA_Init(void)
{
	uint8_t delay;
  SYSCTL_RCGC2_R |= 0x00000001;     // 1) activate clock for Port A
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTA_AMSEL_R &= ~0x80;      // 3) disable analog on PA7
  GPIO_PORTA_PCTL_R &= ~0xF0000000; // 4) PCTL GPIO on PA7
  GPIO_PORTA_DIR_R |= 0x00;         // 5) PA7 in
  GPIO_PORTA_AFSEL_R &= ~0x80;      // 6) disable alt funct on PA7
  GPIO_PORTA_DEN_R |= 0x80;         // 7) enable digital I/O on PA7
	delay++;													// gets rid of compiler error about delay never beeing used
}

void GPIOB_Init(void)
{
	uint8_t delay;
  SYSCTL_RCGC2_R |= 0x00000002;     // 1) activate clock for Port B
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTB_AMSEL_R &= ~0xFF;      // 3) disable analog on PB
  GPIO_PORTB_PCTL_R &= ~0xF0000000; // 4) PCTL GPIO on PB
  GPIO_PORTB_DIR_R |= 0xFF;         // 5) PB out
  GPIO_PORTB_AFSEL_R &= ~0xFF;      // 6) disable alt funct on PB
  GPIO_PORTB_DEN_R |= 0xFF;         // 7) enable digital I/O on PB
	delay++;													// gets rid of compiler error about delay never beeing used
}

int main()
{
	uint8_t currentIndex = 0;	// the current index of our FSM
	PLL_Init(Bus80MHz);               // initialize 80 MHz system clock
  SysTick_Init();                   // initialize SysTick timer
  GPIOB_Init();
	GPIOA_Init();
  while(1)
	{
		GPIO_PORTB_DATA_R |= FSM[currentIndex].LEDAddresses[0]; // turn the LED's on
		GPIO_PORTB_DATA_R |= FSM[currentIndex].LEDAddresses[1]; // turn the LED's on
		if(GPIO_PORTA_DATA_R == BUTTONPRESSED) // the button was not pressed, so we increment out current location
		{
				currentIndex = FSM[currentIndex].nextState;
		}
		SysTick_Wait10ms(50);	// causes the clock to create an interupt resulting in a wait
		GPIO_PORTB_DATA_R &= FSM[currentIndex].LEDAddresses[0]; // turn the LED's off
		GPIO_PORTB_DATA_R &= FSM[currentIndex].LEDAddresses[1]; // turn the LED's off
  }
}
