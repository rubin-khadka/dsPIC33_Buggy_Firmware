/**
 * FIle: init.c
 * Author: Rubin Khadka
 * System hardware configuration and initialization
 */

#include "acc_spi.h"
#include "adc_sensor.h"
#include "state_machine.h"

/* Initialize all system hardware configurations */
void config_init() {
    
    /* Digital I/O configuration*/
    // Disable analog functionality on all ports
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000;
    
    /* GPIO Direction Settings */
    // LED Outputs
    TRISAbits.TRISA0 = 0;  // LED A0 (RA0) as output
    TRISGbits.TRISG9 = 0;  // LED G9 as output  
    TRISBbits.TRISB8 = 0;  // LED B8 as output
    TRISFbits.TRISF1 = 0;  // LED F1 as output
     
    // Button Inputs
    TRISEbits.TRISE8 = 1;  // Button RE8 as input
    TRISEbits.TRISE9 = 1;  // Button RE9 as input
    
    // UART Pins
    TRISDbits.TRISD0 = 0;   // UART TX (RD0/RP64) as output
    TRISDbits.TRISD11 = 1;  // UART RX (RD11/RP75) as input
    
    // SPI Pins
    TRISAbits.TRISA1 = 1;   // MISO (RA1/RP17) as input
    TRISFbits.TRISF12 = 0;  // SCK (RF12/RP108) as output
    TRISFbits.TRISF13 = 0;  // MOSI (RF13/RP109) as output
    
    /* Peripheral Pin remapping */
    // Button Interrupt Remapping
    RPINR0bits.INT1R = 0x58;    // Map RE8 (RP88) to INT1
    RPINR1bits.INT2R = 0x59;    // Map RE9 (RP89) to INT2
    INTCON2bits.INT1EP = 1;     // INT1 triggers on falling edge
    
    // SPI Peripheral Remapping
    RPINR20bits.SDI1R = 0b00010001; // MISO = RP17 (RA1)
    RPOR12bits.RP109R = 0b000101;   // MOSI = RF13 (RP109)
    RPOR11bits.RP108R = 0b000110;   // SCK = RF12 (RP108)
    
    /* Peripheral Initialization */
    // Communication Interfaces
    UART1_Init(BAUDRATE);   // Initialize UART1 at 115200 bps
    spi_init();             // Initialize SPI peripheral
    
    // Sensor Initialization
    acc_init();             // Configure accelerometer
    battery_init();         // Setup battery monitoring
    ir_sensor_init();       // Initialize IR distance sensor
    
    // Output Devices
    init_emergency_LED();   // Configure emergency indicator
    init_motor_control();   // Setup motor control outputs
    
    /* Interrupt Configuration */
    // Button Debounce Timer
    debounce_tmr_init();    // Setup Timer2 for debouncing
    
    // Configure Button Interrupt
    IFS1bits.INT1IF = 0;    // Clear INT1 flag
    IEC1bits.INT1IE = 1;    // Enable INT1 interrupt
    
    // Global Interrupt Enable
    INTCON2bits.GIE = 1;    // Enable all interrupts
}