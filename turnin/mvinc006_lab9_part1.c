/*	Author: Mark Alan Vincent II
 *  Partner(s) Name: NA
 *	Lab Section: A01
 *	Assignment: Lab #9  Exercise #1
 *  Youtube: https://youtu.be/gvMS2LCej30
 *	Exercise Description:
 *
 *  We have four LEDs, connected to PB0, PB1, PB2 and PB3.
 *  One SM will output to a shared varible, and run a sequence on B0 - B2.
 *  One SM will blink the PB3 on a shared varible.
 *  A third SM will combine both shared varibles and write to PORTB.
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
#endif

/*
 *
 *
 *
*/

// globals (shared varibles for the SMs)
unsigned char tmpA = 0x00;
unsigned char threeLEDs = 0x00;
unsigned char blinkingLED = 0x00;

// Our concurrent synchSMs
enum SM1_States { SM1_Start, SM1_OffLED, SM1_OnLED } sm1_state;
int TickFct_BlinkLED(int state) {
    
    // Read Input from tmpA
    tmpA = ~PINA & 0xFF;

    // Transitions
    switch(state) {
    
        case SM1_Start:
            // Go into OffLED state
            state = SM1_OffLED;
            break;
    
        case SM1_OffLED:
            // Go into OnLED state
            state = SM1_OnLED;
            break;

        case SM1_OnLED:
            // Go into OffLED state
            state = SM1_OffLED;
            break;
    
        default:
            // Default
            state = SM1_Start;
            break;

    }

    // Actions
    switch(state) {
 
        case SM1_Start:
            // Init to 0
            blinkingLED = 0x00;
            break;
    
        case SM1_OffLED:
            // turn LED off
            blinkingLED = 0x00;
            break;

        case SM1_OnLED:
            // turn LED on
            blinkingLED = 0x08;
            break;
    
        default:
            break;

    }

    // End of Tick Function SM1
    return state;
}

/*
 *
 *
 *
*/

enum SM2_States { SM2_Start, SM2_Seq0, SM2_Seq1, SM2_Seq2} sm2_state;
int TickFct_SeqLED(int state) {
    
    // Transitions
    switch(state) {
        
        case SM2_Start:
            // Go into Seq0
            state = SM2_Seq0;
            break;
        
        case SM2_Seq0:
            // Go into Seq1
            state = SM2_Seq1;
            break;

        case SM2_Seq1:
            // Go into Seq2
            state = SM2_Seq2;
            break;

        case SM2_Seq2:
            // Go into Seq0
            state = SM2_Seq0;
            break;

        default:
            // Default
            state = SM2_Start;
            break;

    }

    // Actions
    switch(state) {

        case SM2_Start:
            // Init var
            threeLEDs = 0x00;
            break;
        
        case SM2_Seq0:
            // Turn on B0
            threeLEDs = 0x01;
            break;

        case SM2_Seq1:
            // Turn on B1
            threeLEDs = 0x02;
            break;

        case SM2_Seq2:
            // Turn on B2
            threeLEDs = 0x04;
            break;

        default:
            break;

    }

    // End of Tick Function SM2
    return state;
}

enum SM3_States { SM3_Start, SM3_Write} sm3_state;
int TickFct_WriteSM(int state) {
    
    // Static var
    static char tmpB;

    // Transitions
    switch(state) {
        
        case SM3_Start:
            // Go into Write
            state = SM3_Write;
            break;
        
        case SM3_Write:
            // Go into Write
            state = SM3_Write;
            break;

        default:
            // Default
            state = SM3_Start;
            break;

    }

    // Actions
    switch(state) {

        case SM3_Start:
            // Write 0
            tmpB = 0x00;
            PORTB = tmpB;
            break;
        
        case SM3_Write:
            // Combine Two Shared Vars into tmpB
            tmpB = ((threeLEDs & 0x07) | (blinkingLED & 0x08));
            // Write
            PORTB = tmpB;
            break;

        default:
            break;

    }
    
    // End of Tick Function SM3
    return state;
}



// Main Funcntion
int main(void) {
    
    // PORTS
    DDRA = 0x00; PORTA = 0xFF; // PortA as input
    DDRB = 0xFF; PORTB = 0x00; // PortB as output
    
    // Setup Task List
    unsigned char i = 0;
	
    // SM1
    tasks[i].state = SM1_Start;
	tasks[i].period = 1500;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_BlinkLED;
	i++;
    
    // SM2
	tasks[i].state = SM2_Start;
	tasks[i].period = 1000;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_SeqLED;
    i++;

    // SM3
    tasks[i].state = SM3_Start;
	tasks[i].period = 1000;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_WriteSM;
    
    // Setup System Period & Timer to ON.
    TimerSet(500);
	TimerOn();
    
    while (1) {}

    return 1;
}
