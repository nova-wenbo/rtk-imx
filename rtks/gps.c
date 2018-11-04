#include "gps.h"

static int GetComma(int num,char *str)
{
    int i,j=0;
    int len=strlen(str);
    for(i=0;i<len;i++)
    {
        if(str[i]==',')j++;
        if(j==num)return i+1;   
    }
    return 0;   
}

static double Get_Double_Number(char *s)
{
    char buf[128];
    int i;
    double rev;
    i=GetComma(1,s);
    strncpy(buf,s,i);
    buf[i]=0;
    rev=atof(buf);
    return rev;

}

static float Get_Float_Number(char *s)
{
    char buf[128];
    int i;
    float rev;
    i=GetComma(1,s);
    strncpy(buf,s,i);
    buf[i]=0;
    rev=atof(buf);
    return rev;
}

static int Get_Int_Number(char *s)
{
    char buf[128];
    int i;
    int rev;
    i=GetComma(1,s);
    strncpy(buf,s,i);
    buf[i]=0;
    rev=atof(buf);
    return rev;
}


static void UTC2BTC(DATE_TIME *GPS)
{
   //如果秒号先出,再出时间数据,则将时间数据+1秒
    GPS->second++; //加一秒
    if(GPS->second>59){
    GPS->second=0;
    GPS->minute++;
    if(GPS->minute>59){
        GPS->minute=0;
        GPS->hour++;
    }
    }

    GPS->hour+=8;
    if(GPS->hour>23){
        GPS->hour-=24;
        GPS->day+=1;
        if(GPS->month==2 || GPS->month==4 ||GPS->month==6 ||GPS->month==9 ||GPS->month==11 ){
            if(GPS->day>30){
            GPS->day=1;
            GPS->month++;
        }
        }
        else{
            if(GPS->day>31){
                GPS->day=1;
                GPS->month++;
            }
        }

        if(GPS->year % 4 == 0 ){
             if(GPS->day > 29 && GPS->month ==2){
                        GPS->day=1;
                        GPS->month++;
            }
        }
        else{
            if(GPS->day>28 &&GPS->month ==2){
                        GPS->day=1;
                        GPS->month++;
            }
        }

        if(GPS->month>12){
            GPS->month-=12;
            GPS->year++;
        }

    }
}



int gps_rmc_parse(char *line, gps_info *GPS)
{
    char ch, status, tmp;
    float lati_cent_tmp, lati_second_tmp;
    float long_cent_tmp, long_second_tmp;
    float speed_tmp;
    char *buf = line;
    ch = buf[5];
    status = buf[GetComma(2, buf)];

    if (ch == 'C') //如果第五个字符是C，($GPRMC)
    {
        if (status == 'A') //如果数据有效，则分析
        {
            GPS->NS = buf[GetComma(4, buf)];
            GPS->EW = buf[GetComma(6, buf)];

            GPS->latitude = Get_Double_Number(&buf[GetComma(3, buf)]);
            GPS->longitude = Get_Double_Number(&buf[GetComma(5, buf)]);
	    
            GPS->latitude_Degree = (int)GPS->latitude / 100; //分离纬度
            lati_cent_tmp = (GPS->latitude - GPS->latitude_Degree * 100);
            GPS->latitude_Cent = (int)lati_cent_tmp;
            lati_second_tmp = (lati_cent_tmp - GPS->latitude_Cent) * 60;
            GPS->latitude_Second = (int)lati_second_tmp;

            GPS->longitude_Degree = (int)GPS->longitude / 100;    //分离经度
            long_cent_tmp = (GPS->longitude - GPS->longitude_Degree * 100);
            GPS->longitude_Cent = (int)long_cent_tmp; 
            long_second_tmp = (long_cent_tmp - GPS->longitude_Cent) * 60;
            GPS->longitude_Second = (int)long_second_tmp;

            speed_tmp = Get_Float_Number(&buf[GetComma(7, buf)]); //速度(单位：海里/时)
            GPS->speed = speed_tmp * 1.85; //1海里=1.85公里
            GPS->direction = Get_Float_Number(&buf[GetComma(8, buf)]); //角度            

            GPS->D.hour = (buf[7] - '0') * 10 + (buf[8] - '0');        //时间
            GPS->D.minute = (buf[9] - '0') * 10 + (buf[10] - '0');
            GPS->D.second = (buf[11] - '0') * 10 + (buf[12] - '0');
            tmp = GetComma(9, buf);
            GPS->D.day = (buf[tmp + 0] - '0') * 10 + (buf[tmp + 1] - '0'); //日期
            GPS->D.month = (buf[tmp + 2] - '0') * 10 + (buf[tmp + 3] - '0');
            GPS->D.year = (buf[tmp + 4] - '0') * 10 + (buf[tmp + 5] - '0') + 2000;
            UTC2BTC(&GPS->D);
            
            return 1;
        }        
    }
    
    return 0;
}

int gps_gga_parse(char *line, gps_info *GPS)
{
    char ch, status;
    char *buf = line;
    ch = buf[4];
    status = buf[GetComma(2, buf)];

    if (ch == 'G') //$GPGGA
    {
        if (status != ',')
        {
            GPS->height = Get_Float_Number(&buf[GetComma(9, buf)]);
            GPS->satellite = Get_Int_Number(&buf[GetComma(7, buf)]);

            return 1;
        }
    }
    
    return 0;
}

