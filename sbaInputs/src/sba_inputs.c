#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>
#include <sys/wait.h>

#include "sba_header.h"

/*******************************************************************************************************************|
 * 	      File Name:  sba_inputs.c
 *	        Purpose:  Handles the inputs from the user for the program
 *          Authors:  Nathan M. Abbey
 *
 *        Professor:  Gerry Hurdle
 *           Course:  CST8244 - Realtime Programming
 *          Purpose:  Skills Based Assessment (SBA)
 *  Submission Date:  October 28, 2018
 *
 *    Methods:  I. main(int argc, char* argv[])
 *
 *******************************************************************************************************************/


/* I ----------------------------------------------------------------------------------/
 *    Method: main
 *   Purpose: Runs the mainline of the program. Presents a menu and sends messages to
 *            the server based on user input to menu.
 *   Returns: EXIT_SUCCESS on success, EXIT_FAILURE on error
 *
 * Variables:      char  choice[4]	-->   Used to accept menu input
 * 			             int  coid	-->   Connection ID
 * 			programStatus statusOut	-->   Struct object to hold variables for output
 * 			programStatus statusIn	-->   Struct object to hold incoming variables
 * 			      pid_t  serverpid	-->   Used to obtain Server PID from command line
 *
 * 	   Steps:  1.  Establish a connection to the server
 * 	   		   2.  If 'coid' equals -1, something went wrong and END
 * 	   		   3.  Set the struct to the first stage
 * 	   		   4.  Use WHILE loop for keeping the program running
 * 	   		   5.  Check to see if menu needs displaying
 * 	   		   6.  Prints out choice options and accepts input
 *             7.  Handles if input is "LD"
 *             8.  Handles if input is "RD"
 *             9.  Handles if input is "S"
 *            10.  Send the struct w/ MsgSend()
 *            11.  Receive confirmation from controller
 *            12.  Assign the received data to struct to see if menu needs displaying
 *            13.  Break out of loop if input indicates
 *            14.  Detaches the channel when finished
 *
 * -----------------------------------------------------------------------------------*/
int main(int argc, char* argv[]) {

	programStatus statusOut;
	programStatus statusIn;
	char choice[10];

	int coid;

	coid = name_open("pcontroller", 0);

	if(coid == NULL){
		perror("name)_open ERROR.");
		return EXIT_FAILURE;

	}




	// 3. Set the struct to the first stage ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
	statusOut.messageIndex = READY;
	statusOut.skipMenu = TRUE;



	// 4. Use WHILE loop for keeping the program running ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
	do {

		// 5. Check to see if menu needs displaying ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
		if (statusOut.skipMenu == FALSE){

			// 6. Prints out choice options and accepts input ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
			setvbuf (stdout, NULL, _IONBF, 0);
			printf("Enter the command (LD, RD, LU, RU, P #, S)." );
			scanf("%s", &choice);


			// 7. If input is "LD" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
			if (strcmp(choice, inCommands[LEFT_DOWN]) == 0)
				statusOut.messageIndex = LEFT_DOWN;


			// 8. If input is "RD" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
			else if (strcmp(choice, inCommands[RIGHT_DOWN]) == 0)
				statusOut.messageIndex = RIGHT_DOWN;


			// 9. If input is "S" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
			else if (strcmp(choice, inCommands[STOP]) == 0)
				statusOut.messageIndex = STOP;


			// 10. If input is "P" and an integer ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
			else if (strstr(choice, "P") != NULL) {


			}
		}



			MsgSend(coid, &statusOut, sizeof(statusOut), NULL, 0);
			/*                 coid --> The connection ID received in ConnectAttach()
			 *            statusOut --> Pointer to a send buffer containing Program Status object
			 *    sizeof(statusOut) --> Size of the send buffer
			 *             statusIn --> Pointer to a receive buffer filled w/ Program Status object
			 *     sizeof(statusIn) --> Size of the receiver buffer
			 * */






		// 12. Assign the received data to struct to see if menu needs displaying ~~~~~~~~~~~~~~~~~~~~~~~|
		statusOut.messageIndex = statusIn.messageIndex;
		statusOut.skipMenu = statusIn.skipMenu;


		// 13. Break out of loop if input indicates ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~_|
	} while (strcmp(choice, inCommands[STOP]));


	// 14. Detaches the channel when finished ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|W
	return EXIT_SUCCESS;

} // End of sba_inputs.c
