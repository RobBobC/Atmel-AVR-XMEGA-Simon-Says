//
//	University of Louisville
//
//	ECE 412: Introduction to Embedded Systems
//
//	Fall 2014
//
//	Team Celadon Final Project: Simon Says Push-LED Game
//
//	Authors: 	Robert Cook
//				Taylor Hans
//				Austin Schroder
//				Ricardo Benitez
//

//	Defines
#define SEQUENCE_SIZE 8

//	Included standard C library headers
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//	Included Atmel Software Framework (ASF) library headers
//	TODO: Figure out all necessary ASF dependencies
//
//	- LCD
//	- Board Push-buttons
//	- Board LEDs
//	- Interrupts?
//	- GPIO
//
//#include <asf.h>

//	Globals
//	TODO: Find necessary ASF data structure that probably can be used
typedef struct LEDPortRegisterData {
	// Memory Address
	// Value
} LEDPortRegisterData_t;

//	Array to hold increasing game memory sequence
//	TODO (v1.01): Convert to holding board LED port register data?
//	TODO (v1.02): Convert to holding circuit LED GPIO port register data?
int iMemorySequence[SEQUENCE_SIZE];
//LEDPortRegisterData_t MemorySequence[];

//	Counter variable for sequence size
int iCount = 0;

//	LED enumeration
typedef enum {
	WHITE_LED,
	RED_LED,
	GREEN_LED,
	BLUE_LED
} LED_t;

//	Generates a random integer for adding to the memory sequence
int GetRandomNumber(void)
{
	// TODO: More in-depth random number generation (RNG) algorithm
	srand(time(NULL));
	
	// Generate a random number between 0 - 3
	int RandomNumber = rand() % 4;
	
	// Return result back to the caller
	return RandomNumber;
}

//	Adds to the game memory sequence
void AddSequenceElement(void)
{
	iMemorySequence[iCount] = GetRandomNumber();
	iCount++;
	
	return;
}

// Pauses the game for a certain amount of time passed
void Pause(int iTime)
{
	int iCurrentTime = 0;
	int iElapsedTime = 0;
	
	iCurrentTime = time(NULL);
	
	do
	{
		iElapsedTime = time(NULL);
	} while((iElapsedTime - iCurrentTime) < iTime);
	
	return;
}

// Displays the game sequence to memorize to the user
// TODO: Figure out delays, ASF delays are more accurate / precise
void DisplaySequence(void)
{
	int iIndex;
	
	printf("\n\tSequence: ");
	
	//	Loop through to display elements in the sequence
	for(iIndex = 0; iIndex < iCount; iIndex++)
	{
		if(iIndex == SEQUENCE_SIZE - 1)
			printf("%d", iMemorySequence[iIndex]);
		else
			printf("%d, ", iMemorySequence[iIndex]);
			
		// Pause between showing next sequence element
		Pause(1);
	}
	
	// Pause the game for a little bit so user can memorize sequence
	//Pause(3);
	
	return;
}

//	Starts a new game
void Start(void)
{
	int iIndex;
	
	for(iIndex = 0; iIndex < SEQUENCE_SIZE; iIndex++)
	{
		AddSequenceElement();
		DisplaySequence();
	}
	
	return;
}

//	Prints the game menu
void GameMenu(void)
{
	printf("\n\n\tSimon Says (v1.00 CLI)");
	printf("\n\t\t1) Start");
	printf("\n\t\t2) Quit");
	
	return;
}

//	Runs the game
void Run(void)
{
	int iMenuSelection = 0;
	
	// Loop to display game menu
	do
	{
		system("clear");
		
		GameMenu();
		
		printf("\n\tEnter Selection: ");
		scanf("%d", &iMenuSelection);
		
		//	User selects to start a new game
		if(iMenuSelection == 1)
		{
			Start();
		}
		
	} while(iMenuSelection != 2);
	
	return;
}

// Application main function
int main(void)
{
	Run();
}
