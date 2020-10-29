// SPDX-License-Identifier: MIT
// github linux-serial-test

#include "serial_test.h"

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <linux/serial.h>
#include <errno.h>
#include <stdbool.h>

#include "data.h"
#include "factory_test_function.h"


extern bool STOPTEST;
extern pthread_mutex_t mutex_sn;


/*
 * glibc for MIPS has its own bits/termios.h which does not define
 * CMSPAR, so we vampirise the value from the generic bits/termios.h
 */
#ifndef CMSPAR
#define CMSPAR 010000000000
#endif

/*
 * Define modem line bits
 */
#ifndef TIOCM_LOOP
#define TIOCM_LOOP	0x8000
#endif

#define UART_BAUD 9600

// USED
int _cl_rts_cts = 0;
int _cl_2_stop_bit = 0;
int _cl_parity = 0;

int _cl_no_rx = 0; //Don't receive data 
int _cl_no_tx = 0; //Dont't ransmit data
int _cl_loopback = 0; //

int _cl_rs485_rts_after_send = 0; //
int _cl_rs485_before_delay = 0; //
int _cl_rs485_after_delay = -1; //


int _cl_odd_parity = 0;
int _cl_stick_parity = 0;

static void set_baud_divisor(int speed, int custom_divisor, int fd)
{
	// default baud was not found, so try to set a custom divisor
	struct serial_struct ss;
	if (ioctl(fd, TIOCGSERIAL, &ss) < 0) {
		perror("TIOCGSERIAL failed");
		//exit(1);
		return;
	}

	ss.flags = (ss.flags & ~ASYNC_SPD_MASK) | ASYNC_SPD_CUST;
	if (custom_divisor) {
		ss.custom_divisor = custom_divisor;
	} else {
		ss.custom_divisor = (ss.baud_base + (speed/2)) / speed;
		int closest_speed = ss.baud_base / ss.custom_divisor;

		if (closest_speed < speed * 98 / 100 || closest_speed > speed * 102 / 100) {
			fprintf(stderr, "Cannot set speed to %d, closest is %d\n", speed, closest_speed);
			exit(1);
		}

		printf("closest baud = %i, base = %i, divisor = %i\n", closest_speed, ss.baud_base,
				ss.custom_divisor);
	}

	if (ioctl(fd, TIOCSSERIAL, &ss) < 0) {
		perror("TIOCSSERIAL failed");
		//exit(1);
		return;
	}
}

static void clear_custom_speed_flag(int fd)
{
	struct serial_struct ss;
	if (ioctl(fd, TIOCGSERIAL, &ss) < 0) {
		// return silently as some devices do not support TIOCGSERIAL
		return;
	}

	if ((ss.flags & ASYNC_SPD_MASK) != ASYNC_SPD_CUST)
		return;

	ss.flags &= ~ASYNC_SPD_MASK;

	if (ioctl(fd, TIOCSSERIAL, &ss) < 0) {
		perror("TIOCSSERIAL failed");
		//exit(1);
	}
}

// converts integer baud to Linux define
static int get_baud(int baud)
{
	switch (baud) {
	case 9600:
		return B9600;
	case 19200:
		return B19200;
	case 38400:
		return B38400;
	case 57600:
		return B57600;
	case 115200:
		return B115200;
	case 230400:
		return B230400;
	case 460800:
		return B460800;
	case 500000:
		return B500000;
	case 576000:
		return B576000;
	case 921600:
		return B921600;
#ifdef B1000000
	case 1000000:
		return B1000000;
#endif
#ifdef B1152000
	case 1152000:
		return B1152000;
#endif
#ifdef B1500000
	case 1500000:
		return B1500000;
#endif
#ifdef B2000000
	case 2000000:
		return B2000000;
#endif
#ifdef B2500000
	case 2500000:
		return B2500000;
#endif
#ifdef B3000000
	case 3000000:
		return B3000000;
#endif
#ifdef B3500000
	case 3500000:
		return B3500000;
#endif
#ifdef B4000000
	case 4000000:
		return B4000000;
#endif
	default:
		return -1;
	}
}

bool set_modem_lines(int fd, int bits, int mask)
{
	int status, ret;

	ret = ioctl(fd, TIOCMGET, &status);
	if (ret < 0) {
		perror("TIOCMGET failed");
		//exit(1);
		return false;
	}

	status = (status & ~mask) | (bits & mask);

	ret = ioctl(fd, TIOCMSET, &status);
	if (ret < 0) {
		perror("TIOCMSET failed");
		//exit(1);
		return false;
	}

	return true;
}


