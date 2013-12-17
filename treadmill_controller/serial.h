/*****************************************************************************
* University of Southern Denmark
*
* MODULENAME.: serial.h
*
* PROJECT....: EMB4 Project
*
* DESCRIPTION: Driver for serial communication
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 131111  KeH	Module created
*
*****************************************************************************/

#ifndef _SERIAL_H
  #define _SERIAL_H

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/

int serialInit(char *device);
void serialWrite(int fd, char *data, int bytes);
void serialWriteSpeed(int fd, int speed);
int serialRead(int fd, char *data);
void serialClose(int fd);

/****************************** End Of Module *******************************/
#endif
