

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"
#include "mickey.h"
#include "logo.h"
#include "one.h"
#include "two.h"
#include "three.h"
#include "four.h"
#include "five.h"
#include "six.h"
#include "seven.h"
#include "eight.h"
#include"driverlib/adc.h"


//joustik read data global variables
uint32_t read_data_x ;
uint32_t read_data_y ;
unsigned int i ,j;
unsigned int p=0 ;
unsigned int delay = 4000000;
/*To display image include an array with hex values and index it accordingly*/

/* void glcd_cmd(cmd)
* This function sends commands to the GLCD.
* Value of RS is 0
* Command is written on data lines.
* Enable is made 1 for a short duration.
*/
void glcd_cmd(unsigned char cmd)
{
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,0x00);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,0x00);

    /* RS = 0 */
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6,0x00);

    /* Put command on data lines */
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,cmd);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,cmd);

    /* Generate a high to low pulse on enable */
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0,0x01);
    SysCtlDelay(134/2);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0,0x00);

}




/* void glcd_data(data)
* This function sends data to the GLCD.
* Value of RS is 1
* Data is written on data lines.
* Enable is made 1 for a short duration.
*/
void glcd_data(unsigned char data)
{
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,0x00);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,0x00);

    /* RS = 1 */
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6,0x40);

    /* Put command on data lines */
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,data);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,data);

    /* Generate a high to low pulse on enable */
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0,0x01);
    SysCtlDelay(134/2);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0,0x00);
}



/* void glcd_init()
* This function initializes the GLCD.
* Always call this function at the beginning of main program after configuring the port pins.
*/
void glcd_init()
{
    SysCtlDelay(6700000/50);                            // creates ~10ms delay - TivaWare fxn
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, 0x00);    //cbi(GPORTC_2,GLCD_RST);
    SysCtlDelay(6700000/50);
    /* Set RST */
    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_5,0x20);

    /* Set CS1 (CS1=1 and CS2=0) The right side is selected(column>64) */
    GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3,0x00);

    /* Select the start line */
    glcd_cmd(0xC0);
    //SysCtlDelay(6700);

    /* Send the page */
    glcd_cmd(0xB8);
//  SysCtlDelay(6700);

    /*Send the column */
    glcd_cmd(0x40);
    //SysCtlDelay(6700);

    /* Send glcd on command */
    glcd_cmd(0x3F);


    /* Initialize the right side of GLCD */
    /* Set CS2 (CS2=1 and CS1=0) The right side is selected(column>64) */
    GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3,0x08);

    /* Select the start line */
    glcd_cmd(0xC0);
    SysCtlDelay(6700);

    /* Send the page */
    glcd_cmd(0xB8);
//  SysCtlDelay(6700);

    /*Send the column */
    glcd_cmd(0x40);
//  SysCtlDelay(6700);

    /* Send glcd on command */
    glcd_cmd(0x3F);
//  SysCtlDelay(6700);
}




/* void glcd_setpage(page)
* This function selects page number on GLCD.
* Depending on the value of column number CS1 or CS2 is made high.
*/
void glcd_setpage(unsigned char page)
{
    /* Set CS1 (CS1=1 and CS2=0) The right side is selected(column>64) */
    GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3,0x00);
    glcd_cmd(0xB8 | page);
    SysCtlDelay(100/2);

    /* Set CS2 (CS2=1 and CS1=0) The right side is selected(column>64) */
    GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3,0x08);
    glcd_cmd(0xB8 | page);
    SysCtlDelay(100/2);
}



/* void glcd_setcolumn(column)
* This function selects column number on GLCD.
* Depending on the value of column number CS1 or CS2 is made high.
*/
void glcd_setcolumn(unsigned char column)
{
    if(column < 64)
    {
        /* Set CS1 (CS1=1 and CS2=0) The right side is selected(column>64) */
        GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3,0x00);
        glcd_cmd(0x40 | column);
        SysCtlDelay(670/2);
    }
    else
    {
        /* Set CS2 (CS2=1 and CS1=0) The right side is selected(column>64) */
        GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3,0x08);
        glcd_cmd(0x40 | (column-64));
        SysCtlDelay(670/2);
    }

}



/* void glcd_cleardisplay()
* This function clears the data on GLCD by writing 0 on all pixels.
*/
void glcd_cleardisplay()
{
    unsigned char i,j;
    for(i=0;i<8;i++)
    {
        glcd_setpage(i);
        for(j=0;j<128;j++)
        {
            glcd_setcolumn(j);
            glcd_data(0x00);
        }
    }
}

void print_image_one(){

	p=0;
	j = 0 ;
	while(p<8){
		glcd_setpage(p);

		for(i=0;i<128;i++){
			glcd_setcolumn(i);
			glcd_data(one[j]);
			j++;
		}
	p++;
	}
}

