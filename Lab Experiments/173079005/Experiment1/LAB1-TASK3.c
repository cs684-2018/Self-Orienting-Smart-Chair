

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

//to unlock PORTF 0th pin which was initially locled
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F (*((volatile unsigned long *)0x40025524))

//macro for leds for easy reference
#define red_led GPIO_PIN_1
#define blue_led GPIO_PIN_2
#define green_led GPIO_PIN_3

//macro for switches for easy reference
#define switch1 GPIO_PIN_4
#define switch2 GPIO_PIN_0

//macro for switch press
#define switch1_is_press (read_data & 0x00000010)==0
#define switch2_is_press (read_data & 0x00000001)==0

//state variable for colour and delay (model as FSM with 3 state)
unsigned int counter_sw1 = 0 ;
unsigned int counter_sw2 = 0 ;

//port values variable  to make LEDs on or off after masking
uint8_t turn_on=0b00001110 ;
uint8_t turn_off=0b00000000 ;

//function to set colour of blinking LED
//here counter_sw2 represents state of FSM states 1,2 and 3 corresponding ro R G B colours
void place_colour(){
	switch(counter_sw2){
	case 1 :
		      GPIOPinWrite(GPIO_PORTF_BASE,red_led,turn_on);
	          GPIOPinWrite(GPIO_PORTF_BASE,green_led,turn_off);
	          GPIOPinWrite(GPIO_PORTF_BASE,blue_led,turn_off);
              break;
	case 2 :
		      GPIOPinWrite(GPIO_PORTF_BASE,red_led,turn_off);
              GPIOPinWrite(GPIO_PORTF_BASE,green_led,turn_on);
              GPIOPinWrite(GPIO_PORTF_BASE,blue_led,turn_off);
              break;
	case 3 :
		      GPIOPinWrite(GPIO_PORTF_BASE,red_led,turn_off);
              GPIOPinWrite(GPIO_PORTF_BASE,green_led,turn_off);
              GPIOPinWrite(GPIO_PORTF_BASE,blue_led,turn_on);
              break;


	}

}

//Delay function
//delay changes between three states 1 2 and 3 in FSM reffered by counter_sw1 global variable
void delay(){
	switch(counter_sw1){
		case 1 : SysCtlDelay(670000);

		case 2 : SysCtlDelay(900000);

		case 3 : SysCtlDelay(150000);

		}


}

//Function to Make all LEDs off
void lit_off(){
	GPIOPinWrite(GPIO_PORTF_BASE,red_led,turn_off);
	GPIOPinWrite(GPIO_PORTF_BASE,green_led,turn_off);
	GPIOPinWrite(GPIO_PORTF_BASE,blue_led,turn_off);

}

//Main function starts here
int main(void)
{
//setting system clock and enable peripherals
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

//to unlock pin 4 and 0 to use it as a switch
	LOCK_F=0x4C4F434BU;
    CR_F=GPIO_PIN_0|GPIO_PIN_4;

//pins input and output configurations
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); //make portF pins 1 2 3 as output
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4); //pin4 sw2 pin0 sw1 as input pins (switches)
    GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU); //enable pull down for swichws at pin 0 and 4

//internal variables declarations

 unsigned int read_data = 0;
//main while loop
	while(1)
	{
    read_data=GPIOPinRead(GPIO_PORTF_BASE,switch2|switch1) ; //read switch2 and switch1
    SysCtlDelay(20000); //default delay

    //read_delay countersw1 1=0.5s 2=1s 3=2s
    // this if construct is to modify state for delay through 1 2 and 3
    //Here I have taken care of key debouncing using software construct
    if(switch1_is_press){
    	SysCtlDelay(20000); //wait for debounce
    	counter_sw1 = counter_sw1 + 1 ;
    	   if(counter_sw1==4){
    	    counter_sw1=1;
    	                     }
    	while(1){
    		read_data=GPIOPinRead(GPIO_PORTF_BASE,switch2|switch1) ; //read switch2 and switch1
    		if((read_data & 0x00000010)!=0){
    			break;
    		}
    	}

                        }

    //read colour countersw2 : 1 = red 2=green 3=blue
    // this if construct is to modify state for colour through 1 2 and 3
    //Here I have taken care of key debouncing using software construct again
    if(switch2_is_press){
        	SysCtlDelay(20000); //wait for debounce
        	counter_sw2 = counter_sw2 + 1 ;
        	        if(counter_sw2==4){
        	       counter_sw2=1;
        	                         }
        	while(1){
        		read_data=GPIOPinRead(GPIO_PORTF_BASE,switch2|switch1) ; //read switch2 and switch1
        		if((read_data & 0x00000001)!=0){
        			break;
        		}
        	}

                            }

//turn  LED on of colour depending on State specified by counter_sw2 global variable
place_colour();

//delay depending on state spevified by counter_sw1 global variable
delay();

//Make all LEDs off
lit_off();

delay();

//IN this code at the end of each cycle of LED blink , we pool for any switch press to modify state (i.e colour and delay )
//and blink next cycle using this coloured LED and delay
//Code can be modified by usng intterupts for pooling. Most of the time controller is busy in this case due to software delay and button pooling
	}
}
