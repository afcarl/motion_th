/* SerialLib.cCOPYRIGHT:  This file may be distributed freely as long as this notice accompanies  it and any changes are noted in the source.  It is distributed as is,  without any warranty implied or provided.  We accept no liability for  any damage or loss resulting from the use of this software.PURPOSE:  Functions to talk to MacIntosh serial ports by driver name.HISTORY:	7/28/93		dhb, jdt	Major rewrite to eliminate global variables.											Possibly a big mistake.	9/25/97		dhb				Add error checking on FSRead, FSWrite.						dhb				Don't close printer/modem drivers, as per advice in Inside Mac.						dhb				Reset input buffer to default.  Failure to do this while disposing											of our buffer pointer may have been the source of crashes.	9/30/97		dhb				As per developer note, kill i/o at close.											As per developer note, which says inside Mac is wrong, close printer/modem drivers.KNOWN BUGS:	7/28/93		dhb				No routine for setting serial port parameters is available.						dhb				In general the routines are not thoroughly tested.*/#include "SerialLib.h"#include <stdlib.h>#include <stdio.h>#include <string.h>#include <Strings.h>/* c2pstr() and p2cstr() *//********************************************************************************																																								*  Function: SerialInit																												 *																																							*  Purpose: Initialze serial ports for input and output.  Driver names should*           be C strings with full driver name for input/output.  In principle,*           the routines will work if either string is empty or NULL. 											 *																																							 *******************************************************************************/void SerialInit(SerialInfo *info, char *inDriver, char *outDriver)   {	OSErr err;			// Put the driver names where they belong	SerialSetDrivers(info,inDriver,outDriver);	// printf("Set driver names\r");	// printf("In driver: ---%s---\r",PtoCstr(info->inDriver)); CtoPstr((char *) info->inDriver);	// printf("Out driver: ---%s---\r",PtoCstr(info->outDriver)); CtoPstr((char *) info->outDriver);	// Initialize output driver.  This should come before the input for reasons	// we don't understand.	if (info->outDriver[0] != 0) {		err = OpenDriver(info->outDriver,&(info->outRefNum));		if (err) {			info->outRefNum = -1;			// printf("Error initializing output = %ld\r",(long) err);		}		else {			// printf("Opened output driver\r");		}	}			// Initialize input driver.	if (info->inDriver[0] != 0) {		err = OpenDriver(info->inDriver,&(info->inRefNum));		if (err) {			info->inRefNum = -1;			// printf("Error initializing input = %5d\r",(int) err);		}		else {			// printf("Opened input driver\r");			// printf("After open, inRefNum is %ld\r",(long) (info->inRefNum));						// Create a buffer to hold input characters.  This space			// is accessible to user programs.  It is where GetSerialChars			// dumps its output.			if ( !(info->inCharBuf = (char *) NewPtr(SERBUFSIZ)) ) {				info->inRefNum = -1;				// printf("Memory error inCharBuf\r");			}			else {				// printf("Allocated inCharBuf\r");			}					// Give the serial manager a SERBUFSIZ buffer for input.			// This is, we think, where the serial driver puts characters			// as they come in.  We do not access this directly ourselves,			// since that would screw up the driver queuing. 			if(!(info->inMgrBuf = (char *) NewPtr(SERBUFSIZ))) {				info->inRefNum = -1;				// printf("Memory error inMgrBuf\r");			}			else {				// printf("About to set inMgrBuf, inRefNum is %ld\r",(long) (info->inRefNum));				// printf("Pointer to buffer is %lx\r",info->inMgrBuf);				err = SerSetBuf(info->inRefNum,info->inMgrBuf,(short) SERBUFSIZ);				if (err) {					info->inRefNum = -1;					// printf("Error setting inMgrBuf\r");				}				else {					// printf("Input port initialized\r");				}			}			// printf("Allocated and set inMgrBuf\r");		}	}}/********************************************************************************																																								*  Function: SerialClose																												 *																																							*  Purpose: Close up serial ports for input and output 											 *																																							 *******************************************************************************/void SerialClose(SerialInfo *info){		// Convert names to C strings for checking.	p2cstr(info->inDriver);	p2cstr(info->outDriver);		// Kill any pending i/o	KillIO(info->inRefNum);	KillIO(info->outRefNum);		// Reset original input buffer and dispose of ours	if (info->inMgrBuf) {		SerSetBuf(info->inRefNum,NULL,0);		DisposePtr(info->inMgrBuf);		info->inMgrBuf = NULL;	}		// Close input driver.  Commented out code prevents close for printer/modem.	//if ( !strcmp((char *) info->outDriver,".AIn") && !strcmp((char *) info->inDriver,".BIn") ) {		if (info->inRefNum != -1) {			CloseDriver(info->inRefNum);			info->inRefNum = -1;		}	//}		// Close output driver.  Commented out code prevents close for printer/modem.	//if ( !strcmp((char *) info->outDriver,".AOut") && !strcmp((char *) info->inDriver,".BOut") ) {		if (info->outRefNum != -1) {			CloseDriver(info->outRefNum);			info->outRefNum = -1;		}	//}		// Dispose of our internal buffer	if (info->inCharBuf) {			DisposePtr(info->inCharBuf);			info->inCharBuf = NULL;	}		// Clear driver names from our structure	info->inDriver[0] = '\0';	info->outDriver[0] = '\0';}/********************************************************************************																																								*  Function: SerialSetDrivers																												 *																																							*  Purpose: Copy the driver names into the SerialInfo structure. 											 *																																							 *******************************************************************************/void SerialSetDrivers(SerialInfo *info, char *inDriver, char *outDriver){	if (inDriver == NULL) {		strcpy((char *) info->inDriver,"");	}	else {			strcpy((char *) info->inDriver,inDriver);	}	c2pstr((char *) info->inDriver);		if (outDriver == NULL) {		strcpy((char *) info->outDriver,"");	}	else {		strcpy((char *) info->outDriver,outDriver);	}	c2pstr((char *) info->outDriver);}/********************************************************************************																																								*  Function: SerialSetSpeed																												 *																																							*  Purpose: One could use the following code as the start of a routine*  to set the parameters of the serial port.  The handshaking flags are*  not tested.* 																																	 *******************************************************************************/void SerialSetSpeed(SerialInfo *info,short baudRate){	OSErr err;		if (info->outRefNum != -1) {		err = SerReset(info->outRefNum,baudRate + data8 + stop10 + noParity);		if (err) {			printf("Error setting output\n");		}	}		if (info->inRefNum != -1) {		err = SerReset(info->inRefNum,baudRate + data8 + stop10 + noParity);		if (err) {			printf("Error setting input\n");		}	}}/********************************************************************************																																							 	*  Function: SerialCharsAvail																									*																																							 *  Purpose: Returns number of characters in serial input buffer								 *																																							 *******************************************************************************/long  SerialCharsAvail(SerialInfo *info) {	OSErr err;	long count;		// Check that we have an input driver.	if (info->inRefNum == -1) {		return -1L;	}		// Try to get a count.	// Return the count if no error, -1 otherwise.	err = SerGetBuf(info->inRefNum, &count);	if (err == 0) {		return count;	}	else {		return -1L;	}}/********************************************************************************																																							 	*  Function: GetSerialChars																										 *																																							 *  Purpose:  Reads input from serial port into input buffer	and appends a \0.									 *																																							 *******************************************************************************/long GetSerialChars(SerialInfo *info, long count)  {	long actual = 0L;	OSErr err;	// Check that this could possibly be done	if (info->inRefNum == -1 || count == 0L) {		return(actual);	}		// Another check	if (count > SERBUFSIZ) {		//printf("Requested more bytes than may be in the buffer\r");		return(actual);	}		// Get the characters into the allocated buffer	actual = count;	err = FSRead(info->inRefNum, &actual, info->inCharBuf);	if (err) {		printf("Error on FSRead in GetSerialChars\n");		exit;	}	info->inCharBuf[actual] = '\0';		// Error checks	if (actual != count) {		printf("Count mismatch during read in GetSerialChars\n");		exit;	}	if (actual > SERBUFSIZ) {		printf("Read more bytes than are allocated to buffer in GetSerialChars\n");		exit;	}		return(actual);}/********************************************************************************																																							 	*  Function: 	SerialWriteNL																											 *																																							 *  Purpose: Sends string to serial port with an appended new line character																				 **																																							 *******************************************************************************/long SerialWriteNL(SerialInfo *info, char *msg){	long actual = 0L;		actual = SerialWrite(info,msg);	actual += SerialWrite(info,"\n");	return actual;	}/********************************************************************************																																							 	*  Function: 	SerialWriteCR																											 *																																							 *  Purpose: Sends string to serial port with an appended new line character																				 **																																							 *******************************************************************************/long SerialWriteCR(SerialInfo *info, char *msg){	long actual = 0L;		actual = SerialWrite(info,msg);	actual += SerialWrite(info,"\r");	return actual;	}/********************************************************************************																																							 	*  Function: 	SerialWrite																											 *																																							 *  Purpose: Sends string to serial port																			 *																																							 *******************************************************************************/long SerialWrite(SerialInfo *info, char *snd){	OSErr err;	long count;	long actual = 0L;	// Get length to send	count = (long) strlen(snd);		// Send the string out the serial port	actual = count;	err = FSWrite(info->outRefNum, &actual, snd);	if (err) {		printf("Error on FSWrite in SerialWrite\n");		exit;	}	return actual;}/********************************************************************************																																							 	*  Function: 	SerialWriteN																										 *																																							 *  Purpose: Sends string to serial port																			 *																																							 *******************************************************************************/long SerialWriteN(SerialInfo *info, char *snd, int count){	OSErr err;	long actual = 0L;		// Send the string out the serial port	actual = (long) count;	err = FSWrite(info->outRefNum, &actual, snd);	if (err) {		printf("Error on FSWrite in SerialWriteN\n");		exit;	}	return actual;}