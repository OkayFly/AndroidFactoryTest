#include "can_test.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include "data.h"
#include <time.h>

extern bool HASCPUSN;
extern pthread_mutex_t mutex_sn;
const int canfd_on = 1;

static void reply_sn( parameters* param, int s, fsm_state_t *fsm) // just use 8 bytes for write
{
    printf("-------%s:%s\n",param->port,__func__);

    unsigned char cpu_sn[20] = {0};
    pthread_mutex_lock (&mutex_sn);
    if( strlen(param->product->cpu_sn) != 0 )
    {
        memcpy(cpu_sn, param->product->cpu_sn, strlen(param->product->cpu_sn));
    }
    else
    {
        printf("\n***  XXXXXXXXXXXXXX%s:  strlen(param->product->cpu_sn) == 0 wtf\n", param->port);
    }

    pthread_mutex_unlock (&mutex_sn);

    struct can_frame frame_send;

    int cursor = 0;
    /* configure can_id and can data length */
    frame_send.can_id = 0x88;
    frame_send.can_dlc = 8;
    //printf(" ID=%#x data length=%d\n", "ifr.ifr_name", frame_send.can_id, frame_send.can_dlc);
    /* prepare data for sending: 0xAA,0x81,0x11,0x11,0x11,0x11,0x11,0x55 */
    //printf("sizeof can_frame.data:%d\n",sizeof(frame_send.data));

    //第一帧
    frame_send.data[0] = 0xAA;
    frame_send.data[1] = CTRL_GET_MAC;
    for(int i=2;i<8;i++)//6
    {
        frame_send.data[i] = cpu_sn[cursor++];
    }
    // for (int i=0; i<8; i++) 
    // {
    //     printf("%02x ", frame_send.data[i]);
    // }
    // printf("success to Sent out\n");
    /* Sending data */
    if(write(s, &frame_send, sizeof(frame_send)) < 0)
    {
    perror("Send failed");
    sleep(1);
    //close(s);
    //exit(-4);
    return ;
    }
    usleep(100);

    //第2帧
    for(int i=0;i<8;i++) //8
    {
        frame_send.data[i] =  cpu_sn[cursor++];
    }
    // for (int i=0; i<8; i++)
    // {
    //     printf("%02x ", frame_send.data[i]);
    // }
    // printf("success to Sent out\n");
    /* Sending data */
    if(write(s, &frame_send, sizeof(frame_send)) < 0)
    {
    perror("Send failed");
    sleep(1);
    //close(s);
    //exit(-4);
    return ;
    }

        usleep(100);

    //第3帧
    frame_send.can_dlc = 3;  //2
    frame_send.data[0] =  cpu_sn[cursor++];
    frame_send.data[1] =  cpu_sn[cursor++];
    frame_send.data[2] = 0x55;

    
    // for (int i=0; i<2; i++)
    // {
    //     // frame_send.data[i] = ((i+1)<<4) | (i+1);
    //     //         frame_send.data[7] =number;
    //     // printf("%#x ", frame_send.data[i]);
    //     printf("%02x ", frame_send.data[i]);
    // }
    // printf("success to Sent out\n");
    /* Sending data */
    if(write(s, &frame_send, sizeof(frame_send)) < 0)
    {
    perror("Send failed");
    sleep(1);
    //close(s);
    //exit(-4);
    return ;
    }
    //printf("~~~~~~~~~~~~send over\n");
    return;

}

