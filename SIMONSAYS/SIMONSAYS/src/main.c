//
//	University of Louisville
//
//	ECE 412: Introduction to Embedded Systems
//
//	Fall 2014
//
//	Team CELADON Final Project: Simon Says Push-LED Game
//
//	Authors: Robert Cook
//           Taylor Hans
//           Austin Schroder
//           Ricardo Benitez

// Program constants
#define INIT_DELAY 200
#define SEQUENCE_SIZE 200

// Interrupt macros
#define SBI(ADDRESS, BIT) ADDRESS |= (1 << BIT) // Set bit
#define CBI(ADDRESS, BIT) ADDRESS &= ~(1 << BIT) // Clear bit
#define TOGGLE(ADDRESS, BIT) ADDRESS ^= (1 << BIT) // Change bit

// Used libraries
#include<asf.h>
#include<delay.h>
#include<avr/interrupt.h>
#include<avr/eeprom.h>

// Globals
int gameOverIndicator, // 1 indicates a game over.
	globalDelay = 150, // Delay used in the game for light-up and in between delay. In milliseconds.
	inputDelay = 75, // Delay used in the game for input light-up and in between delay. In milliseconds.
	count, // Number of items in the sequence.
	memorySequence[SEQUENCE_SIZE], // Contains the sequence, holds in order.
	lastButtonRead, // Indicates which button was the last to be read.
	inputReceived = 0, // 1 indicates a new input has been received.
	loopBreak = 0,
	crazyShit = 0,
	i, j,
	menuMode = 1,
	highScore = 0,
	gameRun = 0;

// LED pneumonic enumeration
typedef enum {
	GREEN_LED,
	BLUE_LED,
	WHITE_LED,
	RED_LED
} LED_t;

// Function prototypes
int getRandomNumber(void);
void gameRunCheck(void);
void lightShow(void);
void addSequenceElement(void);
void displaySequence(void);
void gameOver(void);
void init(void);
void readSequence(void);
void run(void);
void toggleLight(LED_t outputButton);	
void toggleInputLight(LED_t outputButton);
void waitForInput(void);

// Function definitions
// Program main entry point
int main(void)
{
	init();
	
	// Enable interrupts
	sei();
	
	// Always checks if the game is read to run
	while(1)
	{
		gameRunCheck();
	}
	
	return 0;
}

// Generates a random number between 0 - 3 and returns the result back to the caller.
int getRandomNumber(void)
{
	int randomNumber = rand() % 4;
	
	return randomNumber;
}

void lightShow(void)
{
	if(crazyShit == 1)
	{
		crazyShit = 0;
	}
	else
	{
		crazyShit = 1;
	}
}

// Interrupt service routines for the LED buttons to play the game with.
// GREEN LED Interrupt Service Routine.
ISR(PORTC_INT0_vect)
{
	cli();
	
	if(menuMode == 1)
	{
		delay_ms(500);
		TOGGLE(PORTA.OUT, WHITE_LED);
		TOGGLE(PORTA.OUT, GREEN_LED);
		TOGGLE(PORTA.OUT, RED_LED);
		TOGGLE(PORTA.OUT, BLUE_LED);
		delay_ms(globalDelay);
		TOGGLE(PORTA.OUT, WHITE_LED);
		TOGGLE(PORTA.OUT, GREEN_LED);
		TOGGLE(PORTA.OUT, RED_LED);
		TOGGLE(PORTA.OUT, BLUE_LED);
		delay_ms(globalDelay);
		gameRun = 1;
	}
	
	else
	{
		toggleInputLight(GREEN_LED);
		lastButtonRead = GREEN_LED;
	}
}

// BLUE LED Interrupt Service Routine.
ISR(PORTC_INT1_vect)
{
	cli();
	
	if(menuMode == 1)
	{
		lightShow();
		delay_ms(75);
	}
	
	if(menuMode != 1)
	{
		lastButtonRead = BLUE_LED;
		toggleInputLight(BLUE_LED);
	}	
}

