//
//  University of Louisville
//
//  ECE 412: Introduction to Embedded Systems
//
//  Fall 2014
//
//  Team Celadon Final Project: Simon Says Push-LED Game
//
//  Authors:    Robert Cook
//              Taylor Hans
//              Austin Schroder
//              Ricardo Benitez
//

#include<asf.h>
#include<delay.h>
#include<avr/interrupt.h>

#define SBI(ADDRESS,BIT) ADDRESS |= (1<<BIT)
#define CBI(ADDRESS,BIT) ADDRESS &= ~(1<<BIT)
#define TOGGLE(ADDRESS,BIT) ADDRESS ^= (1<<BIT)

#define INIT_DELAY 200
#define SEQUENCE_SIZE 8

typedef enum {
    WHITE_LED,
    RED_LED,
    GREEN_LED,
    BLUE_LED
} LED_t;

int gameOverIndicator = 0, // 1 indicates a game over.
    globalDelay = 200, // Delay used in the game for light-up and in between delay. In milliseconds.
    iCount = 0, // Number of items in the sequence.
    iMemorySequence[SEQUENCE_SIZE], // Contains the sequence, holds in order.
    lastButtonRead, // Indicates which button was the last to be read.
    inputReceived; // 1 indicates a new input has been received.

void addSequenceElement(void)
{
    // Adds a random light to the sequence in the next available position.
    iMemorySequence[iCount] = getRandomNumber();
    iCount++;
    
    return;
}

void displaySequence(void)
{
    int iIndex;
    
    //  Loop through to display elements in the sequence
    for(iIndex = 0; iIndex < iCount; iIndex++)
    {
        switch(iMemorySequence[iIndex])
        {
            case 0:
                toggleLight(WHITE_LED);
                break;
            case 1:
                toggleLight(RED_LED);
                break;
            case 2:
                toggleLight(GREEN_LED);
                break;
            case 3:
                toggleLight(BLUE_LED);
                break;
            default:
                break;
        }
        
        delay_ms(globalDelay);
    }
    
    return;
}

void gameMenu(void)
{
    /* To do: Move over to LCD screen integration. */
    printf("\n\n\tSimon Says (v1.00 CLI)");
    printf("\n\t\t1) Start");
    printf("\n\t\t2) Quit");
    
    return;
}

void gameOver(void)
{
    globalDelay = INIT_DELAY;
    
    /* Flashes the lights 5 times in order to display a game over indication. */
    for(int i = 0; i < 5; i++)
    {
        toggleLight(WHITE_LED);
        toggleLight(RED_LED);
        toggleLight(BLUE_LED);
        toggleLight(GREEN_LED);
        delay_ms(globalDelay);
        toggleLight(WHITE_LED);
        toggleLight(RED_LED);
        toggleLight(BLUE_LED);
        toggleLight(GREEN_LED);
        delay_ms(globalDelay);
    }
    
    return;
}

int getRandomNumber(void)
{
    /* NOTE: I don't think the AVR boards allow time.h, so we'll have to find something else. Looked like you were going to change it up anyway though. */
    
    // TODO: More in-depth random number generation (RNG) algorithm
    srand(time(NULL));
    
    // Generate a random number between 0 - 3
    int RandomNumber = rand() % 4;
    
    // Return result back to the caller
    return RandomNumber;
}

