#include "data.h"
#include "stdbool.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include <pthread.h>


//mutext
pthread_mutex_t mutex_sn =  PTHREAD_MUTEX_INITIALIZER; //init mutex

#define LENGTHBUF 1024
#define DATA_HEAD (0xAA)
#define DATA_TAIL (0x55)

bool STOPTEST = false;
//#define DATA_TRANSLATE (0xCC)



DPStatus parse_data( unsigned char* in,  int length, unsigned char* out, int* out_length)
{
    bool has_head = false;
    int  head_index = 0;

    for(int i=0; i<length; i++)
    {
        if(in[i] == DATA_HEAD)
        {
            has_head = true;
            head_index = i;
            break;
        }
    }

    if(!has_head)
        return DATA_HEAD_ERROR;
    
    int  tail_index =0;

    for(int i=head_index+1; i<length; i++)
    {
        if(in[i] == DATA_TAIL)
        {
            tail_index = i;
            break;
        }
    }

    if(tail_index <1)
        return DATA_TAIL_ERROR;

    *out_length = tail_index - head_index-1;
    printf("tail_index:%d, *outlength:%d\n", tail_index, *out_length);

    memcpy(out, in+head_index+1, *out_length);
    
    return DATA_PROCESS_SUCCESS;
}


void process_data(unsigned char* data, int length,AndriodProduct* product, fsm_state_t* fsm)
{
    switch (data[0])
    {
    case CTRL_GET_MAC:
        get_sn(data+1, length-1, product, fsm);
        break;
    case CTRL_GET_END:
        get_end(data+1, length-1, product, fsm);
        break;
    case CTRL_GET_IDLE:
        get_idle(data+1, length-1, product, fsm);
        break;  

    default:
        break;
    }

}

void get_sn(unsigned char* data, int length, AndriodProduct* product, fsm_state_t* fsm)
{

    
    printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> %s \t\t\n", __func__);
    // for(int i=0; i<length; i++)
    // {
    //     printf("\t\t %02x \t\t", data[i]);
    // }

    if(*fsm != FSM_IDLE)
    {
        printf("!!!!!!!!!!!!!!wfk\n");

        printf("Fsm:%d\n", *fsm);
    }
    pthread_mutex_lock (&mutex_sn);
    if( strlen(product->cpu_sn) != 0 && strncmp(product->cpu_sn, data, length))
    {
        printf("\t\tError The Android product is not same!!!\n");
        printf("product cpu sn:%s\n", product->cpu_sn);
        printf("data:%s\n",data);
        product->SAMESN = false;
    }
    else if(strlen(product->cpu_sn) == 0)
    {
        memcpy(product->cpu_sn, data, length);
        printf("product cpu sn:%s\n", product->cpu_sn);
    }

    *fsm = FSM_GET_MAC;
    pthread_mutex_unlock (&mutex_sn);
}

void get_end(unsigned char* data, int length, AndriodProduct* product, fsm_state_t* fsm)
{

    printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> %s \t\t\n", __func__);

    *fsm = FSM_GET_END;
    // for(int i=0; i<length; i++)
    // {
    //     printf("\t\t %02x \t\t", data[i]);
    // }
    // if( strlen(product->cpu_sn) != 0 && strncmp(product->cpu_sn, data, length))
    // {
    //     printf("\t\tError The Android product is not same!!!\n");
    //     printf("product cpu sn:%s\n", product->cpu_sn);
    //     printf("data:%s\n",data);
    //     product->SAMESN = false;
    // }
    // else if(strlen(product->cpu_sn) == 0)
    // {
    //     memcpy(product->cpu_sn, data, length);
    //     printf("product cpu sn:%s\n", product->cpu_sn);
    // }
    // STOPTEST = true;
    //save_data(data, data);
}

void get_idle(unsigned char* data, int length, AndriodProduct* product, fsm_state_t* fsm)
{

    printf("\n>>mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm %s \t\t\n", __func__);
     printf("\n>>mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm %s \t\t\n", __func__);
      printf("\n>>mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm %s \t\t\n", __func__);

    *fsm = FSM_TEST_OK;
    // for(int i=0; i<length; i++)
    // {
    //     printf("\t\t %02x \t\t", data[i]);
    // }
    // if( strlen(product->cpu_sn) != 0 && strncmp(product->cpu_sn, data, length))
    // {
    //     printf("\t\tError The Android product is not same!!!\n");
    //     printf("product cpu sn:%s\n", product->cpu_sn);
    //     printf("data:%s\n",data);
    //     product->SAMESN = false;
    // }
    // else if(strlen(product->cpu_sn) == 0)
    // {
    //     memcpy(product->cpu_sn, data, length);
    //     printf("product cpu sn:%s\n", product->cpu_sn);
    // }
    // STOPTEST = true;
    //save_data(data, data);
}

