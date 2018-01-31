#include <mega328p.h>
#include <alcd.h>
#include <delay.h>
#include <stdio.h>

#define BUF_SIZE 880
#define CORR_ARR_SIZE 110

// signatures
void read_adc(void);
void auto_corr(void);
void show_note(void);

// our initialization 
unsigned int pitch_freq;
unsigned char adc_value;

//main buffer
unsigned char buffer[BUF_SIZE];
unsigned int buffer_count = 0;

// Timer1 overflow interrupt service routine
interrupt [TIM1_OVF] void timer1_ovf_isr(void)
{
// Reinitialize Timer1 value
TCNT1H=0xF910 >> 8;
TCNT1L=0xF910 & 0xff;
// Place your code here
    if(buffer_count <= BUF_SIZE){
            read_adc();
            buffer[buffer_count] = adc_value;
            buffer_count++;
    }
}

// Voltage Reference: AVCC pin
#define ADC_VREF_TYPE ((0<<REFS1) | (1<<REFS0) | (1<<ADLAR))

// Read the 8 most significant bits
// of the AD conversion result
void read_adc(void)
{
// Delay needed for the stabilization of the ADC input voltage
delay_us(10);
// Wait for the AD conversion to complete
while ((ADCSRA & (1<<ADIF))==0);
ADCSRA|=(1<<ADIF);
adc_value = ADCH;
}

