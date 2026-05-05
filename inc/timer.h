/* 
 * File:   Timer.h
 * Author: Rubin Khadka
 *
 * Header File for TIMER
 */

#ifndef TIMER_H
#define TIMER_H

#include "init.h" 

#ifdef __cplusplus
extern "C" { 
#endif

/* Timer Module Selection */
#define TIMER1 1  // Timer1 identifier
#define TIMER2 2  // Timer2 identifier
#define TIMER3 3  // Timer3 identifier
#define TIMER4 4  // Timer4 identifier
#define TIMER5 5  // Timer5 identifier
#define TIMER6 6  // Timer6 identifier
#define TIMER7 7  // Timer7 identifier
#define TIMER8 8  // Timer8 identifier
#define TIMER9 9  // Timer9 identifier
    
    
/* Timer Constants */
#define TIMER_MAX_COUNT 0xFFFF      // Maximum 16-bit timer value (65535)
#define DEBOUNCE_DELAY  20          // Debounce delay
    
/* Prescaler Configuration */
typedef struct {
    uint16_t divisor;  // Actual division factor 
    uint8_t tckps;     // Register bits value 
} PrescalerConfig;

/* Timer Calculation Result */
typedef struct {
    const PrescalerConfig *prescaler;  // Selected prescaler configuration
    uint32_t cycles_with_prescaler;    // Calculated cycles after prescaling
} PrescalerSelection; 

/* Function Prototypes */
void debounce_tmr_init();
void tmr_setup_period(uint8_t timer, uint16_t ms);
uint8_t tmr_wait_period(uint8_t timer);
void tmr_wait_ms(uint8_t timer, uint16_t ms);

#ifdef __cplusplus
}
#endif

#endif	/* TIMER_H */

