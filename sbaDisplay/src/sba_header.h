
/*********************************************************************************************|
 * 	File Name:	sba_2018.h
 *	  Purpose:  Hold and define the data structures to be used for the SBA.
 *     Author:  Nathan M. Abbey
 *       Date:  October 20, 2018
 *    Objects:
 *   			1.  Struct used to for message passing between programs.
 *   			2.  Booleans used for simplicity in error checking
 *   			3.  Char array with Commands that the user can use
 *   			4.  Enumerated states representing the states of the FSM
 *
 ********************************************************************************************/


// 1. Struct used to for message passing between programs. --------------------|
struct program_status {
	int messageIndex;
	int skipMenu;
} typedef programStatus;



// 2. Booleans used for simplicty in error checking ---------------------------|
typedef enum {
	TRUE = 316,
	FALSE= 613
} Bool;


// 3. Char array with Commands that the user can use -------------------------|
const char *inCommands[7] = {
		"--- ",			// 0
		"---",			// 1
		"LD",			// 2
		"RD",			// 3
		"---",			// 4
		"---",    		// 5
		"S"				// 6
};


// 4. Char array used to display messages to output -------------------------|
const char *outStatusMessage[7] = {
		"EMPTY",													// 0
		"Ready...",													// 1
		"Left Button Down - press right button to arm press",		// 2
		"Right Button Down - press left button to arm press",		// 3
		"DANGER - Press is Armed! - Hold buttons for 2 seconds.",	// 4
		"Press Cutting Now",										// 5
		"Powering down."											// 6
};


// 4. Enumerated states representing the states of the FSM -----------------|
typedef enum {
	EMPTY,					// 0
	READY,					// 1
	LEFT_DOWN,				// 2
	RIGHT_DOWN,				// 3
	ARMED,					// 4
	PUNCH,					// 5
	STOP					// 6
} State2;

