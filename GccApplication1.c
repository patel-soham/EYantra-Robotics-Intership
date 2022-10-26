/*
 * GccApplication1.c
 *
 * Created: 01-06-2019 12:35:02
 *  Author: soham
 */ 
#define F_CPU 14745600 //working frequency of firebirdv atmega2560 is 14.7456Mhz
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "firebirdv_atmega2560.h"




int main(void)
{
	firebird_init();
	forward();
	lcd_print("dist:");
    while(1)
    {
		//lcd_printa(48+(snsr/1000));
		/*lcd_printa(48+((snsr/100)%10));
		lcd_printa(48+((snsr%100)/10));
		lcd_printa(48+((snsr%100)%10));
		snsr=adc_conv(5);
		_delay_ms(100);
		lcd_setcursor(0,0);*/
		lcd_print_dist(6,0);
		lcd_print_test(0,1,1);
		lcd_print_test(9,1,0);
		velocity_control();
		_delay_ms(50);
    }
}
