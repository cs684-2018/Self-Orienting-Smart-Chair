

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include <stdint.h>
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"
#include"driverlib/adc.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include <string.h>

//to unlock PORTF 0th pin which was initially locled
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F (*((volatile unsigned long *)0x40025524))

//macro for leds for easy reference
#define red_led PWM_OUT_5
#define blue_led PWM_OUT_6
#define green_led PWM_OUT_7

//macro for switches for easy reference
#define switch1 GPIO_PIN_4
#define switch2 GPIO_PIN_0

//macro for switch press
#define switch1_is_press (read_data & 0x00000010)==0
#define switch2_is_press (read_data & 0x00000001)==0

//state variable for colour and delay (model as FSM with 3 state)
unsigned int counter_sw1 = 0 ;
unsigned int counter_sw2 = 0 ;
unsigned int state_sw1 = 1 ;
unsigned int state_sw2 = 1 ;

unsigned int flag_sw1 = 0 ;
unsigned int flag_sw2 = 0 ;
double delay = 0.05 ; //Delay in seconds
unsigned int delay_normal = 400000 ;

unsigned int level = 1 ;
unsigned int location = 34 ;
unsigned int mode = 0 ;
unsigned int global_counter = 0 ;
//port values variable  to make LEDs on or off after masking
uint8_t turn_on=0b00001110 ;
uint8_t turn_off=0b00000000 ;


uint32_t read_data_x ;
uint32_t read_data_y ;





//Main function starts here
int main(void)
{

//setting system clock and enable peripherals
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    uint32_t ui32ADC0Value[4];
    volatile uint32_t ui32TempAvg;

    //ADC configurations
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); // UART enable
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));


    UARTCharPut(UART0_BASE, 'E');

    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4);
    GPIOPinTypeADC(GPIO_PORTD_BASE,GPIO_PIN_0|GPIO_PIN_1); //ADC input pins


    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);

    ADCSequenceStepConfigure(ADC0_BASE, 1, 0,ADC_CTL_CH7 ); // chabne for PD0 X axis
    ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH6); // channel for PD1 Y axis
    ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 1);


    //make buzzer off
    GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_4,0b00010000);


 while(1){
	 ADCIntClear(ADC0_BASE, 1);
	 ADCProcessorTrigger(ADC0_BASE, 1);
//wait for conversion to complete
	// while(!ADCIntStatus(ADC0_BASE, 1, false))
	//    {
	//	 ;
	//    }
	 SysCtlDelay(400000);
	 ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);

  read_data_x = ui32ADC0Value[1] ;
  read_data_y = ui32ADC0Value[0] ;

 if(read_data_x < 20 ){
	 UARTCharPut(UART0_BASE, 'R');
 }
 else if(read_data_x > 4000 ){
	 UARTCharPut(UART0_BASE, 'L');
 }
 else if(read_data_y > 4000 ){
	 UARTCharPut(UART0_BASE, 'U');
 }
 else if(read_data_y < 20 ){
	 UARTCharPut(UART0_BASE, 'D');
 }else {
	 UARTCharPut(UART0_BASE, 'C');
 }



// Y axis values -> ui32ADC0Value[0]
// X axis values -> ui32ADC0Value[1]





 SysCtlDelay(4000000);

   }//end of main while loop
}//end of main function








