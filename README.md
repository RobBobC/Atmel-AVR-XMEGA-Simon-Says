ECE-412-Final-Project-Simon-Game
================================

University of Louisville
Fall 2014
ECE 412: Introduction to Embedded Systems

Team CELADON: ATxmega256A3BU-XPLAINED - Simon Says Atmel ASF/AVR C Push-LED Game

Authors:

(John) Taylor Hans, Robert Cook, Ricardo Benitez, Austin Schroder

Press the Green LED to start a new game.

The game flashes all buttons twice before starting the sequence to let the user know it is starting
The game flashes all buttons four times when the game is over from the making an error and inputting the wrong order.

Just try to pace yourself and time the buttons correctly, we use a 75 ms delay in the ISR
(interrupt service routines) currently for each button between sequential user inputs
to detect the input push.

Speed, timing, and sound, are two aspects that could be tweaked in future improvements.

Press the White LED to see the stored high score in EEPROM.
Press the Red LED to clear the high score in EEPROM.
Press the Blue LED to turn on/off a crazy light show mode.

A Project Report and Presentation, that we created for our class is also in the repo.

Here is the link to the YouTube video: https://www.youtube.com/watch?v=s9xVoz6UuYk.
