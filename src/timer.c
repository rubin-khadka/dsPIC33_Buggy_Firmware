
/* Source file for timer */

#include "timer.h"

/* Configuration of timer 2 for debouncing delay*/
void debounce_tmr_init(){
    uint32_t cycles = (uint32_t)DEBOUNCE_DELAY * 72000; // Calculate period

    T2CONbits.TON = 0;  // Stop Timer 2
    T2CONbits.TCKPS = 0b11;  // Set prescaler to 1:256
    TMR2 = 0;  // Reset Timer 2
    PR2 = (uint16_t)(cycles / 256);  // Set period register
    IFS0bits.T2IF = 0;  // Clear Timer 2 interrupt flag
    IEC0bits.T2IE = 1;  // Enable Timer 2 interrupt
}

/* Prescaler configurations for 16-bit timers */
static const PrescalerConfig prescalers[] = {
    {1, 0b00},    // 1:1 
    {8, 0b01},    // 1:8
    {64, 0b10},   // 1:64
    {256, 0b11}   // 1:256 
};

/* Function to choose suitable prescaler */
static PrescalerSelection choose_prescaler(uint16_t ms) {
    uint32_t required_cycles = (uint32_t)(ms * (FCY / 1000)); // ms -> cycles

    // Iterate through pre scalers starting from the smallest
    for (uint8_t i = 0; i < sizeof(prescalers) / sizeof(prescalers[0]); i++) {
        uint32_t cycles_with_prescaler = required_cycles / prescalers[i].divisor;

        // Check if the cycles fit within the timer maximum period
        if (cycles_with_prescaler <= TIMER_MAX_COUNT) {
            // Return the selected pre scaler and the calculated cycles
            PrescalerSelection selection = {&prescalers[i], cycles_with_prescaler};
            return selection;
        }
    }

    // If no pre scaler can handle the delay, return NULL
    PrescalerSelection selection = {NULL, 0};
    return selection;
}

/* Function to setup timer period */
void tmr_setup_period(uint8_t timer, uint16_t ms){
    // Choose suitable timer
    PrescalerSelection selection = choose_prescaler(ms);
    if (selection.prescaler == NULL) {
        return;
    }
    
    // Use Computed cycle with selected pre scaler
    uint32_t cycles = selection.cycles_with_prescaler;
    
    // Timer Initialize
    switch(timer) {
        case TIMER1:
            TMR1 = 0;  // Reset Timer 1 counter
            PR1 = (uint16_t)cycles;  // Set period register
            T1CONbits.TCKPS = selection.prescaler->tckps;  // Set pre scaler
            T1CONbits.TON = 1;  // Start Timer 1
            break;

        case TIMER2:
            TMR2 = 0;  // Reset Timer 2 counter
            PR2 = (uint16_t)cycles;  // Set period register
            T2CONbits.TCKPS = selection.prescaler->tckps;  // Set pre scaler
            T2CONbits.TON = 1;  // Start Timer 2
            break;

        case TIMER3:
            TMR3 = 0;  // Reset Timer 3 counter
            PR3 = (uint16_t)cycles;  // Set period register
            T3CONbits.TCKPS = selection.prescaler->tckps;  // Set pre scaler
            //T3CONbits.TON = 1;  // Start Timer 3
            break;

        case TIMER4:
            TMR4 = 0;  // Reset Timer 4 counter
            PR4 = (uint16_t)cycles;  // Set period register
            T4CONbits.TCKPS = selection.prescaler->tckps;  // Set pre scaler
            T4CONbits.TON = 1;  // Start Timer 4
            break;

        default:
            // Handle invalid timer number
            break;
    }
}

