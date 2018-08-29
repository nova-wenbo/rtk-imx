#ifndef __COMMON_H__
#define __COMMON_H__

//E0=gyr E1=tmp E2=rtk
struct tty_msg{
        char type; 
        int  length;
        void *data;
};
struct rtk_gps{
        int time;
        int data;
};

#endif
