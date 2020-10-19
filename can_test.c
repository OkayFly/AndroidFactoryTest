#include "can_test.h"

void can_test(AndriodProduct* product)
{
    can_process(CAN0Port);
    can_process(CAN1Port);
}

void can_process(char* canport)
{
    printf("**\t serail_process:%s\n", canport);
}