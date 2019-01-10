/*
 * anenometer.c
 *
 * Created: 06.01.2019 02:02:02
 * Author : user
 */ 

#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include "I2C_slave.h"

void timer_config();
void abs_read();
void abs_encoder_config();

volatile uint8_t db_buffer = 0;
volatile uint8_t bf_index = 0;
volatile uint16_t abs_input = 0;
volatile uint8_t abs_flag = 0;
volatile uint8_t abs_message_counter = 0;
volatile uint8_t debug_flag = 0;



int main(void)
{
	
	
	DDRD |=1<<2;
	PORTD |= 1<<2;
	db_buffer = 15;
	cli();
	I2C_init(6);
	timer_config();
	abs_encoder_config();
	sei();
	
    /* Replace with your application code */
    while (1) 
    { 
		db_buffer=rxbuffer[1];
		
		txbuffer[0]=8;
		txbuffer[1]=rxbuffer[1];
		txbuffer[2]=(uint8_t)(abs_input & 0xFF);
		txbuffer[3]=(uint8_t)(abs_input>>8);
		txbuffer[4]=abs_message_counter;
		txbuffer[5]=debug_flag;
		txbuffer[6]=abs_flag;
		abs_message_counter++;
		
		abs_read();
		while(abs_flag==1)
		{
			txbuffer[5]=debug_flag;
			txbuffer[6]=abs_flag;
			//
		}
		while()
		{
			
			
		}
		
		//db_buffer=0;
		
    }
}

void abs_read()
{
	abs_flag = 1;
	PORTD &= ~(1<<PORTD1);
	TIMSK0 |=  1<<OCIE0A;
}

void timer_config()
{
	  TCCR0A |= 1<<COM0A0 | 1<<COM0A1; //set on compare match PWM
	  TCCR0A |= (1<<WGM01); //CTC
	  TCCR0B |= (1<<CS01) | (1<<CS00); //prescaler /64
	  TCNT0 = 0; //timer initial value
	  //TIMSK0 |=  1<<OCIE0A;
	  OCR0A = 37;
}

void abs_encoder_config()
{
	DDRD |= 1<<DDD0; //ABS_CSn as output
	PORTD |= 1<<PORTD0; //default inactive
	DDRD |= 1<<DDD6; //ABS_CLK as output
	PORTD |= 1<<PORTD6; //default high
	DDRD &= ~(1<<DDD6); //ABS_DO as input
	
	//programming pin, always high
	DDRB |= 1<<DDB2;
	PORTB |=1<<PORTB2;
}

ISR(TIMER0_COMPA_vect)
{
	/* Kubas debugging
	if(bf_index<=7){
		if(db_buffer & (1<<bf_index) ){
			PORTD |= (1<<2);
		}
		else{
			PORTD &=~(1<<2);
		}
	}
	
	if(bf_index>7 && bf_index<15){
		if(bf_index==14){
			PORTD &=~(1<<2);
		}
		else{
			PORTD |= (1<<2);
		}
		
		bf_index++;
	}
	else if(bf_index>=15){
		bf_index=0;
		PORTD |= (1<<2);
	}
	else{
		bf_index++;
	}*/
	debug_flag ++;//= 1;
	if (PIND & (1<<PIND6))
	{
		if(abs_message_counter)
		{
			if(PIND & (1<<PIND5))
			{
				abs_input |= 1<<abs_message_counter;
			}
			else
			{
				abs_input &= ~(1<<abs_message_counter);
			}
		}
		abs_message_counter++;
		if(abs_message_counter > 17) //17
		{
			//end of transmission
			abs_message_counter = 0;
			abs_flag = 0;
			TIMSK0 &=  ~(1<<OCIE0A);
			PORTD |= (1<<PORTD1);
			
		}
	}
	if(abs_message_counter)
	{
		PORTD ^= 1<<PORTD6;
	}

	
}