static void reply_end( parameters* param, int s, fsm_state_t *fsm) // just use 8 bytes for write
{
    printf("-------%s:%s\n",param->port,__func__);

    unsigned char cpu_sn[20] = {0};
    pthread_mutex_lock (&mutex_sn);
    if( strlen(param->product->cpu_sn) != 0 )
    {
        memcpy(cpu_sn, param->product->cpu_sn, strlen(param->product->cpu_sn));
    }
    else
    {
        printf("\n***  XXXXXXXXXXXXXX%s:  strlen(param->product->cpu_sn) == 0 wtf\n", param->port);
    }

    pthread_mutex_unlock (&mutex_sn);

    struct can_frame frame_send;

    int cursor = 0;
    /* configure can_id and can data length */
    frame_send.can_id = 0x88;
    frame_send.can_dlc = 8;
    //printf("%s ID=%#x data length=%d\n", "ifr.ifr_name", frame_send.can_id, frame_send.can_dlc);
    /* prepare data for sending: 0xAA,0x81,0x11,0x11,0x11,0x11,0x11,0x55 */
   // printf("sizeof can_frame.data:%d\n",sizeof(frame_send.data));

    //第一帧
    frame_send.data[0] = 0xAA;
    frame_send.data[1] = CTRL_GET_END;
    for(int i=2;i<8;i++)//6
    {
        frame_send.data[i] = cpu_sn[cursor++];
    }
    // for (int i=0; i<8; i++) 
    // {
    //     printf("%02x ", frame_send.data[i]);
    // }
    // printf("success to Sent out\n");
    /* Sending data */
    if(write(s, &frame_send, sizeof(frame_send)) < 0)
    {
    perror("Send failed");
    sleep(1);
    //close(s);
    //exit(-4);
    return ;
    }
    usleep(100);

    //第2帧
    for(int i=0;i<8;i++) //8
    {
        frame_send.data[i] =  cpu_sn[cursor++];
    }
    // for (int i=0; i<8; i++)
    // {
    //     printf("%02x ", frame_send.data[i]);
    // }
    // printf("success to Sent out\n");
    /* Sending data */
    if(write(s, &frame_send, sizeof(frame_send)) < 0)
    {
    perror("Send failed");
    sleep(1);
    //close(s);
    //exit(-4);
    return ;
    }

        usleep(100);

    //第3帧
    frame_send.can_dlc = 3;  //2
    frame_send.data[0] =  cpu_sn[cursor++];
    frame_send.data[1] =  cpu_sn[cursor++];
    frame_send.data[2] = 0x55;

    
    // for (int i=0; i<2; i++)
    // {
    //     // frame_send.data[i] = ((i+1)<<4) | (i+1);
    //     //         frame_send.data[7] =number;
    //     // printf("%#x ", frame_send.data[i]);
    //     printf("%02x ", frame_send.data[i]);
    // }
    // printf("success to Sent out\n");
    /* Sending data */
    if(write(s, &frame_send, sizeof(frame_send)) < 0)
    {
    perror("Send failed");
    sleep(1);
    //close(s);
    //exit(-4);
    return ;
    }
  //  printf("~~~~~~~~~~~~send over\n");
    return;

}

static int can_process_read_data(parameters* param, char *buff, int *buff_len, char* port, int s, fsm_state_t *fsm)
{
    struct timeval timeout, timeout_config = { 0, 0 }, *timeout_current = NULL;
    timeout_config.tv_usec = 1000;//msecs// -T <msecs>  (terminate after <msecs> without any reception)\n"); //20s
    
    //???????????
    timeout_config.tv_sec = timeout_config.tv_usec / 1000;
    timeout_config.tv_usec = (timeout_config.tv_usec % 1000) * 1000;
    timeout_current = &timeout;

    fd_set rdfs;

    /* set filter for only receiving packet with can id 0x88 */
    struct can_frame frame;
    struct can_filter rfilter[1];
    rfilter[0].can_id = 0x88;
    rfilter[0].can_mask = CAN_SFF_MASK;
    if(setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter)) < 0)
    {
        perror("set receiving filter error\n");
       // close(s);
        //exit(-3);
        return -3;
    }

        FD_ZERO(&rdfs);
        FD_SET(s,&rdfs);

        
		if (timeout_current)
		*timeout_current = timeout_config;
       // printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~select~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

        //???????????s+1
		if ((select(s+1, &rdfs, NULL, NULL, timeout_current)) <= 0) {
			//perror("select");
            //printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~timeout_current~Iiiiii~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
            return -4;
		}

    //printf("\n\t~~~~~~~~~~~~~~~read\n\t");

    unsigned char data[95] = {0};
    int data_length;
    int   nbytes = read(s, &frame, sizeof(frame));
    if(nbytes <= 0)
        return -1;

    // printf("read datas:%s ID=%#x data length=%d\n", ifr.ifr_name, frame.can_id, frame.can_dlc);
    // printf("read datas:xx ID=%#x data length=%d\n", frame.can_id, frame.can_dlc);
    // for (int i=0; i < frame.can_dlc; i++)
    //     printf("%#x ", frame.data[i]);
    // printf("\n");

    // printf("read can data over\n");
    if(frame.can_dlc <=0 )
    {
        printf("\n***  XXXXXXXXXXXXXX%s: frame.can_dlc <=0 wtf\n", param->port);
        return -2;
    }
    //put int serail buffer //TODO 超过了怎么说
    // if(receive_len + frame.can_dlc > 1000)
    // {
    //     memset(receive_data, 0, 1024);
    //     receive_len = 0;
    // }
    *buff_len = strlen(buff);
    memcpy(&buff[*buff_len], &frame.data[0], frame.can_dlc);
    *buff_len += frame.can_dlc;

    if(parse_data(buff, *buff_len, data, &data_length) == DATA_PROCESS_SUCCESS)
    {
        // printf("=====================================>%s, get data:%s, length[%d]\n",param->port, data, data_length);
        // for(int i=0; i< data_length; i++)
        // {
        //     printf("-------------------get data:%02x\n", data[i]);
        // }

        // for(int i=0; i<data_length; i++)
        // {
        // 	printf("\t\t %02x\t", data[i]);
        // }
        if(process_data(data, data_length, param, fsm))
        {
            if(*fsm == FSM_GET_MAC)
                reply_sn( param, s, fsm);
            else if(*fsm == FSM_GET_END)
                reply_end( param, s, fsm);
            else
            {
                printf("\n*** XXXXXXX%s: wfk  fsm:%d\n", param->port, *fsm);
            }

        }
        memset(buff,0,1024);
        *buff_len = 0;




    }


    return 0;


}


