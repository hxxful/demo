#include <string.h>
#include "lib_math.h"

double linear_xy(double xn, double x1, double x2, double y1, double y2)
{
    double yn;
    if(xn < x1)yn = y1;
    else if(xn > x2)yn = y2;
    else if(y1 < y2) yn = y1 + ((y2 - y1) * (xn - x1) / (x2 - x1));

    else yn = y1 - ((y1 - y2) * (xn - x1) / (x2 - x1));
    return(yn);
}

double linearArray(double xn, const double Queue_x[], const double Queue_y[], uint32_t len)
{
    uint32_t i;
    double yn;
    for(i = 1; i < len - 1; i++) {
        if(xn < Queue_x[i])break;
    }
    yn = linear_xy(xn, Queue_x[i - 1], Queue_x[i], Queue_y[i - 1], Queue_y[i]);

    return(yn);
}

const double arctan_x[91] = {0.0f, 0.017455065f, 0.034920769f, 0.052407779f, 0.069926812f, 0.087488664f, \
                             0.105104235f, 0.122784561f, 0.140540835f, 0.15838444f, 0.176326981f, 0.194380309f, 0.212556562f, \
                             0.230868191f, 0.249328003f, 0.267949192f, 0.286745386f, 0.305730681f, 0.324919696f, 0.344327613f, \
                             0.363970234f, 0.383864035f, 0.404026226f, 0.424474816f, 0.445228685f, 0.466307658f, 0.487732589f, \
                             0.509525449f, 0.531709432f, 0.554309051f, 0.577350269f, 0.600860619f, 0.624869352f, 0.649407593f, \
                             0.674508517f, 0.700207538f, 0.726542528f, 0.75355405f, 0.781285627f, 0.809784033f, 0.839099631f, \
                             0.869286738f, 0.900404044f, 0.932515086f, 0.965688775f, 1.0f, 1.035530314f, 1.07236871f, 1.110612515f, \
                             1.150368407f, 1.191753593f, 1.234897157f, 1.279941632f, 1.327044822f, 1.37638192f, 1.428148007f, \
                             1.482560969f, 1.539864964f, 1.600334529f, 1.664279482f, 1.732050808f, 1.804047755f, 1.880726465f, \
                             1.962610506f, 2.050303842f, 2.144506921f, 2.246036774f, 2.355852366f, 2.475086853f, 2.605089065f, \
                             2.747477419f, 2.904210878f, 3.077683537f, 3.270852618f, 3.487414444f, 3.732050808f, 4.010780934f, \
                             4.331475874f, 4.704630109f, 5.144554016f, 5.67128182f, 6.313751515f, 7.115369722f, 8.144346428f, \
                             9.514364454f, 11.4300523f, 14.30066626f, 19.08113669f, 28.63625328f, 57.28996163f, 65535.f
                            };

const double x0_90[91] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, \
                          28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, \
                          59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90
                         };

const double x0_pi[91] = {0.f, 0.017453293f, 0.034906585f, 0.052359878f, 0.06981317f, 0.087266463f, 0.104719755f,
                          0.122173048f, 0.13962634f, 0.157079633f, 0.174532925f, 0.191986218f, 0.20943951f, 0.226892803f, 0.244346095f,
                          0.261799388f, 0.27925268f, 0.296705973f, 0.314159265f, 0.331612558f, 0.34906585f, 0.366519143f, 0.383972435f,
                          0.401425728f, 0.41887902f, 0.436332313f, 0.453785606f, 0.471238898f, 0.488692191f, 0.506145483f, 0.523598776f,
                          0.541052068f, 0.558505361f, 0.575958653f, 0.593411946f, 0.610865238f, 0.628318531f, 0.645771823f, 0.663225116f,
                          0.680678408f, 0.698131701f, 0.715584993f, 0.733038286f, 0.750491578f, 0.767944871f, 0.785398163f, 0.802851456f,
                          0.820304748f, 0.837758041f, 0.855211333f, 0.872664626f, 0.890117919f, 0.907571211f, 0.925024504f, 0.942477796f,
                          0.959931089f, 0.977384381f, 0.994837674f, 1.012290966f, 1.029744259f, 1.047197551f, 1.064650844f, 1.082104136f,
                          1.099557429f, 1.117010721f, 1.134464014f, 1.151917306f, 1.169370599f, 1.186823891f, 1.204277184f, 1.221730476f,
                          1.239183769f, 1.256637061f, 1.274090354f, 1.291543646f, 1.308996939f, 1.326450232f, 1.343903524f, 1.361356817f,
                          1.378810109f, 1.396263402f, 1.413716694f, 1.431169987f, 1.448623279f, 1.466076572f, 1.483529864f, 1.500983157f,
                          1.518436449f, 1.535889742f, 1.553343034f, 1.570796327f
                         };