// WHITE LED Interrupt Service Routine.
ISR(PORTD_INT0_vect)
{
	cli();
	int k;
	
	if(menuMode == 1)
	{
		for (k = 0; k < highScore - 1; k++)
		{
			TOGGLE(PORTA.OUT, WHITE_LED);
			TOGGLE(PORTA.OUT, RED_LED);
			TOGGLE(PORTA.OUT, GREEN_LED);
			TOGGLE(PORTA.OUT, BLUE_LED);
			delay_ms(globalDelay);
		}
	}
	
	else
	{
		lastButtonRead = WHITE_LED;
		toggleInputLight(WHITE_LED);
	}
}

// RED LED Interrupt Service Routine.
ISR(PORTD_INT1_vect)
{
	cli();
	
	if(menuMode == 1)
	{
		eeprom_write_byte(0, 0);
		highScore = 0;
	}
	
	else
	{
		lastButtonRead = RED_LED;
		toggleInputLight(RED_LED);
	}
}

// Adds a random light to the sequence in the next available position.
void addSequenceElement(void)
{
	memorySequence[count] = getRandomNumber();
	count++;
	
	return;
}

// Loops through the memory sequence array and displays the sequence
void displaySequence(void)
{
	int i;
	
	for(i = 0; i < count; i++)
	{
		switch(memorySequence[i])
		{
			case WHITE_LED:
				toggleLight(WHITE_LED);
				break;
			case RED_LED:
				toggleLight(RED_LED);
				break;
			case GREEN_LED:
				toggleLight(GREEN_LED);
				break;
			case BLUE_LED:
				toggleLight(BLUE_LED);
				break;
			default:
				break;
		}
		
		delay_ms(globalDelay);
	}
	
	return;
}

// Flashes the LEDs 4 times in order to display a game over indication.
void gameOver(void)
{
	int i = 0;
	globalDelay = INIT_DELAY;
	
	for(i = 0; i < 4; i++)
	{
		TOGGLE(PORTA.OUT, WHITE_LED);
		TOGGLE(PORTA.OUT, RED_LED);
		TOGGLE(PORTA.OUT, GREEN_LED);
		TOGGLE(PORTA.OUT, BLUE_LED);
		delay_ms(globalDelay);
		TOGGLE(PORTA.OUT, WHITE_LED);
		TOGGLE(PORTA.OUT, RED_LED);
		TOGGLE(PORTA.OUT, GREEN_LED);
		TOGGLE(PORTA.OUT, BLUE_LED);
		delay_ms(globalDelay);
	}
	
	if(count >= highScore)
	{
		highScore = count;
	}
	
	eeprom_write_byte(0, highScore);
	
	menuMode = 1;
	
	return;
}

void gameRunCheck(void)
{
	if(crazyShit == 1)
	{
		globalDelay = 50;
		toggleLight(getRandomNumber());
		globalDelay = INIT_DELAY;
		gameRun = 0;
	}
	
	else if (gameRun == 1)
	{
		run();
	}
	
	return;
}

// Initializes the game, namely the interrupts required for the button inputs to be received.
void init(void)
{
		// Initializes the board target resources
		board_init();
		
		// System clock initialization
		sysclk_init();
		
		// Initializes the delay driver
		delay_init(sysclk_get_cpu_hz());
		
		// Read the high score stored in the EEPROM
		highScore = eeprom_read_byte(0);
		
		// Disables all interrupts
		cli();
		
		// Set PORTC / PORTD for input
		CBI(PORTC.DIR, 1);
		CBI(PORTD.DIR, 1);
		
		// Set PORTC / PORTD to Interrupt Mode
		SBI(PORTC.INT0MASK, 1);
		SBI(PORTC.INT1MASK, 2);
		SBI(PORTD.INT0MASK, 0);
		SBI(PORTD.INT1MASK, 1);
		
		// Set information for the interrupt pins on PORTC / PORTD
		PORTC.PIN1CTRL = PORT_OPC_WIREDANDPULL_gc | PORT_ISC_FALLING_gc;
		PORTC.PIN2CTRL = PORT_OPC_WIREDANDPULL_gc | PORT_ISC_FALLING_gc;
		PORTD.PIN0CTRL = PORT_OPC_WIREDANDPULL_gc | PORT_ISC_FALLING_gc;
		PORTD.PIN1CTRL = PORT_OPC_WIREDANDPULL_gc | PORT_ISC_FALLING_gc;
		
		// Set PORTC / PORTD and its INT0MASK for LO level priority
		PORTC.INTCTRL = PORT_INT0LVL_LO_gc | PORT_INT1LVL_LO_gc;
		PORTD.INTCTRL = PORT_INT0LVL_LO_gc | PORT_INT1LVL_LO_gc;
		
		// Set Interrupt Priority Level Configuration
		PMIC.CTRL |= PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;
		
		// Enable data outputs for PORTA, where the light outputs are plugged
		PORTA.DIRSET = 0xFF;
		
		// Turns off initial zombie RED LED
		CBI(PORTA.OUTCLR, 0);
}

