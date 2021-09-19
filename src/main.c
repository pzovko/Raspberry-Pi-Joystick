/*
*
* RPi Joystick - TLC0820 ADC
*
* Written by: Petar Zovko
* Last changed: 01.06.2020
*
*/


#include <stdio.h>
#include <unistd.h>
#include <pigpio.h> // http://abyz.me.uk/rpi/pigpio/download.html

#define DEBUG 0

#define clrscr() 	 printf("\e[1;1H\e[2J")
#define hidecursor() printf("\e[?25l")

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 30


#if DEBUG
#define CLK_ADC 22
short pulse_adc(void);
#endif

void printVal(double, double);

const short ADC_X[8] = {3, 5, 7, 11, 13, 15,  19, 21}; 		// LSB - MSB
const short ADC_Y[8] = {2, 4, 6, 8, 9, 10,  12, 14};    	// LSB - MSB


int main (void)
{
  int x_value_read[8], y_value_read[8];
  int x_val = 0, y_val = 0;

  hidecursor();

  if (gpioInitialise()<0)
  {
	  printf("Error initialising PiGPIO");
	  return true;
  }

  #if DEBUG
  if(gpioSetMode(CLK_ADC, PI_OUTPUT))
  {
	  printf("Error setting CLK pin");
	  return true;
  }
  #endif

  for(int i = 0; i <= 7; i++)
  {
	  if(gpioSetMode(ADC_X[i], PI_INPUT)||gpioSetPullUpDown(ADC_X[i], PI_PUD_UP))
	  {
		  printf("Error setting pin mode X");
		  return 1;
	  }

	  if(gpioSetMode(ADC_Y[i], PI_INPUT)||gpioSetPullUpDown(ADC_Y[i], PI_PUD_UP))
	  {
		  printf("Error setting pin mode Y");
		  return 1;
	  }
  }

  while(true)
  {
	  #if DEBUG
	  if(pulse_adc())
		return 1;
	  usleep(200);
	  #endif

	  for(int i = 0; i <= 7; i++)
	  {
		  if(PI_BAD_GPIO == (x_value_read[i] = gpioRead(ADC_X[i])))
			  return 1;
		  x_val |= x_value_read[i] << i;

		  if(PI_BAD_GPIO == (y_value_read[i] = gpioRead(ADC_Y[i])))
			  return 1;
		  y_val |= y_value_read[i] << i;
	  }

	  #if DEBUG
		  printf("X bits: ");
		  for(int i = 0; i <= 7; i++)
		  {
			  printf("%d", x_value_read[i]);
		  }
		  printf("\n%d\n", y_val);

		  printf("\nY bits: ");
		  for(int i = 0; i <= 7; i++)
		  {
			  printf("%d", y_value_read[i]);
		  }

		  printf("\n%d\n", x_val);
		  clrscr();
	  #else
		printVal(((double)x_val/(double)255), ((double)y_val/(double)255));
	  #endif

	  x_val = 0;
	  y_val = 0;
	  usleep(200);
  }

  gpioTerminate();
  return 0 ;
}

void printVal(double x, double y) {
    int x_val =  (int)((x * 100)+0.5), y_val =  (int)((y * 100)+0.5);
    int x_lpad =  (int)(x * PBWIDTH), y_lpad =  (int)(y * PBWIDTH);
    int x_rpad = PBWIDTH - x_lpad, y_rpad = PBWIDTH - y_lpad;
    clrscr();
    printf("X: %3d%% [%.*s%*s]\n\n", x_val, x_lpad, PBSTR, x_rpad, "");
    printf("Y: %3d%% [%.*s%*s]", y_val, y_lpad, PBSTR, y_rpad, "");

    fflush(stdout);
}

#if DEBUG
short pulse_adc()
{
	usleep(200);
	if(gpioWrite(CLK_ADC, 1))
	{
		printf("Error setting pin 1");
		return 1;
	}
	usleep(1);
	if(gpioWrite(CLK_ADC, 0))
	{
		printf("Error setting pin 0");
		return 1;
	}
	usleep(20);
	return 0;
}
#endif
