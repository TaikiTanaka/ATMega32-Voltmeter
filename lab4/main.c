
#include <avr/io.h>
#include "avr.h"
#include "lcd.h"
#include <stdio.h>
float getAverage(float avg, float current);

struct reading{
	float min,max,current,avg,sum;
	int count;
};

int is_pressed(int r, int c)
{
	//Setting Port C as N/C
	DDRC = 0;
	PORTC = 0;
	
	//Set r to strong 0
	SET_BIT(DDRC,r);
	CLR_BIT(PORTC,r);
	
	//Set col to weak 1
	CLR_BIT(DDRC,c+4);
	SET_BIT(PORTC,c+4);
	
	//Since pullup resister, if its high then return false
	wait_avr(1);
	if(GET_BIT(PINC,c+4))
	{
		return	0;
	}
	return 1;
}

int get_key()
{
	//r and c are the higher and lower bits on PORTC respectively
	//simply scans through every cell and checks if pressed
	int row,col;
	for (row=0;row<4;++row)
	{
		for (col=0;col<4;++col)
		{
			if(is_pressed(row,col))
			{
				wait_avr(300);
				return row*4 + col + 1;
			}
			//returns the index of the keypad
		}
	}
	return 0;
}
int translateKey(int k)
{
	//A -- enter programming mode
	if(k == 1)
	{return 1;}
	
	if(k == 2)
	{return 2;}
	
	if(k == 3)
	{return 3;}
	
	if(k == 4)
	{return 10;}
	
	if(k == 5)
	{return 4;}
	
	if(k==6)
	{return 5;}
	
	if(k==7)
	{return 6;}
		
	if(k==8)
	{return 11;}
	
	if(k==9)
	{return 7;}
	
	if(k==10)
	{return 8;}
	
	if(k==11)
	{return 9;}
	
	if(k==12)
	{return 12;}
	
	if(k==13)
	{return 13;}
	
	if(k==14)
	{return 0;}
		
	if(k==15)
	{return 15;}
			
	if(k==16)
	{return 16;}

	//If nothing is pressed
	return 17;
}

void displayKey(int key)
{
	char buf[17];
	pos_lcd(0,0);
	sprintf(buf,"%02i",key);
	puts_lcd2(buf);
}

unsigned short get_a2c() {
	CLR_BIT(ADMUX, 7); // REFS1 - 0
	SET_BIT(ADMUX, 6); // REFS0 - 1
	SET_BIT(ADCSRA, 7); // ADEN - 1
	SET_BIT(ADCSRA, 6); // ADSC - 1
	return ADC;
}

float convertVoltage(unsigned short raw) {
	return raw / 1024.0 * 5.0;
}

void updateReading(struct reading * rd)
{
	float current = convertVoltage(get_a2c());
	rd->current = current;
	if(current > rd->max)
	{
		rd->max = current;
	}
	if(current < rd->min)
	{
		rd->min = current;
	}
	rd->count += 1;
	rd->sum+=current;
	rd->avg = rd->sum/rd->count;
}

void displayLCD(struct reading * rd)
{
	char buf[17];
	pos_lcd(0,0);
	unsigned short whole1,whole2,frac1,frac2;
	whole1 = (int)rd->min;
	whole2 = (int)rd->max;
	frac1 = (rd->min - (int)rd->min)*100 ;
	frac2 = (rd->max - (int)rd->max)*100 ;
	sprintf(buf,"m:%02i.%02i x:%02i.%02i ",whole1,frac1,whole2,frac2);
	puts_lcd2(buf);
	
	whole1 = (int)rd->current;
	whole2 = (int)rd->avg;
	frac1 = (rd->current - (int)rd->current)*100;
	frac2 = (rd->avg - (int)rd->avg)*100;
	pos_lcd(1,0);
	sprintf(buf,"i:%02i.%02i a:%02i.%02i ",whole1,frac1,whole2,frac2);
	puts_lcd2(buf);
}
void initializeReading(struct reading * rd)
{
	rd->min = 5;//makes sure it goes down to some initial min value
	rd->max = 0;
	rd->current = 0;
	rd->avg = 0;
	rd->count = 0;
	rd->sum = 0;
}
int main(void)
{
	//disabling jtag
	MCUCSR = (1<<JTD);
	MCUCSR = (1<<JTD);
	
	char buf[17];
	ini_lcd();
	int key;
	//setting Port A as an input
	CLR_BIT(DDRA,0);

	struct reading rd;
	initializeReading(&rd);
	int sample = 0;
	int initial = 0;

	while(1)
	{
		key = translateKey(get_key());
		if(key == 1)
		{
			initializeReading(&rd);
			clr_lcd();
			pos_lcd(0,0);
			sprintf(buf,"Ready to sample");
			puts_lcd2(buf);
			sample = 0;
		}
		//initiate sampling
		if (key==2)
		{
			sample = 1;
		}
		else if (1==sample)
		{
			updateReading(&rd);
			displayLCD(&rd);
			wait_avr(500);//sample every 500ms
		}
	}//end while
}//end main


