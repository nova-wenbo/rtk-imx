#include <stdio.h>


int main(int argc , char** argv)
{
	double num1 = 3413.814760;
	double num2 = 10852.251187;
	num1 = (double)((int)(num1/100) + (num1 - 100*(int)(num1/100))/60);
	num2 = (double)((int)(num2/100) + (num2 - 100*(int)(num2/100))/60);
	printf("num1 = %0.9lf\n",num1);
	printf("num2 = %0.9lf\n",num2);
	return 0;
	
}