void save_data(unsigned char* data, unsigned char* name)
{
    printf("\n** \t\t %s\n",__func__);
    FILE *fp;
    unsigned  char* filename = (unsigned char*)malloc(strlen(name)+5);
    if(filename == NULL)
        printf("error maclloc\n");
    //fprintf(filename, "%s.txt",name);
    memcpy(filename, name, strlen(name));
    memcpy(filename+ strlen(name), ".txt", sizeof(".txt"));
    printf("filename:%s\n",filename);

    if((fp = fopen(filename, "wb")) == NULL)
    {
        printf("\t\tXXX error cant open %s\n",filename);
        return;
    }

    fprintf(fp, "%s",data);
    fclose(fp);
    for(int i=0; i<30;i++)
    {
        printf(">");
    }
    printf("%s\n",filename);
    printf("save %s ok\n",filename);

}

void product_save_result(AndriodProduct* product)
{
    printf("\n** \t\t %s\n",__func__);
    FILE *fp;
    unsigned  char* filename = (unsigned char*)malloc(strlen(product->cpu_sn)+5);
    if(filename == NULL)
        printf("error maclloc\n");
    //fprintf(filename, "%s.txt",name);
    memcpy(filename, product->cpu_sn, strlen(product->cpu_sn));
    memcpy(filename+ strlen(product->cpu_sn), ".txt", sizeof(".txt"));
    printf("filename:%s\n",filename);

    if((fp = fopen(filename, "wb")) == NULL)
    {
        printf("\t\tXXX error cant open %s\n",filename);
        return;
    }
    fprintf(fp, "%s:\n",product->cpu_sn);
    fprintf(fp, "%s:[%s]\n",TTYS1Port == FSM_TEST_OK, product->TTYS1 ? "OK":"Fail");
    fprintf(fp, "%s:[%s]\n",TTYS3Port == FSM_TEST_OK, product->TTYS3 ? "OK":"Fail");
    fprintf(fp, "%s:[%s]\n",CAN0Port == FSM_TEST_OK, product->CAN0 ? "OK":"Fail");
    fprintf(fp, "%s:[%s]\n",CAN1Port == FSM_TEST_OK, product->CAN1 ? "OK":"Fail");
    fclose(fp);
    for(int i=0; i<30;i++)
    {
        printf(">");
    }
    printf("%s\n",filename);
    printf("save %s ok\n",filename);
    free(filename);
}

static bool  product_test_complete(AndriodProduct* product)
{

    if(product->SAMESN && (product->TTYS1 == FSM_TEST_OK || product->TTYS1 == FSM_TEST_FAIL) 
                        && (product->TTYS3 == FSM_TEST_OK || product->TTYS3 == FSM_TEST_FAIL)
                         &&  (product->CAN0 == FSM_TEST_OK || product->CAN0 == FSM_TEST_FAIL) 
                         && ( product->CAN1 == FSM_TEST_OK || product->CAN1 == FSM_TEST_FAIL)
    )
    {
            printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^product_test_complete^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
                printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^product_test_complete^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
                    printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^product_test_complete^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
                        printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^product_test_complete^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");

                            printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^product_test_complete^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
                                printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^product_test_complete^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");

        return true;
    }
    else
    {
        printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^product_test_completeproduct->SAMESN:[ %d ] \n",product->SAMESN);
    printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^product_test_completeproduct->TTYS1:[ %d ]\n",product->TTYS1);
        printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^product_test_completeproduct->TTYS3:[ %d ]\n", product->TTYS3);
            printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^product_test_completproduct->CAN0:[ %d ]\n", product->CAN0);
             printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^product_test_completproduct->CAN1:[ %d ]\n", product->CAN1);


    }
    
    
    return false;
}


void product_clear(AndriodProduct* product)
{
    pthread_mutex_lock (&mutex_sn);
    memset(product->cpu_sn, 0, 20);
    pthread_mutex_unlock (&mutex_sn);
    
    product->SAMESN = true;
    product->TTYS1 = FSM_IDLE;
    product->TTYS3 = FSM_IDLE;
    product->CAN0 = FSM_IDLE;
    product->CAN1 = FSM_IDLE;
}

void save_process_t(void* params)
{

    parameters *data = (parameters*) params;
	printf_func_mark(__func__);

    while(1)
    {
        //
        // pthread_mutex_lock(&mutex);
        if(product_test_complete(data->product))
        {
            product_save_result(data->product);
            product_clear(data->product);
        }
        usleep(1000000);
        // pthread_mutex_unlock(&mutex);

    }

}