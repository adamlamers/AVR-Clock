#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#define pin_low(port,pin) port &= ~(1<<pin)
#define pin_high(port,pin) port |= (1<<pin)
#define set_input(portdir,pin) portdir &= ~(1<<pin)
#define set_output(portdir,pin) portdir |= (1<<pin)
#define pin_toggle(port, pin) port ^= (1 << pin)

#define pulse_pin(port, pin) pin_low(port,pin); _delay_us(1); pin_high(port,pin)


//7-segment hookup position
// A B C D E F G || PORT B PIN:
// 1 2 3 4 5 6 7
// digit hookup position
// 1 2 3 4 || PORT D PIN:
// 1 2 3 4

#define DIGIT_1_PIN PD1
#define DIGIT_2_PIN PD2
#define DIGIT_3_PIN PD3
#define DIGIT_4_PIN PD4

#define COLON_PIN PD0

#define BUTTON_MINUTES PD5
#define BUTTON_HOURS PD6

#define SEGMENT_A_PIN PB1
#define SEGMENT_B_PIN PB2
#define SEGMENT_C_PIN PB3
#define SEGMENT_D_PIN PB4
#define SEGMENT_E_PIN PB5
#define SEGMENT_F_PIN PB6
#define SEGMENT_G_PIN PB7


long millis = 0;
int16_t seconds = 35;
int8_t minutes = 0;
int8_t hours = 5;
int8_t lastButtonState = 0;
int8_t lastHourButtonState = 0;


void setup()
{
	set_output(DDRD, DIGIT_1_PIN);
	set_output(DDRD, DIGIT_2_PIN);
	set_output(DDRD, DIGIT_3_PIN);
	set_output(DDRD, DIGIT_4_PIN);
	
	set_output(DDRD, COLON_PIN);
	
	set_input(DDRD, BUTTON_MINUTES);
	set_input(DDRD, BUTTON_HOURS);
	
	set_output(DDRB, SEGMENT_A_PIN);
	set_output(DDRB, SEGMENT_B_PIN);
	set_output(DDRB, SEGMENT_C_PIN);
	set_output(DDRB, SEGMENT_D_PIN);
	set_output(DDRB, SEGMENT_E_PIN);
	set_output(DDRB, SEGMENT_F_PIN);
	set_output(DDRB, SEGMENT_G_PIN);
	TCCR1B |= (1 << CS12); //set prescaler to 256
}

void showDigit(int8_t digit, int8_t place);
void allDigitsOff();
void allSegmentsOff();
void clearDisplay();
void displayNumber(short number);

int main(void)
{
	setup();
	
	pin_high(PORTD, COLON_PIN);
	while(1)
	{
		if(PIND & _BV(BUTTON_MINUTES))
		{
			if(lastButtonState == 0)
			{
				if(minutes >= 59)
					minutes = 0;
				else
					minutes++;
			}
			lastButtonState = 1;
		}
		else
		{
			lastButtonState = 0;
		}
		
		if(PIND & _BV(BUTTON_HOURS))
		{
			if(lastHourButtonState == 0)
			{
				if(hours >= 12)
					hours = 1;
				else
					hours++;
			}
			lastHourButtonState = 1;
		}
		else
		{
			lastHourButtonState = 0;
		}
		
		clearDisplay();
		if(hours < 10)
		{
			showDigit(hours, 2);
			_delay_us(200);
		}
		else
		{
			showDigit(hours / 10, 1);
			_delay_us(200);
			showDigit(hours % 10, 2);
			_delay_us(200);
		}
		if(minutes < 10)
		{
			showDigit(0, 3);
			_delay_us(200);
			showDigit(minutes, 4);
			_delay_us(200);
		}
		else if(minutes == 0)
		{
			showDigit(0, 3);
			_delay_us(200);
			showDigit(0, 4);
			_delay_us(200);
		}
		else
		{
			showDigit(minutes / 10, 3);
			_delay_us(200);
			showDigit(minutes % 10, 4);
			_delay_us(200);
		}
		clearDisplay();
		_delay_us(1000);
		if(TCNT1 >= 2972) millis++;
		if(TCNT1 >= 29720) //approximately 1 second
		{
			TCNT1 = 0;
			seconds++;
			if(seconds >= 60)
			{
				minutes++;
				seconds = 0;
			}
			
			if(minutes >= 60)
			{
				hours++;
				minutes = 0;
			}
			
			if(hours > 12)
			{
					hours = 1;
			}
		}
	}
}