void main(void)
{
// Declare your local variables here

// Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=(1<<CLKPCE);
CLKPR=(0<<CLKPCE) | (0<<CLKPS3) | (0<<CLKPS2) | (0<<CLKPS1) | (0<<CLKPS0);
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+               
#endif

// Input/Output Ports initialization
// Port B initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRB=(0<<DDB7) | (0<<DDB6) | (1<<DDB5) | (1<<DDB4) | (1<<DDB3) | (1<<DDB2) | (1<<DDB1) | (1<<DDB0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

// Port C initialization
// Function: Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRC=(0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
// State: Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTC=(0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

// Port D initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRD=(1<<DDD7) | (1<<DDD6) | (1<<DDD5) | (1<<DDD4) | (1<<DDD3) | (1<<DDD2) | (1<<DDD1) | (1<<DDD0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 16000.000 kHz
// Mode: Normal top=0xFFFF
// OC1A output: Disconnected
// OC1B output: Disconnected
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer Period: 1 ms
// Timer1 Overflow Interrupt: On
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (1<<CS10);
TCNT1H=0xF9;
TCNT1L=0x10;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer/Counter 1 Interrupt(s) initialization
TIMSK1=(0<<ICIE1) | (0<<OCIE1B) | (0<<OCIE1A) | (1<<TOIE1);

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// Interrupt on any change on pins PCINT0-7: Off
// Interrupt on any change on pins PCINT8-14: Off
// Interrupt on any change on pins PCINT16-23: Off
EICRA=(0<<ISC11) | (0<<ISC10) | (0<<ISC01) | (0<<ISC00);
EIMSK=(0<<INT1) | (0<<INT0);
PCICR=(0<<PCIE2) | (0<<PCIE1) | (0<<PCIE0);

// ADC initialization
// ADC Clock frequency: 125.000 kHz
// ADC Voltage Reference: AVCC pin
// ADC Auto Trigger Source: Timer1 Overflow
// Only the 8 most significant bits of
// the AD conversion result are used
// Digital input buffers on ADC0: On, ADC1: On, ADC2: On, ADC3: On
// ADC4: On, ADC5: On
DIDR0=(0<<ADC5D) | (0<<ADC4D) | (0<<ADC3D) | (0<<ADC2D) | (0<<ADC1D) | (0<<ADC0D);
ADMUX= 0x00 | ADC_VREF_TYPE;
ADCSRA=(1<<ADEN) | (0<<ADSC) | (1<<ADATE) | (0<<ADIF) | (0<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
ADCSRB=(1<<ADTS2) | (1<<ADTS1) | (0<<ADTS0);

// Global enable interrupts
#asm("sei")

buffer_count = 0;

// Alphanumeric LCD initialization
// RS - PORTD Bit 0
// RD - PORTD Bit 1
// EN - PORTD Bit 2
// D4 - PORTD Bit 4
// D5 - PORTD Bit 5
// D6 - PORTD Bit 6
// D7 - PORTD Bit 7
// Characters/line: 16
lcd_init(16);
lcd_puts("Hello");
while (1)
      {
            //calling autocorrelation function after #BUF_SIZE times
      		//and showing the main note and freq
            if (buffer_count > BUF_SIZE){
                auto_corr();
                buffer_count = 0;
                show_note();
            } 
            
            
      }
}


void auto_corr(void){

    unsigned long sum[CORR_ARR_SIZE];
    unsigned long corr_arr[CORR_ARR_SIZE];
    unsigned long sum_t;
    unsigned long max1; 
    unsigned long max2;
    unsigned char max_index1;
    unsigned char max_index2;
    unsigned char i;
    unsigned int j;
    unsigned char pitch_dist;

    max_index1 = 0; 
    max_index2 = 0;
    max1 = 0;
    max2 = 0;                      
        
    for (i = 0; i < CORR_ARR_SIZE; i++){
		sum_t = 0;

		for (j = 0; j < BUF_SIZE - i; j++){
			sum_t += (unsigned int)(buffer[j]) * (unsigned int)(buffer[j+i]);
		}

		sum[i] = sum_t;
	}

	/*********************	smothing method		*********************/
    for(i = 4; i < CORR_ARR_SIZE - 4; i++){
        corr_arr[i] = (sum[i-4] + sum[i-3] + sum[i-2] + sum[i-1] + sum[i] 
        + sum[i+1] + sum[i+2] + sum[i+3] + sum[i+4]) / 9; 
    }
    corr_arr[0] = (sum[0] + sum[1] + sum[2] + sum[3] + sum[4])/5;
    corr_arr[1] = (sum[0] + sum[1] + sum[2] + sum[3] + sum[4] + sum[5])/6;
    corr_arr[2] = (sum[0] + sum[1] + sum[2] + sum[3] + sum[4] + sum[5] + sum[6])/7;
    corr_arr[3] = (sum[0] + sum[1] + sum[2] + sum[3] + sum[4] + sum[5] + sum[6] + sum[7])/8;
    corr_arr[CORR_ARR_SIZE - 1] = (sum[CORR_ARR_SIZE - 1] +sum[CORR_ARR_SIZE - 2]
                                    +sum[CORR_ARR_SIZE - 3] + sum[CORR_ARR_SIZE - 4]+ sum[CORR_ARR_SIZE - 5])/5;
    corr_arr[CORR_ARR_SIZE - 2] = (sum[CORR_ARR_SIZE - 1] +sum[CORR_ARR_SIZE - 2]
                                    +sum[CORR_ARR_SIZE - 3] + sum[CORR_ARR_SIZE - 4]+ sum[CORR_ARR_SIZE - 5]
                                    + sum[CORR_ARR_SIZE - 6])/6;
    corr_arr[CORR_ARR_SIZE - 3] = (sum[CORR_ARR_SIZE - 1] +sum[CORR_ARR_SIZE - 2]
                                    +sum[CORR_ARR_SIZE - 3] + sum[CORR_ARR_SIZE - 4]+ sum[CORR_ARR_SIZE - 5]
                                    + sum[CORR_ARR_SIZE - 6]+ sum[CORR_ARR_SIZE - 7])/7;
    corr_arr[CORR_ARR_SIZE - 4] = (sum[CORR_ARR_SIZE - 1] +sum[CORR_ARR_SIZE - 2]
                                    +sum[CORR_ARR_SIZE - 3] + sum[CORR_ARR_SIZE - 4]+ sum[CORR_ARR_SIZE - 5]
                                    + sum[CORR_ARR_SIZE - 6]+ sum[CORR_ARR_SIZE - 7]+ sum[CORR_ARR_SIZE - 8])/8;
    
    /************************************************************/

    /*************	find the maximums		************/
    for(i = 0; i < CORR_ARR_SIZE; i++){
        
        if (sum[i] > max1){
            max2 = max1;
            max_index2 = max_index1;
            max1 = sum[i];
            max_index1 = i;
            
            while(sum[i+1] < sum[i] && i < CORR_ARR_SIZE){
                i++;
            }
        }
        else if (sum[i] > max2){
            max2 = sum[i];
            max_index2 = i;
            
            while(sum[i+1] < sum[i] && i < CORR_ARR_SIZE){
                i++;
            }           
        }
    }

    /*************************************************/
    
    if (max_index1 > max_index2)
        pitch_dist = max_index1 - max_index2;
    else
        pitch_dist = max_index2 - max_index1;
        
    pitch_freq = 9009 / pitch_dist;  
}

//notes between freq 82 to 369
//you can add more notes
void show_note(){
    char freq[3];
    sprintf(freq,"%u",pitch_freq);
    if(pitch_freq < 87 && pitch_freq >= 82){
        lcd_clear();
        lcd_puts("E2");
        lcd_gotoxy(0,1);
        lcd_puts(freq);
    }
    else if(pitch_freq < 92 && pitch_freq >= 87){
        lcd_clear();
        lcd_puts("F2");
        lcd_gotoxy(0,1);
        lcd_puts(freq);
    }
    else if(pitch_freq < 98 && pitch_freq >= 92){
        lcd_clear();
        lcd_puts("F#2/Gb2");
        lcd_gotoxy(0,1);
        lcd_puts(freq);
    }
    else if(pitch_freq < 103 && pitch_freq >= 98){
        lcd_clear();
        lcd_puts("G2");
        lcd_gotoxy(0,1);
        lcd_puts(freq);
    }
    else if(pitch_freq < 110 && pitch_freq >= 103){
        lcd_clear();        
        lcd_puts("G#2/Ab2");
        lcd_gotoxy(0,1);
        lcd_puts(freq);
    }
    else if(pitch_freq < 116 && pitch_freq >= 110){
        lcd_clear();
        lcd_puts("A2");
        lcd_gotoxy(0,1);
        lcd_puts(freq);
    }
    else if(pitch_freq < 123 && pitch_freq >= 116){
        lcd_clear();
        lcd_puts("A#2/Bb2");
        lcd_gotoxy(0,1);
        lcd_puts(freq);
    }
    else if(pitch_freq < 130 && pitch_freq >= 123){
        lcd_clear();
        lcd_puts("B2");
        lcd_gotoxy(0,1);
        lcd_puts(freq);
    }
    else if(pitch_freq < 138 && pitch_freq >= 130){
        lcd_clear();
        lcd_puts("C3");
        lcd_gotoxy(0,1);
        lcd_puts(freq);
    }
    else if(pitch_freq < 146 && pitch_freq >= 138){
        lcd_clear();
        lcd_puts("C#3/Db3");
        lcd_gotoxy(0,1);
        lcd_puts(freq);
        
    }
    else if(pitch_freq < 155 && pitch_freq >= 146){
        lcd_clear();
        lcd_puts("D3");
        lcd_gotoxy(0,1);
        lcd_puts(freq);
        
    }
    else if(pitch_freq < 164 && pitch_freq >= 155){
        lcd_clear();
        lcd_puts("D#3/Eb3");
        lcd_gotoxy(0,1);
        lcd_puts(freq);
        
        
    }
    else if(pitch_freq < 174 && pitch_freq >= 164){
        lcd_clear();
        lcd_puts("E3");
        lcd_gotoxy(0,1);
        lcd_puts(freq);
        
    }
    else if(pitch_freq < 185 && pitch_freq >= 174){
        lcd_clear();
        lcd_puts("F3");
        lcd_gotoxy(0,1);
        lcd_puts(freq);
        
    }
    else if(pitch_freq < 196 && pitch_freq >= 185){
        lcd_clear();
        lcd_puts("F#3/Gb3");
        lcd_gotoxy(0,1);
        lcd_puts(freq);
        
    }
    else if(pitch_freq < 207 && pitch_freq >= 196){
        lcd_clear();
        lcd_puts("G3");
        lcd_gotoxy(0,1);
        lcd_puts(freq);
        
    }
    else if(pitch_freq < 220 && pitch_freq >= 207){
        lcd_clear();
        lcd_puts("G#3/Ab3");
        lcd_gotoxy(0,1);
        lcd_puts(freq);
        
    }
    else if(pitch_freq < 233 && pitch_freq >= 220){
        lcd_clear();
        lcd_puts("A3");
        lcd_gotoxy(0,1);
        lcd_puts(freq);
        
    }
    else if(pitch_freq < 246 && pitch_freq >= 233){
        lcd_clear();
        lcd_puts("A#3/Bb3");
        lcd_gotoxy(0,1);
        lcd_puts(freq);
        
    }
    else if(pitch_freq < 261 && pitch_freq >= 246){
        lcd_clear();
        lcd_puts("B3");
        lcd_gotoxy(0,1);
        lcd_puts(freq);
        
    }
    else if(pitch_freq < 277 && pitch_freq >= 261){
        lcd_clear();
        lcd_puts("C4");
        lcd_gotoxy(0,1);
        lcd_puts(freq);
        
    }
    else if(pitch_freq < 311 && pitch_freq >= 293){
        lcd_clear();
        lcd_puts("C#4/Db4");
        lcd_gotoxy(0,1);
        lcd_puts(freq);
        
    }
    else if(pitch_freq < 329 && pitch_freq >= 311){
        lcd_clear();
        lcd_puts("D4");
        lcd_gotoxy(0,1);
        lcd_puts(freq);
        
    }
    else if(pitch_freq < 349 && pitch_freq >= 329){
        lcd_clear();
        lcd_puts("D#4/Eb4");
        lcd_gotoxy(0,1);
        lcd_puts(freq);
        
    }
    else if(pitch_freq < 369 && pitch_freq >= 349){
        lcd_clear();
        lcd_puts("E4");
        lcd_gotoxy(0,1);
        lcd_puts(freq);
        
    }
}