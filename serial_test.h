#ifndef __ANDROID_SERIAL_TEST_H__
#define __ANDROID_SERIAL_TEST_H__

#include "data.h"

void serial_test(AndriodProduct* product);
void serial_process(char* serial, AndriodProduct* product);
void serial_ttyS1_process_t(void* params);
void serial_ttyS3_process_t(void* params);


#endif //__ANDROID_SERIAL_TEST_H__