void print_image_two(){

	p=0;
	j = 0 ;
	while(p<8){
		glcd_setpage(p);

		for(i=0;i<128;i++){
			glcd_setcolumn(i);
			glcd_data(two[j]);
			j++;
		}
	p++;
	}
}

void print_image_three(){

	p=0;
	j = 0 ;
	while(p<8){
		glcd_setpage(p);

		for(i=0;i<128;i++){
			glcd_setcolumn(i);
			glcd_data(three[j]);
			j++;
		}
	p++;
	}
}

void print_image_four(){

	p=0;
	j = 0 ;
	while(p<8){
		glcd_setpage(p);

		for(i=0;i<128;i++){
			glcd_setcolumn(i);
			glcd_data(four[j]);
			j++;
		}
	p++;
	}
}

void print_image_five(){

	p=0;
	j = 0 ;
	while(p<8){
		glcd_setpage(p);

		for(i=0;i<128;i++){
			glcd_setcolumn(i);
			glcd_data(five[j]);
			j++;
		}
	p++;
	}
}

void print_image_six(){

	p=0;
	j = 0 ;
	while(p<8){
		glcd_setpage(p);

		for(i=0;i<128;i++){
			glcd_setcolumn(i);
			glcd_data(six[j]);
			j++;
		}
	p++;
	}
}

void print_image_seven(){

	p=0;
	j = 0 ;
	while(p<8){
		glcd_setpage(p);

		for(i=0;i<128;i++){
			glcd_setcolumn(i);
			glcd_data(seven[j]);
			j++;
		}
	p++;
	}
}

void print_image_eight(){

	p=0;
	j = 0 ;
	while(p<8){
		glcd_setpage(p);

		for(i=0;i<128;i++){
			glcd_setcolumn(i);
			glcd_data(eight[j]);
			j++;
		}
	p++;
	}
}

void init(){
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);


	    /* Enable all the peripherals */
	    /* PORTS A,E,F,C,D,B */

	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);


	    /* Unlock pin PF0 */
	    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK)= GPIO_LOCK_KEY;    // unlocking sw2 switch
	    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK)= 0;

	    /* Configure Enable pin as output */
	   GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);


	    /*This ensures buzzer remains OFF, since PC4 when logic 0 turns ON buzzer */
	    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4);
	    GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_4,16);

	    /* Configure PE5 (RST), PE0 to PE3 (D0 to D3) and PB4 to PB7(D4 to D7) as output pins */
	   GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_5); //reset pin
	   GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |GPIO_PIN_4); //D0 to D3
	   GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7); //D4 to D7


	        /* Configure RS as output */
	    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_6); // RS pin as output

	        /* Configure CS1 or CS2 as output */
	    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_3); // CS as output

	        /*initialize glcd*/

	    //ADC configurations
	        SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	        GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_4);
	        GPIOPinTypeADC(GPIO_PORTD_BASE,GPIO_PIN_0|GPIO_PIN_1); //ADC input pins
	        ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	        ADCSequenceStepConfigure(ADC0_BASE, 1, 0,ADC_CTL_CH7 ); // chabne for PD0 X axis
	        ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH6); // channel for PD1 Y axis
	        ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
	        ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
	        ADCSequenceEnable(ADC0_BASE, 1);

}


int main()
{
	uint32_t ui32ADC0Value[4];
	init();
        /*initialize glcd*/
    glcd_init(); 
   

glcd_cleardisplay();
while(1){
	ADCIntClear(ADC0_BASE, 1);
			 ADCProcessorTrigger(ADC0_BASE, 1);
		//wait for conversion to complete
			 while(!ADCIntStatus(ADC0_BASE, 1, false))
			    {
				 ;
			    }
			 SysCtlDelay(400000);
			 ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);

		  read_data_x = ui32ADC0Value[1] ;
		  read_data_y = ui32ADC0Value[0] ;
	if(read_data_x > 100){
		delay*=2;

	}else if(read_data_x < 20){
		delay/=2 ;

	}


print_image_one();
SysCtlDelay(delay);
glcd_cleardisplay();

print_image_two();
SysCtlDelay(delay);

glcd_cleardisplay();

print_image_three();
SysCtlDelay(delay);

glcd_cleardisplay();

print_image_four();
SysCtlDelay(delay);

glcd_cleardisplay();

print_image_five();
SysCtlDelay(delay);

glcd_cleardisplay();

print_image_six();
SysCtlDelay(delay);

glcd_cleardisplay();

print_image_seven();
SysCtlDelay(delay);

glcd_cleardisplay();

print_image_eight();
SysCtlDelay(delay);

glcd_cleardisplay();

}       /* Select a page and display lines on it from column 0 to 127 */



}

