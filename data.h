#ifndef __ANDROID_DATA_H__
#define __ANDROID_DATA_H__

#include<stdbool.h>

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


typedef struct 
{
    unsigned char cpu_sn[20]; //cpu serial number
    bool TTYS1;
    bool TTYS3;
    bool CAN0;
    bool CAN1;
    bool SAMECPU;
    char cur_cmd;
}AndriodProduct;

typedef enum {
    CTRL_GET_TTYS1_MAC = 0x80,
    CTRL_GET_TTYS3_MAC ,
    CTRL_GET_CAN0_MAC ,
    CTRL_GET_CAN1_MAC ,
    CTRL_END,
}ctrl_t;

DPStatus parse_data( unsigned char* in,  int length,  unsigned char* out, int* out_length);
void process_data( unsigned char* data, int length, AndriodProduct* product);
void get_mac( unsigned char* data, int length, AndriodProduct* AndriodProduct);
void save_data(unsigned char* data, unsigned char* name);

void save_test_result(AndriodProduct* product);

#endif//__ANDROID_DATA_H__