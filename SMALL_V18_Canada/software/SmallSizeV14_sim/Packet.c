#include "Packet.h"

char packet_flag;
extern robot_t g_robot;
//extern unsigned char g_power;
//extern int ADvalue;
extern unsigned int g_comm_overflow;			//�յ��Լ������ݣ�ͨѶ������㡣
extern char g_comm_overflow_flag;
extern char shooter;
extern char comm_down;

/*-----------------------------------------------------------------------------
 *  �������ƣ� stop_mode_packet
 *  ���������� ���յ�stop��־λ��������һ��Э��Ļذ�
 *-------------�μ�Э��0.3������ͣʱ��֡���ذ���ʽ-----------*
 *  ���ߣ�     ����
 *  �޸���:	�����
 *  �޸����ڣ� 2012-12-3
 -------------------------------------------------------------------------------*/ 
//int stop_mode_packet(char *q)
//{
//	q[0]=0xff;
//	q[1]=0x01;
//	q[2]=g_robot.num&0x0f;
//	g_power=(IORD(ADGETNEW2_0_BASE,0)&0x00ff);
//	if(g_power==0xFF)
//	{
//		g_power=0xFE;
//	}
//	q[3]=g_power;
//	ADvalue=(IORD(ADGETNEW2_0_BASE,1)&0x00ff);
//	if(ADvalue==0xFF)
//	{
//		ADvalue=0xFE;
//	}
//	q[4]=ADvalue;
//	packet_flag=1;
//	return 0;
//}


/*-----------------------------------------------------------------------------
 *  �������ƣ� packet
 *  ���������� ��������������践����λ������Ϣ�����
 		������Ƿ���״̬�仯(ƽ�䡢���䡢������һ�ֱ仯)��
 		������״̬�仯ʱ����λ�ϴ���־ packet_flag
 *-------------�μ�Э��0.2����ʱ��֡���ذ���ʽ-----------*
 *  ���ߣ�     ����
 *  �޸���:	  �
 *  �޸����ڣ� 2014-5-22 2018 Before Canada Modified
 -------------------------------------------------------------------------------*/ 
inline void packet(char *q)
{
    static char upload_packet_count_down = 0;
    static char last_upload_status = 0;
    static char is_kicked   = 0;
    static char is_chipped  = 0;
    char current_status     = 0;
    unsigned char battery_voltage = 0;
    unsigned char cap_voltage = 0;
    
/* -------------- Comm Report Test Code -------------*/
    battery_voltage=(IORD(ADGETNEW2_0_BASE,0)&0x00ff);  // Get Battery Voltage 8bits
    if(battery_voltage==0xFF)
    {
        battery_voltage=0xFE;
    }
    q[4] = battery_voltage;
    
    cap_voltage=(IORD(ADGETNEW2_0_BASE,1)&0x00ff);  // Get Cap Voltage 8bits
    if(cap_voltage==0xFF)
    {
        cap_voltage=0xFE;
    }
    q[5] = cap_voltage;
    
    q[6]  = (((IORD(MOTOR_0_BASE, 2))>>8) & 0xffff);
    q[7]  = (((IORD(MOTOR_0_BASE, 2))) & 0xffff);
    q[8]  = (((IORD(MOTOR_1_BASE, 2))>>8) & 0xffff);
    q[9]  = (((IORD(MOTOR_1_BASE, 2))) & 0xffff);
    q[10] = (((IORD(MOTOR_2_BASE, 2))>>8) & 0xffff);
    q[11] = (((IORD(MOTOR_2_BASE, 2))) & 0xffff);
    q[12] = (((IORD(MOTOR_3_BASE, 2))>>8) & 0xffff);
    q[13] = (((IORD(MOTOR_3_BASE, 2))) & 0xffff);
/* -------------- Comm Report Test Code -------------*/

    packet_flag = 0;
    q[2]=g_robot.num;
    
    current_status = (g_robot.infrared << 6);
	if(shooter == 2)
	{
		shooter = 0;
        is_kicked = 1;
	}
    current_status = current_status | (is_kicked << 5);
    if(shooter == 1)
	{
    	shooter = 0;
        is_chipped = 1;
	}
    current_status = current_status | (is_chipped << 4);
    
    if (current_status == 0){
        if (upload_packet_count_down == 0){
            last_upload_status = 0;
            return;
        }
    }
    
    if (current_status == last_upload_status){
        if (g_robot.infrared == 0){
            upload_packet_count_down --;
            if (upload_packet_count_down == 0){
                is_kicked = 0;
                is_chipped = 0;
            }
            q[3] = current_status;
            packet_flag = 1;
            return;
        }
        else{
            upload_packet_count_down = 4;
            q[3] = current_status;
            packet_flag = 1;
            return;
        }
    }
    
    upload_packet_count_down = 4;
    q[3] = current_status;
    last_upload_status = current_status;
    packet_flag = 1;


    return;
}



/*-----------------------------------------------------------------------------
 *  �������ƣ� get_packet_type
 *  ������������ȡ����ģʽ������֮����
 *  ���ߣ�     ����
 *  �޸���:	�����
 *  �޸����ڣ� 2012-12-3
 -------------------------------------------------------------------------------*/ 
