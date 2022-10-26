#ifndef FIREBIRDV_ATMEGA2560_H_
#define FIREBIRDV_ATMEGA2560_H_

#define IR_PORT PORTF
#define IR_SENSOR1 4

#define INT PORTE
#define POSENL 4
#define POSENR 5
#define BUTTON 7
#define MAX_DIGITS 7

#define MOTOR_PORT_DIR DDRA
#define MOTOR_PWM_PORT_DIR DDRL
#define MOTOR_PORT PORTA
#define MOTOR_PWM_PORT PORTL
#define LFWD 1 
#define LBWD 0	
#define RFWD 2
#define RBWD 3
#define LEFT 3
#define RIGHT 4

#define BUZZER 3
#define BUZZER_PORT PORTC

#define LCD_BUZZER_DIR DDRC 
#define LCD_COL 16
#define LCD_ROW 2
#define LCD_PORT PORTC
#define RS 0
#define RW 1
#define EN 2

#define LED_BAR_GRAPH_PORT PORTJ
#define LED_BAR_GRAPH_DIR DDRJ

void lcd_print_test(unsigned char,unsigned char,unsigned char);

void firebird_init(void);
void velocity_control(void);
void motorspeed_config(void);
void velocity(void);
void lcd_print_dist(unsigned char,unsigned char);
void dis_data_op(void);
void interrupt_begin(void);
void adc_begin(void);
unsigned int adc_conv(unsigned char); // enter ADC number 
void motor_begin(void);
void motion_set(unsigned char);
void forward(void);
void backward(void);
void stop(void);
void left(void);
void softleft(void);
void softleft2(void);
void right(void);
void softright(void);
void softright2(void);
void buzzer_on(void);
void buzzer_off(void);
void ledbarlevel(unsigned char); //enter no of led bars to glow
void lcd_begin(void);
void lcd_clear(void); //clear lcd
void lcd_setcursor(unsigned char,unsigned char); //col,row
void lcd_cursor(void); // turn on cursor on lcd
void lcd_cursoroff(void); // turn off cursor on lcd
void lcd_ctrl(unsigned char);
void lcd_print(unsigned char*); // "Hello World!"
void lcd_printa(unsigned char); // enter ascii value directly

unsigned char upper,lower,logic,stopper,unit;
unsigned long posl,posr;
int cnt;
long distance,lft,rgt;
char distance_data[MAX_DIGITS];
#endif


