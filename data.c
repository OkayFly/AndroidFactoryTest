#include "data.h"
#include "stdbool.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"


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


void process_data(unsigned char* data, int length,AndriodProduct* product)
{
    switch (data[0])
    {
    case CTRL_GET_TTYS1_MAC:
        get_mac(data+1, length-1, product);
        product->cur_cmd = CTRL_GET_TTYS1_MAC;
        break;
    case CTRL_GET_TTYS3_MAC:
        get_mac(data+1, length-1, product);
        product->cur_cmd = CTRL_GET_TTYS3_MAC;
        break;
    case CTRL_GET_CAN0_MAC:
        get_mac(data+1, length-1, product);
        product->cur_cmd = CTRL_GET_CAN0_MAC;
        break;
    case CTRL_GET_CAN1_MAC:
        get_mac(data+1, length-1, product);
        product->cur_cmd = CTRL_GET_CAN1_MAC;
        break;
    default:
        break;
    }

}

void get_mac(unsigned char* data, int length, AndriodProduct* product)
{

    printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> %s \t\t\n", __func__);
    // for(int i=0; i<length; i++)
    // {
    //     printf("\t\t %02x \t\t", data[i]);
    // }
    if( strlen(product->cpu_sn) != 0 && strncmp(product->cpu_sn, data, length))
    {
        printf("\t\tError The Android product is not same!!!\n");
        printf("product cpu sn:%s\n", product->cpu_sn);
        printf("data:%s\n",data);
        product->SAMECPU = false;
    }
    else if(strlen(product->cpu_sn) == 0)
    {
        memcpy(product->cpu_sn, data, length);
        printf("product cpu sn:%s\n", product->cpu_sn);
    }
    STOPTEST = true;
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

void save_test_result(AndriodProduct* product)
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
    fprintf(fp, "%s:[%s]\n",TTYS1Port ,product->TTYS1 ? "OK":"Fail");
    fprintf(fp, "%s:[%s]\n",TTYS3Port ,product->TTYS3 ? "OK":"Fail");
    fprintf(fp, "%s:[%s]\n",CAN0Port ,product->CAN0 ? "OK":"Fail");
    fprintf(fp, "%s:[%s]\n",CAN1Port ,product->CAN1 ? "OK":"Fail");
    fclose(fp);
    for(int i=0; i<30;i++)
    {
        printf(">");
    }
    printf("%s\n",filename);
    printf("save %s ok\n",filename);
    free(filename);
}
