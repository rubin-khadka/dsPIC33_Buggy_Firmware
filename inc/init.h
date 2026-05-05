/* 
 * File:   init.h
 * Author: Rubin Khadka 
 *
 * Header file for peripherals configuration
 */

#ifndef INIT_H
#define	INIT_H

#include "xc.h"          
#include "stdint.h"      
#include "stdbool.h"     
#include "stdio.h"     
#include "string.h"     
#include "stddef.h" 
#include "math.h"
#include "stdlib.h"

#ifdef __cplusplus
extern "C" { 
#endif

/* Clock Configuration */
#define FCY (72000000UL)  // Instruction cycle frequency (72 MHz)
    
/* Timing Configuration */
// Main system tick interval (in ms)
#define TIMER1_PERIOD_MS    2       // 2 ms ? 500 Hz main loop
#define LED_BLINK_TICKS     250     // LED blink tick rate (500/2) 500 on - off
#define ACC_IR_SEND_TICKS   50      // Accelerometer logging rate (100/2)
#define BATTERY_SEND_TICKS  500     // Battery sensing rate (1000/2)

/* Hardware Pin Mapping */
// LEDs
#define LEDA0 LATAbits.LATA0        // LED 1 definition
#define LEDG9 LATGbits.LATG9        // LED 2 definition
#define LED_LEFT LATBbits.LATB8     // LED LEFT definition
#define LED_RIGHT LATFbits.LATF1    // LED RIGHT definition
    
// Buttons
#define BUTTON1 PORTEbits.RE8 // Button 1 definition
#define BUTTON2 PORTEbits.RE9 // Button 2 definition

/* System Initialization */
void config_init();  // Initialize clock, ports, peripherals

#ifdef	__cplusplus
}
#endif

#endif	/* INIT_H */

