//Authors: Pardeep Bhattal
//Date: 12/07/2022

///////////////////////////////////////////////////
//																							 //
//  Servo DC Motor Speed Control Using 8051 MCU  //
//																							 //
///////////////////////////////////////////////////

#include <REG52.h>

#define max_freq   255; // Highest possible PWM Frequency, lowest is 1


///////////////////////
//  Initializations  //
///////////////////////


//Dip switch inputs
sbit DIP1 = P0^2; 
sbit DIP2 = P0^1;
sbit DIP3 = P0^0; 

//External in for motor rev
sbit motorEncoder = P3^2;

//PWM output connected to DC motor of fan
sbit PWMoutput = P3^4;

//Create 4 output bits for each display using individual GPIO pins
sbit BCD_port1A = P1^0; 
sbit BCD_port1B = P1^1; 
sbit BCD_port1C = P1^2; 
sbit BCD_port1D = P1^3; 
sbit BCD_port2A = P2^0; 
sbit BCD_port2B = P2^1; 
sbit BCD_port2C = P2^2; 
sbit BCD_port2D = P2^3; 
sbit BCD_port3A = P3^0; 
sbit BCD_port3B = P3^1; 
sbit BCD_port3C = P3^5; 
sbit BCD_port3D = P3^3; 

//Global variables
unsigned char switch_input_value = 0;	//current value of DIP switch
unsigned char edgeCount;	//Falling edge count variable
unsigned char timerCount; //Counter timer
unsigned short RPM; //Average value of RPM1, RPM2, RPM3
unsigned short RPM1;
unsigned short RPM2;
unsigned short RPM3;
float DutyCycle; //Duty cycle is a percentage of 255
unsigned short sub; //added sub to duty cycle and subtracted from RPM
unsigned char PWM = 0;   // It can have a value from 0 (0% duty cycle) to 255 (100% duty cycle)
unsigned int temp = 0;    // Used inside Timer0 ISR to simulate shifted value


////////////////////////////////
//  INITIALIZATION FUNCTIONS  //
////////////////////////////////


//Timer0 initialization
void initialize_timer()
{
	TMOD &= 0xF0;    // Clear 4bit field for timer0
	TMOD |= 0x01;    // Set timer0 in mode 1 = 16bit mode
	TH0 = 0x00;      // First time value
	TL0 = 0x00;      // Set arbitrarily zero
	ET0 = 1;         // Enable Timer0 interrupts
	EA  = 1;         // Global nterrupt enable
	TR0 = 1;         // Start Timer 0
}
//PWM initialization
void initialize_PWM()
{
	PWM = 0;          		 // Initialize with 0% duty cycle
	initialize_timer();    // Initialize timer0 to start generating interrupts
}

//Hard Coding input switch value to avoid confusion
void DIPval()
{ 
	if (DIP3 == 0 && DIP2 == 0 && DIP1 == 0) {switch_input_value = 0;}    //for binary 000 = 0
	else if (DIP3 == 0 && DIP2 == 0 && DIP1 == 1) {switch_input_value = 1;}    //for binary 001 = 1
	else if (DIP3 == 0 && DIP2 == 1 && DIP1 == 0) {switch_input_value = 2;}    //for binary 010 = 2
	else if (DIP3 == 0 && DIP2 == 1 && DIP1 == 1) {switch_input_value = 3;}    //for binary 011 = 3
	else if (DIP3 == 1 && DIP2 == 0 && DIP1 == 0) {switch_input_value = 4;}    //for binary 100 = 4
	else if (DIP3 == 1 && DIP2 == 0 && DIP1 == 1) {switch_input_value = 5;}    //for binary 101 = 5
	else if (DIP3 == 1 && DIP2 == 1 && DIP1 == 0) {switch_input_value = 6;}    //for binary 110 = 6
	else if (DIP3 == 1 && DIP2 == 1 && DIP1 == 1) {switch_input_value = 7;}    //for binary 111 = 7
}

////////////////////////////
//  ENCODER CALCULATIONS  //
////////////////////////////

//Thousandth value
int output1(int num)
{
	int out = (num/1000)%10;
	return out;
}

//Hundredth value
int output2(int num)
{
	int out = (num/100)%10;
	return out; 
}

//Tenth value
int output3(int num)
{
	int out = (num/10)%10;
	return out; 
}

///////////////////
//  BCD DISPLAY  //
///////////////////

