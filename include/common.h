#ifndef __COMMON_H__
#define __COMMON_H__
#define PROT_VERSION 0x01
#define INET_PORT 1002
//E0=gyr E1=tmp E2=rtk
struct tty_msg{
        char type;	 
        char data[0];
};
struct mpu6050_data{
	float a[3];     //xyz轴加速度
	float w[3];     //角加速度
	float angle[3]; //角度
};

struct tmp{
	float tmp;
};

typedef struct msg{
	unsigned char start;   //OxO1
//	char *sn;
	unsigned char version; //0x01 : first version
	unsigned char sign;    //F0实时数据   F1存储数据	
	struct tty_msg tty_msg;
	unsigned char end;    //0xFF
	unsigned int checksum;
}imx_msg_t;


struct rtk_gps{
        int time;
        int data;
};

#endif
