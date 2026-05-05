/**
 * File: state_machine.c
 * 
 * State machine implementation for buggy control system
 */

#include "state_machine.h"
#include "message_handler.h"

// Global state variables
volatile BuggyState current_state = WAIT_FOR_START;  // Current system state
volatile uint8_t emergency_triggered = 0;            // Emergency flag
volatile uint16_t obstacle_clear_timer = 0;          // Obstacle clearance counter

/* Initialize emergency LED indicators */
void init_emergency_LED(void) {
    // Initialize LED outputs to off state
    set_left_led(0);
    set_right_led(0);
}

/* Control left indicator LED */
void set_left_led(uint8_t state) {
    LED_LEFT = state ? 1 : 0;
}

/* Control right indicator LED */
void set_right_led(uint8_t state) {
    LED_RIGHT = state ? 1 : 0;
}

/* Main state machine handler */
void handle_state_machine(void) {
    static BuggyState previous_state = WAIT_FOR_START;  // Track previous state
    static uint16_t blink_counter = 0;                  // Emergency blink timer
    static uint8_t emergency_light_state = 0;           // Emergency LED state
    
    // State entry actions (execute once on state change)
    if (current_state != previous_state) {
        switch(current_state) {
            case WAIT_FOR_START:
                stop_all_motors();  // Ensure motors are stopped
                if (previous_state == EMERGENCY) {
                    clear_emergency();  // Clear emergency flags if coming from EMERGENCY
                }
                // Turn off all indicators
                set_left_led(0);
                set_right_led(0);
                break;
                
            case MOVING:
                // Reset emergency tracking variables
                emergency_triggered = 0;
                obstacle_clear_timer = 0;
                // Turn off indicators
                set_left_led(0);
                set_right_led(0);
                break;
                
            case EMERGENCY:
                trigger_emergency();  // Execute emergency procedures
                // Initialize emergency lights (start on)
                emergency_light_state = 1;
                set_left_led(1);
                set_right_led(1);
                break;
        }
        previous_state = current_state;  // Update state tracking
    }
    
    // State continuous actions (execute every cycle)
    switch(current_state) {
        case WAIT_FOR_START:
            // No continuous actions needed
            stop_all_motors(); // Redundant but project require 500Hz PWM update
            break;
            
        case MOVING:
            // Continuously apply motor control commands
            set_buggy_motion(speed, yawrate);
            break;
            
        case EMERGENCY:
            
            stop_all_motors(); // Redundant but project require 500Hz PWM update
            
            // Handle emergency light blinking (1Hz)
            if (++blink_counter >= EMERGENCY_BLINK_MS) {
                emergency_light_state ^= 1;  // Toggle LED state
                set_left_led(emergency_light_state);
                set_right_led(emergency_light_state);
                blink_counter = 0;  // Reset blink counter
            }
            
            // Handle obstacle clearance timeout
            if (obstacle_clear_timer > 0) {
                if (--obstacle_clear_timer == 0) {
                    current_state = WAIT_FOR_START;  // Return to idle
                }
            }
            break;
    }
}

/* Execute emergency procedures */
void trigger_emergency(void) {
    if (!emergency_triggered) {
        stop_all_motors();  // Immediate motor stop
        uart_send_string("$MEMRG,1*");  // Send emergency message
        emergency_triggered = 1;  // Set emergency flag
        obstacle_clear_timer = 0;  // Reset clearance timer
    }
}

/* Clear emergency condition */
void clear_emergency(void) {
    uart_send_string("$MEMRG,0*");  // Send all-clear message
    emergency_triggered = 0;  // Reset emergency flag
}

/* Update obstacle clearance timer */
void update_obstacle_clear_timer(void) {
    if (current_state == EMERGENCY && obstacle_clear_timer == 0) {
        obstacle_clear_timer = CLEARANCE_DURATION_MS;  // Start clearance delay
    }
}