static bool setup_serial(int baud, char* port, int *fd)
{
	struct termios newtio;
	struct serial_rs485 rs485;

	*fd = open(port, O_RDWR | O_NONBLOCK);

	if (*fd < 0) {
		perror("Error opening serial port");
		//free(_cl_port1);?????????????????TODO
		//exit(1);
		return false;
	}

	bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */

	/* man termios get more info on below settings */
	newtio.c_cflag = baud | CS8 | CLOCAL | CREAD;

	if (_cl_rts_cts) {
		newtio.c_cflag |= CRTSCTS;
	}

	if (_cl_2_stop_bit) {
		newtio.c_cflag |= CSTOPB;
	}

	if (_cl_parity) {
		newtio.c_cflag |= PARENB;
		if (_cl_odd_parity) {
			newtio.c_cflag |= PARODD;
		}
		if (_cl_stick_parity) {
			newtio.c_cflag |= CMSPAR;
		}
	}

	newtio.c_iflag = 0;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0;

	// block for up till 128 characters
	newtio.c_cc[VMIN] = 128;

	// 0.5 seconds read timeout
	newtio.c_cc[VTIME] = 5;

	/* now clean the modem line and activate the settings for the port */
	tcflush(*fd, TCIOFLUSH);
	tcsetattr(*fd,TCSANOW,&newtio);

	/* enable/disable rs485 direction control */
	if(ioctl(*fd, TIOCGRS485, &rs485) < 0) {
		if (_cl_rs485_after_delay >= 0) {
			/* error could be because hardware is missing rs485 support so only print when actually trying to activate it */
			//perror("Error getting RS-485 mode");
		}
	} else if (_cl_rs485_after_delay >= 0) {
		rs485.flags |= SER_RS485_ENABLED | SER_RS485_RX_DURING_TX |
			(_cl_rs485_rts_after_send ? SER_RS485_RTS_AFTER_SEND : SER_RS485_RTS_ON_SEND);
		rs485.flags &= ~(_cl_rs485_rts_after_send ? SER_RS485_RTS_ON_SEND : SER_RS485_RTS_AFTER_SEND);
		rs485.delay_rts_after_send = _cl_rs485_after_delay;
		rs485.delay_rts_before_send = _cl_rs485_before_delay;
		if(ioctl(*fd, TIOCSRS485, &rs485) < 0) {
			//perror("Error setting RS-485 mode");
		}
	} else {
		rs485.flags &= ~(SER_RS485_ENABLED | SER_RS485_RTS_ON_SEND | SER_RS485_RTS_AFTER_SEND);
		rs485.delay_rts_after_send = 0;
		rs485.delay_rts_before_send = 0;
		if(ioctl(*fd, TIOCSRS485, &rs485) < 0) {
			//perror("Error setting RS-232 mode");
		}
	}

	return true;
}

static void reply_end(AndriodProduct* product, int fd)
{
	printf("!~~~%s__\n",__func__);
	char write_data[120];
	write_data[0] = 0xAA;
	write_data[1] = CTRL_GET_END;
	pthread_mutex_lock (&mutex_sn);
	memcpy(write_data+2, product->cpu_sn, strlen(product->cpu_sn));
	write_data[2+strlen(product->cpu_sn)] = 0x55;
	write_data[3+strlen(product->cpu_sn)] = '\0';
	pthread_mutex_unlock (&mutex_sn);
	
	printf("\n\t write_data:%s\n",write_data);
	for(int i=0; i<strlen(write_data); i++)
	{
		printf("%02x ", write_data[i]);
	}
	
	printf("\n");
	ssize_t c =write(fd, write_data, strlen(write_data));
	usleep(50000);
}

static void reply_sn(AndriodProduct* product, int fd)
{
	printf("~~~%s__\n",__func__);
	char write_data[120];
	write_data[0] = 0xAA;
	write_data[1] = CTRL_GET_MAC;
	memcpy(write_data+2, product->cpu_sn, strlen(product->cpu_sn));
	write_data[2+strlen(product->cpu_sn)] = 0x55;
	write_data[3+strlen(product->cpu_sn)] = '\0';
	printf("\n\t write_data:%s\n",write_data);
	for(int i=0; i<strlen(write_data); i++)
	{
		printf("%02x ", write_data[i]);
	}
	
	printf("\n");
	ssize_t c =write(fd, write_data, strlen(write_data));
	usleep(50000);
}