//For each BCD, output 4-line to IC and display
void display(double BCD, int input)
{
	if(BCD == 1)
	{
		switch(input) 
		{
			case 0:
				BCD_port1A = 0; 
				BCD_port1B = 0; 
				BCD_port1C = 0; 
				BCD_port1D = 0; 
			break;
				case 1: 
				BCD_port1A = 1; 
				BCD_port1B = 0; 
				BCD_port1C = 0; 
				BCD_port1D = 0; 
				break;
			case 2: 
				BCD_port1A = 0; 
				BCD_port1B = 1; 
				BCD_port1C = 0; 
				BCD_port1D = 0; 
				break;
			case 3:
				BCD_port1A = 1; 
				BCD_port1B = 1; 
				BCD_port1C = 0; 
				BCD_port1D = 0; 
				break;
			case 4: 
				BCD_port1A = 0; 
				BCD_port1B = 0; 
				BCD_port1C = 1; 
				BCD_port1D = 0; 
				break;
			case 5: 
				BCD_port1A = 1; 
				BCD_port1B = 0; 
				BCD_port1C = 1; 
				BCD_port1D = 0; 
				break;
			case 6: 
				BCD_port1A = 0; 
				BCD_port1B = 1; 
				BCD_port1C = 1; 
				BCD_port1D = 0; 
				break;
			case 7: 
				BCD_port1A = 1; 
				BCD_port1B = 1; 
				BCD_port1C = 1; 
				BCD_port1D = 0; 
				break;
			case 8:
				BCD_port1A = 0; 
				BCD_port1B = 0; 
				BCD_port1C = 0; 
				BCD_port1D = 1; 
				break;
			case 9:
				BCD_port1A = 1; 
				BCD_port1B = 0; 
				BCD_port1C = 0; 
				BCD_port1D = 1; 
				break;
			default: 
			break;
		}
	}
	else if(BCD == 2)
	{
		switch(input)
		{
			case 0:
				BCD_port2A = 0; 
				BCD_port2B = 0; 
				BCD_port2C = 0; 
				BCD_port2D = 0; 
				break;
			case 1: 
				BCD_port2A = 1; 
				BCD_port2B = 0; 
				BCD_port2C = 0; 
				BCD_port2D = 0; 
				break;
			case 2: 
				BCD_port2A = 0; 
				BCD_port2B = 1; 
				BCD_port2C = 0; 
				BCD_port2D = 0; 
				break;
			case 3:
				BCD_port2A = 1; 
				BCD_port2B = 1; 
				BCD_port2C = 0; 
				BCD_port2D = 0; 
				break;
			case 4: 
				BCD_port2A = 0; 
				BCD_port2B = 0; 
				BCD_port2C = 1; 
				BCD_port2D = 0; 
				break;
			case 5: 
				BCD_port2A = 1; 
				BCD_port2B = 0; 
				BCD_port2C = 1; 
				BCD_port2D = 0; 
				break;
			case 6: 
				BCD_port2A = 0; 
				BCD_port2B = 1; 
				BCD_port2C = 1; 
				BCD_port2D = 0; 
				break;
			case 7: 
				BCD_port2A = 1; 
				BCD_port2B = 1; 
				BCD_port2C = 1; 
				BCD_port2D = 0; 
				break;
			case 8:
				BCD_port2A = 0; 
				BCD_port2B = 0; 
				BCD_port2C = 0; 
				BCD_port2D = 1; 
				break;
			case 9:
				BCD_port2A = 1; 
				BCD_port2B = 0; 
				BCD_port2C = 0; 
				BCD_port2D = 1; 
				break;
			default: 
			break;
		}
	}
	else if(BCD == 3)
	{
		switch(input)
		{
			case 0:
				BCD_port3A = 0; 
				BCD_port3B = 0; 
				BCD_port3C = 0; 
				BCD_port3D = 0; 
				break;
			case 1: 
				BCD_port3A = 1; 
				BCD_port3B = 0; 
				BCD_port3C = 0; 
				BCD_port3D = 0; 
				break;
			case 2: 
				BCD_port3A = 0; 
				BCD_port3B = 1; 
				BCD_port3C = 0; 
				BCD_port3D = 0; 
				break;
			case 3:
				BCD_port3A = 1; 
				BCD_port3B = 1; 
				BCD_port3C = 0; 
				BCD_port3D = 0; 
				break;
			case 4: 
				BCD_port3A = 0; 
				BCD_port3B = 0; 
				BCD_port3C = 1; 
				BCD_port3D = 0; 
				break;
			case 5: 
				BCD_port3A = 1; 
				BCD_port3B = 0; 
				BCD_port3C = 1; 
				BCD_port3D = 0; 
				break;
			case 6: 
				BCD_port3A = 0; 
				BCD_port3B = 1; 
				BCD_port3C = 1; 
				BCD_port3D = 0; 
				break;
			case 7: 
				BCD_port3A = 1; 
				BCD_port3B = 1; 
				BCD_port3C = 1; 
				BCD_port3D = 0; 
				break;
			case 8:
				BCD_port3A = 0; 
				BCD_port3B = 0; 
				BCD_port3C = 0; 
				BCD_port3D = 1; 
				break;
			case 9:
				BCD_port3A = 1; 
				BCD_port3B = 0; 
				BCD_port3C = 0; 
				BCD_port3D = 1; 
				break;
			default: 
			break;
		}
	}
}

