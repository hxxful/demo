#ifndef LIB_MATH_H
#define LIB_MATH_H

#include <stdint.h>

#define PI() 	3.1415926535898F
#define PIdv2() 1.5707963267949F
#define PImt2() 6.2831853071796F

double linear_xy(double xn,double x1,double x2,double y1,double y2);
double linearArray(double xn,const double Queue_x[],const double Queue_y[],uint32_t len);	

double myarctan90(double atx);
double myarctan(double atx);
double myatan2(double aty, double atx);

double myarcsin(double atx);

double mycos(double atx);
double mysin(double atx);
double mytan(double atx);

int32_t myabs(int32_t temp);
double myfabs(double temp);


double mysqrt(double atx);
double mysqrterr(double atx,double err);

double clamp(double x	,double max,	double min);
int32_t clampint32_t(int32_t x	,int32_t max,	int32_t min);

double filterg1(double fin, double* lastdata,int32_t *coff,int32_t *dir,int32_t *dirlt);
double filterg2(double fin, double* lastdata,int32_t *coff,int32_t *dir,int32_t *dirlt, 	int32_t base);

int32_t exptr(int32_t pin,int32_t expv);
int32_t clamplast(int32_t pin,	int32_t max	,uint32_t n, int32_t* last,uint32_t* timer);
float clamplastf(float pin,	float max	,uint32_t n, float* last,uint32_t* timer);
uint32_t pow10(uint32_t n);
char* my_itoa(int value, int radix, char *str);
char *my_ftoa(double number,int ndigit,char *buf);

#endif
