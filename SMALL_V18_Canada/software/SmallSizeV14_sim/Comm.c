#include "Comm.h"

//packet_type_t type;                         //���ݰ�����
//unsigned char packet_cnt=0;                 //�հ���������  
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
 *  �������ƣ� init_comm
 *  ���������� ͨ��ģ���ʼ����������ʼ�������Ƚ�ͨ��ģ���ʼ��Ϊ����ģʽ��������
 *             ʼ��2401_dev�ĸ�����Ա
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 *  ��ע:      ����2401��ʼ�������ָ���λ��ζ��壬����<<PTR4000>>������������ط���Ľ���
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
	
	nRF2401_dev.RFconf = g_default_nRF2401_conf;//������init_nRF2401_dev_rx��������д��һ��RFconf��ֵ�����ظ�
	SetPowerUp(1);
	SetCE( 0 );
	SetCS( 1 );
	delay(1000);
	init_nRF2401_dev_rx();

	set_receive_flag();
}


/*--------------------------------------------------------------------------------------------
 *  �������ƣ� do_comm
 *  ���������� ͨѶ���ܺ����������ж��Ƿ��յ�ͨѶ��������յ�����ͨѶ��������g_comm.buffer�У�
 *             ���н�������ϴ�һ������
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 ---------------------------------------------------------------------------------------------*/ 

void do_comm()
{
	if(receive_flag==1){   
		if((nRF2401_dev.get_packet(&nRF2401_dev))>0)//���� nRF2401_dev.get_packet,����յ����ú���������һ������0�����ݳ���
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
 *  �������ƣ� do_comm_up
 *  ���������� �ϴ�ִ�к���������ȫ�ֱ���type����ֵ���������ú���ģʽ�����ϴ�
 *  
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 -------------------------------------------------------------------------------*/ 

int do_comm_up()
{
    short Game_Status, Robot_Position={-1};
	short Robot_Number[4]={-1,-1,-1,-1};

	nRF2401_dev.buf.pos = PACKET_LEN_UP;

    static char data[7]={0,0,0,0,0,0,0};           //2011��������ǰ������ÿ�η���ռ�������data���䣬����ֱ�Ӹ�һ������ռ䣬ÿ�θ���

    Game_Status = (g_comm.buffer[0] & 0x30) >> 4;  //ȷ������״̬
    
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

     /* ---------�μ�Э��3.0������ͣʱ��֡���ذ���ʽ----------- */
    
//    if ((Game_Status == 1) || (Game_Status == 2))  // ������ͣ��Halt
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
//			memset(&nRF2401_dev.buf.buf[6], 0, sizeof(unsigned char) * (PACKET_LEN_UP - 6));//����Ķ���ֵ0
//			nRF2401_dev.send_packet( &nRF2401_dev );
//		}
//	}
    
    /*--------------�μ� ZJUNlict 2018 Communication Protocol V1.2 ����ʱ��֡���ذ���ʽ----------------------*/
    if (Game_Status == 0)  // ����ʱ�ذ�
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
	/*----------------------------------Ƶ�����ģʽ�ϴ� ɾ��-----------------------------------*/
	/*--------------------���ǵ�������ʱ�����⣬��������ϴ��������ϴ���-------------------*/

    return 0;
}


/*-----------------------------------------------------------------------------------------
 *  �������ƣ� do_packet_process
 *  ���������� �Խ��յ�������data���н�������ݲ�ͬ��ģʽ�в�ͬ�Ľ����ʽ����Ӧ�в�ͬ�Ľ������
 *             ����Ľ��������λ��Packet.c��
 *
 *
 *  ���ߣ�     ����
 *  �޸����ڣ� 2011-9-15
 -------------------------------------------------------------------------------------------*/ 
void do_packet_process( unsigned char *data, int len )
{   


//	type = get_packet_type( data, len );
    /*----------------------------ͨ��ģʽ�µ����ݽ��--------------------------------*/
	/*-----------------------�μ�Э��0.1����ʱͨѶ����ʽ-----------------------------*/
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
 *  �������ƣ� is_comm_overflow
 *  ���������� ͨѶ��ʱ��ʲô������
 *
 *
 *  ���ߣ�     lyc
 *  �޸����ڣ� 2016-3-19
 -------------------------------------------------------------------------------------------*/ 

void is_comm_overflow()
{
    extern char g_comm_overflow_flag;
    extern char comm_down;
	if(g_comm_overflow_flag)					//ͨѶ��ʱ��ʲô��������
	{
		g_comm_up_finish_flag = 1; 		//��ֹͨѶ�жϣ���λ����ͨѶΪ����ģ���־λ
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
 *  �������ƣ� comm_overflow
 *  ���������� ��ͨѶ����Ϊ����ģʽ������λ�ɽ��ܱ�־λ
 *
 *
 *  ���ߣ�     lyc
 *  �޸����ڣ� 2016-3-19
 -------------------------------------------------------------------------------------------*/ 
void set_receive_mode()
{
	if(g_set_receive_mode_flag == 1)
	{
		set_a_bit_receive();
		g_set_receive_mode_flag = 0;
	}
}