/////////////////////////////
//  INTERRUPTS AND TIMERS  //
/////////////////////////////

//Fan external interupt edge counter 
void ExternalInterrupt1() interrupt 0 
{ 
	edgeCount++;
}

//Timer1, interupt - RPM calculation
void timer1() interrupt 3
{
	timerCount++;
	
	//1 sec = 1e6us (1e6/1.085/2^16 = 14 loops/sec = 7 loops/0.5 sec)
	if (timerCount == 7) 
	{ 
		timerCount = 0;
		
			// pulses per 0.5 * 2 = pulses per 1.00 second
			// pulses per 1 second *60 = pulses per 60 seconds = pulses per 1 min
			// PPM = pulses per minute = edgeCount * 2 * 60

			// PPR = pulses per rotation = 2 pulses per rotation
			// RPM = PPM/PPR = edgeCount*2*60(pulse/min) * 1/2 (rotation/pulse) = edgecount * 60 (rotations/minute) 
			
			///////////////////////////////
			//							 						 //
			//  RPM = numOfPeriods * 60  // 
			//							 						 //
			///////////////////////////////

		RPM3 = RPM2;
		RPM2 = RPM1;
		RPM1 = (edgeCount)*60; 
		edgeCount = 0; 

		//take average of past 3 values
		RPM = (RPM1 + RPM2 + RPM3)/3 - sub;
		
		//display values of average RPM
		display(1, output1(RPM)); 
		display(2, output2(RPM));
		display(3, output3(RPM));
	}
	TL1 = 0;	//time 1 low byte counter
	TH1 = 0;	//timer 1 high byte counter
	TF1 = 0;  // Clear the interrupt flag
	TR1 = 1;	// Start Timer 1
}




//Timer0 - DutyCycle/PWM output
void output_PWM() interrupt 1   
{
	TR0 = 0;    // Stop Timer 0
	
	//DIPSWITCH input
	switch_input_value = P0;
	DIPval();
	
	//set duty-cycle percentage from switch input
	switch (switch_input_value) 
	{
		case 0: 
			DutyCycle = 1;  //2400 | 100%
			sub = 280;
			break;
		case 1: 
			DutyCycle = 0.8; //1920 | 80%
			sub = 520;
			break; 
		case 2: 
			DutyCycle = 0.7; //1680 | 70%
			sub = 720;
			break;
		case 3: 
			DutyCycle = 0.5; //1200 | 50%
			sub = 980;
			break;
		case 4: 
			DutyCycle = 0.4; //960 | 40%
			sub = 1020;
			break;
		case 5: 
			DutyCycle = 0.3; //720 | 30%
			sub = 1060;
			break;
		case 6: 
			DutyCycle = 0.1; //240 | 10%
			sub = 680;
			break;
		case 7: 
			DutyCycle = 0; //0 | 0%
			sub = 0;
			break; 
		default: 
			DutyCycle = 0.5;
			break;
	}
	
	// PWM calculation
	PWM = 255*DutyCycle;
	
	if(PWM>255){
		PWM=255;
	} 
	else if(PWM<0) {
		PWM = 0;
	}
			
	// if PWM_Pin is high
	if(PWMoutput) 
	{
		PWMoutput = 0;
		temp = (255-PWM)*max_freq ;
		TH0  = 0xFF - (temp>>8)&0xFF;
		TL0  = 0xFF - temp&0xFF; 
	}
	
	// if PWM_Pin is low
	else 
	{
		PWMoutput = 1;
		temp = PWM*max_freq;
		TH0  = 0xFF - (temp>>8)&0xFF;
		TL0  = 0xFF - temp&0xFF;
	}

	TF0 = 0;     // Clear the interupt flag
	TR0 = 1;     // Start Timer 0
}


/////////////////////
//  MAIN FUNCTION  //
/////////////////////


void main()
{
	//reset timers and flag
	TH0 = 0; 
	TL0 = 0;
	TR0 = 1; //start timer0
	TF0 = 0;
	TH1 = 0; 
	TL1 = 0;
	TR1 = 1; //start timer1
	IT0 = 1; //Make external interrupt0 an edge-triggered
	//enable interupt for timer1, timer0, and external interrupt0 (1000 1011 = 8B)
	IE = 0x8B; 

	initialize_PWM();       //initialize PWM and Timers

	while(1){} //Ensure program always running to use timers and int0, int1, int3
}
