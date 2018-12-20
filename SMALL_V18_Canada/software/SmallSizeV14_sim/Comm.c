#include "Comm.h"

//packet_type_t type;                         //数据包类型
//unsigned char packet_cnt=0;                 //收包数计数器  
//unsigned char check_frq_start=0;
//unsigned int check_frq_counter1=0;
//unsigned int check_frq_counter2=0;

extern robot_t g_robot;
extern char packet_flag;
extern nRF2401 nRF2401_dev;
extern char receive_flag;
extern char g_set_receive_mode_flag;
extern char g_comm_up_finish_flag;
extern char g_comm_overflow_flag;

RFConfig g_default_nRF2401_conf = {
	15,
	ADDR_COUNT,
	DATA_LEN2*8,            			//Length of data payload section TX channel 2
	DATA_LEN1*8,                     	//Length of data payload section TX channel 1
	ADDR_CH2 ,                    		//Up to 5 byte address for TX channel 2
	ADDR_CH1 ,                       	//Up to 5 byte address for TX channel 1
	(((ADDR_LEN) << 2) + ((CRC_LEN) << 1) + (CRC_ENABLE)),   
	//Number of address bits (both TX channels),8 or 16 bit CRC,Enable on-chip CRC generation/checking
	//0x4f,                               //Enable two channel receive mode,Communication mode (Direct or ShockBurst?),RF data rate (1Mbps requires 16MHz crystal),Crystal frequency,RF output power
	(((RX2_EN) << 7) + ((CM) << 6) + ((RF_DATA_RATE) << 5) + ((XO_F) << 2) + (RF_PWR)) ,
	(((RF_CH) << 1) + 1)      			//Frequency channel,RX or TX operation

};


/*---------function headers----------*/
int do_comm_up();
void do_packet_process( unsigned char *data, int len );


/*----------------------------------------------------------------------------------------
 *  函数名称： init_comm
 *  函数描述： 通信模块初始化函数，初始化中首先将通信模块初始化为接受模式，并将初
 *             始化2401_dev的各个成员
 *
 *
 *  作者：     不详
 *  修改日期： 2011-9-15
 *  备注:      关于2401初始化配置字各个位如何定义，参阅<<PTR4000>>关于配置字相关方面的介绍
 -----------------------------------------------------------------------------------------*/ 

void init_comm()
{
	//uchar *transfer_buf;

	//transfer_buf = (uchar *)malloc( sizeof(uchar) * nRF2401_BUFFER_LEN);

	//nRF2401_dev.buf.buf = transfer_buf;
	nRF2401_dev.buf.len = nRF2401_BUFFER_LEN;
	nRF2401_dev.buf.pos = 0;
	nRF2401_dev.get_packet = get_nRF2401_packet;
	nRF2401_dev.send_packet = send_nRF2401_packet;
	nRF2401_dev.init_dev = init_nRF2401_dev_rx;
	
	nRF2401_dev.RFconf = g_default_nRF2401_conf;//后面在init_nRF2401_dev_rx中又重新写了一遍RFconf的值，有重复
	SetPowerUp(1);
	SetCE( 0 );
	SetCS( 1 );
	delay(1000);
	init_nRF2401_dev_rx();

	set_receive_flag();
}


/*--------------------------------------------------------------------------------------------
 *  函数名称： do_comm
 *  函数描述： 通讯接受函数，首先判定是否收到通讯包，如果收到，将通讯包拷贝至g_comm.buffer中，
 *             进行解包，并上传一次数据
 *
 *
 *  作者：     不详
 *  修改日期： 2011-9-15
 ---------------------------------------------------------------------------------------------*/ 

void do_comm()
{
	if(receive_flag==1){   
		if((nRF2401_dev.get_packet(&nRF2401_dev))>0)//函数 nRF2401_dev.get_packet,如果收到包该函数将返回一个大于0的数据长度
		{
			receive_flag=0;
			memcpy(g_comm.buffer, nRF2401_dev.buf.buf, nRF2401_dev.buf.pos);
			g_comm.buffer_pos = (unsigned char)nRF2401_dev.buf.pos;
			do_comm_up();
			do_packet_process(g_comm.buffer, g_comm.buffer_pos); 
//			if (packet_cnt>254)
//				packet_cnt = 0;
//			else
//          packet_cnt++;
		}
	}
}


/*-----------------------------------------------------------------------------
 *  函数名称： do_comm_up
 *  函数描述： 上传执行函数，根据全局变量type的数值来决定运用何种模式进行上传
 *  
 *
 *
 *  作者：     不详
 *  修改日期： 2011-9-15
 -------------------------------------------------------------------------------*/ 