const double cos_y[91] = {1.f, 0.999847695f, 0.999390827f, 0.998629535f, 0.99756405f, 0.996194698f, \
                          0.994521895f, 0.992546152f, 0.990268069f, 0.987688341f, 0.984807753f, 0.981627183f, 0.978147601f, \
                          0.974370065f, 0.970295726f, 0.965925826f, 0.961261696f, 0.956304756f, 0.951056516f, 0.945518576f, \
                          0.939692621f, 0.933580426f, 0.927183855f, 0.920504853f, 0.913545458f, 0.906307787f, 0.898794046f, \
                          0.891006524f, 0.882947593f, 0.874619707f, 0.866025404f, 0.857167301f, 0.848048096f, 0.838670568f, \
                          0.829037573f, 0.819152044f, 0.809016994f, 0.79863551f, 0.788010754f, 0.777145961f, 0.766044443f, \
                          0.75470958f, 0.743144825f, 0.731353702f, 0.7193398f, 0.707106781f, 0.69465837f, 0.68199836f, \
                          0.669130606f, 0.656059029f, 0.64278761f, 0.629320391f, 0.615661475f, 0.601815023f, 0.587785252f, \
                          0.573576436f, 0.559192903f, 0.544639035f, 0.529919264f, 0.515038075f, 0.5f, 0.48480962f, 0.469471563f, \
                          0.4539905f, 0.438371147f, 0.422618262f, 0.406736643f, 0.390731128f, 0.374606593f, 0.35836795f, \
                          0.342020143f, 0.325568154f, 0.309016994f, 0.292371705f, 0.275637356f, 0.258819045f, 0.241921896f, \
                          0.224951054f, 0.207911691f, 0.190808995f, 0.173648178f, 0.156434465f, 0.139173101f, 0.121869343f, \
                          0.104528463f, 0.087155743f, 0.069756474f, 0.052335956f, 0.034899497f, 0.017452406f, 0.f
                         };

const double sin_y[91] = {0.0f, 0.017452406f, 0.034899497f, 0.052335956f, 0.069756474f, 0.087155743f, 0.104528463f,
                          0.121869343f, 0.139173101f, 0.156434465f, 0.173648178f, 0.190808995f, 0.207911691f, 0.224951054f, 0.241921896f,
                          0.258819045f, 0.275637356f, 0.292371705f, 0.309016994f, 0.325568154f, 0.342020143f, 0.35836795f, 0.374606593f,
                          0.390731128f, 0.406736643f, 0.422618262f, 0.438371147f, 0.4539905f, 0.469471563f, 0.48480962f, 0.5f, 0.515038075f,
                          0.529919264f, 0.544639035f, 0.559192903f, 0.573576436f, 0.587785252f, 0.601815023f, 0.615661475f, 0.629320391f,
                          0.64278761f, 0.656059029f, 0.669130606f, 0.68199836f, 0.69465837f, 0.707106781f, 0.7193398f, 0.731353702f, 0.743144825f,
                          0.75470958f, 0.766044443f, 0.777145961f, 0.788010754f, 0.79863551f, 0.809016994f, 0.819152044f, 0.829037573f, 0.838670568f,
                          0.848048096f, 0.857167301f, 0.866025404f, 0.874619707f, 0.882947593f, 0.891006524f, 0.898794046f, 0.906307787f,
                          0.913545458f, 0.920504853f, 0.927183855f, 0.933580426f, 0.939692621f, 0.945518576f, 0.951056516f, 0.956304756f,
                          0.961261696f, 0.965925826f, 0.970295726f, 0.974370065f, 0.978147601f, 0.981627183f, 0.984807753f, 0.987688341f,
                          0.990268069f, 0.992546152f, 0.994521895f, 0.996194698f, 0.99756405f, 0.998629535f, 0.999390827f, 0.999847695f, 1.0f
                         };