void init(void)
{
        delay_init(); // Initializes the delay library.
        
        /* Initializes the game, namely the interrupts required for the button inputs to be received. */
        cli();
        
        // set PORTB to input.
        CBI(PORTB.DIR,1);
        
        // Set PORTB to Interrupt Mode
        SBI(PORTB.INT0MASK, 4);
        SBI(PORTB.INT0MASK, 5);
        SBI(PORTB.INT0MASK, 6);
        SBI(PORTB.INT0MASK, 7);
        
        // Set information for the interrupt pins on PORTB.
        PORTB.PIN4CTRL = PORT_OPC_WIREDANDPULL_gc | PORT_ISC_FALLING_gc;
        PORTB.PIN5CTRL = PORT_OPC_WIREDANDPULL_gc | PORT_ISC_FALLING_gc;
        PORTB.PIN6CTRL = PORT_OPC_WIREDANDPULL_gc | PORT_ISC_FALLING_gc;
        PORTB.PIN7CTRL = PORT_OPC_WIREDANDPULL_gc | PORT_ISC_FALLING_gc;
        
        // set PORTF and its INT0MASK for LO level priority
        PORTB.INTCTRL = PORT_INT0LVL_LO_gc;
        
        // Set Interrupt Priority Level Configuration
        PMIC.CTRL|=PMIC_HILVLEN_bm|PMIC_MEDLVLEN_bm|PMIC_LOLVLEN_bm;
        
        // Enable data outputs for PortA, where the light outputs are plugged.
        PORTA.DIRSET = 0xFF;
}

ISR(PORTB_INT0_vect)
{
    /* Interrupt function for any of the 4 buttons to play the game with. */
    cli();
    
    switch(PORTB.IN)
    {
        case 4:
            lastButtonRead = 0;
            break;
        case 5:
            lastButtonRead = 1;
            break;
        case 6:
            lastButtonRead = 2;
            break;
        case 7:
            lastButtonRead = 3;
            break;
        default:
            break;
    }
    
    inputReceived = 1;
    sei();
}

int main(void)
{
    init();
    run();
    return 0;
}

void readSequence(void)
{
    sei(); // Enable sequence to be read.
    
    for(int i = 0; i < iCount; i++)
    {
        while(inputReceived == 0)
        {
            // Loop until an input is received. Eventually, timeouts will have to be taken into account.
        }
        
        // Take input, and match it up to the current one in the sequence.
        if(lastButtonRead != iMemorySequence[i])
        {
            gameOverIndicator = 1;
            break;
        }
        
        inputReceived = 0; // Resets, so the game doesn't think an input has been received for the next sequence item.
    }
    
    cli(); // Disable buttons from being read.
}

void run(void)
{
    /* To-do : Move over to LCD integration, so command line isn't needed. */
    int iMenuSelection = 0;
    
    // Loop to display game menu
    do
    {
        system("clear");
        
        gameMenu();
        
        printf("\n\tEnter Selection: ");
        scanf("%d", &iMenuSelection);
        
        //  User selects to start a new game
        if(iMenuSelection == 1)
        {
            start();
        }
        
    } while(iMenuSelection != 2);
    
    return;
}

void start(void)
{
    int iIndex;
    
    for(iIndex = 0; iIndex < SEQUENCE_SIZE; iIndex++)
    {
        addSequenceElement();
        displaySequence();
        readSequence();
        
        if(gameOverIndicator == 1)
        {
            gameOver();
            break;
        }
    }
    
    return;
}

void toggleLight(LED_t outputButton)
{
    /* This function takes an LED input from the enum, and determines which pin to energize, and does so within the switch function. 
        Pins are able to be switched, if after setup, they are hooked to different pins. Uses the bit-shift toggle function defined in the
        definitions section at the top of the code. */
    
    switch(outputButton)
    {
        case WHITE_LED:
            TOGGLE(PORTA.OUT, 0);
            delay_ms(globalDelay);
            TOGGLE(PORTA.OUT, 0);
            break;
        case RED_LED:
            TOGGLE(PORTA.OUT, 1);
            delay_ms(globalDelay);
            TOGGLE(PORTA.OUT, 1);
            break;
        case GREEN_LED:
            TOGGLE(PORTA.OUT, 2);
            delay_ms(globalDelay);
            TOGGLE(PORTA.OUT, 2);
            break;
        case BLUE_LED:
            TOGGLE(PORTA.OUT, 3);
            delay_ms(globalDelay);
            TOGGLE(PORTA.OUT, 3);
            break;
        default:
            break;
    }
}
