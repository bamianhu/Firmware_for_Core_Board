#include "Action.h"

char shooter;
char g_is_shoot_set = 0;
int g_is_shoot_in_main_loop = 0;
int g_is_chip_in_main_loop = 0;
shooter_t g_shooter[ 2 ];

extern robot_t g_robot;
extern int wheel_reduction_ratio_x_set;
extern int wheel_reduction_ratio_yz_set;


/*---------function headers----------*/
void shooter_off();
long V2N( double vel );
int set_shooter( int channel, int value );



inline void update_shooter()
{
	if(g_shooter[0].count_down > 0)
		g_shooter[0].count_down--;
	else
		g_shooter[0].count_down = 0;
        
    if(g_shooter[1].count_down > 0)
        g_shooter[1].count_down--;
    else
        g_shooter[1].count_down = 0;
}

void shooter_off()
{
	// shoot & chip disable 
	IOWR(SHOOT_TIMER_SHOOT_BASE,0,0x0);
	IOWR(SHOOT_TIMER_CHIP_BASE,0,0x0);
	g_shooter[ 0 ].count_down = 0;
    g_shooter[ 1 ].count_down = 0;
}


void init_shooter()
{
	shooter_off();
}


/*-----------------------------------------------------------------
 *  函数名称： do_dribbler
 *  函数描述： 给带球电机力度参数（g_robot.dribbler.set）赋值，
 *			   输入为带球力度档数，档数分为0-1-2-3，
 *					 
 *  作者：     方立
 *  修改日期： 2012—11-29
 -----------------------------------------------------------------*/ 
void do_dribbler(int dribbler)		//0 4 8 12 100MHz Version 0 40 80 160 Gear ratio modification 0 8 16 24 50MHz Version 
{
switch(dribbler){
    case 0: IOWR_AVALONCONTROL_SET(MOTOR_4_BASE, 0); break;
    case 1: IOWR_AVALONCONTROL_SET(MOTOR_4_BASE, -8); break;
    case 2: IOWR_AVALONCONTROL_SET(MOTOR_4_BASE, -16); break;
    case 3: IOWR_AVALONCONTROL_SET(MOTOR_4_BASE, -24); break;
    default: IOWR_AVALONCONTROL_SET(MOTOR_4_BASE, 0); break;
}
}

/*-----------------------------------------------------------------
 *  函数名称： do_move
 *  函数描述： 设定小车的x,y,z的速度，形参为x,y,z速度，
 *			   		 速度单位为国际单位
 *					
 *
 *  作    者： 方立
 *  修改日期： 2012—11-29
 -----------------------------------------------------------------*/
void do_move( int speed_x, int speed_y, int speed_rot )
{
	int i = 0;
    int motor_set_temp[4];
	/* 线速度 vx, vy, vz are all measured in m/s */

	double vx = (double)(speed_x)  / 100;
	double vy = (double)(speed_y)  / 100;
	double vz = (double)(speed_rot) * 0.025 * WHEEL_CENTER_OFFSET;

	/* 各轮子线速度(m/s)设定值 */
	double speed[ CHANNEL_NUM ] = { 0 };

	for( i = 0; i < 4; i++ )
	{
		/* trasnform wheel angle */
		speed[ i ] = ( g_robot.sin_angle[ i ] * vx + g_robot.cos_angle[ i ] * vy + vz );
        motor_set_temp[i] = speed[ i ] * g_robot.kv2n;
	}
    IOWR_AVALONCONTROL_SET(MOTOR_0_BASE,motor_set_temp[0]);
	IOWR_AVALONCONTROL_SET(MOTOR_1_BASE,motor_set_temp[1]);
	IOWR_AVALONCONTROL_SET(MOTOR_2_BASE,motor_set_temp[2]);
	IOWR_AVALONCONTROL_SET(MOTOR_3_BASE,motor_set_temp[3]);
}



/*限定车子的加速度*/
/*-----------------------------------------------------------------
 *  函数名称： do_acc_handle_move
 *  函数描述：限定小车xy方向上合加速度值小于acc,
 *			  并且设定小车x,y,z方向上移动速度分别为
 *			  speed_x, speed_y, speed_rot，单位为国际单位 cm/s
 *
 *  作者：     方立
 *  修改日期： 2012—11-29
 -----------------------------------------------------------------*/

void do_acc_handle_move(int speed_x, int speed_y, int speed_rot, int acc)
{
	static double last_speed_x = 0;
	static double last_speed_y = 0;
//	int i = 0;
	double acc_x = 0;
	double acc_y = 0;
	double acc_whole = 0;
	double sin_x = 0;
	double sin_y = 0;
	acc_x = speed_x - last_speed_x;
	acc_y = speed_y - last_speed_y; 
	acc_whole = acc_x * acc_x + acc_y * acc_y ;
	acc_whole = sqrt(acc_whole) + 0.001;
	sin_x = acc_x / acc_whole;
	sin_y = acc_y / acc_whole;

	if (acc_whole > acc)
	{
		acc_whole = acc;
		acc_x = acc_whole * sin_x;
		acc_y = acc_whole * sin_y;
		speed_x = ceil(last_speed_x + acc_x);
		speed_y = ceil(last_speed_y + acc_y); 
	}

	do_move(speed_x,speed_y,speed_rot);
	last_speed_x = speed_x;
	last_speed_y = speed_y;
}