int do_comm_up()
{
    short Game_Status, Robot_Position={-1};
	short Robot_Number[4]={-1,-1,-1,-1};

	nRF2401_dev.buf.pos = PACKET_LEN_UP;

    static char data[7]={0,0,0,0,0,0,0};           //2011国内赛以前是依靠每次分配空间来进行data分配，现在直接给一段数组空间，每次更新

    Game_Status = (g_comm.buffer[0] & 0x30) >> 4;  //确定比赛状态
    
    if (((g_comm.buffer[0] & 0x8) >> 3) == 1){
        Robot_Number[0] = (g_comm.buffer[1] & 0x0f);
        if (Robot_Number[0] == (g_robot.num -1)){
            Robot_Position = 0;
        }
    }
    
    if (((g_comm.buffer[0] & 0x4) >> 2) == 1){
        Robot_Number[1] = (g_comm.buffer[5] & 0x0f) ;
        if (Robot_Number[1] == (g_robot.num -1)){
            Robot_Position = 1;
        }
    }
    
    if (((g_comm.buffer[0] & 0x2) >> 1) == 1){
        Robot_Number[2] = (g_comm.buffer[9] & 0x0f) ;
        if (Robot_Number[2] == (g_robot.num -1)){
            Robot_Position = 2;
        }
    }    
    
    if (((g_comm.buffer[0] & 0x1)     ) == 1){
        Robot_Number[3] = (g_comm.buffer[13] & 0x0f) ;
        if (Robot_Number[3] == (g_robot.num -1)){
            Robot_Position = 3;
        }
    }
    
    if (Robot_Position == -1){
//        g_comm_up_finish_flag = 1;//
        set_receive_flag();
        return 0;
    }

     /* ---------参见协议3.0比赛暂停时分帧返回包格式----------- */
    
//    if ((Game_Status == 1) || (Game_Status == 2))  // 比赛暂停或Halt
//	{
//        stop_mode_packet(data);
//		if(packet_flag==1)
//		{
//			nRF2401_dev.buf.buf[0]=data[0];
//			nRF2401_dev.buf.buf[1]=data[1];
//			nRF2401_dev.buf.buf[2]=data[2]; 
//			nRF2401_dev.buf.buf[3]=data[3];
//			nRF2401_dev.buf.buf[4]=data[4];
//			nRF2401_dev.buf.buf[5]=data[5]; 
//			memset(&nRF2401_dev.buf.buf[6], 0, sizeof(unsigned char) * (PACKET_LEN_UP - 6));//后面的都赋值0
//			nRF2401_dev.send_packet( &nRF2401_dev );
//		}
//	}
    
    /*--------------参见 ZJUNlict 2018 Communication Protocol V1.2 比赛时分帧返回包格式----------------------*/
    if (Game_Status == 0)  // 比赛时回包
    {
        packet(data);
        if(packet_flag == 1)
        {
            nRF2401_dev.buf.buf[0]=0xFF;
            nRF2401_dev.buf.buf[1]=0x02;
            nRF2401_dev.buf.buf[2]=data[2]; 
            nRF2401_dev.buf.buf[3]=data[3];
            nRF2401_dev.buf.buf[4]=data[4];
            nRF2401_dev.buf.buf[5]=data[5];
            nRF2401_dev.buf.buf[6]=data[6];
            nRF2401_dev.buf.buf[7]=data[7];
            nRF2401_dev.buf.buf[8]=data[8];
            nRF2401_dev.buf.buf[9]=data[9];
            nRF2401_dev.buf.buf[10]=data[10];
            nRF2401_dev.buf.buf[11]=data[11];
            nRF2401_dev.buf.buf[12]=data[12];
            nRF2401_dev.buf.buf[13]=data[13];
            memset(&nRF2401_dev.buf.buf[14], 0, sizeof(unsigned char) * (PACKET_LEN_UP - 14));            
            nRF2401_dev.send_packet( &nRF2401_dev );
            g_comm_up_finish_flag = 1;
        }
        else{
            set_receive_flag();
        }
    }
    else{
            set_receive_flag();
    }
	/*----------------------------------频点测试模式上传 删除-----------------------------------*/
	/*--------------------考虑到兼容与时序问题，将解包与上传均做在上传中-------------------*/

    return 0;
}


/*-----------------------------------------------------------------------------------------
 *  函数名称： do_packet_process
 *  函数描述： 对接收到的数据data进行解包，根据不同的模式有不同的解包方式，对应有不同的解包函数
 *             具体的解包函数均位于Packet.c中
 *
 *
 *  作者：     不详
 *  修改日期： 2011-9-15
 -------------------------------------------------------------------------------------------*/ 
void do_packet_process( unsigned char *data, int len )
{   


//	type = get_packet_type( data, len );
    /*----------------------------通常模式下的数据解包--------------------------------*/
	/*-----------------------参见协议0.1比赛时通讯包格式-----------------------------*/
	if((data[0] & 0xC0) == (1<<6))
	{
		/* parse robot command */
		packet_robot_t packet;
		memset( &packet, 0, sizeof( packet ) );
		if( decode_packet_robot( &packet, data, len ) < 0 )
		{
			/* parse error */
//			g_comm.packet_error++;
			return;
		}
		
		/* execute the command */
		on_robot_command( &packet );
	}
}


/*-----------------------------------------------------------------------------------------
 *  函数名称： is_comm_overflow
 *  函数描述： 通讯超时，什么都不做
 *
 *
 *  作者：     lyc
 *  修改日期： 2016-3-19
 -------------------------------------------------------------------------------------------*/ 

void is_comm_overflow()
{
    extern char g_comm_overflow_flag;
    extern char comm_down;
	if(g_comm_overflow_flag)					//通讯超时，什么都不做。
	{
		g_comm_up_finish_flag = 1; 		//防止通讯中断，置位设置通讯为接收模块标志位
//		do_dribbler(0);
//		do_acc_handle_move(0,0,0,8);
//      usleep(16000);
//		do_shoot(0);
//		do_chip(0);
//		#ifdef ENABLE_2401_RESTART
//			rst_nRF2401();
//		#else
		set_a_bit_receive();
//		#endif
		g_comm_overflow_flag = 0;
        comm_down = 1;
	}
    if(comm_down){
        do_dribbler(0);
        do_acc_handle_move(0,0,0,8);
        usleep(16000);
    }
}


/*-----------------------------------------------------------------------------------------
 *  函数名称： comm_overflow
 *  函数描述： 将通讯设置为接收模式，并置位可接受标志位
 *
 *
 *  作者：     lyc
 *  修改日期： 2016-3-19
 -------------------------------------------------------------------------------------------*/ 
void set_receive_mode()
{
	if(g_set_receive_mode_flag == 1)
	{
		set_a_bit_receive();
		g_set_receive_mode_flag = 0;
	}
}

