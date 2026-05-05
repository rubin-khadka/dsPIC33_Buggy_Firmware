/*
 * File:   main.c
 * Author: Rubin Khadka
 *
 * Main application file for the embedded project.
 */

#include "acc_spi.h"    // Accelerometer driver
#include "message_handler.h"  // Message handle functions

// Global variables
extern volatile uint8_t re8_button_pressed; // External flag for button press interrupt

int main(void) {
    
    // Initialize all hardware configurations
    config_init();  // Sets up GPIO, UART, SPI, Timers, and Sensors
    
    static uint8_t led_timer_count = 0;     // Counter for LED blinking
    static uint16_t send_counter_10Hz = 0;  // 10Hz data (accelerometer/IR)
    static uint16_t send_counter_1Hz = 0;   // 1Hz data (battery)
    
    // Initialize message parser state
    parser_state pstate = {
        .state = STATE_DOLLAR,
        .index_type = 0,
        .index_payload = 0
    };
       
    // Set up 2ms periodic timer for main loop timing
    tmr_setup_period(TIMER1, TIMER1_PERIOD_MS);
    
    while(1) {
        
        /* Code to handle the Project */
        
        /* Blink LED A0 with 1Hz frequency */
        if (led_timer_count++ >= LED_BLINK_TICKS) { // 1Hz
            LEDA0 ^= 1;                  // Toggle LED (1Hz blink indicates system alive)
            led_timer_count = 0;         // Reset the counter
        }
       
        /* Process incoming UART messages - read buffer until empty */
        if (!UART1_Buffer_IsEmpty(&uart1_rx)) {  // Quick empty check first
            uint8_t byte;
            // Read bytes while available in buffer
            while (UART1_Buffer_Critical_Read(&byte)) {
                // Parse each byte and check for complete message
                if (parse_byte(&pstate, byte) == NEW_MESSAGE) {
                    process_message(&pstate);  // Handle complete message
                }
            }
        }
                      
        /* Read IR distance sensor (continuous monitoring) */
        uint16_t ir_distance = get_ir_distance_cm(); // Get distance in cm
        
        /* Emergency stop check - highest priority */
        if (current_state == MOVING && ir_distance < EMERGENCY_THRESHOLD_CM) {
            current_state = EMERGENCY; // Immediate emergency trigger
        }
        else if (current_state == EMERGENCY) {
            // Handle obstacle clearance timer
            if (ir_distance < EMERGENCY_THRESHOLD_CM) {
                obstacle_clear_timer = 0;  // Reset timer if obstacle reappears
            } else if (ir_distance >= EMERGENCY_THRESHOLD_CM) {
                update_obstacle_clear_timer(); // Count time since obstacle cleared
            }
        }
        
        /* Check for Button press and perform required task */
        if (re8_button_pressed) {
            switch(current_state) {
                case WAIT_FOR_START:
                    current_state = MOVING;
                    break;
                case MOVING:
                    current_state = WAIT_FOR_START;
                    break;
                case EMERGENCY:
                    // No action
                    break;
            }
            re8_button_pressed = 0;  // Clear flag
        }
        
        handle_state_machine(); // Perform task based on current state
        
        /* Send Accelerometer and IR distance data at 10 Hz */
        if (send_counter_10Hz++ >= ACC_IR_SEND_TICKS){
            // Read and transmit accelerometer data
            AccData acc = read_acc_all();       // Read all accelerometer axes
            send_acc_data(&acc);                // Send via UART
            
            // Transmit IR distance data
            send_ir_distance(ir_distance);
            
            send_counter_10Hz = 0;  // Reset counter
        }
        
        /* Read and send Battery data at 1 Hz */
        if (send_counter_1Hz++ >= BATTERY_SEND_TICKS){
            // Read and transmit battery voltage
            send_battery_data(get_battery_voltage());
            
            send_counter_1Hz = 0;  // Reset counter
        }
           
        uint8_t ret = tmr_wait_period(TIMER1);
        
        if (ret > 0) LEDG9 ^= 1;  // Toggle LED2 if deadline missed (debug), Should never happen
    }
    
    return 0;
    
}