// Reads the input sequence from the user to confirm that it is correct.
void readSequence(void)
{	
	for(i = 0; i < count; i++)
	{
		// Wait until an input is received from the user, eventually timeouts will have to taken into account and implemented
		waitForInput();
		
		// Take input and check if it is the current one in the sequence.
		if(lastButtonRead != memorySequence[i])
		{
			gameOverIndicator = 1;
			break;
		}
		
		// Resets, so the game doesn't think an input has been received for the next sequence item
		lastButtonRead = -1;
	}
	
	// Disable buttons from being read
	cli();
}

// Called to run the game.
void run(void)
{
	int i;
	
	gameRun = 0;
	menuMode = 0;
	count = 0;
	gameOverIndicator = 0;
	lastButtonRead = -1;
	
	// Game loop
	for(i = 0; i < SEQUENCE_SIZE; i++)
	{
		delay_ms(200);
		inputReceived = 0;
		addSequenceElement();
		displaySequence();
		
		// Enable interrupts
		sei();
		
		// Reads to confirm the sequence
		readSequence();
		
		// Checks if the user has lost the game
 		if(gameOverIndicator == 1)
 		{
 			gameOver();
 			break;
 		}
	}
	
	// Enable interrupts
	sei();
	
	return;
}

// This function takes an LED input from the enum, and determines which pin to energize, and does so within the switch function.
// Pins are able to be switched, if after setup, they are hooked to different pins. Uses the bit-shift toggle macro defined in the
// definitions section at the top of the code.
void toggleLight(LED_t outputButton)
{
	switch(outputButton)
	{
		case WHITE_LED:
			TOGGLE(PORTA.OUT, WHITE_LED);
			delay_ms(globalDelay);
			TOGGLE(PORTA.OUT, WHITE_LED);
			break;
		case BLUE_LED:
			TOGGLE(PORTA.OUT, BLUE_LED);
			delay_ms(globalDelay);
			TOGGLE(PORTA.OUT, BLUE_LED);
			break;
		case GREEN_LED:
			TOGGLE(PORTA.OUT, GREEN_LED);
			delay_ms(globalDelay);
			TOGGLE(PORTA.OUT, GREEN_LED);
			break;
		case RED_LED:
			TOGGLE(PORTA.OUT, RED_LED);
			delay_ms(globalDelay);
			TOGGLE(PORTA.OUT, RED_LED);
			break;
		default:
			break;
	}
}

void toggleInputLight(LED_t outputButton)
{
	switch(outputButton)
	{
		case WHITE_LED:
			TOGGLE(PORTA.OUT, WHITE_LED);
			delay_ms(inputDelay);
			TOGGLE(PORTA.OUT, WHITE_LED);
			break;
		case BLUE_LED:
			TOGGLE(PORTA.OUT, BLUE_LED);
			delay_ms(inputDelay);
			TOGGLE(PORTA.OUT, BLUE_LED);
			break;
		case GREEN_LED:
			TOGGLE(PORTA.OUT, GREEN_LED);
			delay_ms(inputDelay);
			TOGGLE(PORTA.OUT, GREEN_LED);
			break;
		case RED_LED:
			TOGGLE(PORTA.OUT, RED_LED);
			delay_ms(inputDelay);
			TOGGLE(PORTA.OUT, RED_LED);
			break;
		default:
			break;
	}	
}

// Waits for user input.
void waitForInput(void)
{
	for(j = 0; j < 1000; j++)
	{
		delay_ms(5);
		if(lastButtonRead > -1)
		{
			break;
		}
	}
}
