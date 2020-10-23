// SPDX-License-Identifier: MIT

#include <stddef.h>

#include "serial_test.h"
#include "data.h"
#include "serial_test.h"
#include "can_test.h"
#include <pthread.h>
#include <string.h>




int main(int argc, char * argv[])
{
	// ====== create the product ====
	AndriodProduct my_android = {'\0', 0, 0, 0, 0, true, '\0'};
	// ====== create their threads parameters ===
	parameters *paramTTYS1 = (parameters*) malloc(sizeof(parameters));
	//memcpy(paramTTYS1->port, TTYS1Port, strlen(TTYS1Port));
	memcpy(paramTTYS1->port, USBPort, strlen(USBPort));
	paramTTYS1->product = &my_android;

	parameters *paramTTYS3 = (parameters*) malloc(sizeof(parameters));
	memcpy(paramTTYS3->port, TTYS3Port, strlen(TTYS3Port));
	paramTTYS3->product = &my_android;

	// parameters *paramCAN0 = (parameters*) malloc(sizeof(parameters));
	// memcpy(paramCAN0->port, CAN0Port, strlen(CAN0Port));
	// paramCAN0->product = &my_android;

	// parameters *paramCAN1 = (parameters*) malloc(sizeof(parameters));
	// memcpy(paramCAN1->port, CAN1Port, strlen(CAN1Port));
	// paramCAN1->product = &my_android;

	parameters *paramSave = (parameters*) malloc(sizeof(parameters));
	paramSave->product = &my_android;



	

printf("______________________________________________________________________________________\n");
printf("______________________________________________________________________________________\n\n");
printf("\tAndroid factory test ===>>>\n");
printf("______________________________________________________________________________________\n");
printf("______________________________________________________________________________________\n\n");

// 4个线程
// ====== Create the thread ======
pthread_t thread_ttys1, thread_ttys3, 
			thread_can0, thread_can1,
			thread_save;

// ====== Create the return values for the threads ======
void* ttys1Value;
void* ttys3Value;
void* can0Value;
void* can1Value;
void* saveValue;

// ====== Initialize the thread ======
pthread_create(&thread_ttys1, NULL, serial_ttyS1_process_t, (void *)paramTTYS1);
pthread_create(&thread_ttys3, NULL, serial_ttyS3_process_t, (void *)paramTTYS3);
// pthread_create(&thread_can0, NULL, can_process_t, (void *)paramCAN0);
// pthread_create(&thread_can1, NULL, can_process_t, (void *)paramCAN1);
pthread_create(&thread_save, NULL, save_process_t, (void *)paramSave);

// ====== Wait for all thread to finish thieir tasks ======
pthread_join(thread_ttys1, &ttys1Value);
pthread_join(thread_ttys3, &ttys3Value);
// pthread_join(thread_can0, &can0Value);
// pthread_join(thread_can1, &can1Value);

pthread_join(thread_save, &saveValue);



return 0;






// 改用多线程
	// char userinput[LENUSERINPUT];
	// struct command* cmd;

	// while(1)
	// {
	// 	//printf("\t^_^ please input [START] and click [Enter] end for the [%d]'s factory test^_^\t\n:", ++test_count);
	// 	//fgets(userinput, LENUSERINPUT, stdin);
	// 	//cmd = userinputtocommand(userinput);
	// 	//if(cmd == NULL)
	// 	//	continue;
	// 	AndriodProduct my_android = {'\0', 0, 0, 0, 0, true, '\0'};
	// 	printf("***************************************************\n");
	// 	printf("***************************************************\n");
	// 	printf("***************  Step1: serial_process  ***********\n");
	// 	printf("***************************************************\n");
	// 	printf("*****************************************\n");
	// 	serial_test(&my_android);
	// 	can_test(&my_android);

	// 	if( strlen(my_android.cpu_sn) != 0)
	// 	{
	// 		save_test_result(&my_android);

	// 	}
		

	// }
	

	
	
	
}
