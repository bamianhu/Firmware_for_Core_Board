#ifndef _CONFIG_H_
#define _CONFIG_H_

/**************************
  Function selection
***************************/
//#define ENABLE_BEEP
#define ENABLE_LED
#define ENABLE_SHOOTER
#define ENABLE_HEARTBEAT
#define ENABLE_POWERMON
//#define ENABLE_SHOOT_COUNT
//#define ENABLE_2401_RESTART


/***************************
  System Configuration
***************************/
#define CHANNEL_NUM 4
#define ERROR_NUM 1000
//#define SHOOTER_NUM 2


/***************************
  Electrical & Mechanical 
      Configuration
***************************/
#define MOTOR_KP  210
#define MOTOR_KI  150

#define WHEEL_CENTER_OFFSET 0.082  /* 轮子距车中心距离(m) */
#define D_WHEEL_ANGLE_FRONT 58//60/* 前轮与轴线角度(度) */
#define D_WHEEL_ANGLE_BACK 135//125 //134 /* 后轮与轴线角度(度) */
#define D_WHEEL_REDUCTION_RATIO_X 3//3; /*减速比*/     // X.YZ
#define D_WHEEL_REDUCTION_RATIO_YZ 18//28; /*减速比*/   //X.YZ

#define ENCODER_COUNTS_PER_TURN_SET 360
#define WHEEL_RADIUS 0.028

#define SHOOT_DELAY 15


/***************************
  Circuit Configuration
***************************/

//#define D_MAX_DRIBBLER  125
//#define V_MAX_DRIBBLER  100

#define DO_PID_FRQ 500

#define CLKFREQ    100000000UL
#define TICKFREQ    1000 // 定时器频率
#define INFRA_FREQ 38000
#define INFRA_DUTY 0.2

#define SHOOTER_INTERVAL 1000// 两次射门间隔时间/ms 由Countdown限制

#define NRF2401_ADDR_COUNT    5

#define NRF2401_ADDR1_4     0x10
#define NRF2401_ADDR1_3     0x71
#define NRF2401_ADDR1_2     0x45
#define NRF2401_ADDR1_1     0x98
#define NRF2401_ADDR1_0     0x00

#define NRF2401_CAR_4     0x00
#define NRF2401_CAR_3     0x10
#define NRF2401_CAR_2     0x13
#define NRF2401_CAR_1     0x21
#define NRF2401_CAR_0     0x88

#define PACKET_LEN    25   //This Packet_len must accord with Transfer's Packet_len 
#define PACKET_LEN_UP 25	//2401向上通讯字节数//

#define LOWPOWERTIME 30
#define CAP_POWER_LOW 20 //15V

//#define STR_9557_SET 0;
//#define STEP_9557_SET 4;


#endif

