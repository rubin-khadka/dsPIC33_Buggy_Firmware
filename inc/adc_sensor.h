/* 
 * File:   adc_sensor.h
 * Author: Rubin Khadka
 * 
 * Header for ADC-based IR and battery sensing functions
 */

#ifndef ADC_SENSOR_H
#define	ADC_SENSOR_H

#include "uart.h"

// Constants for ADC conversion
#define REF_VOLT    3.3f        // Reference voltage (V)
#define ADC_RES     1023.0f     // 10-bit ADC max value

// Convert raw ADC reading to voltage
static inline float adc_to_voltage(uint16_t raw) {
    return ((float)raw) * REF_VOLT / ADC_RES;
}

/* IR Sensor Configuration */
#define IR_AN_CHANNEL       15                  // ADC channel for IR sensor
#define IR_ANALOG_PIN       ANSELBbits.ANSB15   // RB15 analog select
#define IR_TRIS_PIN         TRISBbits.TRISB15   // RB15 as input
#define IR_ENABLE_LAT       LATAbits.LATA3      // IR enable pin (RA3)
#define IR_ENABLE_TRIS      TRISAbits.TRISA3    // RA3 as output

// Distance thresholds in cm
#define EMERGENCY_THRESHOLD_CM  30

// The sensor can sense form 10 cm to 150cm so this is added here
#define IR_MIN_DISTANCE_CM      10.0f
#define IR_MAX_DISTANCE_CM      150.0f

// Function declarations for IR sensor
void ir_sensor_init(void);
uint16_t read_adc_ir(void);             // Read raw IR ADC value
uint16_t get_ir_distance_cm(void);      // Convert to distance (cm)

/* Battery Sensing Configuration */
#define BATT_AN_CHANNEL     11                  // ADC channel for battery
#define BATT_ANALOG_PIN     ANSELBbits.ANSB11   // RB11 analog select
#define BATT_TRIS_PIN       TRISBbits.TRISB11   // RB11 as input

/* Voltage Divider: 
 *   R1 = 200k? (upper resistor)
 *   R2 = 100k? (lower resistor)
 *   Scale Factor = (R1 + R2) / R2 = 3.0
 */
#define BATTERY_SCALE_FACTOR  3.0f

// Function declarations for battery
void battery_init(void);
uint16_t read_adc_batt(void);           // Read raw battery ADC
float get_battery_voltage(void);        // Get real voltage (V)

// Communication Functions
void send_battery_data(float voltage);
void send_ir_distance(uint16_t distance);

#endif	/* ADC_SENSOR_H */