//const double tan_y[91]={0.0f,0.017455065f,0.034920769f,0.052407779f,0.069926812f,0.087488664f,0.105104235f,0.122784561f,
//	0.140540835f,0.15838444f,0.176326981f,0.194380309f,0.212556562f,0.230868191f,0.249328003f,0.267949192f,0.286745386f,
//	0.305730681f,0.324919696f,0.344327613f,0.363970234f,0.383864035f,0.404026226f,0.424474816f,0.445228685f,0.466307658f,
//	0.487732589f,0.509525449f,0.531709432f,0.554309051f,0.577350269f,0.600860619f,0.624869352f,0.649407593f,0.674508517f,
//	0.700207538f,0.726542528f,0.75355405f,0.781285627f,0.809784033f,0.839099631f,0.869286738f,0.900404044f,0.932515086f,
//	0.965688775f,1.0f,1.035530314f,1.07236871f,1.110612515f,1.150368407f,1.191753593f,1.234897157f,1.279941632f,1.327044822f,
//	1.37638192f,1.428148007f,1.482560969f,1.539864964f,1.600334529f,1.664279482f,1.732050808f,1.804047755f,1.880726465f,
//	1.962610506f,2.050303842f,2.144506921f,2.246036774f,2.355852366f,2.475086853f,2.605089065f,2.747477419f,2.904210878f,
//	3.077683537f,3.270852618f,3.487414444f,3.732050808f,4.010780934f,4.331475874f,4.704630109f,5.144554016f,5.67128182f,
//	6.313751515f,7.115369722f,8.144346428f,9.514364454f,11.4300523f,14.30066626f,19.08113669f,28.63625328f,57.28996163f,65535.f};

double myarctan(double atx)
{
    if(atx >= 0)	return(		 linearArray( atx, arctan_x, x0_pi, 91)	);
    else 	    return(		-linearArray(-atx, arctan_x, x0_pi, 91)	);
}
double myarctan90(double atx)
{
    return(		myarctan(atx) * 180 / PI()	);
}


double myatan2(double aty, double atx)
{
    if(atx >= 0)			return(		 myarctan(aty / atx)	);		//x>0				0- +-PIdv2()
    else if(aty >= 0) 	    return(	PI() + myarctan(aty / atx)	);		//x<0 y>0 atan<0;	PI()  -
    else  			 	    return(-PI() + myarctan(aty / atx)	);		//x<0 y<0 atan>0	-PI() +
}

double mycos(double atx)
{
    while(atx > PI())atx -= PImt2();
    while(atx < -PI())atx += PImt2();

    if(atx >= PIdv2())      	return(-linearArray(PI() - atx,	x0_pi, cos_y, 91)	);
    else if(atx >= 0)  		return( linearArray(atx,		x0_pi, cos_y, 91)	);
    else if(atx >= -PIdv2())	return( linearArray(-atx,		x0_pi, cos_y, 91)	);
    else 		  	 		return(-linearArray(PI() + atx,	x0_pi, cos_y, 91)	);

}

double mysin(double atx)
{
    while(atx > PI())atx -= PImt2();
    while(atx < -PI())atx += PImt2();

    if(atx >= PIdv2())      		return( linearArray(PI() - atx,	x0_pi, sin_y, 91)	);
    else if(atx >= 0)  			return( linearArray( atx,		x0_pi, sin_y, 91)	);
    else if(atx >= -PIdv2())		return(-linearArray(-atx,		x0_pi, sin_y, 91)	);
    else 		  	 			return(-linearArray(PI() + atx,	x0_pi, sin_y, 91)	);

}

double myarcsin(double atx)
{
    if		(atx > 1)	return(		 PIdv2()		);
    else if(atx < -1)	return(		-PIdv2()		);

    else if(atx >= 0)	return(		 linearArray( atx, sin_y, x0_pi, 91)	);
    else 	    	return(		-linearArray(-atx, sin_y, x0_pi, 91)	);
}



double mytan(double atx)
{
    while(atx > PIdv2())atx -= PI();
    while(atx < -PIdv2())atx += PI();

    if(atx >= 0)  					return( linearArray( atx,	x0_pi, arctan_x, 91)	);
    else 		  	 				return(-linearArray(-atx,	x0_pi, arctan_x, 91)	);

}

int32_t myabs(int32_t temp)
{
    if(temp > 0)	return temp;
    else	return (-temp);

}

double myfabs(double temp)
{
    if(temp > 0)	return temp;
    else	return (-temp);

}

double mysqrt(double atx)
{
    double x, x0, f, f1;
    double err = 0.01;
    if(atx < 0)return(0);
    x = 1;
    do {
        x0 = x;
        f = x0 * x0 - atx;
        f1 = 2 * x0;
        x = x0 - f / f1;
    } while(myfabs(x - x0) > err);

    return(x);
}


