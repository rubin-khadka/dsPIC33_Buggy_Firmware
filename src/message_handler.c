/**
 * File: message_handler.c
 * 
 * UART message processing and message transmission
 */

#include "message_handler.h"

// Global motion control variables (volatile for ISR access)
volatile int16_t speed = 0;    // Current speed setting (-100 to 100)
volatile int16_t yawrate = 0;  // Current yaw rate setting (-100 to 100)

/* Process a complete UART message */
void process_message(parser_state* ps) {
    // Input validation
    if (!ps || !ps->msg_type || ps->msg_type[0] == '\0') {
        return;  // Ignore invalid messages
    }

    // Process based on message type
    if (strcmp(ps->msg_type, "PCREF") == 0 && ps->msg_payload[0] != '\0') {
        /* Handle motion reference update ($PCREF,speed,yawrate) */
        int i = 0;
        // Extract and clamp speed value (-100 to 100)
        int parsed_speed = extract_integer(&ps->msg_payload[i]);
        i = next_value(ps->msg_payload, i);  // Find next value position
        
        // Extract and clamp yawrate value (-100 to 100)
        int parsed_yawrate = extract_integer(&ps->msg_payload[i]);

        // Apply clamped values to global variables
        speed = CLAMP(parsed_speed, -100, 100);
        yawrate = CLAMP(parsed_yawrate, -100, 100);
    }
    else if (strcmp(ps->msg_type, "PCSTP") == 0) {
        /* Handle stop command ($PCSTP) */
        uint8_t ack = (current_state != EMERGENCY);  // Deny if in emergency
        if (ack) current_state = WAIT_FOR_START;     // Transition if allowed
        uart_send_string(ack ? "$MACK,1*" : "$MACK,0*");  // Send response
    }
    else if (strcmp(ps->msg_type, "PCSTT") == 0) {
        /* Handle start command ($PCSTT) */
        uint8_t ack = (current_state != EMERGENCY);  // Deny if in emergency
        if (ack) current_state = MOVING;             // Transition if allowed
        uart_send_string(ack ? "$MACK,1*" : "$MACK,0*");  // Send response
    }
    // Other message types are silently ignored (per requirements)
}

/* Transmit string via UART */
void uart_send_string(const char *str) {
    // Validate input
    if (!str || !*str) return;  // Skip NULL or empty strings

    // Critical section - disable UART TX interrupts
    IEC0bits.U1TXIE = 0;
    
    // Write all characters to TX buffer
    while (*str) {
        UART1_Buffer_Write(&uart1_tx, *str++);
    }

    // End critical section - restore interrupts and trigger transmission
    IEC0bits.U1TXIE = 1;
    IFS0bits.U1TXIF = 1;  // Force TX interrupt to start transmission
}