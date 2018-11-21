#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <process.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/dispatch.h>
#include "sba_header.h"



/*******************************************************************************************************************|
 * 	      File Name:  sba_controller.c
 *	        Purpose:  Runs the state machine and sends values to display, then returns values to inputs
 * *        Authors:  Nathan M. Abbey and Billy SaintFort
 *
 *        Professor:  Gerry Hurdle
 *           Course:  CST8244 - Realtime Programming
 *          Purpose:  SBA
 *  Submission Date:  October 28, 2018
 *
 * Global Variables:           	int rcvid - The pid of the input client
 *                             int rcvid2 - The pid of the display server
 *                           	 int chid - Communication channel ID created
 *                     	int connection_id - The server PID which controller connected to
 *                    StateFunc statefunc - Function pointer initialized to first State
 *					 programStatus status - The state the state machine is currently at
 *         programStatus receiving_status - Struct used for sending data to display
 *                    person personOutput - Struct used for getting data from inputs
 *                     person personInput - Struct used for sending data to inputs
 *
 *          Methods:    I. main(int argc, char* argv[])
 *          		   II. void *ready_state()
 *          		  III. void *left_down()
 *          		   IV. void *right_down()
 *          		    V. void *armed_state()
 *          		   VI. void *punch_state()
 *                    VII. void *exit_program()
 *
 *
 *******************************************************************************************************************/


// Function Prototypes ----------|
typedef void *(*StateFunc)();
void *ready_state();
void *left_down();
void *right_down();
void *armed_state();
void *punch_state();
void *exit_state();




// Global Variables -------------|
int     rcvid;
int 	rcvid2;
int     chid;
int 	side;
int connection_id;
StateFunc statefunc = ready_state;
programStatus statusOut;
programStatus statusIn;
name_attach_t *attach;




/* I ----------------------------------------------------------------------------------/
 *    Method: main
 *   Purpose: Operates the state machine for the program.
 *   Returns: EXIT_SUCCESS on success, EXIT_FAILURE on error
 *
 * Variables:  pid_t serverpid - Server pid which is taken from command line
 *
 * 	   Steps:  1.  Grab server pid from command line to connect with server in display
 *   		   2.  Establish connection with display server using the pid
 *   		   3.  Print out the controller pid
 *   		   4.  Create a channel and returns the Child ID
 *   		   5.  If it fails it will go to -1 and end
 *   		   6.  Server begins in an infinite loop, awaiting input
 *   		   7.  Receives input from assign_inputs.c and assigns it to struct personInput
 *   		   8.  Uses the statefunc to point the function pointer to appropriate function
 *   		   9.  Send status of the state machine to assign1_display.c to show message
 *   		  10.  Error and exit program if MsgSend() fails
 *   		  11.  Receive response from the controller
 *   		  12.  Assign the received data to appropriate struct
 *   		  13. Wait for appropriate times based on message
 *   		  14.  Send struct to inputs
 *   		  15.  Jump out if at the EXIT state
 *            16.  Print out exit message and destroy the channel
 * -----------------------------------------------------------------------------------*/
int main(void){

	int coid;
	programStatus status_message;

	coid = name_open("pdisplay", 0);

	if(coid == NULL){
		perror("name_attach() Error!");
		return EXIT_FAILURE;
	}

	//3. Print out the controller pid ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
	printf("The controller is running as process_id: %d\n\n", getpid());


	attach = name_attach(NULL, "pcontroller", 0);

	if (attach == NULL){
		perror("name_attach() Error!");
		return EXIT_FAILURE;
	}




	// 5. If it fails it will go to -1 and end ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
	if (chid == -1) {
		perror("failed to create the channel.");
		exit (EXIT_FAILURE);
	}


	// 6. Server begins in an infinite loop, awaiting input ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
	do {


		// 7. Receives input from assign_inputs.c and assigns it to struct personInput ~~~~~~~~~~~~~~~~~~|
		rcvid = MsgReceive(attach->chid, &status_message, sizeof(status_message), NULL);



		// 8. Uses the statefunc to point the function pointer to appropriate function
		statefunc = (StateFunc)(*statefunc)();



		// 9. Send status of the state machine to assign1_display.c to show message ~~~~~~~~~~~~~~~~~~~~~|
		if (MsgSend(connection_id, &statusOut, sizeof(statusOut), &statusIn, sizeof(statusIn)) == - 1)
		{

			// 10. Error and exit program if MsgSend() fails ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
			fprintf(stderr, "Error during MsgSend in controller to display\n");
			perror(NULL);
			exit(EXIT_FAILURE);
		}

		// 11. Receive response from the controller ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
		rcvid2 = MsgReceive (connection_id, &statusIn, sizeof(statusIn), NULL);

		// 12. Assign the received data to appropriate struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
		statusOut.messageIndex = statusIn.messageIndex;

		// 13. Wait for appropriate times based on message ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
		if (statusOut.messageIndex == READY)
			sleep(3);
		if (statusOut.messageIndex == ARMED)
			sleep(2);
		if (statusOut.messageIndex == PUNCH)
			sleep(1);
		if (statusOut.messageIndex == STOP)
			sleep(5);


		// 14. Send struct to inputs ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
		MsgReply (rcvid, EOK, &statusOut, sizeof(statusOut));

		// 15. Jump out if at the EXIT state ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
	} while (statusOut.messageIndex != STOP);

	// 16. Print out exit message and destroy the channel ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
	printf("Exiting Controller.\n");
	ChannelDestroy(chid);

	return EXIT_SUCCESS;
}