static int serial_process_read_data(AndriodProduct* product, char * buff, int* buff_len, int fd, fsm_state_t* fsm)
{

	unsigned char rb[95] = {};
	unsigned char data[95] = {};
	int data_length;
	int c = read(fd, &rb, sizeof(rb));

	if(c<= 0)
	{
		//printf("return -1\n");
		return -1;

	}

	printf(":c:%d\n",c);
	fflush(stdout);
	for(int i=0; i<c; i++)
	{
		printf("[%02x]",rb[i]);
	}

	printf("\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//put int serail buffer //TODO 超过了怎么说
	*buff_len = strlen(buff);
	memcpy(&buff[*buff_len], rb, c);
	*buff_len += c;

	if(parse_data(buff, *buff_len, data, &data_length) == DATA_PROCESS_SUCCESS)
	{
		
		printf("get data:%s, length[%d]\n",data, data_length);

		for(int i=0; i<data_length; i++)
		{
			printf("\t\t %02x ", data[i]);
		}
		process_data(data, data_length, product, fsm);

		if(*fsm != FSM_GET_END)
			reply_sn(product, fd);

		memset(buff,0,1024);
		*buff_len = 0;
	}
//printf("return 0\n");
	return 0;
}




void serial_process_t(void* params)
{

	printf_func_mark(__func__);
	parameters *data = (parameters*) params;

	int baud = 115200;
#ifdef UART_BAUT
	baud = UART_BAUD;
#endif
	baud = get_baud(baud);


	char *serial_port;
	serial_port = data->port;

	fsm_state_t*  fsm;
	if( !strcmp(serial_port, TTYS1Port))
	{
		printf("@@@1\n");
		fsm = &data->product->TTYS1;
	}
	else if(!strcmp(serial_port, TTYS3Port))
	{
		printf("@@@1\n");
		fsm = &data->product->TTYS3;
	}
	else
	{
		fsm = &data->product->TTYS1;
		printf("????????????????wtftttyUSB0\n");
	}
	

	int serial_fd = -1;

	if(!setup_serial(baud,serial_port, &serial_fd))
	{
		perror("set serial port baud");
		printf("port:%s\n",serial_port);
		free(data->port);
		return;
		//	exit;//??????????????
	}

/////////////////////////////////////////////////////////////////////////////////////////
	//clear_custom_speed_flag
	struct serial_struct ss;
	if (ioctl(serial_fd, TIOCGSERIAL, &ss) < 0) {
		// return silently as some devices do not support TIOCGSERIAL
		return;
	}
	if ((ss.flags & ASYNC_SPD_MASK) != ASYNC_SPD_CUST)
	{
		printf("222 wtf todo\n");
	}
		//return;
	ss.flags &= ~ASYNC_SPD_MASK;
	if (ioctl(serial_fd, TIOCSSERIAL, &ss) < 0) {
		perror("TIOCSSERIAL failed");
		//exit(1);
	}
	printf("3333\n");
	if(!set_modem_lines(serial_fd, _cl_loopback ? TIOCM_LOOP : 0, TIOCM_LOOP))
	{
		perror("et_modem_lines");
		return;
		//	exit;//??????????????

	}

/////////////////////////////////////////////////////////////////////////////////////////
	struct pollfd serial_poll;
	serial_poll.fd = serial_fd;
	if(!_cl_no_rx)
		serial_poll.events |= POLLIN;
	else
	{
		serial_poll.events &= ~POLLIN;
	}
	if(!_cl_no_tx)
		serial_poll.events |= POLLOUT;
	else
	{
		serial_poll.events &= ~POLLOUT;
	}


	printf("** \t wait receive cpu ID\n");
	struct timespec  last_read;
	clock_gettime(CLOCK_MONOTONIC, &last_read);


	struct timespec current_time;
	char serial_buff[1024] = {0};
	int  buff_len =0;


	while(1)
	{
		int retval = poll(&serial_poll, 1, 1000);//?????
		if(retval == -1)
		{
			perror("**\t poll()");
		}
		else if(retval)
		{
			switch (*fsm)
			{
			case FSM_IDLE:
				if(serial_process_read_data(data->product, serial_buff, &buff_len, serial_fd, fsm)== 0)
					clock_gettime(CLOCK_MONOTONIC, &last_read);
				break;
			case FSM_GET_MAC:
				if(serial_process_read_data(data->product, serial_buff, &buff_len, serial_fd, fsm)==0)
					clock_gettime(CLOCK_MONOTONIC, &last_read);
				break;
			case FSM_GET_END:
				reply_end(data->product, serial_fd);
				wait_save(data->port);
				break;
			
			default:
				break;
			}
			
		}


		clock_gettime(CLOCK_MONOTONIC, &current_time);
		int  consum_tm = diff_ms(&current_time,&last_read);

		if(consum_tm >= 20000 )//20s//还有一直受到垃圾消息  不能通过last_read
		{
			printf("\t\t Error %s time consuming >60s but can't receive corrent data\n", data->port);
			clock_gettime(CLOCK_MONOTONIC, &last_read);
			*fsm = FSM_IDLE;
			memset(serial_buff, 0, 1024);
			buff_len = 0;
		}
		

	}

	//close serial
	printf("**\t close serial:%s\n", data->port);
	fflush(stdout);
	tcdrain(serial_fd);
	set_modem_lines(serial_fd, 0, TIOCM_LOOP);
	tcflush(serial_fd, TCIOFLUSH);
	free(serial_port);
	return;
}