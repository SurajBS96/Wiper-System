#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>
#define input 1
#define low 0
#define one_sec 10
unsigned int ignition=0,wiper=0,count=0;
double dutyCycle1=30,dutyCycle2=60,dutyCycle3=90;
//Function to initialize Port Pins
void initializePortPins()
{ DDRD|=(input<<PD1);//Output Pin for Motor(wiper).
  PORTD&=~(input<<PD1);//Writing 0 as initial value on PortD Pin1
  DDRD&=~(input<<PD2);//Port D's Pin 1 defined as Input Pin.
  PORTD|=(input<<PD2);//Enabling Pull Up Register.
  DDRD&=~(input<<PD3);//Port D's Pin 1 defined as Input Pin.
  PORTD|=(input<<PORTD3);
}

//Initialize Timer 2 for 1 sec
void timerInitialize() 
{ TCNT2 = low;
  TCCR2B |= ((input<<CS22) | (input<<CS21)|(input<<CS20)); // PRESCALER 1024
  TIMSK2 = (input<<TOIE2);
}
//Initialize Timer 0 to generate PWM
void genDutyCycle(double a)
{ OCR0A=255;
  OCR0B=(a/100.0)*255.0;
  TCCR0A|=(input<<WGM01);//SET CTC
  TCCR0B|=(input<<CS02)|(input<<CS00);//Set clocksource PreScaler 1024
  TIMSK0|=((input<<OCIE0A)|(input<<OCIE0B));
}
//Initialize External Interrupt for Ignition & Wiper switch
void setupExternalInterrupt()
{ EICRA|=(low<<ISC01)|(input<<ISC00);//any logical change
  EICRA |= ((low<< ISC11) | (input << ISC10) ); //any logical change
  EIMSK|=((input<<INT1)|(input<<INT0));
}
//Intialize ADC to read Potentiometer Values
void setupADC()	
{
  ADMUX|=((input<<REFS0)|(input<<MUX2));//A4
  ADCSRA|=(input<<ADEN);//Enable ADC
}

int pot_read()	//func for returning ADC value
{
  ADCSRA|=(input<<ADSC);
  while(ADSC==input)
  {
  }
  return(ADC);
}


int main(void)
{
  initializePortPins();
  sei();//Global Interrupt part enabling
  setupExternalInterrupt();
  setupADC();
  int pot=low;
  while(1)
  {
    if(ignition==input)
    {
    	if(wiper==input)
    	{
            if(count>=one_sec)
        	 { pot=pot_read();
              count=0;
         	 	if(pot>0&&pot<=256)
      			{genDutyCycle(dutyCycle1);
       			 
     			 }
      
     		 	else if(pot>256&&pot<=512)
       			{genDutyCycle(dutyCycle2);
           		
       			}
          
            	else if(pot>512&&pot<=1023)
       			{genDutyCycle(dutyCycle3); 		
       			}
            } 
        }
          else 
         {
          PORTD&=~(input<<PD1);
         }
    } 
  }//while ends
}//main ends

//Interrupt Service Routine for External Interrupt
ISR(INT0_vect)
{ ignition=!ignition;
  _delay_ms(200);
}

ISR(INT1_vect)
{ wiper=!wiper;
  timerInitialize();
  _delay_ms(200);
}

ISR(TIMER0_COMPA_vect)
{ PORTD|=(1<<PD1);
}

ISR(TIMER0_COMPB_vect)
{PORTD&=~(1<<PD1);
}
ISR(TIMER2_OVF_vect)
{  count++;
}