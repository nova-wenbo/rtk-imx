#ifndef __COMMON_H__
#define __COMMON_H__

//E0=gyr E1=tmp E2=rtk
struct tty_msg{
        char type;	 
        void *data;
};
struct mpu6050_data{
	float a[3];     //xyz轴加速度
	float w[3];     //角加速度
	float angle[3]; //角度
};

typedef struct msg{
	unsigned char start;
	unsigned char version;
	unsigned char sign;  //F0实时数据   F1存储数据	
	struct tm *time;
	struct tty_msg *tty_msg;
	unsigned int checksum;
	unsigned char end;
}imx_msg_t;


struct rtk_gps{
        int time;
        int data;
};

#endif