/* Function to wait timer finishing count with busy wait */
uint8_t tmr_wait_period(uint8_t timer) {
    switch (timer) {
        case TIMER1:
            if (IFS0bits.T1IF) {  // Check if Timer 1 has already expired
                IFS0bits.T1IF = 0;  // Clear the flag
                return 1;  // Timer has already expired
            }
            while (!IFS0bits.T1IF);  // Wait for Timer 1 interrupt flag
            IFS0bits.T1IF = 0;  // Clear the flag
            return 0;  // Timer expired after waiting
            break;

        case TIMER2:
            if (IFS0bits.T2IF) {  // Check if Timer 2 has already expired
                IFS0bits.T2IF = 0;  // Clear the flag
                return 1;  // Timer has already expired
            }
            while (!IFS0bits.T2IF);  // Wait for Timer 2 interrupt flag
            IFS0bits.T2IF = 0;  // Clear the flag
            return 0;  // Timer expired after waiting
            break;

        case TIMER3:
            if (IFS0bits.T3IF) {  // Check if Timer 3 has already expired
                IFS0bits.T3IF = 0;  // Clear the flag
                return 1;  // Timer has already expired
            }
            while (!IFS0bits.T3IF);  // Wait for Timer 3 interrupt flag
            IFS0bits.T3IF = 0;  // Clear the flag
            return 0;  // Timer expired after waiting
            break;
        
        case TIMER4:
            if (IFS1bits.T4IF) {  // Check if Timer 3 has already expired
                IFS1bits.T4IF = 0;  // Clear the flag
                return 1;  // Timer has already expired
            }
            while (!IFS1bits.T4IF);  // Wait for Timer 3 interrupt flag
            IFS1bits.T4IF = 0;  // Clear the flag
            return 0;  // Timer expired after waiting
            break;
            
        default:
            // Handle invalid timer number
            return 1;
            break;
    }
}

/* Function to create delay */
void tmr_wait_ms(uint8_t timer, uint16_t ms) {
    // Choose suitable pre scaler
    PrescalerSelection selection = choose_prescaler(ms);
    if (selection.prescaler == NULL) {
        return;  // No suitable pre scaler found
    }
    
    // Timer Initialize
    switch(timer) {
        case TIMER1:
            T1CONbits.TON = 0;  // Stop Timer 1
            TMR1 = 0;  // Timer 1 initialize 
            PR1 = (uint16_t)selection.cycles_with_prescaler;  // Set period register
            T1CONbits.TCKPS = selection.prescaler->tckps;  // Set selected pre scaler
            T1CONbits.TON = 1;  // Start Timer 1
            while (!IFS0bits.T1IF);  // Wait for Timer 1 interrupt flag, busy wait
            IFS0bits.T1IF = 0;      // Clear the flag
            T1CONbits.TON = 0;  // Stop Timer 1
            break;

        case TIMER2:
            T2CONbits.TON = 0;  // Stop Timer 2
            TMR2 = 0;  // Timer 2 initialize
            PR2 = (uint16_t)selection.cycles_with_prescaler;  // Set period register
            T2CONbits.TCKPS = selection.prescaler->tckps;  // Set selected pre scaler
            T2CONbits.TON = 1;  // Start Timer 2
            while (!IFS0bits.T2IF);  // Wait for Timer 2 interrupt flag, busy wait
            IFS0bits.T2IF = 0;      // Clear the flag
            T2CONbits.TON = 0;  // Stop Timer 2
            break;

        case TIMER3:
            T3CONbits.TON = 0;  // Stop Timer 3
            TMR3 = 0;  // Reset Timer 3 counter
            PR3 = (uint16_t)selection.cycles_with_prescaler;  // Set period register
            T3CONbits.TCKPS = selection.prescaler->tckps;  // Set selected pre scaler
            T3CONbits.TON = 1;  // Start Timer 3
            while (!IFS0bits.T3IF);  // Wait for Timer 3 interrupt flag, busy wait
            IFS0bits.T3IF = 0;      // Clear the flag
            T3CONbits.TON = 0;  // Stop Timer 3
            break;
                
        case TIMER4:
            T4CONbits.TON = 0;  // Stop Timer 4
            TMR4 = 0;  // Reset Timer 4 counter
            PR4 = (uint16_t)selection.cycles_with_prescaler;  // Set period register
            T4CONbits.TCKPS = selection.prescaler->tckps;  // Set selected pre scaler
            T4CONbits.TON = 1;  // Start Timer 4
            while (!IFS1bits.T4IF);  // Wait for Timer 4 interrupt flag, busy wait
            IFS1bits.T4IF = 0;      // Clear the flag
            T4CONbits.TON = 0;  // Stop Timer 4
            break;
            
        default:
            // Handle invalid timer number
            break;
    }
}
