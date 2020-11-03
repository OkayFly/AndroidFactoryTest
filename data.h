#ifndef __ANDROID_DATA_H__
#define __ANDROID_DATA_H__

#include<stdbool.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>

#define TTYS3Port "/dev/ttyS3"
#define TTYS1Port "/dev/ttyS1"
#define USBPort "/dev/ttyUSB0"
#define CAN0Port "can0"
#define CAN1Port "can1"

#define NCOMMANDS 2
enum COMMAND
{
    START,
    END,
};

struct command
{
    short int comid;
    short int count;
};


typedef enum DataProcessStatus
{
    DATA_PROCESS_SUCCESS,
    DATA_HEAD_ERROR,
    DATA_TAIL_ERROR,

}DPStatus;

typedef enum 
{
    FSM_IDLE,
    FSM_GET_MAC,
    FSM_GET_END,
    FSM_TEST_OK,
    FSM_TEST_FAIL,
}fsm_state_t;


typedef struct 
{
    unsigned char cpu_sn[20]; //cpu serial number
    fsm_state_t TTYS1;
    fsm_state_t TTYS3;
    fsm_state_t CAN0;
    fsm_state_t CAN1;
    bool SAMESN;
}AndriodProduct;


typedef struct 
{
    char port[14];//open which port
    AndriodProduct* product; //The point to the product  

}parameters;

// no disting the tty1 2 can0 can1 one to one 
typedef enum {
    CTRL_GET_MAC = 0x80,
    CTRL_GET_END ,
    CTRL_GET_IDLE,
}ctrl_t;

DPStatus parse_data( unsigned char* in,  int length,  unsigned char* out, int* out_length);
bool process_data( unsigned char* data, int length, parameters* param, fsm_state_t* serial_fsm);
bool get_sn(unsigned char* data, int length, parameters* param, fsm_state_t* fsm);
bool get_end( unsigned char* data, int length, parameters* param, fsm_state_t* serial_fsm);
bool get_idle( unsigned char* data, int length,parameters* param, fsm_state_t* serial_fsm);

void save_data(unsigned char* data, unsigned char* name);

void save_test_result(AndriodProduct* product);

void save_process_t(void* params);

#endif//__ANDROID_DATA_H__