/*	Author: Mark Alan Vincent II
 *  Partner(s) Name: NA
 *	Lab Section: A01
 *	Assignment: Lab #9  Exercise #4
 *  Youtube: https://youtu.be/OZ_KUOcomz4
 *	Exercise Description:
 *
 *  We have four LEDs, connected to PB0, PB1, PB2 and PB3.
 *  One SM will output to a shared varible, and run a sequence on B0 - B2.
 *  One SM will blink the PB3 on a shared varible.
 *  A third SM will combine both shared varibles and write to PORTB.
 *
 *  In this case, we need the sequence to complete in 300ms, not 1000ms.
 *
 *  I need to add a speaker to B4 and pulse it, 2ms high, 2ms low.
 *  A0 - Turn the speaker on.
 *
 *  I also need to add two buttons on A1 and A2 for changing the frequency.
 *  A1 - Down, A2 - Up.
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
unsigned char pulseCounter = 0x00;
unsigned char pulseOutput = 0x00;
unsigned char highPulse = 0x02;

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
            tmpB = ((threeLEDs & 0x07) | (blinkingLED & 0x08) | 
                    (pulseOutput & 0x10));

            // Write
            PORTB = tmpB;
            break;

        default:
            break;

    }
    
    // End of Tick Function SM3
    return state;
}


enum SM4_States { SM4_Start, SM4_High, SM4_Low} sm4_state;
int TickFct_SoundSM(int state) {
    
    // Read Input from tmpA
    tmpA = ~PINA & 0xFF;
    
    // Transitions
    switch(state) {
        
        case SM4_Start:
            // Only Goes to High if A0 is 1.
            if (tmpA == 0x01) {
                // Go to High Phase
                state = SM4_High;
            } else {
                // Stay in SM4_Start
                state = SM4_Start;
            }
            break;
        
        case SM4_High:
            // Goes back to SM4_Start if no input
            if (tmpA == 0x00) {
                // go to SM4_Start
                state = SM4_Start;
                pulseCounter = 0;
                break;
            } else {
                // stay in SM4_High
                state = SM4_High;
            }

            // Checks whether or not to go to low using counter.
            if (pulseCounter >= highPulse) {
                // go to Low, reset pulse Counter.
                state = SM4_Low;
                pulseCounter = 0;
            } else {
                // Stay in High, increment pulseCounter
                state = SM4_High;
                pulseCounter++;
            }
            break;
        
        case SM4_Low:
            // Checks whether or not to go to high using counter.
            if (pulseCounter >= highPulse) {
                // go to High, reset pulse Counter.
                state = SM4_High;
                pulseCounter = 0;
            } else {
                // Stay in Low, increment pulseCounter
                state = SM4_Low;
                pulseCounter++;
            }
            
            break;

        default:
            // Default
            state = SM4_Start;
            break;

    }

    // Actions
    switch(state) {

        case SM4_Start:
            // Init output to 0
            pulseOutput = 0x00;
            break;
        
        case SM4_High:
            // Set output to high
            pulseOutput = 0x10;
            break;
        
        case SM4_Low:
            // Set output to low
            pulseOutput = 0x00;
            break;

        default:
            break;

    }
    
    // End of Tick Function SM4
    return state;
}

enum SM5_States { SM5_Start, SM5_A1, SM5_A2} sm5_state;
int TickFct_ChangeFreSM(int state) {
    
    // Read Input from tmpA
    tmpA = ~PINA & 0xFF;
    
    // Transitions
    switch(state) {
        
        case SM5_Start:
            if ((tmpA & 0x06) == 0x02) {
                // A1 is pressed
                state = SM5_A1;
            } else if ((tmpA & 0x06) == 0x04) {
                // A2 is pressed
                state = SM5_A2;
            } else if ((tmpA & 0x06) == 0x06) {
                // Both are pressed
                state = SM5_Start;
            } else {
                // None are pressed
                state = SM5_Start;
            }
            break;

        case SM5_A1:
            if ((tmpA & 0x06) == 0x02) {
                // A1 is pressed
                state = SM5_A1;
            } else if ((tmpA & 0x06) == 0x06) {
                // Both are pressed
                state = SM5_Start;
            } else {
                // None are pressed
                state = SM5_Start;
            } 
            break;

        case SM5_A2:
            if ((tmpA & 0x06) == 0x04) {
                // A2 is pressed
                state = SM5_A2;
            } else if ((tmpA & 0x06) == 0x06) {
                // Both are pressed
                state = SM5_Start;
            } else {
                // None are pressed
                state = SM5_Start;
            }
            break;

        default:
            // Default
            state = SM5_Start;
            break;

    }

    // Actions
    switch(state) {

        case SM5_Start:
            // No changes are made.
            break;
        
        case SM5_A1:
            // Checks if pulse rate is too low.
            if (highPulse > 1) {
                // Lower pulse rate by 1
                highPulse = highPulse - 1;
            } else {
                // Do nothing
            }
            break;

        case SM5_A2:
            /// Checks if pulse rate is too large.
            if (highPulse < 1001) {
                // Raise pulse rate by 1
                highPulse = highPulse + 1;
            } else {
                // Do nothing
            }
            break;

        default:
            break;

    }
    
    // End of Tick Function SM5
    return state;
}



// Main Funcntion
int main(void) {
    
    // PORTS
    DDRA = 0x00; PORTA = 0xFF; // PortA as input
    DDRB = 0xFF; PORTB = 0x00; // PortB as output
    
    // Init pulse rate
    highPulse = 2;

    // Setup Task List
    unsigned char i = 0;
	
    // SM1
    tasks[i].state = SM1_Start;
	tasks[i].period = 1000;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_BlinkLED;
	i++;
    
    // SM2
	tasks[i].state = SM2_Start;
	tasks[i].period = 300;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_SeqLED;
    i++;

    // SM3
    tasks[i].state = SM3_Start;
	tasks[i].period = 1;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_WriteSM;
    i++;

    // SM4
	tasks[i].state = SM4_Start;
	tasks[i].period = 1;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_SoundSM;
    i++;

    // SM5
    tasks[i].state = SM5_Start;
	tasks[i].period = 35;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_ChangeFreSM;
    
    // Setup System Period & Timer to ON.
    TimerSet(tasksGCD);
	TimerOn();
    
    while (1) {}

    return 1;
}
