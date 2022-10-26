#define F_CPU 14745600 //working frequency of firebirdv atmega2560 is 14.7456Mhz

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "firebirdv_atmega2560.h"

ISR(INT7_vect)
{
	if(stopper)
	{
		forward();
		stopper=0;
	}
	else
	{
		stop();
		stopper=1;
	}
	sei();
}

ISR(INT5_vect)
{
	posr++;
	sei();
}

ISR(INT4_vect)
{
	posl++;
	sei();
}

void firebird_init(void)
{	
	lcd_begin();
	motor_begin();
	adc_begin();
	LED_BAR_GRAPH_DIR=0xff; //initialize led bar graph (8 leds)
	motorspeed_config();
	interrupt_begin();
}
void motorspeed_config(void)
{
	TCCR5A=0xa1;
	TCCR5B=0x0b;
	
	OCR5AL=0xff; //full velocity left motor 100% duty cycle
	OCR5BL=0xff; //full velocity right motor 100% duty cycle
	
	MOTOR_PWM_PORT_DIR=(1<<LEFT)|(1<<RIGHT);//pwm pins as output
	lft=255;
	rgt=255;
}
void velocity(void)
{
	OCR5AL=lft;
	OCR5BL=rgt;
}
void velocity_control(void)
{
	if(posr>posl)
	{
		if(lft<245)
		lft+=1;
		else
		rgt-=1;
	}
	else
	{
		if(rgt<245)
		rgt+=1;
		else
		lft-=1;
	}
	velocity();
}
void lcd_print_dist(unsigned char col,unsigned char row)
{
	lcd_setcursor(col,row);
	if(posl>posr)
	distance=(posl-posr)*0.544;
	else
	distance=(posr-posl)*0.544;
	dis_data_op();
	for(cnt=MAX_DIGITS-1;cnt>=0;cnt--)
	{
		logic+=distance_data[cnt];
		if(logic!=0)
		{
			lcd_printa(48+(distance_data[cnt]));
			if(unit==1 && cnt==2)
			{
				lcd_printa(0x2E); //ascii of decimal point
			}
		}
	}
	logic=0;
	/*if(unit)
	lcd_print("m");
	else
	lcd_print("cm");*/
}
void lcd_print_test(unsigned char col,unsigned char row,unsigned char side)
{
	lcd_setcursor(col,row);
	if(side==1)
	distance=(posl)*0.544;
	else
	distance=(posr)*0.544;
	dis_data_op();
	for(cnt=MAX_DIGITS-1;cnt>=0;cnt--)
	{
		logic+=distance_data[cnt];
		if(logic!=0)
		{
			lcd_printa(48+(distance_data[cnt]));
			if(unit==1 && cnt==2)
			{
				lcd_printa(0x2E); //ascii of decimal point
			}
		}
	}
	logic=0;
	/*if(unit)
	lcd_print("m");
	else
	lcd_print("cm");*/
}

void dis_data_op(void)
{
	//distance_data[6]=(distance%10000000)/1000000;
	//distance_data[5]=(distance%1000000)/100000;
	//distance_data[4]=(distance%100000)/10000;
	distance_data[3]=(distance%10000)/1000;
	distance_data[2]=(distance%1000)/100;
	distance_data[1]=(distance%100)/10;
	distance_data[0]=(distance%10);
	if(distance_data[2]>0)
	{
		unit=1;
	}
}

void interrupt_begin(void)
{
	cli();
	INT|=(1<<POSENL)|(1<<POSENR)|(1<<BUTTON); //activate pull-up resistor for both position encoders as well as for pushbutton 
	EICRB|=(1<<ISC71)|(1<<ISC51)|(1<<ISC41); //falling edge interrupt 
	EIMSK|=(1<<INT7)|(1<<INT5)|(1<<INT4);
	EIFR|=(1<<INTF7); //requires clearing of interrupt flag for edge trigerred interrupts
	//intf7 is 1 when isr is running and 0 after finishing isr
	sei();
}

void adc_begin(void)
{
	ADCSRA=(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADEN);
	ADCSRB=0x00;
	ADMUX=(1<<ADLAR);// |(1<<REFS1)|(1<<REFS0);
	ACSR=(1<<ACD); //Analog comparator disable
}

unsigned int adc_conv(unsigned char adc_no)
{
	unsigned int adc_value;
	if(adc_no > 7)
	{
		ADCSRB|=(1<<MUX5);
	}
	adc_no&=0x07;
	ADMUX=( (ADMUX & 0xE0) | (adc_no) );
	ADCSRA|=(1<<ADSC);
	while(!(ADCSRA & (1<<ADIF))); //wait for conversion time
	//lower=ADCL;
	upper=ADCH;
	adc_value=upper;
	adc_value=adc_value<<8;
	lower=lower>>6;
	adc_value=(upper | (lower & 0x03));
	ADCSRA|=(1<<ADIF); // clear interrupt flag
	ADCSRB&=~(1<<MUX5);
	return adc_value;
}