static int can_frame_process_write_data(int s, char* canport, AndriodProduct* product) // just use 8 bytes for write
{
        struct can_frame frame_send;

        int cursor = 0;
        /* configure can_id and can data length */
	    frame_send.can_id = 0x88;
		frame_send.can_dlc = 8;
		// printf("%s ID=%#x data length=%d\n", "ifr.ifr_name", frame_send.can_id, frame_send.can_dlc);
		// /* prepare data for sending: 0xAA,0x81,0x11,0x11,0x11,0x11,0x11,0x55 */
        // printf("sizeof can_frame.data:%d\n",sizeof(frame_send.data));

        //第一帧
        frame_send.data[0] = 0xAA;
        //frame_send.data[1] = product->cur_cmd; //TODO
        for(int i=2;i<8;i++)//6
        {
            frame_send.data[i] = product->cpu_sn[cursor++];
        }
		// for (int i=0; i<8; i++) 
		// {
        //     printf("%02x ", frame_send.data[i]);
		// }
		// printf("success to Sent out\n");
		/* Sending data */
		if(write(s, &frame_send, sizeof(frame_send)) < 0)
        {
        perror("Send failed");
        sleep(1);
        //close(s);
        //exit(-4);
        return 2;
        }
        usleep(100);

        //第2帧
        for(int i=0;i<8;i++) //8
        {
            frame_send.data[i] =  product->cpu_sn[cursor++];
        }
		// for (int i=0; i<8; i++)
		// {
		//     // frame_send.data[i] = ((i+1)<<4) | (i+1);
        //     //         frame_send.data[7] =number;
		//     // printf("%#x ", frame_send.data[i]);
        //     printf("%02x ", frame_send.data[i]);
		// }
		// printf("success to Sent out\n");
		/* Sending data */
		if(write(s, &frame_send, sizeof(frame_send)) < 0)
        {
        perror("Send failed");
        sleep(1);
        //close(s);
        //exit(-4);
        return 2;
        }

         usleep(100);

        //第3帧
        frame_send.can_dlc = 3;  //2
        frame_send.data[0] =  product->cpu_sn[cursor++];
        frame_send.data[1] =  product->cpu_sn[cursor++];
        frame_send.data[2] = 0x55;

     
		// for (int i=0; i<2; i++)
		// {
		//     // frame_send.data[i] = ((i+1)<<4) | (i+1);
        //     //         frame_send.data[7] =number;
		//     // printf("%#x ", frame_send.data[i]);
        //     printf("%02x ", frame_send.data[i]);
		// }
		// printf("success to Sent out\n");
		/* Sending data */
		if(write(s, &frame_send, sizeof(frame_send)) < 0)
        {
        perror("Send failed");
        sleep(1);
        //close(s);
        //exit(-4);
        return 2;
        }
        // printf("~~~~~~~~~~~~send over\n");
        return 0;



}