/* II ---------------------------------------------------------------------------------/
 *    Method: *ready_stage()
 *   Returns: Function pointer 'statefunc'
 * -----------------------------------------------------------------------------------*/
void *ready_state(){

	statusOut.messageIndex = statusIn.messageIndex;
	statusOut.skipMenu = FALSE;

	if (statusIn.messageIndex == LEFT_DOWN)
		statefunc = left_down;

	if (statusIn.messageIndex == RIGHT_DOWN)
		statefunc = right_down;

	return statefunc;
}





/* III --------------------------------------------------------------------------------/
 *    Method:left_down()
 *   Returns: Function pointer 'statefunc'
 *
 * 	    Step: Sets the appropriate data, updates function pointer and returns
 * -----------------------------------------------------------------------------------*/
void *left_down(){

	statusOut.messageIndex = statusIn.messageIndex;

	if (statusIn.messageIndex == RIGHT_DOWN) {
		statusOut.messageIndex = ARMED;
		statusOut.skipMenu = TRUE;
		statefunc = armed_state;
	}

	return statefunc;
}



/* IV ---------------------------------------------------------------------------------/
 *    Method: right_down()
 *   Returns: Function pointer 'statefunc'
 *
 * 	    Step: Sets the appropriate data, updates function pointer and returns
 * -----------------------------------------------------------------------------------*/
void *right_down(){

	statusOut.messageIndex = statusIn.messageIndex;

	if (statusIn.messageIndex == LEFT_DOWN) {
			statusOut.messageIndex = ARMED;
			statusOut.skipMenu = TRUE;
			statefunc = armed_state;
		}
	return statefunc;
}



/* V ----------------------------------------------------------------------------------/
 *    Method: armed_state()
 *   Returns: Function pointer 'statefunc'
 *
 * 	    Step: 1.  Will exit program if input is incorrect
 * 	          2.  Update state of program and send status, and update function pointer
 * 	          3.  Return function pointer 'statefunc'
 * -----------------------------------------------------------------------------------*/
void *armed_state(){
	statusOut.messageIndex = PUNCH;
	statefunc = punch_state;
	return statefunc;
}




/* VI ---------------------------------------------------------------------------------/
 *    Method: punch_state()
 *   Returns: Function pointer 'statefunc'
 *
 * 	    Step: 1.  Will exit program if input is incorrect
 * 	          2.  Update state of program and send status, and update function pointer
 * 	          3.  Return function pointer 'statefunc'
 * -----------------------------------------------------------------------------------*/
void *punch_state(){
	statusOut.messageIndex = READY;
	statusOut.skipMenu = FALSE;
	statefunc = ready_state;
	return statefunc;
}





/* VII --------------------------------------------------------------------------------/
 *    Method: exit_state()
 *   Returns: Function pointer 'statefunc'
 *
 * 	    Step: 1.  Error check and exit if one is present
 * 	          2.  Update the data and the function pointer
 * 	          3.  Returns the function pointer
 * -----------------------------------------------------------------------------------*/

void *exit_state(){
	statusOut.messageIndex = STOP;
	statusOut.skipMenu = TRUE;
	statefunc = exit_state();
	return statefunc;
}