//packet_type_t get_packet_type( unsigned char* data, int len )
//{
//	/* check run-time mode */
//	volatile unsigned char temp;
///*	temp = data[1];
//	temp = temp & 0xF0;
//	temp = temp >> 4;
//	
//	switch(temp){
//		case DEBUG_FUNCTION_Normal:                  	//����ģʽ
//			return PACKET_Normal;
//
//		case DEBUG_FUNCTION_Set_9557_error:
//			return PACKET_DEBUG_Set_9557_error; 
//
//		default:
//			return PACKET_NONE;                    	//�������ݰ�
//	}  2011 Legacy Protocol */
//    temp = data[0];
//    temp = temp & 0xC0;
//    temp = temp >> 6;
//    switch(temp){
//        case 1:
//        return PACKET_Normal;
//        
//        default:
//            return PACKET_NONE;
//    }
//}

/*-----------------------------------------------------------------------------
 *  �������ƣ� decode_packet_robot
 *  ���������� ����Ӧ���ŵ�С�������˶�����
 *  ���ߣ�     ����
 *  �޸���:	�����
 *  �޸����ڣ� 2012-11-24
 -------------------------------------------------------------------------------*/ 
int decode_packet_robot( packet_robot_t *packet, unsigned char *data, int len )
{
	unsigned short temp = 0;
    short velocity_data_pos = 0;    
    short shoot_power_pos = 0;
	unsigned char  i=0;
    short Robot_Position={-1};
    short Robot_Number[4]={-1,-1,-1,-1};
    
	if( packet == NULL || data == NULL)
		return -1; 

    if (((data[0] & 0x8) >> 3) == 1){
        Robot_Number[0] = (data[1] & 0x0f) ;
        if (Robot_Number[0] == (g_robot.num -1)){
            Robot_Position = 0;
        }
    }
    
    if (((data[0] & 0x4) >> 2) == 1){
        Robot_Number[1] = (data[5] & 0x0f) ;
        if (Robot_Number[1] == (g_robot.num -1)){
            Robot_Position = 1;
        } 
    }
    
    if (((data[0] & 0x2) >> 1) == 1){
        Robot_Number[2] = (data[9] & 0x0f)  ;
        if (Robot_Number[2] == (g_robot.num -1)){
            Robot_Position = 2;
        } 
    }    
    
    if (((data[0] & 0x1)     ) == 1){
        Robot_Number[3] = (data[13] & 0x0f) ;
        if (Robot_Number[3] == (g_robot.num -1)){
            Robot_Position = 3;
        } 
    }
    
    if (Robot_Position == -1){
        return -1;
    }
    
    i = 1 + 4 * Robot_Position;
    velocity_data_pos = 17 + Robot_Position;
    shoot_power_pos = 21 + Robot_Position;
    
/*
    if(g_robot.num > 8)
    {
	    if(((data[1]&0x0f)&(0x01<<(g_robot.num-9)))==0)
	    {
		    return  -1;  	
	    }
    }
    else
    {
 	    if(((data[2]&0xff)&(0x01<<(g_robot.num-1)))==0)  //�ж��������Ƿ����Լ�������
	    { 
	      	return  -1;  	
	    }
    }
    Original Package Authentication
*/

    g_comm_overflow = 0;			//�յ��Լ������ݣ�ͨѶ������㡣
    g_comm_overflow_flag = 0;
    comm_down = 0;
    temp=0;
 
 /*
    if(g_robot.num < 9)
    {
	    for(i=0;i<(g_robot.num-1);i++)
	  	{
	  	    if(data[2]&(0x01<<i))
		    temp++;
		 }
    }
    else
    {
 	    for(i=0;i<8;i++)
	  	{
	  	 	if(data[2]&(0x01<<i))
		 	temp++;	
		}
        for(i=0;i<(g_robot.num-9);i++)
    	{
        	if((data[1]&0x0f)&(0x01<<i))
        	temp++;
     	}
  	}
  	i=temp*4+3;                           //������ʼ��
*/
 
  	packet->robot_num = g_robot.num;
    
    temp= data[i];
    packet->dribbler = ((( temp >> 4 ) & 0x03));
    packet->dribbler = ( ( temp & 0x80) ? (-packet->dribbler) : packet->dribbler);
    temp= data[shoot_power_pos] & 0x7F;

    if(( data[i] >> 6 ) & 0x1 )  //tiao she
    {
        packet->chip = temp;
        packet->shoot = 0;
    }
    else                          //ping she
    {
        packet->shoot = temp;
        packet->chip = 0;
    }

    temp = data[ i+1 ];
    packet->speed_x = temp & 0x7F;
    temp = (data[velocity_data_pos] & 0xc0) << 1;
    packet->speed_x = packet->speed_x + temp;
    temp = data[ i+1 ];
    packet->speed_x = ( ( temp & 0x80 ) ? ( -packet->speed_x ) : packet->speed_x );
       
    temp = data[ i+2 ];
    packet->speed_y = temp & 0x7F;
    temp = (data[velocity_data_pos] & 0x30) << 3;
    packet->speed_y = packet->speed_y + temp;
    temp = data[ i+2 ];
    packet->speed_y = ( ( temp & 0x80 ) ? ( -packet->speed_y ) : packet->speed_y );
       
    temp = data[ i+3 ];
    packet->speed_rot = temp & 0x7F;
    temp = (data[velocity_data_pos] & 0x0f) << 7;
    packet->speed_rot = packet->speed_rot+temp;
    temp = data[ i+3 ];
    packet->speed_rot = ( ( temp & 0x80 ) ? ( -packet->speed_rot ) : packet->speed_rot );
	
	return 0;
}