void showDigit(int8_t digit, int8_t place)
{
	allDigitsOff();
	switch(place)
	{
		case 1:
			pin_high(PORTD, DIGIT_1_PIN);
		break;
		
		case 2:
			pin_high(PORTD, DIGIT_2_PIN);
		break;
		
		case 3:
			pin_high(PORTD, DIGIT_3_PIN);
		break;
		
		case 4:
			pin_high(PORTD, DIGIT_4_PIN);
		break;
		
		default:
		break;
	}
	
	PORTB |= 0b11111110; //Turn all segments off
	
	switch(digit)
	{
		case 0:
			PORTB &= ~((1 << SEGMENT_A_PIN) | 
							(1 << SEGMENT_B_PIN) |
							(1 << SEGMENT_C_PIN) |
							(1 << SEGMENT_D_PIN) |
							(1 << SEGMENT_E_PIN) |
							(1 << SEGMENT_F_PIN));
		break;
		
		case 1:
			PORTB &= ~((1 << SEGMENT_B_PIN) |
							(1 << SEGMENT_C_PIN));
		break;
		
		case 2:
			PORTB &= ~((1 << SEGMENT_A_PIN) |
								(1 << SEGMENT_B_PIN) |
								(1 << SEGMENT_G_PIN) |
								(1 << SEGMENT_E_PIN) |
								(1 << SEGMENT_D_PIN));
		break;
		
		case 3:
			PORTB &= ~((1 << SEGMENT_A_PIN) |
								(1 << SEGMENT_B_PIN) |
								(1 << SEGMENT_C_PIN) |
								(1 << SEGMENT_D_PIN) |
								(1 << SEGMENT_G_PIN));
		break;
		
		case 4:
			PORTB &= ~((1 << SEGMENT_B_PIN) |
								(1 << SEGMENT_C_PIN) |
								(1 << SEGMENT_F_PIN) |
								(1 << SEGMENT_G_PIN));
		break;
		
		case 5: 
			PORTB &= ~((1 << SEGMENT_A_PIN) |
								(1 << SEGMENT_F_PIN) |
								(1 << SEGMENT_G_PIN) | 
								(1 << SEGMENT_C_PIN) |
								(1 << SEGMENT_D_PIN));
		break;
		
		case 6: 
			PORTB &= ~((1 << SEGMENT_A_PIN) |
								(1 << SEGMENT_F_PIN) |
								(1 << SEGMENT_G_PIN) | 
								(1 << SEGMENT_C_PIN) |
								(1 << SEGMENT_D_PIN) |
								(1 << SEGMENT_E_PIN));
		break;
		
		case 7:
			PORTB &= ~((1 << SEGMENT_A_PIN) |
								(1 << SEGMENT_B_PIN) |
								(1 << SEGMENT_C_PIN));
		break;
		
		case 8:
			PORTB &= ~((1 << SEGMENT_A_PIN) | 
							(1 << SEGMENT_B_PIN) |
							(1 << SEGMENT_C_PIN) |
							(1 << SEGMENT_D_PIN) |
							(1 << SEGMENT_E_PIN) |
							(1 << SEGMENT_F_PIN) |
							( 1 << SEGMENT_G_PIN));
		break;
		
		case 9:
			PORTB &= ~((1 << SEGMENT_A_PIN) |
								(1 << SEGMENT_B_PIN) |
								(1 << SEGMENT_G_PIN) |
								(1 << SEGMENT_F_PIN) |
								(1 << SEGMENT_C_PIN));
		break;
	}
	
	
}
void allDigitsOff()
{
	pin_low(PORTD, DIGIT_1_PIN);
	pin_low(PORTD, DIGIT_2_PIN);
	pin_low(PORTD, DIGIT_3_PIN);
	pin_low(PORTD, DIGIT_4_PIN);
}

void allSegmentsOff()
{
	PORTB |= 0b11111110;
}

void clearDisplay()
{
	allDigitsOff();
	allSegmentsOff();
}

#define BRIGHTNESS 750

void displayNumber(short number)
{
  if(number >= 1000)
  {
    showDigit(number % 10, 4);
    number /= 10;
	_delay_us(200);
  }
  if(number >= 100)
  {
    showDigit(number % 10, 3);
    number /= 10;
	_delay_us(200);
  }
  if(number >= 10)
  {
    showDigit(number % 10, 2);
    number /= 10;
    _delay_us(200);
  }
  
  showDigit(number % 10, 1);
  number /= 10;
  _delay_us(200);
  
  clearDisplay();
  
  _delay_us(BRIGHTNESS);
  
}

