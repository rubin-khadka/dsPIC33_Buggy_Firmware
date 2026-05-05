/* 
 * File:   motor_control.h
 * Author: Rubin Khadka
 *
 * Header file for Motor Control
 */

#ifndef MOTOR_CONTROL_H
#define	MOTOR_CONTROL_H

#include "init.h"

#ifdef	__cplusplus
extern "C" {
#endif

// Constants
#define PWM_PERIOD  7199        // For 10kHz at 72MHz (72,000,000/10,000 - 1)
#define MIN_DUTY    2880        // 40% minimum duty
#define MAX_DUTY    PWM_PERIOD
  
#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

/* Function Prototype */
void init_motor_control(void);
void set_buggy_motion(int speed, int yawrate);
void stop_all_motors(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* MOTOR_CONTROL_H */

