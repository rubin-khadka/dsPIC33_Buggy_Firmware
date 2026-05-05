#include "parser.h"

/**
 * Parses incoming bytes to detect NMEA-style messages ($TYPE,PAYLOAD* format)
 * Returns NEW_MESSAGE when a complete message is received, NO_MESSAGE otherwise
 */
int parse_byte(parser_state* ps, char byte) {
    switch (ps->state) {
        case STATE_DOLLAR:
            // Wait for start of message ('$')
            if (byte == '$') {
                ps->state = STATE_TYPE;
                ps->index_type = 0;
            }
            break;

        case STATE_TYPE:
            // Collect message type until comma or asterisk
            if (byte == ',') {
                ps->state = STATE_PAYLOAD;
                ps->msg_type[ps->index_type] = '\0';  // Null-terminate type
                ps->index_payload = 0;
            } 
            // Handle malformed message 
            else if (ps->index_type == 6) {
                ps->state = STATE_DOLLAR;
                ps->index_type = 0;
            }
            // Handle message without payload
            else if (byte == '*') {
                ps->state = STATE_DOLLAR;
                ps->msg_type[ps->index_type] = '\0';
                ps->msg_payload[0] = '\0';
                return NEW_MESSAGE;
            } 
            // Store valid type character
            else {
                ps->msg_type[ps->index_type] = byte;
                ps->index_type++;
            }
            break;

        case STATE_PAYLOAD:
            // End of message detected
            if (byte == '*') {
                ps->state = STATE_DOLLAR;
                ps->msg_payload[ps->index_payload] = '\0';  // Null-terminate
                return NEW_MESSAGE;
            } 
            // Handle payload overflow
            else if (ps->index_payload == 100) {
                ps->state = STATE_DOLLAR;
                ps->index_payload = 0;
            } 
            // Store payload byte
            else {
                ps->msg_payload[ps->index_payload] = byte;
                ps->index_payload++;
            }
            break;
    }
    return NO_MESSAGE;
}

/**
 * Extracts integer from string
 * Stops at comma or null terminator
 */
int extract_integer(const char* str) {
    int i = 0, number = 0, sign = 1;
    
    // Handle sign
    if (str[i] == '-') {
        sign = -1;
        i++;
    }
    else if (str[i] == '+') {
        i++;  // sign remains 1
    }

    // Convert digits to integer
    while (str[i] != ',' && str[i] != '\0') {
        number *= 10;
        number += str[i] - '0';  // ASCII to digit
        i++;
    }
    return sign * number;
}

/**
 * Finds start of next comma-separated value in message string
 * Returns index of next value or string end
 */
int next_value(const char* msg, int i) {
    // Skip current value
    while (msg[i] != ',' && msg[i] != '\0') { 
        i++; 
    }
    // Move past comma if exists
    if (msg[i] == ',') {
        i++;
    }
    return i;
}