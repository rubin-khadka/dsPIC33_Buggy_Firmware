/* 
 * File:   adc_sensor.c
 * Author: Rubin Khadka
 * 
 * ADC Sensor Driver - Handles analog sensor measurements including:
 * - IR distance sensor (ADC1)
 * - Battery voltage monitoring (ADC2)
 */

#include "adc_sensor.h"

/* IR Sensor Functions ------------------------------------------------------ */

/* Initialize IR sensor hardware and ADC module */
void ir_sensor_init(void) {
    // Configure IR LED enable pin
    IR_ENABLE_TRIS = 0;     // Set as output
    IR_ENABLE_LAT = 1;      // Enable IR LED 

    // Configure ADC input pin
    IR_TRIS_PIN = 1;        // Set as input
    IR_ANALOG_PIN = 1;      // Enable analog mode

    // Reset and configure ADC1 module
    AD1CON1 = 0x0000;       // Clear control registers
    AD1CON2 = 0x0000;
    AD1CON3 = 0x0000;

    // ADC1 settings:
    AD1CON1bits.ASAM = 0;   // Manual sampling trigger
    AD1CON1bits.SSRC = 7;   // Auto-conversion after sampling
    AD1CON3bits.SAMC = 16;  // 16 TAD sample time
    AD1CON3bits.ADCS = 8;   // ADC clock prescaler

    // Select IR sensor channel and enable ADC
    AD1CHS0bits.CH0SA = IR_AN_CHANNEL;
    AD1CON1bits.ADON = 1;   // Power on ADC
}

/* Read raw ADC value from IR sensor */
uint16_t read_adc_ir(void) {
    AD1CON1bits.SAMP = 1;       // Start sampling
    while (!AD1CON1bits.DONE);  // Wait for conversion complete
    
    uint16_t result = ADC1BUF0; // Read conversion result
    AD1CON1bits.DONE = 0;       // Clear conversion flag
    return result;
}

/* Convert IR sensor reading to distance in cm */
uint16_t get_ir_distance_cm(void) {
    uint16_t raw = read_adc_ir();
    float volt = adc_to_voltage(raw);

    // Polynomial coefficients for voltage-to-distance conversion
    const float ir_coeffs[5] = {2.34f, -4.74f, 4.06f, -1.60f, 0.24f};
    
    // Calculate distance using Horner's method for polynomial evaluation
    float distance = ir_coeffs[4];
    distance = volt * distance + ir_coeffs[3];
    distance = volt * distance + ir_coeffs[2];
    distance = volt * distance + ir_coeffs[1];
    distance = volt * distance + ir_coeffs[0];
    distance *= 100;  // Convert to cm

    // Ensure distance stays within sensor limits, its best practice to do it 
    // But not required in this project
    // if (distance < IR_MIN_DISTANCE_CM) distance = IR_MIN_DISTANCE_CM;
    // if (distance > IR_MAX_DISTANCE_CM) distance = IR_MAX_DISTANCE_CM;

    return (uint16_t)roundf(distance);
}

/* Battery Sensing Functions ----------------------------------------------- */

/* Initialize battery voltage monitoring ADC */
void battery_init(void) {
    // Configure battery voltage input pin
    BATT_TRIS_PIN = 1;       // Set as input
    BATT_ANALOG_PIN = 1;     // Enable analog mode

    // Reset and configure ADC2 module
    AD2CON1 = 0x0000;        // Clear control registers
    AD2CON2 = 0x0000;
    AD2CON3 = 0x0000;

    // ADC2 settings:
    AD2CON1bits.ASAM = 0;    // Manual sampling trigger
    AD2CON1bits.SSRC = 7;    // Auto-conversion after sampling
    AD2CON3bits.SAMC = 16;   // 16 TAD sample time
    AD2CON3bits.ADCS = 8;    // ADC clock prescaler

    // Select battery channel and enable ADC
    AD2CHS0bits.CH0SA = BATT_AN_CHANNEL;
    AD2CON1bits.ADON = 1;    // Power on ADC
}

/* Read raw ADC value from battery channel */
uint16_t read_adc_batt(void) {
    AD2CON1bits.SAMP = 1;       // Start sampling
    while (!AD2CON1bits.DONE);  // Wait for conversion complete
    
    uint16_t result = ADC2BUF0; // Read conversion result
    AD2CON1bits.DONE = 0;       // Clear conversion flag
    return result;
}

/* Calculate actual battery voltage */
float get_battery_voltage(void) {
    uint16_t raw = read_adc_batt();
    float v_adc = adc_to_voltage(raw);
    
    // Account for voltage divider ratio
    return v_adc * BATTERY_SCALE_FACTOR;
}

/* UART Communication Functions ----------------------------------------------- */

/* Send battery voltage via UART in NMEA-style format */
void send_battery_data(float voltage) {
    char buffer[16];  // Holds formatted message
    
    // Format voltage with 2 decimal places
    int len = snprintf(buffer, sizeof(buffer), "$MBATT,%.2f*", (double)voltage);
    
    // Validate message length
    if (len <= 0 || len >= sizeof(buffer)) return;

    // Atomic UART transmission
    IEC0bits.U1TXIE = 0;  // Disable TX interrupts
    for (int i = 0; i < len; i++) {
        UART1_Buffer_Write(&uart1_tx, buffer[i]);
    }
    IEC0bits.U1TXIE = 1;  // Re-enable interrupts
    IFS0bits.U1TXIF = 1;  // Trigger transmission
}

/* Send IR distance measurement via UART in NMEA-style format */
void send_ir_distance(uint16_t distance) {
    char buffer[16];  // Holds formatted message
    
    // Format distance as integer
    int len = snprintf(buffer, sizeof(buffer), "$MDIST,%d*", distance);
    
    // Validate message length
    if (len <= 0 || len >= sizeof(buffer)) return;

    // Atomic UART transmission
    IEC0bits.U1TXIE = 0;  // Disable TX interrupts
    for (int i = 0; i < len; i++) {
        UART1_Buffer_Write(&uart1_tx, buffer[i]);
    }
    IEC0bits.U1TXIE = 1;  // Re-enable interrupts
    IFS0bits.U1TXIF = 1;  // Trigger transmission
}