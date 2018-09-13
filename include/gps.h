#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "debuglog.h"
#include "common.h"

typedef struct{
    int year; 
    int month; 
    int day;
    int hour;
    int minute;
    int second;
}DATE_TIME;

typedef struct{
    double latitude; //经度
    double longitude; //纬度
    int latitude_Degree;    //度
    int latitude_Cent;   //分
    int latitude_Second; //秒
    int longitude_Degree;    //度
    int longitude_Cent;  //分
    int longitude_Second; //秒
    float speed; //速度
    float direction; //航向
    float height; //海拔高度
    int satellite;
    char     NS;
    char    EW;
    DATE_TIME D;
}gps_info;



extern gps_rmc_parse(char *line, gps_info *GPS);
extern int gps_gga_parse(char *line, gps_info *GPS);