void forward(void)
{
	motion_set((1<<LFWD)|(1<<RFWD));
}
void backward(void)
{
	motion_set((1<<LBWD)|(1<<RBWD));
}
void left(void)
{
	motion_set((1<<RFWD)|(1<<LBWD));
}
void softleft2(void)
{
	motion_set((1<<LBWD));
}
void right(void)
{
	motion_set((1<<LFWD)|(1<<RBWD));
}
void softright2(void)
{
	motion_set((1<<RBWD));
}
void softleft(void)
{
	motion_set((1<<RFWD));
}
void softright(void)
{
	motion_set((1<<LFWD));
}
void stop(void)
{
	motion_set(0x00);
}

void motion_set(unsigned char data)
{
	MOTOR_PORT= ( (MOTOR_PORT & 0xf0) | (data) );
}

void motor_begin(void)
{
	MOTOR_PORT_DIR|=(1<<LFWD)|(1<<LBWD)|(1<<RFWD)|(1<<RBWD); //initialize inputs for left and right dc-motors connected to L293D IC
	MOTOR_PORT&=0xf0; //No direction of movement to DC motors at startup
	MOTOR_PWM_PORT_DIR|=(1<<LEFT)|(1<<RIGHT); //initialize velocity/PWM inputs for left and right dc-motors connected to L293D IC
	MOTOR_PWM_PORT&=0xe7;
	MOTOR_PWM_PORT|=0x18;
}

void buzzer_on(void)
{	
	BUZZER_PORT|=1<<BUZZER;
}

void buzzer_off(void)
{
	BUZZER_PORT&=(~(1<<BUZZER));
}

void ledbarlevel(unsigned char level)
{
	switch(level)
	{
		case 0:
		LED_BAR_GRAPH_PORT=0x00;
		break;
		case 1:
		LED_BAR_GRAPH_PORT=0x01;
		break;
		case 2:
		LED_BAR_GRAPH_PORT=0x03;
		break;
		case 3:
		LED_BAR_GRAPH_PORT=0x07;
		break;
		case 4:
		LED_BAR_GRAPH_PORT=0x0f;
		break;
		case 5:
		LED_BAR_GRAPH_PORT=0x1f;
		break;
		case 6:
		LED_BAR_GRAPH_PORT=0x3f;
		break;
		case 7:
		LED_BAR_GRAPH_PORT=0x7f;
		break;
		case 8:
		LED_BAR_GRAPH_PORT=0xff;
		break;
		default:
		LED_BAR_GRAPH_PORT=0x00;
		break;
	}
}

void lcd_begin(void)
{
	LCD_BUZZER_DIR=0xff; //initialize buzzer and lcd as output
	LCD_PORT=0x00; //No input to lcd
	_delay_ms(5);
	
	lcd_ctrl(0x33);
	lcd_ctrl(0x32);
	lcd_ctrl(0x28); 
		
	lcd_ctrl(0x0C); //cursor off
	lcd_ctrl(0x06); //move l to r
	lcd_ctrl(0x01); //clear
	lcd_ctrl(0x80); //starting address till 8f
}
void lcd_clear(void)
{
	lcd_ctrl(0x01);
	lcd_ctrl(0x80);
}
void lcd_setcursor(unsigned char a,unsigned char b)
{
	//a is col and b is row
	if(b==0)
	lcd_ctrl((0x80)+a);
	if(b==1)
	lcd_ctrl((0xc0)+a); //add of row 1 c0 to cf
	if(LCD_ROW==4 && LCD_COL==20)
	{
		if(b==2)
		lcd_ctrl((0x94)+a);
		if(b==3)
		lcd_ctrl((0xd4)+a);
	}
	
}

void lcd_cursor(void)
{
	lcd_ctrl(0x0E); //cursor on
}
void lcd_cursoroff(void)
{
	lcd_ctrl(0x0C);
}
void lcd_ctrl(unsigned char cmd)
{
	LCD_PORT&=~(1<<RS); //RS is  0 for commands
	LCD_PORT=(LCD_PORT & 0x0f) | (cmd & 0xf0); //sending upper nibble
	LCD_PORT|=1<<EN;
	_delay_ms(1);
	LCD_PORT&=~(1<<EN);
	_delay_ms(1);
	cmd=cmd<<4;
	LCD_PORT=(LCD_PORT & 0x0f) | (cmd & 0xf0); //sending lower nibble
	LCD_PORT|=1<<EN;
	_delay_ms(1);
	LCD_PORT&=~(1<<EN);
	_delay_ms(1);
}
void lcd_print(unsigned char *data)
{
	LCD_PORT|=1<<RS; //RS is 1 for data
	while(*data !='\0')
	{
		lcd_printa(*data);
		*data++;
	}
}
void lcd_printa(unsigned char ascii) //ascii value directly
{
	LCD_PORT|=1<<RS;
	LCD_PORT=(LCD_PORT & 0x0f) | (ascii & 0xf0); //sending upper nibble
	LCD_PORT|=1<<EN;
	_delay_ms(1);
	LCD_PORT&=~(1<<EN);
	_delay_ms(1);
	ascii=ascii<<4;
	LCD_PORT=(LCD_PORT & 0x0f) | (ascii & 0xf0); //sending lower nibble
	LCD_PORT|=1<<EN;
	_delay_ms(1);
	LCD_PORT&=~(1<<EN);
	_delay_ms(1);
}