/*	Author: David Perez  dpere048
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab6 #  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
volatile unsigned char TimerFlag = 0; //TimerISR() sets this to 1, C program clears it to 0

//variables for mapping AVR TSR
unsigned long _avr_timer_M = 1; //start count from here down to zero, default = 1ms
unsigned long _avr_timer_cntcurr = 0; //internal count of ticks (1ms)
unsigned char ThreeLeds = 0x00;
unsigned char BlinkingLeds = 0x00;
unsigned char tmpB = 0x00; //initializes variable
enum threeStates{start, lightA, lightB, lightC}threeState;

void TimerOn() {
	//avr timer/cnter
	TCCR1B = 0x0B; //BIT3 = 0: CTC Mode (clears time on compare)
	//0x0B = 00001011, 8MHz clock or 8,000,000/64 = 125k ticks
	//TCNT1 register will count 125k ticks
	//AVR OUTPUT COMPARE REGISTER
	OCR1A = 125; //Timer interrupt will be generated when TCNT1=OCR1A
	//We want a 1ms tick. .001s*125k = 125
	//when TCNT1 register = 125, 1ms has passed, so we comparte 10 124
	//AVR TIMER INTERRUP MASK REGISTER
	TIMSK1	 = 0x02; //bit 1: OC1E1A--- enables compare match interrupt

	//initialize avr counter
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	//TimerISR will be called every _Avr_timer_cntcurr ms
	
	//Enable global interrupts
	SREG |= 0x80; // 0x80: 1000000
}
void TimerOff() {
	TCCR1B = 0x00; //b3b1b0 = 000, timer off
}
void TimerISR() {
	TimerFlag = 1;
}
//C programmer does not touch this ISR but rather TIMERISR
ISR(TIMER1_COMPA_vect) {
	//CPU calls when TCNT1 == OCR1 (every 1ms when TimerOn settings
	_avr_timer_cntcurr--; //count down to 0 rathen tahn up to TOP
	if (_avr_timer_cntcurr == 0){ //more effiecient compare
		TimerISR(); //calls ISR that user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
void ThreeLedsSM_Tick(){
	switch(threeState)
	{
		case start:
			threeState = lightA;
			break;
		case lightA:
			threeState = lightB;
			break;
		case lightB:
			threeState = lightC;
			break;
		case light C:
			threeState = lightA;
			break;
	}
	switch(threeState)
	{
		case start:
			break;			
		case lightA:
			ThreeLeds = 0x01;
			break;
		case lightB:
			ThreeLeds = 0x02;
			break;
		case lightC:
			ThreeLeds = 0x04;
			break;
	}
}
enum blinkingStates{on, off}blinkState;
void BlinkingLedsSM_Tick(){
	switch(blinkState){
		case on:
			blinkState = off;
			break;
		case off:
			blinkState = on;
			break;
		}
	switch(blinkState){
		case on:
			BlinkingLeds = 0x08;
			break;
		case off:
			BlinkingLeds = 0x00;
			break;
		}
}
enum combineStates{combination}combineState;
void CombineLedsSM_Tick(){
	switch(combineState){
		case combination:
		break;
	}
	switch(combineState){
		case combination:
			tmpB = BlinkingLeds | ThreeLeds;
		break;
	}
}
int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFF; //set port b to output
	PORTB = 0x00; //init port B to 0s
	const unsigned long TimerThreeLeds = 300;
	unsigned long TimerBlink = 1000;
	unsigned long Time = 0;
	
	TimerSet(TimerThreeLeds);
	TimerOn();
	threeState = start;
	combineState = combination;
	blinkState = on;
    /* Insert your solution below */
    while (1) {
	if(Time >= TimerThreeLeds){
		BlinkingLedsSM_Tick();
		Time = 0;
	}
	ThreeLedsSM_Tick();
	CombineLedsSM_Tick();
	PORTB = tmpB;
	while (!TimerFlag); //wait 1 seec
	TimerFlag = 0;
	Time += TimerThreeLeds;
    }
    return 0;
}
