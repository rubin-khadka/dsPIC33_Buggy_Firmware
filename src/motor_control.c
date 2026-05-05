/**
 * File: motor_control.c  
 * Author: Rubin Khadka
 * 
 * Motor control module for differential drive robot
 */

#include "motor_control.h"

/* Initialize Timer4 as PWM timebase */
void init_timer4_pwm_base(void) {
    T4CONbits.TON = 0;          // Disable timer before configuration
    T4CONbits.TCKPS = 0b00;     // No prescaling (72MHz clock)
    T4CONbits.TGATE = 0;        // Disable gated timer mode
    PR4 = PWM_PERIOD;           // Set period for 10kHz (7200 counts)
    TMR4 = 0;                   // Reset timer counter
    IFS1bits.T4IF = 0;          // Clear interrupt flag
    IEC1bits.T4IE = 0;          // Disable timer interrupts (PWM doesn't need them)
    T4CONbits.TON = 1;          // Enable Timer4
}

/* Configure Output Compare module for PWM generation */
void configure_oc_channel(int channel) {
    switch (channel) {
        case 1:  // OC1 - Left motor reverse
            OC1CON1 = 0;
            OC1CON1bits.OCTSEL = 0b010;     // Timer4 as clock source  
            OC1CON1bits.OCM = 0b110;        // Edge-aligned PWM mode
            OC1CON2 = 0;
            OC1CON2bits.SYNCSEL = 0b01110;  // Synchronize with Timer4
            OC1RS = PWM_PERIOD-1;           // Match PWM period
            OC1R = 0;                       // Start with 0% duty cycle
            break;
            
        case 2:  // OC2 - Left motor forward
            OC2CON1 = 0;
            OC2CON1bits.OCTSEL = 0b010;
            OC2CON1bits.OCM = 0b110;
            OC2CON2 = 0;
            OC2CON2bits.SYNCSEL = 0b01110;
            OC2RS = PWM_PERIOD-1; // not needed as timer (PR4) handles the actual period
            OC2R = 0;
            break;
            
        case 3:  // OC3 - Right motor reverse
            OC3CON1 = 0;
            OC3CON1bits.OCTSEL = 0b010;
            OC3CON1bits.OCM = 0b110;
            OC3CON2 = 0;
            OC3CON2bits.SYNCSEL = 0b01110;
            OC3RS = PWM_PERIOD-1;
            OC3R = 0;
            break;
            
        case 4:  // OC4 - Right motor forward
            OC4CON1 = 0;
            OC4CON1bits.OCTSEL = 0b010;
            OC4CON1bits.OCM = 0b110;
            OC4CON2 = 0;
            OC4CON2bits.SYNCSEL = 0b01110;
            OC4RS = PWM_PERIOD-1;
            OC4R = 0;
            break;
            
        default:
            break;  // Invalid channel - do nothing
    }
}

/* Map Output Compare outputs to physical pins */
void map_output_compare_to_pins(void) {
    // Left wheel control
    RPOR0bits.RP65R = 0b010000;  // OC1 -> RD1 (Left reverse)
    RPOR1bits.RP66R = 0b010001;  // OC2 -> RD2 (Left forward)

    // Right wheel control  
    RPOR1bits.RP67R = 0b010010;  // OC3 -> RD3 (Right reverse)
    RPOR2bits.RP68R = 0b010011;  // OC4 -> RD4 (Right forward)
}

/* Initialize complete motor control system */
void init_motor_control(void) {
    init_timer4_pwm_base();      // Setup PWM timebase
    
    // Configure all four OC channels
    for (int i = 1; i <= 4; i++) {
        configure_oc_channel(i);
    }
    
    map_output_compare_to_pins(); // Route signals to motor driver pins
    stop_all_motors();            // Start with motors stopped
}

/* Convert user speed (0-100%) to PWM duty cycle */
static uint16_t scale_to_pwm(uint16_t user_input) {
    if (user_input == 0) return 0;
    return MIN_DUTY + ((user_input * (PWM_PERIOD - MIN_DUTY)) / 100);
}

/* Set differential drive motion */
void set_buggy_motion(int speed, int yawrate) {
    // Differential drive calculation with saturation
    int left = speed - yawrate;
    int right = speed + yawrate;
    
     // Ratio-preserving scaling
    int max_val = (abs(left) > abs(right)) ? abs(left) : abs(right);
    if (max_val > 100) {
        left = (left * 100) / max_val;
        right = (right * 100) / max_val;
    }

    /* LEFT MOTOR CONTROL */
    if (left > 0) {  // Forward
        OC2R = scale_to_pwm(left);  // Forward PWM
        OC1R = 0;                   // Disable reverse
    } 
    else if (left < 0) {  // Backward
        OC1R = scale_to_pwm(-left); // Reverse PWM
        OC2R = 0;                   // Disable forward
    } 
    else {  // Stop
        OC1R = OC2R = 0;
    }

    /* RIGHT MOTOR CONTROL */ 
    if (right > 0) {  // Forward
        OC4R = scale_to_pwm(right); // Forward PWM
        OC3R = 0;                   // Disable reverse
    } 
    else if (right < 0) {  // Backward
        OC3R = scale_to_pwm(-right); // Reverse PWM
        OC4R = 0;                    // Disable forward
    } 
    else {  // Stop
        OC3R = OC4R = 0;
    }
}

/* Stop all motors immediately */
void stop_all_motors(void) {
    OC1R = OC2R = OC3R = OC4R = 0;
}