/*-----------------------------------------------------------------
 *  函数名称： set_shooter
 *  函数描述： 执行射门指令。形参channel为0时平射,
 * 			   为1时挑射；形参value为射门力度范围
 *			   一般为......,执行射门前检查电容电压和
 *			   什么力度大小，只有电容电压足够且
 *             射门力度不为0时才能射门。
 *					 
 *  作者：     方立
 *  修改日期： 2012—11-29
 -----------------------------------------------------------------*/ 

int set_shooter( int channel, int value )
{
    if(value <= 0){
        return -1;
    }
    if(g_shooter[ channel ].count_down && (g_robot.mode!=OFFLINE_TEST_MODE)) 
	{
		return -1;
	}
	if(IORD(PIO_SHOOT_OFF_BASE, 0))
	{
		g_robot.is_cap_low = 0;
	}
	else
	{
		g_robot.is_cap_low = 1;
	}

	if(!g_robot.is_cap_low)		//value=0的时候 不射 ，不需要给count_down赋值，否则影响正常的射门
	{
//		g_shooter[ channel ].strength = value;
		g_shooter[ channel ].count_down = SHOOTER_INTERVAL;

		switch ( channel )
		{
		    // channel 0 is shoot , high is active , 
			case SHOOT : 
				{
		    	IOWR(SHOOT_TIMER_SHOOT_BASE,0,value * 5000);  //测试新版射门机构 Original *2500	
			 	shooter=2;
		    	}break;
		    // channel 1 is chip ,  high is active , 
		    case CHIP :
				{
		      	//IOWR(SHOOT_TIMER_CHIP_BASE,0,value * 2500);	//0~127*2500  ->   60*2500 ~ n*2500 
                                                            //value * (100*2500 - 2500*55)/127 + 2500*55
                                                            //(value*1.0 / 127.0 * 45.0) + 55
                IOWR(SHOOT_TIMER_CHIP_BASE,0,value * 5000);  //测试新版射门机构 Original *2500
			 	shooter=1;
	      	    }break;
		    // default
		    default:
		    	IOWR(SHOOT_TIMER_SHOOT_BASE,0,0x0);
		    	IOWR(SHOOT_TIMER_CHIP_BASE,0,0x0);
		    	return -1;
		    	break;
		  }
	  }

	return 0;
	
}



/*-----------------------------------------------------------------
 *  函数名称： do_shoot
 *  函数描述：形参shoot为1时，若嘴里有球才能平射，
 *			  若嘴里没球，则不射门重新开启红外中断。
 *			  若形参shoot和chip都为零，则关闭红外中断
 *
 *  作者：    方立
 *  修改日期：2012—11-29
 -----------------------------------------------------------------*/ 
void do_shoot( int shoot_power )
{
	/* 先保存平射指令 */
//	g_robot.shoot = shoot;
        g_robot.infrared = is_ball_detected();
		if ( g_robot.infrared == 1 )
		{
		   set_shooter(0, shoot_power);
		}
//		else
//		{
//			IOWR(PIO_INFRA_BASE, 3, 0x00);        //清中断标志
			//IOWR(PIO_INFRA_BASE, 2, 0x1);			//开启红外中断。
//		}
//	else if((chip == 0) && (shoot == 0))      //没有射门直也没有挑射值得时候关闭中断 
//	{
//		IOWR(PIO_INFRA_BASE, 2, 0x0);			//关闭红外中断。
//	}
}


/*-----------------------------------------------------------------
 *  函数名称： do_chip
 *  函数描述：形参chip为1时，若嘴里有球才能挑射，
 *			  若嘴里没球，则不射门重新开启红外中断。
 *			  若形参shoot和chip都为零，则关闭红外中断
 *
 *  作者：     方立
 *  修改日期： 2012—11-29
 -----------------------------------------------------------------*/

void do_chip(int chip_power)
{
    g_robot.infrared = is_ball_detected();
	if ( g_robot.infrared == 1 )
	{
		set_shooter( 1 , chip_power );
	}
//		else
//		{
//			IOWR(PIO_INFRA_BASE, 3, 0x00);        //清中断标志
			//IOWR(PIO_INFRA_BASE, 2, 0x1);			//开启红外中断。
//		}
//	}
//  	else if((chip == 0) && (shoot == 0))
//	{
//		IOWR(PIO_INFRA_BASE, 2, 0x0);			//关闭红外中断。
//	}
}



/* robot command handler */
/*-----------------------------------------------------------------
 *  函数名称： on_robot_command
 *  函数描述： 形参指针指向存储运动参数的packet_robot_t
 *			   结构体packet packet，执行各项运动指令
 *					
 *
 *  作者：     方立
 *  修改日期： 2012-11-29
 -----------------------------------------------------------------*/

void on_robot_command( packet_robot_t *packet )
{
	static int comm_count;

	/*count the right packet */
	comm_count++;
	if(comm_count==16)
	{
		led_on(1);
	}
    if(comm_count==32)
    {
        led_off(1);
        comm_count=0;
    }
    
	if( g_robot.mode == NORMAL_MODE )
	{
	 		
		do_dribbler( packet->dribbler );
		 
		#ifdef ENABLE_SHOOTER
            // hwj hot fix in 2016.4.8 Iran Open
            g_is_shoot_in_main_loop = packet->shoot;
            g_is_chip_in_main_loop = packet->chip;
            
            //do_shoot(packet->shoot, packet->chip);
			//do_chip(packet->shoot, packet->chip);
		#endif

		//do_move( packet->speed_x, packet->speed_y, packet->speed_rot);
		do_acc_handle_move(packet->speed_x, packet->speed_y, packet->speed_rot, 25);
		
	}
}
