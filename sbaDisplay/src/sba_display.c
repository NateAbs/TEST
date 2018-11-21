#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include "sba_header.h"

/*******************************************************************************************************************|
 * 	      File Name:  sba_display.c
 *	        Purpose:  Handles the outputs to the screen for user interaction
 *          Authors:  Nathan M. Abbey
 *
 *        Professor:  Gerry Hurdle
 *           Course:  CST8244 - Realtime Programming
 *          Purpose:  Skills Based Assessment (SBA)
 *  Submission Date:
 *
 *    Methods:  I. main(int argc, char* argv[])
 *
 *******************************************************************************************************************/


#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL

typedef union {
	struct _pulse  pulse;
	char msg[255];
} my_message_t;



programStatus outgoing_status;
programStatus status_message;


name_attach_t *attach;

/* I ----------------------------------------------------------------------------------/
 *    Method: main
 *   Purpose: Prints out a message to screen based on input, then responds
 *   Returns: EXIT_SUCCESS on success, EXIT_FAILURE on error
 *
 * Variables:                 int channel_id - Holds the comm channel id
 *			     int status_message_received - Holds status of received message
 *			   programStatus outgoing_status - Struct to send back to controller
 *			    programStatus status_message - Holds status message coming from controller
 *
 * 	   Steps: 1.  Create a communication channel
 * 	          2.  Verify if channel was created successfully
 * 	          3.  Display the server pid to screen
 * 	          4.  Begins a loop for the server which runs until stopped
 * 	          5.  Receives data from controller
 * 	          6.  Prints out message depending on input
 * 	          7.  Update the outgoing message
 * 	          8.  Reply to controller with message
 * 	          9.  Check to see if status is an exit status
 * 	         10.  Print exit message and destroy channel
 * -----------------------------------------------------------------------------------*/
int main(void) {

	int channel_id;

	int status_message_received;


	attach = name_attach(NULL, "pdisplay", 0);

	if (attach == NULL){
		perror("name_attach() Error!");
		return EXIT_FAILURE;
	}




	// 4. Begins a loop for the server which runs until stopped ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
	do {

		// 5. Receives data from controller ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
		status_message_received = MsgReceive(attach->chid, &status_message, sizeof(status_message), NULL);


		// 6. Prints out message depending on input ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
		printf("%s\n", outStatusMessage[status_message.messageIndex]);

		// 7. Update the outgoing message ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
		outgoing_status.messageIndex = status_message.messageIndex;

		// 8. Reply to controller with message ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
		MsgReply (status_message_received, EOK, &outgoing_status, sizeof(outgoing_status));

		// 9. Check to see if status is an exit status ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
	} while (outgoing_status.messageIndex != STOP);

	// 10. Print exit message and destroy channel ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
	printf("Exit Display\n");
	ChannelDestroy(channel_id);

	return EXIT_SUCCESS;

} // END of sba_display.c