void set_can_down(char* canport)
{
    // ip link set can0 down
    char* commit_buf[120] = {0};
    sprintf(commit_buf, " ip link set %s down", canport);

    system(commit_buf);    

}
void set_can_bitrate(char* canport)
{
    //ip link set can0 type can bitrate 500000
    char* commit_buf[120] = {0};
    sprintf(commit_buf, "ip link set %s type can bitrate 500000", canport);

    system(commit_buf);   
}
void set_can_up(canport)
{
    // ip link set can0 up
    char* commit_buf[120] = {0};
    sprintf(commit_buf, " ip link set %s up", canport);

    system(commit_buf);   
    system(commit_buf);
    system(commit_buf);
    system(commit_buf);
    system(commit_buf);   
    system(commit_buf);
    system(commit_buf);
    system(commit_buf);
    char commit_echo[] =  "echo  4096 > /sys/class/net/can0/tx_queue_len";

     system(commit_echo);
    
}


void judge_can_status(char* canport)
{

}
void start_can(char* canport)
{
    //ip link set can0 down
    set_can_down(canport);
    set_can_bitrate(canport);
    set_can_up(canport);
    judge_can_status(canport);
}




void can_process_t(void* params)
{
    printf_func_mark(__func__);
    parameters *data = (parameters*) params;

    int s; // can raw socket
    int i;
    unsigned char number = 0;
    int nbytes;
    
    pid_t pid = -1;

    struct sockaddr_can addr;
    struct ifreq ifr;   
    struct can_filter rfilter[1];
    struct can_frame frame;
    struct can_frame frame_send;

    start_can(data->port);

    //create socket
    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
	{
	   perror("Create socket failed");
	   return ;
	}

    /* set up can interface */
	strcpy(ifr.ifr_name, data->port);
	//printf("can port is %s\n",ifr.ifr_name);

    /* assign can device */
	ioctl(s, SIOCGIFINDEX, &ifr);//指定can设备
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

    /* try to switch the socket into CAN FD mode */
    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &canfd_on, sizeof(canfd_on));

	/* bind can device */
	if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)//将套接字与can0绑定
	{
	     perror("Bind can device failed\n");
	     close(s);
	     return ;
	}

    fsm_state_t* fsm;
    if( !strcmp(data->port, CAN0Port))
	{
		fsm = &data->product->CAN0;
	}
	else if(!strcmp(data->port, CAN1Port))
	{
		fsm = &data->product->CAN1;
	}
	else
	{
		printf("\n***  XXXXXXXXXXXXXX%s: is not can0 or can1  wtf\n",data->port);
	}


	printf("\n*** \t %s wait receive cpu ID\n", data->port);
	struct timespec  last_read;
	clock_gettime(CLOCK_MONOTONIC, &last_read);
	struct timespec current_time;
    char can_buff[1024] = {0};
	int  buff_len =0;


    while(1)
    {   

        switch (*fsm)
        {
        case FSM_IDLE:
            if(can_process_read_data(data, can_buff, &buff_len, data->port,  s, fsm)== 0)
                clock_gettime(CLOCK_MONOTONIC, &last_read);
            break;
        case FSM_GET_MAC:
            if(can_process_read_data(data, can_buff, &buff_len, data->port, s, fsm)== 0)
                clock_gettime(CLOCK_MONOTONIC, &last_read);
            break;
        case FSM_GET_END:
            if(can_process_read_data(data, can_buff, &buff_len, data->port, s, fsm)== 0)
                clock_gettime(CLOCK_MONOTONIC, &last_read);
            break;
        
        default:
            break;
        }

        
		clock_gettime(CLOCK_MONOTONIC, &current_time);
        if(!HASCPUSN)
		{
			clock_gettime(CLOCK_MONOTONIC, &last_read);
			continue;
		}
		int  consum_tm = diff_ms(&current_time,&last_read);

		if(consum_tm >= 20000 )//20s//还有一直受到垃圾消息  不能通过last_read
		{
			printf("\t\t Error %s time consuming >60s but can't receive corrent data\n", data->port);
			
			*fsm = FSM_TEST_FAIL;
			memset(can_buff, 0, 1024);
			buff_len = 0;
            usleep(1000000);
            clock_gettime(CLOCK_MONOTONIC, &last_read);
		}
            
	 }

	close(s);
	return;

}