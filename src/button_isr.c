/* 
 * File:   button_isr.c
 * Author: Rubin Khadka
 *
 * Button Interrupt Service Routines
 * Handles external button press detection with debouncing
 * using Timer2 for hardware debounce timing
 */

#include "timer.h"

// Global flag to communicate button state to main application
volatile uint8_t re8_button_pressed = 0;

/* INT1 External Interrupt Service Routine */
void __attribute__((__interrupt__, no_auto_psv)) _INT1Interrupt(void) {
    
    IEC1bits.INT1IE = 0;    // Disable INT1 interrupt
    // Clear interrupt flag 
    IFS1bits.INT1IF = 0;
    
    // Reset and start debounce timer (20ms period)
    TMR2 = 0;            // Reset timer counter
    IFS0bits.T2IF = 0;   // Clear any pending timer interrupt
    T2CONbits.TON = 1;   // Enable Timer2 for debounce period
}

/* Timer2 Interrupt Service Routine */
void __attribute__((__interrupt__, no_auto_psv)) _T2Interrupt(void) {
    
    IFS0bits.T2IF = 0;      // Clear Timer2 interrupt flag
    T2CONbits.TON = 0;      // Stop Timer2

    // Check if the button is still pressed
    if (BUTTON1 == 0) 
        re8_button_pressed = 1;
    else 
        re8_button_pressed = 0;

    // Re-enable the external interrupt
    IEC1bits.INT1IE = 1;
}