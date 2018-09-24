#ifndef __COMMON_H__
#define __COMMON_H__

#define PROT_VERSION 0x01
#define MPUDATA 0xE0
#define GPSDATA 0xE2
#define TMPDATA 0XE1

#define RSENVAL 0    //return successful value
#define RFENVAL -1   //return faild value
#define OFENVAL -2   //open device faild

typedef unsigned char uchar;
//E0=gyr E1=tmp E2=rtk
struct mpu6050_data{
	float a[3];     //xyz轴加速度
	float w[3];     //角加速度
	float angle[3]; //角度
};
struct rtk_data{
        int time;
        int data;
};

typedef struct{
	uchar start;   //OxO1
	uchar version; //0x01 : first version
	uchar sign;    //F0实时数据   F1存储数据
	uchar type;
	struct mpu6050_data data;	
	uchar end;    //0xFF
}gyr_msg;
typedef struct{
        uchar start;   //OxO1
        uchar version; //0x01 : first version
        uchar sign;    //F0实时数据   F1存储数据
        uchar type; 
        uchar data;
        uchar end;    //0xFF
}temp_msg;

typedef struct{
        uchar start;   //OxO1
        uchar version; //0x01 : first version
        uchar sign;    //F0实时数据   F1存储数据
        uchar type;
        struct rtk_data data;
        uchar end;    //0xFF
}gps_msg;
/*-------------------------------------------------------------*/
/*  start | version |   sign   |  type  | data |   end  |      */
/*-------------------------------------------------------------*/
/* 起始位 |协议版本 |数据实时性|数据类型| 数据 | 结束位 |      */
/*-------------------------------------------------------------*/




#endif
