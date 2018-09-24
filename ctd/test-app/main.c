#include "gps.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>


char *GPGGA = "$GPGGA,061838.00,3413.3305281,N,10851.9637132,E,4,04,1.0,431.294,M,-33.496,M,1.0,*65";
char *GPRMC = "$GPRMC,061843.00,A,3413.3300688,N,10851.9651599,E,0.20,154.65,090918,0.0,E,D*30";
int main()
{
	gps_info gps;
	gps_rmc_parse(GPRMC, &gps);
	gps_gga_parse(GPGGA, &gps);
	printf("height : %02f, satellite : %d \n", gps.height, gps.satellite);
	printf("time : %d-%d-%d-%d:%d:%d \n",gps.D.year,gps.D.month,gps.D.day,gps.D.hour,gps.D.minute,gps.D.second);
	printf("gps: %lf-%lf\n", gps.latitude, gps.longitude);	
	printf("latitude : %d-%d-%d\n",gps.latitude_Degree,gps.latitude_Cent,gps.latitude_Second);
	return 0;
}