double mysqrterr(double atx, double err)
{
    double x, x0, f, f1;
    if(atx < 0)return(0);
    x = 1;
    do {
        x0 = x;
        f = x0 * x0 - atx;
        f1 = 2 * x0;
        x = x0 - f / f1;
    } while(myfabs(x - x0) > err);

    return(x);
}


double clamp(double x	, double max,	double min)
{
    if(x > max)	return(max);
    else if(x < min)	return(min);
    else			return(x );
}

int32_t clampint32_t(int32_t x, int32_t max,	int32_t min)
{
    if(x > max)	return(max);
    else if(x < min)	return(min);
    else			return(x );
}


double filterg1(double fin, double* lastdata, int32_t *coff, int32_t *dir, int32_t *dirlt)
{

    if( fin > (*lastdata)) 	(*dir) = 1;
    else 	(*dir) = -1;


    if( (*dir) * (*dirlt) > 0)	(*coff)++;
    else	(*coff) = 1;
    (*dirlt) = (*dir);

    (*lastdata) = ((*coff) * fin + (19 * (*lastdata)) ) / ((*coff) + 19);

    return((*lastdata));
}

double filterg2(double fin, double* lastdata, int32_t *coff, int32_t *dir, int32_t *dirlt, 	int32_t base)
{

    if( fin > (*lastdata)) 	(*dir) = 1;
    else 	(*dir) = -1;


    if( (*dir) * (*dirlt) > 0)	(*coff)++;
    else	(*coff) = 1;
    (*dirlt) = (*dir);

    if(*coff < base / 2)
        (*lastdata) = ((*coff) * fin 		+ (base * (*lastdata)) ) / ((*coff)	+ base);
    else
        (*lastdata) = ((*coff) * 2 * fin 	+ (base * (*lastdata)) ) / ((*coff) * 2	+ base);

    return((*lastdata));
}

int32_t exptr(int32_t pin, int32_t expv)
{

    int32_t tmp = myabs(pin);
    if( tmp < expv)  return( tmp * pin / expv);		   // |x| < expv
    else return pin;
}


int32_t clamplast(int32_t pin,	int32_t max	, uint32_t n, int32_t* last, uint32_t* timer)
{
    if(myabs(pin -	*last)	<= max ) {
        *timer = 0;
        return(pin);
    } else if(*timer < n)		{
        (*timer) ++ ;
        return(*last);
    } else 			{
        *timer = 0;
        *last = pin ;
        return( pin );
    }
}

float clamplastf(float pin,	float max	, uint32_t n, float* last, uint32_t* timer)
{
    if(myfabs(pin -	*last)	<= max )	{
        *timer = 0;
        return(pin);
    } else if(*timer < n)		{
        (*timer) ++ ;
        return(*last);
    } else 			{
        *timer = 0;
        *last = pin ;
        return( pin );
    }
}

uint32_t pow10(uint32_t n)
{
    uint32_t tmp = 1;

    while(n != 0) {
        tmp *= 10;
        n--;
    };

    return(tmp);
}

char* my_itoa(int value, int radix, char *str)
{
    int sign = 0;
    int i = 0;
    int j = 0;
    //char *s = str;
    char ps[32];
    memset(ps, 0, 32);

    if(value < 0) {
        sign = -1;
        value = -value;
    }
    do {
        if(value % radix > 9)
            ps[i] = value % radix + '0' + 7;
        else
            ps[i] = value % radix + '0';
        i++;
    } while((value /= radix) > 0);
    if(sign < 0)
        ps[i] = '-';
    else
        i--;
    for(j = i; j >= 0; j--) {
        str[i - j] = ps[j];
    }
    return str;
}

char *my_ftoa(double number, int ndigit, char *buf)
{
    long int_part;
    double float_part;
    char str_int[32];
    char str_float[32];
    int i = 0;
    memset(str_int, 0, 32);
    memset(str_float, 0, 32);
    int_part = (long)number;
    float_part = number - (double)int_part + 1;
    my_itoa(int_part, 10, str_int);

    if(ndigit > 0) {
        float_part = myfabs((double)(pow10(ndigit) * float_part)) + 0.01;
        my_itoa((long)float_part, 10, str_float);
        str_float[ndigit + 1] = '\0';
        i = strlen(str_int);
        str_int[i] = '.';
        strcat(str_int, &str_float[1]);
    }
    strcpy(buf, str_int);
    return buf;
}





