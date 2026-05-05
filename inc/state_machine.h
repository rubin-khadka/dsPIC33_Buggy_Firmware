/* 
 * File:   state_machine.h
 * Author: Rubin Khadka
 *
 * Header file of state machine implementation
 */

#ifndef STATE_MACHINE_H
#define	STATE_MACHINE_H

#include "adc_sensor.h"
#include "motor_control.h"

#ifdef	__cplusplus
extern "C" {
#endif

// Local constants
#define EMERGENCY_BLINK_MS      250  // 1Hz blinking (500ms on/off)
#define CLEARANCE_DURATION_MS   2500  // 5 second obstacle clearance
    
// States
typedef enum {
    WAIT_FOR_START,
    MOVING,
    EMERGENCY
} BuggyState;

// External variables
extern volatile BuggyState current_state;
extern volatile uint16_t obstacle_clear_timer;

// Function prototypes for emergency LED
void init_emergency_LED(void);
void set_left_led(uint8_t state);
void set_right_led(uint8_t state);

// Functions for state machine
void handle_state_machine(void);
void trigger_emergency(void);
void clear_emergency(void);
void update_obstacle_clear_timer(void);

#ifdef	__cplusplus
}
#endif

#endif	/* STATE_MACHINE_H */

