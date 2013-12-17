/*****************************************************************************
* University of Southern Denmark
*
* MODULENAME.: serial.c
*
* PROJECT....: EMB4 Project
*
* DESCRIPTION: See module specification file (.h-file).
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 131111  KeH	Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/

int serialInit(char *device)
{
	  int fd;
	  struct termios newtio;

	  fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY );
	  if (fd <0) {perror(device); exit(-1); }

	  bzero(&newtio, sizeof(newtio));
	  newtio.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
	  newtio.c_iflag = IGNPAR;
	  newtio.c_oflag = 0;

	  /* set input mode (non-canonical, no echo,...) */
	  newtio.c_lflag = 0;

	//  newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
	//  newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */

	  tcflush(fd, TCIFLUSH);
	  tcsetattr(fd,TCSANOW,&newtio);

	  return fd;
}

void serialWrite(int fd, char *data, int bytes)
{
  	write(fd,data,bytes);
}



void serialWriteSpeed(int fd, int speed)
{
	char speedBuf[5];
	sprintf(speedBuf, "%d ", speed);
  	write(fd,speedBuf,5);
}



int serialRead(int fd, char *data)
{
	  int res,n;
	  char buf[3];
	  res = read(fd,buf,2);   /* returns after 5 chars have been input */
	  buf[res]=0;               /* so we can printf... */
  	  sscanf(buf,"%d",&n);
	  return n;
}



void serialClose (int fd)
{
  	close(fd);
}
/****************************** End Of Module *******************************/
